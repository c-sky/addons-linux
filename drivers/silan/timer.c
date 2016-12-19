/*
 * arch/csky/cskyevb/timer.c --- timer handles
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004, Kang Sun (sunk@vlsi.zju.edu.cn)
 * Copyright (C) 2004, Li Chunqiang (chunqiang_li@c-sky.com)
 * Copyright (C) 2009, Hu Junshan (junshan_hu@c-sky.com)
 * Copyright (C) 2015, Chen Linfei (linfei_chen@c-sky.com)
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/rtc.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>

#include <asm/irq.h>
#include <asm/traps.h>
#include <asm/csky.h>
#include <asm/delay.h>
#include "silan_irq.h"
#include "cktimer.h"

static cycle_t notrace csky_clocksource_read(struct clocksource *unused)
{
	volatile unsigned long  *timerp;

	timerp = (volatile unsigned long *) (CKTIMER_BASE);

	return CKTIMER_COUNT_VALUE - timerp[CKTIMER_TCN2_CVR];
}

/*
 *  Clears the interrupt from timer1.
 */
void csky_tick(void)
{
	volatile unsigned long	*timerp;
	unsigned long	temp;

	/* Ack and Clear the interrupt from timer1 */
	timerp = (volatile unsigned long *) (CKTIMER_BASE);
	timerp[CKTIMER_TCN1_EOI] = 0;
	temp = timerp[CKTIMER_TCN1_EOI];
}

static struct clocksource csky_clocksource = {
	.name	= "csky timer2",
	.rating	= 300,
	.read	= csky_clocksource_read,
	.mask	= CLOCKSOURCE_MASK(32),
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
};

static void __init csky_timer1_hw_init(void)
{
	volatile unsigned long  *timerp;

	timerp = (volatile unsigned long *) (CKTIMER_BASE);

	/*
	 * Set up TIMER 1 as poll clock
	 * Set timer control register, 8 bits, clock = APB bus clock
	 * The work mode of timer1 is user-defined running mode
	 * Enable timer1 and tiemr1 interrupt
	 */
	timerp[CKTIMER_TCN1_CR] = 0 | TIMER_EN(1)
                                | TIMER_MODE(1)
                                | TIMER_INT_EN(1)
                                | TIMER_PRE(0)
                                | TIMER_SIZE(1)
                                | TIMER_ONE_SHOT(0);

	/*
	 *  set the init value of timer1 load counter, 24bits
	 *  when the counter overflow, interrupt occurs
	 */
	timerp[CKTIMER_TCN1_LDCR] = (CK_BUSCLK /  HZ);
}

static void __init csky_timer2_hw_init(void)
{
	volatile unsigned long  *timerp;

	timerp = (volatile unsigned long *) (CKTIMER_BASE);

	/*
	 * Set up TIMER 2 as poll clock
	 * Set timer control register, 8 bits, clock = APB bus clock
	 * The work mode of timer2 is user-defined running mode
	 * Enable timer2 and tiemr1 interrupt
	 */
	timerp[CKTIMER_TCN2_CR] = 0 | TIMER_EN(1)
                                | TIMER_MODE(1)
                                | TIMER_INT_EN(0)
                                | TIMER_PRE(0)
                                | TIMER_SIZE(1)
                                | TIMER_ONE_SHOT(0);

	/*
	 *  set the init value of timer2 load counter, 24bits
	 *  when the counter overflow, interrupt occurs
	 */
	timerp[CKTIMER_TCN2_LDCR] = CKTIMER_COUNT_VALUE;
}

static int csky_timer_set_next_event(unsigned long cycles, struct clock_event_device *evt)
{
	volatile unsigned long  *timerp;

	timerp = (volatile unsigned long *) (CKTIMER_BASE);
	timerp[CKTIMER_TCN1_LDCR] = cycles;

	return 0;
}

static void csky_timer_set_mode(enum clock_event_mode mode,
                              struct clock_event_device *evt)
{
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
	case CLOCK_EVT_MODE_ONESHOT:
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_RESUME:
		break;
	}
}

static struct clock_event_device csky_clockevent = {
	.name 		= "csky timer1",
	.features	= CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.set_next_event	= csky_timer_set_next_event,
	.set_mode	= csky_timer_set_mode,
};

static irqreturn_t csky_timer_interrupt(int irq, void *dev_id)
{
	volatile unsigned long  *timerp;
	struct clock_event_device *evt = &csky_clockevent;

	timerp = (volatile unsigned long *) (CKTIMER_BASE);
	timerp[CKTIMER_TCN1_EOI] = 0x0;

	evt->event_handler(evt);

	return IRQ_HANDLED;
}


static struct irqaction csky_timer_irq = {
	.name           = "Ckcore Timer Tick",
	.flags          = IRQF_DISABLED,
	.handler        = csky_timer_interrupt,
};

void __init csky_timer_init(void)
{
	unsigned int csky_clock_rate = CK_BUSCLK;

	csky_timer1_hw_init();
	csky_timer2_hw_init();
	setup_irq(PIC_IRQ_SPU_TIMER, &csky_timer_irq);

	clockevents_calc_mult_shift(&csky_clockevent, csky_clock_rate, 4);
	csky_clockevent.max_delta_ns = clockevent_delta2ns((CK_BUSCLK / HZ), &csky_clockevent);
	csky_clockevent.min_delta_ns = clockevent_delta2ns(1, &csky_clockevent);
	csky_clockevent.cpumask      = cpumask_of(0);

	clockevents_register_device(&csky_clockevent);

	clocksource_register_hz(&csky_clocksource, csky_clock_rate);
}

unsigned long csky_timer_offset(void)
{
	volatile unsigned long  *timerp;
	unsigned long       trr, tcn, offset;

	timerp = (volatile unsigned long *) (CKTIMER_BASE);
	tcn = timerp[CKTIMER_TCN1_CVR];
	trr = timerp[CKTIMER_TCN1_LDCR];
	
	tcn = trr - tcn;

	offset = ((tcn * (10000 / HZ)) / (trr / 100));
	return offset;
}

/*
 *  set or get the real time clock. But now, we don't support.
 */
int csky_hwclk(int set, struct rtc_time *t)
{
	t->tm_year = 1980;
	t->tm_mon  = 1;
	t->tm_mday = 1;
	t->tm_hour = 0;
	t->tm_min  = 0;
	t->tm_sec  = 0;	

	return 0;
}
