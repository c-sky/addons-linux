#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/module.h>
#include <linux/irqdomain.h>
#include <linux/irqchip.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/machdep.h>
#include <asm/irq.h>
#include <asm/io.h>

#define NC_VA_INTC_NINT31_00		(void *)(intc_reg + 0x00)
#define NC_VA_INTC_NINT63_32		(void *)(intc_reg + 0x04)
#define NC_VA_INTC_NPEND31_00		(void *)(intc_reg + 0x10)
#define NC_VA_INTC_NPEND63_32		(void *)(intc_reg + 0x14)
#define NC_VA_INTC_NENSET31_00		(void *)(intc_reg + 0x20)
#define NC_VA_INTC_NENSET63_32		(void *)(intc_reg + 0x24)
#define NC_VA_INTC_NENCLR31_00		(void *)(intc_reg + 0x30)
#define NC_VA_INTC_NENCLR63_32		(void *)(intc_reg + 0x34)
#define NC_VA_INTC_NEN31_00		(void *)(intc_reg + 0x40)
#define NC_VA_INTC_NEN63_32		(void *)(intc_reg + 0x44)
#define NC_VA_INTC_NMASK31_00		(void *)(intc_reg + 0x50)
#define NC_VA_INTC_NMASK63_32		(void *)(intc_reg + 0x54)
#define NC_VA_INTC_SOURCE		(void *)(intc_reg + 0x60)

static unsigned int irq_num[NR_IRQS];
static unsigned int intc_reg;

static void nc_irq_mask(struct irq_data *d)
{
	unsigned int mask;
	unsigned int irq_chan;

	irq_chan = irq_num[d->irq];
	if (irq_chan == 0xff) return;

	if (irq_chan < 32) {
		mask = __raw_readl(NC_VA_INTC_NMASK31_00);
		mask |= 1 << irq_chan;
		__raw_writel(mask, NC_VA_INTC_NMASK31_00);
	} else {
		mask = __raw_readl(NC_VA_INTC_NMASK63_32);
		mask |= 1 << (irq_chan - 32);
		__raw_writel(mask, NC_VA_INTC_NMASK63_32);
	}
}

static void nc_irq_unmask(struct irq_data *d)
{
	unsigned int mask;
	unsigned int irq_chan;

	irq_chan = irq_num[d->irq];
	if (irq_chan == 0xff) return;

	if (irq_chan < 32) {
		mask = __raw_readl(NC_VA_INTC_NMASK31_00);
		mask &= ~( 1 << irq_chan);
		__raw_writel(mask, NC_VA_INTC_NMASK31_00);
	} else {
		mask = __raw_readl( NC_VA_INTC_NMASK63_32);
		mask &= ~(1 << (irq_chan - 32));
		__raw_writel(mask, NC_VA_INTC_NMASK63_32);
	}
}

static void nc_irq_en(unsigned int irq)
{
	unsigned int mask;
	unsigned int irq_chan;

	irq_chan = irq_num[irq];
	if (irq_chan == 0xff) return;

	if (irq_chan < 32) {
		mask = 1 << irq_chan;
		__raw_writel(mask, NC_VA_INTC_NENSET31_00);
	} else {
		mask = 1 << (irq_chan - 32);
		__raw_writel(mask, NC_VA_INTC_NENSET63_32);
	}
}

static void nc_irq_dis(unsigned int irq)
{
	unsigned int mask;
	unsigned int irq_chan;

	irq_chan = irq_num[irq];
	if (irq_chan == 0xff) return;

	if (irq_chan < 32) {
		mask = 1 << irq_chan;
		__raw_writel(mask, NC_VA_INTC_NENCLR31_00);
	} else {
		mask = 1 << (irq_chan - 32);
		__raw_writel(mask, NC_VA_INTC_NENCLR63_32);
	}
}

static inline void nc_irq_ack(struct irq_data *d) {}

unsigned int nc_irq_channel_set(struct irq_data *d)
{
	unsigned int status, i;

	i = d->irq;
	irq_num[i] = i;
	status = __raw_readl(NC_VA_INTC_SOURCE + i - i%4) &
				((i << ((i%4)*8)) | ~(0xff << ((i%4)*8)));
	__raw_writel(status, NC_VA_INTC_SOURCE + i - i%4);

	nc_irq_en(d->irq);
	nc_irq_unmask(d);
	return 0;
}

void nc_irq_channel_clear(struct irq_data *d)
{
	unsigned int i, status;

	i = irq_num[d->irq];
	irq_num[d->irq] = 0xff;

	nc_irq_mask(d);
	nc_irq_dis(d->irq);

	status = __raw_readl(NC_VA_INTC_SOURCE + i - i%4);
	status = status | (0xff << ((i%4)*8));
	__raw_writel(status, NC_VA_INTC_SOURCE + i - i%4);
}

struct irq_chip nc_irq_chip = {
	.name =		"nationalchip_intc_v1",
	.irq_ack =	nc_irq_ack,
	.irq_mask =	nc_irq_mask,
	.irq_unmask =	nc_irq_unmask,
	.irq_startup =	nc_irq_channel_set,
	.irq_shutdown =	nc_irq_channel_clear,
};

inline int ff1_64(unsigned int hi, unsigned int lo)
{
	int result;
	__asm__ __volatile__(
		"ff1 %0"
		:"=r"(hi)
		:"r"(hi)
		:
	);

	__asm__ __volatile__(
		"ff1 %0"
		:"=r"(lo)
		:"r"(lo)
		:
	);
	if( lo != 32 )
		result = 31-lo;
	else if( hi != 32 ) result = 31-hi + 32;
	else {
		printk("mach_get_auto_irqno error hi:%x, lo:%x.\n", hi, lo);
		result = NR_IRQS;
	}
	return result;
}

unsigned int nc_get_irqno(void)
{
	unsigned int nint64hi;
	unsigned int nint64lo;
	int irq_no;
	nint64lo = __raw_readl(NC_VA_INTC_NINT31_00);
	nint64hi = __raw_readl(NC_VA_INTC_NINT63_32);
	irq_no = ff1_64(nint64hi, nint64lo);

	return irq_no;
}

static int irq_map(struct irq_domain *h, unsigned int virq,
				irq_hw_number_t hw_irq_num)
{
	irq_set_chip_and_handler(virq, &nc_irq_chip, handle_level_irq);

	irq_num[virq] = 0xff;

	if(virq%4 == 0)
		__raw_writel(0xffffffff, NC_VA_INTC_SOURCE + virq);

	return 0;
}

static const struct irq_domain_ops nc_irq_ops = {
	.map	= irq_map,
	.xlate	= irq_domain_xlate_onecell,
};

static int __init
intc_init(struct device_node *intc, struct device_node *parent)
{
	struct irq_domain *root_domain;

	if (parent)
		panic("DeviceTree incore intc not a root irq controller\n");

	mach_get_auto_irqno = nc_get_irqno;

	intc_reg = (unsigned int) of_iomap(intc, 0);
	if (!intc_reg)
		panic("Nationalchip Intc Reg: %x.\n", intc_reg);

	__raw_writel(0xffffffff, NC_VA_INTC_NENCLR31_00);
	__raw_writel(0xffffffff, NC_VA_INTC_NENCLR63_32);
	__raw_writel(0xffffffff, NC_VA_INTC_NMASK31_00);
	__raw_writel(0xffffffff, NC_VA_INTC_NMASK63_32);

	root_domain = irq_domain_add_legacy(intc, NR_IRQS, 0, 0, &nc_irq_ops, NULL);
	if (!root_domain)
		panic("root irq domain not avail\n");

	irq_set_default_host(root_domain);

	return 0;
}

IRQCHIP_DECLARE(nationalchip_intc_v1, "nationalchip,intc-v1", intc_init);

