/*
 * arch/csky/ck6408evb/irq.c ---IRQ vector handles
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006 Hangzhou C-SKY Microsystems co.,ltd.
 * Copyright (C) 2009 by Hu Junshan <junshan_hu@c-sky.com> 
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <asm/irq.h>
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


void static inline sl_writel(u32 val, unsigned long reg)
{
    *(volatile u32 *)(reg) = val;
}
static inline u32 sl_readl(unsigned long reg)
{
    return (*(volatile u32 *)reg);
}

DEFINE_RAW_SPINLOCK(irq_lock);
/*
 *  Mask the interrupt and the irp number is irqno.
 */
static void csky_irq_mask(struct irq_data *d)
{
	unsigned long flags;
	u32 data,reg,shift;

	raw_spin_lock_irqsave(&irq_lock, flags);

	reg = (d->irq >= 32)?(CKPIC_BASE+CKPIC_NIERH):(CKPIC_BASE+CKPIC_NIERL);
	shift = (d->irq >= 32)?(d->irq - 32):d->irq;
	data = sl_readl(reg);
	data &= ~(1UL << shift);
	sl_writel(data, reg);

	raw_spin_unlock_irqrestore(&irq_lock, flags);
}

/*
 *  Unmask the interrupt and the irp number is irqno.
 */
static void csky_irq_unmask(struct irq_data *d)
{
	unsigned long flags;
	u32 data,reg,shift;

	raw_spin_lock_irqsave(&irq_lock, flags);

	reg = (d->irq >= 32)?(CKPIC_BASE+CKPIC_NIERH):(CKPIC_BASE+CKPIC_NIERL);
	shift = (d->irq >= 32)?(d->irq - 32):d->irq;

	data = sl_readl(reg);
	data |= (1UL << shift);
	sl_writel(data, reg);

	raw_spin_unlock_irqrestore(&irq_lock, flags);
}

void csky_irq_disable(struct irq_data *d)
{
	unsigned long flags;
	u32 data,reg,shift;

	raw_spin_lock_irqsave(&irq_lock, flags);

	reg = (d->irq >= 32)?(CKPIC_BASE+CKPIC_NIERH):(CKPIC_BASE+CKPIC_NIERL);
	shift = (d->irq >= 32)?(d->irq - 32):d->irq;

	data = sl_readl(reg);
	data &= ~(1UL << shift);
	sl_writel(data, reg);

	raw_spin_unlock_irqrestore(&irq_lock, flags);
}
EXPORT_SYMBOL(csky_irq_disable);

void csky_irq_enable(struct irq_data *d)
{
	unsigned long flags;
	u32 data,reg,shift;

	raw_spin_lock_irqsave(&irq_lock, flags);

	reg = (d->irq >= 32)?(CKPIC_BASE+CKPIC_NIERH):(CKPIC_BASE+CKPIC_NIERL);
	shift = (d->irq >= 32)?(d->irq - 32):d->irq;

	data = sl_readl(reg);
	data |= (1UL << shift);
	sl_writel(data, reg);

	raw_spin_unlock_irqrestore(&irq_lock, flags);
}
EXPORT_SYMBOL(csky_irq_enable);

static void csky_irq_ack(struct irq_data *d)
{
	unsigned long flags;
	u32 data,reg,shift;

	raw_spin_lock_irqsave(&irq_lock, flags);

	reg = (d->irq >= 32)?(CKPIC_BASE+CKPIC_NIERH):(CKPIC_BASE+CKPIC_NIERL);
	shift = (d->irq >= 32)?(d->irq - 32):d->irq;

	data = sl_readl(reg);
	data &= ~(1UL << shift);
	sl_writel(data, reg);

	raw_spin_unlock_irqrestore(&irq_lock, flags);
}

struct irq_chip csky_irq_chip = 
{
        .name           = "csky",
        .irq_mask           = csky_irq_mask,
        .irq_mask_ack       = csky_irq_mask,
        .irq_unmask         = csky_irq_unmask,
        .irq_eoi            = csky_irq_unmask,
		.irq_enable         = csky_irq_enable,
		.irq_disable        = csky_irq_disable,
		.irq_ack            = csky_irq_ack,
};

unsigned int csky_get_auto_irqno(void)
{
	unsigned int irqno;

	irqno = sl_readl(CKPIC_BASE + CKPIC_ISR) & 0xff;

	return irqno;
}

/*
 *  Initial the interrupt controller of c-sky.
 */
void __init csky_init_IRQ(void)
{
	int i;
	unsigned data;
	
	volatile unsigned int *icrp;
	icrp = (volatile unsigned int *) (CKPIC_BASE);
	/*
	 * Initial the interrupt control register.
	 * 	1. Program the vector to be an auto-vectored.
	 * 	2. Mask all Interrupt.
	 * 	3. Unique vector numbers for fast vectored interrupt requests and fast 
     *  	vectored interrupts Number are 64-95.
	 */
	data = CKPIC_ICR_AVE;
	sl_writel(data,CKPIC_BASE + CKPIC_ICR);

	icrp[CKPIC_PR0 / 4] = 0x00010203;
	icrp[CKPIC_PR4 / 4] = 0x04050607;
	icrp[CKPIC_PR8 / 4] = 0x08090a0b;
	icrp[CKPIC_PR12 / 4] = 0x0c0d0e0f;
	icrp[CKPIC_PR16 / 4] = 0x10111213;
	icrp[CKPIC_PR20 / 4] = 0x14151617;
	icrp[CKPIC_PR24 / 4] = 0x18191a1b;
	icrp[CKPIC_PR28 / 4] = 0x1c1d1e1f;
	icrp[CKPIC_PR32 / 4] = 0x20212223;
	icrp[CKPIC_PR36 / 4] = 0x24252627;
	icrp[CKPIC_PR40 / 4] = 0x28292a2b;
	icrp[CKPIC_PR44 / 4] = 0x2c2d2e2f;
	icrp[CKPIC_PR48 / 4] = 0x30313233;
	icrp[CKPIC_PR52 / 4] = 0x34353637;
	icrp[CKPIC_PR56 / 4] = 0x38393a3b;
	icrp[CKPIC_PR60 / 4] = 0x3c3d3e3f;

	for(i = 0; i < NR_IRQS; i++)
	{
		irq_set_chip_and_handler(i, &csky_irq_chip, handle_level_irq);
	}
}


