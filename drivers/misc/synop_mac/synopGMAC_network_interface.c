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


/** \file
 * This is the network dependent layer to handle network related functionality.
 * This file is tightly coupled to neworking frame work of linux 2.6.xx kernel.
 * The functionality carried out in this file should be treated as an example only
 * if the underlying operating system is not Linux.
 *
 * \note Many of the functions other than the device specific functions
 *  changes for operating system other than Linux 2.6.xx
 * \internal
 *-----------------------------REVISION HISTORY-----------------------------------
 * Synopsys			01/Aug/2007				Created
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>

#include <linux/netdevice.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>


#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include "synopGMAC_Host.h"
#include "synopGMAC_plat.h"
#include "synopGMAC_network_interface.h"
#include "synopGMAC_Dev.h"


#define IOCTL_READ_REGISTER  SIOCDEVPRIVATE+1
#define IOCTL_WRITE_REGISTER SIOCDEVPRIVATE+2
#define IOCTL_READ_IPSTRUCT  SIOCDEVPRIVATE+3
#define IOCTL_READ_RXDESC    SIOCDEVPRIVATE+4
#define IOCTL_READ_TXDESC    SIOCDEVPRIVATE+5
#define IOCTL_POWER_DOWN     SIOCDEVPRIVATE+6

#ifdef AVB_SUPPORT
//#define IOCTL_AVB_SET_CONFIG SIOCDEVPRIVATE+7
//#define IOCTL_AVB_RUN_TEST   SIOCDEVPRIVATE+8
//#define IOCTL_AVB_GET_RESULT SIOCDEVPRIVATE+9

static u32 global_avb_debug_enable;
static u32 global_avb_tx_count_debug[3];

#define IOCTL_AVB_TEST     SIOCDEVPRIVATE+7
#define AVB_SET_CONFIG     0x00000001
#define AVB_CONFIG_HW      0x00000002
#define AVB_RUN_TEST       0x00000003
#define AVB_GET_RESULT     0x00000004
#define AVB_DEBUG_ENABLE   0x00000005
#define AVB_DEBUG_DISABLE  0x00000006
#define AVB_TX_FRAMES      0x00000007

static struct timer_list synopGMAC_AVB_test_timer;

#define FRAME_PATTERN_CH2   0xAAAAAAAA
#define FRAME_PATTERN_CH1   0x55555555
#define FRAME_PATTERN_CH0   0x11223344

static int frame_header_ch2[] = {
	0x73560D00, //   DST: 00:0D:56:73:D0:F3
	0x5500F3D0, //   SRC: 00:55:7B:B5:7D:F7
	0xF77DB57B,

	0x00A00081 //   Eth type: 0x8100 VID = 0x0000 CFI=0 PCP =5
		//   AVTYPE is read from the #define and used while creating a frame

};
static int frame_header_ch1[] = {
	0x73560D00, //   DST: 00:0D:56:73:D0:F3
	0x5500F3D0, //   SRC: 00:55:7B:B5:7D:F7
	0xF77DB57B,

	0x00800081  //   Eth type: 0x8100 VID = 0x0000 CFI=0 PCP =4
		//   AVTYPE is read from the #define and used while creating a frame
};
static int frame_header_ch0[] = { //BEST EFFORT ETHERNET HEADER
	0x73560D00, //   DST: 00:0D:56:73:D0:F3
	0x5500F3D0, //   SRC: 00:55:7B:B5:7D:F7
	0xF77DB57B,

	0x00000008  //   Eth type: 0x0800
};

#endif
static struct timer_list synopGMAC_cable_unplug_timer;
static u32 GMAC_Power_down; // This global variable is used to indicate the ISR whether the interrupts occured in the process of powering down the mac or not
static int tx_desc = TRANSMIT_DESC_SIZE;
static int rx_desc = RECEIVE_DESC_SIZE;
static int rmii_flag = 3;
static int napi = 1;
u8 synopGMAC_mac_addr[6] = DEFAULT_MAC_ADDRESS;

module_param(napi, int, S_IRUGO);
module_param(rmii_flag, int, S_IRUGO);
module_param(tx_desc, int, S_IRUGO);
module_param(rx_desc, int, S_IRUGO);
module_param_array_named(mac_addr, synopGMAC_mac_addr, byte, NULL, 0);

static int parse_option(char **s, char *n) {
	char *tmp;
	int len = strlen(n);
	tmp = *s;

	if(strncmp(tmp, n, len) == 0) {
		tmp += len;
		if (*tmp == '@') {
			tmp++;
			return simple_strtol(tmp, s, 0);
		}
	}

	return -1;
}

static int parse_option_string(char **s, char *n) {
	char *tmp;
	int len = strlen(n);
	tmp = *s;

	if(strncmp(tmp, n, len) == 0) {
		tmp += len;
		if (*tmp == '@') {
			tmp++;
			*s = tmp;
			return 1;
		}
	}

	return -1;
}

static int synopGMAC_options(char **s)
{
	int ret, i;
	char *tmp;

	ret = parse_option( s, "napi");
	if (ret >= 0) {
		napi = ret;
		printk("%s:", __func__);
		printk("napi:%d\n", napi);
		return 0;
	}

	ret = parse_option( s, "rx_desc");
	if (ret >= 0) {
		rx_desc = ret;
		printk("%s:", __func__);
		printk("rx_desc:%d\n", rx_desc);
		return 0;
	}

	ret = parse_option( s, "rmii_flag");
	if (ret >= 0) {
		rmii_flag = ret;
		printk("%s:", __func__);
		printk("rmii_flag:%d\n", rmii_flag);
		return 0;
	}

	ret = parse_option( s, "tx_desc");
	if (ret >= 0) {
		tx_desc = ret;
		printk("%s:", __func__);
		printk("tx_desc:%d\n", tx_desc);
		return 0;
	}

	ret = parse_option( s, "mac_irq");
	if (ret >= 0) {
		synopGMAC_mac_irq = ret;
		printk("%s:", __func__);
		printk("synopGMAC_mac_irq:%d\n", synopGMAC_mac_irq);
		return 0;
	}

	ret = parse_option( s, "mac_fb");
	if (ret >= 0) {
		synopGMAC_mac_fb = ret;
		printk("%s:", __func__);
		printk("synopGMAC_mac_fb:%d\n", synopGMAC_mac_fb);
		return 0;
	}

	ret = parse_option_string( s, "mac_addr");
	if (ret >= 0) {
		printk("%s: mac_addr got.\n", __func__);
		for(i = 0; i < 6; i++) {
			tmp = *s;
			synopGMAC_mac_addr[i] = (u8) simple_strtoul(tmp, s, 16);
			if(**s == ':') *s += 1;
			else break;
		}
		return 0;
	}

        ret = parse_option( s, "phy_addr");
	if (ret >= 0) {
		synopGMAC_phy_addr = ret;
		printk("%s:", __func__);
		printk("synopGMAC_phy_addr:%d\n", synopGMAC_phy_addr);
		return 0;
	}

	return 1;
}

static int options_setup(char *s)
{
	char *tmp;
	tmp = s;

	do {
		if (synopGMAC_options(&tmp)) break;
		if (*tmp != ',') break;
		else tmp++;
	} while(1);

	return 1;
}
__setup("synopGMAC=", options_setup);

/*These are the global pointers for their respecive structures*/
extern synopGMACPciNetworkAdapter * synopGMACadapter;
//extern synopGMACdevice	          * synopGMACdev;
extern struct net_dev             * synopGMACnetdev;
extern struct platform_device             * synopGMACpcidev;
#ifdef AVB_SUPPORT
extern synopGMACavbStruct         * synopGMACavb;
#endif


/*these are the global data for base address and its size*/
extern u8* synopGMACMappedAddr;
extern u32 synopGMACMappedAddrSize;
//extern u32 synop_pci_using_dac;

/*Sample Wake-up frame filter configurations*/

u32 synopGMAC_wakeup_filter_config0[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x00000000,	// For Filter1 CRC is not computed may be it is 0x0000
	0x00000000,	// For Filter2 CRC is not computed may be it is 0x0000
	0x5F5F5F5F, // For Filter3 CRC is based on 0,1,2,3,4,6,8,9,10,11,12,14,16,17,18,19,20,22,24,25,26,27,28,30 bytes from offset
	0x09000000, // Filter 0,1,2 are disabled, Filter3 is enabled and filtering applies to only multicast packets
	0x1C000000, // Filter 0,1,2 (no significance), filter 3 offset is 28 bytes from start of Destination MAC address
	0x00000000, // No significance of CRC for Filter0 and Filter1
	0xBDCC0000  // No significance of CRC for Filter2, Filter3 CRC is 0xBDCC
};
u32 synopGMAC_wakeup_filter_config1[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x00000000,	// For Filter1 CRC is not computed may be it is 0x0000
	0x7A7A7A7A,	// For Filter2 CRC is based on 1,3,4,5,6,9,11,12,13,14,17,19,20,21,25,27,28,29,30 bytes from offset
	0x00000000, // For Filter3 CRC is not computed may be it is 0x0000
	0x00010000, // Filter 0,1,3 are disabled, Filter2 is enabled and filtering applies to only unicast packets
	0x00100000, // Filter 0,1,3 (no significance), filter 2 offset is 16 bytes from start of Destination MAC address
	0x00000000, // No significance of CRC for Filter0 and Filter1
	0x0000A0FE  // No significance of CRC for Filter3, Filter2 CRC is 0xA0FE
};
u32 synopGMAC_wakeup_filter_config2[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x000000FF,	// For Filter1 CRC is computed on 0,1,2,3,4,5,6,7 bytes from offset
	0x00000000,	// For Filter2 CRC is not computed may be it is 0x0000
	0x00000000, // For Filter3 CRC is not computed may be it is 0x0000
	0x00000100, // Filter 0,2,3 are disabled, Filter 1 is enabled and filtering applies to only unicast packets
	0x0000DF00, // Filter 0,2,3 (no significance), filter 1 offset is 223 bytes from start of Destination MAC address
	0xDB9E0000, // No significance of CRC for Filter0, Filter1 CRC is 0xDB9E
	0x00000000  // No significance of CRC for Filter2 and Filter3
};

/*
   The synopGMAC_wakeup_filter_config3[] is a sample configuration for wake up filter.
   Filter1 is used here
   Filter1 offset is programmed to 50 (0x32)
   Filter1 mask is set to 0x000000FF, indicating First 8 bytes are used by the filter
   Filter1 CRC= 0x7EED this is the CRC computed on data 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55

   Refer accompanied software DWC_gmac_crc_example.c for CRC16 generation and how to use the same.
   */

u32 synopGMAC_wakeup_filter_config3[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x000000FF,	// For Filter1 CRC is computed on 0,1,2,3,4,5,6,7 bytes from offset
	0x00000000,	// For Filter2 CRC is not computed may be it is 0x0000
	0x00000000, // For Filter3 CRC is not computed may be it is 0x0000
	0x00000100, // Filter 0,2,3 are disabled, Filter 1 is enabled and filtering applies to only unicast packets
	0x00003200, // Filter 0,2,3 (no significance), filter 1 offset is 50 bytes from start of Destination MAC address
	0x7eED0000, // No significance of CRC for Filter0, Filter1 CRC is 0x7EED,
	0x00000000  // No significance of CRC for Filter2 and Filter3
};
/**
 * Function used to detect the cable plugging and unplugging.
 * This function gets scheduled once in every second and polls
 * the PHY register for network cable plug/unplug. Once the
 * connection is back the GMAC device is configured as per
 * new Duplex mode and Speed of the connection.
 * @param[in] u32 type but is not used currently.
 * \return returns void.
 * \note This function is tightly coupled with Linux 2.6.xx.
 * \callgraph
 */

static void synopGMAC_linux_cable_fes_function(u32 notused)
{
	u16 data;
	synopGMACPciNetworkAdapter *adapter = (synopGMACPciNetworkAdapter *)synopGMAC_cable_unplug_timer.data;
	synopGMACdevice            *gmacdev = adapter->synopGMACdev;

	init_timer(&synopGMAC_cable_unplug_timer);
	synopGMAC_cable_unplug_timer.expires = CHECK_TIME + jiffies;

	synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase,gmacdev->PhyBase, PHY_CONTROL_REG, &data);

	if(data & Mii_phy_status_speed)
		gmacdev->Speed      =   SPEED100;
	else
		gmacdev->Speed      =   SPEED10;

	gmacdev->DuplexMode = (data & Mii_phy_status_duplex)  ? FULLDUPLEX: HALFDUPLEX ;

	if(gmacdev->Speed == SPEED100)
		TR("Link is with 100M Speed \n");
	if(gmacdev->Speed == SPEED10)
		TR("Link is with 10M Speed \n");

	synopGMAC_setup_fes(gmacdev);

	add_timer(&synopGMAC_cable_unplug_timer);
}

static void synopGMAC_linux_powerdown_mac(synopGMACdevice *gmacdev)
{
	TR0("Put the GMAC to power down mode..\n");
	// Disable the Dma engines in tx path
	GMAC_Power_down = 1;	// Let ISR know that Mac is going to be in the power down mode
	synopGMAC_disable_dma_tx(gmacdev);
	plat_delay(10000);		//allow any pending transmission to complete
	// Disable the Mac for both tx and rx
	synopGMAC_tx_disable(gmacdev);
	synopGMAC_rx_disable(gmacdev);
	plat_delay(10000); 		//Allow any pending buffer to be read by host
	//Disable the Dma in rx path
	synopGMAC_disable_dma_rx(gmacdev);

	//enable the power down mode
	//synopGMAC_pmt_unicast_enable(gmacdev);

	//prepare the gmac for magic packet reception and wake up frame reception
	synopGMAC_magic_packet_enable(gmacdev);
	synopGMAC_write_wakeup_frame_register(gmacdev, synopGMAC_wakeup_filter_config3);

	synopGMAC_wakeup_frame_enable(gmacdev);

	//gate the application and transmit clock inputs to the code. This is not done in this driver :).

	//enable the Mac for reception
	synopGMAC_rx_enable(gmacdev);

	//Enable the assertion of PMT interrupt
	synopGMAC_pmt_int_enable(gmacdev);
	//enter the power down mode
	synopGMAC_power_down_enable(gmacdev);
	return;
}

static void synopGMAC_linux_powerup_mac(synopGMACdevice *gmacdev)
{
	GMAC_Power_down = 0;	// Let ISR know that MAC is out of power down now
	if( synopGMAC_is_magic_packet_received(gmacdev))
		TR("GMAC wokeup due to Magic Pkt Received\n");
	if(synopGMAC_is_wakeup_frame_received(gmacdev))
		TR("GMAC wokeup due to Wakeup Frame Received\n");
	//Disable the assertion of PMT interrupt
	synopGMAC_pmt_int_disable(gmacdev);
	//Enable the mac and Dma rx and tx paths
	synopGMAC_rx_enable(gmacdev);
	synopGMAC_enable_dma_rx(gmacdev);

	synopGMAC_tx_enable(gmacdev);
	synopGMAC_enable_dma_tx(gmacdev);
	return;
}

/**
 * This sets up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures for ring mode or chain mode.
 * This function depends on the pcidev structure for allocation consistent dma-able memory in case of linux.
 * This limitation is due to the fact that linux uses pci structure to allocate a dmable memory
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 *	- Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to pci_device structure.
 * @param[in] number of descriptor expected in tx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->TxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */

s32 synopGMAC_setup_tx_desc_queue(synopGMACdevice * gmacdev,struct platform_device * pcidev,u32 no_of_desc, u32 desc_mode)
{
	s32 i;

	DmaDesc *first_desc = NULL;
	DmaDesc *second_desc = NULL;
	dma_addr_t dma_addr;
	gmacdev->TxDescCount = 0;

	if(desc_mode == RINGMODE){
		TR("Total size of memory required for Tx Descriptors in Ring Mode = 0x%08x\n",((sizeof(DmaDesc) * no_of_desc)));
		first_desc = plat_alloc_consistent_dmaable_memory (pcidev, sizeof(DmaDesc) * no_of_desc,&dma_addr);
		if(first_desc == NULL){
			TR("Error in Tx Descriptors memory allocation\n");
			return -ESYNOPGMACNOMEM;
		}
		gmacdev->TxDescCount = no_of_desc;
		gmacdev->TxDesc      = first_desc;
		gmacdev->TxDescDma   = dma_addr;

		for(i =0; i < gmacdev -> TxDescCount; i++){
			synopGMAC_tx_desc_init_ring(gmacdev->TxDesc + i, i == gmacdev->TxDescCount-1);
			TR("%02d %08x %08x\n",i, (unsigned int)(gmacdev->TxDesc + i), dma_addr + i*sizeof(DmaDesc));
		}

	}
	else{
		//Allocate the head descriptor
		first_desc = plat_alloc_consistent_dmaable_memory (pcidev, sizeof(DmaDesc),&dma_addr);
		if(first_desc == NULL){
			TR("Error in Tx Descriptor Memory allocation in Ring mode\n");
			return -ESYNOPGMACNOMEM;
		}
		gmacdev->TxDesc       = first_desc;
		gmacdev->TxDescDma    = dma_addr;

		TR("Tx===================================================================Tx\n");
		first_desc->buffer2   = gmacdev->TxDescDma;
		first_desc->data2     = (u32)gmacdev->TxDesc;

		gmacdev->TxDescCount = 1;

		for(i =0; i <(no_of_desc-1); i++){
			second_desc = plat_alloc_consistent_dmaable_memory(pcidev, sizeof(DmaDesc),&dma_addr);
			if(second_desc == NULL){
				TR("Error in Tx Descriptor Memory allocation in Chain mode\n");
				return -ESYNOPGMACNOMEM;
			}
			first_desc->buffer2  = dma_addr;
			first_desc->data2    = (u32)second_desc;

			second_desc->buffer2 = gmacdev->TxDescDma;
			second_desc->data2   = (u32)gmacdev->TxDesc;

			synopGMAC_tx_desc_init_chain(first_desc);
			TR("%02d %08x %08x %08x %08x %08x %08x %08x \n",gmacdev->TxDescCount, (u32)first_desc, first_desc->status, first_desc->length, first_desc->buffer1,first_desc->buffer2, first_desc->data1, first_desc->data2);
			gmacdev->TxDescCount += 1;
			first_desc = second_desc;
		}

		synopGMAC_tx_desc_init_chain(first_desc);
		TR("%02d %08x %08x %08x %08x %08x %08x %08x \n",gmacdev->TxDescCount, (u32)first_desc, first_desc->status, first_desc->length, first_desc->buffer1,first_desc->buffer2, first_desc->data1, first_desc->data2);
		TR("Tx===================================================================Tx\n");
	}

	gmacdev->TxNext = 0;
	gmacdev->TxBusy = 0;
	gmacdev->TxNextDesc = gmacdev->TxDesc;
	gmacdev->TxBusyDesc = gmacdev->TxDesc;
	gmacdev->BusyTxDesc  = 0;

	return -ESYNOPGMACNOERR;
}


/**
 * This sets up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures in ring mode or chain mode.
 * This function depends on the pcidev structure for allocation of consistent dma-able memory in case of linux.
 * This limitation is due to the fact that linux uses pci structure to allocate a dmable memory
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 *	- Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to pci_device structure.
 * @param[in] number of descriptor expected in rx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->RxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */
s32 synopGMAC_setup_rx_desc_queue(synopGMACdevice * gmacdev,struct platform_device * pcidev,u32 no_of_desc, u32 desc_mode)
{
	s32 i;

	DmaDesc *first_desc = NULL;
	DmaDesc *second_desc = NULL;
	dma_addr_t dma_addr;
	gmacdev->RxDescCount = 0;

	if(desc_mode == RINGMODE){
		TR("total size of memory required for Rx Descriptors in Ring Mode = 0x%08x\n",((sizeof(DmaDesc) * no_of_desc)));
		first_desc = plat_alloc_consistent_dmaable_memory (pcidev, sizeof(DmaDesc) * no_of_desc, &dma_addr);
		if(first_desc == NULL){
			TR("Error in Rx Descriptor Memory allocation in Ring mode\n");
			return -ESYNOPGMACNOMEM;
		}
		gmacdev->RxDescCount = no_of_desc;
		gmacdev->RxDesc      = first_desc;
		gmacdev->RxDescDma   = dma_addr;

		for(i =0; i < gmacdev -> RxDescCount; i++){
			synopGMAC_rx_desc_init_ring(gmacdev->RxDesc + i, i == gmacdev->RxDescCount-1);
			TR("%02d %08x %08x\n",i, (unsigned int)(gmacdev->RxDesc + i), dma_addr + i*sizeof(DmaDesc));
		}

	}
	else{
		//Allocate the head descriptor
		first_desc = plat_alloc_consistent_dmaable_memory (pcidev, sizeof(DmaDesc),&dma_addr);
		if(first_desc == NULL){
			TR("Error in Rx Descriptor Memory allocation in Ring mode\n");
			return -ESYNOPGMACNOMEM;
		}
		gmacdev->RxDesc       = first_desc;
		gmacdev->RxDescDma    = dma_addr;

		TR("Rx===================================================================Rx\n");
		first_desc->buffer2   = gmacdev->RxDescDma;
		first_desc->data2     = (u32) gmacdev->RxDesc;

		gmacdev->RxDescCount = 1;

		for(i =0; i < (no_of_desc-1); i++){
			second_desc = plat_alloc_consistent_dmaable_memory(pcidev, sizeof(DmaDesc),&dma_addr);
			if(second_desc == NULL){
				TR("Error in Rx Descriptor Memory allocation in Chain mode\n");
				return -ESYNOPGMACNOMEM;
			}
			first_desc->buffer2  = dma_addr;
			first_desc->data2    = (u32)second_desc;

			second_desc->buffer2 = gmacdev->RxDescDma;
			second_desc->data2   = (u32)gmacdev->RxDesc;

			synopGMAC_rx_desc_init_chain(first_desc);
			TR("%02d  %08x %08x %08x %08x %08x %08x %08x \n",gmacdev->RxDescCount, (u32)first_desc, first_desc->status, first_desc->length, first_desc->buffer1,first_desc->buffer2, first_desc->data1, first_desc->data2);
			gmacdev->RxDescCount += 1;
			first_desc = second_desc;
		}
		synopGMAC_rx_desc_init_chain(first_desc);
		TR("%02d  %08x %08x %08x %08x %08x %08x %08x \n",gmacdev->RxDescCount, (u32)first_desc, first_desc->status, first_desc->length, first_desc->buffer1,first_desc->buffer2, first_desc->data1, first_desc->data2);
		TR("Rx===================================================================Rx\n");

	}

	gmacdev->RxNext = 0;
	gmacdev->RxBusy = 0;
	gmacdev->RxNextDesc = gmacdev->RxDesc;
	gmacdev->RxBusyDesc = gmacdev->RxDesc;

	gmacdev->BusyRxDesc   = 0;

	return -ESYNOPGMACNOERR;
}

/**
 * This gives up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the pcidev structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 *     - Free the network buffer memory if any.
 *	- Fee the memory allocated for the descriptors.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to pci_device structure.
 * @param[in] number of descriptor expected in rx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note No referece should be made to descriptors once this function is called. This function is invoked when the device is closed.
 */
void synopGMAC_giveup_rx_desc_queue(synopGMACdevice * gmacdev, struct platform_device *pcidev, u32 desc_mode)
{
	s32 i;

	DmaDesc *first_desc = NULL;
	dma_addr_t first_desc_dma_addr;
	u32 status;
	dma_addr_t dma_addr1;
	dma_addr_t dma_addr2;
	u32 length1;
	u32 length2;
	u32 data1;
	u32 data2;

	if(desc_mode == RINGMODE){
		for(i =0; i < gmacdev -> RxDescCount; i++){
			synopGMAC_get_desc_data(gmacdev->RxDesc + i, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
			if((length1 != 0) && (data1 != 0)){
				dma_unmap_single(&pcidev->dev, dma_addr1, 0, DMA_FROM_DEVICE);
				dev_kfree_skb((struct sk_buff *) data1);	// free buffer1
				TR("(Ring mode) rx buffer1 %08x of size %d from %d rx descriptor is given back\n",data1, length1, i);
			}
			if((length2 != 0) && (data2 != 0)){
				dma_unmap_single(&pcidev->dev, dma_addr2, 0, DMA_FROM_DEVICE);
				dev_kfree_skb((struct sk_buff *) data2);	//free buffer2
				TR("(Ring mode) rx buffer2 %08x of size %d from %d rx descriptor is given back\n",data2, length2, i);
			}
		}
		plat_free_consistent_dmaable_memory(pcidev,(sizeof(DmaDesc) * gmacdev->RxDescCount),gmacdev->RxDesc,gmacdev->RxDescDma); //free descriptors memory
		TR("Memory allocated %08x  for Rx Desriptors (ring) is given back\n",(u32)gmacdev->RxDesc);
	}
	else{
		TR("rx-------------------------------------------------------------------rx\n");
		first_desc          = gmacdev->RxDesc;
		first_desc_dma_addr = gmacdev->RxDescDma;
		for(i =0; i < gmacdev -> RxDescCount; i++){
			synopGMAC_get_desc_data(first_desc, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
			TR("%02d %08x %08x %08x %08x %08x %08x %08x\n",i,(u32)first_desc,first_desc->status,first_desc->length,first_desc->buffer1,first_desc->buffer2,first_desc->data1,first_desc->data2);
			if((length1 != 0) && (data1 != 0)){
				dma_unmap_single(&pcidev->dev, dma_addr1, 0, DMA_FROM_DEVICE);
				dev_kfree_skb((struct sk_buff *) data1);	// free buffer1
				TR("(Chain mode) rx buffer1 %08x of size %d from %d rx descriptor is given back\n",data1, length1, i);
			}
			plat_free_consistent_dmaable_memory(pcidev,(sizeof(DmaDesc)),first_desc,first_desc_dma_addr); //free descriptors
			TR("Memory allocated %08x for Rx Descriptor (chain) at  %d is given back\n",data2,i);

			first_desc = (DmaDesc *)data2;
			first_desc_dma_addr = dma_addr2;
		}

		TR("rx-------------------------------------------------------------------rx\n");
	}
	gmacdev->RxDesc    = NULL;
	gmacdev->RxDescDma = 0;
	return;
}

/**
 * This gives up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the pcidev structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 *     - Free the network buffer memory if any.
 *     - Fee the memory allocated for the descriptors.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to pci_device structure.
 * @param[in] number of descriptor expected in tx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note No reference should be made to descriptors once this function is called. This function is invoked when the device is closed.
 */
void synopGMAC_giveup_tx_desc_queue(synopGMACdevice * gmacdev,struct platform_device * pcidev, u32 desc_mode)
{
	s32 i;

	DmaDesc *first_desc = NULL;
	dma_addr_t first_desc_dma_addr;
	u32 status;
	dma_addr_t dma_addr1;
	dma_addr_t dma_addr2;
	u32 length1;
	u32 length2;
	u32 data1;
	u32 data2;

	if(desc_mode == RINGMODE){
		for(i =0; i < gmacdev -> TxDescCount; i++){
			synopGMAC_get_desc_data(gmacdev->TxDesc + i,&status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
			if((length1 != 0) && (data1 != 0)){
				dma_unmap_single(&pcidev->dev ,dma_addr1,0,DMA_TO_DEVICE);
				dev_kfree_skb((struct sk_buff *) data1);	// free buffer1
				TR("(Ring mode) tx buffer1 %08x of size %d from %d rx descriptor is given back\n",data1, length1, i);
			}
			if((length2 != 0) && (data2 != 0)){
				dma_unmap_single(&pcidev->dev ,dma_addr2,0,DMA_TO_DEVICE);
				dev_kfree_skb((struct sk_buff *) data2);	//free buffer2
				TR("(Ring mode) tx buffer2 %08x of size %d from %d rx descriptor is given back\n",data2, length2, i);
			}
		}
		plat_free_consistent_dmaable_memory(pcidev,(sizeof(DmaDesc) * gmacdev->TxDescCount),gmacdev->TxDesc,gmacdev->TxDescDma); //free descriptors
		TR("Memory allocated %08x for Tx Desriptors (ring) is given back\n",(u32)gmacdev->TxDesc);
	}
	else{
		TR("tx-------------------------------------------------------------------tx\n");
		first_desc          = gmacdev->TxDesc;
		first_desc_dma_addr = gmacdev->TxDescDma;
		for(i =0; i < gmacdev -> TxDescCount; i++){
			synopGMAC_get_desc_data(first_desc, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
			TR("%02d %08x %08x %08x %08x %08x %08x %08x\n",i,(u32)first_desc,first_desc->status,first_desc->length,first_desc->buffer1,first_desc->buffer2,first_desc->data1,first_desc->data2);
			if((length1 != 0) && (data1 != 0)){
				dma_unmap_single(&pcidev->dev ,dma_addr1,0,DMA_TO_DEVICE);
				dev_kfree_skb((struct sk_buff *) data2);	// free buffer1
				TR("(Chain mode) tx buffer1 %08x of size %d from %d rx descriptor is given back\n",data1, length1, i);
			}
			plat_free_consistent_dmaable_memory(pcidev,(sizeof(DmaDesc)),first_desc,first_desc_dma_addr); //free descriptors
			TR("Memory allocated %08x for Tx Descriptor (chain) at  %d is given back\n",data2,i);

			first_desc = (DmaDesc *)data2;
			first_desc_dma_addr = dma_addr2;
		}
		TR("tx-------------------------------------------------------------------tx\n");

	}
	gmacdev->TxDesc    = NULL;
	gmacdev->TxDescDma = 0;
	return;
}
#ifdef AVB_SUPPORT
static void populate_tx_avb_data (u32 dma_index, u32 avb_frame_size, u32 * skb_data, synopGMACavbStruct * gmacavb)
{
	u32 data_count;
	u32 avtype;

	if(dma_index == 0){
		* skb_data++ = frame_header_ch0[0];
		* skb_data++ = frame_header_ch0[1];
		* skb_data++ = frame_header_ch0[2];
		* skb_data++ = frame_header_ch0[3]; //Ethernet header ends here
		for(data_count = 0; data_count < avb_frame_size;){
			* skb_data++ = FRAME_PATTERN_CH0;
			data_count += 4;// increment by 4 since we are writing one word at a time
		}
	}

	if(dma_index == 1){
		* skb_data++ = frame_header_ch1[0];
		* skb_data++ = frame_header_ch1[1];
		* skb_data++ = frame_header_ch1[2];
		* skb_data++ = frame_header_ch1[3];

		avtype = gmacavb->AvType;
		avtype = ((avtype << 8) | (avtype >> 8)) & 0x0000FFFF;
		* skb_data++ = (FRAME_PATTERN_CH1 <<16) | avtype; //AVB header ends here

		for(data_count = 0; data_count < avb_frame_size;){
			* skb_data++ = FRAME_PATTERN_CH1;
			data_count += 4;// increment by 4 since we are writing one word at a time
		}
	}
	if(dma_index == 2){
		* skb_data++ = frame_header_ch2[0];
		* skb_data++ = frame_header_ch2[1];
		* skb_data++ = frame_header_ch2[2];
		* skb_data++ = frame_header_ch2[3];

		avtype = gmacavb->AvType;
		avtype = ((avtype << 8) | (avtype >> 8)) & 0x0000FFFF;
		* skb_data++ = (FRAME_PATTERN_CH2 <<16) | avtype; //AVB header ends here

		for(data_count = 0; data_count < avb_frame_size;){
			* skb_data++ = FRAME_PATTERN_CH2;
			data_count += 4;// increment by 4 since we are writing one word at a time
		}
	}
}

static void synopGMAC_prepare_hw_for_avb_test (synopGMACPciNetworkAdapter *adapter)//synopGMACconfigureAvb
{
	synopGMACdevice *gmacdev;
	synopGMACdevice * gmacdev_ch[3]; // [0] for Best Effor Ethernet, [1] For DMA CH 1, [2] For DMA CH 2
	u8 ch_tx_desc_slot_no;
	u8 ch_tx_desc_slot_no_skip;

	synopGMACavbStruct    *gmacavb;
	u32 avb_frame_size,dma_index,desc_count;
	u32 status;
	u32 dma_addr1, dma_addr2;
	u32 length1, length2;
	u32 data1, data2;

	struct sk_buff *skb;

	gmacdev_ch[0] = adapter->synopGMACdev;
	gmacdev_ch[1] = adapter->synopGMACdev_ch1;
	gmacdev_ch[2] = adapter->synopGMACdev_ch2;

	gmacavb       = adapter->synopGMACavb;


	gmacdev=gmacdev_ch[0];
	synopGMACsetAvType(gmacdev,gmacavb->AvType);       //Set the AvType in Avmac register
	synopGMACsetAvPrio(gmacdev,gmacavb->PrioTagForAV); //set the Avpriority tag
	synopGMACsetAvCtrlCh(gmacdev,gmacavb->AvControlCh);//set the Av control Channel
	synopGMACsetAvPTPCh(gmacdev,gmacavb->PTPCh);       //set the PTP Channel

	//Channel 0 Specific Configuration

	synopGMACsetChPrioWts(gmacdev,gmacavb->Ch0PrioWts);
	synopGMACsetTxRxPrioPolicy(gmacdev,gmacavb->Ch0_tx_rx_prio_policy); //1=>RR 0=>Strict prioriyt
	synopGMACsetTxRxPrio(gmacdev,gmacavb->Ch0_use_tx_high_prio);        //1 => tx has high prio over rx
	synopGMACsetTxRxPrioRatio(gmacdev,gmacavb->Ch0_tx_rx_prio_ratio);   //tx-rx priority weights

	synopGMAC_TS_subsecond_init(gmacdev, 0x1E); //Because of 33MHz PCI clock
	synopGMAC_TS_digital_rollover_enable(gmacdev);
	synopGMAC_TS_enable(gmacdev);
	synopGMAC_TS_timestamp_init(gmacdev, 0x00, 0x00);

	//Channel 1 Specific Configuration
	gmacdev=gmacdev_ch[1];

	synopGMACsetChPrioWts(gmacdev,gmacavb->Ch1PrioWts);

	if(gmacavb->Ch1_EnableSlotCheck){
		synopGMACAvbEnSlot(gmacdev);
	}
	else {
		synopGMACAvbDisSlot(gmacdev);
		synopGMACAvbDisSlotInterrupt(gmacdev);
	}
	synopGMACAvbSetSlotCount(gmacdev,gmacavb->Ch1SlotCount);
	//synopGMACAvbEnSlotInterrupt(gmacdev);

	if(gmacavb->Ch1_AdvSlotInterval)
		synopGMACAvbEnAdvSlotInt(gmacdev);
	else
		synopGMACAvbDisAdvSlotInt(gmacdev);

	if(gmacavb->Ch1CrSh == 0){// when zero we need to disable creditshape algorithm
		synopGMACAvbDisableCrSh(gmacdev);
		synopGMACAvbDisableCrControl(gmacdev);
	}
	else{
		synopGMACAvbEnableCrSh(gmacdev);

		if(gmacavb->Ch1CreditControl)
			synopGMACAvbEnableCrControl(gmacdev);
		else
			synopGMACAvbDisableCrControl(gmacdev);

		synopGMACsetAvbSendSlope(gmacdev,gmacavb->Ch1SendSlope);
		synopGMACsetAvbIdleSlope(gmacdev,gmacavb->Ch1IdleSlope);
		synopGMACsetAvbHiCredit(gmacdev,gmacavb->Ch1HiCredit);
		synopGMACsetAvbLoCredit(gmacdev,gmacavb->Ch1LoCredit);
	}

	synopGMACsetTxRxPrioPolicy(gmacdev,gmacavb->Ch1_tx_rx_prio_policy); //1=>RR 0=>Strict prioriyt
	synopGMACsetTxRxPrio(gmacdev,gmacavb->Ch1_use_tx_high_prio); //1 => tx has high prio over rx
	synopGMACsetTxRxPrioRatio(gmacdev,gmacavb->Ch1_tx_rx_prio_ratio);//tx-rx priority weights

	//Channel 2 Specific Configuration
	gmacdev=gmacdev_ch[2];

	synopGMACsetChPrioWts(gmacdev,gmacavb->Ch2PrioWts);

	if(gmacavb->Ch2_EnableSlotCheck){
		synopGMACAvbEnSlot(gmacdev);
	}
	else {
		synopGMACAvbDisSlot(gmacdev);
		synopGMACAvbDisSlotInterrupt(gmacdev);
	}
	synopGMACAvbSetSlotCount(gmacdev,gmacavb->Ch2SlotCount);
	//synopGMACAvbEnSlotInterrupt(gmacdev);

	if(gmacavb->Ch2_AdvSlotInterval)
		synopGMACAvbEnAdvSlotInt(gmacdev);
	else
		synopGMACAvbDisAdvSlotInt(gmacdev);

	if(gmacavb->Ch2CrSh == 0){// when zero we need to disable creditshape algorithm
		synopGMACAvbDisableCrSh(gmacdev);
		synopGMACAvbDisableCrControl(gmacdev);
	}
	else{
		synopGMACAvbEnableCrSh(gmacdev);

		if(gmacavb->Ch2CreditControl)
			synopGMACAvbEnableCrControl(gmacdev);
		else
			synopGMACAvbDisableCrControl(gmacdev);

		synopGMACsetAvbSendSlope(gmacdev,gmacavb->Ch2SendSlope);
		synopGMACsetAvbIdleSlope(gmacdev,gmacavb->Ch2IdleSlope);
		synopGMACsetAvbHiCredit(gmacdev,gmacavb->Ch2HiCredit);
		synopGMACsetAvbLoCredit(gmacdev,gmacavb->Ch2LoCredit);
	}


	synopGMACsetTxRxPrioPolicy(gmacdev,gmacavb->Ch2_tx_rx_prio_policy); //1=>RR 0=>Strict prioriyt
	synopGMACsetTxRxPrio(gmacdev,gmacavb->Ch2_use_tx_high_prio);        //1 => tx has high prio over rx
	synopGMACsetTxRxPrioRatio(gmacdev,gmacavb->Ch2_tx_rx_prio_ratio);   //tx-rx priority weights


	TR("ChSelMask = %02x\n",gmacavb->ChSelMask);
	TR("DurationOfExp= %02x\n",gmacavb->DurationOfExp);
	TR("AvControlCh= %02x\n",gmacavb->AvControlCh);
	TR("PTPCh= %02x\n",gmacavb->PTPCh);
	TR("PrioTagForAV= %02x\n",gmacavb->PrioTagForAV);
	TR("AvType= %04x\n",gmacavb->AvType);

	TR("Ch2PrioWts = %08x\n",gmacavb->Ch2PrioWts);
	TR("Ch2Bw= %02x\n",gmacavb->Ch2Bw);
	TR("Ch2_frame_size= %08x\n",gmacavb->Ch2_frame_size);
	TR("Ch2_EnableSlotCheck= %02x\n",gmacavb->Ch2_EnableSlotCheck);
	TR("Ch2_AdvSlotInterval= %02x\n",gmacavb->Ch2_AdvSlotInterval);
	TR("Ch2CrSh= %02x\n",gmacavb->Ch2CrSh);
	TR("Ch2SlotCount= %02x\n",gmacavb->Ch2SlotCount);
	TR("Ch2AvgBitsPerSlot= %08x\n",gmacavb->Ch2AvgBitsPerSlot);
	TR("Ch2AvgBitsPerSlotAccH= %08x\n",gmacavb->Ch2AvgBitsPerSlotAccH);
	TR("Ch2AvgBitsPerSlotAccL= %08x\n",gmacavb->Ch2AvgBitsPerSlotAccL);
	TR("Ch2CreditControl= %02x\n",gmacavb->Ch2CreditControl);
	TR("Ch2_tx_rx_prio_policy= %02x\n",gmacavb->Ch2_tx_rx_prio_policy);
	TR("Ch2_use_tx_high_prio= %02x\n",gmacavb->Ch2_use_tx_high_prio);
	TR("Ch2_tx_rx_prio_ratio= %02x\n",gmacavb->Ch2_tx_rx_prio_ratio);
	TR("Ch2SendSlope= %08x\n",gmacavb->Ch2SendSlope);
	TR("Ch2IdleSlope= %08x\n",gmacavb->Ch2IdleSlope);
	TR("Ch2HiCredit= %08x\n",gmacavb->Ch2HiCredit);
	TR("Ch2LoCredit= %08x\n",gmacavb->Ch2LoCredit);
	TR("Ch2FramecountTx= %08x\n",gmacavb->Ch2FramecountTx);
	TR("Ch2FramecountRx= %08x\n",gmacavb->Ch2FramecountRx);

	TR("Ch1PrioWts = %08x\n",gmacavb->Ch1PrioWts);
	TR("Ch1Bw= %02x\n",gmacavb->Ch1Bw);
	TR("Ch1_frame_size= %08x\n",gmacavb->Ch1_frame_size);
	TR("Ch1_EnableSlotCheck= %02x\n",gmacavb->Ch1_EnableSlotCheck);
	TR("Ch1_AdvSlotInterval= %02x\n",gmacavb->Ch1_AdvSlotInterval);
	TR("Ch1CrSh= %02x\n",gmacavb->Ch1CrSh);
	TR("Ch1SlotCount= %02x\n",gmacavb->Ch1SlotCount);
	TR("Ch1AvgBitsPerSlot= %08x\n",gmacavb->Ch1AvgBitsPerSlot);
	TR("Ch1AvgBitsPerSlotAccH= %08x\n",gmacavb->Ch1AvgBitsPerSlotAccH);
	TR("Ch1AvgBitsPerSlotAccL= %08x\n",gmacavb->Ch1AvgBitsPerSlotAccL);
	TR("Ch1CreditControl= %02x\n",gmacavb->Ch1CreditControl);
	TR("Ch1_tx_rx_prio_policy= %02x\n",gmacavb->Ch1_tx_rx_prio_policy);
	TR("Ch1_use_tx_high_prio= %02x\n",gmacavb->Ch1_use_tx_high_prio);
	TR("Ch1_tx_rx_prio_ratio= %02x\n",gmacavb->Ch1_tx_rx_prio_ratio);
	TR("Ch1SendSlope= %08x\n",gmacavb->Ch1SendSlope);
	TR("Ch1IdleSlope= %08x\n",gmacavb->Ch1IdleSlope);
	TR("Ch1HiCredit= %08x\n",gmacavb->Ch1HiCredit);
	TR("Ch1LoCredit= %08x\n",gmacavb->Ch1LoCredit);
	TR("Ch1FramecountTx= %08x\n",gmacavb->Ch1FramecountTx);
	TR("Ch1FramecountRx= %08x\n",gmacavb->Ch1FramecountRx);

	TR("Ch0PrioWts = %08x\n",gmacavb->Ch0PrioWts);
	TR("Ch0_frame_size= %08x\n",gmacavb->Ch0_frame_size);
	TR("Ch0_tx_rx_prio_policy= %02x\n",gmacavb->Ch0_tx_rx_prio_policy);
	TR("Ch0_use_tx_high_prio= %02x\n",gmacavb->Ch0_use_tx_high_prio);
	TR("Ch0_tx_rx_prio_ratio= %02x\n",gmacavb->Ch0_tx_rx_prio_ratio);
	TR("Ch0FramecountTx= %08x\n",gmacavb->Ch0FramecountTx);
	TR("Ch0FramecountRx= %08x\n",gmacavb->Ch0FramecountRx);

	//Descriptor Memory Allocation for Transmission.
	avb_frame_size = 0;//Initialized to remove compiler warning
	for(dma_index = 0; dma_index<3; dma_index++){
		gmacdev = gmacdev_ch[dma_index];
		if(dma_index == 0)
			avb_frame_size = gmacavb->Ch0_frame_size;
		else if (dma_index == 1)
			avb_frame_size = gmacavb->Ch1_frame_size;
		else if (dma_index == 2)
			avb_frame_size = gmacavb->Ch2_frame_size;

		TR("avb_frame_size of %2d channel = %d\n",dma_index,avb_frame_size);
#if 0
		for(desc_count = 0; desc_count < gmacdev->TxDescCount; desc_count++  ){
			TR("dma_index = %02d desc_count = %02d\n",dma_index,desc_count);
			synopGMAC_get_desc_data(gmacdev->TxDesc + desc_count, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
			TR("status=%08x,dma_addr1 = %08x, length1 = %08x, data1 = %08x, dma_addr2 = %08x, length2 = %08x, data2 = %08x\n",
					status, dma_addr1,length1,data1,dma_addr2,length2,data2 );
			status = 0;
			if(((struct sk_buff *)data1) != NULL){
				TR("data1 is not NULL\n");
				skb = (struct sk_buff *) data1;
				populate_tx_avb_data (dma_index, avb_frame_size, (u32 *)skb->data, gmacavb);
				synopGMAC_set_desc_data(gmacdev->TxDesc + desc_count, status, dma_addr1, avb_frame_size,data1, dma_addr2,length2, data2);
			}
		}

#endif
		ch_tx_desc_slot_no = 0;
		ch_tx_desc_slot_no_skip = 0;
		if(dma_index == 1){//For channel 1
			ch_tx_desc_slot_no       = gmacavb->Ch1_tx_desc_slot_no_start;
			ch_tx_desc_slot_no_skip  = gmacavb->Ch1_tx_desc_slot_no_skip;
		}
		if(dma_index == 2){//For channel 2
			ch_tx_desc_slot_no       = gmacavb->Ch2_tx_desc_slot_no_start;
			ch_tx_desc_slot_no_skip  = gmacavb->Ch2_tx_desc_slot_no_skip;
		}


		for(desc_count = 0; desc_count < gmacdev->TxDescCount; desc_count++  ){

			TR("dma_index = %02d desc_count = %02d\n",dma_index,desc_count);
			synopGMAC_get_desc_data(gmacdev->TxDesc + desc_count, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
			TR("status=%08x,dma_addr1 = %08x, length1 = %08x, data1 = %08x, dma_addr2 = %08x, length2 = %08x, data2 = %08x\n",
					status, dma_addr1,length1,data1,dma_addr2,length2,data2 );
			status = 0;
			if(((struct sk_buff *)data1) != NULL){
				TR("data1 is not NULL\n");
				skb = (struct sk_buff *) data1;
				printk(KERN_CRIT "dma_index = %02d desc_count=%03d ch_tx_desc_slot_no = %01x\n",dma_index,desc_count,ch_tx_desc_slot_no);
				if(dma_index !=0){
					status = (ch_tx_desc_slot_no << 3);
					if(ch_tx_desc_slot_no_skip != 0){
						if(((desc_count + 1) % ch_tx_desc_slot_no_skip) == 0){
							ch_tx_desc_slot_no ++;
							ch_tx_desc_slot_no = ch_tx_desc_slot_no & 0xF;
						}
					}
				}
				populate_tx_avb_data (dma_index, avb_frame_size, (u32 *)skb->data, gmacavb);
				synopGMAC_set_desc_data(gmacdev->TxDesc + desc_count, status, dma_addr1, avb_frame_size,data1, dma_addr2,length2, data2);
			}
		}

	}

}

static void synopGMAC_AVB_timer_expired(u32 notused)
{
	u32 dma_index;
	synopGMACdevice       *gmacdev   ;
	synopGMACdevice * gmacdev_ch[3]; // [0] for Best Effor Ethernet, [1] For DMA CH 1, [2] For DMA CH 2

	synopGMACPciNetworkAdapter *adapter = (synopGMACPciNetworkAdapter *)synopGMAC_AVB_test_timer.data;

	gmacdev_ch[0] = adapter->synopGMACdev;
	gmacdev_ch[1] = adapter->synopGMACdev_ch1;
	gmacdev_ch[2] = adapter->synopGMACdev_ch2;

	for(dma_index=0;dma_index <3; dma_index ++){
		gmacdev= gmacdev_ch[dma_index];
		synopGMAC_disable_dma_tx(gmacdev);// Disable transmission
		if(dma_index !=0){
			synopGMACAvbDisSlotInterrupt(gmacdev);// Disable the Slot interrupt if enabled for channel 1
			synopGMACAvbDisSlotInterrupt(gmacdev);// Disable the Slot interrupt if enabled for channel 2
		}
	}

	plat_delay(10000); 		//Allow any pending buffer to be read by host
	printk(KERN_CRIT "AVB Experiment is Complete... You can Retrieve the Report\n");
}

static void synopGMAC_AVB_setup_timer(struct net_device *netdev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	synopGMACPciNetworkAdapter *adapter = netdev->priv;
#else
	synopGMACPciNetworkAdapter *adapter = netdev_priv(netdev);
#endif
	synopGMACavbStruct         *gmacavb = adapter->synopGMACavb;

	TR("Setting up the AVB Timer\n");
	init_timer(&synopGMAC_AVB_test_timer);
	synopGMAC_AVB_test_timer.function = (void *) synopGMAC_AVB_timer_expired;
	synopGMAC_AVB_test_timer.data     = (u32) adapter;
	//    synopGMAC_AVB_test_timer.expires = (CHECK_TIME * 2) + jiffies;
	synopGMAC_AVB_test_timer.expires = (CHECK_TIME * (gmacavb->DurationOfExp)) + jiffies;
	//    synopGMAC_AVB_test_timer.expires = (HZ * (gmacavb->DurationOfExp) * 60) + jiffies;
}

static void synopGMAC_AVB_run_test(struct net_device *netdev, u32 start_or_continue)
{
	u32 dma_index;
	synopGMACPciNetworkAdapter * adapter;
	synopGMACavbStruct         * gmacavb;

	synopGMACdevice       *gmacdev   ;
	synopGMACdevice       *gmacdev_ch[3]; // [0] for Best Effor Ethernet, [1] For DMA CH 1, [2] For DMA CH 2

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter =  (synopGMACPciNetworkAdapter *) netdev->priv;
#else
	adapter =  (synopGMACPciNetworkAdapter *) netdev_priv(netdev);
#endif
	gmacavb =  (synopGMACavbStruct *)adapter->synopGMACavb;

	gmacdev_ch[0] = adapter->synopGMACdev;
	gmacdev_ch[1] = adapter->synopGMACdev_ch1;
	gmacdev_ch[2] = adapter->synopGMACdev_ch2;

	if(start_or_continue) //If start then only start the timer, For continue test timer is not started
		add_timer(&synopGMAC_AVB_test_timer);// Start the experiment timer before enabling the DMA's

	for(dma_index = 0; dma_index<3; dma_index++){
		gmacdev = gmacdev_ch[dma_index];
		if((gmacavb->ChSelMask) & (1 << dma_index)){ // Enable only that channel for which Enable is set
			TR("Enabling the DMA Channel[%02d]\n",dma_index);
			//	synopGMAC_clear_interrupt(gmacdev);		  //Clear all pending interrupts
			synopGMACAvbEnSlotInterrupt(gmacdev);
			synopGMAC_enable_dma_tx(gmacdev);                 //Enable Dma Tx for all channels
		}
	}

}
#endif

/**
 * Function to handle housekeeping after a packet is transmitted over the wire.
 * After the transmission of a packet DMA generates corresponding interrupt
 * (if it is enabled). It takes care of returning the sk_buff to the linux
 * kernel, updating the networking statistics and tracking the descriptors.
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context
 */
void synop_handle_transmit_over(struct net_device *netdev)
{
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;
	struct platform_device *pcidev;
	s32 desc_index;
	u32 data1, data2;
	u32 status;
	u32 length1, length2;
	u32 dma_addr1, dma_addr2;
#ifdef ENH_DESC_8W
	u32 ext_status;
	u16 time_stamp_higher;
	u32 time_stamp_high;
	u32 time_stamp_low;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = netdev->priv;
#else
	adapter = netdev_priv(netdev);
#endif
	if(adapter == NULL){
		TR("Unknown Device\n");
		return;
	}

	gmacdev = adapter->synopGMACdev;
	if(gmacdev == NULL){
		TR("GMAC device structure is missing\n");
		return;
	}

	pcidev  = (struct platform_device *)adapter->synopGMACpcidev;
	/*Handle the transmit Descriptors*/
	do {
#ifdef ENH_DESC_8W
		desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2,&ext_status,&time_stamp_high,&time_stamp_low);
		synopGMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);
#else
		desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
#endif
		//desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr, &length, &data1);
		if(desc_index >= 0 && data1 != 0){
			TR("Finished Transmit at Tx Descriptor %d for skb 0x%08x and buffer = %08x whose status is %08x \n", desc_index,data1,dma_addr1,status);
#ifdef	IPC_OFFLOAD
			if(synopGMAC_is_tx_ipv4header_checksum_error(gmacdev, status)){
				TR("Harware Failed to Insert IPV4 Header Checksum\n");
			}
			if(synopGMAC_is_tx_payload_checksum_error(gmacdev, status)){
				TR("Harware Failed to Insert Payload Checksum\n");
			}
#endif

			dma_unmap_single(&pcidev->dev,dma_addr1,length1,DMA_TO_DEVICE);
			dev_kfree_skb_irq((struct sk_buff *)data1);

			if(synopGMAC_is_desc_valid(status)){
				adapter->synopGMACNetStats.tx_bytes += length1;
				adapter->synopGMACNetStats.tx_packets++;
			}
			else {
				TR("Error in Status %08x\n",status);
				adapter->synopGMACNetStats.tx_errors++;
				adapter->synopGMACNetStats.tx_aborted_errors += synopGMAC_is_tx_aborted(status);
				adapter->synopGMACNetStats.tx_carrier_errors += synopGMAC_is_tx_carrier_error(status);
			}
		}	adapter->synopGMACNetStats.collisions += synopGMAC_get_tx_collision_count(status);
	} while(desc_index >= 0);
	netif_wake_queue(netdev);
}




/**
 * Function to Receive a packet from the interface.
 * After Receiving a packet, DMA transfers the received packet to the system memory
 * and generates corresponding interrupt (if it is enabled). This function prepares
 * the sk_buff for received packet after removing the ethernet CRC, and hands it over
 * to linux networking stack.
 *	- Updataes the networking interface statistics
 *	- Keeps track of the rx descriptors
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context.
 */

u32 synop_handle_received_data(struct net_device *netdev, u32 budget)
{
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;
	struct platform_device *pcidev;
	s32 desc_index;

	u32 data1;
	u32 data2;
	u32 len;
	u32 status;
	u32 dma_addr1;
	u32 dma_addr2;
#ifdef ENH_DESC_8W
	u32 ext_status;
	u16 time_stamp_higher;
	u32 time_stamp_high;
	u32 time_stamp_low;
#endif
	//u32 length;

	struct sk_buff *skb; //This is the pointer to hold the received data
#ifdef DEBUG
	u32 i;
#endif
	u32 packets_done = 0;

	TR("%s\n",__FUNCTION__);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = netdev->priv;
#else
	adapter = netdev_priv(netdev);
#endif
	if(adapter == NULL){
		TR("Unknown Device\n");
		return packets_done;
	}

	gmacdev = adapter->synopGMACdev;
	if(gmacdev == NULL){
		TR("GMAC device structure is missing\n");
		return packets_done;
	}

	pcidev  = (struct platform_device *)adapter->synopGMACpcidev;
	/*Handle the Receive Descriptors*/
	do{
		if (packets_done >= budget) break;
#ifdef ENH_DESC_8W
		desc_index = synopGMAC_get_rx_qptr(gmacdev, &status,&dma_addr1,NULL, &data1,&dma_addr2,NULL,&data2,&ext_status,&time_stamp_high,&time_stamp_low);
		if(desc_index >0){
			synopGMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);
			TR("S:%08x ES:%08x DA1:%08x d1:%08x DA2:%08x d2:%08x TSH:%08x TSL:%08x TSHW:%08x \n",status,ext_status,dma_addr1, data1,dma_addr2,data2, time_stamp_high,time_stamp_low,time_stamp_higher);
		}
#else

		desc_index = synopGMAC_get_rx_qptr(gmacdev, &status,&dma_addr1,NULL, &data1,&dma_addr2,NULL,&data2);

#endif
		//desc_index = synopGMAC_get_rx_qptr(gmacdev, &status,&dma_addr,NULL, &data1);

		if(desc_index >= 0 && data1 != 0){
			TR("Received Data at Rx Descriptor %d for skb 0x%08x whose status is %08x\n",desc_index,data1,status);
			/*At first step unmapped the dma address*/
			dma_unmap_single(&pcidev->dev,dma_addr1,0,DMA_FROM_DEVICE);

			skb = (struct sk_buff *)data1;
			if(synopGMAC_is_rx_desc_valid(status)){
				len =  synopGMAC_get_rx_desc_frame_length(status) - 4; //Not interested in Ethernet CRC bytes
#ifdef DEBUG
				printk("%s: len: %x\n", __FUNCTION__, len + 4);

				printk("gary:\n");
				for(i=0; i < (len + 4); i++) {
					printk("%02x ", skb->data[i]);
					if(((i+1)%8) == 0) {
						printk("\n");
					}
				}
				printk("gary\n");
#endif
				skb_put(skb,len);
#ifdef IPC_OFFLOAD
				// Now lets check for the IPC offloading
				/*  Since we have enabled the checksum offloading in hardware, lets inform the kernel
				    not to perform the checksum computation on the incoming packet. Note that ip header
				    checksum will be computed by the kernel immaterial of what we inform. Similary TCP/UDP/ICMP
				    pseudo header checksum will be computed by the stack. What we can inform is not to perform
				    payload checksum.
				    When CHECKSUM_UNNECESSARY is set kernel bypasses the checksum computation.
				    */

				TR("Checksum Offloading will be done now\n");
				skb->ip_summed = CHECKSUM_UNNECESSARY;

#ifdef ENH_DESC_8W
				if(synopGMAC_is_ext_status(gmacdev, status)){ // extended status present indicates that the RDES4 need to be probed
					TR("Extended Status present\n");
					if(synopGMAC_ES_is_IP_header_error(gmacdev,ext_status)){       // IP header (IPV4) checksum error
						//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
						TR("(EXTSTS)Error in IP header error\n");
						skb->ip_summed = CHECKSUM_NONE;     //Let Kernel compute the checkssum
					}
					if(synopGMAC_ES_is_rx_checksum_bypassed(gmacdev,ext_status)){   // Hardware engine bypassed the checksum computation/checking
						TR("(EXTSTS)Hardware bypassed checksum computation\n");
						skb->ip_summed = CHECKSUM_NONE;             // Let Kernel compute the checksum
					}
					if(synopGMAC_ES_is_IP_payload_error(gmacdev,ext_status)){       // IP payload checksum is in error (UDP/TCP/ICMP checksum error)
						TR("(EXTSTS) Error in EP payload\n");
						skb->ip_summed = CHECKSUM_NONE;             // Let Kernel compute the checksum
					}
				}
				else{ // No extended status. So relevant information is available in the status itself
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxNoChkError ){
						TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 4>  \n");
						skb->ip_summed = CHECKSUM_UNNECESSARY;	//Let Kernel bypass computing the Checksum
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
						//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
						TR(" Error in 16bit IPV4 Header Checksum <Chk Status = 6>  \n");
						skb->ip_summed = CHECKSUM_UNNECESSARY;	//Let Kernel bypass the TCP/UDP checksum computation
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxLenLT600 ){
						TR("IEEE 802.3 type frame with Length field Lesss than 0x0600 <Chk Status = 0> \n");
						skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrPayLoadChkBypass ){
						TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 1>\n");
						skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxChkBypass ){
						TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 3>  \n");
						skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxPayLoadChkError ){
						TR(" TCP/UDP payload checksum Error <Chk Status = 5>  \n");
						skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
						//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
						TR(" Both IP header and Payload Checksum Error <Chk Status = 7>  \n");
						skb->ip_summed = CHECKSUM_NONE;	        //Let Kernel compute the Checksum
					}
				}
#else
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxNoChkError ){
					TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 4>  \n");
					skb->ip_summed = CHECKSUM_UNNECESSARY;	//Let Kernel bypass computing the Checksum
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
					//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
					TR(" Error in 16bit IPV4 Header Checksum <Chk Status = 6>  \n");
					skb->ip_summed = CHECKSUM_UNNECESSARY;	//Let Kernel bypass the TCP/UDP checksum computation
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxLenLT600 ){
					TR("IEEE 802.3 type frame with Length field Lesss than 0x0600 <Chk Status = 0> \n");
					skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrPayLoadChkBypass ){
					TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 1>\n");
					skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxChkBypass ){
					TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 3>  \n");
					skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxPayLoadChkError ){
					TR(" TCP/UDP payload checksum Error <Chk Status = 5>  \n");
					skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
					//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
					TR(" Both IP header and Payload Checksum Error <Chk Status = 7>  \n");
					skb->ip_summed = CHECKSUM_NONE;	        //Let Kernel compute the Checksum
				}

#endif
#endif //IPC_OFFLOAD
				skb->dev = netdev;
				skb->protocol = eth_type_trans(skb, netdev);
				if(napi)
					netif_receive_skb(skb);
				else
					netif_rx(skb);
				packets_done++;

				netdev->last_rx = jiffies;
				adapter->synopGMACNetStats.rx_packets++;
				adapter->synopGMACNetStats.rx_bytes += len;
			}
			else{
				/*Now the present skb should be set free*/
				dev_kfree_skb_irq(skb);
				printk(KERN_CRIT "s: %08x\n",status);
				adapter->synopGMACNetStats.rx_errors++;
				adapter->synopGMACNetStats.collisions       += synopGMAC_is_rx_frame_collision(status);
				adapter->synopGMACNetStats.rx_crc_errors    += synopGMAC_is_rx_crc(status);
				adapter->synopGMACNetStats.rx_frame_errors  += synopGMAC_is_frame_dribbling_errors(status);
				adapter->synopGMACNetStats.rx_length_errors += synopGMAC_is_rx_frame_length_errors(status);
			}

			//Now lets allocate the skb for the emptied descriptor
			skb = dev_alloc_skb(netdev->mtu + ETHERNET_PACKET_EXTRA);
			if(skb == NULL){
				TR("SKB memory allocation failed \n");
				adapter->synopGMACNetStats.rx_dropped++;
			}

			dma_addr1 = dma_map_single(&pcidev->dev,skb->data,skb_tailroom(skb),DMA_FROM_DEVICE);

			desc_index = synopGMAC_set_rx_qptr(gmacdev,dma_addr1, skb_tailroom(skb), (u32)skb,0,0,0);

			if (desc_index < 0) {
				TR("Cannot set Rx Descriptor for skb %08x\n",(u32)skb);
				dev_kfree_skb_irq(skb);
			}
		}
	}while(desc_index >= 0);

	return packets_done;
}

#ifdef AVB_SUPPORT

/**
 * Function to Receive a packet from the interface.
 * After Receiving a packet, DMA transfers the received packet to the system memory
 * and generates corresponding interrupt (if it is enabled). This function prepares
 * the sk_buff for received packet after removing the ethernet CRC, and hands it over
 * to linux networking stack.
 *	- Updataes the networking interface statistics
 *	- Keeps track of the rx descriptors
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context.
 */

void synop_handle_received_data_avb(struct net_device *netdev, u32 channel_index)
{
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;
	struct platform_dev *pcidev;
	synopGMACavbStruct *gmacavb;
	s32 desc_index;

	u32 frame_count;

	u32 data1;
	u32 data2;
	u32 len;
	u32 status;
	u32 dma_addr1;
	u32 dma_addr2;
#ifdef ENH_DESC_8W
	u32 ext_status;
	u16 time_stamp_higher;
	u32 time_stamp_high;
	u32 time_stamp_low;
#endif
	//u32 length;

	struct sk_buff *skb; //This is the pointer to hold the received data

	TR("%s\n",__FUNCTION__);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = netdev->priv;
#else
	adapter = netdev_priv(netdev);
#endif
	if(adapter == NULL){
		TR("Unknown Device\n");
		return;
	}
	gmacavb = adapter->synopGMACavb;
	gmacdev = adapter->synopGMACdev; //Initialized to avoid compiler warning

	//Get the apporpriate handle for gmacdev for respective channel
	if(channel_index == 0){
		gmacdev = adapter->synopGMACdev;
		if(gmacdev == NULL){
			TR("GMAC device structure is missing\n");
			return;
		}
	}
	if(channel_index == 1){
		gmacdev = adapter->synopGMACdev_ch1;
		if(gmacdev == NULL){
			TR("GMAC device structure is missing\n");
			return;
		}
	}
	if(channel_index == 2){
		gmacdev = adapter->synopGMACdev_ch2;
		if(gmacdev == NULL){
			TR("GMAC device structure is missing\n");
			return;
		}
	}


	pcidev  = (struct platform_device *)adapter->synopGMACpcidev;
	/*Handle the Receive Descriptors*/
	do{
#ifdef ENH_DESC_8W
		desc_index = synopGMAC_get_rx_qptr(gmacdev, &status,&dma_addr1,NULL, &data1,&dma_addr2,NULL,&data2,&ext_status,&time_stamp_high,&time_stamp_low);
		if(desc_index >0){
			synopGMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);
			TR("S:%08x ES:%08x DA1:%08x d1:%08x DA2:%08x d2:%08x TSH:%08x TSL:%08x TSHW:%08x \n",status,ext_status,dma_addr1, data1,dma_addr2,data2, time_stamp_high,time_stamp_low,time_stamp_higher);
		}
#else
		desc_index = synopGMAC_get_rx_qptr(gmacdev, &status,&dma_addr1,NULL, &data1,&dma_addr2,NULL,&data2);
#endif

		if(desc_index >= 0 && data1 != 0){
			TR("Received Data at Rx Descriptor %d for skb 0x%08x whose status is %08x\n",desc_index,data1,status);
			/*At first step unmapped the dma address*/
			if(synopGMAC_is_rx_desc_valid(status)){
				len =  synopGMAC_get_rx_desc_frame_length(status) - 4; //Not interested in Ethernet CRC bytes
#ifdef IPC_OFFLOAD
				// Now lets check for the IPC offloading
				/*  Since we have enabled the checksum offloading in hardware, lets inform the kernel
				    not to perform the checksum computation on the incoming packet. Note that ip header
				    checksum will be computed by the kernel immaterial of what we inform. Similary TCP/UDP/ICMP
				    pseudo header checksum will be computed by the stack. What we can inform is not to perform
				    payload checksum.
				    When CHECKSUM_UNNECESSARY is set kernel bypasses the checksum computation.
				    */


#ifdef ENH_DESC_8W
				if(synopGMAC_is_ext_status(gmacdev, status)){ // extended status present indicates that the RDES4 need to be probed
					TR("Extended Status present\n");
					if(synopGMAC_ES_is_IP_header_error(gmacdev,ext_status)){       // IP header (IPV4) checksum error
						//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
						TR("(EXTSTS)Error in IP header error\n");
					}
					if(synopGMAC_ES_is_rx_checksum_bypassed(gmacdev,ext_status)){   // Hardware engine bypassed the checksum computation/checking
						TR("(EXTSTS)Hardware bypassed checksum computation\n");
					}
					if(synopGMAC_ES_is_IP_payload_error(gmacdev,ext_status)){       // IP payload checksum is in error (UDP/TCP/ICMP checksum error)
						TR("(EXTSTS) Error in EP payload\n");
					}
				}
				else{ // No extended status. So relevant information is available in the status itself
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxNoChkError ){
						TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 4>  \n");
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
						//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
						TR(" Error in 16bit IPV4 Header Checksum <Chk Status = 6>  \n");
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxLenLT600 ){
						TR("IEEE 802.3 type frame with Length field Lesss than 0x0600 <Chk Status = 0> \n");
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrPayLoadChkBypass ){
						TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 1>\n");
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxChkBypass ){
						TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 3>  \n");
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxPayLoadChkError ){
						TR(" TCP/UDP payload checksum Error <Chk Status = 5>  \n");
					}
					if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
						//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
						TR(" Both IP header and Payload Checksum Error <Chk Status = 7>  \n");
					}
				}
#else
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxNoChkError ){
					TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 4>  \n");
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
					//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
					TR(" Error in 16bit IPV4 Header Checksum <Chk Status = 6>  \n");
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxLenLT600 ){
					TR("IEEE 802.3 type frame with Length field Lesss than 0x0600 <Chk Status = 0> \n");
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrPayLoadChkBypass ){
					TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 1>\n");
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxChkBypass ){
					TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 3>  \n");
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxPayLoadChkError ){
					TR(" TCP/UDP payload checksum Error <Chk Status = 5>  \n");
				}
				if(synopGMAC_is_rx_checksum_error(gmacdev, status) == RxIpHdrChkError ){
					//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
					TR(" Both IP header and Payload Checksum Error <Chk Status = 7>  \n");
				}

#endif
#endif //IPC_OFFLOAD

				if(ext_status & DescRxAvTagPktRx){
					if (((ext_status & DescRxVlanPrioVal) >> DescRxVlanPrioShVal) >= gmacavb->PrioTagForAV)
						gmacavb->Ch2FramecountRx++;
					else
						gmacavb->Ch1FramecountRx++;
				}
				else{
					gmacavb->Ch0FramecountRx++;
				}

				netdev->last_rx = jiffies;
				adapter->synopGMACNetStats.rx_packets++;
				adapter->synopGMACNetStats.rx_bytes += len;
			}
			else{
				printk(KERN_CRIT "Packet Dropped here due to status = %08x\n",status);
				adapter->synopGMACNetStats.rx_errors++;
				adapter->synopGMACNetStats.collisions       += synopGMAC_is_rx_frame_collision(status);
				adapter->synopGMACNetStats.rx_crc_errors    += synopGMAC_is_rx_crc(status);
				adapter->synopGMACNetStats.rx_frame_errors  += synopGMAC_is_frame_dribbling_errors(status);
				adapter->synopGMACNetStats.rx_length_errors += synopGMAC_is_rx_frame_length_errors(status);
			}
			//Now lets allocate the skb for the emptied descriptor
			skb = (struct sk_buff *)data1;
			for(frame_count = 0; frame_count<10; frame_count++)
				TR("skb->data[%02d]= %08x\n",frame_count,* (u32 *)((skb->data)+(frame_count*4)));
			desc_index = synopGMAC_set_rx_qptr(gmacdev,dma_addr1, skb_tailroom(skb), (u32)skb,0,0,0);

			if(desc_index < 0){
				TR("Cannot set Rx Descriptor for skb %08x\n",(u32)skb);
			}

		}
	}while(desc_index >= 0);
}

/**
 * Function to handle housekeeping after a packet is transmitted over the wire.
 * After the transmission of a packet DMA generates corresponding interrupt
 * (if it is enabled). It takes care of returning the sk_buff to the linux
 * kernel, updating the networking statistics and tracking the descriptors.
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context
 */
void synop_handle_transmit_over_avb(struct net_device *netdev, u32 channel_index)
{
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;
	struct pci_dev *pcidev;
	synopGMACavbStruct *gmacavb;
	s32 desc_index;
	s32 desc_status;
	u32 data1, data2;
	u32 status;
	u32 length1, length2;
	u32 dma_addr1, dma_addr2;

	u32 avb_frame_size;

#ifdef ENH_DESC_8W
	u32 ext_status;
	u16 time_stamp_higher;
	u32 time_stamp_high;
	u32 time_stamp_low;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = netdev->priv;
#else
	adapter = netdev_priv(netdev);
#endif
	if(adapter == NULL){
		TR("Unknown Device\n");
		return;
	}
	gmacavb = adapter -> synopGMACavb;
	gmacdev = adapter->synopGMACdev;//Initialized to avaoid compiler warning
	avb_frame_size = 0;// Initialized to avoid compier warning
	if(channel_index == 0)
	{
		gmacdev = adapter->synopGMACdev;
		if(gmacdev == NULL){
			TR("GMAC device structure is missing for Best Effor Ethernet Channel\n");
			return;
		}
		avb_frame_size = gmacavb->Ch0_frame_size;
	}
	if(channel_index == 1)
	{
		gmacdev = adapter->synopGMACdev_ch1;
		if(gmacdev == NULL){
			TR("GMAC device structure is missing for DMA Channel 1\n");
			return;
		}
		avb_frame_size = gmacavb->Ch1_frame_size;
	}
	if(channel_index == 2)
	{
		gmacdev = adapter->synopGMACdev_ch2;
		if(gmacdev == NULL){
			TR("GMAC device structure is missing for DMA Channel 2\n");
			return;
		}
		avb_frame_size = gmacavb->Ch2_frame_size;
	}

	pcidev  = (struct pci_dev *)adapter->synopGMACpcidev;
	/*Handle the transmit Descriptors*/
	do {
#ifdef ENH_DESC_8W
		desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2,&ext_status,&time_stamp_high,&time_stamp_low);
		synopGMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);
#else
		desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
#endif
		//desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr, &length, &data1);
		if(desc_index >= 0 && data1 != 0){
			TR("Finished Transmit at Tx Descriptor %d for skb 0x%08x and buffer = %08x whose status is %08x \n", desc_index,data1,dma_addr1,status);
#ifdef	IPC_OFFLOAD
			if(synopGMAC_is_tx_ipv4header_checksum_error(gmacdev, status)){
				TR("Harware Failed to Insert IPV4 Header Checksum\n");
			}
			if(synopGMAC_is_tx_payload_checksum_error(gmacdev, status)){
				TR("Harware Failed to Insert Payload Checksum\n");
			}
#endif

			if(channel_index == 2){
				gmacavb->Ch2FramecountTx++;
			}
			else if(channel_index == 1){
				gmacavb->Ch1FramecountTx++;
			}
			else if(channel_index == 0){
				gmacavb->Ch0FramecountTx++;
			}

			if(synopGMAC_is_desc_valid(status)){
				adapter->synopGMACNetStats.tx_bytes += length1;
				adapter->synopGMACNetStats.tx_packets++;
			}
			else {
				TR("Error in Status %08x\n",status);
				adapter->synopGMACNetStats.tx_errors++;
				adapter->synopGMACNetStats.tx_aborted_errors += synopGMAC_is_tx_aborted(status);
				adapter->synopGMACNetStats.tx_carrier_errors += synopGMAC_is_tx_carrier_error(status);
			}

			// Create the frame and assign it to the respective descriptor
			if(global_avb_debug_enable){ // Only for Debugging
				global_avb_tx_count_debug[channel_index]--;
				if(global_avb_tx_count_debug[channel_index] == 0)
					synopGMACAvbDisSlotInterrupt(gmacdev);
				synopGMAC_disable_dma_tx(gmacdev);
			}

			desc_status = synopGMAC_set_tx_qptr(gmacdev,dma_addr1,length1, data1,0,0,0,0);//offload_needed is set to zero
			if(desc_status < 0){
				TR("No More free Descriptors??\n");
			}


		}	adapter->synopGMACNetStats.collisions += synopGMAC_get_tx_collision_count(status);
	} while(desc_index >= 0);
	//	netif_wake_queue(netdev); //For AVB we dont use networking stack
}

/**
 * Interrupt service routing.
 * This is the function registered as ISR for device interrupts.
 * @param[in] interrupt number.
 * @param[in] void pointer to device unique structure (Required for shared interrupts in Linux).
 * @param[in] pointer to pt_regs (not used).
 * \return Returns IRQ_NONE if not device interrupts IRQ_HANDLED for device interrupts.
 * \note This function runs in interrupt context
 *
 */
irqreturn_t synopGMAC_intr_handler_avb(s32 intr_num, void * dev_id)//, struct pt_regs *regs)
{
	/*Kernels passes the netdev structure in the dev_id. So grab it*/
	struct net_device *netdev;
	synopGMACPciNetworkAdapter *adapter;

	synopGMACavbStruct         *gmacavb;

	synopGMACdevice * gmacdev;       // For Place Holder
	synopGMACdevice * gmacdev_ch[3]; // [0] for Best Effor Ethernet, [1] For DMA CH 1, [2] For DMA CH 2


	struct pci_dev *pcidev;

	u32 dma_status_reg;     // For Place Holder
	u32 dma_status_reg_ch[3];  // [0]For Best Effort Ethernet, [1] for CH1, [2] for CH[2]
	//        u32 temp_avg_bits = 0;
	u32 interrupt;
	//	s32 status;
	//u32 dma_addr;
	u32 dma_ch_index ;

	netdev  = (struct net_device *) dev_id;
	if(netdev == NULL){
		TR("Unknown Device\n");
		return -1;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter  = netdev->priv;
#else
	adapter  = netdev_priv(netdev);
#endif
	if(adapter == NULL){
		TR("Adapter Structure Missing\n");
		return -1;
	}

	gmacavb       = adapter->synopGMACavb;

	gmacdev_ch[0] = adapter->synopGMACdev;                         // for Best Effort Ethernet
	if(gmacdev_ch[0] == NULL){
		TR("GMAC device structure Missing for CH0\n");
		return -1;
	}
	gmacdev_ch[1] = adapter->synopGMACdev_ch1;                        // for Prioritized DMA Channel 1
	if(gmacdev_ch[1] == NULL){
		TR("GMAC device structure Missing for CH1\n");
		return -1;
	}
	gmacdev_ch[2] = adapter->synopGMACdev_ch2;                        // for Prioritized DMA Channel 2
	if(gmacdev_ch[2] == NULL){
		TR("GMAC device structure Missing for CH2\n");
		return -1;
	}

	pcidev  = (struct pci_dev *)adapter->synopGMACpcidev;

	/*Read the Dma interrupt status to know whether the interrupt got generated by our device or not*/
	dma_status_reg_ch[0] = synopGMACReadReg((u32 *)gmacdev_ch[0]->DmaBase, DmaStatus);
	dma_status_reg_ch[1] = synopGMACReadReg((u32 *)gmacdev_ch[1]->DmaBase, DmaStatus);
	dma_status_reg_ch[2] = synopGMACReadReg((u32 *)gmacdev_ch[2]->DmaBase, DmaStatus);

	if((dma_status_reg_ch[0] == 0) && (dma_status_reg_ch[1]== 0) && (dma_status_reg_ch[2] == 0)) // If all the three Dma_status registers are zero,
		// it is not our interrupt
		return IRQ_NONE;


	// Handle the interrupts one after the other starting with Channel 0, the reason is it contains the Mac Interrupts
	synopGMAC_disable_interrupt_all(gmacdev_ch[0]);
	synopGMAC_disable_interrupt_all(gmacdev_ch[1]);
	synopGMAC_disable_interrupt_all(gmacdev_ch[2]);

	gmacdev = gmacdev_ch[0];
	dma_status_reg = dma_status_reg_ch[0];

	//Handle the GMAC core interrupts first
	if(dma_status_reg & GmacPmtIntr){
		TR("%s:: Interrupt due to PMT module\n",__FUNCTION__);
		synopGMAC_linux_powerup_mac(gmacdev);
	}

	if(dma_status_reg & GmacMmcIntr){
		TR("%s:: Interrupt due to MMC module\n",__FUNCTION__);
		TR("%s:: synopGMAC_rx_int_status = %08x\n",__FUNCTION__,synopGMAC_read_mmc_rx_int_status(gmacdev));
	}

	if(dma_status_reg & GmacLineIntfIntr){
		TR("%s:: Interrupt due to GMAC LINE module\n",__FUNCTION__);
	}

	for(dma_ch_index = 0; dma_ch_index < 3; dma_ch_index++){
		dma_status_reg = dma_status_reg_ch[dma_ch_index];
		gmacdev        = gmacdev_ch[dma_ch_index];

		if(dma_status_reg != 0){   //Handle Channel 0 Interrupts
			TR("%s:Dma Status Reg for ch[%01d]: 0x%08x\n",__FUNCTION__,dma_ch_index, dma_status_reg);

			/*Now lets handle the DMA interrupts*/
			interrupt = synopGMAC_get_interrupt_type(gmacdev);
			TR("%s:Interrupts to be handled: 0x%08x\n",__FUNCTION__,interrupt);

			if(interrupt & synopGMADmaSlotCounter){
				if(dma_ch_index == 2){
					gmacavb->Ch2AvgBitsNoOfInterrupts++;
					gmacavb->Ch2AvgBitsPerSlot = synopGMACReadReg ((u32 *)gmacdev->DmaBase,DmaChannelAvSts) & ChannelAvgBitsPerSlotMsk ;
					//					printk(KERN_CRIT "CH2: %08x\n",gmacavb->Ch2AvgBitsPerSlot);
					if( ((gmacavb->Ch2AvgBitsPerSlotAccL & 0x80000000) != 0) &&
							(((gmacavb->Ch2AvgBitsPerSlotAccL + gmacavb->Ch2AvgBitsPerSlot) & 0x80000000) == 0))
						gmacavb->Ch2AvgBitsPerSlotAccH++;

					gmacavb->Ch2AvgBitsPerSlotAccL = gmacavb->Ch2AvgBitsPerSlotAccL + gmacavb->Ch2AvgBitsPerSlot;

				}
				if(dma_ch_index == 1){
					gmacavb->Ch1AvgBitsNoOfInterrupts++;
					gmacavb->Ch1AvgBitsPerSlot = synopGMACReadReg ((u32 *)gmacdev->DmaBase,DmaChannelAvSts) & ChannelAvgBitsPerSlotMsk ;
					//				printk(KERN_CRIT "CH1: %08x\n",gmacavb->Ch1AvgBitsPerSlot);
					if( ((gmacavb->Ch1AvgBitsPerSlotAccL & 0x80000000) != 0) &&
							(((gmacavb->Ch1AvgBitsPerSlotAccL + gmacavb->Ch1AvgBitsPerSlot) & 0x80000000) == 0))
						gmacavb->Ch1AvgBitsPerSlotAccH++;

					gmacavb->Ch1AvgBitsPerSlotAccL = gmacavb->Ch1AvgBitsPerSlotAccL + gmacavb->Ch1AvgBitsPerSlot;
				}
			}

			if(interrupt & synopGMACDmaError){
				TR("%s::Fatal Bus Error Inetrrupt Seen\n",__FUNCTION__);
				synopGMAC_disable_dma_tx(gmacdev);
				synopGMAC_disable_dma_rx(gmacdev);

				synopGMAC_take_desc_ownership_tx(gmacdev);
				synopGMAC_take_desc_ownership_rx(gmacdev);

				synopGMAC_init_tx_rx_desc_queue(gmacdev);

				synopGMAC_reset(gmacdev);//reset the DMA engine and the GMAC ip

				synopGMAC_set_mac_addr(gmacdev,GmacAddr0High,GmacAddr0Low, synopGMAC_mac_addr);
				synopGMAC_dma_bus_mode_init(gmacdev,DmaFixedBurstEnable| DmaBurstLength8 | DmaDescriptorSkip2 );
				synopGMAC_dma_control_init(gmacdev,DmaStoreAndForward);
				synopGMAC_init_rx_desc_base(gmacdev);
				synopGMAC_init_tx_desc_base(gmacdev);
				synopGMAC_mac_init(gmacdev);
				synopGMAC_enable_dma_rx(gmacdev);
				synopGMAC_enable_dma_tx(gmacdev);
			}

			if(interrupt & synopGMACDmaRxNormal){ //Handle Receive Interrupt
				TR("%s:: Rx Normal \n", __FUNCTION__);
				synop_handle_received_data_avb(netdev,dma_ch_index);
			}

			if(interrupt & synopGMACDmaRxAbnormal){ //Handle Receive Abnormal Interrupt
				TR("%s::Abnormal Rx Interrupt Seen\n",__FUNCTION__);
#if 1
				if(GMAC_Power_down == 0){	// If Mac is not in powerdown
					adapter->synopGMACNetStats.rx_over_errors++;
					/*Now Descriptors have been created in synop_handle_received_data().
					 * Just issue a poll demand to resume DMA operation*/
					synopGMAC_resume_dma_rx(gmacdev);//To handle GBPS with 12 descriptors
				}
#endif
			}

			if(interrupt & synopGMACDmaRxStopped){
				TR("%s::Receiver stopped seeing Rx interrupts\n",__FUNCTION__); //Receiver gone in to stopped state
			}

			if(interrupt & synopGMACDmaTxNormal){
				//xmit function has done its job
				TR("%s::Finished Normal Transmission \n",__FUNCTION__);
				synop_handle_transmit_over_avb(netdev,dma_ch_index);//Do whatever you want after the transmission is over
			}

			if(interrupt & synopGMACDmaTxAbnormal){
				TR("%s::Abnormal Tx Interrupt Seen\n",__FUNCTION__);
#if 1
				if(GMAC_Power_down == 0){	// If Mac is not in powerdown
					synop_handle_transmit_over_avb(netdev,dma_ch_index);
				}
#endif
			}

			if(interrupt & synopGMACDmaTxStopped){
				TR("%s::Transmitter stopped sending the packets\n",__FUNCTION__);
			}
		}// Per Channel interrupt handling is done

	}
	/* Enable the interrrupt before returning from ISR*/
	synopGMAC_enable_interrupt(gmacdev_ch[0],DmaIntEnable);
	synopGMAC_enable_interrupt(gmacdev_ch[1],DmaIntEnable);
	synopGMAC_enable_interrupt(gmacdev_ch[2],DmaIntEnable);

	return IRQ_HANDLED;
}

#endif

/**
 * Interrupt service routing.
 * This is the function registered as ISR for device interrupts.
 * @param[in] interrupt number.
 * @param[in] void pointer to device unique structure (Required for shared interrupts in Linux).
 * @param[in] pointer to pt_regs (not used).
 * \return Returns IRQ_NONE if not device interrupts IRQ_HANDLED for device interrupts.
 * \note This function runs in interrupt context
 *
 */
irqreturn_t synopGMAC_intr_handler(s32 intr_num, void * dev_id)//, struct pt_regs *regs)
{
	/*Kernels passes the netdev structure in the dev_id. So grab it*/
	struct net_device *netdev;
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;
	struct platform_device *pcidev;
	u32 interrupt,dma_status_reg;
	s32 status;
	u32 dma_addr;


	netdev  = (struct net_device *) dev_id;
	if(netdev == NULL){
		TR("Unknown Device\n");
		return -1;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter  = netdev->priv;
#else
	adapter  = netdev_priv(netdev);
#endif
	if(adapter == NULL){
		TR("Adapter Structure Missing\n");
		return -1;
	}

	gmacdev = adapter->synopGMACdev;
	if(gmacdev == NULL){
		TR("GMAC device structure Missing\n");
		return -1;
	}

	pcidev  = (struct platform_device *)adapter->synopGMACpcidev;

	/*Read the Dma interrupt status to know whether the interrupt got generated by our device or not*/
	dma_status_reg = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaStatus);

	if(dma_status_reg == 0)
		return IRQ_NONE;

	synopGMAC_disable_interrupt_all(gmacdev);

	TR("%s:Dma Status Reg: 0x%08x\n",__FUNCTION__,dma_status_reg);

	if(dma_status_reg & GmacPmtIntr){
		TR("%s:: Interrupt due to PMT module\n",__FUNCTION__);
		synopGMAC_linux_powerup_mac(gmacdev);
	}

	if(dma_status_reg & GmacMmcIntr){
		TR("%s:: Interrupt due to MMC module\n",__FUNCTION__);
		TR("%s:: synopGMAC_rx_int_status = %08x\n",__FUNCTION__,synopGMAC_read_mmc_rx_int_status(gmacdev));
		TR("%s:: synopGMAC_tx_int_status = %08x\n",__FUNCTION__,synopGMAC_read_mmc_tx_int_status(gmacdev));
	}

	if(dma_status_reg & GmacLineIntfIntr){
		TR("%s:: Interrupt due to GMAC LINE module\n",__FUNCTION__);
	}

	/*Now lets handle the DMA interrupts*/
	interrupt = synopGMAC_get_interrupt_type(gmacdev);
	TR("%s:Interrupts to be handled: 0x%08x\n",__FUNCTION__,interrupt);


	if(interrupt & synopGMACDmaError){
		TR("%s::Fatal Bus Error Inetrrupt Seen\n",__FUNCTION__);
		synopGMAC_disable_dma_tx(gmacdev);
		synopGMAC_disable_dma_rx(gmacdev);

		synopGMAC_take_desc_ownership_tx(gmacdev);
		synopGMAC_take_desc_ownership_rx(gmacdev);

		synopGMAC_init_tx_rx_desc_queue(gmacdev);

		synopGMAC_reset(gmacdev);//reset the DMA engine and the GMAC ip

		synopGMAC_set_mac_addr(gmacdev,GmacAddr0High,GmacAddr0Low, synopGMAC_mac_addr);
		synopGMAC_dma_bus_mode_init(gmacdev,DmaFixedBurstEnable| DmaBurstLength8 | DmaDescriptorSkip2 );
		synopGMAC_dma_control_init(gmacdev,DmaStoreAndForward);
		synopGMAC_init_rx_desc_base(gmacdev);
		synopGMAC_init_tx_desc_base(gmacdev);
		synopGMAC_mac_init(gmacdev);
		synopGMAC_enable_dma_rx(gmacdev);
		synopGMAC_enable_dma_tx(gmacdev);

	}

	if(interrupt & synopGMACDmaRxAbnormal){
		TR("%s::Abnormal Rx Interrupt Seen\n",__FUNCTION__);
		/* clear RxAbnormal intrrupt Status. */
		printk("\n synopGMAC Dma RxAbnormal.\n");
		synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaStatus,
		synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaStatus) | synopGMACDmaRxAbnormal);

#if 1

		if(GMAC_Power_down == 0){	// If Mac is not in powerdown
			adapter->synopGMACNetStats.rx_over_errors++;
			/*Now Descriptors have been created in synop_handle_received_data(). Just issue a poll demand to resume DMA operation*/
		}
#endif
	}

	if(interrupt & synopGMACDmaRxStopped){
		TR("%s::Receiver stopped seeing Rx interrupts\n",__FUNCTION__); //Receiver gone in to stopped state
#if 1
		if(GMAC_Power_down == 0){	// If Mac is not in powerdown
			adapter->synopGMACNetStats.rx_over_errors++;
			do{
				struct sk_buff *skb = alloc_skb(netdev->mtu + ETHERNET_HEADER + ETHERNET_CRC, GFP_ATOMIC);
				if(skb == NULL){
					TR("%s::ERROR in skb buffer allocation Better Luck Next time\n",__FUNCTION__);
					break;
					//			return -ESYNOPGMACNOMEM;
				}

				dma_addr = dma_map_single(&pcidev->dev,skb->data,skb_tailroom(skb),DMA_FROM_DEVICE);
				status = synopGMAC_set_rx_qptr(gmacdev,dma_addr, skb_tailroom(skb), (u32)skb,0,0,0);
				TR("%s::Set Rx Descriptor no %08x for skb %08x \n",__FUNCTION__,status,(u32)skb);
				if(status < 0)
					dev_kfree_skb_irq(skb);//changed from dev_free_skb. If problem check this again

			}while(status >= 0);

			synopGMAC_enable_dma_rx(gmacdev);
		}
#endif
	}

	if(interrupt & synopGMACDmaTxNormal){
		//xmit function has done its job
		TR("%s::Finished Normal Transmission \n",__FUNCTION__);
		synop_handle_transmit_over(netdev);//Do whatever you want after the transmission is over


	}

	if(interrupt & synopGMACDmaTxAbnormal){
		TR("%s::Abnormal Tx Interrupt Seen\n",__FUNCTION__);
#if 1
		if(GMAC_Power_down == 0){	// If Mac is not in powerdown
			synop_handle_transmit_over(netdev);
		}
#endif
	}

	if(interrupt & synopGMACDmaTxStopped){
		TR("%s::Transmitter stopped sending the packets\n",__FUNCTION__);
#if 1
		if(GMAC_Power_down == 0){	// If Mac is not in powerdown
			synopGMAC_disable_dma_tx(gmacdev);
			synopGMAC_take_desc_ownership_tx(gmacdev);

			synopGMAC_enable_dma_tx(gmacdev);
			netif_wake_queue(netdev);
			TR("%s::Transmission Resumed\n",__FUNCTION__);
		}
#endif
	}

	if(interrupt & synopGMACDmaRxNormal){
		TR("%s:: Rx Normal \n", __FUNCTION__);
		if (napi) {
			synopGMAC_enable_interrupt(gmacdev,(DmaIntEnable&(~DmaIntRxNormMask)));
			napi_schedule(&synopGMACadapter->napi);
			goto out;
		}
		else
			synop_handle_received_data(netdev, rx_desc);
	}

	/* Enable the interrrupt before returning from ISR*/
	synopGMAC_enable_interrupt(gmacdev,DmaIntEnable);
out:
	return IRQ_HANDLED;
}


/**
 * Function used when the interface is opened for use.
 * We register synopGMAC_linux_open function to linux open(). Basically this
 * function prepares the the device for operation . This function is called whenever ifconfig (in Linux)
 * activates the device (for example "ifconfig eth0 up"). This function registers
 * system resources needed
 *	- Attaches device to device specific structure
 *	- Programs the MDC clock for PHY configuration
 *	- Check and initialize the PHY interface
 *	- ISR registration
 *	- Setup and initialize Tx and Rx descriptors
 *	- Initialize MAC and DMA
 *	- Allocate Memory for RX descriptors (The should be DMAable)
 *	- Initialize one second timer to detect cable plug/unplug
 *	- Configure and Enable Interrupts
 *	- Enable Tx and Rx
 *	- start the Linux network queue interface
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and error status upon failure.
 * \callgraph
 */

s32 synopGMAC_linux_open(struct net_device *netdev)
{
	s32 status = 0;
	s32 retval = 0;
	s32 ijk;
	//s32 reserve_len=2;
	u32 dma_addr;
	struct sk_buff *skb;
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;
#ifdef AVB_SUPPORT
	synopGMACdevice * gmacdev_ch1;
	synopGMACdevice * gmacdev_ch2;
#endif
	struct platform_device *pcidev;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = (synopGMACPciNetworkAdapter *) netdev->priv;
#else
	adapter = (synopGMACPciNetworkAdapter *) netdev_priv(netdev);
#endif
	gmacdev = (synopGMACdevice *)adapter->synopGMACdev;

#ifdef AVB_SUPPORT
	gmacdev_ch1 = (synopGMACdevice *)adapter->synopGMACdev_ch1;
	gmacdev_ch2 = (synopGMACdevice *)adapter->synopGMACdev_ch2;
#endif

	pcidev  = (struct platform_device *)adapter->synopGMACpcidev;

	/*Now platform dependent initialization.*/

	/*Lets reset the IP*/
	TR("adapter= %08x gmacdev = %08x netdev = %08x pcidev= %08x\n",(u32)adapter,(u32)gmacdev,(u32)netdev,(u32)pcidev);
#ifdef AVB_SUPPORT
	TR("gmacdev_ch1 = %08x gmacdev_ch2 = %08x \n",(u32)gmacdev_ch1,(u32)gmacdev_ch2);
#endif

	if (rmii_flag == 1) {
		synopGMAC_setup_0050a1b0(gmacdev);
	}

	synopGMAC_reset(gmacdev);

	/*Attach the device to MAC struct This will configure all the required base addresses
	  such as Mac base, configuration base, phy base address(out of 32 possible phys )*/
	synopGMAC_attach(synopGMACadapter->synopGMACdev,(u32) synopGMACMappedAddr + MACBASE,(u32) synopGMACMappedAddr + DMABASE, synopGMAC_phy_addr);
#ifdef AVB_SUPPORT
	/*Attach the device to MAC struct This will configure all the required base addresses
	  such as Mac base, configuration base, phy base address(out of 32 possible phys )*/
	synopGMAC_attach(synopGMACadapter->synopGMACdev_ch1,(u32) synopGMACMappedAddr + MACBASE,(u32) synopGMACMappedAddr + DMABASE_CH1, synopGMAC_phy_addr);
	synopGMAC_attach(synopGMACadapter->synopGMACdev_ch2,(u32) synopGMACMappedAddr + MACBASE,(u32) synopGMACMappedAddr + DMABASE_CH2, synopGMAC_phy_addr);
#endif

	/*Lets read the version of ip in to device structure*/
	synopGMAC_read_version(gmacdev);

	//synopGMAC_get_mac_addr(synopGMACadapter->synopGMACdev,GmacAddr0High,GmacAddr0Low, netdev->dev_addr);
	/*Now set the broadcast address*/
	for(ijk = 0; ijk <6; ijk++){
		netdev->broadcast[ijk] = 0xff;
	}

	for(ijk = 0; ijk <6; ijk++){
		TR("netdev->dev_addr[%d] = %02x and netdev->broadcast[%d] = %02x\n",ijk,netdev->dev_addr[ijk],ijk,netdev->broadcast[ijk]);
	}
	/*Check for Phy initialization*/
	synopGMAC_set_mdc_clk_div(gmacdev,GmiiCsrClk3);
	gmacdev->ClockDivMdc = synopGMAC_get_mdc_clk_div(gmacdev);
	status = synopGMAC_check_phy_init(gmacdev, rmii_flag);
	if (status) {
		TR0("Error in synopGMAC_check_phy_init.\n");
	}

	mdelay(10);

	if (rmii_flag == 1) {
		synopGMAC_setup_fes(gmacdev);
	}

	/*Request for an shared interrupt. Instead of using netdev->irq lets use pcidev->irq*/
#ifdef AVB_SUPPORT
	if(request_irq (pcidev->resource[1].start, synopGMAC_intr_handler_avb, 0, netdev->name,netdev)){
		TR0("Error in request_irq\n");
		goto error_in_irq;
	}
#else
	if(request_irq (pcidev->resource[1].start, synopGMAC_intr_handler, 0, netdev->name,netdev)){
		TR0("Error in request_irq\n");
		goto error_in_irq;
	}
#endif

	TR("%s owns a shared interrupt on line %d\n",netdev->name, pcidev->resource[1].start);

	/*Set up the tx and rx descriptor queue/ring*/

	synopGMAC_setup_tx_desc_queue(gmacdev,pcidev,tx_desc, RINGMODE);
	//	synopGMAC_setup_tx_desc_queue(gmacdev,pcidev,tx_desc, CHAINMODE);
	synopGMAC_init_tx_desc_base(gmacdev);	//Program the transmit descriptor base address in to DmaTxBase addr
#ifdef AVB_SUPPORT
	synopGMAC_setup_tx_desc_queue(gmacdev_ch1,pcidev,tx_desc, RINGMODE);
	synopGMAC_init_tx_desc_base(gmacdev_ch1);	//Program the transmit descriptor base address in to DmaTxBase addr

	synopGMAC_setup_tx_desc_queue(gmacdev_ch2,pcidev,tx_desc, RINGMODE);
	synopGMAC_init_tx_desc_base(gmacdev_ch2);	//Program the transmit descriptor base address in to DmaTxBase addr
#endif

	synopGMAC_setup_rx_desc_queue(gmacdev,pcidev,rx_desc, RINGMODE);
	//	synopGMAC_setup_rx_desc_queue(gmacdev,pcidev,rx_desc, CHAINMODE);
	synopGMAC_init_rx_desc_base(gmacdev);	//Program the transmit descriptor base address in to DmaTxBase addr

#ifdef ENH_DESC_8W
	synopGMAC_dma_bus_mode_init(gmacdev, DmaBurstLength32 | DmaDescriptorSkip2 | DmaDescriptor8Words ); //pbl32 incr with rxthreshold 128 and Desc is 8 Words
#ifdef AVB_SUPPORT
	synopGMAC_dma_bus_mode_init(gmacdev_ch1, DmaBurstLength32 | DmaDescriptorSkip2 | DmaDescriptor8Words ); //pbl32 incr with rxthreshold 128 and Desc is 8 Words
	synopGMAC_dma_bus_mode_init(gmacdev_ch2, DmaBurstLength32 | DmaDescriptorSkip2 | DmaDescriptor8Words ); //pbl32 incr with rxthreshold 128 and Desc is 8 Words

#endif
#else
	if (synopGMAC_mac_fb)
		synopGMAC_dma_bus_mode_init(gmacdev, DmaFixedBurstEnable | DmaBurstLength32 | DmaDescriptorSkip2);                      //pbl32 incr with rxthreshold 128
	else
		synopGMAC_dma_bus_mode_init(gmacdev, DmaBurstLength32 | DmaDescriptorSkip2);                      //pbl32 incr with rxthreshold 128
#ifdef AVB_SUPPORT
	synopGMAC_dma_bus_mode_init(gmacdev_ch1, DmaBurstLength32 | DmaDescriptorSkip2);                      //pbl32 incr with rxthreshold 128
	synopGMAC_dma_bus_mode_init(gmacdev_ch2, DmaBurstLength32 | DmaDescriptorSkip2);                      //pbl32 incr with rxthreshold 128
#endif
#endif
	synopGMAC_dma_control_init(gmacdev,DmaStoreAndForward |DmaTxSecondFrame|DmaRxThreshCtrl128);
#ifdef AVB_SUPPORT
	synopGMAC_dma_control_init(gmacdev_ch1,DmaStoreAndForward |DmaTxSecondFrame|DmaRxThreshCtrl128);
	synopGMAC_dma_control_init(gmacdev_ch2,DmaStoreAndForward |DmaTxSecondFrame|DmaRxThreshCtrl128);
#endif

	/*Initialize the mac interface*/

	synopGMAC_mac_init(gmacdev);
	synopGMAC_promisc_enable(gmacdev);
#ifdef AVB_SUPPORT
	//No pasue frames for AVB testing
#else
	synopGMAC_pause_control(gmacdev); // This enables the pause control in Full duplex mode of operation
#endif

#ifdef IPC_OFFLOAD
	/*IPC Checksum offloading is enabled for this driver. Should only be used if Full Ip checksumm offload engine is configured in the hardware*/
	synopGMAC_enable_rx_chksum_offload(gmacdev);    // Enable the offload engine in the receive path
	synopGMAC_rx_tcpip_chksum_drop_enable(gmacdev); // This is default configuration, DMA drops the packets if error in encapsulated ethernet payload
	// The FEF bit in DMA control register is configured to 0 indicating DMA to drop the errored frames.
	/*Inform the Linux Networking stack about the hardware capability of checksum offloading*/
	netdev->features = NETIF_F_HW_CSUM;
#endif

	do{
		skb = alloc_skb(netdev->mtu + ETHERNET_HEADER + ETHERNET_CRC, GFP_ATOMIC);
		if(skb == NULL){
			TR0("ERROR in skb buffer allocation\n");
			break;
		}
		//memset(skb->data, 0x55, skb_tailroom(skb));
		dma_addr = dma_map_single(&pcidev->dev,skb->data,skb_tailroom(skb),DMA_FROM_DEVICE);
		status = synopGMAC_set_rx_qptr(gmacdev,dma_addr, skb_tailroom(skb), (u32)skb,0,0,0);
		if(status < 0)
			dev_kfree_skb(skb);

	}while(status >= 0);
#ifdef AVB_SUPPORT
	do {
		skb = alloc_skb(netdev->mtu + ETHERNET_HEADER_AVB + ETHERNET_CRC, GFP_ATOMIC);
		if(skb == NULL){
			TR0("ERROR in skb buffer allocation\n");
			break;
		}
		dma_addr = dma_map_single(&pcidev->dev,skb->data,skb_tailroom(skb), DMA_TO_DEVICE);
		status = synopGMAC_set_tx_qptr(gmacdev,dma_addr,skb_tailroom(skb), (u32)skb,0,0,0,0);//offload_needed is set to zero
		if(status < 0)
			dev_kfree_skb(skb);

	}while(status >= 0);

	do {
		skb = alloc_skb(netdev->mtu + ETHERNET_HEADER_AVB + ETHERNET_CRC, GFP_ATOMIC);
		if(skb == NULL){
			TR0("ERROR in skb buffer allocation\n");
			break;
		}
		dma_addr = dma_map_single(&pcidev->dev,skb->data,skb_tailroom(skb), DMA_TO_DEVICE);
		status = synopGMAC_set_tx_qptr(gmacdev_ch1,dma_addr,skb_tailroom(skb), (u32)skb,0,0,0,0);//offload_needed is set to zero
		if(status < 0)
			dev_kfree_skb(skb);

	}while(status >= 0);

	do {
		skb = alloc_skb(netdev->mtu + ETHERNET_HEADER_AVB + ETHERNET_CRC, GFP_ATOMIC);
		if(skb == NULL){
			TR0("ERROR in skb buffer allocation\n");
			break;
		}
		dma_addr = dma_map_single(&pcidev->dev,skb->data,skb_tailroom(skb),DMA_TO_DEVICE);
		status = synopGMAC_set_tx_qptr(gmacdev_ch2,dma_addr,skb_tailroom(skb),(u32)skb,0,0,0,0);//offload_needed is set to zero
		if(status < 0)
			dev_kfree_skb(skb);

	}while(status >= 0);


#endif

	if (rmii_flag == 1) {
		TR("Setting up the cable unplug timer\n");
		init_timer(&synopGMAC_cable_unplug_timer);
		synopGMAC_cable_unplug_timer.function = (void *)synopGMAC_linux_cable_fes_function;
		synopGMAC_cable_unplug_timer.data = (u32) adapter;
		synopGMAC_cable_unplug_timer.expires = CHECK_TIME + jiffies;
		add_timer(&synopGMAC_cable_unplug_timer);
	}

	synopGMAC_clear_interrupt(gmacdev);
#ifdef AVB_SUPPORT
	synopGMAC_clear_interrupt(gmacdev_ch1);
	synopGMAC_clear_interrupt(gmacdev_ch2);
#endif
	/*
	   Disable the interrupts generated by MMC and IPC counters.
	   If these are not disabled ISR should be modified accordingly to handle these interrupts.
	   */
	synopGMAC_disable_mmc_tx_interrupt(gmacdev, 0xFFFFFFFF);
	synopGMAC_disable_mmc_rx_interrupt(gmacdev, 0xFFFFFFFF);
	synopGMAC_disable_mmc_ipc_rx_interrupt(gmacdev, 0xFFFFFFFF);

	if(napi) napi_enable(&synopGMACadapter->napi);
#ifdef AVB_SUPPORT
	//For AVB we dont start the queue and transmit dma is stopped
	init_timer(&synopGMAC_AVB_test_timer);

	synopGMAC_enable_interrupt(gmacdev,DmaIntEnable);
	synopGMAC_enable_dma_rx(gmacdev);

	synopGMAC_enable_interrupt(gmacdev_ch1,DmaIntEnable);
	synopGMAC_enable_dma_rx(gmacdev_ch1);

	synopGMAC_enable_interrupt(gmacdev_ch2,DmaIntEnable);
	synopGMAC_enable_dma_rx(gmacdev_ch2);
#else
	synopGMAC_enable_interrupt(gmacdev,DmaIntEnable);
	synopGMAC_enable_dma_rx(gmacdev);
	synopGMAC_enable_dma_tx(gmacdev);
	netif_start_queue(netdev);
#endif

	return retval;

error_in_irq:
	return -ESYNOPGMACBUSY;
}

/**
 * Function used when the interface is closed.
 *
 * This function is registered to linux stop() function. This function is
 * called whenever ifconfig (in Linux) closes the device (for example "ifconfig eth0 down").
 * This releases all the system resources allocated during open call.
 * system resources int needs
 *	- Disable the device interrupts
 *	- Stop the receiver and get back all the rx descriptors from the DMA
 *	- Stop the transmitter and get back all the tx descriptors from the DMA
 *	- Stop the Linux network queue interface
 *	- Free the irq (ISR registered is removed from the kernel)
 *	- Release the TX and RX descripor memory
 *	- De-initialize one second timer rgistered for cable plug/unplug tracking
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and error status upon failure.
 * \callgraph
 */

s32 synopGMAC_linux_close(struct net_device *netdev)
{

	//	s32 status = 0;
	//	s32 retval = 0;
	//	u32 dma_addr;
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;

#ifdef AVB_SUPPORT
	synopGMACdevice * gmacdev_ch1;
	synopGMACdevice * gmacdev_ch2;

	synopGMACavbStruct * gmacavb;
#endif
	struct platform_device *pcidev;
	TR0("%s\n",__FUNCTION__);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = (synopGMACPciNetworkAdapter *) netdev->priv;
#else
	adapter = (synopGMACPciNetworkAdapter *) netdev_priv(netdev);
#endif
	if(adapter == NULL){
		TR0("OOPS adapter is null\n");
		return -1;
	}

	gmacdev = (synopGMACdevice *) adapter->synopGMACdev;
	if(gmacdev == NULL){
		TR0("OOPS gmacdev is null\n");
		return -1;
	}

#ifdef AVB_SUPPORT
	gmacdev_ch1 = (synopGMACdevice *)adapter->synopGMACdev_ch1;
	if(gmacdev_ch1 == NULL){
		TR0("OOPS gmacdev_ch1 is null\n");
		return -1;
	}
	gmacdev_ch2 = (synopGMACdevice *)adapter->synopGMACdev_ch2;
	if(gmacdev_ch2 == NULL){
		TR0("OOPS gmacdev_ch2 is null\n");
		return -1;
	}
	gmacavb = (synopGMACavbStruct *)adapter->synopGMACavb;
	if(gmacavb == NULL){
		TR0("OOPS gmacavb is null\n");
		return -1;
	}
#endif

	pcidev = (struct platform_device *)adapter->synopGMACpcidev;
	if(pcidev == NULL){
		TR("OOPS pcidev is null\n");
		return -1;
	}

	/*Disable all the interrupts*/
	synopGMAC_disable_interrupt_all(gmacdev);

#ifdef AVB_SUPPORT
	synopGMAC_disable_interrupt_all(gmacdev_ch1);
	synopGMAC_disable_interrupt_all(gmacdev_ch2);
	synopGMACAvbDisSlotInterrupt(gmacdev_ch1);// Disable the Slot interrupt if enabled for channel 1
	synopGMACAvbDisSlotInterrupt(gmacdev_ch2);// Disable the Slot interrupt if enabled for channel 2

#endif

	if(napi) napi_disable(&synopGMACadapter->napi);

	TR("the synopGMAC interrupt has been disabled\n");

	/*Disable the reception*/
	synopGMAC_disable_dma_rx(gmacdev);
	synopGMAC_take_desc_ownership_rx(gmacdev);
	TR("the synopGMAC Reception has been disabled\n");

	/*Disable the transmission*/
	synopGMAC_disable_dma_tx(gmacdev);
	synopGMAC_take_desc_ownership_tx(gmacdev);

#ifdef AVB_SUPPORT
	synopGMAC_disable_dma_tx(gmacdev_ch1);
	synopGMAC_take_desc_ownership_tx(gmacdev_ch1);

	synopGMAC_disable_dma_tx(gmacdev_ch2);
	synopGMAC_take_desc_ownership_tx(gmacdev_ch2);
#endif

	TR("the synopGMAC Transmission has been disabled\n");
	netif_stop_queue(netdev);
	/*Now free the irq: This will detach the interrupt handler registered*/
	free_irq(pcidev->resource[1].start, netdev);
	TR("the synopGMAC interrupt handler has been removed\n");

	/*Free the Rx Descriptor contents*/
	TR("Now calling synopGMAC_giveup_rx_desc_queue \n");
	synopGMAC_giveup_rx_desc_queue(gmacdev, pcidev, RINGMODE);
	//	synopGMAC_giveup_rx_desc_queue(gmacdev, pcidev, CHAINMODE);
	TR("Now calling synopGMAC_giveup_tx_desc_queue \n");
	synopGMAC_giveup_tx_desc_queue(gmacdev, pcidev, RINGMODE);
	//	synopGMAC_giveup_tx_desc_queue(gmacdev, pcidev, CHAINMODE);

#ifdef AVB_SUPPORT
	TR("Now calling synopGMAC_giveup_tx_desc_queue for Ch1 and Ch2 \n");
	synopGMAC_giveup_tx_desc_queue(gmacdev_ch1, pcidev, RINGMODE);
	synopGMAC_giveup_tx_desc_queue(gmacdev_ch2, pcidev, RINGMODE);
#endif

	TR("Freeing the cable unplug timer\n");
	del_timer(&synopGMAC_cable_unplug_timer);
#ifdef AVB_SUPPORT
	TR("Freeing AVB timer\n");
	del_timer(&synopGMAC_AVB_test_timer);
#endif
#ifdef AVB_SUPPORT
	if(gmacdev_ch1)
		plat_free_memory(gmacdev_ch1);
	if(gmacdev_ch2)
		plat_free_memory(gmacdev_ch2);
	if(gmacavb)
		plat_free_memory(gmacavb);
#endif

	return -ESYNOPGMACNOERR;

	//	TR("%s called \n",__FUNCTION__);
}

/**
 * Function to transmit a given packet on the wire.
 * Whenever Linux Kernel has a packet ready to be transmitted, this function is called.
 * The function prepares a packet and prepares the descriptor and
 * enables/resumes the transmission.
 * @param[in] pointer to sk_buff structure.
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and Error code on failure.
 * \note structure sk_buff is used to hold packet in Linux networking stacks.
 */
s32 synopGMAC_linux_xmit_frames(struct sk_buff *skb, struct net_device *netdev)
{
	s32 status = 0;
#ifdef DEBUG
	s32 i =0;
#endif
	u32 offload_needed = 0;
	u32 dma_addr;
	//u32 flags;
	synopGMACPciNetworkAdapter *adapter;
	synopGMACdevice * gmacdev;
	struct platform_device * pcidev;
	struct netdev_queue *txq;
	TR("%s called \n",__FUNCTION__);
	if(skb == NULL){
		TR0("skb is NULL What happened to Linux Kernel? \n ");
		return -1;
	}

	txq = netdev_get_tx_queue(netdev, skb_get_queue_mapping(skb));
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = (synopGMACPciNetworkAdapter *) netdev->priv;
#else
	adapter = (synopGMACPciNetworkAdapter *) netdev_priv(netdev);
#endif
	if(adapter == NULL)
		return -1;

	gmacdev = (synopGMACdevice *) adapter->synopGMACdev;
	if(gmacdev == NULL)
		return -1;

	pcidev  = (struct platform_device *)adapter->synopGMACpcidev;
	/*Stop the network queue*/
	netif_stop_queue(netdev);
#ifdef AVB_SUPPORT
	return -EBUSY; // When AVB is enabled Transmit path is controlled by the driver itself
#endif


	if(skb->ip_summed == CHECKSUM_UNNECESSARY){
		/*
			In Linux networking, if kernel indicates skb->ip_summed = CHECKSUM_HW, then only checksum offloading should be performed
			Make sure that the OS on which this code runs have proper support to enable offloading.
			*/
		offload_needed = 0x00000001;
#if 0
		printk(KERN_CRIT"skb->ip_summed = CHECKSUM_HW\n");
		printk(KERN_CRIT"skb->h.th=%08x skb->h.th->check=%08x\n",(u32)(skb->h.th),(u32)(skb->h.th->check));
		printk(KERN_CRIT"skb->h.uh=%08x skb->h.uh->check=%08x\n",(u32)(skb->h.uh),(u32)(skb->h.uh->check));
		printk(KERN_CRIT"\n skb->len = %d skb->mac_len = %d skb->data = %08x skb->csum = %08x skb->h.raw = %08x\n",skb->len,skb->mac_len,(u32)(skb->data),skb->csum,(u32)(skb->h.raw));
		printk(KERN_CRIT"DST MAC addr:%02x %02x %02x %02x %02x %02x\n",*(skb->data+0),*(skb->data+1),*(skb->data+2),*(skb->data+3),*(skb->data+4),*(skb->data+5));
		printk(KERN_CRIT"SRC MAC addr:%02x %02x %02x %02x %02x %02x\n",*(skb->data+6),*(skb->data+7),*(skb->data+8),*(skb->data+9),*(skb->data+10),*(skb->data+11));
		printk(KERN_CRIT"Len/type    :%02x %02x\n",*(skb->data+12),*(skb->data+13));
		if(((*(skb->data+14)) & 0xF0) == 0x40){
			printk(KERN_CRIT"IPV4 Header:\n");
			printk(KERN_CRIT"%02x %02x %02x %02x\n",*(skb->data+14),*(skb->data+15),*(skb->data+16),*(skb->data+17));
			printk(KERN_CRIT"%02x %02x %02x %02x\n",*(skb->data+18),*(skb->data+19),*(skb->data+20),*(skb->data+21));
			printk(KERN_CRIT"%02x %02x %02x %02x\n",*(skb->data+22),*(skb->data+23),*(skb->data+24),*(skb->data+25));
			printk(KERN_CRIT"%02x %02x %02x %02x\n",*(skb->data+26),*(skb->data+27),*(skb->data+28),*(skb->data+29));
			printk(KERN_CRIT"%02x %02x %02x %02x\n\n",*(skb->data+30),*(skb->data+31),*(skb->data+32),*(skb->data+33));
			for(counter = 34; counter < skb->len; counter++)
				printk("%02X ",*(skb->data + counter));
		}
		else{
			printk(KERN_CRIT"IPV6 FRAME:\n");
			for(counter = 14; counter < skb->len; counter++)
				printk("%02X ",*(skb->data + counter));
		}
#endif
	}



	/*Now we have skb ready and OS invoked this function. Lets make our DMA know about this*/
	dma_addr = dma_map_single(&pcidev->dev,skb->data,skb->len,DMA_TO_DEVICE);
#ifdef DEBUG
	printk("%s: skb->len: %x\n", __FUNCTION__, skb->len);

	printk("gary:\n");
	for(i=0; i < skb->len; i++) {
		printk("%02x ", skb->data[i]);
		if(((i+1)%8) == 0) {
			printk("\n");
		}
	}
	printk("gary\n");
#endif
	status = synopGMAC_set_tx_qptr(gmacdev, dma_addr, skb->len, (u32)skb,0,0,0,offload_needed);
	if(status < 0){
		TR0("%s No More Free Tx Descriptors\n",__FUNCTION__);
		//		dev_kfree_skb (skb); //with this, system used to freeze.. ??
		return -EBUSY;
	}

	/*Now force the DMA to start transmission*/
	synopGMAC_resume_dma_tx(gmacdev);
	txq->trans_start = jiffies;

	/*Now start the netdev queue*/
	netif_wake_queue(netdev);

	return -ESYNOPGMACNOERR;
}

/**
 * Function provides the network interface statistics.
 * Function is registered to linux get_stats() function. This function is
 * called whenever ifconfig (in Linux) asks for networkig statistics
 * (for example "ifconfig eth0").
 * @param[in] pointer to net_device structure.
 * \return Returns pointer to net_device_stats structure.
 * \callgraph
 */
struct net_device_stats *  synopGMAC_linux_get_stats(struct net_device *netdev)
{
	TR("%s called \n",__FUNCTION__);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	return( &(((synopGMACPciNetworkAdapter *)(netdev->priv))->synopGMACNetStats) );
#else
	return( &(((synopGMACPciNetworkAdapter *)(netdev_priv(netdev)))->synopGMACNetStats) );
#endif
}

/**
 * Function to set multicast and promiscous mode.
 * @param[in] pointer to net_device structure.
 * \return returns void.
 */
void synopGMAC_linux_set_multicast_list(struct net_device *netdev)
{
	TR("%s called \n",__FUNCTION__);
	//todo Function not yet implemented.
	return;
}

/**
 * Function to set ethernet address of the NIC.
 * @param[in] pointer to net_device structure.
 * @param[in] pointer to an address structure.
 * \return Returns 0 on success Errorcode on failure.
 */
s32 synopGMAC_linux_set_mac_address(struct net_device *netdev, void * macaddr)
{

	synopGMACPciNetworkAdapter *adapter = NULL;
	synopGMACdevice * gmacdev = NULL;
	struct sockaddr *addr = macaddr;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = (synopGMACPciNetworkAdapter *) netdev->priv;
#else
	adapter = (synopGMACPciNetworkAdapter *) netdev_priv(netdev);
#endif
	if(adapter == NULL)
		return -1;

	gmacdev = adapter->synopGMACdev;
	if(gmacdev == NULL)
		return -1;

	if(!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	synopGMAC_set_mac_addr(gmacdev,GmacAddr0High,GmacAddr0Low, addr->sa_data);
	synopGMAC_get_mac_addr(synopGMACadapter->synopGMACdev,GmacAddr0High,GmacAddr0Low, netdev->dev_addr);
	TR("%s called \n",__FUNCTION__);
	return 0;
}

/**
 * Function to change the Maximum Transfer Unit.
 * @param[in] pointer to net_device structure.
 * @param[in] New value for maximum frame size.
 * \return Returns 0 on success Errorcode on failure.
 */
s32 synopGMAC_linux_change_mtu(struct net_device *netdev, s32 newmtu)
{
	TR("%s called \n",__FUNCTION__);
	if (newmtu < MIN_ETHERNET_PAYLOAD || newmtu > MAX_ETHERNET_PAYLOAD) {
		printk("%s new mtu value invalid: %d\n",__FUNCTION__, newmtu);
		return -EINVAL;
	}

	netdev->mtu = newmtu;

	return 0;

}

/**
 * IOCTL interface.
 * This function is mainly for debugging purpose.
 * This provides hooks for Register read write, Retrieve descriptor status
 * and Retreiving Device structure information.
 * @param[in] pointer to net_device structure.
 * @param[in] pointer to ifreq structure.
 * @param[in] ioctl command.
 * \return Returns 0 on success Error code on failure.
 */
s32 synopGMAC_linux_do_ioctl(struct net_device *netdev, struct ifreq *ifr, s32 cmd)
{

	s32 retval = 0;
	u16 temp_data = 0;
	synopGMACPciNetworkAdapter *adapter = NULL;
	synopGMACdevice * gmacdev = NULL;
#ifdef AVB_SUPPORT
	synopGMACavbStruct * gmacavb = NULL;
#endif

	struct ifr_data_struct
	{
		u32 unit;
		u32 addr;
		u32 data;
	} *req;


	if(netdev == NULL)
		return -1;
	if(ifr == NULL)
		return -1;

	req = (struct ifr_data_struct *)ifr->ifr_data;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = (synopGMACPciNetworkAdapter *) netdev->priv;
#else
	adapter = (synopGMACPciNetworkAdapter *) netdev_priv(netdev);
#endif
	if(adapter == NULL)
		return -1;

	gmacdev = adapter->synopGMACdev;
	if(gmacdev == NULL)
		return -1;

#ifdef AVB_SUPPORT
	gmacavb = adapter->synopGMACavb;
	if(gmacavb == NULL)
		return -1;
#endif

	//TR("%s :: on device %s req->unit = %08x req->addr = %08x req->data = %08x cmd = %08x \n",__FUNCTION__,netdev->name,req->unit,req->addr,req->data,cmd);

	switch(cmd)
	{
		case IOCTL_READ_REGISTER:		//IOCTL for reading IP registers : Read Registers
			if      (req->unit == 0)	// Read Mac Register
				req->data = synopGMACReadReg((u32 *)gmacdev->MacBase,req->addr);
			else if (req->unit == 1)	// Read DMA Register
				req->data = synopGMACReadReg((u32 *)gmacdev->DmaBase,req->addr);
#ifdef AVB_SUPPORT
			else if (req->unit == 3){	// Read DMA1 Register
				gmacdev = adapter->synopGMACdev_ch1;
				printk(KERN_CRIT "synopGMACdev_ch1 MACBASE = %08x DMABASE = %08x\n",gmacdev->MacBase,gmacdev->DmaBase);
				if(gmacdev == NULL)
					return -1;
				req->data = synopGMACReadReg((u32 *)gmacdev->DmaBase,req->addr);
			}
			else if (req->unit == 4){	// Read DMA2 Register
				gmacdev = adapter->synopGMACdev_ch2;
				printk(KERN_CRIT "synopGMACdev_ch1 MACBASE = %08x DMABASE = %08x\n",gmacdev->MacBase,gmacdev->DmaBase);
				if(gmacdev == NULL)
					return -1;
				req->data = synopGMACReadReg((u32 *)gmacdev->DmaBase,req->addr);
			}
#endif
			else if (req->unit == 2){	// Read Phy Register
				retval = synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase,gmacdev->PhyBase,req->addr,&temp_data);
				req->data = (u32)temp_data;
				if(retval != -ESYNOPGMACNOERR)
					TR("ERROR in Phy read\n");
			}
			break;

		case IOCTL_WRITE_REGISTER:		//IOCTL for reading IP registers : Read Registers
			if      (req->unit == 0)	// Write Mac Register
				synopGMACWriteReg((u32 *)gmacdev->MacBase,req->addr,req->data);
			else if (req->unit == 1)	// Write DMA Register
				synopGMACWriteReg((u32 *)gmacdev->DmaBase,req->addr,req->data);
#ifdef AVB_SUPPORT
			else if (req->unit == 3){	// Read DMA1 Register
				gmacdev = adapter->synopGMACdev_ch1;
				if(gmacdev == NULL)
					return -1;
				synopGMACWriteReg((u32 *)gmacdev->DmaBase,req->addr,req->data);
			}
			else if (req->unit == 4){	// Read DMA2 Register
				gmacdev = adapter->synopGMACdev_ch2;
				if(gmacdev == NULL)
					return -1;
				synopGMACWriteReg((u32 *)gmacdev->DmaBase,req->addr,req->data);
			}
#endif

			else if (req->unit == 2){	// Write Phy Register
				retval = synopGMAC_write_phy_reg((u32 *)gmacdev->MacBase,gmacdev->PhyBase,req->addr,req->data);
				if(retval != -ESYNOPGMACNOERR)
					TR("ERROR in Phy read\n");
			}
			break;

		case IOCTL_READ_IPSTRUCT:		//IOCTL for reading GMAC DEVICE IP private structure
			if     (req->unit == 0){
				gmacdev = adapter->synopGMACdev;
				if(gmacdev == NULL)
					return -1;
				memcpy((synopGMACdevice *)req->addr, gmacdev, sizeof(synopGMACdevice));
			}
#ifdef AVB_SUPPORT
			else if (req->unit == 1){
				gmacdev = adapter->synopGMACdev_ch1;
				if(gmacdev == NULL)
					return -1;
				memcpy((synopGMACdevice *)req->addr, gmacdev, sizeof(synopGMACdevice));
			}
			else if (req->unit == 2){
				gmacdev = adapter->synopGMACdev_ch2;
				if(gmacdev == NULL)
					return -1;
				memcpy((synopGMACdevice *)req->addr, gmacdev, sizeof(synopGMACdevice));
			}
#endif
			else
				return -1;

			//	        memcpy(ifr->ifr_data, gmacdev, sizeof(synopGMACdevice));
			break;
			/* Total Number of interrupts over which AvbBits are accumulated*/
		case IOCTL_READ_RXDESC:			//IOCTL for Reading Rx DMA DESCRIPTOR
			if     (req->unit == 0){
				gmacdev = adapter->synopGMACdev;
				if(gmacdev == NULL)
					return -1;
				memcpy((DmaDesc *)req->addr, gmacdev->RxDesc + ((DmaDesc *) (req->addr))->data1, sizeof(DmaDesc) );
			}
#ifdef AVB_SUPPORT
			else if     (req->unit == 1){
				gmacdev = adapter->synopGMACdev_ch1;
				if(gmacdev == NULL)
					return -1;
				memcpy((DmaDesc *)req->addr, gmacdev->RxDesc + ((DmaDesc *) (req->addr))->data1, sizeof(DmaDesc) );
			}
			else if     (req->unit == 2){
				gmacdev = adapter->synopGMACdev_ch2;
				if(gmacdev == NULL)
					return -1;
				memcpy((DmaDesc *)req->addr, gmacdev->RxDesc + ((DmaDesc *) (req->addr))->data1, sizeof(DmaDesc) );
			}
#endif
			else
				return -1;

			//		memcpy(ifr->ifr_data, gmacdev->RxDesc + ((DmaDesc *) (ifr->ifr_data))->data1, sizeof(DmaDesc) );
			break;

		case IOCTL_READ_TXDESC:			//IOCTL for Reading Tx DMA DESCRIPTOR
			if     (req->unit == 0){
				gmacdev = adapter->synopGMACdev;
				if(gmacdev == NULL)
					return -1;
				memcpy((DmaDesc *)req->addr, gmacdev->TxDesc + ((DmaDesc *) (req->addr))->data1, sizeof(DmaDesc) );
			}
#ifdef AVB_SUPPORT
			else if     (req->unit == 1){
				gmacdev = adapter->synopGMACdev_ch1;
				if(gmacdev == NULL)
					return -1;
				memcpy((DmaDesc *)req->addr, gmacdev->TxDesc + ((DmaDesc *) (req->addr))->data1, sizeof(DmaDesc) );
			}
			else if     (req->unit == 2){
				gmacdev = adapter->synopGMACdev_ch2;
				if(gmacdev == NULL)
					return -1;
				memcpy((DmaDesc *)req->addr, gmacdev->TxDesc + ((DmaDesc *) (req->addr))->data1, sizeof(DmaDesc) );
			}
#endif
			else
				return -1;

			//		memcpy(ifr->ifr_data, gmacdev->TxDesc + ((DmaDesc *) (ifr->ifr_data))->data1, sizeof(DmaDesc) );
			break;
		case IOCTL_POWER_DOWN:
			if	(req->unit == 1){	//power down the mac
				TR("============I will Power down the MAC now =============\n");
				// If it is already in power down don't power down again
				retval = 0;
				if(((synopGMACReadReg((u32 *)gmacdev->MacBase,GmacPmtCtrlStatus)) & GmacPmtPowerDown) != GmacPmtPowerDown){
					synopGMAC_linux_powerdown_mac(gmacdev);
					retval = 0;
				}
			}
			if	(req->unit == 2){	//Disable the power down  and wake up the Mac locally
				TR("============I will Power up the MAC now =============\n");
				//If already powered down then only try to wake up
				retval = -1;
				if(((synopGMACReadReg((u32 *)gmacdev->MacBase,GmacPmtCtrlStatus)) & GmacPmtPowerDown) == GmacPmtPowerDown){
					synopGMAC_power_down_disable(gmacdev);
					synopGMAC_linux_powerup_mac(gmacdev);
					retval = 0;
				}
			}
			break;
#ifdef AVB_SUPPORT
		case IOCTL_AVB_TEST:               //IOCTL for AVB Testing
			if(req->unit == AVB_SET_CONFIG){
				TR("req->unit = %08x\n",req->unit);
				TR("req->addr = %08x\n",req->addr);
				memcpy(gmacavb, (synopGMACavbStruct *)req->addr, sizeof(synopGMACavbStruct));
			}
			else if(req->unit == AVB_CONFIG_HW){
				TR("req->unit = %08x\n",req->unit);
				TR("req->addr = %08x\n",req->addr);
				synopGMAC_prepare_hw_for_avb_test(adapter);
				TR("AVB CONFIGURE THE HARDWARE ........\n");
			}
			else if(req->unit == AVB_RUN_TEST){
				TR("req->unit = %08x\n",req->unit);
				TR("req->addr = %08x\n",req->addr);
				synopGMAC_AVB_setup_timer(netdev);
				TR("AVB TIMER SETUP DONE ........\n");
				synopGMAC_AVB_run_test(netdev,1);
				TR("AVB RUN TEST STARTED ........\n");
			}
			else if(req->unit == AVB_GET_RESULT){
				TR("req->unit = %08x\n",req->unit);
				TR("req->addr = %08x\n",req->addr);
				memcpy((synopGMACavbStruct *)req->addr,gmacavb,sizeof(synopGMACavbStruct)); //copy the avb structure data to user space
			}
			else if(req->unit == AVB_DEBUG_ENABLE){
				TR("req->unit = %08x\n",req->unit);
				TR("req->addr = %08x\n",req->addr);
				global_avb_debug_enable = 1;
			}
			else if(req->unit == AVB_DEBUG_DISABLE){
				TR("req->unit = %08x\n",req->unit);
				TR("req->addr = %08x\n",req->addr);
				global_avb_debug_enable = 0;
			}
			else if(req->unit == AVB_TX_FRAMES){
				TR("req->unit = %08x\n",req->unit);
				TR("req->addr = %08x\n",req->addr);
				global_avb_tx_count_debug[0] = req->data;
				global_avb_tx_count_debug[1] = req->data;
				global_avb_tx_count_debug[2] = req->data;
				synopGMAC_AVB_run_test(netdev,0);
				TR("AVB RUN TEST STARTED (continue)........\n");
			}
			else{
				TR("Wrong Parameter for IOCTL_AVB_TEST\n");
				return -1;
			}
			break;

			/*
			   case IOCTL_AVB_SET_CONFIG:               //IOCTL for AVB Testing
			   memcpy(gmacavb, (synopGMACavbStruct *)ifr->ifr_data,sizeof(synopGMACavbStruct)); //copy the avb structure data from user space
			   break;

			   case IOCTL_AVB_RUN_TEST:
			   TR("AVB RUN TEST ........\n");
			   synopGMACconfigureAvb(adapter);//configure AVMAC,and channel control registers
			   TR("AVB CONFIGURE THE HARDWARE ........\n");
			   synopGMAC_AVB_setup_timer(netdev);
			   TR("AVB TIMER SETUP DONE ........\n");
			   synopGMAC_AVB_run_test(netdev);
			   TR("AVB RUN TEST STARTED ........\n");
			   break;

			   case IOCTL_AVB_GET_RESULT:
			   memcpy((synopGMACavbStruct *)ifr->ifr_data,gmacavb,sizeof(synopGMACavbStruct)); //copy the avb structure data to user space
			   break;
			   */
#endif
		default:
			retval = -1;
	}
	return retval;
}

/**
 * Function to handle a Tx Hang.
 * This is a software hook (Linux) to handle transmitter hang if any.
 * We get transmitter hang in the device interrupt status, and is handled
 * in ISR. This function is here as a place holder.
 * @param[in] pointer to net_device structure
 * \return void.
 */
void synopGMAC_linux_tx_timeout(struct net_device *netdev)
{
	TR("%s called \n",__FUNCTION__);
	//todo Function not yet implemented
	return;
}

static u32 synopGMAC_get_link(struct net_device *netdev)
{
	synopGMACPciNetworkAdapter	*adapter;
	synopGMACdevice		*gmacdev;
	u16 data;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	adapter = netdev->priv;
#else
	adapter = netdev_priv(netdev);
#endif
	gmacdev = adapter->synopGMACdev;

	synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase,gmacdev->PhyBase, PHY_STATUS_REG, &data);

	return (data & (1 << 2)) >> 2;
}

static struct ethtool_ops synopGMAC_ethtool_ops = {
	.get_link		= synopGMAC_get_link
};


/**
 * Function to initialize the Linux network interface.
 *
 * Linux dependent Network interface is setup here. This provides
 * an example to handle the network dependent functionality.
 *
 * return Returns 0 on success and Error code on failure.
 */

#ifdef CONFIG_NET_POLL_CONTROLLER
void synop_poll_ctrl(struct net_device *netdev)
{
	synopGMAC_intr_handler(0, netdev);
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
#else
static const struct net_device_ops synopGMAC_netdev_ops = {
	.ndo_open		= synopGMAC_linux_open,
	.ndo_stop		= synopGMAC_linux_close,
	.ndo_get_stats		= synopGMAC_linux_get_stats,
	.ndo_start_xmit		= synopGMAC_linux_xmit_frames,
	.ndo_tx_timeout		= synopGMAC_linux_tx_timeout,
	.ndo_validate_addr	= NULL,
	.ndo_change_mtu		= synopGMAC_linux_change_mtu,
	.ndo_set_mac_address	= synopGMAC_linux_set_mac_address,
	.ndo_do_ioctl		= synopGMAC_linux_do_ioctl,
	//.ndo_set_multicast_list	= synopGMAC_linux_set_multicast_list,
#ifdef CONFIG_SYNOPGMAC_VLAN
	.ndo_vlan_rx_register	= NULL,
#endif
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= synop_poll_ctrl,
#endif
};
#endif

static int synop_rx_poll(struct napi_struct *napi, int budget)
{
	synopGMACdevice * gmacdev;

	int packets_done = 0;

	gmacdev = synopGMACadapter->synopGMACdev;

	packets_done = synop_handle_received_data(synopGMACadapter->synopGMACnetdev, budget);

	if (packets_done < budget) {
		napi_complete(&synopGMACadapter->napi);
		synopGMAC_enable_interrupt(gmacdev,DmaIntEnable);
	}

	return packets_done;
}

s32 synopGMAC_init_network_interface(void)
{
	s32 err;
	struct net_device *netdev;

	TR("Now Going to Call register_netdev to register the network interface for GMAC core\n");
	/*
	   Lets allocate and set up an ethernet device, it takes the sizeof the private structure. This is mandatory as a 32 byte
	   allignment is required for the private data structure.
	   */
	netdev = alloc_etherdev(sizeof(synopGMACPciNetworkAdapter));
	if(!netdev){
		err = -ESYNOPGMACNOMEM;
		goto err_alloc_etherdev;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	synopGMACadapter = netdev->priv;
#else
	synopGMACadapter = netdev_priv(netdev);
#endif
	synopGMACadapter->synopGMACnetdev = netdev;
	synopGMACadapter->synopGMACpcidev = synopGMACpcidev;
	synopGMACadapter->synopGMACdev    = NULL;
#ifdef AVB_SUPPORT
	synopGMACadapter->synopGMACdev_ch1= NULL;
	synopGMACadapter->synopGMACdev_ch2= NULL;
#endif


	/*Allocate Memory for the the GMACip structure*/
	synopGMACadapter->synopGMACdev = (synopGMACdevice *) plat_alloc_memory(sizeof (synopGMACdevice));
	if(!synopGMACadapter->synopGMACdev){
		TR0("Error in Memory Allocataion \n");
	}

#ifdef AVB_SUPPORT
	synopGMACadapter->synopGMACdev_ch1 = (synopGMACdevice *) plat_alloc_memory(sizeof (synopGMACdevice));
	if(!synopGMACadapter->synopGMACdev){
		TR0("Error in Memory Allocataion for DMA Channel 1\n");
	}
	synopGMACadapter->synopGMACdev_ch2 = (synopGMACdevice *) plat_alloc_memory(sizeof (synopGMACdevice));
	if(!synopGMACadapter->synopGMACdev_ch2){
		TR0("Error in Memory Allocataion for DMA Channel 2\n");
	}
	synopGMACadapter->synopGMACavb = (synopGMACavbStruct *) plat_alloc_memory(sizeof(synopGMACavbStruct));
	if(!synopGMACadapter->synopGMACavb){
		TR0("Error in Memory Allocataion for AVB structure\n");
	}
#endif

	/*Attach the device to MAC struct This will configure all the required base addresses
	  such as Mac base, configuration base, phy base address(out of 32 possible phys )*/
	synopGMAC_attach(synopGMACadapter->synopGMACdev,(u32) synopGMACMappedAddr + MACBASE,(u32) synopGMACMappedAddr + DMABASE, synopGMAC_phy_addr);
	synopGMAC_get_mac_addr(synopGMACadapter->synopGMACdev,GmacAddr0High,GmacAddr0Low, netdev->dev_addr);
#ifdef AVB_SUPPORT
	/*Attach the device to MAC struct This will configure all the required base addresses
	  such as Mac base, configuration base, phy base address(out of 32 possible phys )*/
	synopGMAC_attach(synopGMACadapter->synopGMACdev_ch1,(u32) synopGMACMappedAddr + MACBASE,(u32) synopGMACMappedAddr + DMABASE_CH1,
			synopGMAC_phy_addr);
	synopGMAC_attach(synopGMACadapter->synopGMACdev_ch2,(u32) synopGMACMappedAddr + MACBASE,(u32) synopGMACMappedAddr + DMABASE_CH2,
			synopGMAC_phy_addr);
#endif
	synopGMAC_reset(synopGMACadapter->synopGMACdev);
#if 0
	if(synop_pci_using_dac){
		TR("netdev->features = %08x\n",netdev->features);
		TR("synop_pci_using dac is %08x\n",synop_pci_using_dac);
		netdev->features |= NETIF_F_HIGHDMA;
		TR("netdev->features = %08x\n",netdev->features);
	}
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	netdev->open = &synopGMAC_linux_open;
	netdev->stop = &synopGMAC_linux_close;
	netdev->hard_start_xmit = &synopGMAC_linux_xmit_frames;
	netdev->get_stats = &synopGMAC_linux_get_stats;
	netdev->set_multicast_list = &synopGMAC_linux_set_multicast_list;
	netdev->set_mac_address = &synopGMAC_linux_set_mac_address;
	netdev->change_mtu = &synopGMAC_linux_change_mtu;
	netdev->do_ioctl = &synopGMAC_linux_do_ioctl;
	netdev->tx_timeout = &synopGMAC_linux_tx_timeout;
#ifdef CONFIG_NET_POLL_CONTROLLER
	netdev->poll_controller = &synop_poll_ctrl;
#endif
#else
	netdev -> netdev_ops = &synopGMAC_netdev_ops;
#endif

	printk("synopGMAC=: napi tx_desc rx_desc mac_irq mac_addr rmii_flag mac_fb phy_addr.\n");
	if(napi) {
		netif_napi_add(netdev, &synopGMACadapter->napi, synop_rx_poll, (rx_desc/4)*3+1);
		printk("napi: %d.\n", napi);
	}
	netdev->watchdog_timeo = 5 * HZ;

	netdev->ethtool_ops = &synopGMAC_ethtool_ops;

	check_rtl8201fl_flag(synopGMACadapter->synopGMACdev);
	check_rmii_flag(synopGMACadapter->synopGMACdev, &rmii_flag);

	/*Now start the network interface*/
	TR("Now Registering the netdevice\n");
	if((err = register_netdev(netdev)) != 0) {
		TR0("Error in Registering netdevice\n");
		return err;
	}

	return 0;
err_alloc_etherdev:
	TR0("Problem in alloc_etherdev()..Take Necessary action\n");
	return err;
}


/**
 * Function to initialize the Linux network interface.
 * Linux dependent Network interface is setup here. This provides
 * an example to handle the network dependent functionality.
 * \return Returns 0 on success and Error code on failure.
 */
void synopGMAC_exit_network_interface(void)
{
	void* tmp;

	tmp = (void*) synopGMACadapter->synopGMACdev;

	TR0("Now Calling network_unregister\n");
	unregister_netdev(synopGMACadapter->synopGMACnetdev);

	plat_free_memory(tmp);
}

#ifdef CONFIG_PM
int mac_synopsis_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
	synopGMAC_linux_close(synopGMACadapter->synopGMACnetdev);

	return 0;
}

int mac_synopsis_drv_resume(struct platform_device *pdev)
{
	struct net_device *netdev;

	netdev = synopGMACadapter->synopGMACnetdev;
	synopGMAC_attach(synopGMACadapter->synopGMACdev,(u32) synopGMACMappedAddr + MACBASE,(u32) synopGMACMappedAddr + DMABASE, synopGMAC_phy_addr);
	synopGMAC_get_mac_addr(synopGMACadapter->synopGMACdev,GmacAddr0High,GmacAddr0Low, netdev->dev_addr);
	synopGMAC_reset(synopGMACadapter->synopGMACdev);

	check_rtl8201fl_flag(synopGMACadapter->synopGMACdev);
	check_rmii_flag(synopGMACadapter->synopGMACdev, &rmii_flag);

	synopGMAC_linux_open(synopGMACadapter->synopGMACnetdev);

	return 0;
}
#endif

/*
   module_init(synopGMAC_init_network_interface);
   module_exit(synopGMAC_exit_network_interface);

   MODULE_AUTHOR("Synopsys India");
   MODULE_DESCRIPTION("SYNOPSYS GMAC DRIVER Network INTERFACE");

   EXPORT_SYMBOL(synopGMAC_init_pci_bus_interface);
   */
