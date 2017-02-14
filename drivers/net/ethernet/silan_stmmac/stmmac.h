/*******************************************************************************
  Copyright (C) 2007-2009  STMicroelectronics Ltd

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
*******************************************************************************/

#ifndef STMMAC_H
#define STMMAC_H

#define DRV_MODULE_VERSION	"Nov_2010"
#include <linux/platform_device.h>
#include "stmmac_in.h"

#include "common.h"
#ifdef CONFIG_STMMAC_TIMER
#include "stmmac_timer.h"
#endif
/* Clock ID */
enum silan_clk_ids {
	SILAN_CLK_START			= 0,

	/* CTR_REG0 - Module Clock Enable Register 0 */
	SILAN_CLK_PX			= 0 * 32 + 0,
	SILAN_CLK_CXB			= 0 * 32 + 1,
	SILAN_CLK_USB_OTG		= 0 * 32 + 2,
	SILAN_CLK_USB_HOST		= 0 * 32 + 3,	/* USB1 */
	SILAN_CLK_GMAC1			= 0 * 32 + 5,
	SILAN_CLK_LSP			= 0 * 32 + 6,
	SILAN_CLK_GMAC0			= 0 * 32 + 7,
	SILAN_CLK_DMAC0			= 0 * 32 + 8,
	SILAN_CLK_DMAC1			= 0 * 32 + 9,
	SILAN_CLK_SPDIF_OUT		= 0 * 32 + 10,
	SILAN_CLK_I2S_T4		= 0 * 32 + 11,//0
	SILAN_CLK_I2S_MIC		= 0 * 32 + 12,//
	SILAN_CLK_I2S_HDMI		= 0 * 32 + 13,
	SILAN_CLK_SDRAM			= 0 * 32 + 14,
	SILAN_CLK_DMABAR		= 0 * 32 + 15,
	SILAN_CLK_PXB			= 0 * 32 + 16,
	SILAN_CLK_GPU			= 0 * 32 + 17,
	SILAN_CLK_VPU0			= 0 * 32 + 18,
	SILAN_CLK_VPP			= 0 * 32 + 19,
	SILAN_CLK_JPU			= 0 * 32 + 20,
	SILAN_CLK_DSP0			= 0 * 32 + 21,
	SILAN_CLK_DSP1			= 0 * 32 + 22,
	SILAN_CLK_SECDMX		= 0 * 32 + 23,
	SILAN_CLK_TIMER			= 0 * 32 + 24,
	SILAN_CLK_UART0			= 0 * 32 + 25,
	SILAN_CLK_SPI			= 0 * 32 + 26,
	SILAN_CLK_GPIO1			= 0 * 32 + 27,
	SILAN_CLK_SSP			= 0 * 32 + 28,
	SILAN_CLK_GMAC_BUS		= 0 * 32 + 30,
	SILAN_CLK_DXB			= 0 * 32 + 31,

	/* CTR_REG1 - Module Clock Enable Register 1 */
	SILAN_CLK_AUDIO			= 1 * 32 + 0,
	SILAN_CLK_HDMI			= 1 * 32 + 1,
	SILAN_CLK_DVE			= 1 * 32 + 2,
	SILAN_CLK_TCON			= 1 * 32 + 3,
	SILAN_CLK_BT656			= 1 * 32 + 4,	/* Renamed from "BT656" */
	SILAN_CLK_HDDVE			= 1 * 32 + 5,
	SILAN_CLK_CAN			= 1 * 32 + 8,
	SILAN_CLK_SD			= 1 * 32 + 9,
	SILAN_CLK_EMMC			= 1 * 32 + 10,
	SILAN_CLK_SDIO			= 1 * 32 + 11,
	SILAN_CLK_SCI			= 1 * 32 + 12,
	SILAN_CLK_PWM			= 1 * 32 + 13,
	SILAN_CLK_RTC			= 1 * 32 + 14,
	SILAN_CLK_VPU1			= 1 * 32 + 16,
	SILAN_CLK_VJB			= 1 * 32 + 17,		/* Bus */
	SILAN_CLK_SPI_NORMAL		= 1 * 32 + 18,
	SILAN_CLK_HPB			= 1 * 32 + 19,
	//    SILAN_CLK_HPU_SPU_BUS	= 1 * 32 + 20,
	SILAN_CLK_VIU			= 1 * 32 + 21,
	//    SILAN_CLK_SPU			= 1 * 32 + 22,	/* CK810 */
	SILAN_CLK_DIT			= 1 * 32 + 23,
	SILAN_CLK_UART6			= 1 * 32 + 24,
	SILAN_CLK_UART1			= 1 * 32 + 25,
	SILAN_CLK_UART2			= 1 * 32 + 26,
	SILAN_CLK_UART3			= 1 * 32 + 27,
	SILAN_CLK_I2C0			= 1 * 32 + 28,
	SILAN_CLK_I2C1			= 1 * 32 + 29,
	SILAN_CLK_I2C2			= 1 * 32 + 30,
	SILAN_CLK_PMU			= 1 * 32 + 31,

	/* CTR_REG19 - Module Clock Enable Register 19 */
	SILAN_CLK_DDR0_PHY		= 19 * 32 + 14,
	SILAN_CLK_DDR0_CTRL		= 19 * 32 + 15,
	SILAN_CLK_DDR1_PHY		= 19 * 32 + 30,
	SILAN_CLK_DDR1_CTRL		= 19 * 32 + 31,

	/* CTR_REG22 - Module Clock Enable Register 22  */
	SILAN_CLK_USBPHY		= 22 * 32 + 8,

	/* CTR_REG28 - Module Clock Enable Register 28  */

	/* CTR_REG30 - Module Clock Enable Register 2  */
	SILAN_CLK_ADC			= 30 * 32 + 8,
	SILAN_CLK_ADC_PCLK		= 30 * 32 + 9,
	SILAN_CLK_CM0_HCLK		= 30 * 32 + 10,
	SILAN_CLK_CM0_DCLK		= 30 * 32 + 11,
	SILAN_CLK_CM0_FCLK		= 30 * 32 + 12,
	SILAN_CLK_EFUSE			= 30 * 32 + 13,
	SILAN_CLK_CCU			= 30 * 32 + 14,

	SILAN_CLK_FDIP			= 30 * 32 + 15,
	SILAN_CLK_GPIO2			= 30 * 32 + 16,
	SILAN_CLK_GPIO3			= 30 * 32 + 17,
	SILAN_CLK_GPIO4			= 30 * 32 + 18,
	SILAN_CLK_GPIO5			= 30 * 32 + 19,
	SILAN_CLK_UART4			= 30 * 32 + 20,
	SILAN_CLK_UART5			= 30 * 32 + 21,
	SILAN_CLK_I2C3			= 30 * 32 + 22,
	SILAN_CLK_I2C4			= 30 * 32 + 23,
	SILAN_CLK_IVS_BUS		= 30 * 32 + 24,
	SILAN_CLK_ISP			= 30 * 32 + 25,
	SILAN_CLK_MPU			= 30 * 32 + 26,
	SILAN_CLK_VIP			= 30 * 32 + 27,
	SILAN_CLK_VPREP			= 30 * 32 + 28,
	SILAN_CLK_VENC			= 30 * 32 + 29,
	SILAN_CLK_GPIO6			= 30 * 32 + 30,
	SILAN_CLK_SPDIF_IN		= 30 * 32 + 31,
	/* note: watchdog doesn't belong to sys ctl regs */
	SILAN_CLK_WATCHDOG		= 30 * 32 + 32,

	/* This should always be the last */
	SILAN_CLK_END,
};


struct silan_clk {
	enum silan_clk_ids clk_id[3];
	struct silan_clk *parent;
	int (*enable)(struct silan_clk *);
	void (*disable)(struct silan_clk *);
	unsigned long (*get_rate)(struct silan_clk *);
	int (*set_rate)(struct silan_clk *, unsigned long);
	/* some devices have multiple clock like core and bus*/
	unsigned long (*get_bus_rate)(struct silan_clk *);
	int (*set_bus_rate)(struct silan_clk *, unsigned long);
};

struct silan_clk *silan_clk_get(struct device *dev, const char *id);
int silan_clk_enable(struct silan_clk *clk);
void silan_clk_disable(struct silan_clk *clk);

struct stmmac_priv {
	/* Frequently used values are kept adjacent for cache effect */
	struct dma_desc *dma_tx ____cacheline_aligned;
	dma_addr_t dma_tx_phy;
	struct sk_buff **tx_skbuff;
	unsigned int cur_tx;
	unsigned int dirty_tx;
	unsigned int dma_tx_size;
	int tx_coalesce;
#ifdef CONFIG_SILAN_GMAC_TX_POSTCHK
    unsigned int napi_state;
#endif
	int platform_id;

	struct dma_desc *dma_rx ;
	unsigned int cur_rx;
	unsigned int dirty_rx;
	struct sk_buff **rx_skbuff;
	dma_addr_t *rx_skbuff_dma;
	struct sk_buff_head rx_recycle;

	struct net_device *dev;
	dma_addr_t dma_rx_phy;
	unsigned int dma_rx_size;
	unsigned int dma_buf_sz;
	struct device *device;
	struct mac_device_info *hw;
	void __iomem *ioaddr;

	struct stmmac_extra_stats xstats;
	struct napi_struct napi;

	phy_interface_t phy_interface;
	int phy_addr;
	int phy_mask;
	int (*phy_reset) (void *priv);
	int rx_coe;
	int no_csum_insertion;

	int phy_irq;
	struct phy_device *phydev;
	int oldlink;
	int speed;
	int oldduplex;
	unsigned int flow_ctrl;
	unsigned int pause;
	struct mii_bus *mii;

	u32 msg_enable;
	spinlock_t lock;
	int wolopts;
	int wolenabled;
#ifdef CONFIG_STMMAC_TIMER
	struct stmmac_timer *tm;
#endif
#ifdef STMMAC_VLAN_TAG_USED
	struct vlan_group *vlgrp;
#endif
	struct plat_stmmacenet_data *plat;
	struct silan_clk *clk;
};

extern int stmmac_mdio_unregister(struct net_device *ndev);
extern int stmmac_mdio_register(struct net_device *ndev);
extern void stmmac_set_ethtool_ops(struct net_device *netdev);
extern const struct stmmac_desc_ops enh_desc_ops;
extern const struct stmmac_desc_ops ndesc_ops;

#endif /* STMMAC_H */
