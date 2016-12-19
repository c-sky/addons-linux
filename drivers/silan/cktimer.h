/*
 * cktimer.h -- CKcore on board TIMER support defines.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * (C) Copyright 2009, Li Chunqiang (chunqiang_li@c-sky.com)
 * (C) Copyright 2004, Kang Sun (sunk@vlsi.zju.edu.cn)
 * (C) Copyright 1999-2003, Greg Ungerer (gerg@snapgear.com)
 * (C) Copyright 2000, Lineo Inc. (www.lineo.com) 
 */
#ifndef	_CSKY_TIMER_H
#define	_CSKY_TIMER_H

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
#endif	/* _CSKY_TIMER_H */
