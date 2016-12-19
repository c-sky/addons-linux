/*
 * Driver for C-Sky Timer.
 *
 * Copyright (C) 2016, C-Sky Microsystems Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

/*
 * The address map of timer register
 */
#define CKTIMER_TCN1_LDCR	0x00	/* Timer1 Load Count register */
#define CKTIMER_TCN1_CVR	0x04	/* Timer1 Current Value register */
#define CKTIMER_TCN1_CR		0x08	/* Timer1 Control register */
#define CKTIMER_TCN1_EOI	0x0C	/* Timer1 Interrupt clear register */
#define CKTIMER_TCN1_ISR	0x10	/* Timer1 Interrupt status */
#define CKTIMER_TCN2_LDCR	0x14	/* Timer2 Load Count register */
#define CKTIMER_TCN2_CVR	0x18	/* Timer2 Current Value register */
#define CKTIMER_TCN2_CR		0x1C	/* Timer2 Control register */
#define CKTIMER_TCN2_EOI	0x20	/* Timer2 Interrupt clear register */
#define CKTIMER_TCN2_ISR	0x24	/* Timer2 Interrupt status */

#define CKTIMER_SYS_ISR		0xA0	/* Interrupts status of all timers */
#define CKTIMER_SYS_EOI		0xA4	/* Read it to clear all active interrupts */
#define CKTIMER_SYS_RISR	0xA8	/* Unmasked interrupts status register */

/*
 *  Bit definitions for the Timer Control Register (Timer CR).
 */
#define CKTIMER_TCR_RCS		0x8	/* Timer Reference Clock selection */
#define CKTIMER_TCR_IM		0x4	/* Timer Interrupt mask (0:unmask, 1:mask) */
#define CKTIMER_TCR_MS		0x2	/* Timer Mode select (0:free-running, 1:user-defined) */
#define CKTIMER_TCR_EN		0x1	/* Timer Enable select (0:disable, 1:enable) */

#define FREE_RUNNING_MAX_CNT	0xffffffffu

static unsigned int cktimer_base;
#define CKTIMER_REG(offset)	((void *)(cktimer_base + (offset)))

static void __init csky_timer1_hw_init(unsigned int freq)
{
	/*
	 * set up TIMER 1 as clock_event_device
	 * Timer Interrupt: unmask
	 * Timer Mode:      user-defined
	 * Timer Enable:    enable
	 */
	__raw_writel(CKTIMER_TCR_MS | CKTIMER_TCR_EN, CKTIMER_REG(CKTIMER_TCN1_CR));

	/* set the init value of timer1 load counter(32bits) */
	__raw_writel(freq / HZ, CKTIMER_REG(CKTIMER_TCN1_LDCR));
}

static void __init csky_timer2_hw_init(void)
{
	/*
	 * Set up TIMER 2 as clocksource
	 * Timer Interrupt: mask
	 * Timer Mode:      free-running
	 * Timer Enable:    enable
	 */
	__raw_writel(CKTIMER_TCR_IM | CKTIMER_TCR_EN, CKTIMER_REG(CKTIMER_TCN2_CR));

	/* set the init value of timer2 load counter(32bits) */
	__raw_writel(FREE_RUNNING_MAX_CNT, CKTIMER_REG(CKTIMER_TCN2_LDCR));
}

static irqreturn_t csky_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = (struct clock_event_device *)dev_id;
	unsigned long temp;

	/* clear timer1 interrupt */
	temp = __raw_readl(CKTIMER_REG(CKTIMER_TCN1_EOI));

	evt->event_handler(evt);
	return IRQ_HANDLED;
}

static int csky_timer_set_next_event(unsigned long cycles, struct clock_event_device *evt)
{
	__raw_writel(cycles, CKTIMER_REG(CKTIMER_TCN1_LDCR));
	return 0;
}

static struct clock_event_device csky_clockevent = {
	.name			= "csky_timer1",
	.features		= CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.set_next_event	= csky_timer_set_next_event,
};

static cycle_t notrace csky_clocksource_read(struct clocksource *unused)
{
	return FREE_RUNNING_MAX_CNT - __raw_readl(CKTIMER_REG(CKTIMER_TCN2_CVR));
}

static struct clocksource csky_clocksource = {
	.name	= "csky_timer2",
	.rating	= 300,
	.read	= csky_clocksource_read,
	.mask	= CLOCKSOURCE_MASK(32),
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
};

static int __init csky_timer_init(struct device_node *np)
{
	unsigned int irq;
	unsigned int freq;

	/* parse from devicetree */
	cktimer_base = (unsigned int) of_iomap(np, 0);
	if (!cktimer_base)
		panic("%s, of_iomap err.\n", __func__);

	irq = irq_of_parse_and_map(np, 0);
	if (!irq)
		panic("%s, irq_parse err.\n", __func__);

	if (of_property_read_u32(np, "clock-frequency", &freq))
		panic("%s, clock-frequency error.\n", __func__);

	pr_info("csky timer init, reg: %x, irq: %d, freq: %d.\n",
		cktimer_base, irq, freq);

	/* init timer */
	csky_timer1_hw_init(freq);
	csky_timer2_hw_init();

	/* setup irq */
	if (request_irq(irq, csky_timer_interrupt, IRQF_TIMER, np->name, &csky_clockevent))
		panic("%s csky_timer_interrupt error.\n", __func__);

	/* register */
#if 1
	clockevents_config_and_register(&csky_clockevent, freq, 1, ULONG_MAX);
#else
	csky_clockevent.cpumask = cpumask_of(0);
	csky_clockevent.min_delta_ticks = 1;
	csky_clockevent.max_delta_ticks = (freq / HZ); /* why not ULONG_MAX ? */
	clockevents_calc_mult_shift(&csky_clockevent, freq, 4); /* why 4 ? */
	csky_clockevent.min_delta_ns =
		clockevent_delta2ns(csky_clockevent.min_delta_ticks, &csky_clockevent);
	csky_clockevent.max_delta_ns =
		clockevent_delta2ns(csky_clockevent.max_delta_ticks, &csky_clockevent);

	clockevents_register_device(&csky_clockevent);
#endif

	clocksource_register_hz(&csky_clocksource, freq);

	return 0;
}
CLOCKSOURCE_OF_DECLARE(csky_timer, "csky,timer", csky_timer_init);
