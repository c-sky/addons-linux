#ifndef __GX6605S_VPU_REG_H__
#define __GX6605S_VPU_REG_H__

#include "gxav_bitops.h"
#include "gx6605s.h"

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

#define bVPU_RW_BYTESEQ_HIGH	(28)
#define bVPU_RW_BYTESEQ_LOW		(20)
#define mVPU_RW_BYTESEQ_LOW		(0x3<<bVPU_RW_BYTESEQ_LOW)

#define VPU_SET_RW_BYTESEQ(reg, byte_seq) \
	do \
{ \
	REG_SET_FIELD(&(reg), mVPU_RW_BYTESEQ_LOW, byte_seq&0x3, bVPU_RW_BYTESEQ_LOW); \
	if(byte_seq>>2) \
	REG_SET_BIT(&(reg), bVPU_RW_BYTESEQ_HIGH); \
	else \
	REG_CLR_BIT(&(reg), bVPU_RW_BYTESEQ_HIGH); \
	REG_SET_FIELD(&(reg), 0x7<<12, byte_seq&0x7, 12); \
}while(0)

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

#endif
