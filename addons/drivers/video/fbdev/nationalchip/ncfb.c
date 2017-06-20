#include <linux/err.h>
#include <linux/module.h>
#include <linux/fb.h>
#include <linux/platform_device.h>
#include <linux/semaphore.h>
#include <linux/dma-mapping.h>
#include <asm/uaccess.h>
#include <linux/font.h>
#include <linux/linux_logo.h>
#include "ncfb.h"
#include "gx6605s.h"

static __u32 xres = 1280;
static __u32 yres = 680;
static __u32 bits_per_pixel = 16;

static int __init ncfb_vpu_parse(char *p)
{
	char **pp;

	pp = &p;

	if (*p == '@')
		bits_per_pixel = memparse((*pp) + 1, pp);
	else {
		xres = memparse(*pp, pp);
		if (xres == 1080) {
			yres = 1080;
			xres = 1920;
		}
		if (xres == 720) {
			yres = 640;
			xres = 1280;
		}
	}

	if (**pp == 'x')
		yres = memparse((*pp) + 1, pp);
	else if (**pp == '@') {
		bits_per_pixel = memparse((*pp) + 1, pp);
		goto out;
	} else
		goto out;

	if (**pp == '@')
		bits_per_pixel = memparse((*pp) + 1, pp);
	else
		goto out;

out:
	if (bits_per_pixel == 1 && xres < 1280)
		xres = 1280;

	return 0;
}
__setup("vpu=", ncfb_vpu_parse);

void ncfb_save_state(struct fb_info *info)
{
	printk("hello gary-%s.\n", __func__);
	return;
}

void ncfb_restore_state(struct fb_info *info)
{
	printk("hello gary-%s.\n", __func__);
	return;
}

/* blank display */
int ncfb_blank(int blank, struct fb_info *info)
{
	printk("hello gary-%s.\n", __func__);
	return 0;
}

/* pan display */
int ncfb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	printk("%s.\n", __func__);
	gx6605s_fbinit(info);
	return 0;
}

static struct fb_ops ncfb_ops = {
	.owner          = THIS_MODULE,
	/* Generic function to draw */
	.fb_imageblit   = cfb_imageblit,
	.fb_copyarea    = cfb_copyarea,
	.fb_fillrect    = cfb_fillrect,
	.fb_blank	= ncfb_blank,
	.fb_pan_display	= ncfb_pan_display,
};

extern const struct linux_logo logo_linux_mono;
/* our vpu 1bpp problem, so we need to exchange bitseq in one byte.*/
static void exchange_font(void) {
	unsigned char *p;
	unsigned char tmp;
	unsigned int i;

#ifdef CONFIG_FONT_8x16
	p = (char *)font_vga_8x16.data;
	for ( i = 0; i < 4096; i++) {
		tmp = 0;
		tmp |= (*(p+i)&0x1) << 7;
		tmp |= (*(p+i)&0x2) << 5;
		tmp |= (*(p+i)&0x4) << 3;
		tmp |= (*(p+i)&0x8) << 1;
		tmp |= (*(p+i)&0x10) >> 1;
		tmp |= (*(p+i)&0x20) >> 3;
		tmp |= (*(p+i)&0x40) >> 5;
		tmp |= (*(p+i)&0x80) >> 7;
		*(p+i) = tmp;
	}
#endif
#ifdef CONFIG_FONT_8x8
	p = (char *)font_vga_8x8.data;
	for ( i = 0; i < 2048; i++) {
		tmp = 0;
		tmp |= (*(p+i)&0x1) << 7;
		tmp |= (*(p+i)&0x2) << 5;
		tmp |= (*(p+i)&0x4) << 3;
		tmp |= (*(p+i)&0x8) << 1;
		tmp |= (*(p+i)&0x10) >> 1;
		tmp |= (*(p+i)&0x20) >> 3;
		tmp |= (*(p+i)&0x40) >> 5;
		tmp |= (*(p+i)&0x80) >> 7;
		*(p+i) = tmp;
	}
#endif

#ifdef CONFIG_LOGO_LINUX_MONO
	p = (char *)logo_linux_mono.data;
	for ( i = 0; i < 788; i++) {
		tmp = 0;
		tmp |= (*(p+i)&0x1) << 7;
		tmp |= (*(p+i)&0x2) << 5;
		tmp |= (*(p+i)&0x4) << 3;
		tmp |= (*(p+i)&0x8) << 1;
		tmp |= (*(p+i)&0x10) >> 1;
		tmp |= (*(p+i)&0x20) >> 3;
		tmp |= (*(p+i)&0x40) >> 5;
		tmp |= (*(p+i)&0x80) >> 7;
		*(p+i) = tmp;
	}
#endif
}

static int ncfb_alloc_fbmem(struct fb_info *info) {
	struct ncfb_pri *fbi = info->par;
	dma_addr_t map_dma;
	unsigned map_size = PAGE_ALIGN(info->fix.smem_len);

	printk(KERN_WARNING "map_video_memory(fbi=%p) map_size %u\n", fbi, map_size);

	info->screen_base = dma_alloc_coherent(fbi->dev, map_size,
			&map_dma, GFP_KERNEL);

	if (info->screen_base) {
		/* prevent initial garbage on screen */
		printk(KERN_WARNING "map_video_memory: clear %p:%08x\n",
				info->screen_base, map_size);

		info->fix.smem_start = map_dma;

		printk(KERN_WARNING "map_video_memory: dma=%08lx cpu=%p size=%08x\n",
				info->fix.smem_start, info->screen_base, map_size);
	}

	memset(info->screen_base, 0xff, map_size);
	printk(KERN_WARNING "map_dma %x\n", map_dma);
	return info->screen_base ? 0 : -ENOMEM;
}

static u32 palette_buffer[16];
static struct fb_info *fbinfo = NULL;

static int ncfb_probe(struct platform_device *pdev)
{
	int ret = 0; int i = 0;
	struct ncfb_pri *fbi;

	if (!pdev) {
		printk(KERN_ERR "nc %s pdev %p!?\n", __func__, pdev);
		return -EINVAL;
	}

	/* alloc fb_info */
	fbinfo = framebuffer_alloc(sizeof(struct ncfb_pri), &pdev->dev);
	if (!fbinfo) {
		printk(KERN_ERR "nc %s framebuffer_alloc failed.\n", __func__);
		return -ENOMEM;
	}

	/* setup nc fbi */
	fbi = fbinfo->par;
	fbi->dev = &pdev->dev;

	/* init fb_info */
	fbinfo->var.nonstd = 0;
	fbinfo->var.activate = FB_ACTIVATE_NOW;
	fbinfo->var.accel_flags = 0;
	fbinfo->var.vmode = FB_VMODE_NONINTERLACED;

	fbinfo->flags = FBINFO_DEFAULT;

	fbinfo->var.xres = xres;
	fbinfo->var.yres = yres;
	fbinfo->var.xres_virtual = fbinfo->var.xres;
	fbinfo->var.yres_virtual = fbinfo->var.yres;
	fbinfo->var.xoffset = 0;
	fbinfo->var.yoffset = 0;

	fbinfo->var.bits_per_pixel = bits_per_pixel;

	fbinfo->var.red.msb_right = 0;
	fbinfo->var.green.msb_right = 0;
	fbinfo->var.blue.msb_right = 0;

	fbinfo->var.transp.offset = 0;
	fbinfo->var.transp.length = 0;
	fbinfo->var.transp.msb_right = 0;

	fbinfo->fix.type = FB_TYPE_PACKED_PIXELS;
	fbinfo->fix.type_aux = 0;
	fbinfo->fix.xpanstep = 0;
	fbinfo->fix.ypanstep = 0;
	fbinfo->fix.ywrapstep = 0;
	fbinfo->fix.accel = FB_ACCEL_NONE;

	fbinfo->fix.smem_len = fbinfo->var.xres;

	if (fbinfo->var.bits_per_pixel == 1) {
		fbinfo->fix.visual = FB_VISUAL_MONO10;
		exchange_font();
		fbinfo->var.red.offset = 0;
		fbinfo->var.green.offset = 0;
		fbinfo->var.blue.offset = 0;
		fbinfo->var.red.length = 1;
		fbinfo->var.green.length = 1;
		fbinfo->var.blue.length = 1;
		fbinfo->fix.line_length = fbinfo->var.xres/8;
		fbinfo->fix.smem_len *= fbinfo->var.yres/8;
	} else {
		fbinfo->fix.visual = FB_VISUAL_TRUECOLOR;
		fbinfo->var.red.offset = 11;
		fbinfo->var.green.offset = 5;
		fbinfo->var.blue.offset = 0;
		fbinfo->var.red.length = 5;
		fbinfo->var.green.length = 6;
		fbinfo->var.blue.length = 5;
		fbinfo->fix.line_length = fbinfo->var.xres * (fbinfo->var.bits_per_pixel >> 3);
		fbinfo->fix.smem_len *= fbinfo->var.yres * (fbinfo->var.bits_per_pixel >> 3);
	}

	fbinfo->pseudo_palette = palette_buffer;
	for (i = 0; i < 16; i++) {
		palette_buffer[i] = i|i<<4| i<<8|i<<12|i<<16|i<<20|i<<24|i<<28;
	}

	fbinfo->fbops = &ncfb_ops;

	/* alloc framebuffer */
	ret = ncfb_alloc_fbmem(fbinfo);
	if (ret) {
		printk(KERN_ERR "nc %s alloc_fbmem failed: %d.\n", __func__, ret);
		return ret;
	}

	ret = gx6605s_fbinit(fbinfo);
	if (ret) {
		printk(KERN_ERR "nc %s gx6605s_fbinit failed: %d.\n", __func__, ret);
		return ret;
	}

	/* register framebuffer */
	if (register_framebuffer(fbinfo)) {
		printk(KERN_ERR "nc %s register_framebuffer err.\n", __func__);
		framebuffer_release(fbinfo);
		return -EINVAL;
	}

	return 0;
}

static int ncfb_remove(struct platform_device *pdev)
{
	struct fb_info *info = NULL;

	info = platform_get_drvdata(pdev);
	if (!info)
		return -EINVAL;

	unregister_framebuffer(info);

	framebuffer_release(info);

	platform_set_drvdata(pdev,NULL);

	return 0;
}

static u64 ncfb_dmamask = ~(u32)0;
static void release(struct device *dev) {return;}
static struct platform_device ncfb_device = {
	.name		= "ncfb",
	.id		= 0,
	.dev		= {
		.dma_mask          = &ncfb_dmamask,
		.coherent_dma_mask = 0xffffffff,
		.release           = release,
	},
};

void ncfb_resume(void)
{
	gx6605s_fbinit(fbinfo);
}

#ifdef CONFIG_PM
static int ncfb_internal_resume(struct platform_device *dev)
{
	printk("%s. %d.\n", __func__, __LINE__);
	ncfb_resume();
	return 0;
}

extern void ncfb_hdmi_write(unsigned char addr, unsigned char data);
static int ncfb_internal_suspend(struct platform_device *dev, pm_message_t state)
{
	printk("%s. %d.\n", __func__, __LINE__);
	ncfb_hdmi_write(0x00,0x20);
	return 0;
}
#endif

static struct platform_driver ncfb_driver =
{
	.probe  = ncfb_probe,
	.remove = ncfb_remove,
#ifdef CONFIG_PM
	.suspend = ncfb_internal_suspend,
	.resume = ncfb_internal_resume,
#endif
	.driver		= {
		.name	= "ncfb",
		.owner	= THIS_MODULE,
	},
};

static int __init ncfb_init(void)
{
	platform_device_register(&ncfb_device);
	return platform_driver_register(&ncfb_driver);
}

static void __exit ncfb_exit(void)
{
	platform_driver_unregister(&ncfb_driver);
	platform_device_unregister(&ncfb_device);
}

EXPORT_SYMBOL(ncfb_resume);

module_init(ncfb_init);
module_exit(ncfb_exit);

