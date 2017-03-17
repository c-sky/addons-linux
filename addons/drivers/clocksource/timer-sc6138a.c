#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/profile.h>
#include <linux/irq.h>
#include <linux/rtc.h>
#include <linux/sizes.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/sched_clock.h>

static unsigned int timer_reg;

#define SCTIMER_TCN1_CVR	(void *)(timer_reg + 0x00)
#define SCTIMER_TCN1_CMPR	(void *)(timer_reg + 0x04)
#define SCTIMER_TCN1_CR		(void *)(timer_reg + 0x08)

#define SCTIMER_TCN2_CVR	(void *)(timer_reg + 0x10)
#define SCTIMER_TCN2_CMPR	(void *)(timer_reg + 0x14)
#define SCTIMER_TCN2_CR		(void *)(timer_reg + 0x18)

static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *dev = (struct clock_event_device *) dev_id;

	__raw_writel(0x0, SCTIMER_TCN1_CR);

	dev->event_handler(dev);

	return IRQ_HANDLED;
}

static int sc_timer_set_next_event(unsigned long delta, struct clock_event_device *evt)
{
	__raw_writel(delta,	SCTIMER_TCN1_CMPR);
	__raw_writel(0x0,	SCTIMER_TCN1_CVR);
	__raw_writel(0x1,	SCTIMER_TCN1_CR);
	return 0;
}

static int sc_timer_shutdown(struct clock_event_device *dev)
{
	__raw_writel(0x0, SCTIMER_TCN1_CR);

	return 0;
}

static struct clock_event_device sc_ced = {
	.name			= "sc6138a-clkevent",
	.features		= CLOCK_EVT_FEAT_ONESHOT,
	.rating			= 200,
	.set_state_shutdown	= sc_timer_shutdown,
	.set_next_event		= sc_timer_set_next_event,
};

static u64 notrace sc_sched_clock_read(void)
{
	return (u64) __raw_readl(SCTIMER_TCN2_CVR);
}

static void sc_csd_enable(void)
{
	__raw_writel(0x0,		SCTIMER_TCN2_CR);
	__raw_writel(0xffffffff,	SCTIMER_TCN2_CMPR);
	__raw_writel(0x1,		SCTIMER_TCN2_CR);
}

static int __init sc_timer_init(struct device_node *np)
{
	unsigned int irq;
	unsigned int freq;

	/* parse from devicetree */
	timer_reg = (unsigned int) of_iomap(np, 0);
	if (!timer_reg)
		panic("%s, of_iomap err.\n", __func__);

	irq = irq_of_parse_and_map(np, 0);
	if (!irq)
		panic("%s, irq_parse err.\n", __func__);

	if (of_property_read_u32(np, "clock-frequency", &freq))
		panic("%s, clock-frequency error.\n", __func__);

	pr_info("Nationalchip Timer Init, reg: %x, irq: %d, freq: %d.\n",
		timer_reg, irq, freq);

	/* setup irq */
	if (request_irq(irq, timer_interrupt, IRQF_TIMER, np->name, &sc_ced))
		panic("%s timer_interrupt error.\n", __func__);

	/* register */
	clockevents_config_and_register(&sc_ced, freq, 1, ULONG_MAX);

	sc_csd_enable();
	clocksource_mmio_init(SCTIMER_TCN2_CVR, "sc6138a-clksource", freq, 200, 32, clocksource_mmio_readl_up);

	sched_clock_register(sc_sched_clock_read, 32, freq);

	return 0;
}
CLOCKSOURCE_OF_DECLARE(sc_timer, "silan,timer-sc6138a", sc_timer_init);

