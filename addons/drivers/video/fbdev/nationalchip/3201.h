#ifndef _3201_H_
#define _3201_H_

int gx3201_fbinit(struct fb_info *info);

enum cvbs_mode_enum {
	G_PAL,
	G_PAL_M,
	G_PAL_N,
	G_PAL_NC,
	G_NTSC_M,
	G_NTSC_443,
};

enum ypbpr_hdmi_mode_enum {
	G_YPBPR_HDMI_480I,
	G_YPBPR_HDMI_480P,
	G_YPBPR_HDMI_576I,
	G_YPBPR_HDMI_576P,
	G_YPBPR_HDMI_720P_50HZ,
	G_YPBPR_HDMI_720P_60HZ,
	G_YPBPR_HDMI_1080I_50HZ,
	G_YPBPR_HDMI_1080I_60HZ,
	G_YPBPR_HDMI_1080P_50HZ,
	G_YPBPR_HDMI_1080P_60HZ,
};

struct config_regs {
        unsigned int   cfg_mpeg_cold_reset;
        unsigned int   cfg_mpeg_cold_reset_set;
        unsigned int   cfg_mpeg_cold_reset_clr;
        unsigned int   cfg_mpeg_hot_reset;           /* 0x0c */
        unsigned int   cfg_mpeg_hot_reset_set;
        unsigned int   cfg_mpeg_hot_reset_clr;
        unsigned int   cfg_mepg_clk_inhibit;
        unsigned int   cfg_mepg_clk_inhibit_set;
        unsigned int   cfg_mepg_clk_inhibit_clr;     /* 0x20 */
        unsigned int   cfg_clk;
        unsigned int   cfg_dto1;
        unsigned int   cfg_dto2;
        unsigned int   cfg_dto3;                     /* 0x30 */
        unsigned int   cfg_dto4;
        unsigned int   cfg_dto5;
        unsigned int   cfg_dto6;
        unsigned int   cfg_dto7;
        unsigned int   cfg_dto8;
        unsigned int   cfg_dto9;
        unsigned int   cfg_dto10;                    /* 0x4c */
        unsigned int   cfg_resv0[28];
        unsigned int   cfg_pll1;                     /* 0xc0 */
        unsigned int   cfg_pll2;
        unsigned int   cfg_pll3;
        unsigned int   cfg_pll4;
        unsigned int   cfg_pll5;                     /* 0xD0 */
        unsigned int   cfg_pll6;
        unsigned int   cfg_pll7;
        unsigned int   cfg_pll8;
        unsigned int   cfg_resv1[8];
        unsigned int   cfg_usb_config;               /* 0x100 */
        unsigned int   cfg_usb1_config;
        unsigned int   cfg_resv2[6];
        unsigned int   cfg_emi_config;               /* 0x120 */
        unsigned int   cfg_resv3[3];
        unsigned int   cfg_resv4; //pin_interconnect /* 0x130 */
        unsigned int   cfg_resv5; //pin_interconnect
        unsigned int   cfg_resv6; //pin_interconnect
        unsigned int   cfg_resv7[5];
        unsigned int   cfg_dll_config;               /* 0x150 */
        unsigned int   cfg_resv8[3];
        unsigned int   cfg_pdm_sel_1;                /* 0x160 */
        unsigned int   cfg_pdm_sel_2;
        unsigned int   cfg_pdm_sel_3;
        unsigned int   cfg_resv9;
        unsigned int   cfg_source_sel;               /* 0x170 */
        unsigned int   cfg_resv10[3];
        unsigned int   cfg_chip_info;                /* 0x180 */
        unsigned int   cfg_resv11[3];
        unsigned int   cfg_efuse_data;               /* 0x190 */
        unsigned int   cfg_efuse_ctrl;
        unsigned int   cfg_resv12[2];
        unsigned int   cfg_audio_codec_data;         /* 0x1a0 */
        unsigned int   cfg_audio_codec_ctrl;
        unsigned int   cfg_resv13[2];
        unsigned int   cfg_eth_config;               /* 0x1b0 */
        unsigned int   cfg_resv14[3];
        unsigned int   cfg_mpeg_cold_rst_2_1set;     /* 0x1c0 */
        unsigned int   cfg_resv15[3];
        unsigned int   cfg_mpeg_hot_rst_2_1set;      /* 0x1d0 */
        unsigned int   cfg_resv16[3];
        unsigned int   cfg_hdmi_base;                /* 0x1e0 */
        unsigned int   cfg_hdmi_sel;                 /* 0x1e4 */
};

typedef struct gx3201_vpu_reg {
        unsigned int rPP_CTRL; //0x0000
        unsigned int rPP_V_PHASE;
        unsigned int rPP_POSITION;
        unsigned int rPP_SOURCE_SIZE;
        unsigned int rPP_VIEW_SIZE;
        unsigned int rPP_ZOOM;
        unsigned int rPP_FRAME_STRIDE;
        unsigned int rPP_FILTER_SIGN;
        unsigned int rPP_PHASE_0_H;
        unsigned int rPP_PHASE_0_V;
        unsigned int rPP_DISP_CTRL;
        unsigned int rPP_DISP_R_PTR;
        unsigned int rPP_BACK_COLOR; //0x0030
        unsigned int rRESERVED_A[3]; //0x0034 38 3C

        unsigned int rPIC_CTRL; //0x0040
        unsigned int rPIC_V_PHASE;
        unsigned int rPIC_POSITION;
        unsigned int rPIC_SOURCE_SIZE;
        unsigned int rPIC_VIEW_SIZE;
        unsigned int rPIC_ZOOM;
        unsigned int rPIC_PARA;
        unsigned int rPIC_FILTER_SIGN;
        unsigned int rPIC_PHASE_0_H;
        unsigned int rPIC_PHASE_0_V;
        unsigned int rPIC_Y_TOP_ADDR;
        unsigned int rPIC_Y_BOTTOM_ADDR;
        unsigned int rPIC_UV_TOP_ADDR;
        unsigned int rPIC_UV_BOTTOM_ADDR;
        unsigned int rPIC_BACK_COLOR; //0x0078
        unsigned int rRESERVED_B[5]; //0x007C 80 84 88 8C

        unsigned int rOSD_CTRL; //0x0090
        unsigned int rOSD_FIRST_HEAD_PTR;
        unsigned int rOSD_VIEW_SIZE;
        unsigned int rOSD_ZOOM;
        unsigned int rOSD_COLOR_KEY;
        unsigned int rOSD_ALPHA_5551;
        unsigned int rOSD_PHASE_0;
        unsigned int rOSD_POSITION; //0x00AC

        unsigned int rCAP_CTRL; //0x00B0
        unsigned int rCAP_ADDR;
        unsigned int rCAP_HEIGHT;
        unsigned int rCAP_WIDTH; //0x00BC

        unsigned int rRESERVED_X[4]; //0x00C0 C4 C8 CC

        unsigned int rVBI_CTRL; //0x00D0
        unsigned int rVBI_FIRST_ADDR;
        unsigned int rVBI_ADDR; //0x00D8

        unsigned int rMIX_CTRL; //0x00DC
        unsigned int rCHIPTEST;
        unsigned int rSCAN_LINE;
        unsigned int rSYS_PARA;
        unsigned int rBUFF_CTRL1;
        unsigned int rBUFF_CTRL2;
        unsigned int rEMPTY_GATE_1;
        unsigned int rEMPTY_GATE_2;
        unsigned int rFULL_GATE;
        unsigned int rBUFFER_INT; //0x0100
        unsigned int rRESERVED_C[63]; //0x0104 108 10C ... 0x01FC

        unsigned int rPP_PARA_H[64]; //0x0200 ~ 0x02FC
        unsigned int rPP_PARA_V[64]; //0x0300 ~ 0x03FC

        unsigned int rOSD_PARA[64];  //0x0400 ~ 0x04FC

        unsigned int rDISP0_CTRL[8]; //0x0500 ~ 0x051C
        unsigned int rDISP1_CTRL[8]; //0x0520 ~ 0x053C
        unsigned int rDISP2_CTRL[8]; //0x0540 ~ 0x055C
        unsigned int rDISP3_CTRL[8]; //0x0560 ~ 0x057C
        unsigned int rDISP4_CTRL[8]; //0x0580 ~ 0x059C
        unsigned int rDISP5_CTRL[8]; //0x05A0 ~ 0x05BC
        unsigned int rDISP6_CTRL[8]; //0x05C0 ~ 0x05DC
        unsigned int rDISP7_CTRL[8]; //0x05E0 ~ 0x05FC

        unsigned int rPIC_PARA_H[64]; //0x0600 ~ 0x06FC
        unsigned int rPIC_PARA_V[64]; //0x0700 ~ 0x07FC
}Gx3201VpuReg;

typedef struct gx3201_svpu_reg {
        unsigned int    rSVPU_CTRL;
        unsigned int    rSVPU_CTRL1;
        unsigned int    rCAP_PARA_T;
        unsigned int    rCAP_PARA_B;
        unsigned int    rRST_CTRL;   // 0x10
        unsigned int    rZOOM;
        unsigned int    rV_PHASE;
        unsigned int    rZOOM_CTRL;
        unsigned int    rVBI_CTRL;   // 0x20
        unsigned int    rVBI_FIRST_ADDR;
        unsigned int    rVBI_ADDR;  // 0x28
        unsigned int    rRESERVE[21];
        unsigned int    rGAIN_DAC;   // 0x80
        unsigned int    rPOWER_DOWN;   // 0x84
        unsigned int    rPOWER_DOWN_BYSELF;
}Gx3201SvpuReg;

typedef enum {
	GXAV_VOUT_PAL = 1                   ,
	GXAV_VOUT_PAL_M                     ,
	GXAV_VOUT_PAL_N                     ,
	GXAV_VOUT_PAL_NC                    ,
	GXAV_VOUT_NTSC_M                    ,
	GXAV_VOUT_NTSC_443                  ,

	GXAV_VOUT_YCBCR_480I                ,
	GXAV_VOUT_YCBCR_576I                ,

	GXAV_VOUT_YPBPR_1080I_50HZ          ,
	GXAV_VOUT_YPBPR_1080I_60HZ          ,

	GXAV_VOUT_YPBPR_480P                ,
	GXAV_VOUT_YPBPR_576P                ,

	GXAV_VOUT_YPBPR_720P_50HZ           ,
	GXAV_VOUT_YPBPR_720P_60HZ           ,
	GXAV_VOUT_YPBPR_1080P_50HZ          ,
	GXAV_VOUT_YPBPR_1080P_60HZ          ,

	GXAV_VOUT_VGA_480P                  ,
	GXAV_VOUT_VGA_576P                  ,

	GXAV_VOUT_DIGITAL_RGB_720x480_0_255 ,
	GXAV_VOUT_DIGITAL_RGB_320x240_0_255 ,
	GXAV_VOUT_DIGITAL_RGB_16_235        ,

	GXAV_VOUT_BT656_YC_8BITS            ,
	GXAV_VOUT_BT656_YC_10BITS           ,
	GXAV_VOUT_HDMI_480I                 ,
	GXAV_VOUT_HDMI_576I                 ,
	GXAV_VOUT_HDMI_480P                 ,
	GXAV_VOUT_HDMI_576P                 ,
	GXAV_VOUT_HDMI_720P_50HZ            ,
	GXAV_VOUT_HDMI_720P_60HZ            ,
	GXAV_VOUT_HDMI_1080I_50HZ           ,
	GXAV_VOUT_HDMI_1080I_60HZ           ,
	GXAV_VOUT_HDMI_1080P_50HZ           ,
	GXAV_VOUT_HDMI_1080P_60HZ           ,

	GXAV_VOUT_NULL_MAX                  ,
}GxVideoOutProperty_Mode;

enum videoout_hdmi_type {
    HDMI_RGB_OUT = 1,
    HDMI_YUV_422 = 2,
    HDMI_YUV_444 = 4,
};

#define CFG_VPU_HOT_SET(base)                                                   \
    REG_SET_BIT(&(base->cfg_mpeg_hot_reset), 28)

#define CFG_VPU_HOT_CLR(base)                                                   \
    REG_CLR_BIT(&(base->cfg_mpeg_hot_reset), 28)

#define bVPU_BUFF_STATE_DELAY     (16)
#define mVPU_BUFF_STATE_DELAY     (0xFF<<bVPU_BUFF_STATE_DELAY)

#define VPU_SET_BUFF_STATE_DELAY(reg,value) \
        REG_SET_FIELD(&(reg),mVPU_BUFF_STATE_DELAY,value,bVPU_BUFF_STATE_DELAY)

#define REG_SET_CLR_BIT(reg, bit, bit_val) do {         \
        unsigned int tmp = __raw_readl(reg);            \
        tmp &= ~(1UL << (bit));                 \
        tmp |= ((bit_val) << (bit));                    \
        __raw_writel(tmp, reg);                         \
}while(0)

#define REG_GET_BIT(reg, bit) ((unsigned int)((__raw_readl(reg) >> (bit)) & 1))

#define REG_SET_FIELD(reg,mask,val,offset) do {         \
        unsigned int tmp = __raw_readl(reg);            \
        tmp  &=  ~(mask);                               \
        tmp  |=  ((val) << (offset)) & (mask);          \
        __raw_writel(tmp, reg);                         \
}while(0)

#define REG_SET_BIT(reg,bit) do {                       \
        unsigned int tmp = __raw_readl(reg);            \
        tmp |= (1UL << (bit));                          \
        __raw_writel(tmp, reg);                         \
}while(0)

#define REG_CLR_BIT(reg,bit) do {                       \
        unsigned int tmp = __raw_readl(reg);            \
        tmp &= ~(1UL << (bit));                         \
        __raw_writel(tmp, reg);                         \
}while(0)

#define REG_GET_VAL(reg)        __raw_readl(reg)

#endif

