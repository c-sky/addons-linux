/*
 * ckpic.h -- Programmable Interrupt Controller support.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006  Hangzhou C-SKY Microsystems co.,ltd.
 * Copyright (C) 2006  Li Chunqiang (chunqiang_li@c-sky.com)
 * Copyright (C) 2009  Hu Junshan (junshan_hu@c-sky.com)
 */

#ifndef	_CSKY_PIC_H_
#define	_CSKY_PIC_H_
#include <asm/addrspace.h>
/*
 *	Define the base address of the C-SKY interrupt controller
 */
#define SILAN_ICTL_PHY_BASE				0x1FCB0000
#define CKPIC_BASE       KSEG1ADDR(SILAN_ICTL_PHY_BASE)

/*
 *	Define the offset(Index) of the Programmable Interrupt Controller registers
 */
#define CKPIC_ICR        0x00     // PIC interrupt control register(High 16bits)
#define CKPIC_ISR        0x00     // PIC interrupt status register(Low 16bits)
#define CKPIC_IFRL       0x08     // PIC interrupt force register low
#define CKPIC_IFRH       0x20     // PIC interrupt force register high
#define CKPIC_IPRL       0x0c     // PIC interrupt pending register low
#define CKPIC_IPRH       0x24     // PIC interrupt pending register high
#define CKPIC_NIERL      0x10     // PIC normal interrupt enable register low
#define CKPIC_NIERH      0x28     // PIC normal interrupt enable register high
#define CKPIC_NIPRL      0x14     // PIC normal interrupt pending register low
#define CKPIC_NIPRH      0x2c     // PIC normal interrupt pending register high
#define CKPIC_FIERL      0x18     // PIC fast interrupt enable register low
#define CKPIC_FIERH      0x30     // PIC fast interrupt enable register high
#define CKPIC_FIPRL      0x1c     // PIC fast interrupt pending register low
#define CKPIC_FIPRH      0x34     // PIC fast interrupt pending register high

#define CKPIC_PR0        0x40     // PIC prior register 0(High-High 8 bit)
#define CKPIC_PR4        0x44     // PIC prior register 4(High-High 8 bit)
#define CKPIC_PR8        0x48     // PIC prior register 8(High-High 8 bit)
#define CKPIC_PR12       0x4c     // PIC prior register 12(High-High 8 bit)
#define CKPIC_PR16       0x50     // PIC prior register 16(High-High 8 bit)
#define CKPIC_PR20       0x54     // PIC prior register 20(High-High 8 bit)
#define CKPIC_PR24       0x58     // PIC prior register 24(High-High 8 bit)
#define CKPIC_PR28       0x5c     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR32       0x60     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR36       0x64     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR40       0x68     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR44       0x6c     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR48       0x70     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR52       0x74     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR56       0x78     // PIC prior register 28(High-High 8 bit)
#define CKPIC_PR60       0x7c     // PIC prior register 28(High-High 8 bit)

/*
 *  Bit Definition for the PIC Interrupt control register
 */
#define CKPIC_ICR_AVE    0x80000000 // F/N auto-interrupt requests enable
#define CKPIC_ICR_FVE    0x40000000 // F interrupt request to have vector number
#define CKPIC_ICR_ME     0x20000000 // Interrupt masking enable
#define	CKPIC_ICR_MFI	 0x10000000	// Masking of fast interrupt rquests enable

/*
 * Bit definition for the PIC Normal Interrup Enable register
 */
#define CKPIC_NIER_NIE(x)   (1 << x)   // Prioity=x normal interrupt enable bit

/*
 * Bit definition for the PIC Fast Interrup Enable register
 */
#define CKPIC_FIER_FIE(x)   (1 << x)   // Prioity=x Fast interrupt enable bit

#define csky_irq_wake NULL

#endif /* _CSKY_PIC_H_ */
