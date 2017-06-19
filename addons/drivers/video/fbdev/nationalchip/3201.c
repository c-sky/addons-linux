#include <linux/err.h>
#include <linux/fb.h>
#include <linux/platform_device.h>
#include <linux/semaphore.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include "ncfb.h"
#include "3201.h"
#include "gx3201_vpu_reg.h"

#define ENABLE_PAL
#define ENABLE_YPBPR_HDMI_720P_50HZ
#define ENABLE_YPBPR_HDMI_720P_60HZ
#define ENABLE_YPBPR_HDMI_1080I_50HZ
#define ENABLE_YPBPR_HDMI_1080I_60HZ

static volatile Gx3201VpuReg *gx3201vpu_reg = NULL;
static volatile u8 *vpu_vout_base_addr = NULL;
static volatile Gx3201SvpuReg *gx3201svpu_reg = NULL;
static volatile u8 *svpu_vout_base_addr = NULL;
static volatile struct config_regs *gx3201_config_reg = NULL;
static OsdRegionHead *gx3201osd_head_ptr = NULL;
static dma_addr_t gx3201osd_head_dma;

static u32 *p_vpu_init = NULL;
static u32 *p_svpu_init = NULL;
static u32 vpu_init_cnt = 0;
static u32 svpu_init_cnt = 0;
static int svpu_mode = 0;
static GxVideoOutProperty_Mode vpu_hdmi_vout_mode = 0;
static unsigned int gx3201_hdmi_out_mode = HDMI_RGB_OUT;
static u32 byte_seq = 0;

/////////////////////  CVBS PAL/NTSC  /////////////////////
#ifdef ENABLE_PAL
static unsigned int gx3201_dve_pal_bdghi[] = {
	0x00010000, 0x00000000, 0x0097D47E, 0x001AE677,
	0x00781E13, 0x00F09618, 0x25D03940, 0x21C39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x69973F3F, 0x00013F37, 0x00000000, 0x00000098,
	0x00000000, 0x00000000, 0x000000D7, 0x00000000,
	0x00002C22, 0x008004B0, 0x0003F2E0, 0x06400000,
	0x00000018, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xAAAAAAAA, 0x000000A9,
	0x00000000, 0x0001FFFF, 0x10002000, 0x40826083,
	0xA082B082, 0xD082F000, 0x0026135F, 0x00002093,
	0x028506BF, 0x00001096, 0x0000002C, 0x00025625,
	0x00004AC4, 0x0000A935, 0x000A726E, 0x0009BA70,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08A8F20D,
	0x08A8F20D, 0x08A8F20D, 0x1705C170, 0x00000180
};
#endif

#ifdef ENABLE_PAL_M
static unsigned int gx3201_dve_pal_m[] = {
	0x00010001, 0x00000000, 0x009de17e, 0x001A2671,
	0x00781e13, 0x00b0a610, 0x25d05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6b953c47, 0x00004737, 0x00000000, 0x00000089,
	0x00000000, 0x00000000, 0x000000c2, 0x00000000,
	0x00002C22, 0x008004b0, 0x0003F2E0, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0026135F, 0x00002093,
	0xa000b083, 0x00001096, 0x0000002c, 0x00025625,
	0x00004AC4, 0x00007901, 0x0008c208, 0x0008320b,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000180
};
#endif

#ifdef ENABLE_PAL_N
static unsigned int gx3201_dve_pal_n[] = {
	0x00010002, 0x00000000, 0x0097d47e, 0x001AE677,
	0x00781e13, 0x00b0a610, 0x25d05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x69973C47, 0x00003F37, 0x00000000, 0x00000089,
	0x00000000, 0x00000000, 0x000000c2, 0x00000000,
	0x00002C22, 0x008004b0, 0x0003F2E0, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0026135F, 0x00002093,
	0x028506BF, 0x00001096, 0x0000002c, 0x00025625,
	0x00004AC4, 0x0000A935, 0x000A726E, 0x0009BA70,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

#ifdef ENABLE_PAL_NC
static unsigned int gx3201_dve_pal_nc[] = {
	0x00010003, 0x00000000, 0x0097da7e, 0x001AE677,
	0x00781e13, 0x00b0a610, 0x25d05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x69973F3F, 0x00003F37, 0x00000000, 0x00000089,
	0x00000000, 0x00000000, 0x000000c2, 0x00000000,
	0x00002C22, 0x008004b0, 0x0003F2E0, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0026135F, 0x00002093,
	0x028506BF, 0x00001096, 0x0000002c, 0x00025625,
	0x00004AC4, 0x0000A935, 0x000A726E, 0x0009BA70,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

#ifdef ENABLE_NTSC_M
static unsigned int gx3201_dve_ntsc_m[] = {
	0x00010004, 0x00000000, 0x0090d47e, 0x0017a65d,//0x001A2671,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x0004b800, 0x00000000,
	0x64803c47, 0x00003f37, 0x00000000, 0x0000008e,
	0x00000000, 0x00000000, 0x000000c6, 0x00000000,
	0x00002C22, 0x008004B0, 0x0003D2D8, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x001FA359, 0x00002093,
	0x028506B3, 0x00001096, 0x0000002c, 0x00025625,
	0x00004AC4, 0x00007901, 0x0008C208, 0x0008320B,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

#ifdef ENABLE_NTSC_443
static unsigned int gx3201_dve_ntsc_443[] = {
	0x00010005, 0x00000000, 0x0090c67e, 0x001A2671,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x0004b800, 0x00000000,
	0x64803c47, 0x00003f37, 0x00000000, 0x0000008e,
	0x00000000, 0x00000000, 0x000000c6, 0x00000000,
	0x00002C22, 0x008004B0, 0x0003D2D8, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x001FA359, 0x00002093,
	0x028506B3, 0x00001096, 0x0000002c, 0x00025625,
	0x00004AC4, 0x00007901, 0x0008C208, 0x0008320B,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

/////////////////////  YPbPr/HDMI  /////////////////////
#ifdef ENABLE_YPBPR_HDMI_480I
static unsigned int gx3201_dve_ycbcr_480i[] = {
	0x14010004, 0x00000000, 0x0097D47E, 0x0015c64f,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00002F22, 0x008004B0, 0x0003D2D8, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x001FA359, 0x00002093,
	0x028506B3, 0x00001096, 0x0000002c, 0x00025625,
	0x00004AC4, 0x00007901, 0x0008C208, 0x0008320B,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_576I
static unsigned int gx3201_dve_ycbcr_576i[] = {
	0x14010000, 0x00000000, 0x0097D47E, 0x00182661,//0x001BE67F
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00002F22, 0x008004B0, 0x0003F2E0, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0026135f, 0x00002093,
	0x028506BF, 0x00001096, 0x0000002c, 0x00025625,
	0x00004AC4, 0x0000a935, 0x000a726e, 0x0009ba70,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0xC00001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_480P
static unsigned int gx3201_dve_ypbpr_480p[] = {
	0x14010008, 0x00000000, 0x0097D43F, 0x00162651,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00003810, 0x008004B0, 0x0207e5b0, 0x06400000,//0x0081f2d8
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x001fa359, 0x00002093,
	0x028506b3, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x00011A02, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_576P
static unsigned int gx3201_dve_ypbpr_576p[] = {
	0x14010009, 0x00000000, 0x0097D43F, 0x00188664,//0x001B867C
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x0000341b, 0x008004B0, 0x0207e5C0, 0x06400000,//0x0081f2e0
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0026135f, 0x00002093,
	0x028506bf, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x00014267, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_50HZ
static unsigned int gx3201_dve_ypbpr_720p_50hz[] = {
	0x1405000D, 0x00000000, 0x0097D40A, 0x0042cc16,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00001d12, 0x00a584B0, 0x0207e671, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x002E87BB, 0x00002093,
	0x02850F77, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x0000C2E7, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_60HZ
static unsigned int gx3201_dve_ypbpr_720p_60hz[] = {
	0x1405000D, 0x00000000, 0x0097D40A, 0x0042cc16,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00001d12, 0x00a584B0, 0x0207e62C, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x002E8671, 0x00002093,
	0x02850CE3, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x0000C2E7, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_50HZ
static unsigned int gx3201_dve_ypbpr_1080i_50hz[] = {
	0x14050006, 0x00000000, 0x0097D40A, 0x002e1070,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00003129, 0x00aC04B0, 0x0207e83E, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0045Aa4f, 0x00002093,
	0x058B149F, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x00009A2E, 0x00123461, 0x00119464,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_60HZ
static unsigned int gx3201_dve_ypbpr_1080i_60hz[] = {
	0x14050006, 0x00000000, 0x0097D40A, 0x002e1070,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00003129, 0x00aC04B0, 0x0207e83E, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0045A897, 0x00002093,
	0x058B112D, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x00009A2E, 0x00123461, 0x00119464,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_50HZ
static unsigned int gx3201_dve_ypbpr_1080p_50hz[] = {
	0x1405000E, 0x00000000, 0x0097D40A, 0x00190848,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00001612, 0x008B04B0, 0x0081F436, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0045F527, 0x00002093,
	0x02C58a4f, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x0001445F, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_60HZ
static unsigned int gx3201_dve_ypbpr_1080p_60hz[] = {
	0x1405000E, 0x00000000, 0x0097D40A, 0x00190848,
	0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
	0x20885218, 0x2010F094, 0x00050000, 0x00000000,
	0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
	0x00000000, 0x00000000, 0x0000009d, 0x00000000,
	0x00001612, 0x008B04B0, 0x0081F436, 0x06400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
	0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x0045F44B, 0x00002093,
	0x02C58897, 0x00001096, 0x04000024, 0x00025625,
	0x00004AC4, 0x0001445F, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x10002083, 0x40866082,
	0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
	0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

static void Gx3201_Vpu_SetBufferStateDelay(int v)
{
	VPU_SET_BUFF_STATE_DELAY(gx3201vpu_reg->rBUFF_CTRL2, v);
}

static void config_videoout_ClockDIVConfig(unsigned int clock, unsigned int div)
{
	if (div)
		REG_SET_BIT(&(gx3201_config_reg->cfg_clk), clock);
	else
		REG_CLR_BIT(&(gx3201_config_reg->cfg_clk), clock);
}

static void config_videoout_ClockSourceSel(unsigned int clock, unsigned int source)
{
	if (source)
		REG_SET_BIT(&(gx3201_config_reg->cfg_source_sel), clock);
	else
		REG_CLR_BIT(&(gx3201_config_reg->cfg_source_sel), clock);
}

static void Gx3201_Vpu_GetScanInfo(int *scan_line, int *top)
{
	int tmp = REG_GET_VAL(&(gx3201vpu_reg->rSCAN_LINE));
	*scan_line = tmp & 0x7ff;
	*top       = tmp & 0x800;
}

static void hdmi_write(unsigned char addr, unsigned char data)
{
	if(addr==0x0) {
		//R/W System control reg address 0x00
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 0);     //clock =1
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff << 11), 0, 11);   //address=0x00
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff <<  3), data, 3);//write data
		REG_CLR_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 0);     //clock =0
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=1
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 0);     //clock =1
		REG_CLR_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 0);     //clock =0
		REG_CLR_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=0
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 0);     //clock =1
	}
	else {
		//R/W System control reg address 0x01~0xff
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff << 11), addr, 11);  // address
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff << 11), addr, 11);  // address
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff << 11), addr, 11);  // address
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff << 11), addr, 11);  // address
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff <<  3), data,  3);  // write data
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff <<  3), data,  3);  // write data
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff <<  3), data,  3);  // write data
		REG_SET_FIELD(&gx3201_config_reg->cfg_hdmi_sel, (0xff <<  3), data,  3);  // write data
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=1
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=1
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=1
		REG_SET_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=1
		REG_CLR_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=0
		REG_CLR_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=0
		REG_CLR_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=0
		REG_CLR_BIT  (&gx3201_config_reg->cfg_hdmi_sel, 1);     //we=0
	}
}

void ncfb_hdmi_write(unsigned char addr, unsigned char data)
{
	hdmi_write(addr, data);
}

/* add to fix some tv audio noise when show logo. */
static void gx3201_audio_spdif_reset(void)
{
	/* AUDIO_SPDIF_CTRL 27bit: ~ Hbr_soft_n,  1 for reset */
	*(volatile unsigned int*)0xa4c00000 |= 1<<27;
}

static void gx3201_hdmi_mode_select(void)
{
	hdmi_write(0x00,0x2c);
	msleep(2);
	hdmi_write(0x00,0x20);
	msleep(2);
}

static int hdmi_video_set_mode(GxVideoOutProperty_Mode vout_mode)
{
	int hdmi_mode = -1;

	if (gx3201_hdmi_out_mode & HDMI_RGB_OUT)
		hdmi_mode = HDMI_RGB_OUT;
	else if (gx3201_hdmi_out_mode & HDMI_YUV_422)
		hdmi_mode = HDMI_YUV_422;
	else if (gx3201_hdmi_out_mode & HDMI_YUV_444)
		hdmi_mode = HDMI_YUV_444;

	switch(hdmi_mode)
	{
	case HDMI_RGB_OUT:

#if (defined(ENABLE_YPBPR_HDMI_480I) || defined(ENABLE_YPBPR_HDMI_576I))
		if ((GXAV_VOUT_HDMI_480I == vout_mode) ||
				(GXAV_VOUT_HDMI_576I == vout_mode)) {
			// TV
			hdmi_write(0x64, 0x00);
			hdmi_write(0x65, 0x00);
			// HDMI\C4\u06b2\BF\B4\A6\C0\ED
			hdmi_write(0x15, 0x00);
			hdmi_write(0x16, 0x35);
			hdmi_write(0x17, 0x22);
			hdmi_write(0xd3, 0xa4);
			hdmi_write(0xd4, 0x10);
			hdmi_write(0x3b, 0x01);
		}
		else
#endif
#if (defined(ENABLE_YPBPR_HDMI_576P) || defined(ENABLE_YPBPR_HDMI_720P_50HZ) || \
		defined(ENABLE_YPBPR_HDMI_720P_50HZ) || defined(ENABLE_YPBPR_HDMI_1080I_50HZ))
			if ((GXAV_VOUT_HDMI_480P == vout_mode) ||
					(GXAV_VOUT_HDMI_576P == vout_mode) ||
					(GXAV_VOUT_HDMI_720P_50HZ == vout_mode) ||
					(GXAV_VOUT_HDMI_720P_60HZ == vout_mode) ||
					(GXAV_VOUT_HDMI_1080I_50HZ == vout_mode)) {
				// TV
				hdmi_write(0x64, 0x00);
				hdmi_write(0x65, 0x00);
				// HDMI\C4\u06b2\BF\B4\A6\C0\ED
				hdmi_write(0x15, 0x00);
				hdmi_write(0x16, 0x37);
				hdmi_write(0x17, 0x22);
				hdmi_write(0xd3, 0xa5);
				hdmi_write(0xd4, 0x00);
				hdmi_write(0x3b, 0x01);
			}
			else
#endif
#if (defined(ENABLE_YPBPR_HDMI_1080I_60HZ) || defined(ENABLE_YPBPR_HDMI_1080P_50HZ) || \
		defined(ENABLE_YPBPR_HDMI_1080P_60HZ))
				if ((GXAV_VOUT_HDMI_1080I_60HZ == vout_mode) ||
						(GXAV_VOUT_HDMI_1080P_50HZ == vout_mode) ||
						(GXAV_VOUT_HDMI_1080P_60HZ == vout_mode)) {
					// TV
					hdmi_write(0x64, 0x00);
					hdmi_write(0x65, 0x58);
					// HDMI\C4\u06b2\BF\B4\A6\C0\ED
					hdmi_write(0x15, 0x00);
					hdmi_write(0x16, 0x37);
					hdmi_write(0x17, 0x22);
					hdmi_write(0xd3, 0xa5);
					hdmi_write(0xd4, 0x00);
					hdmi_write(0x3b, 0x01);
				}
#endif
		break;
	default:
		printk(KERN_ERR "%s failed:not support.\n", __func__);
		break;
	}

	return 0;
}

static void gx3201_hdmi_videoout_set(GxVideoOutProperty_Mode vout_mode)
{
	struct vmode_data {
		int vout_mode;
		int values[17][2];
	};

#ifdef ENABLE_YPBPR_HDMI_480I
	static struct vmode_data HDMI_480I = {
		.vout_mode = GXAV_VOUT_HDMI_480I,
		.values = {
			{0x66, 0x10},
			{0x67, 0x06},
			{0x68, 0x01},
			{0x30, 0x33},
			{0x31, 0xb4},
			{0x32, 0x06},
			{0x33, 0x14},
			{0x34, 0x01},
			{0x35, 0xee},
			{0x36, 0x00},
			{0x37, 0x7c},
			{0x38, 0x00},
			{0x39, 0x0d},
			{0x3a, 0x02},
			{0x3d, 0x16},
			{0x3e, 0x15},
			{0x3f, 0x03}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_576I
	static struct vmode_data HDMI_576I = {
		.vout_mode = GXAV_VOUT_HDMI_576I,
		.values = {
			{0x66, 0x10},
			{0x67, 0x15},
			{0x68, 0x01},
			{0x30, 0x03},
			{0x31, 0xC0},
			{0x32, 0x06},
			{0x33, 0x20},
			{0x34, 0x01},
			{0x35, 0xd8},
			{0x36, 0x00},
			{0x37, 0x7E},
			{0x38, 0x00},
			{0x39, 0x71},
			{0x3a, 0x02},
			{0x3d, 0x18},
			{0x3e, 0x16},
			{0x3f, 0x03}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_480P
	static struct vmode_data HDMI_480P = {
		.vout_mode = GXAV_VOUT_HDMI_480P,
		.values = {
			{0x66, 0x10},
			{0x67, 0x03},
			{0x30, 0x61},
			{0x31, 0x5a},
			{0x32, 0x03},
			{0x33, 0x8a},
			{0x34, 0x00},
			{0x35, 0x5a},
			{0x36, 0x00},
			{0x37, 0x3e},
			{0x38, 0x00},
			{0x39, 0x0d},
			{0x3a, 0x02},
			{0x3d, 0x2d},
			{0x3e, 0x10},
			{0x3f, 0x06}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_576P
	static struct vmode_data HDMI_576P = {
		.vout_mode = GXAV_VOUT_HDMI_576P,
		.values = {
			{0x66, 0x10},
			{0x67, 0x12},
			{0x30, 0x01},
			{0x31, 0x60},
			{0x32, 0x03},
			{0x33, 0x90},
			{0x34, 0x00},
			{0x35, 0x65},
			{0x36, 0x00},
			{0x37, 0x40},
			{0x38, 0x00},
			{0x39, 0x71},
			{0x3a, 0x02},
			{0x3d, 0x31},
			{0x3e, 0x0c},
			{0x3f, 0x05}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_50HZ
	static struct vmode_data HDMI_720P_50HZ = {
		.vout_mode = GXAV_VOUT_HDMI_720P_50HZ,
		.values = {
			{0x66, 0x10},
			{0x67, 0x13},
			{0x30, 0x0D},
			{0x31, 0xBC},
			{0x32, 0x07},
			{0x33, 0xBC},
			{0x34, 0x02},
			{0x35, 0x28},
			{0x36, 0x01},
			{0x37, 0x28},
			{0x38, 0x00},
			{0x39, 0xEE},
			{0x3a, 0x02},
			{0x3d, 0x1E},
			{0x3e, 0x19},
			{0x3f, 0x05},
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_60HZ
	static struct vmode_data HDMI_720P_60HZ = {
		.vout_mode = GXAV_VOUT_HDMI_720P_60HZ,
		.values = {
			{0x66, 0x10},
			{0x67, 0x04},
			{0x30, 0x0D},
			{0x31, 0x72},
			{0x32, 0x06},
			{0x33, 0x72},
			{0x34, 0x01},
			{0x35, 0x29},
			{0x36, 0x01},
			{0x37, 0x28},
			{0x38, 0x00},
			{0x39, 0xEE},
			{0x3a, 0x02},
			{0x3d, 0x1E},
			{0x3e, 0x19},
			{0x3f, 0x05}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_50HZ
	static struct vmode_data HDMI_1080I_50HZ = {
		.vout_mode = GXAV_VOUT_HDMI_1080I_50HZ,
		.values = {
			{0x66, 0x10},
			{0x67, 0x14},
			{0x30, 0x0F},
			{0x31, 0x50},
			{0x32, 0x0A},
			{0x33, 0xd0},
			{0x34, 0x02},
			{0x35, 0xD3},
			{0x36, 0x00},
			{0x37, 0x2C},
			{0x38, 0x00},
			{0x39, 0x65},
			{0x3a, 0x04},
			{0x3d, 0x16},
			{0x3e, 0x14},
			{0x3f, 0x05}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_60HZ
	static struct vmode_data HDMI_1080I_60HZ = {
		.vout_mode = GXAV_VOUT_HDMI_1080I_60HZ,
		.values = {
			{0x66, 0x00},
			{0x67, 0x05},
			{0x30, 0x0F},
			{0x31, 0x98},
			{0x32, 0x08},
			{0x33, 0x18},
			{0x34, 0x01},
			{0x35, 0xD3},
			{0x36, 0x00},
			{0x37, 0x2C},
			{0x38, 0x00},
			{0x39, 0x65},
			{0x3a, 0x04},
			{0x3d, 0x16},
			{0x3e, 0x14},
			{0x3f, 0x05}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_50HZ
	static struct vmode_data HDMI_1080P_50HZ = {
		.vout_mode = GXAV_VOUT_HDMI_1080P_50HZ,
		.values = {
			{0x66, 0x00},
			{0x67, 0x10},
			{0x30, 0x0D},
			{0x31, 0x50},
			{0x32, 0x0A},
			{0x33, 0xD0},
			{0x34, 0x02},
			{0x35, 0xE1},
			{0x36, 0x00},
			{0x37, 0x2C},
			{0x38, 0x00},
			{0x39, 0x65},
			{0x3a, 0x04},
			{0x3d, 0x2D},
			{0x3e, 0x29},
			{0x3f, 0x05}
		}
	};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_60HZ
	static struct vmode_data HDMI_1080P_60HZ = {
		.vout_mode = GXAV_VOUT_HDMI_1080P_60HZ,
		.values = {
			{0x66, 0x00},
			{0x67, 0x10},
			{0x30, 0x0D},
			{0x31, 0x98},
			{0x32, 0x08},
			{0x33, 0x18},
			{0x34, 0x01},
			{0x35, 0xE1},
			{0x36, 0x00},
			{0x37, 0x2C},
			{0x38, 0x00},
			{0x39, 0x65},
			{0x3a, 0x04},
			{0x3d, 0x2D},
			{0x3e, 0x29},
			{0x3f, 0x05}
		}
	};
#endif

	struct vmode_data *configs[] = {
#ifdef ENABLE_YPBPR_HDMI_480I
		&HDMI_480I,
#endif
#ifdef ENABLE_YPBPR_HDMI_576I
		&HDMI_576I,
#endif
#ifdef ENABLE_YPBPR_HDMI_480P
		&HDMI_480P,
#endif
#ifdef ENABLE_YPBPR_HDMI_576P
		&HDMI_576P,
#endif
#ifdef ENABLE_YPBPR_HDMI_720P_50HZ
		&HDMI_720P_50HZ,
#endif
#ifdef ENABLE_YPBPR_HDMI_720P_60HZ
		&HDMI_720P_60HZ,
#endif
#ifdef ENABLE_YPBPR_HDMI_1080I_50HZ
		&HDMI_1080I_50HZ,
#endif
#ifdef ENABLE_YPBPR_HDMI_1080I_60HZ
		&HDMI_1080I_60HZ,
#endif
#ifdef ENABLE_YPBPR_HDMI_1080P_50HZ
		&HDMI_1080P_50HZ,
#endif
#ifdef ENABLE_YPBPR_HDMI_1080P_60HZ
		&HDMI_1080P_60HZ,
#endif
		NULL
	};

	int id = 0;
	while(configs[id]) {
		if (configs[id]->vout_mode == vout_mode) {
			int i;

			hdmi_write(0x93, 0xf8); // close EDID interrupt mask
			hdmi_write(0x5f, 0x06); // AVI set
			hdmi_write(0x60, 0x82); // InfoFrame Type
			hdmi_write(0x61, 0x02); // Version 1
			hdmi_write(0x62, 0x0d); // Length of AVI InfoFrame
			hdmi_write(0x17, 0x22); // wanglx

			hdmi_video_set_mode(vout_mode);

			for (i = 0; i < 17; i++)
				hdmi_write(configs[id]->values[i][0], configs[id]->values[i][1]);
			break;
		}
		id++;
	}
}

static void gx3201_hdmi_clock_set(GxVideoOutProperty_Mode mode)
{
	hdmi_write(0x93,0xf8);          // close EDID interrupt mask

	switch(mode){

#if (defined(ENABLE_YPBPR_HDMI_480I) || defined(ENABLE_YPBPR_HDMI_576I) || \
		defined(ENABLE_YPBPR_HDMI_480P) || defined(ENABLE_YPBPR_HDMI_576P))
	case GXAV_VOUT_HDMI_480I:
	case GXAV_VOUT_HDMI_576I:
	case GXAV_VOUT_HDMI_480P:
	case GXAV_VOUT_HDMI_576P:
		hdmi_write(0x56, 0x10);
		hdmi_write(0x57, 0x00);
		hdmi_write(0x58, 0x40);
		hdmi_write(0x59, 0xa8);
		hdmi_write(0x5a, 0x0e);
		hdmi_write(0x5b, 0x38);
		hdmi_write(0x5c, 0x0e);
		hdmi_write(0x5d, 0x31);
		hdmi_write(0x5e, 0x00);
		break;
#endif

#if (defined(ENABLE_YPBPR_HDMI_720P_50HZ) || defined(ENABLE_YPBPR_HDMI_720P_60HZ) || \
		defined(ENABLE_YPBPR_HDMI_1080I_50HZ) || defined(ENABLE_YPBPR_HDMI_1080I_60HZ))
	case GXAV_VOUT_HDMI_720P_50HZ:
	case GXAV_VOUT_HDMI_720P_60HZ:
	case GXAV_VOUT_HDMI_1080I_50HZ:
	case GXAV_VOUT_HDMI_1080I_60HZ:
		hdmi_write(0x56, 0x15);
		hdmi_write(0x57, 0x00);
		hdmi_write(0x58, 0x40);
		hdmi_write(0x59, 0xac);
		hdmi_write(0x5a, 0x0e);
		hdmi_write(0x5b, 0x3f);
		hdmi_write(0x5c, 0x0e);
		hdmi_write(0x5d, 0x31);
		hdmi_write(0x5e, 0x00);
		break;
#endif

#if (defined(ENABLE_YPBPR_HDMI_1080P_50HZ) || defined(ENABLE_YPBPR_HDMI_1080P_60HZ))
	case GXAV_VOUT_HDMI_1080P_50HZ:
	case GXAV_VOUT_HDMI_1080P_60HZ:
		hdmi_write(0x56,0x1a);
		hdmi_write(0x57,0x00);
		hdmi_write(0x58,0x40);
		hdmi_write(0x59,0xac);
		hdmi_write(0x5a,0x0e);
		hdmi_write(0x5b,0x3f);
		hdmi_write(0x5c,0x0e);
		hdmi_write(0x5d,0x31);
		hdmi_write(0x5e,0x00);
		break;
#endif

	default:
		break;
	}

	hdmi_write(0x00,0x4c);
	msleep(2);

	hdmi_write(0x00,0x44);
	msleep(2);

	hdmi_write(0x00,0x84);
	hdmi_write(0x45,0x00);
}

static void gx3201_config_videoout_Clock(int sub_id, int mode){
	if(sub_id == 0){

		switch(mode)
		{

#if (defined(ENABLE_YPBPR_HDMI_480I) || defined(ENABLE_YPBPR_HDMI_576I))
		case GXAV_VOUT_PAL:
		case GXAV_VOUT_PAL_M:
		case GXAV_VOUT_PAL_N:
		case GXAV_VOUT_PAL_NC:
		case GXAV_VOUT_NTSC_M:
		case GXAV_VOUT_NTSC_443:
		case GXAV_VOUT_YCBCR_480I:
		case GXAV_VOUT_YCBCR_576I:
		case GXAV_VOUT_HDMI_480I:
		case GXAV_VOUT_HDMI_576I:
			Gx3201_Vpu_SetBufferStateDelay(0xc0);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_mepg_clk_inhibit))	|=	(1<<30); //DIS SVPU CORRECT
			config_videoout_ClockSourceSel(4,0); //sel pll video 1.188GHz

			config_videoout_ClockDIVConfig(8,0);  //rst -> 0
			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockDIVConfig(8,1);  //rst -> 1

			config_videoout_ClockDIVConfig(6,0);  //bypass -> 0
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 42;//ratio;

			config_videoout_ClockDIVConfig(7,1);  //load_en -> 1

			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockSourceSel(5,1); //sel pll video 148.5M

			config_videoout_ClockDIVConfig(9,1);  //148.5M div config
			config_videoout_ClockDIVConfig(10,0);  //148.5M div config
			config_videoout_ClockDIVConfig(11,0);  //148.5M div rsts
			config_videoout_ClockDIVConfig(11,1);  //148.5M div rst
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<26);
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<25);
			*(volatile unsigned int *)(0xa030a174) |= 1<<25;
			break;
#endif

#if (defined(ENABLE_YPBPR_HDMI_480P))
		case GXAV_VOUT_YPBPR_480P:
		case GXAV_VOUT_HDMI_480P:
		case GXAV_VOUT_VGA_480P:

			Gx3201_Vpu_SetBufferStateDelay(0x60);

			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_mepg_clk_inhibit))	&=	~(1<<30); //EN SVPU CORRECT
			config_videoout_ClockSourceSel(4,0); //sel pll video 297M

			config_videoout_ClockDIVConfig(8,0);  //rst -> 0
			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockDIVConfig(8,1);  //rst -> 1

			config_videoout_ClockDIVConfig(6,0);  //bypass -> 0
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 20;//ratio;

			config_videoout_ClockDIVConfig(7,1);  //load_en -> 1

			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockSourceSel(5,1); //sel pll video 148.5M

			config_videoout_ClockDIVConfig(9,1);  //148.5M div config
			config_videoout_ClockDIVConfig(10,0);  //148.5M div config
			config_videoout_ClockDIVConfig(11,0);  //148.5M div rst
			config_videoout_ClockDIVConfig(11,1);  //148.5M div rst

			*(volatile unsigned int *)(0xa030a174) |= 1<<26;
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<25);
			*(volatile unsigned int *)(0xa030a174) |= 1<<25;
			break;
#endif

#if (defined(ENABLE_YPBPR_HDMI_576P))
		case GXAV_VOUT_YPBPR_576P:
		case GXAV_VOUT_HDMI_576P:
		case GXAV_VOUT_VGA_576P:
			Gx3201_Vpu_SetBufferStateDelay(0x60);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_mepg_clk_inhibit))	|=	(1<<30); //DIS SVPU CORRECT

			config_videoout_ClockSourceSel(4,0); //sel pll video 297M

			config_videoout_ClockDIVConfig(8,0);  //rst -> 0
			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockDIVConfig(8,1);  //rst -> 1

			config_videoout_ClockDIVConfig(6,0);  //bypass -> 0
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 20;//ratio;

			config_videoout_ClockDIVConfig(7,1);  //load_en -> 1

			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockSourceSel(5,1); //sel pll video 148.5M

			config_videoout_ClockDIVConfig(9,1);  //148.5M div config
			config_videoout_ClockDIVConfig(10,0);  //148.5M div config
			config_videoout_ClockDIVConfig(11,0);  //148.5M div rst
			config_videoout_ClockDIVConfig(11,1);  //148.5M div rst 

			*(volatile unsigned int *)(0xa030a174) |= 1<<26;
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<25);
			*(volatile unsigned int *)(0xa030a174) |= 1<<25;
			break;
#endif

#if (defined(ENABLE_YPBPR_HDMI_720P_50HZ) || defined(ENABLE_YPBPR_HDMI_1080I_50HZ))
		case GXAV_VOUT_YPBPR_720P_50HZ:
		case GXAV_VOUT_YPBPR_1080I_50HZ:
		case GXAV_VOUT_HDMI_720P_50HZ:
		case GXAV_VOUT_HDMI_1080I_50HZ:
			Gx3201_Vpu_SetBufferStateDelay(0x30);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_mepg_clk_inhibit))	|=	(1<<30); //DIS SVPU CORRECT

			config_videoout_ClockSourceSel(4,0); //sel pll video 297M

			config_videoout_ClockDIVConfig(8,0);  //rst -> 0
			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockDIVConfig(8,1);  //rst -> 1

			config_videoout_ClockDIVConfig(6,0);  //bypass -> 0
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 6;//ratio;
			//*(volatile unsigned int *)(0xa030a000+0x24) |= 14;//ratio;huanglei tmp 2012-10-11
			config_videoout_ClockDIVConfig(7,1);  //load_en -> 1

			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockSourceSel(5,1); //sel pll video 148.5M

			//config_videoout_ClockDIVConfig(9,0);  //148.5M div config huanglei tmp 2012-10-11
			config_videoout_ClockDIVConfig(9,1);  //148.5M div config
			config_videoout_ClockDIVConfig(10,0);  //148.5M div config
			config_videoout_ClockDIVConfig(11,0);  //148.5M div rst
			config_videoout_ClockDIVConfig(11,1);  //148.5M div rst

			*(volatile unsigned int *)(0xa030a174) |= 1<<26;
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<25);
			*(volatile unsigned int *)(0xa030a174) |= 1<<25;
			break;
#endif

#if (defined(ENABLE_YPBPR_HDMI_720P_60HZ) || defined(ENABLE_YPBPR_HDMI_1080I_60HZ))
		case GXAV_VOUT_YPBPR_720P_60HZ:
		case GXAV_VOUT_YPBPR_1080I_60HZ:
		case GXAV_VOUT_HDMI_720P_60HZ:
		case GXAV_VOUT_HDMI_1080I_60HZ:
			Gx3201_Vpu_SetBufferStateDelay(0x30);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_mepg_clk_inhibit))	|=	(1<<30); //DIS SVPU CORRECT

			config_videoout_ClockSourceSel(4,1); //sel pll video 297M

			config_videoout_ClockDIVConfig(8,0);  //rst -> 0
			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockDIVConfig(8,1);  //rst -> 1

			config_videoout_ClockDIVConfig(6,0);  //bypass -> 0
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 6;//ratio;

			config_videoout_ClockDIVConfig(7,1);  //load_en -> 1

			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockSourceSel(5,1); //sel pll video 148.5M

			config_videoout_ClockDIVConfig(11,0);  //148.5M div rst
			config_videoout_ClockDIVConfig(11,1);  //148.5M div rst
			config_videoout_ClockDIVConfig(9,1);  //148.5M div config
			config_videoout_ClockDIVConfig(10,0);  //148.5M div config

			*(volatile unsigned int *)(0xa030a174) |= 1<<26;
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<25);
			*(volatile unsigned int *)(0xa030a174) |= 1<<25;
			break;	
#endif

#if (defined(ENABLE_YPBPR_HDMI_1080P_50HZ))
		case GXAV_VOUT_YPBPR_1080P_50HZ:
		case GXAV_VOUT_HDMI_1080P_50HZ:
			Gx3201_Vpu_SetBufferStateDelay(0x18);
			//VPU_SET_BUFF_STATE_DELAY(gx3201vpu_reg->rBUFF_CTRL2, 0x40);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_mepg_clk_inhibit))	|=	(1<<30); //DIS SVPU CORRECT

			config_videoout_ClockSourceSel(4,0);

			config_videoout_ClockDIVConfig(8,0);  //rst -> 0
			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockDIVConfig(8,1);  //rst -> 1

			config_videoout_ClockDIVConfig(6,0);  //bypass -> 0
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 2;//ratio;

			config_videoout_ClockDIVConfig(7,1);  //load_en -> 1

			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0


			config_videoout_ClockSourceSel(5,1); //sel pll video 148.5M


			config_videoout_ClockDIVConfig(9,1);  //148.5M div config
			config_videoout_ClockDIVConfig(10,1);  //148.5M div config
			config_videoout_ClockDIVConfig(11,0);  //148.5M div rst
			config_videoout_ClockDIVConfig(11,1);  //148.5M div rst

			*(volatile unsigned int *)(0xa030a174) |= (1<<26);
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<25);
			*(volatile unsigned int *)(0xa030a174) |= 1<<25;
			break;	
#endif

#if (defined(ENABLE_YPBPR_HDMI_1080P_60HZ))
		case GXAV_VOUT_YPBPR_1080P_60HZ:
		case GXAV_VOUT_HDMI_1080P_60HZ:
			Gx3201_Vpu_SetBufferStateDelay(0x18);
			//VPU_SET_BUFF_STATE_DELAY(gx3201vpu_reg->rBUFF_CTRL2, 0x40);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_mepg_clk_inhibit))	|=	(1<<30); //DIS SVPU CORRECT

			config_videoout_ClockSourceSel(4,1); //sel pll video 297M

			config_videoout_ClockDIVConfig(8,0);  //rst -> 0
			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0

			config_videoout_ClockDIVConfig(8,1);  //rst -> 1

			config_videoout_ClockDIVConfig(6,0);  //bypass -> 0
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f);
			*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 2;//ratio;

			config_videoout_ClockDIVConfig(7,1);  //load_en -> 1

			config_videoout_ClockDIVConfig(7,0);  //load_en -> 0


			config_videoout_ClockSourceSel(5,1); //sel pll video 148.5M

			config_videoout_ClockDIVConfig(9,1);  //148.5M div config
			config_videoout_ClockDIVConfig(10,1);  //148.5M div config
			config_videoout_ClockDIVConfig(11,0);  //148.5M div rst
			config_videoout_ClockDIVConfig(11,1);  //148.5M div rst

			*(volatile unsigned int *)(0xa030a174) |= (1<<26);
			//*(volatile unsigned int *)(0xa030a174) &= ~(1<<26);
			*(volatile unsigned int *)(0xa030a174) &= ~(1<<25);
			*(volatile unsigned int *)(0xa030a174) |= 1<<25;
			break;
#endif
		default:
			printk(KERN_ERR "un-supported.\n");
			return;
		}
	}else {
		config_videoout_ClockDIVConfig(20,0);  //rst -> 0
		config_videoout_ClockDIVConfig(19,0);  //load_en -> 0

		config_videoout_ClockDIVConfig(20,1);  //rst -> 1

		config_videoout_ClockDIVConfig(18,0);  //bypass -> 0
		*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) &= ~(0x3f000);
		*(volatile unsigned int *)(&(gx3201_config_reg->cfg_clk)) |= 42<<12;

		config_videoout_ClockDIVConfig(19,1);  //load_en -> 1

		config_videoout_ClockDIVConfig(19,0);  //load_en -> 0

		config_videoout_ClockSourceSel(6,1);   //sel pll video 27

		config_videoout_ClockDIVConfig(22,0);  //148.5M div rst
		config_videoout_ClockDIVConfig(21,1);  //148.5M div config
		config_videoout_ClockDIVConfig(22,1);  //148.5M div rst
	}
}

static void Gx3201_enable_svpu(int mode)
{
	int top_field = 0;
	int active_line = 0;

	*(volatile unsigned int*)(svpu_vout_base_addr + 88) = 1;
	*(volatile unsigned int*)(svpu_vout_base_addr) |= (1<<4);
	CFG_VPU_HOT_SET(gx3201_config_reg);
	CFG_VPU_HOT_CLR(gx3201_config_reg);

	REG_SET_BIT  (&gx3201svpu_reg->rZOOM_CTRL, 0);
	REG_SET_BIT  (&gx3201svpu_reg->rSVPU_CTRL, 1);
	REG_SET_FIELD(&gx3201svpu_reg->rSVPU_CTRL, 0x0F << 4, mode,4);
	REG_SET_BIT  (&gx3201svpu_reg->rSVPU_CTRL, 0);

	while(top_field == 0)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	while(top_field == 0x800)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	while(top_field == 0)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	while(top_field == 0x800)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	while(top_field == 0)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	while(top_field == 0x800)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	while(top_field == 0)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	while(top_field == 0x800)
		Gx3201_Vpu_GetScanInfo(&active_line, &top_field);

	CFG_VPU_HOT_SET(gx3201_config_reg);
	CFG_VPU_HOT_CLR(gx3201_config_reg);
	*(volatile unsigned int*)(svpu_vout_base_addr) &= ~(1<<4);
}

static void vpu_svpu_hdmi_init(void)
{
	u32 i;
	//vpu\CA\E4\B3\F6\D6\C6\u02bd   YPBPR
	for(i=0; i < vpu_init_cnt; i ++)
	{
		*(volatile unsigned int*)(vpu_vout_base_addr + i * 4) = p_vpu_init[i];
	}

	//svpu\CA\E4\B3\F6\D6\C6\u02bd	CVBS
	for(i=0; i < svpu_init_cnt; i++)
	{
		*(volatile unsigned int*)(svpu_vout_base_addr + i * 4) = p_svpu_init[i];
	}

	//vpu&hdmi clk config
	gx3201_config_videoout_Clock(0, vpu_hdmi_vout_mode);

	//svpu clk config & parameters config, vpu/svpu hot reset
	gx3201_config_videoout_Clock(1, 0);
	Gx3201_enable_svpu(svpu_mode);

	//hdmi\CA\E4\B3\F6\D6\C6\u02bd
	gx3201_config_reg->cfg_mpeg_cold_reset |= (1<<7);
	gx3201_config_reg->cfg_mpeg_cold_reset &= ~(1<<7);
	gx3201_hdmi_mode_select();
	gx3201_audio_spdif_reset();
	gx3201_hdmi_videoout_set(vpu_hdmi_vout_mode);
	gx3201_hdmi_clock_set(vpu_hdmi_vout_mode);

	//video dac config
	if(*(volatile unsigned int*)((char *)gx3201svpu_reg + 0X84) == 0)
		*(volatile unsigned int *)((char *)gx3201svpu_reg + 0X84) = 0x1f;
	*(volatile unsigned int *)((char *)gx3201svpu_reg + 0x80) = 0xffffffff;
}

static int pre_init(enum cvbs_mode_enum cvbs_mode, enum ypbpr_hdmi_mode_enum ypbpr_hdmi_mode)
{
	int output_w = 0,output_h = 0;

	switch(ypbpr_hdmi_mode){

#ifdef ENABLE_YPBPR_HDMI_480I
	case G_YPBPR_HDMI_480I:
		svpu_mode = 8;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_480I;
		p_vpu_init = gx3201_dve_ycbcr_480i;
		vpu_init_cnt = sizeof(gx3201_dve_ycbcr_480i) / sizeof(gx3201_dve_ycbcr_480i[0]);
		output_w = 640;
		output_h = 480;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_480P
	case G_YPBPR_HDMI_480P:
		svpu_mode = 9;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_480P;
		p_vpu_init = gx3201_dve_ypbpr_480p;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_480p) / sizeof(gx3201_dve_ypbpr_480p[0]);
		output_w = 640;
		output_h = 480;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_576I
	case G_YPBPR_HDMI_576I:
		svpu_mode = 0;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_576I;
		p_vpu_init = gx3201_dve_ycbcr_576i;
		vpu_init_cnt = sizeof(gx3201_dve_ycbcr_576i) / sizeof(gx3201_dve_ycbcr_576i[0]);
		output_w = 720;
		output_h = 576;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_576P
	case G_YPBPR_HDMI_576P:
		svpu_mode = 1;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_576P;
		p_vpu_init = gx3201_dve_ypbpr_576p;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_576p) / sizeof(gx3201_dve_ypbpr_576p[0]);
		output_w = 720;
		output_h = 576;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_50HZ
	case G_YPBPR_HDMI_720P_50HZ:
		svpu_mode = 2;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_720P_50HZ;
		p_vpu_init = gx3201_dve_ypbpr_720p_50hz;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_720p_50hz) / sizeof(gx3201_dve_ypbpr_720p_50hz[0]);
		output_w = 1280;
		output_h = 720;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_60HZ
	case G_YPBPR_HDMI_720P_60HZ:
		svpu_mode = 10;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_720P_60HZ;
		p_vpu_init = gx3201_dve_ypbpr_720p_60hz;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_720p_60hz) / sizeof(gx3201_dve_ypbpr_720p_60hz[0]);
		output_w = 1280;
		output_h = 720;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_50HZ
	case G_YPBPR_HDMI_1080I_50HZ:
		svpu_mode = 3;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_1080I_50HZ;
		p_vpu_init = gx3201_dve_ypbpr_1080i_50hz;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_1080i_50hz) / sizeof(gx3201_dve_ypbpr_1080i_50hz[0]);
		output_w = 1920;
		output_h = 1080;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_60HZ
	case G_YPBPR_HDMI_1080I_60HZ:
		svpu_mode = 11;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_1080I_60HZ;
		p_vpu_init = gx3201_dve_ypbpr_1080i_60hz;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_1080i_60hz) / sizeof(gx3201_dve_ypbpr_1080i_60hz[0]);
		output_w = 1920;
		output_h = 1080;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_50HZ
	case G_YPBPR_HDMI_1080P_50HZ:
		svpu_mode = 4;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_1080P_50HZ;
		p_vpu_init = gx3201_dve_ypbpr_1080p_50hz;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_1080p_50hz) / sizeof(gx3201_dve_ypbpr_1080p_50hz[0]);
		output_w = 1920;
		output_h = 1080;
		break;
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_60HZ
	case G_YPBPR_HDMI_1080P_60HZ:
		svpu_mode = 12;
		vpu_hdmi_vout_mode = GXAV_VOUT_HDMI_1080P_60HZ;
		p_vpu_init = gx3201_dve_ypbpr_1080p_60hz;
		vpu_init_cnt = sizeof(gx3201_dve_ypbpr_1080p_60hz) / sizeof(gx3201_dve_ypbpr_1080p_60hz[0]);
		output_w = 1920;
		output_h = 1080;
		break;
#endif

	default:
		printk(KERN_ERR "error: ypbyr_hdmi mode not support. please modify .config to enable it. \n");
		break;
	}

	switch(cvbs_mode){

#ifdef ENABLE_PAL
	case G_PAL:
		p_svpu_init = gx3201_dve_pal_bdghi;
		svpu_init_cnt = sizeof(gx3201_dve_pal_bdghi) / sizeof(gx3201_dve_pal_bdghi[0]);
		break;
#endif

#ifdef ENABLE_PAL_M
	case G_PAL_M:
		p_svpu_init = gx3201_dve_pal_m;
		svpu_init_cnt = sizeof(gx3201_dve_pal_m) / sizeof(gx3201_dve_pal_m[0]);
		break;
#endif

#ifdef ENABLE_PAL_N
	case G_PAL_N:
		p_svpu_init = gx3201_dve_pal_n;
		svpu_init_cnt = sizeof(gx3201_dve_pal_n) / sizeof(gx3201_dve_pal_n[0]);
		break;
#endif

#ifdef ENABLE_PAL_NC
	case G_PAL_NC:
		p_svpu_init = gx3201_dve_pal_nc;
		svpu_init_cnt = sizeof(gx3201_dve_pal_nc) / sizeof(gx3201_dve_pal_nc[0]);
		break;
#endif

#ifdef ENABLE_NTSC_M
	case G_NTSC_M:
		p_svpu_init = gx3201_dve_ntsc_m;
		svpu_init_cnt = sizeof(gx3201_dve_ntsc_m) / sizeof(gx3201_dve_ntsc_m[0]);
		break;
#endif

#ifdef ENABLE_NTSC_443
	case G_NTSC_443:
		p_svpu_init = gx3201_dve_ntsc_443;
		svpu_init_cnt = sizeof(gx3201_dve_ntsc_443) / sizeof(gx3201_dve_ntsc_443[0]);
		break;
#endif
	default:
		printk(KERN_ERR "error: cvbs mode not support. please modify .config to enable it. \n");
		break;
	}

	if(((ypbpr_hdmi_mode == G_YPBPR_HDMI_720P_50HZ) ||
				(ypbpr_hdmi_mode == G_YPBPR_HDMI_1080I_50HZ) ||
				(ypbpr_hdmi_mode == G_YPBPR_HDMI_1080P_50HZ)) &&
			((cvbs_mode == G_NTSC_M)  || (cvbs_mode == G_NTSC_443))){
		printk(KERN_ERR "error: can't support 50HZ with NTSC.\n");
	}
	if(((ypbpr_hdmi_mode == G_YPBPR_HDMI_720P_60HZ) ||
				(ypbpr_hdmi_mode == G_YPBPR_HDMI_1080I_60HZ) ||
				(ypbpr_hdmi_mode == G_YPBPR_HDMI_1080P_60HZ)) &&
			((cvbs_mode == G_PAL)  || (cvbs_mode == G_PAL_M) ||
			 (cvbs_mode == G_PAL_N) || (cvbs_mode == G_PAL_NC))){
		printk(KERN_ERR "error: can't support 60HZ with PAL.\n");
	}

	return 0;
}

static int gx3201osd_enable(int enable)
{
	if(enable == 0)
		VPU_OSD_DISABLE(gx3201vpu_reg->rOSD_CTRL);
	else {
		VPU_OSD_SET_FIRST_HEAD(gx3201vpu_reg->rOSD_FIRST_HEAD_PTR, gx3201osd_head_dma);
		VPU_OSD_ENABLE(gx3201vpu_reg->rOSD_CTRL);
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

static int gx3201osd_alpha(u32 alpha)
{
	VPU_OSD_GLOBAL_ALHPA_ENABLE(gx3201osd_head_ptr->word1);
	VPU_OSD_SET_MIX_WEIGHT(gx3201osd_head_ptr->word1, alpha);
	VPU_OSD_SET_ALPHA_RATIO_DISABLE(gx3201osd_head_ptr->word7);

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

static int gx3201osd_color_format(GxColorFormat format)
{
	int true_color_mode;

	if(format <= GX_COLOR_FMT_CLUT8)
		VPU_OSD_CLR_ZOOM_MODE_EN_IPS(gx3201vpu_reg->rOSD_CTRL);
	else
		VPU_OSD_SET_ZOOM_MODE_EN_IPS(gx3201vpu_reg->rOSD_CTRL);

	if((format >= GX_COLOR_FMT_RGBA8888)&&(format <= GX_COLOR_FMT_BGR888)) {
		true_color_mode = format - GX_COLOR_FMT_RGBA8888;
		VPU_OSD_SET_COLOR_TYPE(gx3201osd_head_ptr->word1, 0x7);
		VPU_OSD_SET_TRUE_COLOR_MODE(gx3201osd_head_ptr->word1, true_color_mode);
	} else {
		if( (format==GX_COLOR_FMT_ARGB4444)|| (format==GX_COLOR_FMT_ARGB1555)||
				(format==GX_COLOR_FMT_ARGB8888))
			VPU_OSD_SET_ARGB_CONVERT(gx3201osd_head_ptr->word1,1);
		else
			VPU_OSD_SET_ARGB_CONVERT(gx3201osd_head_ptr->word1,0);

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
		VPU_OSD_SET_COLOR_TYPE(gx3201osd_head_ptr->word1, format);
	}

	return 0;
}
static u32 palette_buffer[2];
static int gx3201osd_main_surface(struct fb_info *info)
{
	GxAvRect rect={0,0,0,0};
	unsigned int request_block=0, bpp=1;
	struct ncfb_pri *fbi = info->par;

	if (gx3201osd_head_ptr == NULL)
		gx3201osd_head_ptr = dma_alloc_coherent(fbi->dev, PAGE_ALIGN(sizeof(*gx3201osd_head_ptr)), &gx3201osd_head_dma, GFP_KERNEL);

	if (gx3201osd_head_ptr == NULL) {
		/* prevent initial garbage on screen */
		printk(KERN_ERR "%s, dma_alloc_coherent err.", __func__);
		while(1);
	}

	printk("osd_head_ptr: %p, %x.\n", gx3201osd_head_ptr, gx3201osd_head_dma);
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

	/* set vpu byte-sequence(both data and command) */
#if(VPU_MOD_VERSION == VPU_MPW)
	if(bpp == 32)
		byte_seq = ABCD_EFGH;
	if(bpp == 16)
		byte_seq = DCBA_HGFE;
#else
	if(bpp == 32)
		byte_seq = ABCD_EFGH;
	if(bpp == 16)
		byte_seq = CDAB_GHEF;
	if(bpp <= 8)
		byte_seq = DCBA_HGFE;
#endif

	VPU_SET_RW_BYTESEQ(gx3201vpu_reg->rSYS_PARA, byte_seq);
	printk("%s,byte_seq: %x.\n", __func__, byte_seq);

	REG_SET_FIELD(&(gx3201vpu_reg->rBUFF_CTRL2),0x7FF<<0,request_block,0);
	VPU_OSD_SET_WIDTH(gx3201osd_head_ptr->word3, rect.x, rect.width + rect.x - 1);
	VPU_OSD_SET_HEIGHT(gx3201osd_head_ptr->word4, rect.y, rect.height + rect.y - 1);
	VPU_OSD_SET_POSITION(gx3201vpu_reg->rOSD_POSITION, rect.x, rect.y);

	palette_buffer[1] = 0xff00ff00;
	palette_buffer[0] = 0x0;

	gx3201osd_alpha(0xff);
	if (bpp == 1) {
		gx3201osd_color_format(GX_COLOR_FMT_CLUT1);
		VPU_OSD_SET_CLUT_PTR(gx3201osd_head_ptr->word2, virt_to_phys(palette_buffer));
		VPU_OSD_SET_CLUT_LENGTH(gx3201osd_head_ptr->word1, 0);
		VPU_OSD_CLUT_UPDATA_ENABLE(gx3201osd_head_ptr->word1);
	} else {
		gx3201osd_color_format(GX_COLOR_FMT_RGB565);
	}

	VPU_OSD_SET_DATA_ADDR(gx3201osd_head_ptr->word5, info->fix.smem_start);
	VPU_OSD_SET_DATA_ADDR(gx3201osd_head_ptr->word6, gx3201osd_head_dma);

	VPU_OSD_SET_FIRST_HEAD(gx3201vpu_reg->rOSD_FIRST_HEAD_PTR, gx3201osd_head_dma);
	VPU_OSD_LIST_END_ENABLE(gx3201osd_head_ptr->word7);
	VPU_OSD_SET_BASE_LINE(gx3201osd_head_ptr->word7,rect.width);

#if 0
	if(VPU_OSD_PHASE_0_ANTI_FLICKER_DISABLE == VPU_OSD_GET_PHASE_0(gx3201vpu_reg->rOSD_PHASE_0))
		VPU_OSD_SET_ZOOM_MODE(gx3201vpu_reg->rOSD_CTRL);
	else
		VPU_OSD_CLR_ZOOM_MODE(gx3201vpu_reg->rOSD_CTRL);
#endif

	VPU_OSD_CLR_ZOOM_MODE(gx3201vpu_reg->rOSD_CTRL);

	gx3201vpu_reg->rOSD_VIEW_SIZE = (rect.height << 16)|rect.width;
	gx3201vpu_reg->rOSD_ZOOM = 0x10001000;

	return 0;
}

int gx3201_fbinit(struct fb_info *info) {
	int ret = 0;

	if (gx3201vpu_reg == NULL) {
		gx3201vpu_reg = ioremap(0x04800000, sizeof(*gx3201vpu_reg));
		if (!gx3201vpu_reg) {
			printk(KERN_ERR "%s, ioremap failed.\n", __func__);
			return -ENOMEM;
		}

		gx3201svpu_reg = ioremap(0x04900000, sizeof(*gx3201svpu_reg));
		if (!gx3201svpu_reg) {
			printk(KERN_ERR "%s, ioremap failed.\n", __func__);
			return -ENOMEM;
		}

		vpu_vout_base_addr = ioremap(0x04804000, 0x1000);
		if (!vpu_vout_base_addr) {
			printk(KERN_ERR "%s, ioremap failed.\n", __func__);
			return -ENOMEM;
		}

		svpu_vout_base_addr = ioremap(0x04904000, 0x1000);
		if (!svpu_vout_base_addr) {
			printk(KERN_ERR "%s, ioremap failed.\n", __func__);
			return -ENOMEM;
		}

		gx3201_config_reg = ioremap(0x0030a000, 0x1000);
		if (!gx3201_config_reg) {
			printk(KERN_ERR "%s, ioremap failed.\n", __func__);
			return -ENOMEM;
		}
	}
	printk("ncfb io addr:%p, %p, %p, %p, %p.\n", gx3201vpu_reg, gx3201svpu_reg, vpu_vout_base_addr, svpu_vout_base_addr, gx3201_config_reg);
	if (info->var.xres <= 1280) 
		pre_init(G_PAL, G_YPBPR_HDMI_720P_50HZ);
	else 
		pre_init(G_PAL, G_YPBPR_HDMI_1080I_50HZ);

	vpu_svpu_hdmi_init();

	ret = gx3201osd_main_surface(info);

	gx3201osd_enable(1);

	return ret;
}

