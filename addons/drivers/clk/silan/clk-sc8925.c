#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <asm/delay.h>

#define	ETH0_MODE_MASK		(0xf << 12)
#define	ETH1_MODE_MASK		(0xf << 16)

#define	ETH0_RGMII_100M		(0x5 << 12)
#define	ETH0_RGMII_1000M	(0x6 << 12)
#define	ETH0_RMII_100M		(0x9 << 12)
#define	ETH0_RMII_1000M		(0xa << 12)
#define	ETH0_RMII_100M		(0x9 << 12)
#define	ETH0_RMII_1000M		(0xa << 12)
#define ETH0_MII_GMII_100M	(0x1 << 12)
#define ETH0_MII_GMII_1000M	(0x2 << 12)

#define	ETH1_RGMII_100M		(0x5 << 16)
#define	ETH1_RGMII_1000M	(0x6 << 16)
#define	ETH1_RMII_100M		(0x9 << 16)
#define	ETH1_RMII_1000M		(0xa << 16)
#define	ETH1_RMII_100M		(0x9 << 16)
#define	ETH1_RMII_1000M		(0xa << 16)
#define ETH1_MII_GMII_100M	(0x1 << 16)
#define ETH1_MII_GMII_1000M	(0x2 << 16)



/* NOTE: SHOULD SYNCHRONIZED UPDATING WITH KERNEL */

/*BB MODULE*/
#define SILAN_GPU_BASE					0xBFA00000
#define SILAN_GPU_PHY_BASE				0x1FA00000
#define SILAN_GPU_SIZE					0xFFFF

#define SILAN_VPU1_BASE					0xBFA10000
#define SILAN_VPU1_PHY_BASE				0x1FA10000
#define SILAN_VPU1_SIZE					0xFFFF

#define SILAN_VPU0_BASE					0xBFA20000
#define SILAN_VPU0_PHY_BASE				0x1FA20000
#define SILAN_VPU0_SIZE					0xFFFF

#define SILAN_JPEG_BASE					0xBFA30000
#define SILAN_JPEG_PHY_BASE				0x1FA30000
#define SILAN_JPEG_SIZE					0xFFFF

#define SILAN_VPP_BASE					0xBFA40000
#define SILAN_VPP_PHY_BASE				0x1FA40000
#define SILAN_VPP_SIZE					0xFFFF

#define SILAN_HDMI_BASE					0xBFA50000
#define SILAN_HDMI_PHY_BASE				0x1FA50000
#define SILAN_HDMI_SIZE					0xFFFF

#define SILAN_DEINT_BASE				0xBFA60000
#define SILAN_DEINT_PHY_BASE			0x1FA60000
#define SILAN_DEINT_SIZE				0xFFFF

#define SILAN_VIU_BASE					0xBFA70000
#define SILAN_VIU_PHY_BASE				0x1FA70000
#define SILAN_VIU_SIZE					0x7FFF

#define SILAN_BBMISC_BASE				0xBFA78000
#define SILAN_BBMISC_PHY_BASE			0x1FA78000
#define SILAN_BBMISC_SIZE				0xFFF

#define SILAN_FDIP_BASE					0xBFA79000
#define SILAN_FDIP_PHY_BASE				0x1FA79000
#define SILAN_FDIP_SIZE					0x6FFF

/* HSP SubSystem */
#define SILAN_HOSTUSB_BASE				0xBFA80000
#define SILAN_HOSTUSB_PHY_BASE			0x1FA80000
#define SILAN_HOSTUSB_SIZE				0xFFFF

#define SILAN_PXBAR_BASE				0xBFA90000
#define SILAN_PXBAR_PHY_BASE			0x1FA90000
#define SILAN_PXBAR_SIZE				0xFFFF

#define SILAN_DMABAR_ARB_BASE			0xBFAA0000
#define SILAN_DMABAR_ARB_PHY_BASE       0x1FAA0000
#define SILAN_DMABAR_ARB_SIZE           0xFFF

#define SILAN_I2C5_BASE					0xBFAA3000
#define SILAN_I2C5_PHY_BASE             0x1FAA3000
#define SILAN_I2C5_SIZE                 0xFFF

#define SILAN_I2C4_BASE					0xBFAA4000
#define SILAN_I2C4_PHY_BASE             0x1FAA4000
#define SILAN_I2C4_SIZE                 0xFFF

#define SILAN_UART5_BASE				0xBFAA5000
#define SILAN_UART5_PHY_BASE			0x1FAA5000
#define SILAN_UART5_SIZE				0xFFF

#define SILAN_UART6_BASE				0xBFAA6000
#define SILAN_UART6_PHY_BASE			0x1FAA6000
#define SILAN_UART6_SIZE				0xFFF

#define SILAN_UART3_BASE				0xBFAA7000
#define SILAN_UART3_PHY_BASE			0x1FAA7000
#define SILAN_UART3_SIZE				0xFFF

#define SILAN_UART4_BASE				0xBFAA8000
#define SILAN_UART4_PHY_BASE			0x1FAA8000
#define SILAN_UART4_SIZE				0xFFF

#define SILAN_DMAC0_BASE				0xBFAB0000
#define SILAN_DMAC0_PHY_BASE			0x1FAB0000
#define SILAN_DMAC0_SIZE				0x7FFF

#define SILAN_OTGUSB_BASE				0xBFAC0000
#define SILAN_OTGUSB_PHY_BASE			0x1FAC0000
#define SILAN_OTGUSB_SIZE				0x7FFF

#define SILAN_GMAC1_BASE				0xBFAC8000
#define SILAN_GMAC1_PHY_BASE			0x1FAC8000
#define SILAN_GMAC1_SIZE				0x7FFF

#define SILAN_GMAC0_BASE				0xBFAD0000
#define SILAN_GMAC0_PHY_BASE			0x1FAD0000
#define SILAN_GMAC0_SIZE				0x7FFF

#define SILAN_GMAC_SRAM_BASE			0xBFAD8000
#define SILAN_GMAC_SRAM_PHY_BASE		0x1FAD8000
#define SILAN_GMAC_SRAM_SIZE			0x7FFF

#define SILAN_SD_BASE					0xBFAE0000
#define SILAN_SD_PHY_BASE				0x1FAE0000
#define SILAN_SD_SIZE					0xFFF

#define SILAN_MMC_BASE					0xBFAE1000
#define SILAN_MMC_PHY_BASE				0x1FAE1000
#define SILAN_MMC_SIZE					0xFFF

#define SILAN_SDIO_BASE					0xBFAE2000
#define SILAN_SDIO_PHY_BASE				0x1FAE2000
#define SILAN_SDIO_SIZE					0xFFF

#define SILAN_SSP_BASE					0xBFAE3000
#define SILAN_SSP_PHY_BASE				0x1FAE3000
#define SILAN_SSP_SIZE					0xFFF

#define SILAN_SPI_NORMAL_BASE			0xBFAE4000
#define SILAN_SPI_NORMAL_PHY_BASE		0x1FAE4000
#define SILAN_SPI_NORMAL_SIZE			0xFFF

#define SILAN_HSPMISC_BASE				0xBFAE5000
#define SILAN_HSPMISC_PHY_BASE			0x1FAE5000
#define SILAN_HSPMISC_SIZE				0xFFF

/* SDRAM SubSystem */
#define SILAN_SDRAM_BASE				0xBFAF0000
#define SILAN_SDRAM_PHY_BASE			0x1FAF0000
#define SILAN_SDRAM_SIZE				0xFFFF

/* IVS SubSystem */
#define SILAN_MPU_BASE					0xBFB00000
#define SILAN_MPU_PHY_BASE              0x1FB00000
#define SILAN_MPU_SIZE					0x1FFFF

#define SILAN_VENC_BASE					0xBFB20000
#define SILAN_VENC_PHY_BASE				0x1FB20000
#define SILAN_VENC_SIZE					0xFFFF

#define SILAN_VPRE_BASE					0xBFB30000
#define SILAN_VPRE_PHY_BASE				0x1FB30000
#define SILAN_VPRE_SIZE					0xFFFF

#define SILAN_DSP1_BASE					0xBFB40000
#define SILAN_DSP1_PHY_BASE				0x1FB40000
#define SILAN_DSP1_SIZE					0xFFFF

#define SILAN_ISP_BASE					0xBFB50000
#define SILAN_ISP_PHY_BASE				0x1FB50000
#define SILAN_ISP_SIZE					0xFFFF

#define SILAN_CCU_BASE					0xBFB60000
#define SILAN_CCU_PHY_BASE				0x1FB60000
#define SILAN_CCU_SIZE					0x7FFF

#define SILAN_IVS_MISC_BASE				0xBFB68000
#define SILAN_IVS_MISC_PHY_BASE			0x1FB68000
#define SILAN_IVS_MISC_SIZE				0x7FFF

#define SILAN_IVS_VDSP_BASE				0xBFB70000
#define SILAN_IVS_VDSP_PHY_BASE			0x1FB70000
#define SILAN_IVS_VDSP_SIZE				0xFFFF

/* MEM SubSystem */
#define SILAN_DDR_BASE					0xBFB90000
#define SILAN_DDR_PHY_BASE				0x1FB90000
#define SILAN_DDR_SIZE					0xFFF

#define SILAN_MEMMISC_BASE				0xBFB91000
#define SILAN_MEMMISC_PHY_BASE			0x1FB91000
#define SILAN_MEMMISC_SIZE				0xFFF

#define SILAN_DDR1_BASE					0xBFBE0000
#define SILAN_DDR1_PHY_BASE				0x1FBE0000
#define SILAN_DDR1_SIZE					0x3FFF

#define SILAN_MEMMISC1_BASE				0xBFBE4000
#define SILAN_MEMMISC1_PHY_BASE			0x1FBE4000
#define SILAN_MEMMISC1_SIZE				0x1FFF

/*MISC SubSystem*/
#define SILAN_UPT_BASE					0xBFBA0000
#define SILAN_UPT_PHY_BASE				0x1FBA0000
#define SILAN_UPT_SIZE					0xFFF

#define SILAN_SPT_BASE					0xBFBA1000
#define SILAN_SPT_PHY_BASE				0x1FBA1000
#define SILAN_SPT_SIZE					0xFFF

#define SILAN_ACODEC_BASE				0xBFBA2000
#define SILAN_ACODEC_PHY_BASE			0x1FBA2000
#define SILAN_ACODEC_SIZE				0xFFF

#define SILAN_VDAC_BASE					0xBFBA3000
#define SILAN_VDAC_PHY_BASE				0x1FBA3000
#define SILAN_VDAC_SIZE					0xFFF

#define SILAN_CR_BASE					0xBFBA9000
#define SILAN_CR_PHY_BASE				0x1FBA9000
#define SILAN_CR_SIZE					0xFFF

/*LSP MODULE*/
#define SILAN_SPIM_BASE					0xBFBB0000 /* SPI 4Bit MEM */
#define SILAN_SPIM_PHY_BASE				0x1FBB0000
#define SILAN_SPIM_SIZE					0xFFFF

#define SILAN_UART1_BASE				0xBFBC0000
#define SILAN_UART1_PHY_BASE			0x1FBC0000
#define SILAN_UART1_SIZE				0xFFF

#define SILAN_UART7_BASE				0xBFBC1000
#define SILAN_UART7_PHY_BASE			0x1FBC1000
#define SILAN_UART7_SIZE				0xFFF

#define SILAN_WDT_SUB_BASE				0xBFBC2000
#define SILAN_WDT_SUB_PHY_BASE			0x1FBC2000
#define SILAN_WDT_SUB_SIZE				0xFFF

#define SILAN_UART2_BASE				0xBFBC3000
#define SILAN_UART2_PHY_BASE			0x1FBC3000
#define SILAN_UART2_SIZE				0xFFF

#define SILAN_SPI_BASE					0xBFBC4000
#define SILAN_SPI_PHY_BASE				0x1FBC4000
#define SILAN_SPI_SIZE					0xFFF

#define SILAN_I2C1_BASE					0xBFBC5000
#define SILAN_I2C1_PHY_BASE				0x1FBC5000
#define SILAN_I2C1_SIZE					0xFFF

#define SILAN_WDG_BASE					0xBFBC6000
#define SILAN_WDG_PHY_BASE				0x1FBC6000
#define SILAN_WDG_SIZE					0xFFF

#define SILAN_LSP_TIMER_BASE			0xBFBC7000
#define SILAN_LSP_TIMER_PHY_BASE		0x1FBC7000
#define SILAN_LSP_TIMER_SIZE			0xFFF

#define SILAN_GPIO1_BASE				0xBFBC8000
#define SILAN_GPIO1_PHY_BASE			0x1FBC8000
#define SILAN_GPIO1_SIZE				0xFFF

#define SILAN_PWM_BASE					0xBFBC9000
#define SILAN_PWM_PHY_BASE				0x1FBC9000
#define SILAN_PWM_SIZE					0xFFF

#define SILAN_SCI_BASE					0xBFBCA000
#define SILAN_SCI_PHY_BASE				0x1FBCA000
#define SILAN_SCI_SIZE					0xFFF

#define SILAN_CAN_BASE					0xBFBCB000
#define SILAN_CAN_PHY_BASE				0x1FBCB000
#define SILAN_CAN_SIZE					0xFFF

#define SILAN_GPIO2_BASE				0xBFBCC000
#define SILAN_GPIO2_PHY_BASE			0x1FBCC000
#define SILAN_GPIO2_SIZE				0xFFF

#define SILAN_I2C2_BASE					0xBFBCD000
#define SILAN_I2C2_PHY_BASE				0x1FBCD000
#define SILAN_I2C2_SIZE					0xFFF

#define SILAN_GPIO3_BASE				0xBFBCE000
#define SILAN_GPIO3_PHY_BASE			0x1FBCE000
#define SILAN_GPIO3_SIZE				0xFFF

#define SILAN_GPIO4_BASE				0xBFBCF000
#define SILAN_GPIO4_PHY_BASE			0x1FBCF000
#define SILAN_GPIO4_SIZE				0xFFF

#define SILAN_GPIO5_BASE				0xBFBD0000
#define SILAN_GPIO5_PHY_BASE			0x1FBD0000
#define SILAN_GPIO5_SIZE				0xFFF

#define SILAN_ADC_BASE					0xBFBD1000
#define SILAN_ADC_PHY_BASE				0x1FBD1000
#define SILAN_ADC_SIZE					0xFFF

#define SILAN_EFUSE_BASE				0xBFBD3000
#define SILAN_EFUSE_PHY_BASE			0x1FBD3000
#define SILAN_EFUSE_SIZE				0xFFF

#define SILAN_RTC_BASE					0xBFBD4000
#define SILAN_RTC_PHY_BASE				0x1FBD4000
#define SILAN_RTC_SIZE					0xFFF

#define SILAN_I2C3_BASE					0xBFBD5000
#define SILAN_I2C3_PHY_BASE				0x1FBD5000
#define SILAN_I2C3_SIZE					0xFFF

#define SILAN_GPIO6_BASE				0xBFBD6000
#define SILAN_GPIO6_PHY_BASE			0x1FBD6000
#define SILAN_GPIO6_SIZE				0xFFF

#define SILAN_LSPMISC_BASE				0xBFBD7000
#define SILAN_LSPMIC_PHY_BASE			0x1FBD7000
#define SILAN_LSPMIC_SIZE				0x8FFF

/* SEC SubSystem*/
#define SILAN_OTP_BASE                  0xBFBE8000
#define SILAN_OTP_PHY_BASE              0x1FBE8000
#define SILAN_OTP_SIZE                  0xFFF

#define SILAN_AR_BASE					0xBFBE9000
#define SILAN_AR_PHY_BASE				0x1FBE9000
#define SILAN_AR_SIZE					0xFFF

#define SILAN_PMU_BASE                  0xBFBEB000
#define SILAN_PMU_PHY_BASE              0x1FBEB000
#define SILAN_PMU_SIZE                  0xFFF

#define SILAN_IVS_CXC_BASE              0xBFBEC000
#define SILAN_IVS_CXC_PHY_BASE          0x1FBEC000
#define SILAN_IVS_CXC_SIZE              0xFFF

#define SILAN_IVS_ICTL_BASE             0xBFBED000
#define SILAN_IVS_ICTL_PHY_BASE         0x1FBED000
#define SILAN_IVS_ICTL_SIZE             0x1FFF

#define SILAN_BOOTRAM_BASE             0xBFC00000
#define SILAN_BOOTRAM_PHY_BASE         0x1FC00000
#define SILAN_BOOTRAM_SIZE             0x3FFF

/* Audio */
#define SILAN_CXC_BASE					0xBFC10000
#define SILAN_CXC_PHY_BASE				0x1FC10000
#define SILAN_CXC_SIZE					0xFFF

#define SILAN_DMAC1_BASE				0xBFC11000
#define SILAN_DMAC1_PHY_BASE			0x1FC11000
#define SILAN_DMAC1_SIZE				0xFFF

#define SILAN_I2ST4_BASE				0xBFC12000
#define SILAN_I2ST4_PHY_BASE			0x1FC12000
#define SILAN_I2ST4_SIZE				0xFFF

#define SILAN_I2SHDMI_BASE				0xBFC13000
#define SILAN_I2SHDMI_PHY_BASE			0x1FC13000
#define SILAN_I2SHDMI_SIZE				0xFFF

#define SILAN_I2SMIC_BASE				0xBFC14000
#define SILAN_I2SMIC_PHY_BASE			0x1FC14000
#define SILAN_I2SMIC_SIZE				0xFFF

#define SILAN_SPDIF_BASE				0xBFC15000
#define SILAN_SPDIF_PHY_BASE			0x1FC15000
#define SILAN_SPDIF_SIZE				0xFFF

#define SILAN_AUDIU_TIMER_BASE			0xBFC17000
#define SILAN_AUDIO_TIMER_PHY_BASE		0x1FC17000
#define SILAN_AUDIO_TIMER_SIZE			0xFFF

#define SILAN_DSP0_BASE					0xBFC18000
#define SILAN_DSP0_PHY_BASE				0x1FC18000
#define SILAN_DSP0_SIZE					0xFFF

#define SILAN_AUDIO_ICTL_BASE			0xBFC19000
#define SILAN_AUDIO_ICTL_PHY_BASE		0x1FC19000
#define SILAN_AUDIO_ICTL_SIZE			0xFFF

#define SILAN_SPDIF_IN_BASE				0xBFC1A000
#define SILAN_SPDIF_IN_PHY_BASE			0x1FC1A000
#define SILAN_SPDIF_IN_SIZE				0xFFF

/* SECDMX */
#define SILAN_SECDMX_DMAC_BASE			0xBFC20000
#define SILAN_SECDMX_DMAC_PHY_BASE		0x1FC20000
#define SILAN_SECDMX_DMAC_SIZE			0xFFFF

#define SILAN_DRM_BASE					0xBFC30000
#define SILAN_DRM_PHY_BASE				0x1FC30000
#define SILAN_DRM_SIZE					0xFFF

#define SILAN_BUF_CTRL_BASE				0xBFC36000
#define SILAN_BUF_CTRL_PHY_BASE			0x1FC36000
#define SILAN_BUF_CTRL_SIZE				0x1FFF

#define SILAN_TS_DMX_BASE				0xBFC38000
#define SILAN_TS_DMX_PHY_BASE			0x1FC38000
#define SILAN_TS_DMX_SIZE				0x7FFF

/* SPU (CK810) */
#define SILAN_ICTL_BASE					0xBFCB0000 /* SPU_ICTL */
#define SILAN_ICTL_PHY_BASE				0x1FCB0000
#define SILAN_ICTL_SIZE					0xFFF

#define SILAN_SPU_TIMER_BASE			0xBFCB1000
#define SILAN_SPU_TIMER_PHY_BASE		0x1FCB1000
#define SILAN_SPU_TIMER_SIZE			0xFFF

/* Cortex-M0 */
#define SILAN_CM0_CXC_BASE              0xBFBEA000
#define SILAN_CM0_CXC_PHY_BASE          0x1FBEA000
#define SILAN_CM0_CXC_SIZE              0xFFF


/* System  Control Register */
#define SILAN_SYS_REG0		SILAN_CR_BASE
#define SILAN_SYS_REG1		(SILAN_CR_BASE + 0x4)
#define SILAN_SYS_REG2          (SILAN_CR_BASE + 0x8)
#define SILAN_SYS_REG3          (SILAN_CR_BASE + 0xc)
#define SILAN_SYS_REG4          (SILAN_CR_BASE + 0x10)
#define SILAN_SYS_REG5          (SILAN_CR_BASE + 0x14)
#define SILAN_SYS_REG6          (SILAN_CR_BASE + 0x18)
#define SILAN_SYS_REG7          (SILAN_CR_BASE + 0x1c)
#define SILAN_SYS_REG8          (SILAN_CR_BASE + 0x20)
#define SILAN_SYS_REG9          (SILAN_CR_BASE + 0x24)
#define SILAN_SYS_REG10         (SILAN_CR_BASE + 0x28)
#define SILAN_SYS_REG11         (SILAN_CR_BASE + 0x2c)
#define SILAN_SYS_REG12         (SILAN_CR_BASE + 0x30)
#define SILAN_SYS_REG13         (SILAN_CR_BASE + 0x34)
#define SILAN_SYS_REG14         (SILAN_CR_BASE + 0x38)
#define SILAN_SYS_REG15         (SILAN_CR_BASE + 0x3c)
#define SILAN_SYS_REG16         (SILAN_CR_BASE + 0x40)
#define SILAN_SYS_REG17         (SILAN_CR_BASE + 0x44)
#define SILAN_SYS_REG18         (SILAN_CR_BASE + 0x48)
#define SILAN_SYS_REG19         (SILAN_CR_BASE + 0x4c)
#define SILAN_SYS_REG20         (SILAN_CR_BASE + 0x50)
#define SILAN_SYS_REG21         (SILAN_CR_BASE + 0x54)
#define SILAN_SYS_REG22         (SILAN_CR_BASE + 0x58)
#define SILAN_SYS_REG23         (SILAN_CR_BASE + 0x5c)
#define SILAN_SYS_REG24         (SILAN_CR_BASE + 0x60)
#define SILAN_SYS_REG25         (SILAN_CR_BASE + 0x64)
#define SILAN_SYS_REG26         (SILAN_CR_BASE + 0x68)
#define SILAN_SYS_REG27         (SILAN_CR_BASE + 0x6c)
#define SILAN_SYS_REG28         (SILAN_CR_BASE + 0x70)
#define SILAN_SYS_REG29         (SILAN_CR_BASE + 0x74)
#define SILAN_SYS_REG30         (SILAN_CR_BASE + 0x78)
#define SILAN_SYS_REG31         (SILAN_CR_BASE + 0x7c)
#define SILAN_SYS_REG32         (SILAN_CR_BASE + 0x80)
#define SILAN_SYS_REG33         (SILAN_CR_BASE + 0x84)
#define SILAN_SYS_REG34         (SILAN_CR_BASE + 0x88)
#define SILAN_SYS_REG35         (SILAN_CR_BASE + 0x8c)
#define SILAN_SYS_REG36         (SILAN_CR_BASE + 0x90)
#define SILAN_SYS_REG37         (SILAN_CR_BASE + 0x94)
#define SILAN_SYS_REG38         (SILAN_CR_BASE + 0x98)
#define SILAN_SYS_REG39         (SILAN_CR_BASE + 0x9c)
#define SILAN_SYS_REG40         (SILAN_CR_BASE + 0xa0)
#define SILAN_SYS_REG41         (SILAN_CR_BASE + 0xa4)
#define SILAN_SYS_REG42         (SILAN_CR_BASE + 0xa8)
#define SILAN_SYS_REG43         (SILAN_CR_BASE + 0xac)
#define SILAN_SYS_REG44         (SILAN_CR_BASE + 0xb0)
#define SILAN_SYS_REG45         (SILAN_CR_BASE + 0xb4)
#define SILAN_SYS_REG46         (SILAN_CR_BASE + 0xb8)
#define SILAN_SYS_REG47         (SILAN_CR_BASE + 0xbc)
#define SILAN_SYS_REG48         (SILAN_CR_BASE + 0xc0)

/*
 * LSP Misc
 */
#define SILAN_LSP_CTRL				SILAN_LSPMISC_BASE

/*
 * PMU
 */
/* PMU configure register */
#define SILAN_PMU_CFG				SILAN_PMU_BASE
/* PLL configure register low */
#define SILAN_PMU_PLL_CFG_L			(SILAN_PMU_BASE + 0x04)
/* PLL configure register high */
#define SILAN_PMU_PLL_CFG_H			(SILAN_PMU_BASE + 0x08)
/* PMU external interrupt mask register */
#define SILAN_PMU_INT_MASK_CFG		(SILAN_PMU_BASE + 0x0c)
/* PMU external interrupt polarity register */
#define SILAN_PMU_INT_POLAR_CFG		(SILAN_PMU_BASE + 0x10)
/* PMU external interrupt configuration register */
#define SILAN_PMU_INT_CFG			(SILAN_PMU_BASE + 0x14)
/* PMU interrupt status register */
#define SILAN_PMU_INT_STATUS		(SILAN_PMU_BASE + 0x18)
/* PMU external interrupt level count register */
#define SILAN_PMU_INT_LEVEL_CNT_CFG	(SILAN_PMU_BASE + 0x1c)
/* PMU status register */
#define SILAN_PMU_STATUS			(SILAN_PMU_BASE + 0x20)
/* PLL status register low */
#define SILAN_PMU_PLL_STATUS_L		(SILAN_PMU_BASE + 0x24)
/* PLL status register high */
#define SILAN_PMU_PLL_STATUS_H		(SILAN_PMU_BASE + 0x28)
/* PMU state change wait count register */
#define SILAN_PMU_WAIT_CNT			(SILAN_PMU_BASE + 0x2c)

/*
 * SEC_AR
 */
#define SILAN_MEM0_REGION0_START	(SILAN_AR_BASE + 0x18)
#define SILAN_MEM0_REGION0_END		(SILAN_AR_BASE + 0x1c)

#define SILAN_MEM0_REGION1_START	(SILAN_AR_BASE + 0x20)
#define SILAN_MEM0_REGION1_END		(SILAN_AR_BASE + 0x24)

#define SILAN_MEM1_REGION0_START	(SILAN_AR_BASE + 0x30)
#define SILAN_MEM1_REGION0_END		(SILAN_AR_BASE + 0x34)

#define SILAN_MEM1_REGION1_START	(SILAN_AR_BASE + 0x38)
#define SILAN_MEM1_REGION1_END		(SILAN_AR_BASE + 0x3c)

#define SILAN_AR_STATUS				(SILAN_AR_BASE + 0x40)
#define SILAN_AR_PHY_STATUS			(SILAN_AR_PHY_BASE + 0x40)

#define SILAN_HSPMISC_ENDIAN		(SILAN_HSPMISC_BASE + 0x4)
#define SILAN_HSPMISC_PHY_ENDIAN	(SILAN_HSPMISC_PHY_BASE + 0x4)

/*
 * audio subsystem
 */

#define SILAN_I2S_BIND_HDMI_SELECT	(SILAN_DSP0_BASE + 0x80)



#define CLK_ID_NUM	3
/* CTR_REG2 - Module Clock Select Register 0 */
#define SILAN_CLK_DXB_MASK_FS	((0xf)<<0)
#define SILAN_CLK_DXB_GET_FS(x)	(((x)>>0)&0xf)
#define SILAN_CLK_DXB_SET_FS(x)	((x)<<0)

#define SILAN_CLK_PXB_MASK_FS	((0xf)<<4)
#define SILAN_CLK_PXB_GET_FS(x)	(((x)>>4)&0xf)
#define SILAN_CLK_PXB_SET_FS(x)	((x)<<4)

#define SILAN_CLK_VJB_MASK_FS	((0xf)<<8)
#define SILAN_CLK_VJB_GET_FS(x)	(((x)>>8)&0xf)
#define SILAN_CLK_VJB_SET_FS(x)	((x)<<8)

#define SILAN_CLK_LSP_MASK_FS	((0xf)<<12)
#define SILAN_CLK_LSP_GET_FS(x)	(((x)>>12)&0xf)
#define SILAN_CLK_LSP_SET_FS(x)	((x)<<12)

#define SILAN_CLK_VPU_MASK_FS	((0xf)<<16)
#define SILAN_CLK_VPU_GET_FS(x)	(((x)>>16)&0xf)
#define SILAN_CLK_VPU_SET_FS(x)	((x)<<16)

#define SILAN_CLK_VPU1_MASK_FS	((0xf)<<0)
#define SILAN_CLK_VPU1_GET_FS(x)	(((x)>>0)&0xf)
#define SILAN_CLK_VPU1_SET_FS(x)	((x)<<0)


#define SILAN_CLK_GPU_MASK_FS	((0xf)<<20)
#define SILAN_CLK_GPU_GET_FS(x)	(((x)>>20)&0xf)
#define SILAN_CLK_GPU_SET_FS(x)	((x)<<20)

#define SILAN_CLK_JPU_MASK_FS	((0xf)<<24)
#define SILAN_CLK_JPU_GET_FS(x)	(((x)>>24)&0xf)
#define SILAN_CLK_JPU_SET_FS(x)	((x)<<24)

#define SILAN_CLK_VPP_MASK_FS	((0xf)<<28)
#define SILAN_CLK_VPP_GET_FS(x)	(((x)>>28)&0xf)
#define SILAN_CLK_VPP_SET_FS(x)	((x)<<28)

/* CTR_REG3 - Module Clock Select Register 1 */
#define SILAN_CLK_DSP0_MASK_FS		((0xf)<<0)
#define SILAN_CLK_DSP0_GET_FS(x)	(((x)>>0)&0xf)
#define SILAN_CLK_DSP0_SET_FS(x)	((x)<<0)

#define SILAN_CLK_DSP1_MASK_FS		((0xf)<<4)
#define SILAN_CLK_DSP1_GET_FS(x)	(((x)>>4)&0xf)
#define SILAN_CLK_DSP1_SET_FS(x)	((x)<<4)

#define SILAN_CLK_SD_MASK_FS		((0xf)<<8)
#define SILAN_CLK_SD_GET_FS(x)		(((x)>>8)&0xf)
#define SILAN_CLK_SD_SET_FS(x)		((x)<<8)

#define SILAN_CLK_EMMC_MASK_FS		((0xf)<<12)
#define SILAN_CLK_EMMC_GET_FS(x)	(((x)>>12)&0xf)
#define SILAN_CLK_EMMC_SET_FS(x)	((x)<<12)

#define SILAN_CLK_SDIO_MASK_FS		((0xf)<<16)
#define SILAN_CLK_SDIO_GET_FS(x)	(((x)>>16)&0xf)
#define SILAN_CLK_SDIO_SET_FS(x)	((x)<<16)

#define SILAN_CLK_VIU_MASK_FS		((0xf)<<20)
#define SILAN_CLK_VIU_GET_FS(x)		(((x)>>20)&0xf)
#define SILAN_CLK_VIU_SET_FS(x)		((x)<<20)

#define SILAN_CLK_DIT_MASK_FS		((0xf)<<24)
#define SILAN_CLK_DIT_GET_FS(x)		(((x)>>24)&0xf)
#define SILAN_CLK_DIT_SET_FS(x)		((x)<<24)

#define SILAN_CLK_PMU_MASK_FS		((0x3)<<28)
#define SILAN_CLK_PMU_GET_FS(x)		(((x)>>28)&0x3)
#define SILAN_CLK_PMU_SET_FS(x)		((x)<<28)

#define SILAN_CLK_SDRAM_MASK_FS		((0x3)<<30)
#define SILAN_CLK_SDRAM_GET_FS(x)	(((x)>>30)&0x3)
#define SILAN_CLK_SDRAM_SET_FS(x)	((x)<<30)

#define SILAN_CLK_IVS_MASK_FS		((0xf)<<8)
#define SILAN_CLK_IVS_GET_FS(x)	(((x)>>8)&0xf)
#define SILAN_CLK_IVS_SET_FS(x)	((x)<<8)

#define SILAN_CLK_CCU_MASK_FS		((0xf)<<12)
#define SILAN_CLK_CCU_GET_FS(x)	    (((x)>>12)&0xf)
#define SILAN_CLK_CCU_SET_FS(x)	    ((x)<<12)

#define SILAN_CLK_HPB_MASK_FS		((0xf)<<0)
#define SILAN_CLK_HPB_GET_FS(x)	    (((x)>>0)&0xf)
#define SILAN_CLK_HPB_SET_FS(x)	    ((x)<<0)

#define SILAN_CLK_ISP_MASK_FS		((0xf)<<4)
#define SILAN_CLK_ISP_GET_FS(x)	    (((x)>>4)&0xf)
#define SILAN_CLK_ISP_SET_FS(x)	    ((x)<<4)

/* CTR_REG4 - Module Clock Select Register 2 (Audio) */
#define SILAN_CLK_AM_MASK_FS		((0x1)<<0)
#define SILAN_CLK_AM_GET_FS(x)		(((x)>>0)&0x1)
#define SILAN_CLK_AM_SET_FS(x)		((x)<<0)

#define SILAN_CLK_SPDIF_MASK_FS		((0x3)<<2)
#define SILAN_CLK_SPDIF_GET_FS(x)	(((x)>>2)&0x3)
#define SILAN_CLK_SPDIF_SET_FS(x)	((x)<<2)

#define SILAN_CLK_I2SHDMI_MASK_FS	((0x7)<<4)
#define SILAN_CLK_I2SHDMI_GET_FS(x)	(((x)>>4)&0x7)
#define SILAN_CLK_I2SHDMI_SET_FS(x)	((x)<<4)

#define SILAN_CLK_I2SMIC_MASK_FS	((0x7)<<7)
#define SILAN_CLK_I2SMIC_GET_FS(x)	(((x)>>7)&0x7)
#define SILAN_CLK_I2SMIC_SET_FS(x)	((x)<<7)

#define SILAN_CLK_I2ST4_MASK_FS		((0x7)<<12)
#define SILAN_CLK_I2ST4_GET_FS(x)	(((x)>>12)&0x7)
#define SILAN_CLK_I2ST4_SET_FS(x)	((x)<<12)

#define SILAN_CLK_HS_SEL			(1<<20)
#define SILAN_CLK_HS_EN				(1<<21)
#define SILAN_CLK_SPDIF_EN			(1<<22)
#define SILAN_CLK_I2SHDMI_FS_EN		(1<<23)
#define SILAN_CLK_I2SHDMI_BIT_EN	(1<<24)
#define SILAN_CLK_I2SHDMI_M_EN		(1<<25)
#define SILAN_CLK_I2SMIC_M_EN		(1<<26)
#define SILAN_CLK_I2ST4_FS_EN		(1<<29)
#define SILAN_CLK_I2ST4_BIT_EN		(1<<30)
#define SILAN_CLK_I2ST4_M_EN		(1<<31)

/* CTR_REG5 - Audio PLL Configure Register 0 */
#define SILAN_CLK_APLL_PD				(1<<0)
#define SILAN_CLK_APLL_BP				(1<<2)
#define SILAN_CLK_APLL_FOUTPOSTDIVPD	(1<<4)
#define SILAN_CLK_APLL_FOUT4PHASEPD		(1<<5)
#define SILAN_CLK_APLL_FOUTVCOPD		(1<<6)
#define SILAN_CLK_APLL_TESTOUTEN		(1<<7)
#define SILAN_CLK_APLL_FRAC_MASK		(0xffffff<<8)
#define SILAN_CLK_APLL_FRAC_GET(x)		(((x)>>8)&0xffffff)
#define SILAN_CLK_APLL_FRAC_SET(x)		((x)<<8)

/* CTR_REG6 - Audio PLL Configure Register 1 */
/* Frclk = 27MHZ, Fref = Frclk/5, Fout=Fref*NF/NR/OD */
#define SILAN_CLK_APLL_CLKR_MASK		(0x3f<<0)
#define SILAN_CLK_APLL_CLKR_GET(x)		(((x)>>0)&0x3f)
#define SILAN_CLK_APLL_CLKR_SET(x)		((x)<<0)

#define SILAN_CLK_APLL_CLKF_MASK		(0xfff<<6)
#define SILAN_CLK_APLL_CLKF_GET(x)		(((x)>>6)&0xfff)
#define SILAN_CLK_APLL_CLKF_SET(x)		((x)<<6)

#define SILAN_CLK_APLL_CLKOD1_MASK		(0x7<<19)
#define SILAN_CLK_APLL_CLKOD1_GET(x)	(((x)>>19)&0x7)
#define SILAN_CLK_APLL_CLKOD1_SET(x)	((x)<<19)

#define SILAN_CLK_APLL_CLKOD2_MASK		(0x7<<22)
#define SILAN_CLK_APLL_CLKOD2_GET(x)	(((x)>>22)&0x7)
#define SILAN_CLK_APLL_CLKOD2_SET(x)	((x)<<22)

#define SILAN_CLK_APLL_DSMPD_MASK		(0x1<<25)
#define SILAN_CLK_APLL_DSMPD_GET(x)		(((x)>>25)&0x1)
#define SILAN_CLK_APLL_DSMPD_SET(x)		((x)<<25)

#define SILAN_CLK_APLL_CS_MASK			(0x3<<28)
#define SILAN_CLK_APLL_CS_GET(x)		(((x)>>28)&0x3)
#define SILAN_CLK_APLL_CS_SET(x)		((x)<<28)


/* CTR_REG7 - Video PLL Configure Register 0 */
#define SILAN_CLK_VPLL_PD				(0x1<<0)
#define SILAN_CLK_VPLL_BP				(0x1<<2)
#define SILAN_CLK_VPLL_FOUTPOSTDIVPD	(0x1<<4)
#define SILAN_CLK_VPLL_FOUT4PHASEPD		(0x1<<5)
#define SILAN_CLK_VPLL_FOUTVCOPD		(0x1<<6)
#define SILAN_CLK_VPLL_TESTOUTEN		(0x1<<7)
#define SILAN_CLK_VPLL_FRAC_MASK		(0xffffff<<8)
#define SILAN_CLK_VPLL_FRAC_GET(x)		(((x)>>8)&0xffffff)
#define SILAN_CLK_VPLL_FRAC_SET(x)		((x)<<8)

/* CTR_REG8 - Video PLL Configure Register 1 */
/* Fosc = 27MHZ, Fref = Fosc, Fout = Fref*NF/NR/OD */
#define SILAN_CLK_VPLL_CLKR_MASK		(0x3f<<0)
#define SILAN_CLK_VPLL_CLKR_GET(x)		(((x)>>0)&0x3f)
#define SILAN_CLK_VPLL_CLKR_SET(x)		((x)<<0)

#define SILAN_CLK_VPLL_CLKF_MASK		(0xfff<<6)
#define SILAN_CLK_VPLL_CLKF_GET(x)		(((x)>>6)&0xfff)
#define SILAN_CLK_VPLL_CLKF_SET(x)		((x)<<6)

#define SILAN_CLK_VPLL_CLKOD1_MASK		(0x7<<19)
#define SILAN_CLK_VPLL_CLKOD1_GET(x)	(((x)>>19)&0x7)
#define SILAN_CLK_VPLL_CLKOD1_SET(x)	((x)<<19)

#define SILAN_CLK_VPLL_CLKOD2_MASK		(0x7<<22)
#define SILAN_CLK_VPLL_CLKOD2_GET(x)	(((x)>>22)&0x7)
#define SILAN_CLK_VPLL_CLKOD2_SET(x)	((x)<<22)

#define SILAN_CLK_VPLL_DSMPD_MASK		(0x1<<25)
#define SILAN_CLK_VPLL_DSMPD_GET(x)		(((x)>>25)&0x1)
#define SILAN_CLK_VPLL_DSMPD_SET(x)		((x)<<25)

#define SILAN_CLK_VPLL_CS_MASK			(0x3<<28)
#define SILAN_CLK_VPLL_CS_GET(x)		(((x)>>28)&0x3)
#define SILAN_CLK_VPLL_CS_SET(x)		((x)<<28)

#define SILAN_CLK_CPLL_TESTOUTEN		(0x1<31)
/* CTR_REG9 - 1st DDR PLL Configure Register 0 */
#define SILAN_CLK_PPLL_PD				(0x1<<0)
#define SILAN_CLK_PPLL_BP				(0x1<<2)
#define SILAN_CLK_PPLL_FOUTPOSTDIVPD	(0x1<<4)
#define SILAN_CLK_PPLL_FOUT4PHASEPD		(0x1<<5)
#define SILAN_CLK_PPLL_FOUTVCOPD		(0x1<<6)
#define SILAN_CLK_PPLL_TESTOUTEN		(0x1<<7)
#define SILAN_CLK_PPLL_FRAC_MASK		(0xffffff<<8)
#define SILAN_CLK_PPLL_FRAC_GET(x)		(((x)>>8)&0xffffff)
#define SILAN_CLK_PPLL_FRAC_SET(x)		((x)<<8)

/* CTR_REG10 - 1st DDR PLL Configure Register 1 */
/* Fosc = 27MHZ, Fref = Fosc, Fout = Fref*NF/NR/OD */
#define SILAN_CLK_PPLL_CLKR_MASK		(0x3f<<0)
#define SILAN_CLK_PPLL_CLKR_GET(x)		(((x)>>0)&0x3f)
#define SILAN_CLK_PPLL_CLKR_SET(x)		((x)<<0)

#define SILAN_CLK_PPLL_CLKF_MASK		(0xfff<<6)
#define SILAN_CLK_PPLL_CLKF_GET(x)		(((x)>>6)&0xfff)
#define SILAN_CLK_PPLL_CLKF_SET(x)		((x)<<6)

#define SILAN_CLK_PPLL_CLKOD1_MASK		(0x7<<19)
#define SILAN_CLK_PPLL_CLKOD1_GET(x)	(((x)>>19)&0x7)
#define SILAN_CLK_PPLL_CLKOD1_SET(x)	((x)<<19)

#define SILAN_CLK_PPLL_CLKOD2_MASK		(0x7<<22)
#define SILAN_CLK_PPLL_CLKOD2_GET(x)	(((x)>>22)&0x7)
#define SILAN_CLK_PPLL_CLKOD2_SET(x)	((x)<<22)

#define SILAN_CLK_PPLL_DSMPD_MASK		(0x1<<25)
#define SILAN_CLK_PPLL_DSMPD_GET(x)		(((x)>>25)&0x1)
#define SILAN_CLK_PPLL_DSMPD_SET(x)		((x)<<25)

#define SILAN_CLK_PPLL_CS_MASK			(0x7<<28)
#define SILAN_CLK_PPLL_CS_GET(x)		(((x)>>28)&0x7)
#define SILAN_CLK_PPLL_CS_SET(x)		((x)<<28)

/* CTR_REG11 - Module Clock Select Register 2 */
#define SILAN_CLK_LCD_MASK_FS				((0x3f)<<0)
#define SILAN_CLK_LCD_GET_FS(x)				(((x)>>0)&0x3f)
#define SILAN_CLK_LCD_SET_FS(x)				((x)<<0)

#define SILAN_CLK_SECDMX_MASK_FS			((0xf)<<8)
#define SILAN_CLK_SECDMX_GET_FS(x)			(((x)>>8)&0xf)
#define SILAN_CLK_SECDMX_SET_FS(x)			((x)<<8)

#define SILAN_CLK_CX_MASK_FS				((0xf)<<0)
#define SILAN_CLK_CX_GET_FS(x)				(((x)>>0)&0xf)
#define SILAN_CLK_CX_SET_FS(x)				((x)<<0)

#define SILAN_CLK_SPU_MASK_FS				((0xf)<<20)
#define SILAN_CLK_SPU_GET_FS(x)				(((x)>>20)&0xf)
#define SILAN_CLK_SPU_SET_FS(x)				((x)<<20)

#define SILAN_CLK_HPU_SPU_BUS_MASK_FS		((0xf)<<24)
#define SILAN_CLK_HPU_SPU_BUS_GET_FS(x)		(((x)>>24)&0xf)
#define SILAN_CLK_HPU_SPU_BUS_SET_FS(x)		((x)<<24)

#define SILAN_CLK_GPU_BUS_MASK_FS			((0xf)<<28)
#define SILAN_CLK_GPU_BUS_GET_FS(x)			(((x)>>28)&0xf)
#define SILAN_CLK_GPU_BUS_SET_FS(x)			((x)<<28)


/* CTR_REG31 - New Register */
#define SILAN_CLK_SDIO_SAMPLE_CFG_MASK		(0x1f<<0)
#define SILAN_CLK_SDIO_SAMPLE_CFG_GET(x)	(((x)>>0)&0x1f)
#define SILAN_CLK_SDIO_SAMPLE_CFG_SET(x)	((x)<<0)

#define SILAN_CLK_SDRAM_DEL_CFG_MASK		(0xffff<<16)
#define SILAN_CLK_SDRAM_DEL_CFG_GET(x)		(((x)>>16)&0xffff)
#define SILAN_CLK_SDRAM_DEL_CFG_SET(x)		((x)<<16)
/* CTR_REG32 - System PLL Configure Register 0 */
#define SILAN_CLK_SPLL_PD				(0x1<<0)
#define SILAN_CLK_SPLL_BP				(0x1<<2)
#define SILAN_CLK_SPLL_FOUTPOSTDIVPD	(0x1<<4)
#define SILAN_CLK_SPLL_FOUT4PHASEPD		(0x1<<5)
#define SILAN_CLK_SPLL_FOUTVCOPD		(0x1<<6)
#define SILAN_CLK_SPLL_TESTOUTEN		(0x1<<7)
#define SILAN_CLK_SPLL_FRAC_MASK		(0xffffff<<8)
#define SILAN_CLK_SPLL_FRAC_GET(x)		(((x)>>8)&0xffffff)
#define SILAN_CLK_SPLL_FRAC_SET(x)		((x)<<8)

/* CTR_REG33 - System PLL Configure Register 1 */
/* Fosc = 27MHZ, Fref = Fosc, Fout = Fref*NF/NR/OD */
#define SILAN_CLK_SPLL_CLKR_MASK		(0x3f<<0)
#define SILAN_CLK_SPLL_CLKR_GET(x)		(((x)>>0)&0x3f)
#define SILAN_CLK_SPLL_CLKR_SET(x)		((x)<<0)

#define SILAN_CLK_SPLL_CLKF_MASK		(0xfff<<6)
#define SILAN_CLK_SPLL_CLKF_GET(x)		(((x)>>6)&0xfff)
#define SILAN_CLK_SPLL_CLKF_SET(x)		((x)<<6)

#define SILAN_CLK_SPLL_CLKOD1_MASK		(0x7<<19)
#define SILAN_CLK_SPLL_CLKOD1_GET(x)	(((x)>>19)&0x7)
#define SILAN_CLK_SPLL_CLKOD1_SET(x)	((x)<<19)

#define SILAN_CLK_SPLL_CLKOD2_MASK		(0x7<<22)
#define SILAN_CLK_SPLL_CLKOD2_GET(x)	(((x)>>22)&0x7)
#define SILAN_CLK_SPLL_CLKOD2_SET(x)	((x)<<22)

#define SILAN_CLK_SPLL_DSMPD_MASK		(0x1<<25)
#define SILAN_CLK_SPLL_DSMPD_GET(x)		(((x)>>25)&0x1)
#define SILAN_CLK_SPLL_DSMPD_SET(x)		((x)<<25)

#define SILAN_CLK_SPLL_CS_MASK			(0x7<<28)
#define SILAN_CLK_SPLL_CS_GET(x)		(((x)>>28)&0x7)
#define SILAN_CLK_SPLL_CS_SET(x)		((x)<<28)

#define ENCODE_SYS_REG33(clkr, clkf, clkod1, clkod2, dsmpd, cs )	\
(								\
	 ((clkr) & 0x3F) |			\
	 (((clkf) & 0xFFF) << 6)  |	\
	 (((clkod1) & 0x7) << 19) |	\
	 (((clkod2) & 0x7) << 22) |	\
	 (((dsmpd) & 0x1) << 25)  |	\
	(((cs) & 0x7) << 28	)		\
)

/* CTR_REG42 - Module Clock Select  Register 3 */
#define SILAN_CLK_MPU_MASK_FS				((0xf)<<8)
#define SILAN_CLK_MPU_GET_FS(x)				(((x)>>8)&0xf)
#define SILAN_CLK_MPU_SET_FS(x)				((x)<<8)

#define SILAN_CLK_MPU_BUS_MASK_FS			((0xf)<<12)
#define SILAN_CLK_MPU_BUS_GET_FS(x)			(((x)>>12)&0xf)
#define SILAN_CLK_MPU_BUS_SET_FS(x)			((x)<<12)

#define SILAN_CLK_VIP_ACLK_MASK_FS		((0xf)<<16)
#define SILAN_CLK_VIP_ACLK_GET_FS(x)	(((x)>>16)&0xf)
#define SILAN_CLK_VIP_ACLK_SET_FS(x)	((x)<<16)

#define SILAN_CLK_VPREP_MASK_FS				((0xf)<<20)
#define SILAN_CLK_VPREP_GET_FS(x)			(((x)>>20)&0xf)
#define SILAN_CLK_VPREP_SET_FS(x)			((x)<<20)

#define SILAN_CLK_VENC_MASK_FS				((0xf)<<24)
#define SILAN_CLK_VENC_GET_FS(x)			(((x)>>24)&0xf)
#define SILAN_CLK_VENC_SET_FS(x)			((x)<<24)

#define SILAN_CLK_FDIP_MASK_FS				((0xf)<<28)
#define SILAN_CLK_FDIP_GET_FS(x)			(((x)>>28)&0xf)
#define SILAN_CLK_FDIP_SET_FS(x)			((x)<<28)
/* CTR_REG44 - 2nd DDR PLL Configure Register 0 */
#define SILAN_CLK_DPLL_PD				(0x1<<0)
#define SILAN_CLK_DPLL_BP				(0x1<<2)
#define SILAN_CLK_DPLL_FOUTPOSTDIVPD	(0x1<<4)
#define SILAN_CLK_DPLL_FOUT4PHASEPD		(0x1<<5)
#define SILAN_CLK_DPLL_FOUTVCOPD		(0x1<<6)
#define SILAN_CLK_DPLL_TESTOUTEN		(0x1<<7)
#define SILAN_CLK_DPLL_FRAC_MASK		(0xffffff<<8)
#define SILAN_CLK_DPLL_FRAC_GET(x)		(((x)>>8)&0xffffff)
#define SILAN_CLK_DPLL_FRAC_SET(x)		((x)<<8)

/* CTR_REG45 - 2nd DDR PLL Configure Register 1 */
#define SILAN_CLK_DPLL_CLKR_MASK		(0x3f<<0)
#define SILAN_CLK_DPLL_CLKR_GET(x)		(((x)>>0)&0x3f)
#define SILAN_CLK_DPLL_CLKR_SET(x)		((x)<<0)

#define SILAN_CLK_DPLL_CLKF_MASK		(0xfff<<6)
#define SILAN_CLK_DPLL_CLKF_GET(x)		(((x)>>6)&0xfff)
#define SILAN_CLK_DPLL_CLKF_SET(x)		((x)<<6)

#define SILAN_CLK_DPLL_CLKOD1_MASK		(0x7<<19)
#define SILAN_CLK_DPLL_CLKOD1_GET(x)	(((x)>>19)&0x7)
#define SILAN_CLK_DPLL_CLKOD1_SET(x)	((x)<<19)

#define SILAN_CLK_DPLL_CLKOD2_MASK		(0x7<<22)
#define SILAN_CLK_DPLL_CLKOD2_GET(x)	(((x)>>22)&0x7)
#define SILAN_CLK_DPLL_CLKOD2_SET(x)	((x)<<22)

#define SILAN_CLK_DPLL_DSMPD_MASK		(0x1<<25)
#define SILAN_CLK_DPLL_DSMPD_GET(x)		(((x)>>25)&0x1)
#define SILAN_CLK_DPLL_DSMPD_SET(x)		((x)<<25)

#define SILAN_CLK_DPLL_CS_MASK			(0x7<<28)
#define SILAN_CLK_DPLL_CS_GET(x)		(((x)>>28)&0x7)
#define SILAN_CLK_DPLL_CS_SET(x)		((x)<<28)

/* PMU_PLL_CFG_L - Core PLL Configure Register 0 */
#define SILAN_CLK_CPLL_PD				(0x1<<0)
#define SILAN_CLK_CPLL_BP				(0x1<<2)
#define SILAN_CLK_CPLL_FOUTPOSTDIVPD	(0x1<<4)
#define SILAN_CLK_CPLL_FOUT4PHASEPD		(0x1<<5)
#define SILAN_CLK_CPLL_FOUTVCOPD		(0x1<<6)
#define SILAN_CLK_CPLL_FRAC_MASK		(0xffffff<<8)
#define SILAN_CLK_CPLL_FRAC_GET(x)		(((x)>>8)&0xffffff)
#define SILAN_CLK_CPLL_FRAC_SET(x)		((x)<<8)

/* PMU_PLL_CFG_H - Core PLL Configure Register 1 */
/* Fosc = 27MHZ, Fref = Fosc, Fout = Fref*NF/NR/OD */
#define SILAN_CLK_CPLL_CLKR_MASK		(0x3f<<0)
#define SILAN_CLK_CPLL_CLKR_GET(x)		(((x)>>0)&0x3f)
#define SILAN_CLK_CPLL_CLKR_SET(x)		((x)<<0)

#define SILAN_CLK_CPLL_CLKF_MASK		(0xfff<<6)
#define SILAN_CLK_CPLL_CLKF_GET(x)		(((x)>>6)&0xfff)
#define SILAN_CLK_CPLL_CLKF_SET(x)		((x)<<6)

#define SILAN_CLK_CPLL_CLKOD1_MASK		(0x7<<19)
#define SILAN_CLK_CPLL_CLKOD1_GET(x)	(((x)>>19)&0x7)
#define SILAN_CLK_CPLL_CLKOD1_SET(x)	((x)<<19)

#define SILAN_CLK_CPLL_CLKOD2_MASK		((0x7)<<22)
#define SILAN_CLK_CPLL_CLKOD2_GET(x)	(((x)>>22)&0x7)
#define SILAN_CLK_CPLL_CLKOD2_SET(x)	((x)<<22)

#define SILAN_CLK_CPLL_DSMPD_MASK		(0x1<<25)
#define SILAN_CLK_CPLL_DSMPD_GET(x)		(((x)>>25)&0x1)
#define SILAN_CLK_CPLL_DSMPD_SET(x)		((x)<<25)

#define SILAN_CLK_CPLL_CS_MASK			(0xf<<28)
#define SILAN_CLK_CPLL_CS_GET(x)		(((x)>>28)&0xf)
#define SILAN_CLK_CPLL_CS_SET(x)		((x)<<28)

/* PMU_PLL_STATUS_H */
#define SILAN_CLK_CPLL_STATUS_GET_FS(x)	(((x)>>28)&0x7)

/* PMU_CFG */
#define SILAN_PMU_CFG_EXIT				(0x1<<0)
#define SILAN_PMU_CFG_REQ				(0x1<<1)
#define SILAN_PMU_CFG_BYPASS			(0x1<<2)

#define SILAN_PMU_CFG_MODE_MASK			(0x3<<3)
#define SILAN_PMU_CFG_MODE_GET(x)		(((x)>>3)&0x3)
#define SILAN_PMU_CFG_MODE_SET(x)		((x)<<3)

#define SILAN_PMU_CFG_CLK_SWITCH_MASK	(0xf<<8)
#define SILAN_PMU_CFG_CLK_SWITCH_GET(x)	(((x)>>8)&0xf)
#define SILAN_PMU_CFG_CLK_SWITCH_SET(x)	((x)<<8)

#define SILAN_PMU_CFG_CLK_ON			(0x1<<12)

/* SEC_AR_STATUS */
#define SILAN_SEC_AR_STATUS_CPU		(0x1<<20)
#define SILAN_SEC_AR_STATUS_AUDIO	(0x1<<21)
#define SILAN_SEC_AR_STATUS_SYS		(0x1<<22)
#define SILAN_SEC_AR_STATUS_PERP	(0x1<<23)
#define SILAN_SEC_AR_STATUS_VIDEO	(0x1<<24)

/* LSP MISC BIT DEFINE */
#define LSP_CTRL_WATCHDOG_ON	    (0x1 << 0)
#define LSP_CTRL_TIMCLK_ON0			(0x1 << 1)
#define LSP_CTRL_TIMCLK_ON1			(0x1 << 2)

/***************************************************************************
 * Configuration related to clock
***************************************************************************/

/* Frequency of external oscillator (Unit: HZ) */
#define EXTERNAL_OSC_FREQ			27000000

/*normal uart number*/
#define UART_NORMAL_NR		3

/*
 * Clock debug on or off
 * define it:  debug on
 * comment it: debuf off
 */
/*#define SILAN_CLK_DEBUG			1*/

/*
 * Use perfect PLL setting follow or not
 * 0: disable
 * 1: enable
 */
#define USE_PERFECT_PLL_SET_FLOW	0

/***************************************************************************
 * Utilities
***************************************************************************/

/*
 * Check if varaible 'mid' is more near to varaible 'big' than to 'smal'
 * Note: 'smal' should be smaller than or equal to 'big'
 */
#define IS_NEAR_BIG(smal, mid, big)	((((big) - (mid)) < ((mid) - (smal))) ? 1 : 0)

/* Get the min or the max value */
#ifndef MIN
#define MIN(x, y)       (((x) > (y))?(y):(x))
#endif

#ifndef MAX
#define MAX(x, y)       (((x) > (y))?(x):(y))
#endif

void static inline sl_writeb(u8 val, unsigned long reg)
{
    *(volatile u8 *)(reg) = val;
}

void static inline sl_writew(u16 val, unsigned long reg)
{
    *(volatile u16 *)(reg) = val;
}

void static inline sl_writel(u32 val, unsigned long reg)
{
    *(volatile u32 *)(reg) = val;
}

static inline u8 sl_readb(unsigned long reg)
{
    return (*(volatile u8 *)reg);
}

static inline u16 sl_readw(unsigned long reg)
{
    return (*(volatile u16 *)reg);
}

static inline u32 sl_readl(unsigned long reg)
{
    return (*(volatile u32 *)reg);
}

enum silan_device_ids {
	SILAN_DEV_START = 0,

	SILAN_DEV_USB_HOST = SILAN_DEV_START,
	SILAN_DEV_USB_OTG,
	SILAN_DEV_GMAC0,
	SILAN_DEV_GMAC1,
	SILAN_DEV_DMAC0,
	SILAN_DEV_DMAC1,
	SILAN_DEV_I2S_T4,
	SILAN_DEV_I2S_MIC,
	SILAN_DEV_I2S_HDMI,
	SILAN_DEV_UART0,
	SILAN_DEV_UART1,
	SILAN_DEV_UART2,
	SILAN_DEV_UART3,
	SILAN_DEV_UART4,
	SILAN_DEV_UART5,
	SILAN_DEV_UART6,
	SILAN_DEV_I2C0,
	SILAN_DEV_I2C1,
	SILAN_DEV_I2C2,
	SILAN_DEV_I2C3,
	SILAN_DEV_I2C4,
	SILAN_DEV_SPI,
	SILAN_DEV_SPI_NORMAL,
	SILAN_DEV_SSP,
	SILAN_DEV_SPDIF_IN,
	SILAN_DEV_SPDIF_OUT,
	SILAN_DEV_HDMI,
	SILAN_DEV_CAN,
	SILAN_DEV_SD,
	SILAN_DEV_EMMC,
	SILAN_DEV_SDIO,
	SILAN_DEV_SCI,
	SILAN_DEV_PWM,
	SILAN_DEV_SDRAM,
	SILAN_DEV_TIMER,
	SILAN_DEV_OTP,
	SILAN_DEV_RTC,
	SILAN_DEV_PMU,
	SILAN_DEV_DIT,
	SILAN_DEV_GPU,
	SILAN_DEV_GPU_BUS,
	SILAN_DEV_VPU0,
	SILAN_DEV_VPU1,
	SILAN_DEV_JPU,
	SILAN_DEV_VJB,
	SILAN_DEV_VPP,
	SILAN_DEV_DSP0,
	SILAN_DEV_DSP1,
	SILAN_DEV_VIU,
	SILAN_DEV_VPREP,
	SILAN_DEV_MPU,
	SILAN_DEV_MPU_BUS,
	SILAN_DEV_VENC,
	SILAN_DEV_FDIP,
	SILAN_DEV_SECDMX,
	SILAN_DEV_GPIO1,
	SILAN_DEV_GPIO2,
	SILAN_DEV_GPIO3,
	SILAN_DEV_GPIO4,
	SILAN_DEV_GPIO5,
	SILAN_DEV_GPIO6,
	SILAN_DEV_DXB,
	SILAN_DEV_PXB,
	SILAN_DEV_CXB,
	SILAN_DEV_LSP,
	SILAN_DEV_JTAG,
	SILAN_DEV_WATCHDOG,
	SILAN_DEV_ISP,
	SILAN_DEV_CCU,
	SILAN_DEV_CM0,
	SILAN_DEV_IVS_ACLK,
	SILAN_DEV_ADC,
	SILAN_DEV_DDR0,
	SILAN_DEV_DDR1,
	/* DUMMY can be used if the user is not a specific device */
	SILAN_DEV_DUMMY,

	/* This should always be the last */
	SILAN_DEV_END
};

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
	enum silan_clk_ids clk_id[CLK_ID_NUM];
	struct silan_clk *parent;
	int (*enable)(struct silan_clk *);
	void (*disable)(struct silan_clk *);
	unsigned long (*get_rate)(struct silan_clk *);
	int (*set_rate)(struct silan_clk *, unsigned long);
	/* some devices have multiple clock like core and bus*/
	unsigned long (*get_bus_rate)(struct silan_clk *);
	int (*set_bus_rate)(struct silan_clk *, unsigned long);
};

struct silan_clk_device
{
	char *dev_name;
	enum silan_device_ids dev_id;
	struct silan_clk clk;
	struct clk_hw hw;
};

DEFINE_SPINLOCK(clocks_lock);

static int clk_enable(struct silan_clk *clk)
{
	int reg_offset, bit_offset, i;
	u32 reg_value = 0;

	spin_lock(&clocks_lock);
	if (SILAN_CLK_WATCHDOG == clk->clk_id[0]) {
		reg_value = sl_readl(SILAN_LSP_CTRL);
		reg_value |= (0x1 << 0);
		sl_writel(reg_value, SILAN_LSP_CTRL);
	} else if (SILAN_CLK_TIMER == clk->clk_id[0]) {
#if 1 // have a hardware bug
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
			printk("%s: clk_id[i]:%x, reg_value: %x.\n", __func__, clk->clk_id[i], reg_value);
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

static void clk_disable(struct silan_clk *clk)
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

/*
 * Get clock of PMU
 */
static unsigned long clk_get_pmu(struct silan_clk *clk)
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
#if 0
/*
 * Set clock of PMU
 * @rate: PMU clock to be set
 */
static int clk_set_pmu_rate(struct silan_clk *clk, unsigned long rate)
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
#endif

/* Get video display PLL clock */
static unsigned long get_silan_video_pllclk(void)
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

/* Get system PLL clock */
static unsigned long get_silan_sys_pllclk(void)
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

/*
 * Get clock of PX bar
 */
static unsigned long clk_get_pxb(struct silan_clk *clk)
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
static int clk_set_pxb_rate(struct silan_clk *clk, unsigned long rate)
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
static unsigned long clk_get_lsp(struct silan_clk * clk)
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
static int clk_set_lsp_rate(struct silan_clk *clk, unsigned long rate)
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

static struct silan_clk_device sc8925_clk_devices[] = {

	[SILAN_DEV_USB_HOST] = {
		.dev_name = "usb-host",
		.dev_id = SILAN_DEV_USB_HOST,
		.clk = {
			.clk_id = {SILAN_CLK_USB_HOST, SILAN_CLK_USBPHY},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_USB_OTG] = {
		.dev_name = "usb-otg",
		.dev_id = SILAN_DEV_USB_OTG,
		.clk = {
			.clk_id = {SILAN_CLK_USB_OTG, SILAN_CLK_USBPHY},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_GMAC0] = {
		.dev_name = "gmac0",
		.dev_id = SILAN_DEV_GMAC0,
		.clk = {
			.clk_id = {SILAN_CLK_GMAC0, SILAN_CLK_GMAC_BUS},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_GMAC1] = {
		.dev_name = "gmac1",
		.dev_id = SILAN_DEV_GMAC1,
		.clk = {
			.clk_id = {SILAN_CLK_GMAC1, SILAN_CLK_GMAC_BUS},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_DMAC0] = {
		.dev_name = "dmac0",
		.dev_id = SILAN_DEV_DMAC0,
		.clk = {
			.clk_id = {SILAN_CLK_DMAC0},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_DMAC1] = {
		.dev_name = "dmac1",
		.dev_id = SILAN_DEV_DMAC1,
		.clk = {
			.clk_id = {SILAN_CLK_DMAC1, SILAN_CLK_AUDIO},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},
#if 0
	[SILAN_DEV_I2S_T4] = {
		.dev_name = "i2s-t4",
		.dev_id = SILAN_DEV_I2S_T4,
		.clk = {
			.clk_id = {SILAN_CLK_I2S_T4, SILAN_PADMUX_I2S_MCLK, SILAN_CLK_AUDIO},
			.parent = &sc8925_clk_devices[SILAN_DEV_CXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = NULL,
			/* Note:there is problem with T4 hardware, we'd better not use it */
			.set_rate = clk_set_i2s_t4_rate,
			//.set_rate = clk_set_i2s_hdmi_rate,
		},
	},

	[SILAN_DEV_I2S_MIC] = {
		.dev_name = "i2s-mic",
		.dev_id = SILAN_DEV_I2S_MIC,
		.clk = {
			.clk_id = {SILAN_CLK_I2S_MIC, SILAN_CLK_AUDIO},
			.parent = &sc8925_clk_devices[SILAN_DEV_CXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = NULL,
			.set_rate = clk_set_i2s_mic_rate,
		},
	},

	[SILAN_DEV_I2S_HDMI] = {
		.dev_name = "i2s-hdmi",
		.dev_id = SILAN_DEV_I2S_HDMI,
		.clk = {
			.clk_id = {SILAN_CLK_I2S_HDMI, SILAN_CLK_AUDIO},
			.parent = &sc8925_clk_devices[SILAN_DEV_CXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = NULL,
			.set_rate = clk_set_i2s_hdmi_rate,
		},
	},
#endif
	[SILAN_DEV_UART0] = {
		.dev_name = "uart0",
		.dev_id = SILAN_DEV_UART0,
		.clk = {
			.clk_id = {SILAN_CLK_UART0},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_UART1] = {
		.dev_name = "uart1",
		.dev_id = SILAN_DEV_UART1,
		.clk = {
			.clk_id = {SILAN_CLK_UART1},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_UART2] = {
		.dev_name = "uart2",
		.dev_id = SILAN_DEV_UART2,
		.clk = {
			.clk_id = {SILAN_CLK_UART2, SILAN_CLK_DMABAR},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_UART3] = {
		.dev_name = "uart3",
		.dev_id = SILAN_DEV_UART3,
		.clk = {
			.clk_id = {SILAN_CLK_UART3, SILAN_CLK_DMABAR},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_UART4] = {
		.dev_name = "uart4",
		.dev_id = SILAN_DEV_UART4,
		.clk = {
			.clk_id = {SILAN_CLK_UART4, SILAN_CLK_DMABAR},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_UART5] = {
		.dev_name = "uart5",
		.dev_id = SILAN_DEV_UART5,
		.clk = {
			.clk_id = {SILAN_CLK_UART5, SILAN_CLK_DMABAR},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_UART6] = {
		.dev_name = "uart6",
		.dev_id = SILAN_DEV_UART6,
		.clk = {
			.clk_id = {SILAN_CLK_UART6},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_I2C0] = {
		.dev_name = "i2c0",
		.dev_id = SILAN_DEV_I2C0,
		.clk = {
			.clk_id = {SILAN_CLK_I2C0},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_I2C1] = {
		.dev_name = "i2c1",
		.dev_id = SILAN_DEV_I2C1,
		.clk = {
			.clk_id = {SILAN_CLK_I2C1},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_I2C2] = {
		.dev_name = "i2c2",
		.dev_id = SILAN_DEV_I2C2,
		.clk = {
			.clk_id = {SILAN_CLK_I2C2},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_I2C3] = {
		.dev_name = "i2c3",
		.dev_id = SILAN_DEV_I2C3,
		.clk = {
			.clk_id = {SILAN_CLK_I2C3, SILAN_CLK_DMABAR},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_I2C4] = {
		.dev_name = "i2c4",
		.dev_id = SILAN_DEV_I2C4,
		.clk = {
			.clk_id = {SILAN_CLK_I2C4, SILAN_CLK_DMABAR},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_SPI] = {
		.dev_name = "spi",
		.dev_id = SILAN_DEV_SPI,
		.clk = {
			.clk_id = {SILAN_CLK_SPI},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_SPI_NORMAL] = {
		.dev_name = "spi-normal",
		.dev_id = SILAN_DEV_SPI_NORMAL,
		.clk = {
			.clk_id = {SILAN_CLK_SPI_NORMAL},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_SSP] = {
		.dev_name = "ssp",
		.dev_id = SILAN_DEV_SSP,
		.clk = {
			.clk_id = {SILAN_CLK_SSP},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = NULL,
		},
	},
#if 0
	[SILAN_DEV_SPDIF_IN] = {
		.dev_name = "spdif-in",
		.dev_id = SILAN_DEV_SPDIF_IN,
		.clk = {
			.clk_id = {SILAN_CLK_SPDIF_IN, SILAN_CLK_AUDIO, SILAN_CLK_DSP0},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_cxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_SPDIF_OUT] = {
		.dev_name = "spdif-out",
		.dev_id = SILAN_DEV_SPDIF_OUT,
		.clk = {
			.clk_id = {SILAN_CLK_SPDIF_OUT, SILAN_CLK_AUDIO, SILAN_CLK_DSP0},
			.parent = &sc8925_clk_devices[SILAN_DEV_PXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = NULL,
			.set_rate = clk_set_spdif_rate,
		},
	},

	[SILAN_DEV_HDMI] = {
		.dev_name = "hdmi",
		.dev_id = SILAN_DEV_HDMI,
		.clk = {
			.clk_id = {SILAN_CLK_HDMI},
			.parent = &sc8925_clk_devices[SILAN_DEV_CXB].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_cxb,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_CAN] = {
		.dev_name = "can",
		.dev_id = SILAN_DEV_CAN,
		.clk = {
			.clk_id = {SILAN_CLK_CAN},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_SD] = {
		.dev_name = "sd",
		.dev_id = SILAN_DEV_SD,
		.clk = {
			.clk_id = {SILAN_CLK_SD},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_sd_emmc_sdio,
			.set_rate = clk_set_sd_emmc_sdio_rate,
		},
	},

	[SILAN_DEV_EMMC] = {
		.dev_name = "emmc",
		.dev_id = SILAN_DEV_EMMC,
		.clk = {
			.clk_id = {SILAN_CLK_EMMC},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_sd_emmc_sdio,
			.set_rate = clk_set_sd_emmc_sdio_rate,
		},
	},

	[SILAN_DEV_SDIO] = {
		.dev_name = "sdio",
		.dev_id = SILAN_DEV_SDIO,
		.clk = {
			.clk_id = {SILAN_CLK_SDIO},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_sd_emmc_sdio,
			.set_rate = clk_set_sd_emmc_sdio_rate,
		},
	},
#endif
	[SILAN_DEV_SCI] = {
		.dev_name = "sci",
		.dev_id = SILAN_DEV_SCI,
		.clk = {
			.clk_id = {SILAN_CLK_SCI},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_PWM] = {
		.dev_name = "pwm",
		.dev_id = SILAN_DEV_PWM,
		.clk = {
			.clk_id = {SILAN_CLK_PWM},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},
#if 0
	[SILAN_DEV_SDRAM] = {
		.dev_name = "sdram",
		.dev_id = SILAN_DEV_SDRAM,
		.clk = {
			.clk_id = {SILAN_CLK_SDRAM},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_sdram,
			.set_rate = clk_set_sdram_rate,
		},
	},
#endif
	[SILAN_DEV_TIMER] = {
		.dev_name = "timer",
		.dev_id = SILAN_DEV_TIMER,
		.clk = {
			.clk_id = {SILAN_CLK_TIMER},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_RTC] = {
		.dev_name = "rtc",
		.dev_id = SILAN_DEV_RTC,
		.clk = {
			.clk_id = {SILAN_CLK_RTC},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},
#if 0
	[SILAN_DEV_PMU] = {
		.dev_name = "pmu",
		.dev_id = SILAN_DEV_PMU,
		.clk = {
			.clk_id = {SILAN_CLK_PMU},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pmu,
			.set_rate = clk_set_pmu_rate,
		},
	},

	[SILAN_DEV_GPU] = {
		.dev_name = "gpu",
		.dev_id = SILAN_DEV_GPU,
		.clk = {
			.clk_id = {SILAN_CLK_GPU, SILAN_CLK_IVS_BUS},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_gpu,
			.set_rate = clk_set_gpu_rate,
			.get_bus_rate = clk_get_ivs_aclk,
			.set_bus_rate = clk_set_ivs_aclk_rate,
		},
	},


	[SILAN_DEV_VPU0] = {
		.dev_name = "vpu0",
		.dev_id = SILAN_DEV_VPU0,
		.clk = {
			.clk_id = {SILAN_CLK_VPU0, SILAN_CLK_VJB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_vpu0,
			.set_rate = clk_set_vpu0_rate,
			.get_bus_rate = clk_get_vjb,
			.set_bus_rate = clk_set_vjb_rate,
		},
	},

	[SILAN_DEV_VPU1] = {
		.dev_name = "vpu1",
		.dev_id = SILAN_DEV_VPU1,
		.clk = {
			.clk_id = {SILAN_CLK_VPU1, SILAN_CLK_VJB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_vpu1,
			.set_rate = clk_set_vpu1_rate,
			.get_bus_rate = clk_get_vjb,
			.set_bus_rate = clk_set_vjb_rate,
		},
	},

	[SILAN_DEV_JPU] = {
		.dev_name = "jpu",
		.dev_id = SILAN_DEV_JPU,
		.clk = {
			.clk_id = {SILAN_CLK_JPU, SILAN_CLK_VJB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_jpu,
			.set_rate = clk_set_jpu_rate,
			.get_bus_rate = clk_get_vjb,
			.set_bus_rate = clk_set_vjb_rate,
		},
	},

	[SILAN_DEV_VJB] = {
		.dev_name = "vjb",
		.dev_id = SILAN_DEV_VJB,
		.clk = {
			.clk_id = {SILAN_CLK_VJB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_vjb,
			.set_rate = clk_set_vjb_rate,
		},
	},

	[SILAN_DEV_VPP] = {
		.dev_name = "vpp",
		.dev_id = SILAN_DEV_VPP,
		.clk = {
#ifdef CONFIG_FB_SILAN_TCON
			.clk_id = {SILAN_CLK_VPP, SILAN_CLK_TCON},
#else
			.clk_id = {SILAN_CLK_VPP, SILAN_CLK_HPB},
#endif
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_vpp,
			.set_rate = clk_set_vpp_rate,
			.get_bus_rate = clk_get_hpb_aclk,
			.set_bus_rate = clk_set_hpb_aclk_rate,
		},
	},


	[SILAN_DEV_DSP0] = {
		.dev_name = "dsp0",
		.dev_id = SILAN_DEV_DSP0,
		.clk = {
			.clk_id = {SILAN_CLK_DSP0, SILAN_CLK_HPB, SILAN_CLK_AUDIO},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_dsp0,
			.set_rate = clk_set_dsp0_rate,
			.get_bus_rate = clk_get_hpb_aclk,
			.set_bus_rate = clk_set_hpb_aclk_rate,
		},
	},

	[SILAN_DEV_DSP1] = {
		.dev_name = "dsp1",
		.dev_id = SILAN_DEV_DSP1,
		.clk = {
			.clk_id = {SILAN_CLK_DSP1, SILAN_CLK_HPB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_dsp1,
			.set_rate = clk_set_dsp1_rate,
			.get_bus_rate = clk_get_hpb_aclk,
			.set_bus_rate = clk_set_hpb_aclk_rate,
		},
	},

	[SILAN_DEV_VIU] = {
		.dev_name = "viu",
		.dev_id = SILAN_DEV_VIU,
		.clk = {
			.clk_id = {SILAN_CLK_VIU, SILAN_CLK_VIP},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_viu,
			.set_rate = clk_set_viu_rate,
			.get_bus_rate = clk_get_vip_aclk,
			.set_bus_rate = clk_set_vip_aclk_rate,
		},
	},

	[SILAN_DEV_VPREP] = {
		.dev_name = "vprep",
		.dev_id = SILAN_DEV_VPREP,
		.clk = {
			.clk_id = {SILAN_CLK_VPREP, SILAN_CLK_VIP},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_vprep,
			.set_rate = clk_set_vprep_rate,
			.get_bus_rate = clk_get_vip_aclk,
			.set_bus_rate = clk_set_vip_aclk_rate,
		},
	},

	[SILAN_DEV_MPU] = {
		.dev_name = "mpu",
		.dev_id = SILAN_DEV_MPU,
		.clk = {
			.clk_id = {SILAN_CLK_MPU, SILAN_CLK_IVS_BUS},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_mpu,
			.set_rate = clk_set_mpu_rate,
			.get_bus_rate = clk_get_ivs_aclk,
			.set_bus_rate = clk_set_ivs_aclk_rate,
		},
	},

	[SILAN_DEV_VENC] = {
		.dev_name = "venc",
		.dev_id = SILAN_DEV_VENC,
		.clk = {
			.clk_id = {SILAN_CLK_VENC, SILAN_CLK_VJB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_venc,
			.set_rate = clk_set_venc_rate,
			.get_bus_rate = clk_get_vjb,
			.set_bus_rate = clk_set_vjb_rate,
		},
	},

	[SILAN_DEV_FDIP] = {
		.dev_name = "fdip",
		.dev_id = SILAN_DEV_FDIP,
		.clk = {
			.clk_id = {SILAN_CLK_FDIP, SILAN_CLK_IVS_BUS},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_fdip,
			.set_rate = clk_set_fdip_rate,
			.get_bus_rate = clk_get_ivs_aclk,
			.set_bus_rate = clk_set_ivs_aclk_rate,
		},
	},

	[SILAN_DEV_SECDMX] = {
		.dev_name = "secdmx",
		.dev_id = SILAN_DEV_SECDMX,
		.clk = {
			.clk_id = {SILAN_CLK_SECDMX},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_secdmx,
			.set_rate = clk_set_secdmx_rate,
		},
	},
#endif
	/* pad should open in padmux.c */
	[SILAN_DEV_GPIO1] = {
		.dev_name = "gpio1",
		.dev_id = SILAN_DEV_GPIO1,
		.clk = {
			.clk_id = {SILAN_CLK_GPIO1},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_GPIO2] = {
		.dev_name = "gpio2",
		.dev_id = SILAN_DEV_GPIO2,
		.clk = {
			.clk_id = {SILAN_CLK_GPIO2},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_GPIO3] = {
		.dev_name = "gpio3",
		.dev_id = SILAN_DEV_GPIO3,
		.clk = {
			.clk_id = {SILAN_CLK_GPIO3},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_GPIO4] = {
		.dev_name = "gpio4",
		.dev_id = SILAN_DEV_GPIO4,
		.clk = {
			.clk_id = {SILAN_CLK_GPIO4},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_GPIO5] = {
		.dev_name = "gpio5",
		.dev_id = SILAN_DEV_GPIO5,
		.clk = {
			.clk_id = {SILAN_CLK_GPIO5},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_GPIO6] = {
		.dev_name = "gpio6",
		.dev_id = SILAN_DEV_GPIO6,
		.clk = {
			.clk_id = {SILAN_CLK_GPIO6},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},
#if 0
	[SILAN_DEV_DXB] = {
		.dev_name = "dxb",
		.dev_id = SILAN_DEV_DXB,
		.clk = {
			.clk_id = {SILAN_CLK_DXB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_dxb,
			.set_rate = clk_set_dxb_rate,
		},
	},
#endif

	[SILAN_DEV_PXB] = {
		.dev_name = "pxb",
		.dev_id = SILAN_DEV_PXB,
		.clk = {
			.clk_id = {SILAN_CLK_PXB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_pxb,
			.set_rate = clk_set_pxb_rate,
		},
	},

#if 0
	[SILAN_DEV_CXB] = {
		.dev_name = "cxb",
		.dev_id = SILAN_DEV_CXB,
		.clk = {
			.clk_id = {SILAN_CLK_CXB},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_cxb,
			.set_rate = clk_set_cxb_rate,
		},
	},
#endif
	[SILAN_DEV_LSP] = {
		.dev_name = "lsp",
		.dev_id = SILAN_DEV_LSP,
		.clk = {
			.clk_id = {SILAN_CLK_LSP},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = clk_set_lsp_rate,
		},
	},

	[SILAN_DEV_JTAG] = {
		.dev_name = "jtag",
		.dev_id = SILAN_DEV_JTAG,
		.clk = {
			.clk_id = {0},
			.parent = NULL,
			.enable = NULL,
			.disable = NULL,
			.get_rate = NULL,
			.set_rate = NULL,
		},
	},

	[SILAN_DEV_WATCHDOG] = {
		.dev_name = "watchdog",
		.dev_id = SILAN_DEV_WATCHDOG,
		.clk = {
			.clk_id = {SILAN_CLK_WATCHDOG},
			.parent = &sc8925_clk_devices[SILAN_DEV_LSP].clk,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_lsp,
			.set_rate = NULL,
		},
	},
#if 0
	[SILAN_DEV_ISP] = {
		.dev_name = "isp",
		.dev_id = SILAN_DEV_ISP,
		.clk = {
			.clk_id = {SILAN_CLK_ISP, SILAN_CLK_VIP},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_isp,
			.set_rate = clk_set_isp_rate,
			.get_bus_rate = clk_get_vip_aclk,
			.set_bus_rate = clk_set_vip_aclk_rate,
		},
	},

	[SILAN_DEV_CCU] = {
		.dev_name = "ccu",
		.dev_id = SILAN_DEV_CCU,
		.clk = {
			.clk_id = {SILAN_CLK_CCU},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_ccu,
			.set_rate = clk_set_ccu_rate,
		},
	},

	[SILAN_DEV_CM0] = {
		.dev_name = "cm0",
		.dev_id = SILAN_DEV_CM0,
		.clk = {
			.clk_id = {SILAN_CLK_CM0_HCLK, SILAN_CLK_CM0_DCLK, SILAN_CLK_CM0_FCLK},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = NULL,
			.set_rate = NULL,
			.get_bus_rate = NULL,
			.set_bus_rate = NULL,
		},
	},

	[SILAN_DEV_IVS_ACLK] = {
		.dev_name = "ivs_aclk",
		.dev_id = SILAN_DEV_IVS_ACLK,
		.clk = {
			.clk_id = {SILAN_CLK_IVS_BUS},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = clk_get_ivs_aclk,
			.set_rate = clk_set_ivs_aclk_rate,
		},
	},

	[SILAN_DEV_ADC] = {
		.dev_name = "adc",
		.dev_id = SILAN_DEV_ADC,
		.clk = {
			.clk_id = {SILAN_CLK_ADC, SILAN_CLK_ADC_PCLK},
			.parent = NULL,
			.enable = clk_enable,
			.disable = clk_disable,
			.get_rate = NULL,
			.set_rate = NULL,
		},
	},
#endif
};

struct silan_clk *silan_clk_get(struct device *dev, const char *id)
{
	int i, found = 0;

	for (i=0; i<ARRAY_SIZE(sc8925_clk_devices); i++)
	{
		if (NULL == sc8925_clk_devices[i].dev_name)
			continue;
		if (strcmp(id, sc8925_clk_devices[i].dev_name) == 0)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return &sc8925_clk_devices[i].clk;
	else {
		printk("ERROR: failed to get device %s from the clock list\n", id);
		return ERR_PTR(-ENOENT);
	}
}

int silan_clk_enable(struct silan_clk *clk)
{
	return clk_enable(clk);
}

void silan_clk_disable(struct silan_clk *clk)
{
	clk_disable(clk);
}
EXPORT_SYMBOL(silan_clk_get);
EXPORT_SYMBOL(silan_clk_enable);
EXPORT_SYMBOL(silan_clk_disable);

#define to_silan_clkdev(_hw) container_of(_hw, struct silan_clk_device, hw)

static int silan_enable(struct clk_hw *hw)
{
	struct silan_clk_device *clk_dev;

	clk_dev = to_silan_clkdev(hw);

	if (clk_dev->clk.enable) {
		clk_dev->clk.enable(&clk_dev->clk);
		printk("%s, %s.\n", __func__, clk_dev->dev_name);
	}
	return 0;
}

static void silan_disable(struct clk_hw *hw)
{
	struct silan_clk_device *clk_dev;

	clk_dev = to_silan_clkdev(hw);

	if (clk_dev->clk.disable) {
		printk("%s, %s.\n", __func__, clk_dev->dev_name);
		clk_dev->clk.disable(&clk_dev->clk);
	}
}

static unsigned long silan_recalc_rate(struct clk_hw *hw,
		unsigned long parent_rate)
{
	struct silan_clk_device *clk_dev;
	unsigned long rate = 0;

	clk_dev = to_silan_clkdev(hw);

	if (clk_dev->clk.get_rate) {
		rate = clk_dev->clk.get_rate(&clk_dev->clk);
		printk("%s, %ld, %s.\n", __func__, rate, clk_dev->dev_name);
	}
	return rate;
}

static const struct clk_ops clk_silan_ops = {
	.enable = silan_enable,
	.disable = silan_disable,
	.recalc_rate = silan_recalc_rate,
};

int __init clk_hw_register_silan(
	struct clk_hw *hw,
	const char *name,
	const char *parent_name,
	unsigned long flags)
{
	int ret;
	struct clk_init_data init;

	init.name = name;
	init.ops = &clk_silan_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);
	hw->init = &init;

	/* register the clock */
	ret = clk_hw_register(NULL, hw);

	return ret;
}

static struct clk_hw *
sc8925_lookup_clk(struct of_phandle_args *spec, void *data)
{
	printk("%s, %d.\n", __func__, spec->args[0]);
	return &sc8925_clk_devices[spec->args[0]].hw;
}

static void __init sc8925_init(struct device_node *np)
{
	int i;

	for (i=0; i < ARRAY_SIZE(sc8925_clk_devices); i++) {
		if (NULL == sc8925_clk_devices[i].dev_name)
			continue;
		clk_hw_register_silan(
				&sc8925_clk_devices[i].hw,
				sc8925_clk_devices[i].dev_name,
				NULL, 0);
	}

	of_clk_add_hw_provider(np, sc8925_lookup_clk, NULL);

	return;
}
CLK_OF_DECLARE(slian_sc8925_clk, "silan,sc8925-clk", sc8925_init);

//#define CONFIG_SILAN_ETH0
//#define CONFIG_SILAN_ETH0_RMII_MODE

#undef readl
#undef writel
#define readl sl_readl
#define writel sl_writel
void silan_mac0_mode_init(void)
{
	unsigned int regval;
	unsigned int value = readl(SILAN_SYS_REG16);

	regval = readl(SILAN_HSPMISC_ENDIAN);
	regval &= (~ETH0_MODE_MASK);
#ifdef CONFIG_SILAN_ETH0_RGMII_MODE
	// gmac0 tx delay chain
	//value |= 1 << 16;
	value &= ~(0xf << 12) ;
	value |= (0xf << 12);

	//gmac0 rx delay chain
	value |= 1 << 21;
	value &= ~(0xf << 17) ;
	value |= (0x0 << 17);

	regval |= ETH0_RGMII_100M;
	writel(0x1d, SILAN_SYS_REG49); //RGMII100M IVS3 新版需要
#elif defined (CONFIG_SILAN_ETH0_RMII_MODE)
	regval |= ETH0_RMII_100M;
	writel(regval | 0xd000, SILAN_HSPMISC_ENDIAN);
#elif defined (CONFIG_SILAN_ETH0_MII_GMII_MODE)
	regval |= ETH0_MII_GMII_100M;
#endif
	udelay(1000);
	writel(regval, SILAN_HSPMISC_ENDIAN);
	writel(value, SILAN_SYS_REG16);

}

void silan_mac1_mode_init(void)
{
	unsigned int regval;
	unsigned int value = readl(SILAN_SYS_REG16);

	regval = readl(SILAN_HSPMISC_ENDIAN);
	regval &= (~ETH1_MODE_MASK);
#ifdef CONFIG_SILAN_ETH1_RGMII_MODE
	// gmac1 tx delay chain
	//	value |= 1 << 26;
	value &= ~(0xf << 22) ;
	value |= (0xf << 22);

	//gmac1 rx delay chain
//	value |= 1 << 31;
	value &= ~(0xf << 27) ;
	value |= (0x8 << 27);

	regval |= ETH1_RGMII_100M;
#elif defined (CONFIG_SILAN_ETH1_RMII_MODE)
	regval |= ETH1_RMII_100M;
	writel(regval | 0xd0000, SILAN_HSPMISC_ENDIAN);
#elif defined (CONFIG_SILAN_ETH1_MII_GMII_MODE)
	// gmac1 tx delay chain
	value &= ~(0xf << 22) ;
	value |= (0x3 << 22);
	regval |= ETH1_MII_GMII_1000M;
#endif
	udelay(1000);
	writel(regval, SILAN_HSPMISC_ENDIAN);
	writel(value, SILAN_SYS_REG16);

}

void silan_emmc_phase_init(void)
{
	unsigned int regval;

	regval = readl(SILAN_SYS_REG29);
	regval |= (1 << 19);
	regval |= (1 << 30);
	writel(regval, SILAN_SYS_REG29);

}

void silan_sd_phase_init(void)
{
	unsigned int regval;

	regval = readl(SILAN_SYS_REG29);
	regval |= (1 << 18);
	regval |= (1 << 25);
//	regval &= ~(1 << 25);


	regval &= ~(0xf << 21);
	regval |= (0xf << 21);
	writel(regval, SILAN_SYS_REG29);

//	regval = readl(SILAN_SYS_REG3);
//	regval |= (1 << 8);
//	writel(regval, SILAN_SYS_REG3);
}

static int __init silan_device_preinit(void)
{

#ifdef CONFIG_SILAN_ETH0
	silan_mac0_mode_init();
#endif
#ifdef CONFIG_SILAN_ETH1
	silan_mac1_mode_init();
#endif
#ifdef CONFIG_USB_SUPPORT
	//usb_host_board_init();
	//usb_otg_board_init();
#endif
#ifdef CONFIG_SILAN_EMMC
	silan_emmc_phase_init();
#endif
#ifdef CONFIG_SILAN_SD
	silan_sd_phase_init();
#endif


	return 0;
}
device_initcall(silan_device_preinit);
