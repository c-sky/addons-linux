////////////////////////////////////////////////////////////////////////////////////////////
//
//
//    File: ckmac.h
//
//    Copyright (C):  2003 Searen Network Software Ltd.
//
//    [ This source code is the sole property of Searen Network Software Ltd.  ]
//    [ All rights reserved.  No part of this source code may be reproduced in ]
//    [ any form or by any electronic or mechanical means, including informa-  ]
//    [ tion storage and retrieval system, without the prior written permission]
//    [ of Searen Network Software Ltd.                                        ]
//    [                                                                        ]
//    [   For use by authorized Searen Network Software Ltd. employees only.   ]
//
//    Description:   This class can read, write and watch one serial port.
//		   It sends messages to its owner when something happends on the port
//		   The class creates a thread for reading and writing so the main
//		   program is not blocked.
//
//
//
//  AUTHOR: Ren Yu.
//  DATE: Dec. 21, 2003
//
//
/////////////////////////////////////////////////////////////////////////////////////////////
//

#ifndef __CKMAC_H__
#define __CKMAC_H__

//#include <mach/ck_iomap.h>
//#include <mach/irqs.h>

#define CSKY_MACC_PHYS          0x10006000
#define CSKY_MAC_BUF_PHYS       0x10007400
#define CSKY_MAC_IRQ         26


#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_MMU
#define CKMAC_DESC_BASEADDR  ((volatile unsigned long *)(CSKY_MAC_BUF_PHYS + 0xa0000000))
#define CKMAC_REG_BASEADDR   ((volatile unsigned long *)(CSKY_MACC_PHYS + 0xa0000000))
#else
#define CKMAC_DESC_BASEADDR  ((volatile unsigned long *)CSKY_MAC_BUF_PHYS)
#define CKMAC_REG_BASEADDR   ((volatile unsigned long *)CSKY_MACC_PHYS)
#endif
#define CKMAC_MODE   0x00  // mode register
#define CKMAC_INT    0x01  // interrupt register
#define CKMAC_IMASK  0x02  // interrupt mask register

#define CKMAC_IPGT   0x03  // inter packet gap register
#define CKMAC_NIPGT1 0x04  // non inter packet gap register
#define CKMAC_NIPGT2 0x05  // non inter packet gap register

#define CKMAC_SIZE   0x06  // packet size register
#define CKMAC_COLL   0x07  // collision and retry configure

#define CKMAC_TXBD   0x08  // transmit buffer descriptor
#define CKMAC_CTL    0x09  // control module mode register

#define CKMAC_MIIM   0x0a  // mii mode register
#define CKMAC_MIICMD 0x0b  // mii command register
#define CKMAC_MIIADD 0x0c  // mii address register
#define CKMAC_MIITX  0x0d  // mii transmit data register
#define CKMAC_MIIRX  0x0e  // mii receive data register
#define CKMAC_MIISTS 0x0f  // mii status register

#define CKMAC_ADDR0  0x10  // mac individual address of LSB
#define CKMAC_ADDR1  0x11  // mac individual address of MSB

#define CKETH_HASH0  0x12  // hash0 register
#define CKETH_HASH1  0x13  // hash1 register
#define CKETH_TXCTL  0x14  // transmit control register

#define CKMAC_DESC   0x100 // basic address of descriptor

/////////////////////////////////////////////////////////////////////////////
#define CHIPID     0x01
//RTL8201BL physical device register
#define RTL8201_MODECTRL	0x00	// Mode control register
#define RTL8201_MODESTS		0x01	// Mode status register (read only)
#define RTL8201_PHYID1		0x02	// phy identifier register1 (read only)
#define RTL8201_PHYID2		0x03	// phy identifier register2 (read only)
#define RTL8201_ANAR		0x04	// Auto-negotiation advertisement register
#define RTL8201_ANLPAR		0x05	// Auto-negotiation link partner ability register (read only)
#define RTL8201_ANER		0x06	// Auto-negotiation expansion register (read only)
#define	RTL8201_NSR		0x16	// Nway setup register
#define RTL8201_LBREMR		0x17	// Loopback, Bypass, Receiver error mask register
#define RTL8201_REC		0x18	// Rx_er counter
#define RTL8201_10MNICR		0x19	// 10Mbps Network Interface Configuration Register
#define RTL8201_PHY1_1		0x20	// PHY 1_1 Register
#define RTL8201_PHY1_2		0x21	// PHY 1_2 Register
#define RTL8201_PHY2		0x22	// PHY 2 register
#define RTL8201_TWISTER1	0x23	// Twister_1 register
#define RTL8201_TWISTER_2	0x24	// Twister_2 register
#define RTL8201_TEST		0x25	// test register (Read only)

/////////////////////////////////////////////////////////////////////////////
//mode register
#define CKMAC_MODE_BES        (1 << 18)    //Little/Big Endian Select( =0,Little Endian, ReadOnly)
#define CKMAC_MODE_LPMD_RXEN  (1 << 17)    //Low power Mode Rx Enable
#define CKMAC_MODE_RECSMALL   (1 << 16)    //Receive small packets
#define CKMAC_MODE_PAD        (1 << 15)    //Padding enable
#define CKMAC_MODE_HUGEN      (1 << 14)    //Huge packets enable
#define CKMAC_MODE_CRCEN      (1 << 13)    //CRC enable
#define CKMAC_MODE_DLYCRCEN   (1 << 12)    //Delayed CRC Enable
#define CKMAC_MODE_FULLD      (1 << 10)    //Full Duplex
#define CKMAC_MODE_EXDFREN    (1 << 9)     //Excess Defer enable
#define CKMAC_MODE_NOBCKOF    (1 << 8)     //No Backoff
#define CKMAC_MODE_LOOPBACK   (1 << 7)     //Loop Back
#define CKMAC_MODE_IFG        (1 << 6)     //Interframe gap for incoming frames
#define CKMAC_MODE_PRO        (1 << 5)     //PROMISCUOUS
#define CKMAC_MODE_IAM        (1 << 4)     //Individual address mode
#define CKMAC_MODE_BRO        (1 << 3)     //Broadcast address
#define CKMAC_MODE_NOPRE      (1 << 2)     //No preamble
#define CKMAC_MODE_TXEN       (1 << 1)     //Transmit enable
#define CKMAC_MODE_RXEN       (0x01)       //Receive enable
//Interrupt source register
#define CKMAC_INT_BER         (1 << 7)     //Bus error
#define CKMAC_INT_RXC         (1 << 6)     //Receive control frame
#define CKMAC_INT_TXC         (1 << 5)     //Transmit control frame
#define CKMAC_INT_BUSY        (1 << 4)     //Busy
#define CKMAC_INT_RXE         (1 << 3)     //Receive error
#define CKMAC_INT_RXB         0x04         //Receive frame
#define CKMAC_INT_TXE         0x02         //Transmit error
#define CKMAC_INT_TXB         0x01         //Transmit frame
//Interrupt mask register
#define CKMAC_IMASK_BER         (1 << 7)   //Bus error interrupt mask
#define CKMAC_IMASK_RXC         (1 << 6)   //Receive control frame interrupt mask
#define CKMAC_IMASK_TXC         (1 << 5)   //Transmit control frame interrupt mask
#define CKMAC_IMASK_BUSY        (1 << 4)   //Busy interrupt mask
#define CKMAC_IMASK_RXE         (1 << 3)   //Receive error interrupt mask
#define CKMAC_IMASK_RXB         0x04       //Receive frame interrupt mask
#define CKMAC_IMASK_TXE         0x02       //Transmit error interrupt mask
#define CKMAC_IMASK_TXB         0x01       //Transmit frame interrupt mask
//MII command register
#define CKMAC_MIICMD_WCTRLDATA  0x04       //Write control data
#define CKMAC_MIICMD_RSTAT      0x02       //Read staus
#define CKMAC_MIICMD_SCANSTAT   0x01       //Scan status
//MII address register
//MII status register (read only)
#define CKMAC_MIISTS_NVALID     0x04       // Mii status register invalid ( = 0, valid; = 1, invalid)
#define CKMAC_MIISTS_BUSY       0x02       // Mii busy ( = 0, ready; = 1, busy)
#define CKMAC_MIISTS_LINKFAIL   0x01       // Link fail ( = 0, ok; = 1, fail)
/////////////////////////////////////////////////////////////////////////////
//RTL8201 mode cnotrol register
#define RTL8201_MODECTRL_RESET		(1 << 15)	// reset
#define RTL8201_MODECTRL_LOOPBACK	(1 << 14)	// Loopback
#define RTL8201_MODECTRL_SPD100		(1 << 13)	// Sets the network speed, if auto-negotiation is disable
#define RTL8201_MODECTRL_ANE		(1 << 12)	// Nway Auto-Negotiation Enable
#define RTL8201_MODECTRL_POWERDOWN	(1 << 11)	// Turn down the power of the phy chip
#define RTL8201_MODECTRL_RESTARTAN	(1 << 9)	// re-start auto-negotiation
#define RTL8201_MODECTRL_DUPLEX		(1 << 8)	// set the duplex mode if auto negotiation is disable
//RTL8201 mode status register
#define RTL8201_MODESTS_100BT4		(1 << 15)	// enable 100base-t4 support
#define RTL8201_MODESTS_100BTXFD	(1 << 14)	// enable 100base-tx full duplex support
#define RTL8201_MODESTS_100BTXHD	(1 << 13)	// enable 100base-tx half duplex support
#define	RTL8201_MODESTS_10BTFD		(1 << 12)	// enable 10base-t full duplex support
#define RTL8201_MODESTS_10BTHD		(1 << 11)	// enable 10base-t half duplex support
#define RTL8201_MODESTS_MFPS		(1 << 6)	// the RTL8201BL will accept management frames with preamble supporessed
#define RTL8201_MODESTS_ANC		(1 << 5)	// Auto-negotiation process completed
#define RTL8201_MODESTS_AN		(1 << 4)	// Auto-negotiation link had not been experienced fail state
#define RTL8201_MODESTS_LINKSTS		(1 << 3)	// Valid link established
#define RTL8201_MODESTS_JD		(1 << 2)	// Jabber condition detected
#define RTL8201_MODESTS_EC		(0x01)		// extended register capability
/////////////////////////////////////////////////////////////////////////////
#define TX_RING_SIZE 0x40    // size of total TX BD (64 = half of total buffer decriptors)
#define RX_RING_SIZE 0x40    // size of total RX BD (64 = half of total buffer decriptors)
#define MACDESC_MAX  0x80    //size of total buffer decriptors

#define NEXT_RX(num)       (((num) + 1) % (RX_RING_SIZE))
#define NEXT_TX(num)       (((num) + 1) % (TX_RING_SIZE))

#define TX_BUFFS_AVAIL(bp)                                    \
        (((bp)->tx_old <= (bp)->tx_new) ?                     \
          (bp)->tx_old + (TX_RING_SIZE - 1) - (bp)->tx_new :  \
                            (bp)->tx_old - (bp)->tx_new - 1)


#define SetBit(status, bit)   (status |= (bit))
#define ClrBit(status, bit)   (status &= ~(bit))

#define RX_BUF_ALLOC_SIZE  (ETH_FRAME_LEN + (64 * 3))
#define MAC_TIMEOUT         0xffffff
#define CSKYMAC_IRQBASE     CSKY_MAC_IRQ
#define CSKYMAC_TABLE_SIZE  64
#define RX_LENGTH_THRESHOLD 0x600
/////////////////////////////////////////////////////////////////////////////
// status bit of transmit decriptor
//
#define TXD_LENGTH   (0xffff)
#define TX_RD_BIT    (1 << 15)
#define TX_IRQ_BIT   (1 << 14)
#define TX_WR_BIT    (1 << 13)
#define TX_PAD_BIT   (1 << 12)
#define TX_CRC_BIT   (1 << 11)
#define TX_UR_BIT    (1 << 8)
#define TX_RTY_BIT   (0x0f << 4)
#define TX_RL_BIT    (1 << 3)
#define TX_LC_BIT    (1 << 2)
#define TX_DF_BIT    (1 << 1)
#define TX_CS_BIT    (0x01)

#define TXBD_ERR_BITS   0x10f

typedef struct
{
    unsigned long   status;
    unsigned long   buf;
} TXDESC, *PTXDESC;

/////////////////////////////////////////////////////////////////////////////
// status bit of receive decriptor
//
#define RXD_LENGTH   (0xffff0000)
#define RX_ETY_BIT   (1 << 15)
#define RX_IRQ_BIT   (1 << 14)
#define RX_WR_BIT    (1 << 13)
#define RX_CF_BIT    (1 << 8)
#define RX_MISS_BIT  (1 << 7)
#define RX_OR_BIT    (1 << 6)
#define RX_IS_BIT    (1 << 5)
#define RX_DN_BIT    (1 << 4)
#define RX_TL_BIT    (1 << 3)
#define RX_SF_BIT    (1 << 2)
#define RX_CRC_BIT   (1 << 1)
#define RX_LC_BIT    (0x01)

#define RXBD_ERR_BITS   0xff

typedef struct
{
    unsigned int   status;
    unsigned int   buf;
} RXDESC, *PRXDESC;


/* Timer state engine. */
enum cskymac_timer_state {
        ltrywait = 1,  /* Forcing try of all modes, from fastest to slowest. */
        asleep   = 2,  /* Timer inactive.                                    */
};

struct cskymac {
        spinlock_t               lock;

        struct sk_buff           *rx_skbs[RX_RING_SIZE];
        struct sk_buff           *tx_skbs[TX_RING_SIZE];

        int                      rx_new, tx_new, rx_old, tx_old;

        int                      board_rev;   /* CSKY MAC board revision. */

        unsigned int             cskymac_bursts;
        unsigned int             paddr;
        struct timer_list        cskymac_timer;
        enum cskymac_timer_state timer_state;
        unsigned int             timer_ticks;

        struct net_device_stats  enet_stats;
        struct net_device        *dev;
};



#ifdef __cplusplus
}
#endif

#endif   // endding __CKMAC_H__
