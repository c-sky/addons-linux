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

/**\file
 *  The top most file which makes use of synopsys GMAC driver code.
 *
 *  This file can be treated as the example code for writing a application driver
 *  for synopsys GMAC device using the driver provided by Synopsys.
 *  This exmple is for Linux 2.6.xx kernel
 *  - Uses 32 bit 33MHz PCI Interface as the host bus interface
 *  - Uses Linux network driver and the TCP/IP stack framework
 *  - Uses the Device Specific Synopsys GMAC Kernel APIs
 *  \internal
 * ---------------------------REVISION HISTORY--------------------------------
 * Synopsys			01/Aug/2007			Created
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>

#include <linux/platform_device.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include "synopGMAC_Host.h"
#include "synopGMAC_banner.h"
#include "synopGMAC_plat.h"
#include "synopGMAC_network_interface.h"
#include "synopGMAC_Dev.h"

/****************************************************/


/* Global declarations: these are required to handle
   Os and Platform dependent functionalities        */

/*GMAC IP Base address and Size   */
u8 *synopGMACMappedAddr = NULL;
//u32 synopGMACMappedAddrSize = 0;

/*global adapter gmacdev pcidev and netdev pointers */
synopGMACPciNetworkAdapter *synopGMACadapter;
//synopGMACdevice		   *synopGMACdev;

#ifdef AVB_SUPPORT
synopGMACdevice		   *synopGMACdev;
synopGMACdevice		   *synopGMACdev;
#endif

struct platform_device     *synopGMACpcidev;
struct net_device          *synopGMACnetdev;

unsigned int synopGMAC_mac_irq = 56;
module_param(synopGMAC_mac_irq, int, S_IRUGO);
unsigned int synopGMAC_mac_fb = 0;
module_param(synopGMAC_mac_fb, int, S_IRUGO);
unsigned int synopGMAC_phy_addr = 1;
module_param(synopGMAC_phy_addr, int, S_IRUGO);

/*-------------------------------------------------------------------------*/

static int mac_synopsis_drv_probe(struct platform_device *pdev)
{
	int retval;
	u32 rsrc_start, rsrc_len;

	TR("Initializing synopsys GMAC interfaces ..\n") ;

	rsrc_start = pdev->resource[0].start;
	rsrc_len = pdev->resource[0].end - pdev->resource[0].start + 1;

	if (!request_mem_region(rsrc_start, rsrc_len, "mac_synopsis Regs")) {
		TR0("request_mem_region failed");
		retval = -EBUSY;
		return retval;
	}

	synopGMACMappedAddr = ioremap(rsrc_start, rsrc_len);
	if (!synopGMACMappedAddr) {
		TR0("ioremap failed");
		retval = -ENOMEM;
		return retval;
	}

	TR("synopGMACMappedAddr, VA: %p, PH: %x, size: %x.\n", synopGMACMappedAddr, rsrc_start, rsrc_len) ;
	if((retval = synopGMAC_init_network_interface())){
		TR0("Could not initialize the Network interface.\n");
		return retval;
	}

	return 0;
}

static int mac_synopsis_drv_remove(struct platform_device *pdev)
{
	u32 rsrc_start, rsrc_len;

	TR0("Exiting synopsys GMAC interfaces ..\n") ;

	synopGMAC_exit_network_interface();

	iounmap(synopGMACMappedAddr);

	rsrc_start = pdev->resource[0].start;
	rsrc_len = pdev->resource[0].end - pdev->resource[0].start + 1;
	release_mem_region(rsrc_start, rsrc_len);

	return 0;
}

extern int mac_synopsis_drv_suspend(struct platform_device *pdev, pm_message_t state);
extern int mac_synopsis_drv_resume(struct platform_device *pdev);

#define MAC_NAME "mac_synopsis"

MODULE_ALIAS(MAC_NAME);
static struct platform_driver mac_synopsis_driver = {
	.probe = mac_synopsis_drv_probe,
	.remove = mac_synopsis_drv_remove,
#ifdef CONFIG_PM
	.suspend = mac_synopsis_drv_suspend,
	.resume = mac_synopsis_drv_resume,
#endif
	.driver = {
		.name = MAC_NAME ,
		.bus = &platform_bus_type
	}
};

static struct resource mac_synopsis_resources[] = {
	[0] = {
#ifdef CONFIG_CSKY
		.start = 0x00700000,
		.end   = 0x00701FFF,
#else
		.start = 0x20000000,
		.end   = 0x20001FFF,
#endif
		.flags = IORESOURCE_MEM,
		.name  = "Regs address space",
	},
	[1] = {
		.flags = IORESOURCE_IRQ,
		.name  = "Interrupt MAC synopsis",
	},
};

static u64 mac_synopsis_dmamask = ~(u32)0;

static void synop_mac_dev_release(struct device *dev)
{
	return;
}

static struct platform_device gx_mac_synopsis_device = {
	.name = MAC_NAME,
	.id   = -1,
	.dev = {
		.dma_mask          = &mac_synopsis_dmamask,
		.coherent_dma_mask = 0xffffffff,
		.release           = &synop_mac_dev_release,
	},
	.num_resources = ARRAY_SIZE(mac_synopsis_resources),
	.resource      = mac_synopsis_resources,
};

/***************************************************/
static int __init SynopGMAC_Host_Interface_init(void)
{
	int retval;

	mac_synopsis_resources[1].start = synopGMAC_mac_irq;

	synopGMACpcidev = &gx_mac_synopsis_device;

	platform_device_register(synopGMACpcidev);

	retval = platform_driver_register(&mac_synopsis_driver);
	if (retval < 0)
		return retval;

	return 0;
}

static void __exit SynopGMAC_Host_Interface_exit(void)
{
	platform_driver_unregister(&mac_synopsis_driver);
	platform_device_unregister(synopGMACpcidev);
}

module_init(SynopGMAC_Host_Interface_init);
module_exit(SynopGMAC_Host_Interface_exit);

MODULE_AUTHOR("Synopsys India");
MODULE_DESCRIPTION("SYNOPSYS GMAC NETWORK DRIVER WITH PCI INTERFACE");
MODULE_LICENSE("GPL");
