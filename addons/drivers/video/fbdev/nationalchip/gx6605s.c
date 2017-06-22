#include <linux/err.h>
#include <linux/fb.h>
#include <linux/platform_device.h>
#include <linux/semaphore.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include "ncfb.h"
#include "gx6605s.h"
#include "gx6605s_vpu_reg.h"

static volatile Gx6605sVpuReg *gxvpu_reg = NULL;
static OsdRegionHead *gxosd_head_ptr = NULL;
static dma_addr_t gxosd_head_dma;

static u32 byte_seq = 0;

static int gxosd_enable(int enable)
{
	if(enable == 0)
		VPU_OSD_DISABLE(gxvpu_reg->rOSD_CTRL);
	else {
		VPU_OSD_SET_FIRST_HEAD(gxvpu_reg->rOSD_FIRST_HEAD_PTR, gxosd_head_dma);
		VPU_OSD_ENABLE(gxvpu_reg->rOSD_CTRL);
	}

	return 0;
}

typedef struct gxav_rect {
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
} GxAvRect;

typedef enum {
	DCBA_HGFE = 0,
	EFGH_ABCD = 1,
	HGFE_DCBA = 2,
	ABCD_EFGH = 3,
	CDAB_GHEF = 4,
	FEHG_BADC = 5,
	GHEF_CDAB = 6,
	BADC_FEHG = 7,
}ByteSequence;

static int gxosd_alpha(u32 alpha)
{
	VPU_OSD_GLOBAL_ALHPA_ENABLE(gxosd_head_ptr->word1);
	VPU_OSD_SET_MIX_WEIGHT(gxosd_head_ptr->word1, alpha);
	VPU_OSD_SET_ALPHA_RATIO_DISABLE(gxosd_head_ptr->word7);

	return 0;
}
typedef enum   {
	GX_COLOR_FMT_CLUT1 = 0, //0
	GX_COLOR_FMT_CLUT2,     //1
	GX_COLOR_FMT_CLUT4,     //2
	GX_COLOR_FMT_CLUT8,     //3
	GX_COLOR_FMT_RGBA4444,  //4
	GX_COLOR_FMT_RGBA5551,  //5
	GX_COLOR_FMT_RGB565,    //6
	GX_COLOR_FMT_RGBA8888,  //7
	GX_COLOR_FMT_RGB888,    //8
	GX_COLOR_FMT_BGR888,    //9

	GX_COLOR_FMT_ARGB4444, //10
	GX_COLOR_FMT_ARGB1555,  //11
	GX_COLOR_FMT_ARGB8888,  //12

	GX_COLOR_FMT_YCBCR422,  //13
	GX_COLOR_FMT_YCBCRA6442,//14
	GX_COLOR_FMT_YCBCR420,  //15

	GX_COLOR_FMT_YCBCR420_Y_UV, //16
	GX_COLOR_FMT_YCBCR420_Y_U_V,//17
	GX_COLOR_FMT_YCBCR420_Y,    //18
	GX_COLOR_FMT_YCBCR420_U,    //19
	GX_COLOR_FMT_YCBCR420_V,    //20
	GX_COLOR_FMT_YCBCR420_UV,   //21

	GX_COLOR_FMT_YCBCR422_Y_UV, //22
	GX_COLOR_FMT_YCBCR422_Y_U_V,//23
	GX_COLOR_FMT_YCBCR422_Y,    //24
	GX_COLOR_FMT_YCBCR422_U,    //25
	GX_COLOR_FMT_YCBCR422_V,    //26
	GX_COLOR_FMT_YCBCR422_UV,   //27

	GX_COLOR_FMT_YCBCR444,      //28
	GX_COLOR_FMT_YCBCR444_Y_UV, //29
	GX_COLOR_FMT_YCBCR444_Y_U_V,//30
	GX_COLOR_FMT_YCBCR444_Y,    //31
	GX_COLOR_FMT_YCBCR444_U,    //32
	GX_COLOR_FMT_YCBCR444_V,    //33
	GX_COLOR_FMT_YCBCR444_UV,   //34

	GX_COLOR_FMT_YCBCR400,      //35
	GX_COLOR_FMT_A8,            //36
	GX_COLOR_FMT_ABGR4444,      //37
	GX_COLOR_FMT_ABGR1555,      //39
	GX_COLOR_FMT_Y8,            //40
	GX_COLOR_FMT_UV16,          //41
	GX_COLOR_FMT_YCBCR422v,     //42
	GX_COLOR_FMT_YCBCR422h,     //43
} GxColorFormat;

static int gxosd_color_format(GxColorFormat format)
{
	int true_color_mode;

	if(format <= GX_COLOR_FMT_CLUT8)
		VPU_OSD_CLR_ZOOM_MODE_EN_IPS(gxvpu_reg->rOSD_CTRL);
	else
		VPU_OSD_SET_ZOOM_MODE_EN_IPS(gxvpu_reg->rOSD_CTRL);

	if((format >= GX_COLOR_FMT_RGBA8888)&&(format <= GX_COLOR_FMT_BGR888)) {
		true_color_mode = format - GX_COLOR_FMT_RGBA8888;
		VPU_OSD_SET_COLOR_TYPE(gxosd_head_ptr->word1, 0x7);
		VPU_OSD_SET_TRUE_COLOR_MODE(gxosd_head_ptr->word1, true_color_mode);
	} else {
		if( (format==GX_COLOR_FMT_ARGB4444)|| (format==GX_COLOR_FMT_ARGB1555)||
				(format==GX_COLOR_FMT_ARGB8888))
			VPU_OSD_SET_ARGB_CONVERT(gxosd_head_ptr->word1,1);
		else
			VPU_OSD_SET_ARGB_CONVERT(gxosd_head_ptr->word1,0);

		switch(format)
		{
		case GX_COLOR_FMT_ARGB4444:
			format = GX_COLOR_FMT_RGBA4444;
			break;
		case GX_COLOR_FMT_ARGB1555:
			format = GX_COLOR_FMT_RGBA5551;
			break;
		case GX_COLOR_FMT_ARGB8888:
			format = GX_COLOR_FMT_RGBA8888;
			break;
		default:
			break;
		}
		VPU_OSD_SET_COLOR_TYPE(gxosd_head_ptr->word1, format);
	}

	return 0;
}
static u32 palette_buffer[2];
static int gxosd_main_surface(struct fb_info *info)
{
	GxAvRect rect={0,0,0,0};
	unsigned int request_block=0, bpp=1;
	struct ncfb_pri *fbi = info->par;

	if (gxosd_head_ptr == NULL)
		gxosd_head_ptr = dma_alloc_coherent(fbi->dev, PAGE_ALIGN(sizeof(*gxosd_head_ptr)), &gxosd_head_dma, GFP_KERNEL);

	if (gxosd_head_ptr == NULL) {
		/* prevent initial garbage on screen */
		printk(KERN_ERR "%s, dma_alloc_coherent err.", __func__);
		while(1);
	}

	printk("osd_head_ptr: %p, %x.\n", gxosd_head_ptr, gxosd_head_dma);
	rect.width = info->var.xres;
	rect.height= info->var.yres;
	bpp = info->var.bits_per_pixel;
	if (bpp == 1) {
		request_block = rect.width/4/128*128/8;
	} else
		request_block = rect.width*(bpp>>3)/4/128*128;
	if(request_block < 128) {
		request_block = 128;
	}
	else if(request_block > 896) {
		request_block = 896;
	}

	if(bpp == 32)
		byte_seq = ABCD_EFGH;
	if(bpp == 16)
		byte_seq = CDAB_GHEF;
	if(bpp <= 8)
		byte_seq = DCBA_HGFE;

	VPU_SET_RW_BYTESEQ(gxvpu_reg->rSYS_PARA, byte_seq);
	printk("%s,byte_seq: %x.\n", __func__, byte_seq);

	REG_SET_FIELD(&(gxvpu_reg->rBUFF_CTRL2),0x7FF<<0,request_block,0);
	VPU_OSD_SET_WIDTH(gxosd_head_ptr->word3, 64, rect.width + rect.x - 1);
	VPU_OSD_SET_HEIGHT(gxosd_head_ptr->word4, 0, rect.height + rect.y - 1);
	VPU_OSD_SET_POSITION(gxvpu_reg->rOSD_POSITION, rect.x, rect.y);

	palette_buffer[1] = 0xff00ff00;
	palette_buffer[0] = 0x0;

	gxosd_alpha(0xff);
	if (bpp == 1) {
		gxosd_color_format(GX_COLOR_FMT_CLUT1);
		VPU_OSD_SET_CLUT_PTR(gxosd_head_ptr->word2, virt_to_phys(palette_buffer));
		VPU_OSD_SET_CLUT_LENGTH(gxosd_head_ptr->word1, 0);
		VPU_OSD_CLUT_UPDATA_ENABLE(gxosd_head_ptr->word1);
	} else {
		gxosd_color_format(GX_COLOR_FMT_RGB565);
	}

	VPU_OSD_SET_DATA_ADDR(gxosd_head_ptr->word5, info->fix.smem_start);
	VPU_OSD_SET_DATA_ADDR(gxosd_head_ptr->word6, gxosd_head_dma);

	VPU_OSD_SET_FIRST_HEAD(gxvpu_reg->rOSD_FIRST_HEAD_PTR, gxosd_head_dma);
	VPU_OSD_LIST_END_ENABLE(gxosd_head_ptr->word7);
	VPU_OSD_SET_BASE_LINE(gxosd_head_ptr->word7,rect.width);

	VPU_OSD_CLR_ZOOM_MODE(gxvpu_reg->rOSD_CTRL);

	gxvpu_reg->rOSD_VIEW_SIZE = (rect.height << 16)|rect.width;
	gxvpu_reg->rOSD_ZOOM = 0x10001000;

	return 0;
}

int gx6605s_fbinit(struct fb_info *info) {
	int ret = 0;

	if (gxvpu_reg == NULL) {
		gxvpu_reg = ioremap(0x04800000, sizeof(*gxvpu_reg));
		if (!gxvpu_reg) {
			printk(KERN_ERR "%s, ioremap failed.\n", __func__);
			return -ENOMEM;
		}
	}

	ret = gxosd_main_surface(info);

	gxosd_enable(1);

	return ret;
}

