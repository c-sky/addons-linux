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
#include <asm/addrspace.h>
/*
 *  Define the Timer Control Register base address.
 */
#define SILAN_SPU_TIMER_PHY_BASE		0x1FCB1000
#define CKTIMER_BASE          KSEG1ADDR(SILAN_SPU_TIMER_PHY_BASE)

/*
 *  Define the offset(index) in CKTIMER_BASE for the registers
 *  addresses of the timer
 */
#define CKTIMER_TCN1_LDCR     0x00    // Timer1 Load Count register
#define CKTIMER_TCN1_CVR      0x01    // Timer1 Current Value register
#define CKTIMER_TCN1_CR       0x02    // Timer1 Control register
#define CKTIMER_TCN1_EOI      0x03    // Timer1 Interrupt clear register
#define CKTIMER_TCN1_ISR      0x04    // Timer1 Interrupt status.


#define CKTIMER_TCN2_LDCR     0x08    /* Timer2 Load Count register*/
#define CKTIMER_TCN2_CVR      0x09    /* Timer2 Current Value register*/
#define CKTIMER_TCN2_CR       0x0a    /* Timer2 Control register*/
#define CKTIMER_TCN2_EOI      0x0b    /* Timer2 Interrupt clear register*/
#define CKTIMER_TCN2_ISR      0x0c    /* Timer2 Interrupt status.*/

#define CKTIMER_COUNT_VALUE	(0xffffffffu)
/*
 *  Bit definitions for the Timer Control Register (Timer CR).
 */
#define CKTIMER_TCR_RCS       0x00000008    // Timer Reference Clock selection
#define CKTIMER_TCR_IM        0x00000004    // Timer Interrupt mask
#define CKTIMER_TCR_MS        0x00000002    // Timer Mode select
#define CKTIMER_TCR_EN        0x00000001    // Timer Enable select

#define TIMER_EN(x)           ((x) << 7)
#define TIMER_MODE(x)         ((x) << 6)
#define TIMER_INT_EN(x)       ((x) << 5)
#define TIMER_PRE(x)          ((x) << 2)
#define TIMER_SIZE(x)         ((x) << 1)
#define TIMER_ONE_SHOT(x)     ((x))

static cycle_t notrace csky_clocksource_read(struct clocksource *unused)
{
	volatile unsigned long  *timerp;

	timerp = (volatile unsigned long *) (CKTIMER_BASE);

	return CKTIMER_COUNT_VALUE - timerp[CKTIMER_TCN2_CVR];
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
	timerp[CKTIMER_TCN1_LDCR] = (198000000 /  HZ);
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
	unsigned int csky_clock_rate = 198000000;

	csky_timer1_hw_init();
	csky_timer2_hw_init();
	setup_irq(63, &csky_timer_irq);

	clockevents_calc_mult_shift(&csky_clockevent, csky_clock_rate, 4);
	csky_clockevent.max_delta_ns = clockevent_delta2ns((198000000 / HZ), &csky_clockevent);
	csky_clockevent.min_delta_ns = clockevent_delta2ns(1, &csky_clockevent);
	csky_clockevent.cpumask      = cpumask_of(0);

	clockevents_register_device(&csky_clockevent);

	clocksource_register_hz(&csky_clocksource, csky_clock_rate);
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
