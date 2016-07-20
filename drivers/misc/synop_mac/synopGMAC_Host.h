/* ===================================================================================
 * Copyright (c) <2009> Synopsys, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software annotated with this license and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * =================================================================================== */

#ifndef SYNOP_GMAC_HOST_H
#define SYNOP_GMAC_HOST_H


#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include "synopGMAC_plat.h"
#include "synopGMAC_Dev.h"

typedef struct synopGMACAdapterStruct{
	/*Device Dependent Data structur*/
	synopGMACdevice * synopGMACdev;
#ifdef AVB_SUPPORT
	synopGMACdevice * synopGMACdev_ch1; // For DMA Channel 1
	synopGMACdevice * synopGMACdev_ch2; // For DMA Channel 2
	synopGMACavbStruct * synopGMACavb;
#endif
	/*Os/Platform Dependent Data Structures*/
	struct platform_device * synopGMACpcidev;
	struct net_device *synopGMACnetdev;
	struct net_device_stats synopGMACNetStats;
	u32 synopGMACPciState[16];

	struct napi_struct napi;
} synopGMACPciNetworkAdapter;

extern unsigned int synopGMAC_mac_fb;
extern unsigned int synopGMAC_phy_addr;
extern unsigned int synopGMAC_mac_irq;

#endif
