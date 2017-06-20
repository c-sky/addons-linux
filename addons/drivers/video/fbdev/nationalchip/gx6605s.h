#ifndef _GX6605S_H_
#define _GX6605S_H_

int gx6605s_fbinit(struct fb_info *info);

typedef struct gx6605s_vpu_reg {
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
}Gx6605sVpuReg;

#define REG_GET_BIT(reg, bit) ((unsigned int)((__raw_readl(reg) >> (bit)) & 1))

#define REG_SET_CLR_BIT(reg, bit, bit_val) do {         \
	unsigned int tmp = __raw_readl(reg);            \
	tmp &= ~(1UL << (bit));                         \
	tmp |= ((bit_val) << (bit));                    \
	__raw_writel(tmp, reg);                         \
}while(0)

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

