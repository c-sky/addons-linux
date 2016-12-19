/*
 * Register address definition
 */
#ifndef _SILAN_REGS_H
#define _SILAN_REGS_H

/* NOTE: SHOULD SYNCHRONIZED UPDATING WITH KERNEL */

/* System  Control Register */
#define SILAN_SYS_REG0			SILAN_CR_BASE
#define SILAN_SYS_REG1			(SILAN_CR_BASE + 0x4)
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

#define SILAN_GPIOPAD_BASE		(SILAN_CR_BASE + 0x90)


/* Physical Address Of System Control Register */
#define SILAN_SYS_PHY_REG0			SILAN_CR_PHY_BASE
#define SILAN_SYS_PHY_REG1			(SILAN_CR_PHY_BASE + 0x4)
#define SILAN_SYS_PHY_REG2          (SILAN_CR_PHY_BASE + 0x8)
#define SILAN_SYS_PHY_REG3          (SILAN_CR_PHY_BASE + 0xc)
#define SILAN_SYS_PHY_REG4          (SILAN_CR_PHY_BASE + 0x10)
#define SILAN_SYS_PHY_REG5          (SILAN_CR_PHY_BASE + 0x14)
#define SILAN_SYS_PHY_REG6          (SILAN_CR_PHY_BASE + 0x18)
#define SILAN_SYS_PHY_REG7          (SILAN_CR_PHY_BASE + 0x1c)
#define SILAN_SYS_PHY_REG8          (SILAN_CR_PHY_BASE + 0x20)
#define SILAN_SYS_PHY_REG9          (SILAN_CR_PHY_BASE + 0x24)
#define SILAN_SYS_PHY_REG10         (SILAN_CR_PHY_BASE + 0x28)
#define SILAN_SYS_PHY_REG11         (SILAN_CR_PHY_BASE + 0x2c)
#define SILAN_SYS_PHY_REG12         (SILAN_CR_PHY_BASE + 0x30)
#define SILAN_SYS_PHY_REG13         (SILAN_CR_PHY_BASE + 0x34)
#define SILAN_SYS_PHY_REG14         (SILAN_CR_PHY_BASE + 0x38)
#define SILAN_SYS_PHY_REG15         (SILAN_CR_PHY_BASE + 0x3c)
#define SILAN_SYS_PHY_REG16         (SILAN_CR_PHY_BASE + 0x40)
#define SILAN_SYS_PHY_REG17         (SILAN_CR_PHY_BASE + 0x44)
#define SILAN_SYS_PHY_REG18         (SILAN_CR_PHY_BASE + 0x48)
#define SILAN_SYS_PHY_REG19         (SILAN_CR_PHY_BASE + 0x4c)
#define SILAN_SYS_PHY_REG20         (SILAN_CR_PHY_BASE + 0x50)
#define SILAN_SYS_PHY_REG21         (SILAN_CR_PHY_BASE + 0x54)
#define SILAN_SYS_PHY_REG22         (SILAN_CR_PHY_BASE + 0x58)
#define SILAN_SYS_PHY_REG23         (SILAN_CR_PHY_BASE + 0x5c)
#define SILAN_SYS_PHY_REG24         (SILAN_CR_PHY_BASE + 0x60)
#define SILAN_SYS_PHY_REG25         (SILAN_CR_PHY_BASE + 0x64)
#define SILAN_SYS_PHY_REG26         (SILAN_CR_PHY_BASE + 0x68)
#define SILAN_SYS_PHY_REG27         (SILAN_CR_PHY_BASE + 0x6c)
#define SILAN_SYS_PHY_REG28         (SILAN_CR_PHY_BASE + 0x70)
#define SILAN_SYS_PHY_REG29         (SILAN_CR_PHY_BASE + 0x74)
#define SILAN_SYS_PHY_REG30         (SILAN_CR_PHY_BASE + 0x78)
#define SILAN_SYS_PHY_REG31         (SILAN_CR_PHY_BASE + 0x7c)
#define SILAN_SYS_PHY_REG32         (SILAN_CR_PHY_BASE + 0x80)
#define SILAN_SYS_PHY_REG33         (SILAN_CR_PHY_BASE + 0x84)
#define SILAN_SYS_PHY_REG34         (SILAN_CR_PHY_BASE + 0x88)
#define SILAN_SYS_PHY_REG35         (SILAN_CR_PHY_BASE + 0x8c)
#define SILAN_SYS_PHY_REG36         (SILAN_CR_PHY_BASE + 0x90)
#define SILAN_SYS_PHY_REG37         (SILAN_CR_PHY_BASE + 0x94)
#define SILAN_SYS_PHY_REG38         (SILAN_CR_PHY_BASE + 0x98)
#define SILAN_SYS_PHY_REG39         (SILAN_CR_PHY_BASE + 0x9c)
#define SILAN_SYS_PHY_REG40         (SILAN_CR_PHY_BASE + 0xa0)
#define SILAN_SYS_PHY_REG41         (SILAN_CR_PHY_BASE + 0xa4)
#define SILAN_SYS_PHY_REG42         (SILAN_CR_PHY_BASE + 0xa8)
#define SILAN_SYS_PHY_REG43         (SILAN_CR_PHY_BASE + 0xac)
#define SILAN_SYS_PHY_REG44         (SILAN_CR_PHY_BASE + 0xb0)
#define SILAN_SYS_PHY_REG45         (SILAN_CR_PHY_BASE + 0xb4)
#define SILAN_GPIOPAD_PHY_BASE		(SILAN_CR_PHY_BASE + 0x90)


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

#define SILAN_PMU_PD_CPU_SW_CNT			(SILAN_PMU_BASE + 0x30)
#define SILAN_PMU_PD_CPU_INTV_CNT		(SILAN_PMU_BASE + 0x34)
#define SILAN_PMU_PD_RTC_CFG			(SILAN_PMU_BASE + 0x40)
#define SILAN_PMU_PD_REQ				(SILAN_PMU_BASE + 0x50)
#define SILAN_PMU_PD_ISO_EN_SET			(SILAN_PMU_BASE + 0x54)
#define SILAN_PMU_PD_ISO_EN_CLR			(SILAN_PMU_BASE + 0x58)
#define SILAN_PMU_PD_SW_EN_SET			(SILAN_PMU_BASE + 0x5c)
#define SILAN_PMU_PD_SW_EN_CLR			(SILAN_PMU_BASE + 0x60)
#define SILAN_PMU_PD_SW_ACK				(SILAN_PMU_BASE + 0x64)
#define SILAN_PMU_PD_SW_CNT_EN			(SILAN_PMU_BASE + 0x68)
#define SILAN_PMU_PD_FSM_RST			(SILAN_PMU_BASE + 0x6c)
#define SILAN_PMU_PD_FSM_ST0			(SILAN_PMU_BASE + 0x70)
#define SILAN_PMU_PD_FSM_ST1			(SILAN_PMU_BASE + 0x74)
#define SILAN_PMU_PD_INT_ST				(SILAN_PMU_BASE + 0x78)
#define SILAN_PMU_PD_INT_MASK			(SILAN_PMU_BASE + 0x7c)
#define SILAN_PMU_PD_BLK_SW_CNT(n)		(SILAN_PMU_BASE + 0x80 + 8*n)
#define SILAN_PMU_PD_BLK_INTV_CNT(n)	(SILAN_PMU_BASE + 0x80 + 8*n)

/* Physical addresses */
#define SILAN_PMU_PHY_CFG			SILAN_PMU_PHY_BASE
#define SILAN_PMU_PHY_INT_CFG		(SILAN_PMU_PHY_BASE + 0x14)

/*
 * Interrupt controller
 */
/* Physical addresses */
#define SILAN_ICTL_PHY_NIERL        (SILAN_ICTL_PHY_BASE + 0x10)
#define SILAN_ICTL_PHY_NIERH        (SILAN_ICTL_PHY_BASE + 0x28)

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

/*
 * Boot RAM
 */
/* Save point of the resume entry */
#define SILAN_BOOTRAM_RESUME_ENTRY  (SILAN_BOOTRAM_BASE + SILAN_BOOTRAM_SIZE + 1 - 512)


#endif /* _SILAN_REGS_H */

