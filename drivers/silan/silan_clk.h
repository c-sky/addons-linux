/*
 * Data type and register bit definition for IVS clock
 *
 * xuhang@silan.com.cn
 */
#ifndef _SILAN_CLOCK_H
#define _SILAN_CLOCK_H
//#include "silan_def.h"

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

/* Clock ID */
enum silan_clk_ids {
	SILAN_CLK_START = 0,

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
    SILAN_CLK_SPI_NORMAL	= 1 * 32 + 18,
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
    SILAN_CLK_EFUSE 		= 30 * 32 + 13,
    SILAN_CLK_CCU	 		= 30 * 32 + 14,

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
    SILAN_CLK_ISP	    	= 30 * 32 + 25,
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

/* The clk struct */
struct clk {
	enum silan_clk_ids clk_id[CLK_ID_NUM];
	struct clk *parent;
	int (*enable)(struct clk *);
	void (*disable)(struct clk *);
	unsigned long (*get_rate)(struct clk *);
	int (*set_rate)(struct clk *, unsigned long);
	/* some devices have multiple clock like core and bus*/
	unsigned long (*get_bus_rate)(struct clk *);
	int (*set_bus_rate)(struct clk *, unsigned long);
};

#if 1
struct sys_pll_freq_t {
	u32	freq;	/* Frequency in HZ */
	u16	bwadj;	/* Bandwidth adjustment */
	u16	clkf;	/* Multiplication factor */
	u8	clkr;	/* Reference clock divider */
	u8	clkod;	/* Post output divider */
	u8	cs;		/* Frequency point selection */
};
#endif

int silan_clk_enable(struct clk *clk);
void silan_clk_disable(struct clk *clk);
unsigned long clk_get_dxb(struct clk *clk);
int clk_set_dxb_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_pxb(struct clk *clk);
int clk_set_pxb_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_lsp(struct clk *clk);
int clk_set_lsp_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_cxb(struct clk *clk);
int clk_set_cxb_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_vjb(struct clk *clk);
int clk_set_vjb_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_hpu_spu_bus(struct clk *clk);
int clk_set_hpu_spu_bus_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_vpu0(struct clk *clk);
int clk_set_vpu0_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_vpu1(struct clk *clk);
int clk_set_vpu1_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_gpu(struct clk *clk);
int clk_set_gpu_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_gpu_bus(struct clk *clk);
int clk_set_gpu_bus_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_jpu(struct clk *clk);
int clk_set_jpu_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_vpp(struct clk *clk);
int clk_set_vpp_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_dsp0(struct clk *clk);
int clk_set_dsp0_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_dsp1(struct clk *clk);
int clk_set_dsp1_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_sd_emmc_sdio(struct clk *clk);
int clk_set_sd_emmc_sdio_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_viu(struct clk *clk);
int clk_set_viu_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_spu(struct clk *clk);
int clk_set_spu_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_dit(struct clk *clk);
int clk_set_dit_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_pmu(struct clk *clk);
int clk_set_pmu_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_am(struct clk * clk);
int clk_set_am_rate(struct clk *clk, unsigned long rate);
int clk_set_spdif_rate(struct clk *clk, unsigned long rate);
int clk_set_i2s_t4_rate(struct clk *clk, unsigned long rate);
int clk_set_i2s_mic_rate(struct clk *clk, unsigned long rate);
int clk_set_i2s_hdmi_rate(struct clk *clk, unsigned long rate);
int clk_set_apwm_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_mpu(struct clk *clk);
int clk_set_mpu_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_ivs_aclk(struct clk *clk);
int clk_set_ivs_aclk_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_vip_aclk(struct clk *clk);
int clk_set_vip_aclk_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_vprep(struct clk *clk);
int clk_set_vprep_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_venc(struct clk *clk);
int clk_set_venc_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_fdip(struct clk *clk);
int clk_set_fdip_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_secdmx(struct clk *clk);
int clk_set_secdmx_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_sdram(struct clk *clk);
int clk_set_sdram_rate(struct clk *clk, unsigned long rate);


unsigned long clk_get_ccu(struct clk *clk);
int clk_set_ccu_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_hpb_aclk(struct clk *clk);
int clk_set_hpb_aclk_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_isp(struct clk *clk);
int clk_set_isp_rate(struct clk *clk, unsigned long rate);


int silan_clk_enable_watchdog(struct clk *clk);
void silan_clk_disable_watchdog(struct clk *clk);

#endif /* _SILAN_CLOCK_H */
