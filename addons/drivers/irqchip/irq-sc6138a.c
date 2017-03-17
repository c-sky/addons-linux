#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/module.h>
#include <linux/irqdomain.h>
#include <linux/irqchip.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/io.h>

static unsigned int intc_reg;

#define CKPIC_BASE intc_reg
/*
 * Define the offset(Index) of the Programmable Interrupt Controller registers
 */
#define IRQ_INTEN		0x0
#define IRQ_INTMASK		0x8
#define IRQ_INTFORCE	0x10
#define IRQ_RAWSTATUS	0x18
#define IRQ_STATUS		0x20
#define IRQ_MASKSTATUS	0x28
#define IRQ_FINALSTATUS	0x30

/*
 *  Bit Definition for the PIC Interrupt control register
 */
#define CKPIC_ICR_AVE    0x80000000 // F/N auto-interrupt requests enable
#define CKPIC_ICR_FVE    0x40000000 // F interrupt request to have vector number
#define CKPIC_ICR_ME     0x20000000 // Interrupt masking enable
#define	CKPIC_ICR_MFI	 0x10000000 // Masking of fast interrupt rquests enable

/*
 * Bit definition for the PIC Normal Interrup Enable register
 */
#define CKPIC_NIER_NIE(x)   (1 << x) // Prioity=x normal interrupt enable bit

/*
 * Bit definition for the PIC Fast Interrup Enable register
 */
#define CKPIC_FIER_FIE(x)   (1 << x)   // Prioity=x Fast interrupt enable bit

#define csky_irq_wake NULL


void static inline sl_writeb(u8 val, unsigned long reg)
{
    *(volatile u8 *)(reg) = val;
}

void static inline sl_writew(u16 val, unsigned long reg)
{
    *(volatile u16 *)(reg) = val;
}

void static inline sl_writel(u32 val, unsigned long reg)
{
    *(volatile u32 *)(reg) = val;
}

static inline u8 sl_readb(unsigned long reg)
{
    return (*(volatile u8 *)reg);
}

static inline u16 sl_readw(unsigned long reg)
{
    return (*(volatile u16 *)reg);
}

static inline u32 sl_readl(unsigned long reg)
{
    return (*(volatile u32 *)reg);
}



static void sc_irq_mask(struct irq_data *d)
{
	u32 data,reg,shift;

	reg = CKPIC_BASE + IRQ_INTMASK;
	shift = d->irq;
	data = sl_readl(reg);
	data |= (1UL << shift);
	sl_writel(data, reg);
}

static void sc_irq_unmask(struct irq_data *d)
{
	u32 data,reg,shift;

	reg = CKPIC_BASE + IRQ_INTMASK;
	shift = d->irq;
	data = sl_readl(reg);
	data &= ~(1UL << shift);
	sl_writel(data, reg);
}

static void sc_irq_disable(struct irq_data *d)
{
	u32 data,reg,shift;

	reg = CKPIC_BASE + IRQ_INTEN;
	shift = d->irq;
	data = sl_readl(reg);
	data &= ~(1UL << shift);
	sl_writel(data, reg);
}

static void sc_irq_enable(struct irq_data *d)
{
	u32 data,reg,shift;

	reg = CKPIC_BASE + IRQ_INTEN;
	shift = d->irq;
	data = sl_readl(reg);
	data |= (1UL << shift);
	sl_writel(data, reg);
}

static void sc_irq_ack(struct irq_data *d)
{
	u32 data,reg,shift;

	reg = CKPIC_BASE + IRQ_INTEN;
	shift = d->irq;
	data = sl_readl(reg);
	data |= (1UL << shift);
	//data &= ~(1UL << shift);
	sl_writel(data, reg);
}

static struct irq_chip sc_irq_chip = {
	.name		= "silan_intc_6138a",
	.irq_mask	= sc_irq_mask,
	.irq_unmask	= sc_irq_unmask,
	.irq_enable     = sc_irq_enable,
	.irq_disable    = sc_irq_disable,
	.irq_ack        = sc_irq_ack,

};

static int sc_irq_map(struct irq_domain *h, unsigned int virq,
				irq_hw_number_t hw_irq_num)
{
	irq_set_chip_and_handler(virq, &sc_irq_chip, handle_level_irq);
	return 0;
}

static const struct irq_domain_ops sc_irq_ops = {
	.map	= sc_irq_map,
	.xlate	= irq_domain_xlate_onecell,
};

static unsigned int find_bit(int irq)
{
	int i;
	for (i = 0; i < 32; i ++) {
		if (irq & (1 << i))
			return i;
	}
	return 0xff;
}

static unsigned int sc_get_irqno(void)
{
	unsigned int irqno;

	irqno = sl_readl(CKPIC_BASE + IRQ_MASKSTATUS);
	irqno = find_bit(irqno);

	return irqno;
}

static int __init
intc_init(struct device_node *np, struct device_node *parent)
{
	struct irq_domain *root_domain;

	csky_get_auto_irqno = sc_get_irqno;

	if (parent)
		panic("pic not a root intc\n");

	intc_reg = (unsigned int)of_iomap(np, 0);
	if (!intc_reg)
		panic("%s, of_iomap err.\n", __func__);

	root_domain = irq_domain_add_legacy(np, NR_IRQS, 0, 0, &sc_irq_ops, NULL);
	if (!root_domain)
		panic("root irq domain not available\n");

	irq_set_default_host(root_domain);

	return 0;
}

IRQCHIP_DECLARE(silan_sc6138a_intc, "silan,sc6138a", intc_init);

