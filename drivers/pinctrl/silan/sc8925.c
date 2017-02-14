#include <linux/module.h>
#include "sc8925_padmux.h"
#include "sc8925_regs.h"

#define PADMUX_DEBUG
#define CONFIG_SILAN_SD
#define CONFIG_SILAN_EMMC
#define CONFIG_CSKY_JTAG_DEBUG
//#define CONFIG_I2C_SILAN_BUS0
//#define CONFIG_I2C_SILAN_BUS1
//#define CONFIG_I2C_SILAN_BUS2
//#define CONFIG_SILAN_HUART_PORT2
//#define CONFIG_SILAN_HUART_PORT3
//#define CONFIG_SILAN_HUART_PORT4
//#define CONFIG_SILAN_HUART_PORT5
#define CONFIG_SILAN_UART_PORT0
//#define ONFIG_SILAN_485_PORT1
//#define CONFIG_SILAN_UART_PORT6
//#define CONFIG_SILAN_ETH0
//#define CONFIG_SILAN_ETH0_RMII_MODE
//#define CONFIG_SILAN_ETH1
//#define CONFIG_SILAN_ETH1_RGMII_MODE
//#define CONFIG_SILAN_LED
//#define CONFIG_SILAN_PMU

enum silan_device_ids{
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

struct silan_module_device
{
	char *dev_name;
	enum silan_device_ids dev_id;
	struct silan_module_pad	pad;
};

static struct silan_module_device silan_module_devices[] = {
	[SILAN_DEV_USB_HOST] = {
		.dev_name = "usb-host",
		.dev_id = SILAN_DEV_USB_HOST,
		.pad = {
			.pad_id = {SILAN_PADMUX_USB_HOST},
			.priority = 0,
			.pin = "GPIO5_16",
		},
	},

	[SILAN_DEV_USB_OTG] = {
		.dev_name = "usb-otg",
		.dev_id = SILAN_DEV_USB_OTG,
		.pad = {
			.pad_id = {SILAN_PADMUX_USB_OTG},
			.priority = 0,
			.pin = "GPIO2_31",
		},
	},

	[SILAN_DEV_GMAC0] = {
		.dev_name = "gmac0",
		.dev_id = SILAN_DEV_GMAC0,
		.pad = {
#ifdef CONFIG_SILAN_ETH0_RGMII_MODE
			.pad_id = {SILAN_PADMUX_ETH0_RGMII},
			.priority = 0,
			.pin =
				"GPIO2_08, GPIO2_09, GPIO2_10, GPIO2_11, GPIO2_12, GPIO2_13,"
				"GPIO2_14, GPIO2_15, GPIO2_16, GPIO2_17, GPIO2_18, GPIO2_19,"
				"GPIO2_20, GPIO2_21",
#elif defined (CONFIG_SILAN_ETH0_RMII_MODE)
			.pad_id = {SILAN_PADMUX_ETH0_RMII},
			.priority = 2,
			.pin =
				"GPIO2_08, GPIO2_09, GPIO2_10, GPIO2_11, GPIO2_12, GPIO2_13,"
				"GPIO2_14, GPIO2_15, GPIO2_16, GPIO2_17",
#elif defined (CONFIG_SILAN_ETH0_MII_GMII_MODE)
			.pad_id = {SILAN_PADMUX_ETH0_MII},
			.priority = 1,
			.pin =
				"GPIO2_08, GPIO2_09, GPIO2_10, GPIO2_11, GPIO2_12, GPIO2_13,"
				"GPIO2_14, GPIO2_15, GPIO2_16, GPIO2_17, GPIO2_18, GPIO2_19,"
				"GPIO2_20, GPIO2_21, GPIO2_22, GPIO2_23, GPIO2_24",
#endif
		},
	},

	[SILAN_DEV_GMAC1] = {
		.dev_name = "gmac1",
		.dev_id = SILAN_DEV_GMAC1,
		.pad = {
#ifdef CONFIG_SILAN_ETH1_RGMII_MODE
			.pad_id = {SILAN_PADMUX_ETH1_RGMII},
			.priority = 0,
			.pin =
				"GPIO2_25, GPIO2_26, GPIO2_27, GPIO2_28, GPIO6_22, GPIO6_23,"
				"GPIO6_24, GPIO6_25, GPIO6_26, GPIO6_27, GPIO6_28, GPIO6_29,"
				"GPIO6_30, GPIO6_31",
#elif defined (CONFIG_SILAN_ETH1_RMII_MODE)
			.pad_id = {SILAN_PADMUX_ETH1_RMII},
			.priority = 2,
			.pin =
				"GPIO6_22, GPIO6_23, GPIO6_24, GPIO6_25, GPIO6_26, GPIO6_27,"
				"GPIO6_28, GPIO6_29, GPIO6_30, GPIO6_31",
#elif defined (CONFIG_SILAN_ETH1_MII_GMII_MODE)
			.pad_id = {SILAN_PADMUX_ETH1_GMII},
			.priority = 1,
			.pin =
				"GPIO2_25, GPIO2_26, GPIO2_27, GPIO2_28, GPIO5_00, GPIO5_01,"
				"GPIO5_02, GPIO5_03, GPIO5_04, GPIO5_05, GPIO5_06, GPIO5_07,"
				"GPIO5_20, GPIO5_21, GPIO5_22, GPIO5_23, GPIO6_22, GPIO6_23,"
				"GPIO6_24, GPIO6_25, GPIO6_26, GPIO6_27, GPIO6_28, GPIO6_29,"
				"GPIO6_30, GPIO6_31",
#endif
		},
	},

	[SILAN_DEV_DMAC0] = {
		.dev_name = "dmac0",
		.dev_id = SILAN_DEV_DMAC0,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_DMAC1] = {
		.dev_name = "dmac1",
		.dev_id = SILAN_DEV_DMAC1,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_I2S_T4] = {
		.dev_name = "i2s-t4",
		.dev_id = SILAN_DEV_I2S_T4,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2S_T4},
			.priority = 0,
			.pin =
				"GPIO5_12, GPIO5_13, GPIO5_14, GPIO5_15, GPIO5_16",
		},
	},

	[SILAN_DEV_I2S_MIC] = {
		.dev_name = "i2s-mic",
		.dev_id = SILAN_DEV_I2S_MIC,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2S_MIC},
			.priority = 0,
			.pin = "GPIO5_20, GPIO5_21, GPIO5_22, GPIO5_23, GPIO5_24, GPIO5_25, GPIO5_26",
		},
	},

	[SILAN_DEV_I2S_HDMI] = {
		.dev_name = "i2s-hdmi",
		.dev_id = SILAN_DEV_I2S_HDMI,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2S_HDMI},
			.priority = 2,
			.pin =
				"GPIO5_12, GPIO5_13, GPIO5_14, GPIO5_15, GPIO5_16, GPIO5_17,"
				"GPIO5_18",
		},
	},

	[SILAN_DEV_UART0] = {
		.dev_name = "uart0",
		.dev_id = SILAN_DEV_UART0,
		.pad = {
			.pad_id = {SILAN_PADMUX_UART0},
			.priority = 0,
			.pin = "GPIO5_08, GPIO5_09",
		},
	},

	[SILAN_DEV_UART1] = {
		.dev_name = "uart1",
		.dev_id = SILAN_DEV_UART1,
		.pad = {
			.pad_id = {SILAN_PADMUX_UART1},
			.priority = 0,
			.pin = "GPIO4_30, GPIO4_31",
		},
	},

	[SILAN_DEV_UART2] = {
		.dev_name = "uart2",
		.dev_id = SILAN_DEV_UART2,
		.pad = {
			.pad_id = {SILAN_PADMUX_UART2},
			.priority = 0,
			.pin = "GPIO2_06, GPIO2_07",
		},
	},

	[SILAN_DEV_UART3] = {
		.dev_name = "uart3",
		.dev_id = SILAN_DEV_UART3,
		.pad = {
			.pad_id = {SILAN_PADMUX_UART3},
			.priority = 0,
			.pin = "GPIO5_12, GPIO5_13",
		},
	},

	[SILAN_DEV_UART4] = {
		.dev_name = "uart4",
		.dev_id = SILAN_DEV_UART4,
		.pad = {
			.pad_id = {SILAN_PADMUX_UART4},
			.priority = 2,
			.pin = "GPIO5_24, GPIO5_25",
		},
	},

	[SILAN_DEV_UART5] = {
		.dev_name = "uart5",
		.dev_id = SILAN_DEV_UART5,
		.pad = {
			.pad_id = {SILAN_PADMUX_UART5},
			.priority = 2,
			.pin = "GPIO5_26, GPIO5_27",
		},
	},

	[SILAN_DEV_UART6] = {
		.dev_name = "uart6",
		.dev_id = SILAN_DEV_UART6,
		.pad = {
			.pad_id = {SILAN_PADMUX_UART6},
			.priority = 2,
			.pin = "GPIO6_20, GPIO6_21",
		},
	},

	[SILAN_DEV_I2C0] = {
		.dev_name = "i2c0",
		.dev_id = SILAN_DEV_I2C0,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2C0},
			.priority = 0,
			.pin = "GPIO2_29, GPIO2_30",
		},
	},

	[SILAN_DEV_I2C1] = {
		.dev_name = "i2c1",
		.dev_id = SILAN_DEV_I2C1,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2C1},
			.priority = 3,
			.pin = "GPIO2_27, GPIO2_28",
		},
	},

	[SILAN_DEV_I2C2] = {
		.dev_name = "i2c2",
		.dev_id = SILAN_DEV_I2C2,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2C2},
			.priority = 0,
			.pin = "GPIO5_10, GPIO5_11",
		},
	},

	[SILAN_DEV_I2C3] = {
		.dev_name = "i2c3",
		.dev_id = SILAN_DEV_I2C3,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2C3},
			.priority = 0,
			.pin = "GPIO4_28, GPIO4_29",
		},
	},

	[SILAN_DEV_I2C4] = {
		.dev_name = "i2c4",
		.dev_id = SILAN_DEV_I2C4,
		.pad = {
			.pad_id = {SILAN_PADMUX_I2C4},
			.priority = 1,
			.pin = "GPIO2_06, GPIO2_07",
		},
	},

	[SILAN_DEV_SPI] = {
		.dev_name = "spi",
		.dev_id = SILAN_DEV_SPI,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = "GPIO3_00, GPIO3_01, GPIO3_02, GPIO3_03, GPIO3_04, GPIO3_05,"
				"GPIO3_06",
		},
	},

	[SILAN_DEV_SPI_NORMAL] = {
		.dev_name = "spi-normal",
		.dev_id = SILAN_DEV_SPI_NORMAL,
		.pad = {
			.pad_id = {SILAN_PADMUX_SPI_NORMAL},
			.priority = 0,
			.pin = "GPIO5_24, GPIO5_25, GPIO5_26, GPIO5_27",
		},
	},

	[SILAN_DEV_SSP] = {
		.dev_name = "ssp",
		.dev_id = SILAN_DEV_SSP,
		.pad = {
			.pad_id = {SILAN_PADMUX_SSP},
			.priority = 1,
			.pin = "GPIO5_20, GPIO5_21, GPIO5_22, GPIO5_23",
		},
	},

	[SILAN_DEV_SPDIF_IN] = {
		.dev_name = "spdif-in",
		.dev_id = SILAN_DEV_SPDIF_IN,
		.pad = {
			.pad_id = {SILAN_PADMUX_SPDIF_IN},
			.priority = 0,
			.pin = "GPIO5_27",
		},
	},

	[SILAN_DEV_SPDIF_OUT] = {
		.dev_name = "spdif-out",
		.dev_id = SILAN_DEV_SPDIF_OUT,
		.pad = {
			.pad_id = {SILAN_PADMUX_SPDIF_OUT},
			.priority = 0,
			.pin = "GPIO5_19",
		},
	},

	[SILAN_DEV_HDMI] = {
		.dev_name = "hdmi",
		.dev_id = SILAN_DEV_HDMI,
		.pad = {
			.pad_id = {SILAN_PADMUX_HDMI},
			.priority = 1,
			.pin = "GPIO5_28, GPIO5_29, GPIO5_30, GPIO5_31",
		},
	},

	[SILAN_DEV_CAN] = {
		.dev_name = "can",
		.dev_id = SILAN_DEV_CAN,
		.pad = {
			.pad_id = {SILAN_PADMUX_CAN},
			.priority = 1,
			.pin = "GPIO2_29, GPIO2_30",
		},
	},

	[SILAN_DEV_SD] = {
		.dev_name = "sd",
		.dev_id = SILAN_DEV_SD,
		.pad = {
			.pad_id = {SILAN_PADMUX_SD},
			.priority = 0,
			.pin =
				"GPIO1_30, GPIO1_31, GPIO2_00, GPIO2_01,GPIO2_02, GPIO2_03,"
				"GPIO2_04, GPIO2_05",
		},
	},

	[SILAN_DEV_EMMC] = {
		.dev_name = "emmc",
		.dev_id = SILAN_DEV_EMMC,
		.pad = {
			.pad_id = {SILAN_PADMUX_EMMC},
			.priority = 0,
			.pin =
				"GPIO1_00, GPIO1_01, GPIO1_02, GPIO1_03,GPIO1_04, GPIO1_05,"
				"GPIO1_06, GPIO1_07, GPIO1_08, GPIO1_09",
		},
	},

	[SILAN_DEV_SDIO] = {
		.dev_name = "sdio",
		.dev_id = SILAN_DEV_SDIO,
		.pad = {
			.pad_id = {SILAN_PADMUX_SDIO},
			.priority = 2,
			.pin =
				"GPIO2_06, GPIO2_07, GPIO2_08, GPIO2_13,GPIO2_14, GPIO2_18,"
				"GPIO2_19, GPIO2_23, GPIO2_24",
		},
	},

	[SILAN_DEV_SCI] = {
		.dev_name = "sci",
		.dev_id = SILAN_DEV_SCI,
		.pad = {
			.pad_id = {SILAN_PADMUX_SCI},
			.priority = 1,
			.pin = "GPIO1_10, GPIO1_11, GPIO1_12, GPIO1_13, GPIO1_14",
		},
	},

	[SILAN_DEV_PWM] = {
		.dev_name = "pwm",
		.dev_id = SILAN_DEV_PWM,
		.pad = {
#ifdef CONFIG_SILAN_PWM0
			.pad_id = {SILAN_PADMUX_PWM0},
			.priority = 1,
			.pin = "GPIO5_19",
#elif defined CONFIG_SILAN_PWM1
			.pad_id = {SILAN_PADMUX_PWM1},
			.priority = 2,
			.pin = "GPIO4_28",
#elif defined CONFIG_SILAN_PWM2
			.pad_id = {SILAN_PADMUX_PWM2},
			.priority = 2,
			.pin = "GPIO4_29",
#elif defined CONFIG_SILAN_PWM3
			.pad_id = {SILAN_PADMUX_PWM3},
			.priority = 2,
			.pin = "GPIO5_17",
#endif
		},
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},

	},

	[SILAN_DEV_SDRAM] = {
		.dev_name = "sdram",
		.dev_id = SILAN_DEV_SDRAM,
		.pad = {
			.pad_id = {SILAN_PADMUX_SDRAM},
			.priority = 5,
			.pin =
				"GPIO2_29, GPIO2_30, GPIO4_00, GPIO4_01, GPIO4_02, GPIO4_03,"
				"GPIO4_04, GPIO4_05, GPIO4_06, GPIO4_07, GPIO4_08, GPIO4_09,"
				"GPIO4_10, GPIO4_11, GPIO4_12, GPIO4_13, GPIO4_14, GPIO4_15,"
				"GPIO4_16, GPIO4_17, GPIO4_18, GPIO4_19, GPIO4_20, GPIO4_21,"
				"GPIO4_22, GPIO4_23, GPIO4_24, GPIO4_25, GPIO4_26, GPIO4_27,"
				"GPIO4_28, GPIO4_29, GPIO4_30, GPIO4_31, GPIO5_11, GPIO5_12,"
				"GPIO5_13, GPIO6_00, GPIO6_01, GPIO6_02, GPIO6_03, GPIO6_04,"
				"GPIO6_05, GPIO6_06, GPIO6_07, GPIO6_08, GPIO6_09, GPIO6_10,"
				"GPIO6_11, GPIO6_12, GPIO6_13, GPIO6_14, GPIO6_15, GPIO6_16,"
				"GPIO6_17, GPIO6_18, GPIO6_19, GPIO6_20, GPIO6_21",
		},
	},

	[SILAN_DEV_TIMER] = {
		.dev_name = "timer",
		.dev_id = SILAN_DEV_TIMER,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_RTC] = {
		.dev_name = "rtc",
		.dev_id = SILAN_DEV_RTC,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_PMU] = {
		.dev_name = "pmu",
		.dev_id = SILAN_DEV_PMU,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_GPU] = {
		.dev_name = "gpu",
		.dev_id = SILAN_DEV_GPU,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},


	[SILAN_DEV_VPU0] = {
		.dev_name = "vpu0",
		.dev_id = SILAN_DEV_VPU0,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_VPU1] = {
		.dev_name = "vpu1",
		.dev_id = SILAN_DEV_VPU1,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_JPU] = {
		.dev_name = "jpu",
		.dev_id = SILAN_DEV_JPU,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_VJB] = {
		.dev_name = "vjb",
		.dev_id = SILAN_DEV_VJB,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_VPP] = {
		.dev_name = "vpp",
		.dev_id = SILAN_DEV_VPP,
		.pad = {
#ifdef CONFIG_FB_SILAN_TCON
			.pad_id = {SILAN_PADMUX_TCON},
			.priority = 0,
			.pin =
				"GPIO4_00, GPIO4_01, GPIO4_02, GPIO4_03, GPIO4_04, GPIO4_05,"
				"GPIO4_06, GPIO4_07, GPIO4_08, GPIO4_09, GPIO4_10, GPIO4_11,"
				"GPIO4_12, GPIO4_13, GPIO4_14, GPIO4_15, GPIO4_16, GPIO4_17,"
				"GPIO4_18, GPIO4_19, GPIO4_20, GPIO4_21, GPIO4_22, GPIO4_23,"
				"GPIO4_24, GPIO4_25, GPIO4_26, GPIO4_27",
#elif defined CONFIG_FB_SILAN_CAT6612
			.pad_id = SILAN_PADMUX_HDMI_DIG,
			.priority = 1,
			.pin =
				"GPIO4_00, GPIO4_01, GPIO4_02, GPIO4_03, GPIO4_04, GPIO4_05,"
				"GPIO4_06, GPIO4_07, GPIO4_08, GPIO4_09, GPIO4_10, GPIO4_11,"
				"GPIO4_12, GPIO4_13, GPIO4_14, GPIO2_15, GPIO4_16, GPIO4_17,"
				"GPIO4_18, GPIO4_19, GPIO4_20, GPIO2_21, GPIO4_22, GPIO4_23,"
				"GPIO4_24, GPIO4_25, GPIO4_26, GPIO2_27",
#elif defined CONFIG_FB_SILAN_BT656
			.pad_id = SILAN_PADMUX_BT656,
			.priority = 3,
			.pin =
				"GPIO4_00, GPIO4_01, GPIO4_02, GPIO4_03, GPIO4_04, GPIO4_05,"
				"GPIO4_06, GPIO4_07, GPIO4_08",
#elif defined CONFIG_FB_SILAN_BT601
			.pad_id = SILAN_PADMUX_BT601,
			.priority = 2,
			.pin =
				"GPIO4_00, GPIO4_01, GPIO4_04, GPIO4_05, GPIO4_06, GPIO4_07,"
				"GPIO4_08, GPIO4_09, GPIO4_10, GPIO4_11",
#elif defined CONFIG_FB_SILAN_BT1120
			.pad_id = SILAN_PADMUX_BT1120,
			.priority = 4,
			.pin =
				"GPIO4_00, GPIO4_01, GPIO4_02, GPIO4_03, GPIO4_04, GPIO4_05,"
				"GPIO4_06, GPIO4_07, GPIO4_08, GPIO4_09, GPIO4_10, GPIO4_11,"
				"GPIO4_12, GPIO4_13, GPIO4_14, GPIO2_15, GPIO4_16, GPIO4_17,"
				"GPIO4_18, GPIO4_19, GPIO4_20, GPIO2_21, GPIO4_22, GPIO4_23,"
				"GPIO4_24, GPIO4_25, GPIO4_26, GPIO2_27, GPIO4_28, GPIO4_29,"
				"GPIO4_30",
#endif
		},
	},


	[SILAN_DEV_DSP0] = {
		.dev_name = "dsp0",
		.dev_id = SILAN_DEV_DSP0,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_DSP1] = {
		.dev_name = "dsp1",
		.dev_id = SILAN_DEV_DSP1,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_VIU] = {
		.dev_name = "viu",
		.dev_id = SILAN_DEV_VIU,
		.pad = {
#if defined CONFIG_SILAN_VIU || defined CONFIG_VIDEO_SILAN_VIU
			.pad_id = {SILAN_PADMUX_VIU_PORT0, SILAN_PADMUX_VIU_PORT1, SILAN_PADMUX_VIU_PORT2, SILAN_PADMUX_VIU_PORT3},
			.priority = 0,
			.pin =
				/* viu port 0*/
				"GPIO1_10, GPIO1_11, GPIO1_12, GPIO1_13, GPIO1_14, GPIO1_15,"
				"GPIO1_16, GPIO1_17, GPIO1_18, GPIO1_19, GPIO1_20"
				/* viu port 1*/
				"GPIO1_21, GPIO1_22, GPIO1_23, GPIO1_24, GPIO1_25, GPIO1_26,"
				"GPIO1_27, GPIO1_28, GPIO1_29"
				/* viu port 2*/
				"GPIO6_00, GPIO6_01, GPIO6_02, GPIO6_03, GPIO6_04, GPIO6_05,"
				"GPIO6_06, GPIO6_07, GPIO6_08, GPIO6_09, GPIO6_10"
				/* viu port 3*/
				"GPIO6_11, GPIO6_12, GPIO6_13, GPIO6_14, GPIO6_15, GPIO6_16,"
				"GPIO6_17, GPIO6_18, GPIO6_19",
#endif
#ifdef CONFIG_VIDEO_SILAN_VIU_SHITENG
			.pad_id = {SILAN_PADMUX_VIU_PORT2},
			.priority = 0,
			.pin =
				"GPIO6_00, GPIO6_01, GPIO6_02, GPIO6_03, GPIO6_04, GPIO6_05,"
				"GPIO6_06, GPIO6_07, GPIO6_08, GPIO6_09, GPIO6_10",
#endif
		},
	},

	[SILAN_DEV_VPREP] = {
		.dev_name = "vprep",
		.dev_id = SILAN_DEV_VPREP,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_MPU] = {
		.dev_name = "mpu",
		.dev_id = SILAN_DEV_MPU,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_VENC] = {
		.dev_name = "venc",
		.dev_id = SILAN_DEV_VENC,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_FDIP] = {
		.dev_name = "fdip",
		.dev_id = SILAN_DEV_FDIP,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_SECDMX] = {
		.dev_name = "secdmx",
		.dev_id = SILAN_DEV_SECDMX,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},
	/* pad should open in padmux.c */
	[SILAN_DEV_GPIO1] = {
		.dev_name = "gpio1",
		.dev_id = SILAN_DEV_GPIO1,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_GPIO2] = {
		.dev_name = "gpio2",
		.dev_id = SILAN_DEV_GPIO2,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_GPIO3] = {
		.dev_name = "gpio3",
		.dev_id = SILAN_DEV_GPIO3,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_GPIO4] = {
		.dev_name = "gpio4",
		.dev_id = SILAN_DEV_GPIO4,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_GPIO5] = {
		.dev_name = "gpio5",
		.dev_id = SILAN_DEV_GPIO5,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_GPIO6] = {
		.dev_name = "gpio6",
		.dev_id = SILAN_DEV_GPIO6,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_DXB] = {
		.dev_name = "dxb",
		.dev_id = SILAN_DEV_DXB,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_PXB] = {
		.dev_name = "pxb",
		.dev_id = SILAN_DEV_PXB,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_CXB] = {
		.dev_name = "cxb",
		.dev_id = SILAN_DEV_CXB,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_LSP] = {
		.dev_name = "lsp",
		.dev_id = SILAN_DEV_LSP,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_JTAG] = {
		.dev_name = "jtag",
		.dev_id = SILAN_DEV_JTAG,
		.pad = {
			.pad_id = {0},
			.priority = 2,
			.pin = "GPIO5_19, GPIO5_20, GPIO5_21, GPIO5_22, GPIO5_23",
		},
	},

	[SILAN_DEV_WATCHDOG] = {
		.dev_name = "watchdog",
		.dev_id = SILAN_DEV_WATCHDOG,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_ISP] = {
		.dev_name = "isp",
		.dev_id = SILAN_DEV_ISP,
		.pad = {
			.pad_id = {SILAN_PADMUX_ISP},
			.priority = 0,
			.pin =
				/* pclk */
				"GPIO1_10"
				/* D0 - D7 */
				"GPIO1_11, GPIO1_12, GPIO1_13, GPIO1_14, GPIO1_15,"
				"GPIO1_16, GPIO1_17, GPIO1_18"
				/* Hsync*/
				"GPIO1_19"
				/* Vsync */
				"GPIO1_20"
				/* D8 - D15 */
				"GPIO1_22, GPIO1_23, GPIO1_24, GPIO1_25, GPIO1_26,"
				"GPIO1_27, GPIO1_28, GPIO1_29",
		},
	},

	[SILAN_DEV_CCU] = {
		.dev_name = "ccu",
		.dev_id = SILAN_DEV_CCU,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_CM0] = {
		.dev_name = "cm0",
		.dev_id = SILAN_DEV_CM0,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_IVS_ACLK] = {
		.dev_name = "ivs_aclk",
		.dev_id = SILAN_DEV_IVS_ACLK,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},

	[SILAN_DEV_ADC] = {
		.dev_name = "adc",
		.dev_id = SILAN_DEV_ADC,
		.pad = {
			.pad_id = {0},
			.priority = 0,
			.pin = NULL,
		},
	},
};


//#define PADMUX_DEBUG

/* totle pin information defined*/
static struct silan_pin_info silan_pin_infos[PIN_NUM];

static inline u32 sl_readl(unsigned long reg)
{
	return (*(volatile u32 *)reg);
}

void static inline sl_writel(u32 val, unsigned long reg)
{
	*(volatile u32 *)(reg) = val;
}

int silan_padmux_ctrl(enum silan_pad_ids pad_id, PAD_ON_OFF flag)
{
	unsigned int value;
	unsigned reg_offset, bit_offset;

	if (pad_id < SILAN_PADMUX_START || pad_id > SILAN_PADMUX_END)
		return -1;

	reg_offset = pad_id / 32;
	bit_offset = pad_id % 32;
	printk("%s: %x, %d, %d.\n", __func__, SILAN_SYS_REG0, reg_offset, bit_offset);
	value = sl_readl(SILAN_SYS_REG0 + (4 * reg_offset));

	if (flag == PAD_OFF)
		value &= ~(1 << bit_offset);
	else if (flag == PAD_ON)
		value |= (1 << bit_offset);
	else
		return -1;

	sl_writel(value, SILAN_SYS_REG0 + (4 * reg_offset));

	return 0;
}


int silan_gpiobitpadmux_ctrl(enum silan_gpio_ids gpio_id, PAD_ON_OFF flag)
{
	unsigned int value;
	unsigned reg_offset, bit_offset;

	if(gpio_id < SILAN_GPIO_START || gpio_id > SILAN_GPIO_END)
		return -1;

	reg_offset = gpio_id / 32;
	bit_offset = gpio_id % 32;

	if (5 == reg_offset)
		reg_offset += 2;

	value = sl_readl(SILAN_SYS_REG36 + (4 * reg_offset));

	if (flag == PAD_OFF)
		value &= ~(1 << bit_offset);
	else if (flag == PAD_ON)
		value |= (1 << bit_offset);
	else
		return -1;

	sl_writel(value, SILAN_SYS_REG36 + (4 * reg_offset));

	return 0;
}



static int gpio_atoi(char *p)
{
	int pin_num;

	if(*p == 'G' && *(p + 1) == 'P' && *(p + 2)  == 'I' && *(p + 3) == 'O')
		pin_num = (simple_strtol(p + 4, NULL, 10) - 1) * 32 + simple_strtol(p + 6, NULL, 10);
	else
		return -1;
	if(pin_num < 0 || pin_num > 192)
		return -1;
	return pin_num;
}

static void module_pad_mark(struct silan_module_pad *pad)
{
	char *p = pad->pin;
	int pin_num;

	if (NULL == p)
		return;

	while (*p != '\0') {
		 pin_num = gpio_atoi(p);
		 if (-1 != pin_num) {
			 if(silan_pin_infos[pin_num].count >= USED_BY_PADS_SIZE) {
				 printk("ERROR: pin GPIO%d_%d%d marked times more than its pad info table size\n",
						 pin_num / 32 + 1, pin_num % 32 / 10, pin_num %32 % 10);
				 p++;
				 continue;
			 } else {
				 silan_pin_infos[pin_num].used_by_pad[silan_pin_infos[pin_num].count] =
							container_of(pad, struct silan_module_device, pad)->dev_name;
				 silan_pin_infos[pin_num].count++;
			 }
		 }
		 p++;
	}

}

static int device_pad_checkout(struct silan_module_pad *pad)
{
	char *p = pad->pin;
	int pin_num;

	if (NULL == p)
		return -1;
	while (*p != '\0') {
		 pin_num = gpio_atoi(p);
		 if (-1 != pin_num) {
			 if (0 != strcmp(silan_pin_infos[pin_num].used_by_pad[0],
						 container_of(pad, struct silan_module_device, pad)->dev_name)) {
				 printk("ERROR: pin GPIO%d_%d%d of device %s is used by device %s\n",
						 pin_num / 32 + 1, pin_num % 32 / 10, pin_num %32 % 10,
						 container_of(pad, struct silan_module_device, pad)->dev_name,
						 silan_pin_infos[pin_num].used_by_pad[0]);
				 return -1;
			 }
		}
		p++;
	}

	return 0;
}

static void gpio_pad_mark(unsigned int pin_num)
{
	sprintf(silan_pin_infos[pin_num].gpio_name,"GPIO%d_%d%d", pin_num / 32 + 1,
			pin_num % 32 / 10, pin_num %32 % 10);

	if(silan_pin_infos[pin_num].count >= USED_BY_PADS_SIZE) {
		printk("ERROR: pin number %d marked times more than its pad info table size\n", pin_num);
		return;
	} else {
		silan_pin_infos[pin_num].used_by_pad[silan_pin_infos[pin_num].count] =  silan_pin_infos[pin_num].gpio_name;
		silan_pin_infos[pin_num].count++;
	}

}


static int gpio_pad_checkout(unsigned int pin_num)
{

	if(0 == strcmp(silan_pin_infos[pin_num].used_by_pad[0], silan_pin_infos[pin_num].gpio_name))
		return 0;
	else {
		printk("ERROR: pin GPIO%d_%d%d is used by device %s\n",
				pin_num / 32 + 1, pin_num % 32 / 10, pin_num %32 % 10,
				silan_pin_infos[pin_num].used_by_pad[0]);
		return -1;
	}
}
static enum silan_device_ids device_pad_open[] =
{
#ifdef CONFIG_SND_SUV_SOC_APWM
	SILAN_DEV_APWM0,
	SILAN_DEV_APWM1,
#endif

#ifdef CONFIG_SND_SUV_SOC_I2S_HDMI
	SILAN_DEV_I2S_HDMI,
#endif

#ifdef CONFIG_SND_SUV_SOC_SPDIF_IN
    SILAN_DEV_SPDIF_IN,
#endif

#ifdef CONFIG_SND_SUV_SOC_SPDIF_OUT
    SILAN_DEV_SPDIF_OUT,
#endif

#ifdef CONFIG_SND_SUV_SOC_I2S
	SILAN_DEV_I2S_T4,
	SILAN_DEV_I2S_MIC,
#endif

#ifdef CONFIG_SILAN_PWM
	SILAN_DEV_PWM,
#endif

#ifdef CONFIG_SMART_CARD
	SILAN_DEV_SCI,
#endif

#if defined CONFIG_SILAN_SD
	SILAN_DEV_SD,
#endif
#if defined CONFIG_SILAN_EMMC
	SILAN_DEV_EMMC,
#endif
#if defined CONFIG_SILAN_SDIO
	SILAN_DEV_SDIO,
#endif

#ifdef CONFIG_I2C_SILAN_BUS0
	SILAN_DEV_I2C0,
#endif
#ifdef CONFIG_I2C_SILAN_BUS1
	SILAN_DEV_I2C1,
#endif
#ifdef CONFIG_I2C_SILAN_BUS2
	SILAN_DEV_I2C2,
#endif
#ifdef CONFIG_I2C_SILAN_BUS3
	SILAN_DEV_I2C3,
#endif
#if defined CONFIG_I2C_SILAN_BUS4 || defined CONFIG_SILAN_I2C_SLAVE
	SILAN_DEV_I2C4,
#endif
/**************NORMAL UART****************/
#ifdef CONFIG_SILAN_UART_PORT0
	SILAN_DEV_UART0,
#endif
#if defined CONFIG_SILAN_UART_PORT1 || defined CONFIG_SILAN_485_PORT1
	SILAN_DEV_UART1,
#endif
#if defined CONFIG_SILAN_HUART_PORT2 || defined CONFIG_SILAN_485_PORT2
	SILAN_DEV_UART2,
#endif
#ifdef CONFIG_SILAN_HUART_PORT3
	SILAN_DEV_UART3,
#endif
#ifdef CONFIG_SILAN_HUART_PORT4
	SILAN_DEV_UART4,
#endif
#ifdef CONFIG_SILAN_HUART_PORT5
	SILAN_DEV_UART5,
#endif
#ifdef CONFIG_SILAN_UART_PORT6
	SILAN_DEV_UART6,
#endif

#ifdef CONFIG_FB_SILAN
	SILAN_DEV_VPP,
#endif

#ifdef CONFIG_FB_SILAN_HDMI
	SILAN_DEV_HDMI,
#endif

#if defined CONFIG_SILAN_VIU || defined CONFIG_VIDEO_SILAN_VIU || defined CONFIG_VIDEO_SILAN_VIU_SHITENG
	SILAN_DEV_VIU,
#endif

#ifdef CONFIG_SILAN_ETH0
	SILAN_DEV_GMAC0,
#endif

#ifdef CONFIG_SILAN_ETH1
	SILAN_DEV_GMAC1,
#endif

#ifdef CONFIG_SILAN_SDRAM
	SILAN_DEV_SDRAM,
#endif

#ifdef CONFIG_CSKY_JTAG_DEBUG
	SILAN_DEV_JTAG,
#endif

	SILAN_DEV_ISP,
};


static enum silan_gpio_ids gpio_pad_open[] =
{
#ifdef CONFIG_SILAN_TW2866
	SILAN_GPIO4_04,					/* TW2866_RESET */
#endif

#ifdef CONFIG_SILAN_CD4052
	SILAN_GPIO2_27,					/* CD4052_IO */
#endif

#ifdef CONFIG_SILAN_DVDRESET
	SILAN_GPIO1_08,					/* DVD_RESET */
#endif

#ifdef CONFIG_SPI_SILAN_GPIO
	SILAN_GPIO5_28,					/* SPI_MOSI */
	SILAN_GPIO5_29,					/* SPI_MISO */
	SILAN_GPIO5_30,					/* SPI_SCK  */
	SILAN_GPIO5_31,					/* SPI_CHIPSELECT */
#endif

#ifdef CONFIG_I2C_GPIO
	SILAN_GPIO2_29,
	SILAN_GPIO2_30,
#endif

#ifdef CONFIG_SILAN_485_PORT1
	SILAN_GPIO4_20,
	SILAN_GPIO4_21,
#endif

	SILAN_GPIO2_31,		/* USB OTG VBUS DRIVE */
	SILAN_GPIO1_30,		/* LED DRIVE */
};


void  __init sc8925_pinctrl_init(void)
{
	int i, j, val;
	struct silan_module_pad	*pad;

	/* mark the device pad in pin information*/
	for (i = 0; i < ARRAY_SIZE(device_pad_open); i++) {
		if (device_pad_open[i] >= ARRAY_SIZE(silan_module_devices)) {
			printk("ERROR: wrong device id in module devices list");
			continue;
		}
		pad = &silan_module_devices[device_pad_open[i]].pad;
		module_pad_mark(pad);
	};

	/* mark the gpio pad in pin information*/
	for (i = 0; i < ARRAY_SIZE(gpio_pad_open); i++)
		gpio_pad_mark(gpio_pad_open[i]);

	/* checkout if the pins is only used by this device pad and
	 * then set the bit in pad register */
	for (i = 0; i < ARRAY_SIZE(device_pad_open); i++) {
		if (device_pad_open[i] >= ARRAY_SIZE(silan_module_devices)) {
			printk("ERROR: wrong device id in module devices list");
			continue;
		}
		pad = &silan_module_devices[device_pad_open[i]].pad;
		val = device_pad_checkout(pad);
		if (0 == val) {
			for (j = 0; j < PAD_ID_NUM; j++) {
				if (pad->pad_id[j] == 0)
					continue;
				silan_padmux_ctrl(pad->pad_id[j], PAD_ON);
				printk("module pad open: %s\n", container_of(pad, struct silan_module_device, pad)->dev_name);
			}
		}
	}
#if 1
	/* checkout if the pins is only used by this gpio and then
	 * set the bit in gpio pad register */
	for(i = 0; i < ARRAY_SIZE(gpio_pad_open); i++) {
		val = gpio_pad_checkout(gpio_pad_open[i]);
		if(0 == val) {
			silan_gpiobitpadmux_ctrl(gpio_pad_open[i], PAD_ON);
			printk("gpio pad open: GPIO%d_%d%d\n", gpio_pad_open[i]/32 + 1,
					gpio_pad_open[i] % 32 / 10, gpio_pad_open[i] % 32 % 10);
		}
	}
#endif
#ifdef PADMUX_DEBUG
	for(i = 0; i < PIN_NUM; i++)
	{
		printk("%d_%d%d:%9s %9s %9s %9s %9s %9s %9s\n",
				i / 32 + 1,
				(i % 32) / 10,
				i % 32 % 10,
				silan_pin_infos[i].used_by_pad[0],
				silan_pin_infos[i].used_by_pad[1],
				silan_pin_infos[i].used_by_pad[2],
				silan_pin_infos[i].used_by_pad[3],
				silan_pin_infos[i].used_by_pad[4],
				silan_pin_infos[i].used_by_pad[5],
				silan_pin_infos[i].used_by_pad[6]);
	}
#endif
}

