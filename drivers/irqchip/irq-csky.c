/*
 * Driver for C-Sky INTC.
 *
 * Copyright (C) 2016, C-Sky Microsystems Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

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

/*
 * The address map of PIC registers
 */
#define CKPIC_ICR		0x00	/* Interrupt control register(High 16bits) */
#define CKPIC_ISR		0x00	/* Interrupt status register(Low 16bits) */
#define CKPIC_IFR		0x08	/* Interrupt force register Low */
#define CKPIC_IFR_H		0x20	/* Interrupt force register High */
#define CKPIC_IPR		0x0C	/* Interrupt pending register Low */
#define CKPIC_IPR_H		0x24	/* Interrupt pending register High */
#define CKPIC_NIER		0x10	/* Normal interrupt enable register Low */
#define	CKPIC_NIER_H	0x28	/* Normal interrupt enable register High */
#define CKPIC_NIPR		0x14	/* Normal interrupt pending register Low */
#define	CKPIC_NIPR_H	0x2C	/* Normal interrupt pending register High */
#define CKPIC_FIER		0x18	/* Fast interrupt enable register Low */
#define	CKPIC_FIER_H	0x30	/* Fast interrupt enable register High */
#define CKPIC_FIPR		0x1C	/* Fast interrupt pending register Low */
#define	CKPIC_FIPR_H	0x34	/* Fast interrupt pending register High */

#define	CKPIC_PR0		0x40	/* Bits 31-24: PR0 */
								/* Bits 23-16: PR1 */
								/* Bits 15-8:  PR2 */
								/* Bits 7-0:   PR3 */
#define CKPIC_PR4		0x44
#define CKPIC_PR8		0x48
#define CKPIC_PR12		0x4C
#define CKPIC_PR16		0x50
#define CKPIC_PR20		0x54
#define CKPIC_PR24		0x58
#define CKPIC_PR28		0x5C
#define CKPIC_PR32		0x60
#define CKPIC_PR36		0x64
#define CKPIC_PR40		0x68
#define CKPIC_PR44		0x6C
#define CKPIC_PR48		0x70
#define CKPIC_PR52		0x74
#define CKPIC_PR56		0x78
#define CKPIC_PR60		0x7C

static unsigned int ckpic_base;
#define CKPIC_REG(offset)	((void *)(ckpic_base + (offset)))

static void csky_irq_mask(struct irq_data *d)
{
	unsigned int irq;
	void __iomem *addr = NULL;
	unsigned long temp;

	if (d->irq < 32) {
		irq = d->irq;
		addr = CKPIC_REG(CKPIC_NIER);
	} else {
		irq = d->irq - 32;
		addr = CKPIC_REG(CKPIC_NIER_H);
	}

	temp = __raw_readl(addr);
	temp &= ~(1 << irq);
	__raw_writel(temp, addr);
}

static void csky_irq_unmask(struct irq_data *d)
{
	unsigned int irq;
	void __iomem *addr = NULL;
	unsigned long temp;

	if (d->irq < 32) {
		irq = d->irq;
		addr = CKPIC_REG(CKPIC_NIER);
	} else {
		irq = d->irq - 32;
		addr = CKPIC_REG(CKPIC_NIER_H);
	}

	temp = __raw_readl(addr);
	temp |= (1 << irq);
	__raw_writel(temp, addr);
}

struct irq_chip csky_irq_chip = {
	.name		= "csky_intc",
	.irq_mask	= csky_irq_mask,
	.irq_unmask	= csky_irq_unmask,
};

static int csky_irq_map(struct irq_domain *h, unsigned int virq,
				irq_hw_number_t hw_irq_num)
{
	irq_set_chip_and_handler(virq, &csky_irq_chip, handle_level_irq);
	return 0;
}

static const struct irq_domain_ops csky_irq_ops = {
	.map	= csky_irq_map,
	.xlate	= irq_domain_xlate_onecell,
};

unsigned int csky_get_auto_irqno(void)
{
	unsigned int irqno;
	irqno = __raw_readl(CKPIC_REG(CKPIC_ISR)) & 0x7f;
	return irqno;
}

static int __init
csky_intc_init(struct device_node *np, struct device_node *parent)
{
	struct irq_domain *root_domain;
	unsigned int nr_irqs;

	if (parent)
		panic("pic not a root intc\n");

	ckpic_base = (unsigned int)of_iomap(np, 0);
	if (!ckpic_base)
		panic("%s, of_iomap err.\n", __func__);

	if (of_property_read_u32(np, "csky,nr-irqs", &nr_irqs))
		panic("%s, parse csky,nr-irqs failed.\n", __func__);
	if ((nr_irqs != 32) && (nr_irqs != 64))
		panic("%s, invalid nr_irqs(%d).\n", __func__, nr_irqs);

	pr_info("csky pic init, reg: %x, nr_irqs: %d.\n",
			ckpic_base, nr_irqs);

	/*
	 * [31]: 1 = Auto vectored interrupt requests
	 *       0 = Vectored interrupt requests
	 * [30]: 1 = Unique vector numbers for fast vectored interrupt requests
	 *       0 = Same vector number for fast and normal vectored interrupt requests
	 */
	__raw_writel(0x40000000, CKPIC_REG(CKPIC_ICR));

	/*
	 * assigns a priority level to interrupt source x
	 */
	__raw_writel(0x00010203, CKPIC_REG(CKPIC_PR0));
	__raw_writel(0x04050607, CKPIC_REG(CKPIC_PR4));
	__raw_writel(0x08090a0b, CKPIC_REG(CKPIC_PR8));
	__raw_writel(0x0c0d0e0f, CKPIC_REG(CKPIC_PR12));
	__raw_writel(0x10111213, CKPIC_REG(CKPIC_PR16));
	__raw_writel(0x14151617, CKPIC_REG(CKPIC_PR20));
	__raw_writel(0x18191a1b, CKPIC_REG(CKPIC_PR24));
	__raw_writel(0x1c1d1e1f, CKPIC_REG(CKPIC_PR28));
	if (nr_irqs >= 64) {
		__raw_writel(0x20212223, CKPIC_REG(CKPIC_PR32));
		__raw_writel(0x24252627, CKPIC_REG(CKPIC_PR36));
		__raw_writel(0x28292a2b, CKPIC_REG(CKPIC_PR40));
		__raw_writel(0x2c2d2e2f, CKPIC_REG(CKPIC_PR44));
		__raw_writel(0x30313233, CKPIC_REG(CKPIC_PR48));
		__raw_writel(0x34353637, CKPIC_REG(CKPIC_PR52));
		__raw_writel(0x38393a3b, CKPIC_REG(CKPIC_PR56));
		__raw_writel(0x3c3d3e3f, CKPIC_REG(CKPIC_PR60));
	}

	root_domain = irq_domain_add_legacy(np, nr_irqs, 0, 0, &csky_irq_ops, NULL);
	if (!root_domain)
		panic("root irq domain not available\n");

	irq_set_default_host(root_domain);
	return 0;
}

IRQCHIP_DECLARE(csky_intc, "csky,intc", csky_intc_init);
