/*******************************************************************************
  Header File to describe the DMA descriptors.
  Enhanced descriptors have been in case of DWMAC1000 Cores.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
*******************************************************************************/

#ifndef STMMAC_DESCS_H
#define STMMAC_DESCS_H

struct dma_desc {
	/* Receive descriptor */
	union {
		struct {
			/* RDES0 */
			u32 reserved1:1;
			u32 crc_error:1;
			u32 dribbling:1;
			u32 mii_error:1;
			u32 receive_watchdog:1;
			u32 frame_type:1;
			u32 collision:1;
			u32 frame_too_long:1;
			u32 last_descriptor:1;
			u32 first_descriptor:1;
			u32 multicast_frame:1;
			u32 run_frame:1;
			u32 length_error:1;
			u32 partial_frame_error:1;
			u32 descriptor_error:1;
			u32 error_summary:1;
			u32 frame_length:14;
			u32 filtering_fail:1;
			u32 own:1;
			/* RDES1 */
			u32 buffer1_size:11;
			u32 buffer2_size:11;
			u32 reserved2:2;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 reserved3:5;
			u32 disable_ic:1;
		} rx;
		struct {
			/* RDES0 */
			u32 payload_csum_error:1;
			u32 crc_error:1;
			u32 dribbling:1;
			u32 error_gmii:1;
			u32 receive_watchdog:1;
			u32 frame_type:1;
			u32 late_collision:1;
			u32 ipc_csum_error:1;
			u32 last_descriptor:1;
			u32 first_descriptor:1;
			u32 vlan_tag:1;
			u32 overflow_error:1;
			u32 length_error:1;
			u32 sa_filter_fail:1;
			u32 descriptor_error:1;
			u32 error_summary:1;
			u32 frame_length:14;
			u32 da_filter_fail:1;
			u32 own:1;
			/* RDES1 */
			u32 buffer1_size:13;
			u32 reserved1:1;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 buffer2_size:13;
			u32 reserved2:2;
			u32 disable_ic:1;
		} erx;		/* -- enhanced -- */

		/* Transmit descriptor */
		struct {
			/* TDES0 */
			u32 deferred:1;
			u32 underflow_error:1;
			u32 excessive_deferral:1;
			u32 collision_count:4;
			u32 heartbeat_fail:1;
			u32 excessive_collisions:1;
			u32 late_collision:1;
			u32 no_carrier:1;
			u32 loss_carrier:1;
			u32 reserved1:3;
			u32 error_summary:1;
			u32 reserved2:15;
			u32 own:1;
			/* TDES1 */
			u32 buffer1_size:11;
			u32 buffer2_size:11;
			u32 reserved3:1;
			u32 disable_padding:1;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 crc_disable:1;
			u32 reserved4:2;
			u32 first_segment:1;
			u32 last_segment:1;
			u32 interrupt:1;
		} tx;
		struct {
			/* TDES0 */
			u32 deferred:1;
			u32 underflow_error:1;
			u32 excessive_deferral:1;
			u32 collision_count:4;
			u32 vlan_frame:1;
			u32 excessive_collisions:1;
			u32 late_collision:1;
			u32 no_carrier:1;
			u32 loss_carrier:1;
			u32 payload_error:1;
			u32 frame_flushed:1;
			u32 jabber_timeout:1;
			u32 error_summary:1;
			u32 ip_header_error:1;
			u32 time_stamp_status:1;
			u32 reserved1:2;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 checksum_insertion:2;
			u32 reserved2:1;
			u32 time_stamp_enable:1;
			u32 disable_padding:1;
			u32 crc_disable:1;
			u32 first_segment:1;
			u32 last_segment:1;
			u32 interrupt:1;
			u32 own:1;
			/* TDES1 */
			u32 buffer1_size:13;
			u32 reserved3:3;
			u32 buffer2_size:13;
			u32 reserved4:3;
		} etx;		/* -- enhanced -- */
	} des01;
	unsigned int des2;
	unsigned int des3;
};

/* Transmit checksum insertion control */
enum tdes_csum_insertion {
	cic_disabled = 0,	/* Checksum Insertion Control */
	cic_only_ip = 1,	/* Only IP header */
	cic_no_pseudoheader = 2,	/* IP header but pseudoheader
					 * is not calculated */
	cic_full = 3,		/* IP header and pseudoheader */
};

enum desc01_erx_e {
	/* RDES0 */
	DESC0_ERX_PAYLOAD_CSUM_ERROR       = 0,  //  :1;
	DESC0_ERX_CRC_ERROR                = 1,  //  :1;
	DESC0_ERX_DRIBBLING                = 2,  //  :1;
	DESC0_ERX_ERROR_GMII               = 3,  //  :1;
	DESC0_ERX_RECEIVE_WATCHDOG         = 4,  //  :1;
	DESC0_ERX_FRAME_TYPE               = 5,  //  :1;
	DESC0_ERX_LATE_COLLISION           = 6,  //  :1;
	DESC0_ERX_IPC_CSUM_ERROR           = 7,  //  :1;
	DESC0_ERX_LAST_DESCRIPTOR          = 8,  //  :1;
	DESC0_ERX_FIRST_DESCRIPTOR         = 9,  //  :1;
	DESC0_ERX_VLAN_TAG                 = 10, //  :1;
	DESC0_ERX_OVERFLOW_ERROR           = 11, //  :1;
	DESC0_ERX_LENGTH_ERROR             = 12, //  :1;
	DESC0_ERX_SA_FILTER_FAIL           = 13, //  :1;
	DESC0_ERX_DESCRIPTOR_ERROR         = 14, //  :1;
	DESC0_ERX_ERROR_SUMMARY            = 15, //  :1;
	DESC0_ERX_FRAME_LENGTH             = 16, //  :14;
	DESC0_ERX_DA_FILTER_FAIL           = 30, //  :1;
	DESC0_ERX_OWN                      = 31, //  :1;
	/* RDES1 */                       
	DESC1_ERX_BUFFER1_SIZE             = 0,  //  :13;
	DESC1_ERX_RESERVED1                = 13, //  :1;
	DESC1_ERX_SECOND_ADDRESS_CHAINED   = 14, //  :1;
	DESC1_ERX_END_RING                 = 15, //  :1;
	DESC1_ERX_BUFFER2_SIZE             = 16, //  :13;
	DESC1_ERX_RESERVED2                = 29, //  :2;
	DESC1_ERX_DISABLE_IC               = 31, //  :1;
};

enum desc01_erx_mask_e {
	/* RDES0 */
	DESC0_ERX_MASK_PAYLOAD_CSUM_ERROR       = ~(1 << 0),       //  :1;
	DESC0_ERX_MASK_CRC_ERROR                = ~(1 << 1),       //  :1;
	DESC0_ERX_MASK_DRIBBLING                = ~(1 << 2),       //  :1;
	DESC0_ERX_MASK_ERROR_GMII               = ~(1 << 3),       //  :1;
	DESC0_ERX_MASK_RECEIVE_WATCHDOG         = ~(1 << 4),       //  :1;
	DESC0_ERX_MASK_FRAME_TYPE               = ~(1 << 5),       //  :1;
	DESC0_ERX_MASK_LATE_COLLISION           = ~(1 << 6),       //  :1;
	DESC0_ERX_MASK_IPC_CSUM_ERROR           = ~(1 << 7),       //  :1;
	DESC0_ERX_MASK_LAST_DESCRIPTOR          = ~(1 << 8),       //  :1;
	DESC0_ERX_MASK_FIRST_DESCRIPTOR         = ~(1 << 9),       //  :1;
	DESC0_ERX_MASK_VLAN_TAG                 = ~(1 << 10),      //  :1;
	DESC0_ERX_MASK_OVERFLOW_ERROR           = ~(1 << 11),      //  :1;
	DESC0_ERX_MASK_LENGTH_ERROR             = ~(1 << 12),      //  :1;
	DESC0_ERX_MASK_SA_FILTER_FAIL           = ~(1 << 13),      //  :1;
	DESC0_ERX_MASK_DESCRIPTOR_ERROR         = ~(1 << 14),      //  :1;
	DESC0_ERX_MASK_ERROR_SUMMARY            = ~(1 << 15),      //  :1;
	DESC0_ERX_MASK_FRAME_LENGTH             = ~(0X3FFF << 16), //  :14;
	DESC0_ERX_MASK_DA_FILTER_FAIL           = ~(1 << 30),      //  :1;
	DESC0_ERX_MASK_OWN                      = ~(1 << 31),      //  :1;
	/* RDES1 */                       
	DESC1_ERX_MASK_BUFFER1_SIZE             = ~(0X1FFF << 0),  //  :13;
	DESC1_ERX_MASK_RESERVED1                = ~(1 << 13),      //  :1;
	DESC1_ERX_MASK_SECOND_ADDRESS_CHAINED   = ~(1 << 14),      //  :1;
	DESC1_ERX_MASK_END_RING                 = ~(1 << 15),      //  :1;
	DESC1_ERX_MASK_BUFFER2_SIZE             = ~(0X1FFF << 16), //  :13;
	DESC1_ERX_MASK_RESERVED2                = ~(0X3 << 29),    //  :2;
	DESC1_ERX_MASK_DISABLE_IC               = ~(1 << 31),      //  :1;
};

enum desc01_etx_e {
	/* TDES0 */
	DESC0_ETX_DEFERRED                = 0,  // :1;
	DESC0_ETX_UNDERFLOW_ERROR         = 1,  // :1;
	DESC0_ETX_EXCESSIVE_DEFERRAL      = 2,  // :1;
	DESC0_ETX_COLLISION_COUNT         = 3,  // :4;
	DESC0_ETX_VLAN_FRAME              = 7,  // :1;
	DESC0_ETX_EXCESSIVE_COLLISIONS    = 8,  // :1;
	DESC0_ETX_LATE_COLLISION          = 9,  // :1;
	DESC0_ETX_NO_CARRIER              = 10, // :1;
	DESC0_ETX_LOSS_CARRIER            = 11, // :1;
	DESC0_ETX_PAYLOAD_ERROR           = 12, // :1;
	DESC0_ETX_FRAME_FLUSHED           = 13, // :1;
	DESC0_ETX_JABBER_TIMEOUT          = 14, // :1;
	DESC0_ETX_ERROR_SUMMARY           = 15, // :1;
	DESC0_ETX_IP_HEADER_ERROR         = 16, // :1;
	DESC0_ETX_TIME_STAMP_STATUS       = 17, // :1;
	DESC0_ETX_RESERVED1               = 18, // :2;
	DESC0_ETX_SECOND_ADDRESS_CHAINED  = 20, // :1;
	DESC0_ETX_END_RING                = 21, // :1;
	DESC0_ETX_CHECKSUM_INSERTION      = 22, // :2;
	DESC0_ETX_RESERVED2               = 24, // :1;
	DESC0_ETX_TIME_STAMP_ENABLE       = 25, // :1;
	DESC0_ETX_DISABLE_PADDING         = 26, // :1;
	DESC0_ETX_CRC_DISABLE             = 27, // :1;
	DESC0_ETX_FIRST_SEGMENT           = 28, // :1;
	DESC0_ETX_LAST_SEGMENT            = 29, // :1;
	DESC0_ETX_INTERRUPT               = 30, // :1;
	DESC0_ETX_OWN                     = 31, // :1;
	/* TDES1 */                      
	DESC1_ETX_BUFFER1_SIZE            = 0,  // :13;
	DESC1_ETX_RESERVED3               = 13, // :3;
	DESC1_ETX_BUFFER2_SIZE            = 16, // :13;
	DESC1_ETX_RESERVED4               = 29, // :3;
};

enum desc01_etx_mask_e {
	/* TDES0 */
	DESC0_ETX_MASK_DEFERRED                = ~(1 << 0),       // :1;
	DESC0_ETX_MASK_UNDERFLOW_ERROR         = ~(1 << 1),       // :1;
	DESC0_ETX_MASK_EXCESSIVE_DEFERRAL      = ~(1 << 2),       // :1;
	DESC0_ETX_MASK_COLLISION_COUNT         = ~(0XF << 3),     // :4;
	DESC0_ETX_MASK_VLAN_FRAME              = ~(1 << 7),       // :1;
	DESC0_ETX_MASK_EXCESSIVE_COLLISIONS    = ~(1 << 8),       // :1;
	DESC0_ETX_MASK_LATE_COLLISION          = ~(1 << 9),       // :1;
	DESC0_ETX_MASK_NO_CARRIER              = ~(1 << 10),      // :1;
	DESC0_ETX_MASK_LOSS_CARRIER            = ~(1 << 11),      // :1;
	DESC0_ETX_MASK_PAYLOAD_ERROR           = ~(1 << 12),      // :1;
	DESC0_ETX_MASK_FRAME_FLUSHED           = ~(1 << 13),      // :1;
	DESC0_ETX_MASK_JABBER_TIMEOUT          = ~(1 << 14),      // :1;
	DESC0_ETX_MASK_ERROR_SUMMARY           = ~(1 << 15),      // :1;
	DESC0_ETX_MASK_IP_HEADER_ERROR         = ~(1 << 16),      // :1;
	DESC0_ETX_MASK_TIME_STAMP_STATUS       = ~(1 << 17),      // :1;
	DESC0_ETX_MASK_RESERVED1               = ~(0X3 << 18),    // :2;
	DESC0_ETX_MASK_SECOND_ADDRESS_CHAINED  = ~(1 << 20),      // :1;
	DESC0_ETX_MASK_END_RING                = ~(1 << 21),      // :1;
	DESC0_ETX_MASK_CHECKSUM_INSERTION      = ~(0X3 << 22),    // :2;
	DESC0_ETX_MASK_RESERVED2               = ~(1 << 24),      // :1;
	DESC0_ETX_MASK_TIME_STAMP_ENABLE       = ~(1 << 25),      // :1;
	DESC0_ETX_MASK_DISABLE_PADDING         = ~(1 << 26),      // :1;
	DESC0_ETX_MASK_CRC_DISABLE             = ~(1 << 27),      // :1;
	DESC0_ETX_MASK_FIRST_SEGMENT           = ~(1 << 28),      // :1;
	DESC0_ETX_MASK_LAST_SEGMENT            = ~(1 << 29),      // :1;
	DESC0_ETX_MASK_INTERRUPT               = ~(1 << 30),      // :1;
	DESC0_ETX_MASK_OWN                     = ~(1 << 31),      // :1;
	/* TDES1 */                      
	DESC1_ETX_MASK_BUFFER1_SIZE            = ~(0X1FFF << 0),  // :13;
	DESC1_ETX_MASK_RESERVED3               = ~(0X7    << 13), // :3;
	DESC1_ETX_MASK_BUFFER2_SIZE            = ~(0X1FFF << 16), // :13;
	DESC1_ETX_MASK_RESERVED4               = ~(0X7    << 29), // :3;
};


#define GMAC_DESC0_SET(desc, offset)  *((volatile u32 *)(desc))     |= (1 << (offset));
#define GMAC_DESC1_SET(desc, offset)  *((volatile u32 *)(desc) + 1) |= (1 << (offset));

#define GMAC_DESC0_CLR(desc, offset)  *((volatile u32 *)(desc))     &= ~(1 << (offset));
#define GMAC_DESC1_CLR(desc, offset)  *((volatile u32 *)(desc) + 1) &= ~(1 << (offset));


#define GMAC_DESC0_WRITE(desc, offset, mask, value)  \
	{ \
	*((volatile u32 *)(desc))      &= (mask); \
	*((volatile u32 *)(desc))      |= ((value) << (offset)); \
	}

#define GMAC_DESC1_WRITE(desc, offset, mask, value)  \
	{ \
	*((volatile u32 *)(desc) + 1 ) &= (mask); \
	*((volatile u32 *)(desc) + 1 ) |= ((value) << (offset)); \
	}

#endif /* STMMAC_DESCS_H */
