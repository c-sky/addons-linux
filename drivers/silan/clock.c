/*
 * Setting up the clock on MSP SOCs.  No RTC typically.
 *
 * Pan jianguang, panjianguang@silan.com.cn
 * Copyright (C) 1999,2010 Silan.  All rights reserved.
 *
 * Fixes:
 *		XuHang	2013-11-27: Update code of clock according to SUVIII PLL network
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 */
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/ptrace.h>
#include <linux/clk.h>
#include <linux/err.h>

#include "silan_resources.h"
#include "silan_regs.h"
#include "silan_def.h"
#include "silan_clk.h"
#include "silan_generic.h"

/* We originally used an mutex here, but some contexts (see resume)
 * are calling functions such as clk_set_parent() with IRQs disabled
 * causing an BUG to be triggered.
 */
DEFINE_SPINLOCK(clocks_lock);

#if USE_PERFECT_PLL_SET_FLOW
static struct sys_pll_freq_t sys_pll_freqs[] =
{
	{
		.freq = 800000000,
		.bwadj = 799,
		.clkf = 1599,
		.clkr = 26,
		.clkod = 0,
		.cs = 0
	},
	
};
#endif

/* Get audio PLL clock */
unsigned long get_silan_audio_pllclk(void)
{
#if defined(CONFIG_SILAN_FPGA)
	return 38000000;
#else
	u32 regval,fs;	

	regval = sl_readl(SILAN_SYS_REG6);	
	fs = SILAN_CLK_APLL_CS_GET(regval);
	switch(fs)
	{
		case 0:
			return 147456000;
		case 1:
			return 135475200;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif
}
EXPORT_SYMBOL(get_silan_audio_pllclk);

/* Set alterable audio clock */
static int set_alter_audio_pll(unsigned long rate)
{
	printk("ERROR: %s: function is not implemented\n", __FUNCTION__);
	return -1;
}

/*
 * Set audio PLL clock
 * @rate: clock to be set
 */
int set_silan_audio_pllclk(unsigned long rate)
{
	u32 regval, fs;

	if (rate == 147456000)
		fs = 0;
	else if (rate == 135475200)
		fs = 1;
	else {
		fs = 2;
		if (set_alter_audio_pll(rate))
			return -1;
	}

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG6);
	regval &= (~SILAN_CLK_APLL_CS_MASK);
	regval |= SILAN_CLK_APLL_CS_SET(fs);
	sl_writel(regval, SILAN_SYS_REG6);
	spin_unlock(&clocks_lock);
	
	return 0;
}
EXPORT_SYMBOL(set_silan_audio_pllclk);

/* Get video display PLL clock */
unsigned long get_silan_video_pllclk(void)
{
#if defined(CONFIG_SILAN_FPGA)
	return 38000000;
#else
	u32 regval,fs;	

	regval = sl_readl(SILAN_SYS_REG8);	
	fs = SILAN_CLK_VPLL_CS_GET(regval);
	switch(fs)
	{
		case 0:
			return 1188000000;
		case 1:
			return 1186816000;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif
}
EXPORT_SYMBOL(get_silan_video_pllclk);

/* Set alterable video clock */
static int set_alter_video_pll(unsigned long rate)
{
	printk("ERROR: %s: function is not implemented\n", __FUNCTION__);
	return -1;
}

/* Set video PLL clock */
int set_silan_video_pllclk(unsigned long rate)
{
	u32 regval, fs;

	if (rate == 1188000000)
		fs = 0;
	else if (rate == 1186816000)
		fs = 1;
	else {
		fs = 2;
		if (set_alter_video_pll(rate))
			return -1;
	}

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG8);
	regval &= (~SILAN_CLK_VPLL_CS_MASK);
	regval |= SILAN_CLK_VPLL_CS_SET(fs);
	sl_writel(regval, SILAN_SYS_REG8);
	spin_unlock(&clocks_lock);
	
	return 0;
}
EXPORT_SYMBOL(set_silan_video_pllclk);

/* Get system PLL clock */
unsigned long get_silan_sys_pllclk(void)
{
#ifdef CONFIG_SILAN_FPGA
	return 30000000;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG33);
	fs = SILAN_CLK_SPLL_CS_GET(regval);
	switch (fs)
	{
		case 0:
			return 1000000000;
		case 1:
			return 1200000000;
		case 2:
			return 800000000;
		case 3:
			return 900000000;
		case 4:
			return 720000000;
		case 5:
			return 600000000;
		case 6:
			return 400000000;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif
}
EXPORT_SYMBOL(get_silan_sys_pllclk);

#ifndef CONFIG_SILAN_FPGA
/* Set alterable system PLL clock */
static int set_alter_sys_pll(unsigned long rate)
{
	printk("ERROR: %s: function is not implemented\n", __FUNCTION__);
	return -1;
}
#endif
/*
 * Set system PLL clock
 * @rate: Frequency to be set
 */
int set_silan_sys_pllclk(unsigned long rate)
{
#if USE_PERFECT_PLL_SET_FLOW
	int i;
	u32 regval;
	struct sys_pll_freq_t *pll_freq = sys_pll_freqs;

	for (i = 0; i < ARRAY_SIZE(sys_pll_freqs); i++) {
		if (pll_freq->freq != rate) {
			pll_freq++;
			continue;
		}

		spin_lock(&clocks_lock);

		/* Step1: Enter system PLL reset status */
		regval = sl_readl(SILAN_SYS_REG32);
		regval |= SILAN_CLK_SPLL_SR;
		sl_writel(regval, SILAN_SYS_REG32);

		/* Step2: Set REG33 */
		regval = ENCODE_SYS_REG33(pll_freq->clkr, pll_freq->clkf, pll_freq->clkod, pll_freq->cs);
		sl_writel(regval, SILAN_SYS_REG33);

		/* Step3: Set bandwidth divider */
		regval = sl_readl(SILAN_SYS_REG32);
		regval &= (~(0xFFF << 16));
		regval |= ((pll_freq->bwadj && 0xFFF) << 16);
		sl_writel(regval, SILAN_SYS_REG32);

		spin_unlock(&clocks_lock);

		break;
	}

	if (i >= ARRAY_SIZE(sys_pll_freqs)) {
		printk("Invalid rate %ld\n", rate);
		return -1;
	}

	/* Step4: Keep system PLL in reset status for 5us */
/* FIXME */
//	usleep(5);

	/* Step5: Exit system PLL reset status */
	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG32);
	regval &= (~SILAN_CLK_SPLL_SR);
	sl_writel(regval, SILAN_SYS_REG32);
	spin_unlock(&clocks_lock);

	/* Step6: Wait until PLL is locked */
	do {
		regval = sl_readl(SILAN_SEC_AR_STATUS);
	} while (regval & SILAN_SEC_AR_STATUS_SYS);
	
	return 0;
#else

#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;

	if (rate >= 1200000000)
		fs = 1;
	else if (rate >= 1000000000) {
		if (IS_NEAR_BIG(1000000000, rate, 1200000000))
			fs = 1;
		else
			fs = 2;
	} else if (rate >= 900000000) {
		if (IS_NEAR_BIG(900000000, rate, 1000000000))
			fs = 2;
		else
			fs = 3;
	} else if (rate >= 800000000) {
		if (IS_NEAR_BIG(800000000, rate, 900000000))
			fs = 3;
		else
			fs = 0;
	} else if (rate >= 720000000) {
		if (IS_NEAR_BIG(720000000, rate, 800000000))
			fs = 0;
		else
			fs = 4;
	} else if (rate >= 600000000) {
		if (IS_NEAR_BIG(600000000, rate, 720000000))
			fs = 4;
		else
			fs = 5;
	} else if (rate >= 400000000) {
		if (IS_NEAR_BIG(400000000, rate, 600000000))
			fs = 5;
		else
			fs = 6;
	} else {
		fs = 7;
		if (set_alter_sys_pll(rate))
			return -1;
	}

#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG33);
	regval &= (~SILAN_CLK_SPLL_CS_MASK);
	regval |= SILAN_CLK_SPLL_CS_SET(fs);
	sl_writel(regval, SILAN_SYS_REG33);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */

#endif /* USE_PERFECT_PLL_SET_FLOW */
}
EXPORT_SYMBOL(set_silan_sys_pllclk);

#ifndef CONFIG_SILAN_FPGA
/* Get alterable CPU clock */
static unsigned long get_alter_cpu_pll(void)
{
	printk("ERROR: %s: function is not implemented\n", __FUNCTION__);
	return 0;
}
#endif
/* Get CPU PLL clock */
unsigned long get_silan_cpu_pllclk(void)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*2; 
#else
	u32 regval, fs;
	
	regval = sl_readl(SILAN_PMU_PLL_STATUS_H);
	fs = SILAN_CLK_CPLL_STATUS_GET_FS(regval);
	switch (fs)
	{
		case 0:
		case 6:
		case 7:
			return 800000000;
		case 1:
			return 1200000000;
		case 2:
			return 1000000000;
		case 3:
			return 600000000;
		case 4:
			return 400000000;
		case 5:
			return get_alter_cpu_pll();
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif
}
EXPORT_SYMBOL(get_silan_cpu_pllclk);

/* Set alterable CPU clock */
static int set_alter_cpu_pll(unsigned long rate)
{
	printk("ERROR: %s: function is not implemented\n", __FUNCTION__);
	return -1;
}

/* 
 * Set CPU PLL clock
 * @rate: rate to be set
 */
int set_silan_cpu_pllclk(unsigned long rate)
{
	u32 regval, fs;

	if (rate >= 1200000000)
		fs = 1;
	else if (rate >= 1000000000) {
		if (IS_NEAR_BIG(1000000000, rate, 1200000000))
			fs = 1;
		else
			fs = 2;
	} else if (rate >= 800000000) {
		if (IS_NEAR_BIG(800000000, rate, 1000000000))
			fs = 2;
		else
			fs = 0;
	} else if (rate >= 600000000) {
		if (IS_NEAR_BIG(600000000, rate, 800000000))
			fs = 0;
		else
			fs = 3;
	} else if (rate >= 400000000) {
		if (IS_NEAR_BIG(400000000, rate, 600000000))
			fs = 3;
		else
			fs = 4;
	} else {
		fs = 5;
		if (set_alter_cpu_pll(rate))
			return -1;
	}

#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_PMU_PLL_CFG_H);
	regval &= (~SILAN_CLK_CPLL_CS_MASK);
	regval |= SILAN_CLK_CPLL_CS_SET(fs);
	sl_writel(regval, SILAN_PMU_PLL_CFG_H);
	spin_unlock(&clocks_lock);
	
	return 0;
}
EXPORT_SYMBOL(set_silan_cpu_pllclk);

/******************************************************************************
 * Bars & buses
 *****************************************************************************/
/*
 * Get clock of DX bar
 */
unsigned long clk_get_dxb(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_DXB_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/4;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/6;
		case 4:
			return get_silan_sys_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of DX bar
 * @rate: rate to be set
 */
int clk_set_dxb_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	parent_clk = get_silan_video_pllclk();
	pllclk[2] = parent_clk / 4;
	pllclk[3] = parent_clk / 8;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_DXB_MASK_FS);
	regval |= SILAN_CLK_DXB_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of PX bar
 */
unsigned long clk_get_pxb(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_PXB_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/6;
		case 1:
			return get_silan_sys_pllclk()/4;
		case 2:
			return get_silan_sys_pllclk()/6;
		case 3:
			return get_silan_sys_pllclk()/8;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of PX bar
 * @rate: rate to be set
 */
int clk_set_pxb_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[4], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 4;
	pllclk[2] = parent_clk / 6;
	pllclk[3] = parent_clk / 8;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 4; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_PXB_MASK_FS);
	regval |= SILAN_CLK_PXB_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of LSP
 */
unsigned long clk_get_lsp(struct clk * clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_LSP_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/10;
		case 1:
			return get_silan_sys_pllclk()/8;
		case 2:
			return get_silan_sys_pllclk()/16;
		case 3:
			return (clk_get_pmu(NULL));
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of LSP
 * @rate: rate to be set
 */
int clk_set_lsp_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[4], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 10; 
	pllclk[1] = parent_clk / 8;
	pllclk[2] = parent_clk / 16;
	pllclk[3] = clk_get_pmu(NULL);
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 4; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_LSP_MASK_FS);
	regval |= SILAN_CLK_LSP_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of CX bar
 */
unsigned long clk_get_cxb(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG11);
	fs = SILAN_CLK_CX_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/6;
		case 1:
			return get_silan_sys_pllclk()/4;
		case 2:
			return get_silan_sys_pllclk()/6;
		case 3:
			return get_silan_sys_pllclk()/8;
		case 4:
			return get_silan_sys_pllclk()/16;
		case 5:
			return (clk_get_pmu(NULL));
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of CX bar
 * @rate: rate to be set
 */
int clk_set_cxb_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[6], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 4;
	pllclk[2] = parent_clk / 6;
	pllclk[3] = parent_clk / 8;
	pllclk[4] = parent_clk / 16;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 6;
	pllclk[5] = clk_get_pmu(NULL);
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 6; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG11);	
	regval &= (~SILAN_CLK_CX_MASK_FS);
	regval |= SILAN_CLK_CX_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG11);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of VPU and JPEG bus
 */
unsigned long clk_get_vjb(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_VJB_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/3;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of VPU and JPEG bus
 * @rate: rate to be set
 */
int clk_set_vjb_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_VJB_MASK_FS);
	regval |= SILAN_CLK_VJB_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of HPU and SPU AXI bus
 */
unsigned long clk_get_hpu_spu_bus(struct clk *clk)
{
	return 0;
}

int clk_set_hpu_spu_bus_rate(struct clk *clk, unsigned long rate)
{
	return 0;
}
/******************************************************************************
 * Devices based on system PLL
 *****************************************************************************/

/*
 * Get clock of VPU
 */
unsigned long clk_get_vpu0(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_VPU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/4;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/6;
		case 4:
			return get_silan_sys_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of VPU
 * @rate: rate to be set
 */
int clk_set_vpu0_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	parent_clk = get_silan_video_pllclk();
	pllclk[2] = parent_clk / 4;
	pllclk[3] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}

#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_VPU_MASK_FS);
	regval |= SILAN_CLK_VPU_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of VPU
 */
unsigned long clk_get_vpu1(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG42);	
	fs = SILAN_CLK_VPU1_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/4;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/6;
		case 4:
			return get_silan_sys_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of VPU
 * @rate: rate to be set
 */
int clk_set_vpu1_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	parent_clk = get_silan_video_pllclk();
	pllclk[2] = parent_clk / 4;
	pllclk[3] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}

#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);//???????
	regval = sl_readl(SILAN_SYS_REG42);	
	regval &= (~SILAN_CLK_VPU1_MASK_FS);
	regval |= SILAN_CLK_VPU1_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG42);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of GPU
 */
unsigned long clk_get_gpu(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_GPU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/3;
		case 1:
			return get_silan_sys_pllclk()/2;
		case 2:
			return get_silan_sys_pllclk()/3;
		case 3:
			return get_silan_video_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of GPU
 * @rate: rate to be set
 */
int clk_set_gpu_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 2;
	pllclk[2] = parent_clk / 3;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}

#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_GPU_MASK_FS);
	regval |= SILAN_CLK_GPU_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of JPU
 */
unsigned long clk_get_jpu(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_JPU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of JPU
 * @rate: rate to be set
 */
int clk_set_jpu_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[4], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[3] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 4; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_JPU_MASK_FS);
	regval |= SILAN_CLK_JPU_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of VPP
 */
unsigned long clk_get_vpp(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG2);	
	fs = SILAN_CLK_VPP_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of VPP
 * @rate: rate to be set
 */
int clk_set_vpp_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[4], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[3] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 4; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}

#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG2);	
	regval &= (~SILAN_CLK_VPP_MASK_FS);
	regval |= SILAN_CLK_VPP_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG2);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of DSP0
 */
unsigned long clk_get_dsp0(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG3);	
	fs = SILAN_CLK_DSP0_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/3;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		case 5:
			return get_silan_sys_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of DSP0
 * @rate: rate to be set
 */
int clk_set_dsp0_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[6], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[2] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	pllclk[5] = parent_clk / 6;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 6; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG3);	
	regval &= (~SILAN_CLK_DSP0_MASK_FS);
	regval |= SILAN_CLK_DSP0_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG3);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of DSP1
 */
unsigned long clk_get_dsp1(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG3);	
	fs = SILAN_CLK_DSP1_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/3;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		case 5:
			return get_silan_sys_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of DSP1
 * @rate: rate to be set
 */
int clk_set_dsp1_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[6], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[2] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	pllclk[5] = parent_clk / 6;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 6; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG3);	
	regval &= (~SILAN_CLK_DSP1_MASK_FS);
	regval |= SILAN_CLK_DSP1_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG3);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of SD/EMMC/SDIO
 */
unsigned long clk_get_sd_emmc_sdio(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/2;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG3);
	switch (clk->clk_id[0])
	{
		case SILAN_CLK_SD:
			fs = SILAN_CLK_SD_GET_FS(regval);
			break;
		case SILAN_CLK_EMMC:
			fs = SILAN_CLK_EMMC_GET_FS(regval);
			break;
		case SILAN_CLK_SDIO:
			fs = SILAN_CLK_SDIO_GET_FS(regval);
			break;
		default:
			printk("ERROR: %s: Invalid device ID %d\n", __FUNCTION__, clk->clk_id[0]);
			return 0;
			break;
	}

	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/6;
		case 1:
			return get_silan_sys_pllclk()/4;
		case 2:
			return get_silan_sys_pllclk()/6;
		default:
			break;
	}

	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of SD/EMMC/SDIO
 * @rate: rate to be set
 */
int clk_set_sd_emmc_sdio_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 45000000;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[3], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 4;
	pllclk[2] = parent_clk / 6;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 3; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	if (clk->clk_id[0] == SILAN_CLK_SD) 
	{
		spin_lock(&clocks_lock);
		regval = sl_readl(SILAN_SYS_REG3);	
		regval &= (~SILAN_CLK_SD_MASK_FS);
		regval |= SILAN_CLK_SD_SET_FS(fs);
		sl_writel(regval, SILAN_SYS_REG3);
		spin_unlock(&clocks_lock);
	} 
	else if (clk->clk_id[0] == SILAN_CLK_EMMC) 
	{
		spin_lock(&clocks_lock);
		regval = sl_readl(SILAN_SYS_REG3);
		regval &= (~SILAN_CLK_EMMC_MASK_FS);
		regval |= SILAN_CLK_EMMC_SET_FS(fs);
		sl_writel(regval, SILAN_SYS_REG3);
		spin_unlock(&clocks_lock);
	} 
	else if(clk->clk_id[0] == SILAN_CLK_SDIO) 
	{
		spin_lock(&clocks_lock);
		regval = sl_readl(SILAN_SYS_REG3);
		regval &= (~SILAN_CLK_SDIO_MASK_FS);
		regval |= SILAN_CLK_SDIO_SET_FS(fs);
		sl_writel(regval, SILAN_SYS_REG3);
		spin_unlock(&clocks_lock);
	} 
	else 
	{
		printk("ERROR: %s: Invalid device ID %d\n", __FUNCTION__, clk->clk_id[0]);
		return -1;
	}

	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of VIU
 */
unsigned long clk_get_viu(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG3);	
	fs = SILAN_CLK_VIU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of VIU
 * @rate: rate to be set
 */
int clk_set_viu_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[4], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[3] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 4; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG3);	
	regval &= (~SILAN_CLK_VIU_MASK_FS);
	regval |= SILAN_CLK_VIU_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG3);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Get clock of SPU
 */
unsigned long clk_get_spu(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_PMU_STATUS);	
	fs = SILAN_CLK_SPU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_cpu_pllclk();
		case 1:
			return get_silan_cpu_pllclk()/2;
		case 2:
			return get_silan_cpu_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/2;
		case 4:
			return get_silan_video_pllclk()/4;
		case 5:
			return get_silan_sys_pllclk()/2;
		case 6:
			return get_silan_sys_pllclk()/4;
		case 7:
			return (clk_get_pmu(NULL));
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*
 * Set clock of SPU
 * @rate: rate to be set
 */
int clk_set_spu_rate(struct clk *clk, unsigned long rate)
{
	return 0;
}

/*
 * Get clock of PMU
 */
unsigned long clk_get_pmu(struct clk *clk)
{
	u32 regval,fs;	

	regval = sl_readl(SILAN_SYS_REG3);	
	fs = SILAN_CLK_PMU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return EXTERNAL_OSC_FREQ;
			break;
		case 1:
			return EXTERNAL_OSC_FREQ/8;
			break;
		case 2:
			return EXTERNAL_OSC_FREQ/64;
			break;
		case 3:
			return EXTERNAL_OSC_FREQ/512;
			break;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
}

/*
 * Set clock of PMU
 * @rate: PMU clock to be set
 */
int clk_set_pmu_rate(struct clk *clk, unsigned long rate)
{
	u32 regval, fs;	

	if (rate >= EXTERNAL_OSC_FREQ)
		fs = 0;
	else if (rate >= EXTERNAL_OSC_FREQ/8) {
		if (IS_NEAR_BIG(EXTERNAL_OSC_FREQ/8, rate, EXTERNAL_OSC_FREQ))
			fs = 0;
		else
			fs = 1;
	} else if (rate >= EXTERNAL_OSC_FREQ/64) {
		if (IS_NEAR_BIG(EXTERNAL_OSC_FREQ/64, rate, EXTERNAL_OSC_FREQ/8))
			fs = 1;
		else
			fs = 2;
	} else if (rate >= EXTERNAL_OSC_FREQ/512) {
		if (IS_NEAR_BIG(EXTERNAL_OSC_FREQ/512, rate, EXTERNAL_OSC_FREQ/64))
			fs = 2;
		else
			fs = 3;
	} else {
		fs = 3;
	}

#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, OSC freq %d, fs %d\n", __FUNCTION__, rate, EXTERNAL_OSC_FREQ, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG3);
	regval &= (~SILAN_CLK_PMU_MASK_FS);
	regval |= SILAN_CLK_PMU_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG3);
	spin_unlock(&clocks_lock);
	
	return 0;
}

/*
 * Set clock of SPDIF
 * @rate: rate to be set
 */
int clk_set_spdif_rate(struct clk *clk, unsigned long rate)
{
	u32 regval, spdif_fs, am_fs;	

	switch (rate) {
		case 44100:
			am_fs = 0x0;
			spdif_fs = 0x2;
			if (set_silan_audio_pllclk(135475200))
				return -1;
			break;
		case 88200:
			am_fs = 0x0;
			spdif_fs = 0x1;
			if (set_silan_audio_pllclk(135475200))
				return -1;
			break;
		case 176400:
			am_fs = 0x0;
			spdif_fs = 0x0;
			if (set_silan_audio_pllclk(135475200))
				return -1;
			break;
		case 48000:
			am_fs = 0x0;
			spdif_fs = 0x2;
			if (set_silan_audio_pllclk(147456000))
				return -1;
			break;
		case 96000:
			am_fs = 0x0;
			spdif_fs = 0x1;
			if (set_silan_audio_pllclk(147456000))
				return -1;
			break;
		case 192000:
			am_fs = 0x0;
			spdif_fs = 0x0;
			if (set_silan_audio_pllclk(147456000))
				return -1;
			break;
		default:
			printk("ERROR: %s: Invalid SPDIF rate (%ld)\n", __FUNCTION__, rate);
			return -1;
	}

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG4);
	/* Audio main clock frequency selection */
	regval &= (~SILAN_CLK_AM_MASK_FS);
	regval |= SILAN_CLK_AM_SET_FS(am_fs);
	/* SPDIF clock frequency selection */
	regval &= (~SILAN_CLK_SPDIF_MASK_FS);
	regval |= SILAN_CLK_SPDIF_SET_FS(spdif_fs);
	/* SPDIF clock enable */
	regval |= SILAN_CLK_SPDIF_EN;
	sl_writel(regval, SILAN_SYS_REG4);
	spin_unlock(&clocks_lock);
	
	return 0;
}

/*
 * Set clock of I2S t4
 * @rate: rate to be set
 */
int clk_set_i2s_t4_rate(struct clk *clk, unsigned long rate)
{
	u32 reg0_value, reg1_value;

	reg0_value = sl_readl(SILAN_SYS_REG4);
	reg0_value &= (~SILAN_CLK_I2ST4_MASK_FS);
	reg1_value = sl_readl(SILAN_SYS_REG6);
	reg1_value &= (~SILAN_CLK_APLL_CS_MASK);

	switch(rate) {
		case 11025:
			reg0_value |= 0x6<<12;	/* extern main clock fs */
			reg1_value |= 0x1<<24;	/* audio pll config */
			break;
		case 22050:
			reg0_value |= 0x4<<12;	/* extern main clock fs */
			reg1_value |= 0x1<<24;	/* audio pll config */
			break;
		case 44100:
			reg0_value |= 0x2<<12;	/* extern main clock fs */
			reg1_value |= 0x1<<24;	/* audio pll config */
			break;
		case 88200:
			reg0_value |= 0x1<<12;	/* extern main clock fs */
			reg1_value |= 0x1<<24;	/* audio pll config */
			break;
		case 176400:
			reg0_value |= 0x0<<12;	/* extern main clock fs */
			reg1_value |= 0x1<<24;	/* audio pll config */
			break;
		case 8000:
			reg0_value |= 0x7<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		case 12000:
			reg0_value |= 0x6<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		case 16000:
			reg0_value |= 0x5<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		case 24000:
			reg0_value |= 0x4<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		case 32000:
			reg0_value |= 0x3<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		case 48000:
			reg0_value |= 0x2<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		case 96000:
			reg0_value |= 0x1<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		case 192000:
			reg0_value |= 0x0<<12;	/* extern main clock fs */
			reg1_value |= 0x0<<24;	/* audio pll config */
			break;
		default:
			printk("ERROR: %s: Invalid I2S T4 rate (%ld)\n", __FUNCTION__, rate);
			return -1;
	}
	/* Audio main clock fs */
	reg0_value &= ~0x1;
	/* T4 audio frame selection, bit clock and audio main clock enable */
	reg0_value |= (SILAN_CLK_I2ST4_FS_EN | SILAN_CLK_I2ST4_BIT_EN | SILAN_CLK_I2ST4_M_EN);

	sl_writel(reg0_value, SILAN_SYS_REG4);
	sl_writel(reg1_value, SILAN_SYS_REG6);

	return 0;
}

/*
 * Set clock of I2S MIC
 * @rate: rate to be set
 */
int clk_set_i2s_mic_rate(struct clk *clk, unsigned long rate)
{
	u32 reg0_value, reg1_value;
	u32 i2cmic_fs,audio_fs;

	reg0_value = sl_readl(SILAN_SYS_REG4);
	reg0_value &= (~SILAN_CLK_I2SMIC_MASK_FS);
	reg1_value = sl_readl(SILAN_SYS_REG6);
	reg1_value &= (~SILAN_CLK_APLL_CS_MASK);

	switch(rate) {
		case 11025:
			i2cmic_fs = 0x6;
			audio_fs  = 0x1;
			break;
		case 22050:
			i2cmic_fs = 0x4;
			audio_fs  = 0x1;
			break;
		case 44100:
			i2cmic_fs = 0x2;
			audio_fs  =	0x1;
			break;
		case 88200:
			i2cmic_fs = 0x1;
			audio_fs  =	0x1;
			break;
		case 176400:
			i2cmic_fs = 0x0;
			audio_fs  = 0x1;
			break;
		case 8000:
			i2cmic_fs = 0x7;
			audio_fs  = 0x0;
			break;
		case 12000:
			i2cmic_fs = 0x6;
			audio_fs  = 0x0;
			break;
		case 16000:
			i2cmic_fs = 0x5;
			audio_fs  = 0x0;
			break;
		case 24000:
			i2cmic_fs = 0x4;
			audio_fs  = 0x0;
			break;
		case 32000:
			i2cmic_fs = 0x3;
			audio_fs  = 0x0;
			break;
		case 48000:
			i2cmic_fs = 0x2;
			audio_fs  = 0x0;
			break;
		case 96000:
			i2cmic_fs = 0x1;
			audio_fs  = 0x0;
			break;
		case 192000:
			i2cmic_fs = 0x0;
			audio_fs  = 0x0;
			break;
		default:
			printk("ERROR: %s: Invalid I2S MIC rate (%ld)\n", __FUNCTION__, rate);
			return -1;
	}

	reg0_value |= SILAN_CLK_I2SMIC_SET_FS(i2cmic_fs);	/* extern main clock fs */
	reg1_value |= SILAN_CLK_APLL_CS_SET(audio_fs);		/* audio pll config */

	reg0_value &= ~SILAN_CLK_AM_MASK_FS;		/* audio main clock fs */
	reg0_value |= SILAN_CLK_I2SMIC_M_EN;		/* I2S MIC main clock enable */

	sl_writel(reg0_value, SILAN_SYS_REG4);
	sl_writel(reg1_value, SILAN_SYS_REG6);

	return 0;
}

/*
 * Set clock of I2S HDMI
 * @rate: rate to be set
 */
int clk_set_i2s_hdmi_rate(struct clk *clk, unsigned long rate)
{
	u32 reg0_value, reg1_value;
	u32 i2cmic_fs,audio_fs;

	reg0_value = sl_readl(SILAN_SYS_REG4);
	reg0_value &= (~SILAN_CLK_I2SHDMI_MASK_FS);
	reg1_value = sl_readl(SILAN_SYS_REG6);
	reg1_value &= (~SILAN_CLK_APLL_CS_MASK);

	switch(rate) {
		case 11025:
			i2cmic_fs = 0x6;
			audio_fs  = 0x1;
			break;
		case 22050:
			i2cmic_fs = 0x4;
			audio_fs  = 0x1;
			break;
		case 44100:
			i2cmic_fs = 0x2;
			audio_fs  =	0x1;
			break;
		case 88200:
			i2cmic_fs = 0x1;
			audio_fs  =	0x1;
			break;
		case 176400:
			i2cmic_fs = 0x0;
			audio_fs  = 0x1;
			break;
		case 8000:
			i2cmic_fs = 0x7;
			audio_fs  = 0x0;
			break;
		case 12000:
			i2cmic_fs = 0x6;
			audio_fs  = 0x0;
			break;
		case 16000:
			i2cmic_fs = 0x5;
			audio_fs  = 0x0;
			break;
		case 24000:
			i2cmic_fs = 0x4;
			audio_fs  = 0x0;
			break;
		case 32000:
			i2cmic_fs = 0x3;
			audio_fs  = 0x0;
			break;
		case 48000:
			i2cmic_fs = 0x2;
			audio_fs  = 0x0;
			break;
		case 96000:
			i2cmic_fs = 0x1;
			audio_fs  = 0x0;
			break;
		case 192000:
			i2cmic_fs = 0x0;
			audio_fs  = 0x0;
			break;
		default:
			printk("ERROR: %s: Invalid I2S HDMI rate (%ld)\n", __FUNCTION__, rate);
			return -1;
	}
	reg0_value |= SILAN_CLK_I2SHDMI_SET_FS(i2cmic_fs);	/* extern main clock fs */
	reg1_value |= SILAN_CLK_APLL_CS_SET(audio_fs);		/* audio pll config */

	reg0_value &= ~SILAN_CLK_AM_MASK_FS;		/* audio main clock fs */
	/* HDMI audio frame selection, bit clock and audio main clock enable */
	reg0_value |= (SILAN_CLK_I2SHDMI_FS_EN | SILAN_CLK_I2SHDMI_BIT_EN | SILAN_CLK_I2SHDMI_M_EN);

	sl_writel(reg0_value, SILAN_SYS_REG4);
	sl_writel(reg1_value, SILAN_SYS_REG6);

	return 0;
}

/* MPU Core Clock Get*/
unsigned long clk_get_mpu(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG42);	
	fs = SILAN_CLK_MPU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/3;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
/*MPU Core Clock Set*/
int clk_set_mpu_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG42);	
	regval &= (~SILAN_CLK_MPU_MASK_FS);
	regval |= SILAN_CLK_MPU_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG42);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/* VIU And VPREP AXI Bus Clock Get*/
unsigned long clk_get_vip_aclk(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG42);	
	fs = SILAN_CLK_VIP_ACLK_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/3;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
/* VIU And VPREP AXI Bus Clock Set*/
int clk_set_vip_aclk_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG42);	
	regval &= (~SILAN_CLK_VIP_ACLK_MASK_FS);
	regval |= SILAN_CLK_VIP_ACLK_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG42);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/* VPREP Core Clock Get*/
unsigned long clk_get_vprep(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG42);	
	fs = SILAN_CLK_VPREP_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/3;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
/* VPREP Core Clock Set*/
int clk_set_vprep_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[2] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG42);	
	regval &= (~SILAN_CLK_VPREP_MASK_FS);
	regval |= SILAN_CLK_VPREP_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG42);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/* VENC(h264) Core Clock Get*/
unsigned long clk_get_venc(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG42);	
	fs = SILAN_CLK_VENC_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/3;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
/* VENC(H264) Core Clock Set*/
int clk_set_venc_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[2] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG42);	
	regval &= (~SILAN_CLK_VENC_MASK_FS);
	regval |= SILAN_CLK_VENC_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG42);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*FDIP Clock Get*/
unsigned long clk_get_fdip(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG42);	
	fs = SILAN_CLK_FDIP_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/3;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
/* FDIP Clock Set*/
int clk_set_fdip_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	pllclk[4] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG42);	
	regval &= (~SILAN_CLK_FDIP_MASK_FS);
	regval |= SILAN_CLK_FDIP_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG42);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/* XPU */
unsigned long clk_get_secdmx(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG11);	
	fs = SILAN_CLK_SECDMX_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/4;
		case 3:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
/* Memory Copy DMA Clock Set*/
int clk_set_secdmx_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[4], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[3] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 4; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG11);	
	regval &= (~SILAN_CLK_SECDMX_MASK_FS);
	regval |= SILAN_CLK_SECDMX_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG11);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/*SDRAM Clock Get*/
unsigned long clk_get_sdram(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG3);	
	fs = SILAN_CLK_SDRAM_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/6;
		case 1:
			return get_silan_video_pllclk()/6;
		case 2:
			return get_silan_sys_pllclk()/8;
		case 3:
			return get_silan_sys_pllclk()/16;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

/* SDRAM Clock Set*/
int clk_set_sdram_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[4], dist, min_dist, parent_clk;

	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 6;
	pllclk[2] = parent_clk / 8;
	pllclk[3] = parent_clk / 16;
	parent_clk = get_silan_video_pllclk();
	pllclk[1] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 4; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG3);	
	regval &= (~SILAN_CLK_SDRAM_MASK_FS);
	regval |= SILAN_CLK_SDRAM_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG3);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

unsigned long clk_get_ivs_aclk(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG48);	
	fs = SILAN_CLK_IVS_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/3;
		case 1:
			return get_silan_sys_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
int clk_set_ivs_aclk_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	pllclk[4] = parent_clk / 6;
	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG48);	
	regval &= (~SILAN_CLK_IVS_MASK_FS);
	regval |= SILAN_CLK_IVS_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG48);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */

}

unsigned long clk_get_ccu(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG48);	
	fs = SILAN_CLK_CCU_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/4;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_sys_pllclk()/3;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

int clk_set_ccu_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 4;
	pllclk[1] = parent_clk / 3;
	pllclk[4] = parent_clk / 6;
	parent_clk = get_silan_sys_pllclk();
	pllclk[2] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG48);	
	regval &= (~SILAN_CLK_CCU_MASK_FS);
	regval |= SILAN_CLK_CCU_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG48);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */

}

unsigned long clk_get_hpb_aclk(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG48);	
	fs = SILAN_CLK_HPB_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_sys_pllclk()/3;
		case 1:
			return get_silan_video_pllclk()/3;
		case 2:
			return get_silan_video_pllclk()/4;
		case 3:
			return get_silan_sys_pllclk()/4;
		case 4:
			return get_silan_video_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

int clk_set_hpb_aclk_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[5], dist, min_dist, parent_clk;

	parent_clk = get_silan_video_pllclk();
	pllclk[1] = parent_clk / 3;
	pllclk[2] = parent_clk / 4;
	pllclk[4] = parent_clk / 6;
	parent_clk = get_silan_sys_pllclk();
	pllclk[0] = parent_clk / 3;
	pllclk[3] = parent_clk / 4;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 5; i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG48);	
	regval &= (~SILAN_CLK_HPB_MASK_FS);
	regval |= SILAN_CLK_HPB_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG48);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

unsigned long clk_get_isp(struct clk *clk)
{
#if defined(CONFIG_SILAN_FPGA)
	return get_silan_sys_pllclk()*3/4;
#else
	u32 regval, fs;

	regval = sl_readl(SILAN_SYS_REG48);	
	fs = SILAN_CLK_ISP_GET_FS(regval);
	switch(fs)
	{
		case 0:
			return get_silan_video_pllclk()/6;
		case 1:
			return get_silan_sys_pllclk()/4;
		case 2:
			return get_silan_sys_pllclk()/6;
		default:
			break;
	}
	printk("ERROR: %s: Invalid reg value of fs (%d)\n", __FUNCTION__, fs);
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}

int clk_set_isp_rate(struct clk *clk, unsigned long rate)
{
#if defined(CONFIG_SILAN_FPGA)
	return 0;
#else
	u32 regval, fs;
	unsigned int i;
	unsigned long pllclk[3], dist, min_dist, parent_clk;

	parent_clk = get_silan_video_pllclk();
	pllclk[0] = parent_clk / 6;
	parent_clk = get_silan_sys_pllclk();
	pllclk[1] = parent_clk / 4;
	pllclk[2] = parent_clk / 6;
	
	min_dist = (pllclk[0] > rate) ? (pllclk[0] - rate) : (rate - pllclk[0]);
	fs = 0;
	for(i = 1; i < 3 ;i++)
	{
		dist = (pllclk[i] > rate) ? (pllclk[i] - rate) : (rate - pllclk[i]);
		if( dist < min_dist)
		{
			min_dist = dist;
			fs = i;
		}
	}
#ifdef SILAN_CLK_DEBUG
	printk("%s: rate %ld, fs %d\n", __FUNCTION__, rate, fs);
#endif

	spin_lock(&clocks_lock);
	regval = sl_readl(SILAN_SYS_REG48);	
	regval &= (~SILAN_CLK_ISP_MASK_FS);
	regval |= SILAN_CLK_ISP_SET_FS(fs);
	sl_writel(regval, SILAN_SYS_REG48);
	spin_unlock(&clocks_lock);
	
	return 0;
#endif /* CONFIG_SILAN_FPGA */
}
int clk_enable(struct clk *clk)
{
	int reg_offset, bit_offset, i;
	u32 reg_value = 0;

	spin_lock(&clocks_lock);
	if (SILAN_CLK_WATCHDOG == clk->clk_id[0]) {
		reg_value = sl_readl(SILAN_LSP_CTRL);
		reg_value |= (0x1 << 0);
		sl_writel(reg_value, SILAN_LSP_CTRL);
	} else if (SILAN_CLK_TIMER == clk->clk_id[0]) {
#if 0 // have a hardware bug
		reg_value = sl_readl(SILAN_LSPMISC_BASE);
		reg_value |= (0x3 << 1);
		sl_writel(reg_value, SILAN_LSPMISC_BASE);
#endif
		reg_offset = clk->clk_id[0] / 32; 
		bit_offset = clk->clk_id[0] % 32;
		reg_value = sl_readl(SILAN_SYS_REG0 + (4 * reg_offset));
		reg_value |= (0x1 << bit_offset);
		sl_writel(reg_value, SILAN_SYS_REG0 + (4 * reg_offset));
	} else {
		for(i = 0; i < CLK_ID_NUM ; i++) {
			if (clk->clk_id[i] == 0)
				break;
			reg_offset = clk->clk_id[i] / 32; 
			bit_offset = clk->clk_id[i] % 32;
			reg_value = sl_readl(SILAN_SYS_REG0 + (4 * reg_offset));
			reg_value |= (0x1 << bit_offset);
			sl_writel(reg_value, SILAN_SYS_REG0 + (4 * reg_offset));
		}
		if (0 == i) {
			spin_unlock(&clocks_lock);
			printk("this device has no valid clock id\n");
			return -1;
		}
	}
	spin_unlock(&clocks_lock);
	return 0;
}
EXPORT_SYMBOL(clk_enable);


void clk_disable(struct clk *clk)
{
	int reg_offset, bit_offset, i;
	u32 reg_value;

	spin_lock(&clocks_lock);

	if (SILAN_CLK_WATCHDOG == clk->clk_id[0]) {
		reg_value = sl_readl(SILAN_LSP_CTRL);
		reg_value &= ~(0x1 << 0);
		sl_writel(reg_value, SILAN_LSP_CTRL); 
	} else if (SILAN_CLK_TIMER == clk->clk_id[0]) {
#if 0 // have a hardware bug
		reg_value = sl_readl(SILAN_LSPMISC_BASE);
		reg_value &= ~(0x3 << 1);
		sl_writel(reg_value, SILAN_LSPMISC_BASE);
#endif
		reg_offset = clk->clk_id[0] / 32; 
		bit_offset = clk->clk_id[0] % 32;
		reg_value = sl_readl(SILAN_SYS_REG0 + (4 * reg_offset));
		reg_value &= ~(0x1 << bit_offset);
		sl_writel(reg_value, SILAN_SYS_REG0 + (4 * reg_offset));
	} else {
		for (i = 0; i < CLK_ID_NUM; i++) {
			if (clk->clk_id[i] == 0)
				continue;
			reg_offset = clk->clk_id[i] / 32; 
			bit_offset = clk->clk_id[i] % 32;
			reg_value = sl_readl(SILAN_SYS_REG0 + (4 * reg_offset));
			reg_value &= ~(0x1 << bit_offset);
			sl_writel(reg_value, SILAN_SYS_REG0 + (4 * reg_offset));
		}
		if (0 == i)
			printk("this device has no valid clock id\n");
	}
	spin_unlock(&clocks_lock);
}
EXPORT_SYMBOL(clk_disable);


unsigned long clk_get_rate(struct clk *clk)
{
	if (NULL == clk->get_rate) {
		printk("ERROR: %s: NULL 'get_rate' handler of device\n",
				__FUNCTION__);
		return 0;
	}

	return clk->get_rate(clk);
}
EXPORT_SYMBOL(clk_get_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	if (NULL == clk->set_rate) {
		printk("ERROR: %s: NULL 'get_rate' handler of device\n",
				__FUNCTION__);
		return -1;
	}

	return clk->set_rate(clk, rate);
}
EXPORT_SYMBOL(clk_set_rate);

void clk_put(struct clk *clk)
{
	printk("ERROR: %s: function is not implemented\n", __FUNCTION__);
}
EXPORT_SYMBOL(clk_put);
