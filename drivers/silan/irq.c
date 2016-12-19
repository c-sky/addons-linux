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
#include "ckpic.h"
#include "silan_irq.h"

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


