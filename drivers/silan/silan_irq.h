#ifndef __SILAN_IRQ_H
#define __SILAN_IRQ_H

#define NO_IRQ							(-1)

/* IRQ type */
#define INTC_INT_RISE_EDGE              0
#define INTC_INT_FALL_EDGE              1
#define INTC_INT_HIGH_LEVEL             2
#define INTC_INT_LOW_LEVEL              3
#define INTC_INT_DISABLE                4

#define PIC_IRQ_BASE					0

/* LSP table */
#define PIC_IRQ_UART7					(0 + PIC_IRQ_BASE)
#define PIC_IRQ_GPIO6					(1 + PIC_IRQ_BASE)
#define PIC_IRQ_WATCHDOG_SUB_RST        (2 + PIC_IRQ_BASE)
#define PIC_IRQ_WATCHDOG_SUB            (3 + PIC_IRQ_BASE)
#define PIC_IRQ_CAN						(4 + PIC_IRQ_BASE)
#define PIC_IRQ_GPIO5					(5 + PIC_IRQ_BASE)
#define PIC_IRQ_GPIO4					(6 + PIC_IRQ_BASE)
#define PIC_IRQ_GPIO3					(7 + PIC_IRQ_BASE)
#define PIC_IRQ_GPIO2					(8 + PIC_IRQ_BASE)
#define PIC_IRQ_GPIO1					(9 + PIC_IRQ_BASE)
#define PIC_IRQ_UART2					(10 + PIC_IRQ_BASE)
#define PIC_IRQ_UART1					(11 + PIC_IRQ_BASE)
#define PIC_IRQ_SCI						(12 + PIC_IRQ_BASE)
#define PIC_IRQ_ADC						(12 + PIC_IRQ_BASE) /* SCI And ADC Reuse IRQ 12*/
#define PIC_IRQ_RTC						(13 + PIC_IRQ_BASE)
#define PIC_IRQ_I2C3					(14 + PIC_IRQ_BASE)
#define PIC_IRQ_I2C2					(15 + PIC_IRQ_BASE)
#define PIC_IRQ_I2C1					(16 + PIC_IRQ_BASE)
#define PIC_IRQ_LSP_TIMER				(17 + PIC_IRQ_BASE)
#define PIC_IRQ_WDG						(18 + PIC_IRQ_BASE)

/* HSP table */
#define PIC_IRQ_GMAC1					(19 + PIC_IRQ_BASE)
#define PIC_IRQ_UART6					(20 + PIC_IRQ_BASE)
#define PIC_IRQ_UART5					(21 + PIC_IRQ_BASE)
#define PIC_IRQ_I2C5                    (22 + PIC_IRQ_BASE)
#define PIC_IRQ_DMAC0                   (23 + PIC_IRQ_BASE) /* HSP DMAC */
#define PIC_IRQ_SPI_NORMAL				(24 + PIC_IRQ_BASE)
#define PIC_IRQ_SSP						(25 + PIC_IRQ_BASE)
#define PIC_IRQ_GMAC0					(26 + PIC_IRQ_BASE)
#define PIC_IRQ_SDIO					(27 + PIC_IRQ_BASE)
#define PIC_IRQ_SD						(28 + PIC_IRQ_BASE)
#define PIC_IRQ_MMC						(29 + PIC_IRQ_BASE)
#define PIC_IRQ_I2C4					(30 + PIC_IRQ_BASE)
#define PIC_IRQ_USB_HOST				(31 + PIC_IRQ_BASE)
#define PIC_IRQ_USB_OTG					(32 + PIC_IRQ_BASE)
#define PIC_IRQ_UART4					(33 + PIC_IRQ_BASE)
#define PIC_IRQ_UART3					(34 + PIC_IRQ_BASE)

/* Ivs table */
#define PIC_IRQ_VPRE					(36 + PIC_IRQ_BASE)
#define PIC_IRQ_VENC					(37 + PIC_IRQ_BASE) /* H264 */
#define PIC_IRQ_MPU 					(38 + PIC_IRQ_BASE)
#define PIC_IRQ_FDIP					(39 + PIC_IRQ_BASE)
#define PIC_IRQ_JPEG					(40 + PIC_IRQ_BASE)
#define PIC_IRQ_HDMI					(42 + PIC_IRQ_BASE)
#define PIC_IRQ_VPU0					(43 + PIC_IRQ_BASE)
#define PIC_IRQ_VPU1					(44 + PIC_IRQ_BASE)
#define PIC_IRQ_GPU						(46 + PIC_IRQ_BASE)
#define PIC_IRQ_VIU						(47 + PIC_IRQ_BASE)
#define PIC_IRQ_VPP						(48 + PIC_IRQ_BASE)
#define PIC_IRQ_CCU						(49 + PIC_IRQ_BASE)
#define PIC_IRQ_ISP_FRAME				(50 + PIC_IRQ_BASE)
#define PIC_IRQ_ISP_INTER				(51 + PIC_IRQ_BASE)

/* Audio table */
#define PIC_IRQ_SPDIF_IN				(52 + PIC_IRQ_BASE)
#define PIC_IRQ_SPDIF_OUT				(53 + PIC_IRQ_BASE)
#define PIC_IRQ_DMAC1                   (54 + PIC_IRQ_BASE) /* Audio DMAC */

/* Mem int */
#define PIC_IRQ_DDR0					(55 + PIC_IRQ_BASE)
#define PIC_IRQ_DDR1					(56 + PIC_IRQ_BASE)
#define PIC_IRQ_SDRAM_DMAC              (57 + PIC_IRQ_BASE)

/* Cxc int */
#define PIC_IRQ_CM0_CXC                 (58 + PIC_IRQ_BASE)
#define PIC_IRQ_SECDMX                  (59 + PIC_IRQ_BASE) 
#define PIC_IRQ_DSP0					(60 + PIC_IRQ_BASE)
#define PIC_IRQ_DSP1					(61 + PIC_IRQ_BASE)

#define PIC_IRQ_DSP_CXC                 PIC_IRQ_DSP0 
#define PIC_IRQ_IDSP_CXC				PIC_IRQ_DSP1

#define PIC_IRQ_PMU_BLOCK               (62 + PIC_IRQ_BASE)

/* Local int */
#define PIC_IRQ_SPU_TIMER				(63 + PIC_IRQ_BASE)

#endif /* __SILAN_IRQ_H */
