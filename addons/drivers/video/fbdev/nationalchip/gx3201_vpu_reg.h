#ifndef __GX3201_VPU_REG_H__
#define __GX3201_VPU_REG_H__

#include "gxav_bitops.h"
#include "3201.h"

#define VPU_MPW  (0)
#define VPU_NRE  (1)

#define VPU_MOD_VERSION VPU_NRE

#define VPU_REG_BASE_ADDR         (0x04800000)
#define SVPU_REG_BASE_ADDR        (0x04900000)

#define OSD_ADDR_MASK             (0xFFFFFFFF)
#define OSD_BASE_LINE_MASK        (0x0001FFF)
#define SPP_ADDR_MASK             (0xFFFFFFFF)
#define VPP_ADDR_MASK             (0xFFFFFFFF)
#define VBI_ADDR_MASK             (0xFFFFFFF)
#define CE_ADDR_MASK              (0xFFFFFFFF)
#define GA_ADDR_MASK              (0x3FFFFFF)
#define GA_HI_ADDR_MASK           (0x0000003)
#define GA_FULL_ADDR_MASK         ((GA_HI_ADDR_MASK << 26) | GA_ADDR_MASK)
#define ZT_ADDR_MASK              (0x3FFFFFF)

typedef struct {
	unsigned int    word1;
	unsigned int    word2;
	unsigned int    word3;
	unsigned int    word4;
	unsigned int    word5;
	unsigned int    word6;
	unsigned int    word7;
}OsdRegionHead;

/*****************************************************************************/
/*                                   VPP                                     */
/*****************************************************************************/

enum {
	VPU_VPP_PLAY_MODE_FIELD = 0,
	VPU_VPP_PLAY_MODE_FRAME,
};

//VPP_CTRL
#define bVPU_VPP_EN               (0)
#define bVPU_VPP_ZOOM_MODE        (1)
#define bVPU_VPP_V_DOWNSCALE      (2)
#define bVPU_VPP_H_DOWNSCALE      (3)
#define bVPU_VPP_FIELD_START      (4)
#define bVPU_VPP_MODE_UV          (6)

#define bVPU_VPP_H_PHASE_BIAS     (8)
#define bVPU_VPP_VT_PHASE_BIAS    (0)
#define bVPU_VPP_VB_PHASE_BIAS    (16)
#define bVPU_VPP_BLUR_EN          (30)

#define mVPU_VPP_EN               (0x01<<bVPU_VPP_EN )
#define mVPU_VPP_ZOOM_MODE        (0x01<<bVPU_VPP_ZOOM_MODE)
#define mVPU_VPP_V_DOWNSCALE      (0x01<<bVPU_VPP_V_DOWNSCALE)
#define mVPU_VPP_H_DOWNSCALE      (0x01<<bVPU_VPP_H_DOWNSCALE)
#define mVPU_VPP_MODE_UV          (0x03<<bVPU_VPP_MODE_UV)
#define mVPU_VPP_H_PHASE_BIAS     (0x7F<<bVPU_VPP_H_PHASE_BIAS)
#define mVPU_VPP_VT_PHASE_BIAS    (0x7FFF<<bVPU_VPP_VT_PHASE_BIAS)
#define mVPU_VPP_VB_PHASE_BIAS    (0x7FFF<<bVPU_VPP_VB_PHASE_BIAS)

#define VPU_VPP_ENABLE(reg) do{\
	while(!REG_GET_BIT(&(reg),bVPU_VPP_EN)) \
		REG_SET_BIT(&(reg),bVPU_VPP_EN);\
}while(0)

#define VPU_VPP_DISABLE(reg) do{\
		REG_CLR_BIT(&(reg),bVPU_VPP_EN);\
}while(0)

#define VPU_VPP_V_DOWNSCALE_ENABLE(reg) \
	REG_SET_BIT(&(reg),bVPU_VPP_V_DOWNSCALE)
#define VPU_VPP_V_DOWNSCALE_DISABLE(reg) \
	REG_CLR_BIT(&(reg),bVPU_VPP_V_DOWNSCALE)

#define VPU_VPP_H_DOWNSCALE_ENABLE(reg) \
	REG_SET_BIT(&(reg),bVPU_VPP_H_DOWNSCALE)
#define VPU_VPP_H_DOWNSCALE_DISABLE(reg) \
	REG_CLR_BIT(&(reg),bVPU_VPP_H_DOWNSCALE)

#define VPU_VPP_SET_PP_PHASE_0_H(reg, val)\
	REG_SET_VAL(&(reg), val)
#define VPU_VPP_SET_PP_PHASE_0_V(reg, val)\
	REG_SET_VAL(&(reg), val)

#define VPU_VPP_ZOOM_MODE_OUT(reg) \
	REG_SET_BIT(&(reg),bVPU_VPP_ZOOM_MODE)
#define VPU_VPP_ZOOM_MODE_IN(reg) \
	REG_CLR_BIT(&(reg),bVPU_VPP_ZOOM_MODE)

#define VPU_VPP_SET_FIELD_MODE_UV_FIELD(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_MODE_UV, value,bVPU_VPP_MODE_UV)

#define VPU_VPP_BLUR_ENABLE(reg) \
	REG_SET_BIT(&(reg),bVPU_VPP_BLUR_EN)
#define VPU_VPP_BLUR_DISABLE(reg) \
	REG_CLR_BIT(&(reg),bVPU_VPP_BLUR_EN)

#define VPU_VPP_SET_H_PHASE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_H_PHASE_BIAS,value,bVPU_VPP_H_PHASE_BIAS)

#define VPU_VPP_SET_VT_PHASE(reg,value) \
	REG_SET_FIELD(&(reg), mVPU_VPP_VT_PHASE_BIAS,value,bVPU_VPP_VT_PHASE_BIAS)

#define VPU_VPP_SET_VB_PHASE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_VB_PHASE_BIAS,value,bVPU_VPP_VB_PHASE_BIAS)

//VPP_DISP_CTRL
#define bVPU_VPP_DISP_CTRL_RST               (12)

#define VPU_VPP_DISP_CTRL_RESET_SET(reg) do{\
		REG_SET_BIT(&(reg),bVPU_VPP_DISP_CTRL_RST);\
}while(0)

#define VPU_VPP_DISP_CTRL_RESET_CLR(reg) do{\
		REG_CLR_BIT(&(reg),bVPU_VPP_DISP_CTRL_RST);\
}while(0)

//VPP_ZOOM
#define VPU_VPP_ZOOM_WIDTH (12)
#define VPU_VPP_ZOOM_NONE (1 << VPU_VPP_ZOOM_WIDTH)
#define VPU_VPP_ZOOM_OUT_MAX (VPU_VPP_ZOOM_NONE << 2)

#define bVPU_VPP_VZOOM            (0)
#define bVPU_VPP_HZOOM            (16)
#define bVPU_VPP_VT_PHASE_BIAS_H  (28)
#define bVPU_VPP_VB_PHASE_BIAS_H  (30)
#define mVPU_VPP_VZOOM            (0x7FFF<<bVPU_VPP_VZOOM)
#define mVPU_VPP_HZOOM            (0x7FFF<<bVPU_VPP_HZOOM)
#define mVPU_VPP_VT_PHASE_BIAS_H  (0x3<<bVPU_VPP_VT_PHASE_BIAS_H)
#define mVPU_VPP_VB_PHASE_BIAS_H  (0x3<<bVPU_VPP_VB_PHASE_BIAS_H)

#define VPU_VPP_SET_ZOOM(reg,hzoom,vzoom) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_VPP_HZOOM,hzoom,bVPU_VPP_HZOOM); \
		REG_SET_FIELD(&(reg),mVPU_VPP_VZOOM,vzoom,bVPU_VPP_VZOOM); \
	}while (0)

#define VPU_VPP_SET_PHASE_H(reg,vt,vb) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_VPP_VT_PHASE_BIAS_H,vt,bVPU_VPP_VT_PHASE_BIAS_H); \
		REG_SET_FIELD(&(reg),mVPU_VPP_VB_PHASE_BIAS_H,vb,bVPU_VPP_VB_PHASE_BIAS_H); \
	}while (0)

//VPP_SIZE
#define bVPU_VPP_PP_SIZE_WIDTH (0)
#define bVPU_VPP_PP_SIZE_HIGH  (16)
#define mVPU_VPP_PP_SIZE_WIDTH (0x7FF << bVPU_VPP_PP_SIZE_WIDTH)
#define mVPU_VPP_PP_SIZE_HIGH  (0x7FF << bVPU_VPP_PP_SIZE_HIGH)

#define VPU_VPP_SET_PP_SIZE(reg,width,high) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_VPP_PP_SIZE_WIDTH,width,bVPU_VPP_PP_SIZE_WIDTH); \
		REG_SET_FIELD(&(reg),mVPU_VPP_PP_SIZE_HIGH,high,bVPU_VPP_PP_SIZE_HIGH); \
	}while (0)

//VPP_SOURCE_SIZE
#define bVPU_VPP_PP_SOURCE_SIZE_WIDTH (0)
#define bVPU_VPP_PP_SOURCE_SIZE_HIGH  (16)
#define mVPU_VPP_PP_SOURCE_SIZE_WIDTH (0xFFF << bVPU_VPP_PP_SOURCE_SIZE_WIDTH)
#define mVPU_VPP_PP_SOURCE_SIZE_HIGH  (0x7FF << bVPU_VPP_PP_SOURCE_SIZE_HIGH)

#define VPU_VPP_SET_PP_SOURCE_SIZE(reg,width,high) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_VPP_PP_SOURCE_SIZE_WIDTH,width,bVPU_VPP_PP_SOURCE_SIZE_WIDTH); \
		REG_SET_FIELD(&(reg),mVPU_VPP_PP_SOURCE_SIZE_HIGH,high,bVPU_VPP_PP_SOURCE_SIZE_HIGH); \
	}while (0)

//VPP_POSITION
#define bVPU_VPP_PP_POSITION_X (0)
#define bVPU_VPP_PP_POSITION_Y (16)
#define mVPU_VPP_PP_POSITION_X (0x7FF << bVPU_VPP_PP_POSITION_X)
#define mVPU_VPP_PP_POSITION_Y (0x7FF << bVPU_VPP_PP_POSITION_Y)

#define VPU_VPP_SET_PP_POSITION(reg,x,y) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_VPP_PP_POSITION_X,x,bVPU_VPP_PP_POSITION_X); \
		REG_SET_FIELD(&(reg),mVPU_VPP_PP_POSITION_Y,y,bVPU_VPP_PP_POSITION_Y); \
	}while (0)

//VPP_LEN_PHASE
#define bVPU_VPP_REQ_LENGTH       (16)
#define bVPU_VPP_INTP_PHASE       (16)

#define mVPU_VPP_REQ_LENGTH       (0x07FF<<bVPU_VPP_REQ_LENGTH) //Ö»ÓÐ[8:0]
#define mVPU_VPP_INTP_PHASE       (0x00FF<<bVPU_VPP_INTP_PHASE)

#define VPU_VPP_SET_LINE_REQ_LENGTH(reg,value) \
        REG_SET_FIELD(&(reg),mVPU_VPP_REQ_LENGTH,value,bVPU_VPP_REQ_LENGTH)

#define VPU_VPP_SET_INTP_PHASE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_INTP_PHASE,value,bVPU_VPP_INTP_PHASE)

//VPP_BACK_COLOR
#define bVPU_VPP_BACK_COLOR       (0)
#define mVPU_VPP_BACK_COLOR       (0xFFFFFF<<bVPU_VPP_BACK_COLOR)
#define VPU_VPP_SET_BG_COLOR(reg,y,cb,cr) do{\
	unsigned int Reg ; \
	Reg = ((unsigned int)cr&0xFF)|(((unsigned int)cb&0xFF)<<8)|(((unsigned int)y&0xFF)<<16);\
	REG_SET_VAL(&(reg),Reg); \
}while(0)

//SPP_BACK_COLOR
#define bVPU_SPP_BACK_COLOR       (0)
#define mVPU_SPP_BACK_COLOR       (0xFFFFFF<<bVPU_SPP_BACK_COLOR)
#define VPU_SPP_SET_BG_COLOR(reg,y,cb,cr) do{\
	unsigned int Reg ; \
	Reg = ((unsigned int)cr&0xFF)|(((unsigned int)cb&0xFF)<<8)|(((unsigned int)y&0xFF)<<16);\
	REG_SET_VAL(&(reg),Reg); \
}while(0)

//MIX_CTRL
#define  bVPU_VPP_ALPHA           (1)
#define  mVPU_VPP_ALPHA           (0x7F<<bVPU_VPP_ALPHA)

#define VPU_VPP_SET_MIXWEIGHT(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_ALPHA,value,bVPU_VPP_ALPHA)

//PHASE
#define VPU_VPP_SET_PHASE(reg,i,Value) \
	REG_SET_VAL(&(reg[i]),Value)

//PP_FRAME_STRIDE
#define bVPU_VPP_FRAME_STRIDE (0)
#define mVPU_VPP_FRAME_STRIDE (0x3FFF<<bVPU_VPP_FRAME_STRIDE)
#define VPU_VPP_SET_BASE_LINE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_FRAME_STRIDE,value,bVPU_VPP_FRAME_STRIDE)
#define VPU_VPP_GET_BASE_LINE(reg) \
        REG_GET_FIELD(&(reg), mVPU_VPP_FRAME_STRIDE, bVPU_VPP_FRAME_STRIDE)
//PP_Y_TOP_ADDR
#define bVPU_VPP_Y_TOP_ADDR          (0)
#define mVPU_VPP_Y_TOP_ADDR          (VPP_ADDR_MASK<<bVPU_VPP_Y_TOP_ADDR)
#define VPU_VPP_SET_Y_TOP_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_Y_TOP_ADDR,value,bVPU_VPP_Y_TOP_ADDR)
//PP_Y_BOTTOM_ADDR
#define bVPU_VPP_Y_BOTTOM_ADDR          (0)
#define mVPU_VPP_Y_BOTTOM_ADDR          (VPP_ADDR_MASK<<bVPU_VPP_Y_BOTTOM_ADDR)
#define VPU_VPP_SET_Y_BOTTOM_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_Y_BOTTOM_ADDR,value,bVPU_VPP_Y_BOTTOM_ADDR)
//PP_UV_TOP_ADDR
#define bVPU_VPP_UV_TOP_ADDR          (0)
#define mVPU_VPP_UV_TOP_ADDR          (VPP_ADDR_MASK<<bVPU_VPP_UV_TOP_ADDR)
#define VPU_VPP_SET_UV_TOP_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_UV_TOP_ADDR,value,bVPU_VPP_UV_TOP_ADDR)
//PP_UV_BOTTOM_ADDR
#define bVPU_VPP_UV_BOTTOM_ADDR          (0)
#define mVPU_VPP_UV_BOTTOM_ADDR          (VPP_ADDR_MASK<<bVPU_VPP_UV_BOTTOM_ADDR)
#define VPU_VPP_SET_UV_BOTTOM_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_UV_BOTTOM_ADDR,value,bVPU_VPP_UV_BOTTOM_ADDR)

#define bVPU_VPP_PLAY_MODE (1)
#define mVPU_VPP_PLAY_MODE (0x1 << bVPU_VPP_PLAY_MODE)
#define VPU_VPP_SET_PLAY_MODE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_PLAY_MODE,value,bVPU_VPP_PLAY_MODE)

//BUFFER_CTRL1
#define bVPU_VPP_REQ_DATA_LEN     (16)
#define mVPU_VPP_REQ_DATA_LEN     (0x7FF<<bVPU_VPP_REQ_DATA_LEN)

#define VPU_VPP_SET_BUFFER_REQ_DATA_LEN(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_REQ_DATA_LEN, value,bVPU_VPP_REQ_DATA_LEN)

//BUFFER_CTRL2
#define bVPU_OSD_BUFF_LEN         (0)
#define bVPU_VPP_LOCK             (16)
#define bVPU_VPP_AV_MODE          (19)
#define bVPU_BUFF_STATE_DELAY     (16)

#define mVPU_OSD_BUFF_LEN         (0x3ff<<bVPU_OSD_BUFF_LEN)
#define mVPU_VPP_LOCK             (0x07<<bVPU_VPP_LOCK)
#define mVPU_VPP_AV_MODE          (0x01<<bVPU_VPP_AV_MODE)
#define mVPU_BUFF_STATE_DELAY     (0xFF<<bVPU_BUFF_STATE_DELAY)

#define VPU_VPP_SET_LOCK(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_VPP_LOCK,value,bVPU_VPP_LOCK)

#define VPU_VPP_SET_AV_MODE_MPEG(reg) \
	REG_CLR_BIT(&(reg),bVPU_VPP_AV_MODE)

#define VPU_VPP_SET_AV_MODE_H264(reg) \
	REG_SET_BIT(&(reg),bVPU_VPP_AV_MODE)

#define VPU_VPP_PRI_PARAM(reg,v1,v2,v3) do{\
	REG_SET_FIELD(&(reg),mVPU_OSD_BUFF_LEN,v1,bVPU_OSD_BUFF_LEN);\
	REG_SET_FIELD(&(reg),mVPU_VPP_LOCK ,v2,bVPU_VPP_LOCK);\
	REG_SET_FIELD(&(reg),mVPU_BUFF_STATE_DELAY,v3,bVPU_BUFF_STATE_DELAY);\
}while(0)

#define VPU_VPP_BUFFER_PRI_LEVEL1(reg1,reg2)  do{\
	reg1 = 0x02000200;\
	VPU_VPP_PRI_PARAM(reg2,0x80,0x04,0x40);\
}while(0)

#define VPU_VPP_BUFFER_PRI_LEVEL2(reg1,reg2) do{\
	reg1 = 0x02000200;\
	VPU_VPP_PRI_PARAM(reg2,0x80,0x05,0x40);\
}while(0)

#define VPU_VPP_BUFFER_PRI_LEVEL3(reg1,reg2) do{\
	reg1 = 0x02000200;\
	VPU_VPP_PRI_PARAM(reg2,0x80,0x05,0x40);\
}while(0)

#define VPU_VPP_BUFFER_PRI_LEVEL4(reg1,reg2) do{\
        reg1 = 0x02000200;\
        VPU_VPP_PRI_PARAM(reg2,0x80,0x07,0x40);\
}while(0)

/*****************************************************************************/
/*                                   BKG                                     */
/*****************************************************************************/
//MIX_CTRL
#define VPU_BKG_SET_COLOR(reg,y,cb,cr) do{\
	unsigned int Reg ; \
	Reg = ((unsigned char)(y)<<8)|((unsigned char)(cb)<<16)|((unsigned char)(cr)<<24);\
	(reg) = ((reg) & 0xFF)|(Reg & 0xFFFFFF00) ;\
}while(0)

/*****************************************************************************/
/*                                   OSD                                     */
/*****************************************************************************/
#define OSD_FLICKER_TABLE_LEN     (64)

//OSD_CTRL
#define bVPU_OSD_EN               (0)
#define bVPU_OSD_VT_PHASE         (8)
#define bVPU_OSD_HZOOM            (0)
#define bVPU_OSD_VZOOM            (16)
#define bVPU_OSD_ZOOM_MODE_EN_IPS (25)
#define bVPU_OSD_HDOWN_SAMPLE_EN  (28)
#define bVPU_OSD_ANTI_FLICKER     (29)
#define bVPU_OSD_ANTI_FLICKER_CBCR (31)
#define mVPU_OSD_EN               (0x1<<bVPU_OSD_EN)
#define mVPU_OSD_VT_PHASE         (0x3FFF<<bVPU_OSD_VT_PHASE)
#define mVPU_OSD_VZOOM            (0x3FFF<<bVPU_OSD_VZOOM)
#define mVPU_OSD_HZOOM            (0x3FFF<<bVPU_OSD_HZOOM)
#define mVPU_OSD_HDOWN_SAMPLE_EN  (0x1<<bVPU_OSD_HDOWN_SAMPLE_EN)

/*
 * sequences ....... value
 * DCBA_HGFE .......   0
 * EFGH_ABCD .......   1
 * HGFE_DCBA .......   2
 * ABCD_EFGH .......   3
 *
 * reg: 0xa48000c8 [21:20]
 * understanding:
 * If points on a line are p0,p1,p2,p3...
 * And the color value of them are vale0, vale1, vale2, vale3, ...
 * VPU read 8 bytes each time.
 * 1)32 bpp VPU defined vale0 = (A<<24)|(B<<16)|(C<<8)|(D<<0) ...
 *      example: p0 = 0x11223344,
 *      gdb>>x /x &p0
 *      gdb>>0x11223344
 *      so we should config: ABCD_EFGH
 * 2)16 bpp VPU defined vale0 = (A<<8 )|(B<<0), vale1 = (C<<8)|(D<<0) ...
 *      example:  p0 = 0x1122, p1 = 0x3344
 *      gdb>>x /x &p0
 *      gdb>>0x33441122
 *      so we should config: CDAB_GHEF
 * 3)8  bpp VPU defined vale0 = (A<<0), vale1 = (B<<0), vale2 = (C<<0) ...
 *      example: p0 = 0x11, p1 = 0x22, p2 = 0x33, p3 = 0x44
 *      gdb>>x /x &p0
 *      gdb>>0x44332211
 *      so we should config: DCBA_HGFE
 */
#if(VPU_MOD_VERSION == VPU_MPW)
	#define bVPU_RW_BYTESEQ	(20)
	#define mVPU_RW_BYTESEQ	(0x3<<bVPU_RW_BYTESEQ)
	#define VPU_SET_RW_BYTESEQ(reg, byte_seq)\
	do\
	{\
		REG_SET_FIELD(&(reg), mVPU_RW_BYTESEQ, byte_seq, bVPU_RW_BYTESEQ);\
	}while(0)
#else
	#define bVPU_RW_BYTESEQ_HIGH	(28)
	#define bVPU_RW_BYTESEQ_LOW		(20)
	#define mVPU_RW_BYTESEQ_LOW		(0x3<<bVPU_RW_BYTESEQ_LOW)
	#define VPU_SET_RW_BYTESEQ(reg, byte_seq)\
	do\
	{\
		REG_SET_FIELD(&(reg), mVPU_RW_BYTESEQ_LOW, byte_seq&0x3, bVPU_RW_BYTESEQ_LOW);\
		if(byte_seq>>2)\
			REG_SET_BIT(&(reg), bVPU_RW_BYTESEQ_HIGH);\
		else\
			REG_CLR_BIT(&(reg), bVPU_RW_BYTESEQ_HIGH);\
		REG_SET_FIELD(&(reg), 0x7<<12, byte_seq&0x7, 12);\
	}while(0)
#endif

#define REG_SET_BIT_ABCD_EFGH	GX_SET_BIT
#define REG_SET_BIT_DCBA_HGFE	GX_SET_BIT_E
#define REG_SET_BIT_CDAB_GHEF(reg, bit)\
	do{\
		unsigned int tmpVal  = gx_ioread32(reg); \
		tmpVal = ((tmpVal&0xffff)<<16)|((tmpVal>>16)&0xffff);\
		tmpVal |=(1<<(bit));\
		tmpVal = ((tmpVal&0xffff)<<16)|((tmpVal>>16)&0xffff);\
		gx_iowrite32(tmpVal, reg);\
	}while(0)
#define OSD_HEAD_SET_BIT(reg, bit) \
	do{\
		switch(byte_seq)\
		{\
		case ABCD_EFGH:\
			REG_SET_BIT_ABCD_EFGH(reg, bit);\
			break;\
		case DCBA_HGFE:\
			REG_SET_BIT_DCBA_HGFE(reg, bit);\
			break;\
		case CDAB_GHEF:\
			REG_SET_BIT_CDAB_GHEF(reg, bit);\
			break;\
		default:\
			break;\
		}\
	}while(0)

#define REG_CLR_BIT_ABCD_EFGH	GX_CLR_BIT
#define REG_CLR_BIT_DCBA_HGFE	GX_CLR_BIT_E
#define REG_CLR_BIT_CDAB_GHEF(reg, bit)\
	do{\
		unsigned int tmpVal  = gx_ioread32(reg); \
		tmpVal  = ((tmpVal&0xffff)<<16)|((tmpVal>>16)&0xffff);\
		tmpVal &= (~(1<<(bit)));\
		tmpVal  = ((tmpVal&0xffff)<<16)|((tmpVal>>16)&0xffff);\
		gx_iowrite32(tmpVal, reg);\
	}while(0)
#define OSD_HEAD_CLR_BIT(reg, bit) \
	do{\
		switch(byte_seq)\
		{\
		case ABCD_EFGH:\
			REG_CLR_BIT_ABCD_EFGH(reg, bit);\
			break;\
		case DCBA_HGFE:\
			REG_CLR_BIT_DCBA_HGFE(reg, bit);\
			break;\
		case CDAB_GHEF:\
			REG_CLR_BIT_CDAB_GHEF(reg, bit);\
			break;\
		default:\
			break;\
		}\
	}while(0)

#define REG_SET_FEILD_ABCD_EFGH	GX_SET_FEILD
#define REG_SET_FEILD_DCBA_HGFE	GX_SET_FEILD_E
#define REG_SET_FEILD_CDAB_GHEF(reg,mask,val,offset)\
	do{\
		unsigned int tmpVal  = gx_ioread32(reg); \
		tmpVal  = ((tmpVal&0xffff)<<16)|((tmpVal>>16)&0xffff);\
		tmpVal &=  ~(mask);\
		tmpVal |=  ((val) << (offset)) & (mask);\
		tmpVal  = ((tmpVal&0xffff)<<16)|((tmpVal>>16)&0xffff);\
		gx_iowrite32(tmpVal, reg);\
	}while(0)
#define OSD_HEAD_SET_FEILD(reg,mask,val,offset)\
	do{\
		switch(byte_seq)\
		{\
		case ABCD_EFGH:\
			REG_SET_FEILD_ABCD_EFGH(reg,mask,val,offset);\
			break;\
		case DCBA_HGFE:\
			REG_SET_FEILD_DCBA_HGFE(reg,mask,val,offset);\
			break;\
		case CDAB_GHEF:\
			REG_SET_FEILD_CDAB_GHEF(reg,mask,val,offset);\
			break;\
		default:\
			break;\
		}\
	}while(0)

#define REG_GET_FEILD_ABCD_EFGH	GX_GET_FEILD
#define REG_GET_FEILD_DCBA_HGFE	GX_GET_FEILD_E
#define REG_GET_FEILD_CDAB_GHEF(reg,mask,val,offset)\
	do{\
		unsigned int tmpVal  = gx_ioread32(reg); \
		tmpVal = ((tmpVal&0xffff)<<16)|((tmpVal>>16)&0xffff);\
		(val)  = (tmpVal & (mask)) >> (offset);\
	}while(0)
#define OSD_HEAD_GET_FEILD(reg,mask,val,offset)\
	do{\
		extern Gx3201Vpu *gx3201vpu_info;\
		switch(gx3201vpu_info->byte_seq)\
		{\
		case ABCD_EFGH:\
			REG_GET_FEILD_ABCD_EFGH(reg,mask,val,offset);\
			break;\
		case DCBA_HGFE:\
			REG_GET_FEILD_DCBA_HGFE(reg,mask,val,offset);\
			break;\
		case CDAB_GHEF:\
			REG_GET_FEILD_CDAB_GHEF(reg,mask,val,offset);\
			break;\
		default:\
			break;\
		}\
	}while(0)

#define VPU_OSD_ENABLE(reg) do{\
	while(!REG_GET_BIT(&(reg),bVPU_OSD_EN)){\
		REG_SET_BIT(&(reg),bVPU_OSD_EN);\
	}\
}while(0)

#define VPU_OSD_DISABLE(reg) do{\
	while(REG_GET_BIT(&(reg),bVPU_OSD_EN)){\
		REG_CLR_BIT(&(reg),bVPU_OSD_EN);\
	}\
}while(0)

#define VPU_OSD_ZOOM_WIDTH (12)
#define VPU_OSD_ZOOM_NONE (1 << VPU_OSD_ZOOM_WIDTH)

#define VPU_OSD_SET_VZOOM(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_OSD_VZOOM,value,bVPU_OSD_VZOOM)
#define VPU_OSD_GET_VZOOM(reg) \
	REG_GET_FIELD(&(reg), mVPU_OSD_VZOOM, bVPU_OSD_VZOOM)
#define VPU_OSD_SET_HZOOM(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_OSD_HZOOM,value,bVPU_OSD_HZOOM)
#define VPU_OSD_GET_HZOOM(reg) \
	REG_GET_FIELD(&(reg), mVPU_OSD_HZOOM, bVPU_OSD_HZOOM)
#define VPU_OSD_SET_VTOP_PHASE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_OSD_VT_PHASE,value,bVPU_OSD_VT_PHASE)
#define VPU_OSD_SET_ZOOM_MODE(reg)  \
	REG_SET_BIT(&(reg),bVPU_OSD_ANTI_FLICKER_CBCR)
#define VPU_OSD_CLR_ZOOM_MODE(reg)  \
	REG_CLR_BIT(&(reg),bVPU_OSD_ANTI_FLICKER_CBCR)
#define VPU_OSD_SET_ZOOM_MODE_EN_IPS(reg) \
	REG_SET_BIT(&(reg),bVPU_OSD_ZOOM_MODE_EN_IPS)
#define VPU_OSD_CLR_ZOOM_MODE_EN_IPS(reg) \
	REG_CLR_BIT(&(reg),bVPU_OSD_ZOOM_MODE_EN_IPS)

#define VPU_OSD_H_DOWNSCALE_ENABLE(reg) \
	REG_SET_BIT(&(reg),bVPU_OSD_HDOWN_SAMPLE_EN)
#define VPU_OSD_H_DOWNSCALE_DISABLE(reg) \
	REG_CLR_BIT(&(reg),bVPU_OSD_HDOWN_SAMPLE_EN)

#define VPU_OSD_PP_ANTI_FLICKER_ENABLE(reg)	 REG_SET_BIT(&(reg),bVPU_OSD_ANTI_FLICKER)
#define VPU_OSD_PP_ANTI_FLICKER_DISABLE(reg) REG_CLR_BIT(&(reg),bVPU_OSD_ANTI_FLICKER)

//OSD_FIRST_HEAD_PTR
#define bVPU_OSD_FIRST_HEAD       (0)
#define mVPU_OSD_FIRST_HEAD       (OSD_ADDR_MASK<<bVPU_OSD_FIRST_HEAD)

#define VPU_OSD_SET_FIRST_HEAD(reg,value)\
	REG_SET_FIELD(&(reg),mVPU_OSD_FIRST_HEAD,value,bVPU_OSD_FIRST_HEAD)

//OSD_PHASE_0
#define VPU_OSD_PHASE_0_ANTI_FLICKER_ENABLE (0x408040)
#define VPU_OSD_PHASE_0_ANTI_FLICKER_DISABLE (0x01FF00)
#define bVPU_OSD_PHASE_0       (0)
#define mVPU_OSD_PHASE_0       (0xFFFFFF<<bVPU_OSD_PHASE_0)
#define VPU_OSD_SET_PHASE_0(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_OSD_PHASE_0,value,bVPU_OSD_PHASE_0)
#define VPU_OSD_GET_PHASE_0(reg) \
	REG_GET_FIELD(&(reg), mVPU_OSD_PHASE_0, bVPU_OSD_PHASE_0)

//OSD_ALPHA_5551
#define bVPU_OSD_ALPHA_5551 (0)
#define mVPU_OSD_ALPHA_5551	(0xFFFF)
#define VPU_OSD_SET_ALPHA_5551(reg, value) \
	REG_SET_FIELD(&(reg), mVPU_OSD_ALPHA_5551,value,bVPU_OSD_ALPHA_5551)

//BUFFER_CTRL2
#define bVPU_OSD_REQ_DATA_LEN     (0)
#define bVPU_BUFF_STATE_DELAY     (16)
#define mVPU_OSD_REQ_DATA_LEN     (0x7FF<<bVPU_OSD_REQ_DATA_LEN)
#define mVPU_BUFF_STATE_DELAY     (0xFF<<bVPU_BUFF_STATE_DELAY)

#define VPU_OSD_SET_BUFFER_REQ_DATA_LEN(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_OSD_REQ_DATA_LEN,value,bVPU_OSD_REQ_DATA_LEN)

#define VPU_SET_BUFF_STATE_DELAY(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_BUFF_STATE_DELAY,value,bVPU_BUFF_STATE_DELAY)

//OSD_COLOR_KEY
#define VPU_OSD_SET_COLOR_KEY(reg,R,G,B,A) do{\
	unsigned int Reg ; \
	Reg = (((unsigned char)R)<<24)|(((unsigned char)G)<<16)|(((unsigned char)B)<<8)|((unsigned char)A);\
	reg = Reg ;\
}while(0)

//rFLICKER
#define VPU_OSD_SET_FLIKER_FLITER(reg,i,Value) \
	REG_SET_VAL(&(reg[i]),Value)

//OSD_POSITION
#define bVPU_OSD_POSITION_X (0)
#define bVPU_OSD_POSITION_Y (16)
#define mVPU_OSD_POSITION_X (0x3FF << bVPU_OSD_POSITION_X)
#define mVPU_OSD_POSITION_Y (0x3FF << bVPU_OSD_POSITION_Y)

#define VPU_OSD_SET_POSITION(reg,x,y) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_OSD_POSITION_X,x,bVPU_OSD_POSITION_X); \
		REG_SET_FIELD(&(reg),mVPU_OSD_POSITION_Y,y,bVPU_OSD_POSITION_Y); \
	}while (0)

//OSD_SIZE
#define bVPU_OSD_VIEW_SIZE_WIDTH (0)
#define bVPU_OSD_VIEW_SIZE_HIGH  (16)
#define mVPU_OSD_VIEW_SIZE_WIDTH (0x7FF << bVPU_OSD_VIEW_SIZE_WIDTH)
#define mVPU_OSD_VIEW_SIZE_HIGH  (0x7FF << bVPU_OSD_VIEW_SIZE_HIGH)
#define VPU_OSD_SET_VIEW_SIZE(reg,width,high) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_OSD_VIEW_SIZE_WIDTH,width,bVPU_OSD_VIEW_SIZE_WIDTH); \
		REG_SET_FIELD(&(reg),mVPU_OSD_VIEW_SIZE_HIGH,high,bVPU_OSD_VIEW_SIZE_HIGH); \
	}while (0)

//OSD_HEAD_WORD1
#define bVPU_OSD_CLUT_SWITCH      (0)
#define bVPU_OSD_CLUT_LENGTH      (8)
#define bVPU_OSD_COLOR_MODE       (10)
#define bVPU_OSD_CLUT_UPDATA_EN   (13)
#define bVPU_OSD_FLIKER_FLITER_EN (14)
#define bVPU_OSD_COLOR_KEY_EN     (15)
#define bVPU_OSD_MIX_WEIGHT       (16)
#define bVPU_OSD_GLOBAL_ALPHA_EN  (23)
#define bVPU_OSD_TRUE_COLOR_MODE  (24)
#define bVPU_OSD_ARGB_CONVERT     (26)

#define mVPU_OSD_CLUT_SWITCH      (0xFF << bVPU_OSD_CLUT_SWITCH)
#define mVPU_OSD_CLUT_LENGTH      (0x3  << bVPU_OSD_CLUT_LENGTH)
#define mVPU_OSD_COLOR_MODE       (0x7  << bVPU_OSD_COLOR_MODE)
#define mVPU_OSD_CLUT_UPDATA_EN   (0x1  << bVPU_OSD_CLUT_UPDATA_EN)
#define mVPU_OSD_FLIKER_FLITER_EN (0x1  << bVPU_OSD_FLIKER_FLITER_EN)
#define mVPU_OSD_COLOR_KEY_EN     (0x1  << bVPU_OSD_COLOR_KEY_EN)
#define mVPU_OSD_MIX_WEIGHT       (0x7F << bVPU_OSD_MIX_WEIGHT)
#define mVPU_OSD_GLOBAL_ALPHA_EN  (0x1  << bVPU_OSD_GLOBAL_ALPHA_EN)
#define mVPU_OSD_TRUE_COLOR_MODE  (0x3  << bVPU_OSD_TRUE_COLOR_MODE)
#define mVPU_OSD_ARGB_CONVERT     (0x1  << bVPU_OSD_ARGB_CONVERT)

#define VPU_OSD_COLOR_KEY_ENABLE(reg) \
	OSD_HEAD_SET_BIT(&(reg),bVPU_OSD_COLOR_KEY_EN)

#define VPU_OSD_COLOR_KEY_DISABLE(reg) \
	OSD_HEAD_CLR_BIT(&(reg),bVPU_OSD_COLOR_KEY_EN)

#define VPU_OSD_CLUT_UPDATA_ENABLE(reg) \
	OSD_HEAD_SET_BIT(&(reg),bVPU_OSD_CLUT_UPDATA_EN)

#define VPU_OSD_CLUT_UPDATA_DISABLE(reg) \
	OSD_HEAD_CLR_BIT(&(reg),bVPU_OSD_CLUT_UPDATA_EN)

#define VPU_OSD_FLIKER_FLITER_ENABLE(reg) \
	OSD_HEAD_SET_BIT(&(reg),bVPU_OSD_FLIKER_FLITER_EN)

#define VPU_OSD_FLIKER_FLITER_DISABLE(reg) \
	OSD_HEAD_CLR_BIT(&(reg),bVPU_OSD_FLIKER_FLITER_EN)

#define VPU_OSD_GLOBAL_ALHPA_ENABLE(reg) \
	OSD_HEAD_SET_BIT(&(reg),bVPU_OSD_GLOBAL_ALPHA_EN)

#define VPU_OSD_GLOBAL_ALHPA_DISABLE(reg) \
	OSD_HEAD_CLR_BIT(&(reg),bVPU_OSD_GLOBAL_ALPHA_EN)

#define VPU_OSD_SET_MIX_WEIGHT(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_MIX_WEIGHT,value,bVPU_OSD_MIX_WEIGHT)

#define VPU_OSD_SET_COLOR_TYPE(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_COLOR_MODE,value,bVPU_OSD_COLOR_MODE)

#define VPU_OSD_GET_COLOR_TYPE(reg,value) \
	OSD_HEAD_GET_FEILD(&(reg), mVPU_OSD_COLOR_MODE, value, bVPU_OSD_COLOR_MODE)

#define VPU_OSD_SET_TRUE_COLOR_MODE(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_TRUE_COLOR_MODE,value,bVPU_OSD_TRUE_COLOR_MODE)

#define VPU_OSD_SET_CLUT_LENGTH(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_CLUT_LENGTH,value,bVPU_OSD_CLUT_LENGTH)

#define VPU_OSD_SET_CLUT_SWITCH(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_CLUT_SWITCH,value,bVPU_OSD_CLUT_SWITCH)

#define VPU_OSD_SET_ARGB_CONVERT(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_ARGB_CONVERT,value,bVPU_OSD_ARGB_CONVERT)

//OSD_HEAD_WORD2
#define bVPU_OSD_CLUT_PTR         (0)
#define mVPU_OSD_CLUT_PTR         (OSD_ADDR_MASK<<bVPU_OSD_CLUT_PTR)

#define VPU_OSD_SET_CLUT_PTR(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg), mVPU_OSD_CLUT_PTR,value,bVPU_OSD_CLUT_PTR)

//OSD_HEAD_WORD3
#define bVPU_OSD_LEFT             (0)
#define bVPU_OSD_RIGHT            (16)
#define mVPU_OSD_LEFT             (0x7FF<<bVPU_OSD_LEFT)
#define mVPU_OSD_RIGHT            (0x7FF<<bVPU_OSD_RIGHT)

#define VPU_OSD_SET_WIDTH(reg,left,right) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_LEFT,left,bVPU_OSD_LEFT); \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_RIGHT,right,bVPU_OSD_RIGHT)

//OSD_HEAD_WORD4
#define bVPU_OSD_TOP              (0)
#define bVPU_OSD_BOTTOM           (16)
#define mVPU_OSD_TOP              (0x7FF<<bVPU_OSD_TOP)
#define mVPU_OSD_BOTTOM           (0x7FF<<bVPU_OSD_BOTTOM)

#define VPU_OSD_SET_HEIGHT(reg,top,bottom) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_TOP,top,bVPU_OSD_TOP); \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_BOTTOM,bottom,bVPU_OSD_BOTTOM)

//OSD_HEAD_WORD5
#define bVPU_OSD_DATA_ADDR         (0)
#define mVPU_OSD_DATA_ADDR         (OSD_ADDR_MASK<<bVPU_OSD_DATA_ADDR)

#define VPU_OSD_SET_DATA_ADDR(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_DATA_ADDR,value,bVPU_OSD_DATA_ADDR)

 //OSD_HEAD_WORD6
#define bVPU_OSD_NEXT_PTR         (0)
#define bVPU_OSD_LIST_END         (31)

#define mVPU_OSD_NEXT_PTR         (OSD_ADDR_MASK<<bVPU_OSD_NEXT_PTR)
#define mVPU_OSD_LIST_END         (0x1<<bVPU_OSD_LIST_END)

#define VPU_OSD_LIST_END_ENABLE(reg) \
        OSD_HEAD_SET_BIT(&(reg),bVPU_OSD_LIST_END)
#define VPU_OSD_LIST_END_DISABLE(reg) \
        OSD_HEAD_CLR_BIT(&(reg),bVPU_OSD_LIST_END)

#define VPU_OSD_SET_NEXT_PTR(reg,value) \
        OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_NEXT_PTR,value,bVPU_OSD_NEXT_PTR)

//OSD_HEAD_WORD7
#define bVPU_OSD_BASE_LINE         (0)
#define mVPU_OSD_BASE_LINE         (OSD_BASE_LINE_MASK<<bVPU_OSD_BASE_LINE)
#define bVPU_OSD_ALPHA_RATIO       (16)
#define mVPU_OSD_ALPHA_RATIO       (0xFF<<bVPU_OSD_ALPHA_RATIO)
#define bVPU_OSD_ALPHA_RATIO_EN    (24)
#define VPU_OSD_SET_BASE_LINE(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_BASE_LINE,value,bVPU_OSD_BASE_LINE)
#define VPU_OSD_SET_ALPHA_RATIO_ENABLE(reg) \
	OSD_HEAD_SET_BIT(&(reg),bVPU_OSD_ALPHA_RATIO_EN)
#define VPU_OSD_SET_ALPHA_RATIO_DISABLE(reg) \
	OSD_HEAD_CLR_BIT(&(reg),bVPU_OSD_ALPHA_RATIO_EN)
#define VPU_OSD_SET_ALPHA_RATIO_VALUE(reg,value) \
	OSD_HEAD_SET_FEILD(&(reg),mVPU_OSD_ALPHA_RATIO,value,bVPU_OSD_ALPHA_RATIO)


/*****************************************************************************/
/*                                   SOSD                                     */
/*****************************************************************************/
#define SOSD_FLICKER_TABLE_LEN     (64)

//SOSD_CTRL
#define bVPU_SOSD_ON_TOP_EN (30)
#define VPU_SOSD_SET_ON_TOP_ENABLE(reg) \
	REG_SET_BIT(&(reg),bVPU_SOSD_ON_TOP_EN)
#define VPU_SOSD_SET_ON_TOP_DISABLE(reg) \
	REG_CLR_BIT(&(reg),bVPU_SOSD_ON_TOP_EN)
#define VPU_SOSD_GET_ON_TOP_EN(reg) \
	REG_GET_BIT(&(reg),bVPU_SOSD_ON_TOP_EN)

#define VPU_SOSD_GET_VZOOM VPU_OSD_GET_VZOOM
#define VPU_SOSD_GET_HZOOM VPU_OSD_GET_HZOOM
#define VPU_SOSD_SET_VZOOM VPU_OSD_SET_VZOOM
#define VPU_SOSD_SET_HZOOM VPU_OSD_SET_HZOOM

#define VPU_SOSD_PP_ANTI_FLICKER_ENABLE VPU_OSD_PP_ANTI_FLICKER_ENABLE
#define VPU_SOSD_CBCR_ANTI_FLICKER_ENABLE VPU_OSD_CBCR_ANTI_FLICKER_ENABLE
#define VPU_SOSD_PP_ANTI_FLICKER_DISABLE VPU_OSD_PP_ANTI_FLICKER_DISABLE
#define VPU_SOSD_CBCR_ANTI_FLICKER_DISABLE VPU_OSD_CBCR_ANTI_FLICKER_DISABLE
#define VPU_SOSD_ZOOM_NONE VPU_OSD_ZOOM_NONE

#define VPU_SOSD_DISABLE VPU_OSD_DISABLE
#define VPU_SOSD_ENABLE VPU_OSD_ENABLE

#define VPU_SOSD_H_DOWNSCALE_ENABLE VPU_OSD_H_DOWNSCALE_ENABLE
#define VPU_SOSD_H_DOWNSCALE_DISABLE VPU_OSD_H_DOWNSCALE_DISABLE

//SOSD_FIRST_HEAD
#define VPU_SOSD_SET_FIRST_HEAD VPU_OSD_SET_FIRST_HEAD

//SOSD_PHASE_0
#define VPU_SOSD_PHASE_0_ANTI_FLICKER_ENABLE VPU_OSD_PHASE_0_ANTI_FLICKER_ENABLE
#define VPU_SOSD_PHASE_0_ANTI_FLICKER_DISABLE VPU_OSD_PHASE_0_ANTI_FLICKER_DISABLE
#define VPU_SOSD_SET_PHASE_0 VPU_OSD_SET_PHASE_0
#define VPU_SOSD_GET_PHASE_0 VPU_OSD_GET_PHASE_0

//SOSD_PARA_0-63
#define VPU_SOSD_SET_FLIKER_FLITER VPU_OSD_SET_FLIKER_FLITER

//SOSD_HEAD_WORD
#define VPU_SOSD_SET_COLOR_TYPE VPU_OSD_SET_COLOR_TYPE
#define VPU_SOSD_GLOBAL_ALHPA_ENABLE VPU_OSD_GLOBAL_ALHPA_ENABLE
#define VPU_SOSD_SET_MIX_WEIGHT VPU_OSD_SET_MIX_WEIGHT
#define VPU_SOSD_GLOBAL_ALHPA_DISABLE VPU_OSD_GLOBAL_ALHPA_DISABLE
#define VPU_SOSD_SET_CLUT_PTR VPU_OSD_SET_CLUT_PTR
#define VPU_SOSD_SET_CLUT_LENGTH VPU_OSD_SET_CLUT_LENGTH
#define VPU_SOSD_CLUT_UPDATA_ENABLE VPU_OSD_CLUT_UPDATA_ENABLE
#define VPU_SOSD_SET_WIDTH VPU_OSD_SET_WIDTH
#define VPU_SOSD_SET_HEIGHT VPU_OSD_SET_HEIGHT
#define VPU_SOSD_SET_DATA_ADDR VPU_OSD_SET_DATA_ADDR
#define VPU_SOSD_LIST_END_ENABLE VPU_OSD_LIST_END_ENABLE
#define VPU_SOSD_SET_BASE_LINE VPU_OSD_SET_BASE_LINE

/*****************************************************************************/
/*                                   SPP                                     */
/*****************************************************************************/
//MIX_CTRL
#define bVPU_SPP_ON_TOP           (0)
#define mVPU_SPP_ON_TOP           (0x1<<bVPU_SPP_ON_TOP)

#define VPU_SPP_ON_TOP(reg) \
        REG_SET_BIT(&(reg),bVPU_SPP_ON_TOP)
#define VPU_SPP_ON_BOTTOM(reg) \
        REG_CLR_BIT(&(reg),bVPU_SPP_ON_TOP)

//PIC_CTRL
#define bVPU_SPP_EN               (0)
#define bVPU_SPP_V_DOWNSCALE      (2)
#define bVPU_SPP_H_DOWNSCALE      (3)
#define bVPU_SPP_VT_PHASE_BIAS    (0)
#define bVPU_SPP_VB_PHASE_BIAS    (16)
#define bVPU_SPP_PLAY_MODE        (29)

#define mVPU_SPP_EN               (0x01   << bVPU_SPP_EN)
#define mVPU_SPP_V_DOWNSCALE      (0x01   << bVPU_SPP_V_DOWNSCALE)
#define mVPU_SPP_H_DOWNSCALE      (0x01   << bVPU_SPP_H_DOWNSCALE)
#define mVPU_SPP_VT_PHASE_BIAS    (0x7FFF << bVPU_SPP_VT_PHASE_BIAS)
#define mVPU_SPP_VB_PHASE_BIAS    (0x7FFF << bVPU_SPP_VB_PHASE_BIAS)
#define mVPU_SPP_PLAY_MODE        (0x01   << bVPU_SPP_PLAY_MODE)


#define VPU_SPP_ENABLE(reg) do{             \
	while(!REG_GET_BIT(&(reg),bVPU_SPP_EN)){ \
		REG_SET_BIT(&(reg),bVPU_SPP_EN);      \
	}\
}while(0) \

#define VPU_SPP_DISABLE(reg) do{           \
	while(REG_GET_BIT(&(reg),bVPU_SPP_EN)){ \
		REG_CLR_BIT(&(reg),bVPU_SPP_EN);     \
	}\
}while(0)

#define VPU_SPP_V_DOWNSCALE_ENABLE(reg) \
	REG_SET_BIT(&(reg),bVPU_SPP_V_DOWNSCALE)
#define VPU_SPP_V_DOWNSCALE_DISABLE(reg) \
	REG_CLR_BIT(&(reg),bVPU_SPP_V_DOWNSCALE)

#define VPU_SPP_H_DOWNSCALE_ENABLE(reg) \
	REG_SET_BIT(&(reg),bVPU_SPP_H_DOWNSCALE)
#define VPU_SPP_H_DOWNSCALE_DISABLE(reg) \
	REG_CLR_BIT(&(reg),bVPU_SPP_H_DOWNSCALE)

#define VPU_SPP_SET_SPP_PHASE_0_H(reg, val)\
    REG_SET_VAL(&(reg), val);
#define VPU_SPP_SET_SPP_PHASE_0_V(reg, val)\
    REG_SET_VAL(&(reg), val);

#define VPU_SPP_SET_VT_PHASE(reg,value) \
	REG_SET_FIELD(&(reg), mVPU_SPP_VT_PHASE_BIAS,value,bVPU_SPP_VT_PHASE_BIAS)

#define VPU_SPP_SET_VB_PHASE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_VB_PHASE_BIAS,value,bVPU_SPP_VB_PHASE_BIAS)

#define VPU_SPP_SET_PLAY_MODE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_PLAY_MODE,value,bVPU_SPP_PLAY_MODE)

//PIC_PARA
enum {
	VPU_SPP_PLAY_MODE_FIELD = 0,
	VPU_SPP_PLAY_MODE_FRAME,
};

enum {
	VPU_SPP_COLOR_TYPE_420 = 0,
	VPU_SPP_COLOR_TYPE_422_Y_CBCR,
	VPU_SPP_COLOR_TYPE_422_CBYCRY,
	VPU_SPP_COLOR_TYPE_6442_YCBCRA,
};

#define bVPU_SPP_BASE_LINE         (0)
#define mVPU_SPP_BASE_LINE         (0x1FFF<<bVPU_SPP_BASE_LINE)
#define VPU_SPP_SET_BASE_LINE(reg,value) \
	GX_SET_FEILD(&(reg),mVPU_SPP_BASE_LINE,value,bVPU_SPP_BASE_LINE)

#define bVPU_SPP_COLOR_TYPE	(30)
#define mVPU_SPP_COLOR_TYPE	(0x3 << bVPU_SPP_COLOR_TYPE)
#define VPU_SPP_SET_COLOR_TYPE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_COLOR_TYPE,value,bVPU_SPP_COLOR_TYPE)

//PIC_Y_TOP_ADDR
#define bVPU_SPP_Y_TOP_ADDR          (0)
#define mVPU_SPP_Y_TOP_ADDR          (SPP_ADDR_MASK<<bVPU_SPP_Y_TOP_ADDR)
#define VPU_SPP_SET_Y_TOP_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_Y_TOP_ADDR,(unsigned int)gx_virt_to_phys(value),bVPU_SPP_Y_TOP_ADDR)
//PIC_Y_BOTTOM_ADDR
#define bVPU_SPP_Y_BOTTOM_ADDR          (0)
#define mVPU_SPP_Y_BOTTOM_ADDR          (SPP_ADDR_MASK<<bVPU_SPP_Y_BOTTOM_ADDR)
#define VPU_SPP_SET_Y_BOTTOM_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_Y_BOTTOM_ADDR,(unsigned int)gx_virt_to_phys(value),bVPU_SPP_Y_BOTTOM_ADDR)
//PIC_UV_TOP_ADDR
#define bVPU_SPP_UV_TOP_ADDR          (0)
#define mVPU_SPP_UV_TOP_ADDR          (SPP_ADDR_MASK<<bVPU_SPP_UV_TOP_ADDR)
#define VPU_SPP_SET_UV_TOP_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_UV_TOP_ADDR,(unsigned int)gx_virt_to_phys(value),bVPU_SPP_UV_TOP_ADDR)
//PIC_UV_BOTTOM_ADDR
#define bVPU_SPP_UV_BOTTOM_ADDR          (0)
#define mVPU_SPP_UV_BOTTOM_ADDR          (SPP_ADDR_MASK<<bVPU_SPP_UV_BOTTOM_ADDR)
#define VPU_SPP_SET_UV_BOTTOM_ADDR(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_UV_BOTTOM_ADDR,(unsigned int)gx_virt_to_phys(value),bVPU_SPP_UV_BOTTOM_ADDR)

//PIC_SIZE
#define bVPU_SPP_PIC_SIZE_WIDTH (0)
#define bVPU_SPP_PIC_SIZE_HIGH  (16)
#define mVPU_SPP_PIC_SIZE_WIDTH (0x7FF << bVPU_SPP_PIC_SIZE_WIDTH)
#define mVPU_SPP_PIC_SIZE_HIGH  (0x7FF << bVPU_SPP_PIC_SIZE_HIGH)

#define VPU_SPP_SET_PIC_SIZE(reg,width,high) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_SPP_PIC_SIZE_WIDTH,width,bVPU_SPP_PIC_SIZE_WIDTH); \
		REG_SET_FIELD(&(reg),mVPU_SPP_PIC_SIZE_HIGH,high,bVPU_SPP_PIC_SIZE_HIGH); \
	}while (0)

//PIC_SOURECE_SIZE
#define bVPU_SPP_PIC_SOURCE_SIZE_WIDTH (0)
#define bVPU_SPP_PIC_SOURCE_SIZE_HIGH  (16)
#define mVPU_SPP_PIC_SOURCE_SIZE_WIDTH (0xFFF << bVPU_SPP_PIC_SOURCE_SIZE_WIDTH)
#define mVPU_SPP_PIC_SOURCE_SIZE_HIGH  (0x7FF << bVPU_SPP_PIC_SOURCE_SIZE_HIGH)

#define VPU_SPP_SET_PIC_SOURCE_SIZE(reg,width,high) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_SPP_PIC_SOURCE_SIZE_WIDTH,width,bVPU_SPP_PIC_SOURCE_SIZE_WIDTH); \
		REG_SET_FIELD(&(reg),mVPU_SPP_PIC_SOURCE_SIZE_HIGH,high,bVPU_SPP_PIC_SOURCE_SIZE_HIGH); \
	}while (0)

//PIC_POSITION
#define bVPU_SPP_PIC_POSITION_X (0)
#define bVPU_SPP_PIC_POSITION_Y (16)
#define mVPU_SPP_PIC_POSITION_X (0x3FF << bVPU_SPP_PIC_POSITION_X)
#define mVPU_SPP_PIC_POSITION_Y (0x3FF << bVPU_SPP_PIC_POSITION_Y)

#define VPU_SPP_SET_PIC_POSITION(reg,x,y) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_SPP_PIC_POSITION_X,x,bVPU_SPP_PIC_POSITION_X); \
		REG_SET_FIELD(&(reg),mVPU_SPP_PIC_POSITION_Y,y,bVPU_SPP_PIC_POSITION_Y); \
	}while (0)

//PIC_ZOOM
#define VPU_SPP_ZOOM_WIDTH (12)
#define VPU_SPP_ZOOM_NONE (1 << VPU_SPP_ZOOM_WIDTH)
#define VPU_SPP_ZOOM_OUT_MAX (VPU_SPP_ZOOM_NONE << 2)

#define bVPU_SPP_VZOOM            (0)
#define bVPU_SPP_HZOOM            (16)
#define mVPU_SPP_VZOOM            (0x7FFF<<bVPU_SPP_VZOOM)
#define mVPU_SPP_HZOOM            (0x7FFF<<bVPU_SPP_HZOOM)

#define VPU_SPP_SET_ZOOM(reg,hzoom,vzoom) \
	do { \
		REG_SET_FIELD(&(reg),mVPU_SPP_HZOOM,hzoom,bVPU_SPP_HZOOM); \
		REG_SET_FIELD(&(reg),mVPU_SPP_VZOOM,vzoom,bVPU_SPP_VZOOM); \
	}while (0)

//PIC_LEN_PHASE
#define bVPU_SPP_REQ_LENGTH       (0)
#define bVPU_SPP_INTP_PHASE       (16)

#define mVPU_SPP_REQ_LENGTH       (0x03FF<<bVPU_SPP_REQ_LENGTH)
#define mVPU_SPP_INTP_PHASE       (0x00FF<<bVPU_SPP_INTP_PHASE)

#define VPU_SPP_SET_LINE_REQ_LENGTH(reg,value) \
        REG_SET_FIELD(&(reg),mVPU_SPP_REQ_LENGTH,value,bVPU_SPP_REQ_LENGTH)

#define VPU_SPP_SET_INTP_PHASE(reg,value) \
	REG_SET_FIELD(&(reg),mVPU_SPP_INTP_PHASE,value,bVPU_SPP_INTP_PHASE)

//BUFFER_CTRL1
#define bVPU_SPP_REQ_DATA_LEN     (16)
#define mVPU_SPP_REQ_DATA_LEN     (0x7FF<<bVPU_SPP_REQ_DATA_LEN)

#define VPU_SPP_SET_BUFFER_REQ_DATA_LEN(reg,value) \
        REG_SET_FIELD(&(reg),mVPU_SPP_REQ_DATA_LEN,value,bVPU_SPP_REQ_DATA_LEN)

//PIC_PARA_V/H
#define VPU_SPP_SET_PHASE(reg,i,Value) \
	REG_SET_VAL(&(reg[i]),Value)

/*****************************************************************************/
/*                                   CE                                      */
/*****************************************************************************/
//CAP_CTRL
#define bVPU_CAP_EN               (0)
#define bVPU_CAP_LEVEL            (1)
#define bVPU_CAP_END              (4)
#define mVPU_CAP_EN               (0x1<<bVPU_CAP_EN)
#define mVPU_CAP_LEVEL            (0x7<<bVPU_CAP_LEVEL)
#define mVPU_CAP_END              (0x1<<bVPU_CAP_END)

#define VPU_CAP_SET_LEVEL(reg,level) \
        REG_SET_FIELD(&(reg),mVPU_CAP_LEVEL,level,bVPU_CAP_LEVEL)

#define VPU_CAP_START(reg) do{\
        REG_CLR_BIT(&(reg),bVPU_CAP_END);\
        REG_CLR_BIT(&(reg),bVPU_CAP_EN); \
        REG_SET_BIT(&(reg),bVPU_CAP_EN); \
        while(!REG_GET_BIT(&(reg),bVPU_CAP_END));\
}while(0)

//CAP_ADDR
#define bVPU_CAP_ADDR             (0)
#define mVPU_CAP_ADDR             (CE_ADDR_MASK<<bVPU_CAP_ADDR)

#define VPU_CAP_SET_PIC_ADDR(reg,addr) \
        REG_SET_FIELD(&(reg),mVPU_CAP_ADDR, addr,bVPU_CAP_ADDR)

//CAP_HEIGHT
#define bVPU_CAP_TOP              (0)
#define bVPU_CAP_BOTTOM           (16)
#define mVPU_CAP_TOP              (0x7FF<<bVPU_CAP_TOP)
#define mVPU_CAP_BOTTOM           (0x7FF<<bVPU_CAP_BOTTOM)

#define VPU_CAP_SET_PIC_VERTICAL(reg,top,bottom) \
        REG_SET_FIELD(&(reg),mVPU_CAP_TOP,top,bVPU_CAP_TOP);\
        REG_SET_FIELD(&(reg),mVPU_CAP_BOTTOM,bottom,bVPU_CAP_BOTTOM)

//CAP_WIDTH
#define bVPU_CAP_LEFT             (0)
#define bVPU_CAP_RIGHT            (16)
#define mVPU_CAP_RIGHT            (0x7FF<<bVPU_CAP_RIGHT)
#define mbVPU_CAP_LEFT            (0x7FF<<bVPU_CAP_LEFT)

#define VPU_CAP_SET_PIC_HORIZONTAL(reg,left,right) \
        REG_SET_FIELD(&(reg),mbVPU_CAP_LEFT,left,bVPU_CAP_LEFT);\
        REG_SET_FIELD(&(reg),mVPU_CAP_RIGHT,right,bVPU_CAP_RIGHT)

/*****************************************************************************/
/*                                   VBI                                     */
/*****************************************************************************/
//VBI_CTRL
#define bVPU_VBI_READ_ADDR        (0)
#define bVPU_VBI_DATA_LEN         (1)
#define bVPU_VBI_ENABLE           (0)
#define mVPU_VBI_DATA_LEN         (0x1F << bVPU_VBI_DATA_LEN)
#define mVPU_VBI_READ_ADDR        (0x3FFFFFF << bVPU_VBI_READ_ADDR)

#define VPU_VBI_START(reg) \
        REG_SET_BIT(&(reg),bVPU_VBI_ENABLE)
#define VPU_VBI_STOP(reg) \
        REG_CLR_BIT(&(reg),bVPU_VBI_ENABLE)
#define VPU_VBI_SET_DATA_LEN(reg,value) \
        REG_SET_FIELD(&(reg), mVPU_VBI_DATA_LEN, value, bVPU_VBI_DATA_LEN)

//VBI_ADDR
#define bVPU_VBI_DATA_ADDR        (0)
#define mVPU_VBI_DATA_ADDR        (0xFFFFFFFF << bVPU_VBI_DATA_ADDR)

#define VPU_VBI_SET_DATA_ADDR(reg,value) \
        REG_SET_FIELD(&(reg), mVPU_VBI_DATA_ADDR, value, bVPU_VBI_DATA_ADDR);

#define VPU_VBI_GET_READ_ADDR(reg,value) do{\
        int val ;\
        val = REG_GET_FIELD(&(reg), mVPU_VBI_DATA_ADDR,bVPU_VBI_DATA_ADDR);\
        value =(void*)val;\
}while(0)

// rDISP_CTRL
#define bVPU_DISP_CTRL_FILED_START        (0)
#define bVPU_DISP_CTRL_FILED_ERROR        (1)
#define bVPU_DISP_CTRL_FILED_ERROR_INT_EN (10)
#define bVPU_DISP_CTRL_FILED_START_INT_EN (11)
#define bVPU_DISP_CTRL_RST                (12)
#define bVPU_DISP_BUFF_CNT                (16)

#define mVPU_DISP_CTRL_FILED_ERROR        (0x1  << bVPU_DISP_CTRL_FILED_ERROR)
#define mVPU_DISP_CTRL_FILED_START        (0x1  << bVPU_DISP_CTRL_FILED_START)
#define mVPU_DISP_CTRL_FILED_ERROR_INT_EN (0x1  << bVPU_DISP_CTRL_FILED_ERROR_INT_EN)
#define mVPU_DISP_CTRL_FILED_START_INT_EN (0x1  << bVPU_DISP_CTRL_FILED_START_INT_EN)
#define mVPU_DISP_BUFF_CNT                (0xFF << bVPU_DISP_BUFF_CNT)

#define VPU_DISP_CLR_FILED_ERROR_INT(reg) \
	REG_SET_BIT(&(reg),bVPU_DISP_CTRL_FILED_ERROR)

#define VPU_DISP_CLR_FILED_START_INT(reg) \
	REG_SET_BIT(&(reg),bVPU_DISP_CTRL_FILED_START)

#define VPU_DISP_FILED_ERROR_INT_EN(reg) \
do{\
    unsigned int value = 0; \
    value = REG_GET_VAL(&(reg)); \
    value &= ~(0x3<<0 ); \
    value |= (1<<bVPU_DISP_CTRL_FILED_ERROR_INT_EN); \
	REG_SET_VAL(&(reg),value); \
}while(0)
#define VPU_DISP_FILED_ERROR_INT_DIS(reg) \
do {\
    unsigned int value = 0; \
    value = REG_GET_VAL(&(reg)); \
    value &= ~(0x3<<0 ); \
    value &= ~(1<<bVPU_DISP_CTRL_FILED_ERROR_INT_EN); \
    REG_SET_VAL(&(reg),value);\
}while(0)
#define VPU_DISP_FILED_START_INT_EN(reg) \
do{\
    unsigned int value = 0; \
    value = REG_GET_VAL(&(reg)); \
    value &= ~(0x3<<0 ); \
    value |= (1<<bVPU_DISP_CTRL_FILED_START_INT_EN); \
	REG_SET_VAL(&(reg),value); \
}while(0)
#define VPU_DISP_FILED_START_INT_DIS(reg) \
do{\
    unsigned int value = 0; \
    value = REG_GET_VAL(&(reg)); \
    value &= ~(0x3<<0 ); \
    value &= ~(1<<bVPU_DISP_CTRL_FILED_START_INT_EN); \
	REG_SET_VAL(&(reg),value); \
}while(0)

#define VPU_DISP_SET_RST(reg) \
	REG_SET_BIT(&(reg),bVPU_DISP_CTRL_RST)

#define VPU_DISP_CLR_RST(reg) \
	REG_CLR_BIT(&(reg),bVPU_DISP_CTRL_RST)

#define VPU_DISP_GET_BUFF_CNT(reg) \
	REG_GET_FIELD(&(reg), mVPU_DISP_BUFF_CNT, bVPU_DISP_BUFF_CNT)

// TV_DISP_VIEW
#define bVPU_TV_DISP_VIEW_ACTIVE_CNT (0)
#define bVPU_TV_DISP_VIEW_FIELD      (11)

#define mVPU_TV_DISP_VIEW_ACTIVE_CNT (0x7ff << bVPU_TV_DISP_VIEW_ACTIVE_CNT)
#define mVPU_TV_DISP_VIEW_FIELD      (0x1   << bVPU_TV_DISP_VIEW_FIELD)

#define VPU_DISP_GET_VIEW_ACTIVE_CNT(reg)  \
	REG_GET_FIELD(&(reg),mVPU_TV_DISP_VIEW_ACTIVE_CNT,bVPU_TV_DISP_VIEW_ACTIVE_CNT)

#define VPU_DISP_GET_VIEW_FIELD(reg) \
	REG_GET_BIT(&(reg),bVPU_TV_DISP_VIEW_FIELD)

#endif

