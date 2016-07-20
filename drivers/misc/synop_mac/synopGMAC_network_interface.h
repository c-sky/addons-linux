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
 * Header file for the nework dependent functionality.
 * The function prototype listed here are linux dependent.
 *   
 * \internal
 * ---------------------------REVISION HISTORY-------------------
 * Synopsys 			01/Aug/2007		Created
 */



#ifndef SYNOP_GMAC_NETWORK_INTERFACE_H
#define SYNOP_GMAC_NETWORK_INTERFACE_H 1

#define NET_IF_TIMEOUT (10*HZ)
#define CHECK_TIME (HZ)

s32   synopGMAC_init_network_interface(void);
void  synopGMAC_exit_network_interface(void);

s32 synopGMAC_linux_open(struct net_device *);
s32 synopGMAC_linux_close(struct net_device *);
s32 synopGMAC_linux_xmit_frames(struct sk_buff *, struct net_device *);
struct net_device_stats * synopGMAC_linux_get_stats(struct net_device *);
void synopGMAC_linux_set_multicast_list(struct net_device *);
s32 synopGMAC_linux_set_mac_address(struct net_device *,void *);
s32 synopGMAC_linux_change_mtu(struct net_device *,s32);
s32 synopGMAC_linux_do_ioctl(struct net_device *,struct ifreq *,s32);
void synopGMAC_linux_tx_timeout(struct net_device *);

#endif /* End of file */
