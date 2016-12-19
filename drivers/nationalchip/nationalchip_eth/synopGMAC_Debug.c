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

/*
 * This is userland program. This opens the network interface and attaches a data gram socket.
 * The communication to the hardware is accomplished through the Ioctl's sent through the
 * datagram socket.
 */

#include <stdio.h>
#include <math.h>

#define ENH_DESC_8W  // Comment this if intended to use normal descriptor or 4 words
#define AVB_SUPPORT  // Comment this if AV support is not used in Driver

#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <string.h>
#include <linux/netdevice.h>

#define IOCTL_READ_REGISTER  SIOCDEVPRIVATE+1
#define IOCTL_WRITE_REGISTER SIOCDEVPRIVATE+2
#define IOCTL_READ_IPSTRUCT  SIOCDEVPRIVATE+3
#define IOCTL_READ_RXDESC    SIOCDEVPRIVATE+4
#define IOCTL_READ_TXDESC    SIOCDEVPRIVATE+5
#define IOCTL_POWER_DOWN     SIOCDEVPRIVATE+6

//#define IOCTL_AVB_SET_CONFIG SIOCDEVPRIVATE+7
//#define IOCTL_AVB_RUN_TEST   SIOCDEVPRIVATE+8
//#define IOCTL_AVB_GET_RESULT SIOCDEVPRIVATE+9

#define IOCTL_AVB_TEST SIOCDEVPRIVATE+7
#define AVB_SET_CONFIG     0x00000001
#define AVB_CONFIG_HW      0x00000002
#define AVB_RUN_TEST       0x00000003
#define AVB_GET_RESULT     0x00000004
#define AVB_DEBUG_ENABLE   0x00000005
#define AVB_DEBUG_DISABLE  0x00000006
#define AVB_TX_FRAMES      0x00000007


#ifdef AVB_SUPPORT
#define AVTYPE 0x8100
#define MAX_INT_FRAME_SIZE 2048//16384//2048
#endif

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;
typedef int bool;
typedef unsigned long dma_addr_t;

struct ifr_data_struct {
	u32 unit;
	u32 addr;
	u32 data;
};

struct reg
{
	u32    addr;
	char   * name;
};

enum WcsCmd
{
	wcs_write = 1,
	wcs_set,
	wcs_clr
};





/*
   DMA Descriptor Structure
   The structure is common for both receive and transmit descriptors
   */
#ifdef ENH_DESC_8W
typedef struct DmaDescStruct {
	u32   status;         /* Status									*/
	u32   length;         /* Buffer 1  and Buffer 2 length						*/
	u32   buffer1;        /* Network Buffer 1 pointer (Dma-able)							*/
	u32   buffer2;        /* Network Buffer 2 pointer or next descriptor pointer (Dma-able)in chain structure	*/
	/* This data below is used only by driver					*/
	u32   extstatus;      /* Extended status of a Rx Descriptor                                           */
	u32   reserved1;      /* Reserved word                                                                */
	u32   timestamplow;   /* Lower 32 bits of the 64 bit timestamp value                                  */
	u32   timestamphigh;  /* Higher 32 bits of the 64 bit timestamp value                                  */
	u32   data1;          /* This holds virtual address of buffer1, not used by DMA			*/
	u32   data2;          /* This holds virtual address of buffer2, not used by DMA			*/
} DmaDesc;

#else
typedef struct DmaDescStruct {                               /* put it here to allow the caller to know its size */
	u32   status;         /* Status */
	u32   length;         /* Buffer length */
	u32   buffer1;        /* Buffer 1 pointer */
	u32   buffer2;        /* Buffer 2 pointer or next descriptor pointer in chain structure */

	u32   data1;          /* driver data, are not used by DMA engine,                       */
	u32   data2;          /* set DmaDescriptorSkip2 in DmaBusModeInit to skip these words */

} DmaDesc;
#endif

/* synopGMACdevice data */

typedef struct synopGMACDeviceStruct {
	u32 MacBase;          /* base address of MAC registers       */
	u32 DmaBase;          /* base address of DMA registers       */
	u32 PhyBase;          /* PHY device address on MII interface */
	u32 Version;          /* Gmac Revision version               */


	dma_addr_t TxDescDma;
	dma_addr_t RxDescDma;
	DmaDesc *TxDesc;      /* start of TX descriptors ring/chain  */
	DmaDesc *RxDesc;      /* start of RX descriptors ring/chain  */

	u32 BusyTxDesc;		 /* Number of Tx Descriptors owned by DMA at any given time*/
	u32 BusyRxDesc;		 /* Number of Rx Descriptors owned by DMA at any given time*/

	u32  RxDescCount;     /* number of rx descriptors */
	u32  TxDescCount;     /* number of tx descriptors */

	u32  TxBusy;          /* first descriptor owned by DMA engine, moved by DmaTxGet */
	u32  TxNext;          /* next available tx descriptor, moved by DmaTxSet */
	u32  RxBusy;          /* first descripror owned by DMA engine, moved by DmaRxGet */
	u32  RxNext;          /* next available rx descriptor, moved by DmaRxSet */

	DmaDesc * TxBusyDesc;    /* first descriptor owned by DMA engine, moved by DmaTxGet */
	DmaDesc * TxNextDesc;    /* next available tx descriptor, moved by DmaTxSet */
	DmaDesc * RxBusyDesc;    /* first descripror owned by DMA engine, moved by DmaRxGet */
	DmaDesc * RxNextDesc;    /* next available rx descriptor, moved by DmaRxSet */

	/*Phy related stuff*/
	u32 ClockDivMdc;
	/* The status of the link */
	u32 LinkState;
	u32 DuplexMode;
	u32 Speed;
	u32 LoopBackMode;
} synopGMACdevice;

static struct reg mac[] = {
	{ 0x0000, "                  Config" },
	{ 0x0004, "            Frame Filter" },
	{ 0x0008, "             MAC HT High" },
	{ 0x000C, "              MAC HT Low" },
	{ 0x0010, "               GMII Addr" },
	{ 0x0014, "               GMII Data" },
	{ 0x0018, "            Flow Control" },
	{ 0x001C, "                VLAN Tag" },
	{ 0x0020, "            GMAC Version" },
	{ 0x0024, "            GMAC Debug  " },
	{ 0x0040, "          MAC Addr0 High" },
	{ 0x0044, "           MAC Addr0 Low" },
	{ 0x0048, "          MAC Addr1 High" },
	{ 0x004c, "           MAC Addr1 Low" },
	{ 0x0100, "           MMC Ctrl Reg " },
	{ 0x010c, "        MMC Intr Msk(rx)" },
	{ 0x0110, "        MMC Intr Msk(tx)" },
	{ 0x0200, "    MMC Intr Msk(rx ipc)" },
	{ 0x0738, "          AVMAC Ctrl Reg" },
	{ 0, 0 }
};

static struct reg ts[] = {
	{ 0x0700, "     TS Ctrl Reg" },
	{ 0x0704, "Sub Sec Incr Reg" },
	{ 0x0708, "     TS High Reg" },
	{ 0x070C, "     TS Low  Reg" },
	{ 0x0710, "TS High Updt Reg" },
	{ 0x0714, "TS Low  Updt Reg" },
	{ 0x0718, "   TS Addend Reg" },
	{ 0x071C, "Tgt Time (H) Reg" },
	{ 0x0720, "Tgt Time (L) Reg" },
	{ 0x0724, "TS High Word Reg" },
	{ 0x0728, "   TS Status Reg" },
	{ 0x072C, " TS PPS Ctrl Reg" },
	{ 0, 0 }
};

static struct reg dma0[] = {
	{ 0x0000, "[CH0] CSR0   Bus Mode" },
	{ 0x0004, "[CH0] CSR1   TxPlDmnd" },
	{ 0x0008, "[CH0] CSR2   RxPlDmnd" },
	{ 0x000C, "[CH0] CSR3    Rx Base" },
	{ 0x0010, "[CH0] CSR4    Tx Base" },
	{ 0x0014, "[CH0] CSR5     Status" },
	{ 0x0018, "[CH0] CSR6    Control" },
	{ 0x001C, "[CH0] CSR7 Int Enable" },
	{ 0x0020, "[CH0] CSR8 Missed Fr." },
	{ 0x0048, "[CH0] CSR18 Tx Desc  " },
	{ 0x004C, "[CH0] CSR19 Rx Desc  " },
	{ 0x0050, "[CH0] CSR20 Tx Buffer" },
	{ 0x0054, "[CH0] CSR21 Rx Buffer" },
	{ 0x0058, "CSR22 HWCFG          " },
	{ 0, 0 }
};

#ifdef AVB_SUPPORT
static struct reg dma1[]= {
	{ 0x0000, "[CH1] CSR64                    Bus Mode" },
	{ 0x0004, "[CH1] CSR65                    TxPlDmnd" },
	{ 0x0008, "[CH1] CSR66                    RxPlDmnd" },
	{ 0x000C, "[CH1] CSR67                     Rx Base" },
	{ 0x0010, "[CH1] CSR68                     Tx Base" },
	{ 0x0014, "[CH1] CSR69                      Status" },
	{ 0x0018, "[CH1] CSR70                         OMR" },
	{ 0x001C, "[CH1] CSR71                  Int Enable" },
	{ 0x0020, "[CH1] CSR72                  Missed Fr." },
	{ 0x0048, "[CH1] CSR82                     Tx Desc" },
	{ 0x004C, "[CH1] CSR83                     Rx Desc" },
	{ 0x0050, "[CH1] CSR84                   Tx Buffer" },
	{ 0x0054, "[CH1] CSR85                   Rx Buffer" },
	{ 0x0030, "[CH1] CSR76        Slot Fn Ctrl Sts reg" },
	{ 0x0060, "[CH1] CSR88                    Ctrl Reg" },
	{ 0x0064, "[CH1] CSR89                  Status Reg" },
	{ 0x0068, "[CH1] CSR90               IdleSlope Reg" },
	{ 0x006C, "[CH1] CSR91               SendSlope Reg" },
	{ 0x0070, "[CH1] CSR92                HiCredit Reg" },
	{ 0x0074, "[CH1] CSR93                LoCredit Reg" },
	{ 0, 0 }
};

static struct reg dma2[]= {
	{ 0x0000, "[CH2] CSR128                  Bus Mode" },
	{ 0x0004, "[CH2] CSR129                  TxPlDmnd" },
	{ 0x0008, "[CH2] CSR130                  RxPlDmnd" },
	{ 0x000C, "[CH2] CSR131                   Rx Base" },
	{ 0x0010, "[CH2] CSR132                   Tx Base" },
	{ 0x0014, "[CH2] CSR133                    Status" },
	{ 0x0018, "[CH2] CSR134                   Control" },
	{ 0x001C, "[CH2] CSR135                Int Enable" },
	{ 0x0020, "[CH2] CSR136                Missed Fr." },
	{ 0x0048, "[CH2] CSR146                   Tx Desc" },
	{ 0x004C, "[CH2] CSR147                   Rx Desc" },
	{ 0x0050, "[CH2] CSR148                 Tx Buffer" },
	{ 0x0054, "[CH2] CSR149                 Rx Buffer" },
	{ 0x0030, "[CH2] CSR140      Slot Fn Ctrl Sts reg" },
	{ 0x0060, "[CH2] CSR152                  Ctrl Reg" },
	{ 0x0064, "[CH2] CSR153                Status Reg" },
	{ 0x0068, "[CH2] CSR154             IdleSlope Reg" },
	{ 0x006C, "[CH2] CSR155             SendSlope Reg" },
	{ 0x0070, "[CH2] CSR156              HiCredit Reg" },
	{ 0x0074, "[CH2] CSR157              LoCredit Reg" },
	{ 0, 0 }
};
#endif

static struct reg mii[] = {
	{ 0x0000, "Phy Control Reg" },
	{ 0x0001, "Phy Status  Reg" },
	{ 0x0002, "Phy Id (02)    " },
	{ 0x0003, "Phy Id (03)    " },
	{ 0x0004, "Auto-nego   Adv" },
	{ 0x0005, "Auto-nego   Lpa" },
	{ 0x0006, "Auto-nego   Exp" },
	{ 0x0007, "Auto-nego    Np" },
	{ 0x0009, "1000 Ctl    Reg" },
	{ 0x000a, "1000 Sts    Reg" },
	{ 0x0010, "PHY  Ctl    Reg" },
	{ 0x0011, "PHY  Sts    Reg" },
	{ 0, 0 }
};

static struct reg mmc[] = {
	{ 0x0114, "No. Of Bytes  Transmitted (Good/Bad)                            " },
	{ 0x0164, "No. Of Bytes  Transmitted (Good)                                " },
	{ 0x0118, "No. Of Frames Trsnsmitted (Good/Bad)                            " },
	{ 0x0168, "No. Of Frames Trsnsmitted (Good)                                " },

	{ 0x013C, "No. Of Unicast Frames Transmitted (Good/Bad)                    " },
	{ 0x011C, "No. Of Broadcast Frames Transmitted (Good)                      " },
	{ 0x0140, "No. Of Broadcast Frames Transmitted (Good/Bad)                  " },
	{ 0x0120, "No. Of Multicast Frames Transmitted (Good)                      " },
	{ 0x0144, "No. Of Multicast Frames Transmitted (Good/Bad)                  " },

	{ 0x0124, "No. Of Tx frames with Length 64 bytes (Good/Bad)                " },
	{ 0x0128, "No. Of Tx frames with Length <064,128> bytes (Good/Bad)         " },
	{ 0x012C, "No. Of Tx frames with Length <128,256> bytes (Good/Bad)         " },
	{ 0x0130, "No. Of Tx frames with Length <256,512> bytes (Good/Bad)         " },
	{ 0x0134, "No. Of Tx frames with Length <512,1024> bytes (Good/Bad)        " },
	{ 0x0138, "No. Of Tx frames with Length <1024,MaxSize> bytes (Good/Bad)    " },

	{ 0x0148, "No. Of Tx aborted due to Underflow error                        " },
	{ 0x0158, "No. Of Tx aborted due to Late Collision                         " },
	{ 0x015C, "No. Of Tx aborted due to Excessive Collision                    " },
	{ 0x0160, "No. Of Tx aborted due to Carrier Sense error                    " },
	{ 0x016C, "No. Of Tx aborted due to Excessive Defferal error               " },

	{ 0x014C, "No. Of Tx frames after single collision in Half Duplex mode     " },
	{ 0x0150, "No. Of Tx frames after multiple collision in Half Duplex mode   " },
	{ 0x0154, "No. Of Tx frames after a deferral                               " },

	{ 0x0170, "No. Of Pause frames Transmitted   (Good)                        " },
	{ 0x0174, "No. Of VLAM tagged frames Transmitted  (Good)                   " },



	{ 0x0184, "No. Of Bytes  Received    (Good/Bad)                            " },
	{ 0x0188, "No. Of Bytes  Received    (Good)                                " },
	{ 0x0180, "No. Of Frames Received    (Good/Bad)                            " },

	{ 0x01C4, "No. Of Unicast Frames Received (Good)                           " },
	{ 0x018C, "No. Of Broadcast Frames Transmitted (Good)                      " },
	{ 0x0190, "No. Of Multicast Frames Transmitted (Good)                      " },

	{ 0x01AC, "No. Of Rx frames with Length 64 bytes (Good/Bad)                " },
	{ 0x01B0, "No. Of Rx frames with Length <064,128> bytes (Good/Bad)         " },
	{ 0x01B4, "No. Of Rx frames with Length <128,256> bytes (Good/Bad)         " },
	{ 0x01B8, "No. Of Rx frames with Length <256,512> bytes (Good/Bad)         " },
	{ 0x01BC, "No. Of Rx frames with Length <512,1024> bytes (Good/Bad)        " },
	{ 0x01C0, "No. Of Rx frames with Length <1024,MaxSize> bytes (Good/Bad)    " },

	{ 0x01A4, "No. Of Runt Rx frames with frames (<64)                         " },
	{ 0x01A8, "No. Of Jabber Rx frames with frames (>1518)                     " },
	{ 0x0194, "No. Of Rx frames with CRC error                                 " },
	{ 0x0198, "No. Of Rx frames with dribble error (only in 10/100)            " },
	{ 0x019C, "No. Of Rx frames with runt error (<64 and CRC error)            " },
	{ 0x01A0, "No. Of Rx frames with jabber error (>1518 and CRC)              " },
	{ 0x01D4, "No. Of Rx frames missed due to FIFO overflow                    " },
	{ 0x01DC, "No. Of Rx frames received with watchdog timeout error           " },
	{ 0x01C8, "No. Of Rx frames with Length != Frame Size                      " },
	{ 0x01CC, "No. Of Rx frames with Length != Valid Frame Size                " },

	{ 0x0170, "No. Of Pause frames Received (Good)                             " },
	{ 0x0174, "No. Of VLAM tagged frames Received (Good)                       " },


	{ 0x0210, "No. Of IPV4 Packets  Received (Good)                            " },
	{ 0x021C, "No. Of IPV4 Packets with fragmentation Received                 " },
	{ 0x0220, "No. Of IPV4 Packets with UDP checksum disabled                  " },

	{ 0x0214, "No. Of IPV4 Packets with Header Error Received                  " },
	{ 0x0218, "No. Of IPV4 Packets with No Payload Error Received              " },

	{ 0x0224, "No. Of IPV6 Packets  Received (Good)                            " },

	{ 0x0228, "No. Of IPV4 Packets with Header Error Received                  " },
	{ 0x022C, "No. Of IPV6 Packets with No Payload Error Received              " },

	{ 0x0240, "No. Of ICMP Frames Received (Good)                              " },
	{ 0x0244, "No. Of ICMP Frames Received with Error                          " },

	{ 0x0230, "No. Of UDP Frames Received (Good)                               " },
	{ 0x0234, "No. Of UDP Frames Received with Error                           " },

	{ 0x0238, "No. Of TCP Frames Received (Good)                               " },
	{ 0x023C, "No. Of TCP Frames Received with Error                           " },

	{ 0, 0 }
};


static char copyright[] =
"\n"
" ********************************************\n"
" *  synopGMAC debug Utility for Linux       *\n"
" *  Copyright(c) 2007-2007 Synopsys, Inc    *\n"
" ********************************************\n\n"
;

#ifdef AVB_SUPPORT
static char usage[] =
"  Usage:\n"
"    synopGMAC_Debug  <interface> <command> {<parameters>}\n"
"\n"
"  Commands:\n"
"    dump          <unit>  - dump registers,     unit ::= {mac|ts|dma0|dma1|dma2|mii|mmc}\n"
"    read          <unit> <reg>                  unit ::= {mac|ts|dma0|dma1|dma2|mii}\n"
"    write/set/clr <unit> <reg> <value>          unit ::= {mac|ts|dma0|dma1|dma2|mii}\n"
"                 - write/set/clr bits\n"
"    status        <unit>                        unit ::= {dma0|dma1|dma2}\n"
"                 - print Channel Specific data and dump Channel DMA descriptors\n"
"    powerdown  <on/off>\n"
"    avbtest  \n"
"\n"
;
#else
static char usage[] =
"  Usage:\n"
"    synopGMAC_Debug  <interface> <command> {<parameters>}\n"
"\n"
"  Commands:\n"
"    dump          <unit>  - dump registers,     unit ::= {mac|ts|dma0|mii|mmc}\n"
"    read          <unit> <reg>                  unit ::= {mac|ts|dma0|mii}\n"
"    write/set/clr <unit> <reg> <value>          unit ::= {mac|ts|dma0|mii}\n"
"                 - write/set/clr bits\n"
"    status        <unit>                        unit ::= {dma0}\n"
"                 - print Channel Specific data and dump Channel DMA descriptors\n"
"    powerdown  <on/off>\n"
"\n"
;
#endif

#ifdef AVB_SUPPORT
struct avb_user_input {
	u8 duration_of_exp;
	u8 dma_ch_en;		    // enable bits for DMA. bit0=>ch0, bit1=>ch1, bit2=>ch2

	//------------------------------------------------------------------------------------------
	u8 Ch2_arb_weight;	   // Channel2 weights for arbitration
	u32 Ch2_fr_size;             // Frame size for Channel 2

	u8 Ch2_bw_alloc;            // The percentage bandwidth allocation for CH1; (Ch1 BW + Ch2 BW)  < 75%

	u8 Ch2_use_slot_no_check;   // Should Ch2 use slot number checking?
	u8 Ch2_use_adv_slot_int;
	u8 Ch2_slot_count_to_use;   // How many slot used to report avb bits per slot value

	u8 Ch2_tx_rx_prio_policy;   // Should Ch2 use Strict or RR policy
	u8 Ch2_use_tx_high_prio;    // Should Ch2 Tx have High priority over Rx
	u8 Ch2_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx

	u8 Ch2_use_credit_shape;    // Should Ch2 use Credid shape algorithm for traffic shaping?
	u8 Ch2CreditControl;        // Sould Ch2 use Credit Control algorithm for traffic shaping?

	u8 Ch2_tx_desc_slot_no_start;

	u8 Ch2_tx_desc_slot_no_skip;
	//------------------------------------------------------------------------------------------
	u8 Ch1_arb_weight;	   // Channel1 weights for arbitration
	u32 Ch1_fr_size;             // Frame size for Channel 1

	u8 Ch1_bw_alloc;            // The percentage bandwidth allocation for CH1; (Ch1 BW + Ch2 BW)  < 75%

	u8 Ch1_use_slot_no_check;   // Should Ch1 use slot number checking?
	u8 Ch1_use_adv_slot_int;
	u8 Ch1_slot_count_to_use;   // How many slot used to report avb bits per slot value

	u8 Ch1_tx_rx_prio_policy;   // Should Ch1 use Strict or RR policy
	u8 Ch1_use_tx_high_prio;    // Should Ch1 Tx have High priority over Rx
	u8 Ch1_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx

	u8 Ch1_use_credit_shape;    // Should Ch1 use Credid shape algorithm for traffic shaping
	u8 Ch1CreditControl;        // Sould Ch1 use Credit Control algorithm for traffic shaping?

	u8 Ch1_tx_desc_slot_no_start;

	u8 Ch1_tx_desc_slot_no_skip;
	//------------------------------------------------------------------------------------------

	u8 Ch0_arb_weight;	   // Channel0 weights for arbitration
	u32 Ch0_fr_size;             // Frame size for Channel 0

	u8 Ch0_tx_rx_prio_policy;   // Should Ch0 use Strict or RR policy
	u8 Ch0_use_tx_high_prio;    // Should Ch0 Tx have High priority over Rx
	u8 Ch0_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx
};

typedef struct AVBStruct {
	u8 ChSelMask;             /* This gives which DMA channel is enabled and which is disabled
				     Bit0 for Ch0
				     Bit1 for Ch1
				     Bit2 for Ch2
				     */
	u8 DurationOfExp;         /* Duration for which experiment should be conducted in minutes - Default 2 Minutes */

	u8 AvControlCh;           /* channel on which AV control channel must be received (Not used)*/
	u8 PTPCh;                 /* Channel on which PTP packets must be received (Not Used)*/
	u8 PrioTagForAV;          /* Used when more than One channel enabled in Rx path (Not Used)
				     for only CH1 Enabled:
				     Frames sith Priority > Value programmed, frames sent to CH1
				     Frames with priority < Value programmed are sent to CH0
				     For both CH1 and CH2 Enabled:
				     Frames sith Priority > Value programmed, frames sent to CH2
				     Frames with priority < Value programmed are sent to CH1
				     */

	u16 AvType;                /* AV Ethernet Type to be programmed for Core to identify AV type */

	u8 Ch1PrioWts;
	u8 Ch1Bw;
	u32 Ch1_frame_size;
	u8 Ch1_EnableSlotCheck;    /* Enable checking of slot numbers programmed in the Tx Desc*/
	u8 Ch1_AdvSlotInterval;    /* When Set Data fetched for current slot and for next 2 slots in advance
				      When reset data fetched for current slot and in advance for next slot*/

	u8  Ch1CrSh;        /* When set Enables the credit based traffic shaping. Now works with Strict priority style*/
	u8  Ch1SlotCount;          /* Over which transmiteed bits per slot needs to be computed (Only for Credit based shaping) */
	u32 Ch1AvgBitsPerSlot;     /* Average bits per slot reported by core once in Ch1SlotCount * 125 micro seconds */
	u32 Ch1AvgBitsPerSlotAccL;  /* No of Avg Bits per slot on Channel1*/
	u32 Ch1AvgBitsPerSlotAccH;  /* No of Avg Bits per slot on Channel1*/
	u32 Ch1AvgBitsNoOfInterrupts;  /* Total Number of interrupts over which AvbBits are accumulated*/

	u8  Ch1CreditControl;      /* Will be zero (Not used) */

	u8 Ch1_tx_rx_prio_policy;   // Should Ch1 use Strict or RR policy
	u8 Ch1_use_tx_high_prio;    // Should Ch1 Tx have High priority over Rx
	u8 Ch1_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx

	u8 Ch1_tx_desc_slot_no_start;
	u8 Ch1_tx_desc_slot_no_skip;

	u32 Ch1SendSlope;
	u32 Ch1IdleSlope;
	u32 Ch1HiCredit;
	u32 Ch1LoCredit;

	u32 Ch1FramecountTx;         /* No of Frames Transmitted on Channel 1 */
	u32 Ch1FramecountRx;         /* No of Frames Received on Channel 1 */

	u8 Ch2PrioWts;
	u8 Ch2Bw;
	u32 Ch2_frame_size;
	u8 Ch2_EnableSlotCheck;    /* Enable checking of slot numbers programmed in the Tx Desc*/
	u8 Ch2_AdvSlotInterval;    /* When Set Data fetched for current slot and for next 2 slots in advance
				      When reset data fetched for current slot and in advance for next slot*/
	u8  Ch2CrSh;        /* When set Enables the credit based traffic shaping. Now works with Strict priority style*/
	u8  Ch2SlotCount;          /* Over which transmiteed bits per slot needs to be computed (Only for Credit based shaping) */
	u32 Ch2AvgBitsPerSlot;     /* Average bits per slot reported by core once in Ch2SlotCount * 125 micro seconds */
	u32 Ch2AvgBitsPerSlotAccL;  /* No of Avg Bits per slot on Channel2*/
	u32 Ch2AvgBitsPerSlotAccH;  /* No of Avg Bits per slot on Channel2*/
	u32 Ch2AvgBitsNoOfInterrupts; /* Total Number of interrupts over which AvbBits are accumulated*/

	u8  Ch2CreditControl;      /* Will be zero at present*/

	u8 Ch2_tx_rx_prio_policy;   // Should Ch1 use Strict or RR policy
	u8 Ch2_use_tx_high_prio;    // Should Ch1 Tx have High priority over Rx
	u8 Ch2_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx


	u8 Ch2_tx_desc_slot_no_start;
	u8 Ch2_tx_desc_slot_no_skip;

	u32 Ch2SendSlope;
	u32 Ch2IdleSlope;
	u32 Ch2HiCredit;
	u32 Ch2LoCredit;

	u32 Ch2FramecountTx;         /* No of Frames Transmitted on Channel 2 */
	u32 Ch2FramecountRx;         /* No of Frames Received on Channel 2 */

	u8 Ch0PrioWts;
	u8 Ch0_tx_rx_prio_policy;   // Should Ch1 use Strict or RR policy
	u8 Ch0_use_tx_high_prio;    // Should Ch1 Tx have High priority over Rx
	u8 Ch0_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx

	u32 Ch0_frame_size;
	u32 Ch0FramecountTx;         /* No of Frames Transmitted on Channel 0 */
	u32 Ch0FramecountRx;         /* No of Frames Received on Channel 0 */
} synopGMACavbStruct;



void initialize_default_avb(struct avb_user_input * avb_params) {
	avb_params->duration_of_exp = 2;         //Duration of experiment is 2 Seconds by default
	avb_params->dma_ch_en     = 0x7;       //Channel enable Bit0=>Ch0 Bit1=>Ch1 Bit2=>Ch2
	//------------------------------------------------------------------------------------------

	avb_params->Ch2_arb_weight = 1;         //Strict
	avb_params->Ch2_fr_size = 1000;         // Frame size for Channel 2 is 1000

	avb_params->Ch2_bw_alloc = 50;            // The percentage bandwidth allocation for CH1; (Ch1 BW + Ch2 BW)  < 75%

	avb_params->Ch2_use_slot_no_check = 0;   // Should Ch2 use slot number checking?
	avb_params->Ch2_use_adv_slot_int  = 0;
	avb_params->Ch2_slot_count_to_use = 16;   // How many slot used to report avb bits per slot value

	avb_params->Ch2_tx_rx_prio_policy = 0;   // Should Ch2 use Strict or RR policy
	avb_params->Ch2_use_tx_high_prio  = 1;    // Should Ch2 Tx have High priority over Rx
	avb_params->Ch2_tx_rx_prio_ratio  = 1;    // For Round Robin what is the ratio between tx-rx or rx-tx

	avb_params->Ch2_use_credit_shape  = 1;    // Should Ch2 use Credid shape algorithm for traffic shaping

	avb_params->Ch2_tx_desc_slot_no_start = 0;
	avb_params->Ch2_tx_desc_slot_no_skip  = 0;
	//------------------------------------------------------------------------------------------
	avb_params->Ch1_arb_weight = 1;          //Strict
	avb_params->Ch1_fr_size  = 1000;             // Frame size for Channel 1

	avb_params->Ch1_bw_alloc = 25;            // The percentage bandwidth allocation for CH1; (Ch1 BW + Ch2 BW)  < 75%

	avb_params->Ch1_use_slot_no_check = 0;   // Should Ch1 use slot number checking?
	avb_params->Ch1_use_adv_slot_int  = 0;
	avb_params->Ch1_slot_count_to_use = 16;   // How many slot used to report avb bits per slot value

	avb_params->Ch1_tx_rx_prio_policy = 0;   // Should Ch1 use Strict or RR policy
	avb_params->Ch1_use_tx_high_prio  = 1;    // Should Ch1 Tx have High priority over Rx
	avb_params->Ch1_tx_rx_prio_ratio  = 1;    // For Round Robin what is the ratio between tx-rx or rx-tx

	avb_params->Ch1_use_credit_shape  = 1;    // Should Ch1 use Credid shape algorithm for traffic shaping

	avb_params->Ch1_tx_desc_slot_no_start = 0;
	avb_params->Ch1_tx_desc_slot_no_skip  = 0;
	//------------------------------------------------------------------------------------------

	avb_params->Ch0_arb_weight = 1;           //Strict
	avb_params->Ch0_fr_size  = 1000;             // Frame size for Channel 0

	avb_params->Ch0_tx_rx_prio_policy = 0;   // Should Ch0 use Strict or RR policy
	avb_params->Ch0_use_tx_high_prio  = 1;    // Should Ch0 Tx have High priority over Rx
	avb_params->Ch0_tx_rx_prio_ratio  = 1;    // For Round Robin what is the ratio between tx-rx or rx-tx
}
#endif

static int powerdown(char *ifname, int argc, char *argv[] )
{

	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h

	struct ifr_data_struct data;
	struct reg *regs;

	if(argc != 1) return 1;

	if     ( strcmp( argv[0], "on"  ) == 0 ) { data.unit = 1;}
	else if( strcmp( argv[0], "off" ) == 0 ) { data.unit = 2;}
	else return 1;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );

	ifr.ifr_data = (void *) &data;

	retval = ioctl( socket_desc, IOCTL_POWER_DOWN, &ifr );
	if( retval < 0 )
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}

	printf("The GMAC is Successfully powered %s\n", (( data.unit == 1 ) ? "Down":"Up" ) );

close_socket:
	close(socket_desc);
	return retval;
}

static int dump( char *ifname, int argc, char *argv[] )
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h

	struct ifr_data_struct data;
	struct reg *regs;

	if(argc!=1) return 1;
	if     ( strcmp( argv[0], "mac" ) == 0 ) { data.unit = 0; regs = mac;  }
	else if( strcmp( argv[0], "ts"  ) == 0 ) { data.unit = 0; regs = ts;   }
	else if( strcmp( argv[0], "dma0") == 0 ) { data.unit = 1; regs = dma0; }
#ifdef AVB_SUPPORT
	else if( strcmp( argv[0], "dma1") == 0 ) { data.unit = 3; regs = dma1; }
	else if( strcmp( argv[0], "dma2") == 0 ) { data.unit = 4; regs = dma2; }
#endif
	else if( strcmp( argv[0], "mii" ) == 0 ) { data.unit = 2; regs = mii;  }
	else if( strcmp( argv[0], "mmc" ) == 0 ) { data.unit = 0; regs = mmc;  }
	else return 1;

	printf("%s Dump %s Registers: \n", ifname, argv[0]);

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );

	ifr.ifr_data = (void *) &data;

	while( regs->name != 0 )
	{
		data.addr = regs->addr;
		retval = ioctl( socket_desc, IOCTL_READ_REGISTER, &ifr );

		if( retval < 0 )
		{
			printf("IOCTL Error\n");
			break;
		}
		printf( "%s (0x%04x) = 0x%08x\n", regs->name, regs->addr, data.data );

		regs++;
	}
	close(socket_desc);
	return retval;
}

static int read(char *ifname, int argc, char *argv[])
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h

	struct ifr_data_struct data;
	struct reg *regs;

	if(argc != 2) return 1;
	if     ( strcmp( argv[0], "mac"  ) == 0 ) { data.unit = 0; regs = mac;  }
	else if( strcmp( argv[0], "ts"   ) == 0 ) { data.unit = 0; regs = ts;   }
	else if( strcmp( argv[0], "dma0" ) == 0 ) { data.unit = 1; regs = dma0; }
#ifdef AVB_SUPPORT
	else if( strcmp( argv[0], "dma1" ) == 0 ) { data.unit = 3; regs = dma1; }
	else if( strcmp( argv[0], "dma2" ) == 0 ) { data.unit = 4; regs = dma2; }
#endif
	else if( strcmp( argv[0], "mii"  ) == 0 ) { data.unit = 2; regs = mii;  }
	else return 1;

	strcpy( ifr.ifr_name, ifname );

	ifr.ifr_data = (void *) &data;


	data.addr = strtol(argv[1],NULL,16); //convert the register address argument from string to long
	if(strcmp(argv[0],"ts") == 0 )
		data.addr = data.addr + regs[0].addr; // this is required to properly access ts register since
	// ts is also accessed with mac base in driver.
	if(((data.unit == 0) && (data.addr > 0x00000FFC)) || ((data.addr & 0x00000003) != 0))
		return 1;
	if(((data.unit == 1) && (data.addr > 0x00000058)) || ((data.addr & 0x00000003) != 0))
		return 1;
	if((data.unit == 2) && (data.addr > 0x0000001c))
		return 1;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}
	retval = ioctl( socket_desc, IOCTL_READ_REGISTER, &ifr );

	if(retval < 0)
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}
	printf( "[%s](0x%04x) = 0x%08x\n ",argv[0],data.addr, data.data);

close_socket:  close(socket_desc);
	       return retval;
}

static int wcs(int cmd, char * ifname, char * cmdname, int argc,  char *argv[])
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr; //defined in if.h

	struct ifr_data_struct data;
	struct reg *regs;
	u32 data_to_write;

	if (argc != 3) return -1;

	if     (strcmp (argv[0],"mac") == 0){
		data.unit = 0;
		regs = mac;
	}
	else if (strcmp (argv[0],"ts") == 0){
		data.unit = 0;
		regs = ts;
	}
	else if (strcmp (argv[0],"dma0") == 0){
		data.unit = 1;
		regs = dma0;
	}
#ifdef AVB_SUPPORT
	else if (strcmp (argv[0],"dma1") == 0){
		data.unit = 3;
		regs = dma1;
	}
	else if (strcmp (argv[0],"dma2") == 0){
		data.unit = 4;
		regs = dma2;
	}
#endif

	else if (strcmp (argv[0],"mii") == 0){
		data.unit = 2;
		regs = mii;
	}
	else return -1;

	sscanf(argv[1], "%x", &data.addr);
	if (strcmp(argv[0],"ts") == 0)
		data.addr = data.addr + regs[0].addr; // this is required to properly access ts register since
	// ts is also accessed with mac base in driver.

	while(regs->name != 0){
		if(regs->addr == data.addr) break;
		regs++;
	}
	if(regs->name == 0){
		printf("Invalid %s Register   - 0x%02x\n",argv[0],data.addr);
		return -1;
	}
	sscanf(argv[2],"%x", &data_to_write);
	printf("%s %s 0x%08x to %s Register 0x%02x\n",ifname,cmdname,data_to_write,argv[0],data.addr);

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );
	ifr.ifr_data = (void *) &data;

	if(cmd == wcs_set || cmd == wcs_clr){
		retval = ioctl(socket_desc,IOCTL_READ_REGISTER, &ifr);
		if(retval < 0){
			perror("IOCTL Error\n");
		}
	}

	if(retval == 0){
		if(cmd == wcs_set  ) data.data |= data_to_write;
		if(cmd == wcs_clr) data.data &= (~data_to_write);
		if(cmd == wcs_write) data.data  = data_to_write;
	}

	retval = ioctl(socket_desc,IOCTL_WRITE_REGISTER, &ifr);
	if(retval < 0)
		perror("IOCTL Error\n");

	close(socket_desc);
	return retval;

}

static int status(char * ifname, int argc, char *argv[])
{
	int socket_desc;
	int retval = 0;
	int count = 0;
	struct ifreq ifr;  //defined in if.h
	struct ifr_data_struct data;

	DmaDesc    dmadesc;
	synopGMACdevice  gmacdev;

	if(argc!=1) return 1;


	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	if     ( strcmp( argv[0], "dma0" ) == 0 ) { data.unit = 0;}
#ifdef AVB_SUPPORT
	else if( strcmp( argv[0], "dma1" ) == 0 ) { data.unit = 1;}
	else if( strcmp( argv[0], "dma2" ) == 0 ) { data.unit = 2;}
#endif
	else return 1;

	printf("%s Dump Status: \n", ifname);

	strcpy( ifr.ifr_name, ifname );

	data.addr = (u32) &gmacdev;
	ifr.ifr_data = (void *) &data;

	retval = ioctl( socket_desc, IOCTL_READ_IPSTRUCT, &ifr );

	if( retval < 0 )
	{
		printf("IOCTL Error\n");
	}

	else {
		printf("GMAC ip Version                              = 0x%08x\n", gmacdev.Version );
		printf("GMAC   Register Base Address                 = 0x%08x\n", gmacdev.MacBase );
		printf("DMA    Register Base Address                 = 0x%08x\n", gmacdev.DmaBase );
		printf("PHY device Base Address(32 phys possible)    = 0x%08x\n", gmacdev.PhyBase );
		printf("Start of Rx Desc Ring                        = %p\n", gmacdev.RxDesc );
		printf("Start of Tx Desc Ring                        = %p\n", gmacdev.TxDesc );

		printf("Busy Rx Desc Count                           = %d\n", gmacdev.BusyRxDesc );
		printf("Busy Tx Desc Count                           = %d\n", gmacdev.BusyTxDesc );

		printf("Start of Rx Desc Ring(DMA)                   = %p\n", gmacdev.RxDescDma );
		printf("Start of Tx Desc Ring(DMA)                   = %p\n", gmacdev.TxDescDma );
		printf("Number of Rx Descriptors                     = %d\n", gmacdev.RxDescCount );
		printf("Number of Tx Descriptors                     = %d\n", gmacdev.TxDescCount );
		printf("First Rx Descriptor Owned by DMA Engine      = %d\n", gmacdev.RxBusy );
		printf("Next Available Rx Descriptor                 = %d\n", gmacdev.RxNext );
		printf("First Tx Descriptor Owned by DMA Engine      = %d\n", gmacdev.TxBusy );
		printf("Next Available Tx Descriptor                 = %d\n", gmacdev.TxNext );
		printf("MDC clock division number                    = 0x%08x\n", gmacdev.ClockDivMdc);
		printf("Link State information                       = 0x%08x\n", gmacdev.LinkState );
		printf("Duplex Mode of the Interface                 = 0x%08x\n", gmacdev.DuplexMode );
		printf("Speed of the Interface                       = 0x%08x\n", gmacdev.Speed );
		printf("LoopBack status of the Interface             = 0x%08x\n", gmacdev.LoopBackMode);

		printf("\n");
#ifdef ENH_DESC
		printf("RxD Idx:   Status      length      buffer1     buffer2     data1       data2     Ext_status  reserved       TSL         TSH    \n");
		printf("---------------------------------------------------------------------------------------------------------------------------- \n");
		for(count = 0; count < gmacdev.RxDescCount; count++ ){
			dmadesc.data1 = count;
			//		ifr.ifr_data = (void *) &dmadesc;

			if     ( strcmp( argv[0], "ch0" ) == 0 ) { data.unit = 0;}
			else if( strcmp( argv[0], "ch1" ) == 0 ) { data.unit = 1;}
			else if( strcmp( argv[0], "ch2" ) == 0 ) { data.unit = 2;}

			data.addr  = (u32) &dmadesc;
			ifr.ifr_data = (void *)  &data;

			retval = ioctl(socket_desc, IOCTL_READ_RXDESC,&ifr);
			if(retval < 0)
				break;
			printf("RxD  %02x: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
					count,dmadesc.status, dmadesc.length,dmadesc.buffer1,dmadesc.buffer2,
					dmadesc.data1,dmadesc.data2,dmadesc.extstatus,dmadesc.reserved1,dmadesc.timestamplow,dmadesc.timestamphigh);
		}
		if(retval < 0)
			perror("IOCTL Error\n");

		printf("\n");

		printf("TxD Idx:   Status      length      buffer1     buffer2     data1       data2   reserved      reserved      TSL           TSH \n");
		printf("---------------------------------------------------------------------------------------------------------------------------- \n");
		for(count = 0; count < gmacdev.TxDescCount; count++ ){
			dmadesc.data1 = count;

			//		ifr.ifr_data = (void *) &dmadesc;
			if     ( strcmp( argv[0], "ch0" ) == 0 ) { data.unit = 0;}
			else if( strcmp( argv[0], "ch1" ) == 0 ) { data.unit = 1;}
			else if( strcmp( argv[0], "ch2" ) == 0 ) { data.unit = 2;}

			data.addr  = (u32) &dmadesc;
			ifr.ifr_data = (void *)  &data;

			retval = ioctl(socket_desc, IOCTL_READ_TXDESC,&ifr);
			if(retval < 0)
				break;
			printf("TxD  %02x: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x   0x%08x  0x%08x  0x%08x  0x%08x\n",
					count,dmadesc.status, dmadesc.length,dmadesc.buffer1,dmadesc.buffer2,
					dmadesc.data1,dmadesc.data2,dmadesc.extstatus,dmadesc.reserved1,dmadesc.timestamplow,dmadesc.timestamphigh);
		}
		if(retval < 0)
			perror("IOCTL Error\n");
#else
		printf("RxD Idx:   Status      length      buffer1     buffer2     data1       data2 \n");
		printf("---------------------------------------------------------------------------\n");
		for(count = 0; count < gmacdev.RxDescCount; count++ ){
			dmadesc.data1 = count;
			//		ifr.ifr_data = (void *) &dmadesc;
			if     ( strcmp( argv[0], "ch0" ) == 0 ) { data.unit = 0;}
			else if( strcmp( argv[0], "ch1" ) == 0 ) { data.unit = 1;}
			else if( strcmp( argv[0], "ch2" ) == 0 ) { data.unit = 2;}

			data.addr  = (u32) &dmadesc;
			ifr.ifr_data = (void *)  &data;

			retval = ioctl(socket_desc, IOCTL_READ_RXDESC,&ifr);
			if(retval < 0)
				break;
			printf("RxD  %02x: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
					count,dmadesc.status, dmadesc.length,dmadesc.buffer1,dmadesc.buffer2,
					dmadesc.data1,dmadesc.data2);
		}
		if(retval < 0)
			perror("IOCTL Error\n");

		printf("\n");

		printf("TxD Idx:   Status      length      buffer1     buffer2     data1       data2 \n");
		printf("-----------------------------------------------------------------------------\n");
		for(count = 0; count < gmacdev.TxDescCount; count++ ){
			dmadesc.data1 = count;
			//		ifr.ifr_data = (void *) &dmadesc;
			if     ( strcmp( argv[0], "ch0" ) == 0 ) { data.unit = 0;}
			else if( strcmp( argv[0], "ch1" ) == 0 ) { data.unit = 1;}
			else if( strcmp( argv[0], "ch2" ) == 0 ) { data.unit = 2;}

			data.addr  = (u32) &dmadesc;
			ifr.ifr_data = (void *)  &data;


			retval = ioctl(socket_desc, IOCTL_READ_TXDESC,&ifr);
			if(retval < 0)
				break;
			printf("TxD  %02x: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x \n",
					count,dmadesc.status, dmadesc.length,dmadesc.buffer1,dmadesc.buffer2,
					dmadesc.data1,dmadesc.data2);
		}
		if(retval < 0)
			perror("IOCTL Error\n");
#endif
	}
	close(socket_desc);
	return 0;
}

#ifdef AVB_SUPPORT
void get_avb_params(struct avb_user_input * avb_params)
{
	int user_input=0;
	char user_string[10];


	do{
		system("clear");
		printf("--------------------------------------------------\n");
		printf("Select the Options Below:\n");
		printf("--------------------------------------------------\n");


		// Minutes for which experiment needs to be run in Seconds
		printf("(01) Duration to run the Experiment (seconds)  : %02d\n\n",avb_params->duration_of_exp);

		// Dma Channel Enable bits
		printf("(02) DMA Channel Enable                        : %02d\n\n",avb_params->dma_ch_en);

		// Frame size for Channel 2
		printf("(11) Ch2 Frame size in bytes (Only Payload)    : %04d\n",avb_params->Ch2_fr_size);

		// The percentage bandwidth allocation for CH2 (CH1 BW + CH2 BW) < 75
		printf("(12) Ch2 Bandwidth Allocation                  : %02d%\n",avb_params->Ch2_bw_alloc);

		// Do Slot number Check is required? 1=> yes, 0=> no
		printf("(13) CH2 Enable Slot Number Check              : %01d\n",avb_params->Ch2_use_slot_no_check);
		// Do advanced slot interval data fetch required? 1=>yes, 0=>no
		printf("(14) CH2 Advanced slot interval data fetch     : %01d\n",avb_params->Ch2_use_adv_slot_int);
		//How Many Slots to be used to report average bits per slot
		//000=>1 slot, 001=>::2 slots, 010=>4 slots, 101=>8 slots, 100=>16 slots
		printf("(15) Ch2 Slot Counter for AVG Bits Reporting   : %01d\n",avb_params->Ch2_slot_count_to_use);
		// Set to 1 for Credit shape algrithm to take effect,  0 means strict priority

		// 0 Indicates weighted round robin, 1 indicates Strict Priority
		printf("(16) Ch2 Tx and Rx Priority Scheme  (1/0)      : %01d\n",avb_params->Ch2_tx_rx_prio_policy);
		// 1 indicates Ch2 TX has High priority over Rx, if 0 Vice versa
		printf("(17) Ch2 Tx has High priority over Rx (1/0)    : %01d\n",avb_params->Ch2_use_tx_high_prio);
		// Tx/RX or Rx/Tx Priority ratio for RR Priority Scheme
		// 00=> 1:1, 01=> 2:1, 10=> 3:1, 11=> 4:1
		printf("(18) Ch2 Tx and Rx Priority Ratio              : %01d\n", avb_params->Ch2_tx_rx_prio_ratio);


		printf("(19) Ch2 Uses Credit Shaped Algorith (1/0)     : %01d\n",avb_params->Ch2_use_credit_shape);
		// 1 Indicates Tx has high priority over Rx,  0 indicates Rx has high priority over Tx
		printf("     Ch2 Uses Credit Control Algorith (1/0)    : %01d\n",avb_params->Ch2CreditControl);

		printf("(20) Ch2 Tx Desc Starting Slot No.   <0 .. 15> : %01x\n",avb_params->Ch2_tx_desc_slot_no_start);
		printf("(21) Ch2 Tx Desc Slot No. Skip count <0 .. 15> : %01x\n",avb_params->Ch2_tx_desc_slot_no_skip);

		printf("(22) Ch2 Arbitration Weight          <1,2,3,4> : %01x\n\n",avb_params->Ch2_arb_weight);

		// Frame size for Channel 1
		printf("(31) Ch1 Frame size in bytes (Only Payload)    : %04d\n",avb_params->Ch1_fr_size);

		// The percentage bandwidth allocation for CH1 (CH1 BW + CH2 BW) < 75
		printf("(32) Ch1 Bandwidth Allocation                  : %02d%\n",avb_params->Ch1_bw_alloc);

		// Do Slot number Check is required? 1=> yes, 0=> no
		printf("(33) CH1 Enable Slot Number Check              : %01d\n",avb_params->Ch1_use_slot_no_check);
		// Do advanced slot interval data fetch required? 1=>yes, 0=>no
		printf("(34) CH1 Advanced slot interval data fetch     : %01d\n",avb_params->Ch1_use_adv_slot_int);
		//How Many Slots to be used to report average bits per slot
		//000=>1 slot, 001=>2 slots, 010=>4 slots, 101=>8 slots, 100=>16 slots
		printf("(35) Ch1 Slot Counter for AVG Bits Reporting   : %01d\n",avb_params->Ch1_slot_count_to_use);
		// Set to 1 for Credit shape algrithm to take effect,  0 means strict priority

		// 0 Indicates weighted round robin, 1 indicates Strict Priority
		printf("(36) Ch1 Tx and Rx Priority Scheme  (1/0)      : %01d\n",avb_params->Ch1_tx_rx_prio_policy);
		// 1 indicates Ch1 TX has High priority over Rx, if 0 Vice versa
		printf("(37) Ch1 Tx has High priority over Rx (1/0)    : %01d\n",avb_params->Ch1_use_tx_high_prio);
		// Tx/RX or Rx/Tx Priority ratio for RR Priority Scheme
		// 00=> 1:1, 01=> 2:1, 10=> 3:1, 11=> 4:1
		printf("(38) Ch1 Tx and Rx Priority Ratio              : %01d\n", avb_params->Ch1_tx_rx_prio_ratio);


		printf("(39) Ch1 Uses Credit Shaped Algorith (1/0)     : %01d\n",avb_params->Ch1_use_credit_shape);
		// 1 Indicates Tx has high priority over Rx,  0 indicates Rx has high priority over Tx
		printf("     Ch1 Uses Credit Control Algorith (1/0)    : %01d\n",avb_params->Ch1CreditControl);

		printf("(40) Ch1 Tx Desc Starting Slot No.   <0 .. 15> : %01x\n",avb_params->Ch1_tx_desc_slot_no_start);
		printf("(41) Ch1 Tx Desc Slot No. Skip count <0 .. 15> : %01x\n",avb_params->Ch1_tx_desc_slot_no_skip);
		printf("(42) Ch1 Arbitration Weight          <1,2,3,4> : %01x\n\n",avb_params->Ch1_arb_weight);


		// Frame size for Channel 0
		printf("(51) Ch0 Frame size in bytes (Only Payload)    : %04d\n",avb_params->Ch0_fr_size);

		// 0 Indicates weighted round robin, 1 indicates Strict Priority
		printf("(52) Ch0 Tx and Rx Priority Scheme  (1/0)      : %01d\n",avb_params->Ch0_tx_rx_prio_policy);
		// 1 indicates Ch0 TX has High priority over Rx, if 0 Vice versa
		printf("(53) Ch0 Tx has High priority over Rx (1/0)    : %01d\n",avb_params->Ch0_use_tx_high_prio);
		// Tx/RX or Rx/Tx Priority ratio for RR Priority Scheme
		// 00=> 1:1, 01=> 2:1, 10=> 3:1, 11=> 4:1
		printf("(54) Ch0 Tx and Rx Priority Ratio              : %01d\n", avb_params->Ch0_tx_rx_prio_ratio);

		printf("(55) Ch0 Arbitration Weight          <1,2,3,4> : %01x\n\n",avb_params->Ch0_arb_weight);

		printf("(99) To quit this menu\n\n");
		printf("Your Option here::");
		fscanf(stdin,"%s",user_string);
		sscanf(user_string,"%d",&user_input);

		switch (user_input){
			case 01:
				//                        printf("user input = %d\n",user_input);
				printf("(01) Duration to run the Experiment (Seconds) <1 .. 200>  : %02d\n",avb_params->duration_of_exp);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input > 0) && (user_input <= 200))//we allow maximum of 200 Seconds of experimentation
					avb_params->duration_of_exp =  user_input;
				break;
			case 02:
				printf("(02) DMA Channel Enable                                  : %02d\n\n",avb_params->dma_ch_en);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input <= 7)//Onmly LS 3 bits are supoosed to change
					avb_params->dma_ch_en =  user_input;
				break;

			case 11:
				printf("(11) Ch2 Frame size in bytes (Only Payload) <48 .. 1500>  : %04d\n",avb_params->Ch2_fr_size);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input > 48) && (user_input <=1500))//we allow frames between 48 and 1500
					avb_params->Ch2_fr_size =  user_input;
				break;
			case 12:
				printf("(12) Ch2 Bandwidth Allocation         <0 .. 75>        : %02d%\n",avb_params->Ch2_bw_alloc);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 0) && (user_input <=75))//we allow bw between 0 and 75%
					avb_params->Ch2_bw_alloc =  user_input;
				break;
			case 13:
				printf("(13) CH2 Enable Slot Number Check        <1 or 0>      : %01d\n",avb_params->Ch2_use_slot_no_check);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 ) //Value greater than 0 is treated as 1
					avb_params->Ch2_use_slot_no_check =  1;
				else
					avb_params->Ch2_use_slot_no_check =  0;
				break;
			case 14:
				printf("(14) CH2 Advanced slot interval data fetch <1 or 0>  : %01d\n",avb_params->Ch2_use_adv_slot_int);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input > 0) && (user_input <=75))//we allow bw between 0 and 75%
					avb_params->Ch2_use_adv_slot_int =  1;
				else
					avb_params->Ch2_use_adv_slot_int =  0;
				break;
			case 15:
				printf("(15) Ch2 Slot Counter for AVG Bits Reporting <1, 2, 4, 8, 16>  : %01d\n",avb_params->Ch2_slot_count_to_use);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input != 1) || (user_input != 2) || (user_input != 4) || (user_input != 8) || (user_input != 16))
					avb_params->Ch2_slot_count_to_use  =  user_input;
				break;
			case 16:
				printf("(16) Ch2 Tx and Rx Priority Scheme  <1 or 0>      : %01d\n",avb_params->Ch2_tx_rx_prio_policy);
				printf("0 => Weighted Round Robin\n");
				printf("1 => Strict Priority\n");
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch2_tx_rx_prio_policy = 1;
				else
					avb_params->Ch2_tx_rx_prio_policy = 0;
				break;
			case 17:
				printf("(17) Ch2 Tx has High priority over Rx <1 or 0>    : %01d\n",avb_params->Ch2_use_tx_high_prio);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch2_use_tx_high_prio = 1;
				else
					avb_params->Ch2_use_tx_high_prio = 0;
				break;
			case 18:
				printf("(18) Ch2 Tx and Rx Priority Ratio <0 , 1, 2, 3>       : %01d\n", avb_params->Ch2_tx_rx_prio_ratio);
				printf("0 => ratio is 1:1\n");
				printf("1 => ratio is 2:1\n");
				printf("2 => ratio is 3:1\n");
				printf("3 => ratio is 4:1\n");
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input != 0) || (user_input != 1) || (user_input != 2) || (user_input != 3))
					avb_params->Ch2_tx_rx_prio_ratio =  user_input;
				break;
			case 19:
				printf("(19a) Ch2 Uses Credit Shaped Algorith <1 or 0>     : %01d\n",avb_params->Ch2_use_credit_shape);
				printf("0 => Disable Credit Shape Algorithm\n");
				printf("1 => Enable Credit Shape Algorithm\n");
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch2_use_credit_shape = 1;
				else{
					avb_params->Ch2_use_credit_shape = 0;
					avb_params->Ch2CreditControl = 0;
				}
				if(avb_params->Ch2_use_credit_shape == 1){
					printf("(19b) Ch2 Uses Credit Control Algorith <1 or 0>     : %01d\n",avb_params->Ch2CreditControl);
					printf("0 => No Credit Control\n");
					printf("1 => Enforce Credit Control\n");
					printf("Your Option here::");
					fscanf(stdin,"%s",user_string);
					sscanf(user_string,"%d",&user_input);
					if(user_input > 0 )//we allow bw between 0 and 75%
						avb_params->Ch2CreditControl = 1;
					else
						avb_params->Ch2CreditControl = 0;
				}
				break;
			case 20:
				printf("(20) Ch2 Tx Desc Uses Starting Slot Number  <0 .. 15>     : %01d\n",avb_params->Ch2_tx_desc_slot_no_start);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 0) && (user_input <= 15) )//we allow bw between 0 and 75%
					avb_params->Ch2_tx_desc_slot_no_start = user_input;
				break;
			case 21:
				printf("(21) Ch2 Tx Desc Uses Starting Slot Skip Number <1 .. 15>  : %01d\n\n",avb_params->Ch2_tx_desc_slot_no_skip);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 0) && (user_input <= 15) )//we allow bw between 0 and 75%
					avb_params->Ch2_tx_desc_slot_no_skip = user_input;
				break;
			case 22:
				printf("(22) Ch2 Arbitration Weight            <1,2,3,4>         : %01x\n\n",avb_params->Ch2_arb_weight);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 1) && (user_input <= 4) )
					avb_params->Ch2_arb_weight= user_input;
				break;

			case 31:
				printf("(31) Ch1 Frame size in bytes (Only Payload) <48 .. 1500>  : %04d\n",avb_params->Ch1_fr_size);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input > 48) && (user_input <=1500))//we allow frames between 48 and 1500
					avb_params->Ch1_fr_size =  user_input;
				break;
			case 32:
				printf("(32) Ch1 Bandwidth Allocation         <0 .. 75>        : %02d%\n",avb_params->Ch1_bw_alloc);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);

				sscanf(user_string,"%d",&user_input);
				if((user_input >= 0) && (user_input <=75))//we allow bw between 0 and 75%
					avb_params->Ch1_bw_alloc =  user_input;
				break;
			case 33:
				printf("(33) CH1 Enable Slot Number Check        <1 or 0>      : %01d\n",avb_params->Ch1_use_slot_no_check);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 ) //Value greater than 0 is treated as 1
					avb_params->Ch1_use_slot_no_check =  1;
				else
					avb_params->Ch1_use_slot_no_check =  0;
				break;
			case 34:
				printf("(34) CH1 Advanced slot interval data fetch <1 or 0>  : %01d\n",avb_params->Ch1_use_adv_slot_int);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input > 0) && (user_input <=75))//we allow bw between 0 and 75%
					avb_params->Ch1_use_adv_slot_int =  1;
				else
					avb_params->Ch1_use_adv_slot_int =  0;
				break;
			case 35:
				printf("(35) Ch1 Slot Counter for AVG Bits Reporting <1, 2, 4, 8, 16>  : %01d\n",avb_params->Ch1_slot_count_to_use);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input != 1) || (user_input != 2) || (user_input != 4) || (user_input != 8) || (user_input != 16))
					avb_params->Ch1_slot_count_to_use  =  user_input;
				break;
			case 36:
				printf("(36) Ch1 Tx and Rx Priority Scheme  <1 or 0>      : %01d\n",avb_params->Ch1_tx_rx_prio_policy);
				printf("0 => Weighted Round Robin\n");
				printf("1 => Strict Priority\n");
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch1_tx_rx_prio_policy = 1;
				else
					avb_params->Ch1_tx_rx_prio_policy = 0;
				break;
			case 37:
				printf("(37) Ch1 Tx has High priority over Rx <1 or 0>    : %01d\n",avb_params->Ch1_use_tx_high_prio);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch1_use_tx_high_prio = 1;
				else
					avb_params->Ch1_use_tx_high_prio = 0;
				break;
			case 38:
				printf("(38) Ch1 Tx and Rx Priority Ratio <0 , 1, 2, 3>       : %01d\n", avb_params->Ch1_tx_rx_prio_ratio);
				printf("0 => ratio is 1:1\n");
				printf("1 => ratio is 2:1\n");
				printf("2 => ratio is 3:1\n");
				printf("3 => ratio is 4:1\n");
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input != 0) || (user_input != 1) || (user_input != 2) || (user_input != 3))
					avb_params->Ch1_tx_rx_prio_ratio =  user_input;
				break;
			case 39:
				printf("(39) Ch1 Uses Credit Shaped Algorith <1 or 0>     : %01d\n",avb_params->Ch1_use_credit_shape);
				printf("0 => Disable Credit Shape Algorithm\n");
				printf("1 => Enable Credit Shape Algorithm\n");
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch1_use_credit_shape = 1;
				else{
					avb_params->Ch1_use_credit_shape = 0;
					avb_params->Ch1CreditControl = 0;
				}
				if(avb_params->Ch1_use_credit_shape == 1){
					printf("(19b) Ch1 Uses Credit Control Algorith <1 or 0>     : %01d\n",avb_params->Ch1CreditControl);
					printf("0 => No Credit Control\n");
					printf("1 => Enforce Credit Control\n");
					printf("Your Option here::");
					fscanf(stdin,"%s",user_string);
					sscanf(user_string,"%d",&user_input);
					if(user_input > 0 )//we allow bw between 0 and 75%
						avb_params->Ch1CreditControl = 1;
					else
						avb_params->Ch1CreditControl = 0;
				}

				break;

			case 40:
				printf("(40) Ch2 Tx Desc Uses Starting Slot Number  <0 .. 15>     : %01d\n",avb_params->Ch1_tx_desc_slot_no_start);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 0) && (user_input <= 15) )//we allow bw between 0 and 75%
					avb_params->Ch1_tx_desc_slot_no_start = user_input;
				break;
			case 41:
				printf("(41) Ch2 Tx Desc Uses Starting Slot Skip Number <0 .. 15>  : %01d\n\n",avb_params->Ch1_tx_desc_slot_no_skip);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 0) && (user_input <= 15) )//we allow bw between 0 and 75%
					avb_params->Ch1_tx_desc_slot_no_skip = user_input;
				break;
			case 42:
				printf("(42) Ch1 Arbitration Weight            <1,2,3,4>         : %01x\n\n",avb_params->Ch1_arb_weight);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 1) && (user_input <= 4) )
					avb_params->Ch1_arb_weight= user_input;
				break;

			case 51:
				printf("(51) Ch0 Frame size in bytes (Only Payload) <48 .. 1500>  : %04d\n",avb_params->Ch0_fr_size);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input > 48) && (user_input <=1500))//we allow frames between 48 and 1500
					avb_params->Ch0_fr_size =  user_input;
				break;

			case 52:
				printf("(52) Ch0 Tx and Rx Priority Scheme  <1 or 0>      : %01d\n",avb_params->Ch0_tx_rx_prio_policy);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch0_tx_rx_prio_policy = 1;
				else
					avb_params->Ch0_tx_rx_prio_policy = 0;
				break;
			case 53:
				printf("(53) Ch0 Tx has High priority over Rx <1 or 0>    : %01d\n",avb_params->Ch0_use_tx_high_prio);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if(user_input > 0 )//we allow bw between 0 and 75%
					avb_params->Ch0_use_tx_high_prio = 1;
				else
					avb_params->Ch0_use_tx_high_prio = 0;
				break;
			case 54:
				printf("(54) Ch0 Tx and Rx Priority Ratio <0 , 1, 2, 3>       : %01d\n", avb_params->Ch0_tx_rx_prio_ratio);
				printf("0 => ratio is 1:1\n");
				printf("1 => ratio is 2:1\n");
				printf("2 => ratio is 3:1\n");
				printf("3 => ratio is 4:1\n");
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input != 0) || (user_input != 1) || (user_input != 2) || (user_input != 3))
					avb_params->Ch0_tx_rx_prio_ratio =  user_input;
				break;
			case 55:
				printf("(55) Ch0 Arbitration Weight            <1,2,3,4>         : %01x\n\n",avb_params->Ch0_arb_weight);
				printf("Your Option here::");
				fscanf(stdin,"%s",user_string);
				sscanf(user_string,"%d",&user_input);
				if((user_input >= 1) && (user_input <= 4) )//we allow bw between 0 and 75%
					avb_params->Ch0_arb_weight= user_input;
				break;

			default:
				printf("Default case\n");

		}

	}while(user_input !=99);
	if((avb_params->Ch2_bw_alloc + avb_params->Ch1_bw_alloc) > 75)
		avb_params->Ch1_bw_alloc = 75 - avb_params->Ch2_bw_alloc;
}

void populate_avb_structure(struct avb_user_input * avb_params, synopGMACavbStruct * avb_structure)
{
	avb_structure->ChSelMask                 = avb_params->dma_ch_en;

	avb_structure->DurationOfExp             = avb_params->duration_of_exp;


	avb_structure->AvControlCh               = 0; //Not Used
	avb_structure->PTPCh                     = 0; //Not Used
	avb_structure->PrioTagForAV              = 5; //Priority of Ch2 is used for this                              */

	avb_structure->AvType                    = AVTYPE; //#def value

	avb_structure->Ch2PrioWts                = avb_params->Ch2_arb_weight;
	avb_structure->Ch2Bw                     = avb_params->Ch2_bw_alloc;
	avb_structure->Ch2_frame_size            = avb_params->Ch2_fr_size;
	avb_structure->Ch2_EnableSlotCheck       = avb_params->Ch2_use_slot_no_check;
	avb_structure->Ch2_AdvSlotInterval       = avb_params->Ch2_use_adv_slot_int;
	avb_structure->Ch2SlotCount              = avb_params->Ch2_slot_count_to_use;
	avb_structure->Ch2AvgBitsPerSlot         = 0;
	avb_structure->Ch2AvgBitsPerSlotAccL     = 0x00000000;
	avb_structure->Ch2AvgBitsPerSlotAccH     = 0;
	avb_structure->Ch2AvgBitsNoOfInterrupts  = 0;

	avb_structure->Ch2_tx_rx_prio_policy     = avb_params->Ch2_tx_rx_prio_policy;   // Should Ch1 use Strict or RR policy
	avb_structure->Ch2_use_tx_high_prio      = avb_params->Ch2_use_tx_high_prio;    // Should Ch1 Tx have High priority over Rx
	avb_structure->Ch2_tx_rx_prio_ratio      = avb_params->Ch2_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx

	avb_structure->Ch2CreditControl          = avb_params->Ch2CreditControl;
	avb_structure->Ch2CrSh                   = avb_params->Ch2_use_credit_shape;


	avb_structure->Ch2IdleSlope              = ((4 * avb_params->Ch2_bw_alloc) * 1024)/100;//4 is for MII for GMII it will be 8
	avb_structure->Ch2SendSlope              = (4 * 1024) - avb_structure->Ch2IdleSlope;
	avb_structure->Ch2HiCredit               = (((MAX_INT_FRAME_SIZE * avb_params->Ch2_bw_alloc)/100) * 1024)*8;
	avb_structure->Ch2LoCredit               = -((avb_structure->Ch2HiCredit * (100 - avb_params->Ch2_bw_alloc))/100);

	avb_structure->Ch2FramecountTx           = 0;
	avb_structure->Ch2FramecountRx           = 0;

	avb_structure->Ch2_tx_desc_slot_no_start = avb_params->Ch2_tx_desc_slot_no_start;
	avb_structure->Ch2_tx_desc_slot_no_skip  = avb_params->Ch2_tx_desc_slot_no_skip;

	avb_structure->Ch1PrioWts                = avb_params->Ch1_arb_weight;
	avb_structure->Ch1Bw                     = avb_params->Ch1_bw_alloc;
	avb_structure->Ch1_frame_size            = avb_params->Ch1_fr_size;
	avb_structure->Ch1_EnableSlotCheck       = avb_params->Ch1_use_slot_no_check;
	avb_structure->Ch1_AdvSlotInterval       = avb_params->Ch1_use_adv_slot_int;
	avb_structure->Ch1SlotCount              = avb_params->Ch1_slot_count_to_use;
	avb_structure->Ch1AvgBitsPerSlot         = 0;
	avb_structure->Ch1AvgBitsPerSlotAccL     = 0x00000000;
	avb_structure->Ch1AvgBitsPerSlotAccH     = 0;
	avb_structure->Ch1AvgBitsNoOfInterrupts  = 0;

	avb_structure->Ch1_tx_rx_prio_policy     = avb_params->Ch1_tx_rx_prio_policy;   // Should Ch1 use Strict or RR policy
	avb_structure->Ch1_use_tx_high_prio      = avb_params->Ch1_use_tx_high_prio;    // Should Ch1 Tx have High priority over Rx
	avb_structure->Ch1_tx_rx_prio_ratio      = avb_params->Ch1_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx

	avb_structure->Ch1CreditControl          = avb_params->Ch1CreditControl;
	avb_structure->Ch1CrSh                   = avb_params->Ch1_use_credit_shape;

	avb_structure->Ch1IdleSlope              = ((4 * avb_params->Ch1_bw_alloc) * 1024)/100;//4 is for MII for GMII it will be 8
	avb_structure->Ch1SendSlope              = (4 * 1024) - avb_structure->Ch1IdleSlope;
	avb_structure->Ch1HiCredit               = (((MAX_INT_FRAME_SIZE * avb_params->Ch1_bw_alloc)/100) * 1024)*8;
	avb_structure->Ch1LoCredit               = -((avb_structure->Ch1HiCredit * (100 - avb_params->Ch1_bw_alloc))/100);
	//avb_structure->Ch1HiCredit               = ((MAX_INT_FRAME_SIZE * avb_params->Ch1_bw_alloc)/100) * 1024;
	//avb_structure->Ch1LoCredit               = -((MAX_INT_FRAME_SIZE * (100 - avb_params->Ch1_bw_alloc))/100);

	avb_structure->Ch1FramecountTx           = 0;
	avb_structure->Ch1FramecountRx           = 0;

	avb_structure->Ch0PrioWts                = avb_params->Ch0_arb_weight;
	avb_structure->Ch0_frame_size            = avb_params->Ch0_fr_size;

	avb_structure->Ch0_tx_rx_prio_policy     = avb_params->Ch0_tx_rx_prio_policy;   // Should Ch1 use Strict or RR policy
	avb_structure->Ch0_use_tx_high_prio      = avb_params->Ch0_use_tx_high_prio;    // Should Ch1 Tx have High priority over Rx
	avb_structure->Ch0_tx_rx_prio_ratio      = avb_params->Ch0_tx_rx_prio_ratio;    // For Round Robin what is the ratio between tx-rx or rx-tx

	avb_structure->Ch0FramecountTx           = 0;
	avb_structure->Ch0FramecountRx           = 0;

	avb_structure->Ch1_tx_desc_slot_no_start = avb_params->Ch1_tx_desc_slot_no_start;
	avb_structure->Ch1_tx_desc_slot_no_skip  = avb_params->Ch1_tx_desc_slot_no_skip;
}
static int Avb_debug_mode(char * ifname, u32 mode)
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h
	struct ifr_data_struct data;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );
	if(mode)
		data.unit = AVB_DEBUG_ENABLE;
	else
		data.unit = AVB_DEBUG_DISABLE;
	data.data = 0;
	data.addr = 0;

	ifr.ifr_data = (void *) &data;
	//  ifr.ifr_data = (void *) avb_structure;

	//  retval = ioctl( socket_desc, IOCTL_AVB_RUN_TEST, &ifr );
	retval = ioctl( socket_desc, IOCTL_AVB_TEST, &ifr );
	if( retval < 0 )
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}

	printf("AVB Tx Frames Started ... \n");

close_socket:
	close(socket_desc);
	return retval;
}


static int Avb_tx_packets(char * ifname)
{
	char frame_count_str[10];
	unsigned int frame_count;
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h
	struct ifr_data_struct data;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	// Prompt for change in AVB parameters
	printf("Enter the No. of Frames  <1 ... 10>:");
	sscanf(frame_count_str,"%d",&frame_count);
	fscanf(stdin,"%s",&frame_count_str);
	if((frame_count > 100) || (frame_count ==0))
		frame_count = 2;

	strcpy( ifr.ifr_name, ifname );

	data.unit = AVB_TX_FRAMES;
	data.data = frame_count;
	data.addr = 0;

	ifr.ifr_data = (void *) &data;
	//  ifr.ifr_data = (void *) avb_structure;

	//  retval = ioctl( socket_desc, IOCTL_AVB_RUN_TEST, &ifr );
	retval = ioctl( socket_desc, IOCTL_AVB_TEST, &ifr );
	if( retval < 0 )
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}

	printf("AVB Tx Frames Started ... \n");

close_socket:
	close(socket_desc);
	return retval;
}

static int Avb_debug_menu(char * ifname,synopGMACavbStruct * avb_structure)
{
	int user_input=0;
	char user_string[10];
	Avb_debug_mode(ifname, 1);// ENTER DEBUG MODE
	do{
		system("clear");
		printf("--------------------------------------------------\n");
		printf("Select the Options Below:\n");
		printf("--------------------------------------------------\n");

		// Set the Frame Count - Frames to transmit
		printf("(01) Set Frame Count for Tx\n");
		printf("(99) To quit this menu\n\n");

		printf("Your Option here::");
		fscanf(stdin,"%s",user_string);
		sscanf(user_string,"%d",&user_input);

		switch (user_input){
			case 01:
				Avb_tx_packets(ifname);
				break;
			default:
				printf("Wrong input.......\n");
		}
	} while(user_input != 99);
	Avb_debug_mode(ifname, 0); //OUT OF DEBUG MODE

	return 0;
}

void generate_avb_report(struct avb_user_input * avb_params, synopGMACavbStruct * avb_structure)
{

	float ch2_bw,ch1_bw,ch0_bw;
	int user_input=0;
	char user_string[10];
	double avg_bits_ch2=0, avg_bits_ch1=0;


	system("clear");
	printf("---------------------------------------------------------------------------\n");
	printf("Duration Of The Experiment (Seconds)       : %02d\n",avb_params->duration_of_exp);
	printf("DMA Channel Enabled                        : %02d\n",avb_params->dma_ch_en);

	printf("---------------------------------------------------------------------------\n");
	printf("                                    CH2            CH1            CH0\n");
	printf("CH Prio Weights                     %01d              %01d              %01d\n",avb_structure->Ch2PrioWts,
			avb_structure->Ch1PrioWts,avb_structure->Ch0PrioWts);
	printf("Frame Size                          %03d           %03d           %03d\n",avb_structure->Ch2_frame_size,
			avb_structure->Ch1_frame_size,avb_structure->Ch0_frame_size);
	printf("Band Width Allocation               %03d(%)         %03d(%)         %03d(%)\n",avb_structure->Ch2Bw,
			avb_structure->Ch1Bw,100 - (avb_structure->Ch2Bw+avb_structure->Ch1Bw));
	printf("Slot Number Check Enabled?          %01d              %01d              %c\n",avb_structure->Ch2_EnableSlotCheck,
			avb_structure->Ch1_EnableSlotCheck,'-');
	printf("Adv Slot Int Data Fetch?            %01d              %01d              %c\n",
			avb_structure->Ch2_EnableSlotCheck,avb_structure->Ch2_EnableSlotCheck,'-');
	printf("Slot Counter For Avg Bit Report     %02d             %02d             %c\n",avb_structure->Ch2SlotCount,
			avb_structure->Ch1SlotCount,'-');
	printf("Tx-Rx Prio Scheme(0=>RR 1=>SP)      %01d              %01d              %1d\n",avb_structure->Ch2_tx_rx_prio_policy,
			avb_structure->Ch1_tx_rx_prio_policy,avb_structure->Ch0_tx_rx_prio_policy);
	printf("Tx Has High Prio Over Rx            %01d              %01d              %1d\n",avb_structure->Ch2_use_tx_high_prio,
			avb_structure->Ch1_use_tx_high_prio,avb_structure->Ch0_use_tx_high_prio);
	printf("Tx and Rx Prio Ratio (For RR )      %01d              %01d              %1d\n",avb_structure->Ch2_tx_rx_prio_ratio,
			avb_structure->Ch1_tx_rx_prio_ratio,avb_structure->Ch0_tx_rx_prio_ratio);
	printf("Ch Uses Credit Shape Algo?          %01d              %01d              %c\n",avb_structure->Ch2CrSh,
			avb_structure->Ch1CrSh,'-' );
	printf("Ch Uses Credit Control Algo?        %01d              %01d              %c\n",avb_structure->Ch2CreditControl,
			avb_structure->Ch1CreditControl,'-' );
	printf("Ch Idle Slope                       %08x       %08x       %c\n",avb_structure->Ch2IdleSlope,
			avb_structure->Ch1IdleSlope,'-' );
	printf("Ch Send Slope                       %08x       %08x       %c\n",avb_structure->Ch2SendSlope,
			avb_structure->Ch1SendSlope,'-' );
	printf("Ch Hi Credit                        %08x       %08x       %c\n",avb_structure->Ch2HiCredit,
			avb_structure->Ch1HiCredit,'-' );
	printf("Ch Lo Credit                        %08x       %08x       %c\n\n",avb_structure->Ch2LoCredit,
			avb_structure->Ch1LoCredit,'-' );
	printf("Ch Transmit Frame Count             %08x       %08x       %08x\n",avb_structure->Ch2FramecountTx,
			avb_structure->Ch1FramecountTx, avb_structure->Ch0FramecountTx);
	printf("Ch Receive Frame Count              %08x       %08x       %08x\n",avb_structure->Ch2FramecountRx,
			avb_structure->Ch1FramecountRx, avb_structure->Ch0FramecountRx);

	ch2_bw =(float) (((avb_structure->Ch2FramecountRx/avb_params->duration_of_exp) * (avb_structure->Ch2_frame_size + 8 + 4) * 8))/1000000;
	ch1_bw =(float) (((avb_structure->Ch1FramecountRx/avb_params->duration_of_exp) * (avb_structure->Ch1_frame_size + 8 + 4) * 8))/1000000;
	ch0_bw =(float) (((avb_structure->Ch0FramecountRx/avb_params->duration_of_exp) * (avb_structure->Ch0_frame_size + 8 + 4) * 8))/1000000;
	printf("Ch Bw Util Achieved                 %3.2F(%)       %3.2F(%)       %3.2F(%)\n",ch2_bw,ch1_bw,ch0_bw);
	printf("(Preamble+Ether-Frame+FCS-IPG )\n");
	printf("Ch Avg Bits Per Slot                %08x       %08x       %c\n",avb_structure->Ch2AvgBitsPerSlot,
			avb_structure->Ch1AvgBitsPerSlot,'-');
	printf("Ch Avg Bits Per Slot Accumulated(Hi)%08x       %08x       %c\n",avb_structure->Ch2AvgBitsPerSlotAccH,
			avb_structure->Ch1AvgBitsPerSlotAccH,'-');
	printf("Ch Avg Bits Per Slot Accumulated(Lo)%08x       %08x       %c\n",avb_structure->Ch2AvgBitsPerSlotAccL,
			avb_structure->Ch1AvgBitsPerSlotAccL,'-');

	avg_bits_ch2 = (((double)avb_structure->Ch2AvgBitsPerSlotAccH * (double)(pow(2.0,32.0)))
			+(double)avb_structure->Ch2AvgBitsPerSlotAccL );

	avg_bits_ch1 = (((double)avb_structure->Ch1AvgBitsPerSlotAccH * (double)(pow((double) 2, (double) 32)))
			+(double)avb_structure->Ch1AvgBitsPerSlotAccL );

	printf("Ch Avg Bits Interrupts (total)      %08x       %08x       %c\n",avb_structure->Ch2AvgBitsNoOfInterrupts,
			avb_structure->Ch1AvgBitsNoOfInterrupts,'-');

	printf("Channel BW from Avg Bits            %3.2F(%)       %3.2F(%)       %c\n",
			((avg_bits_ch2/avb_structure->Ch2AvgBitsNoOfInterrupts)/1000) * 8 ,
			((avg_bits_ch1/avb_structure->Ch1AvgBitsNoOfInterrupts)/1000) * 8,'-');

	printf("---------------------------------------------------------------------------\n");
	printf("Enter 99 return to main menu::");
	do{
		fscanf(stdin,"%s",user_string);
		sscanf(user_string,"%d",&user_input);
	} while(user_input!=99);
}

static int get_avb_result_from_hw(char * ifname,synopGMACavbStruct * avb_structure)// AVB_GET_RESULT
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h
	struct ifr_data_struct data;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );

	data.unit = AVB_GET_RESULT;
	data.data = 0;
	data.addr = (u32) avb_structure;

	ifr.ifr_data = (void *) &data;

	//  ifr.ifr_data = (void *) avb_structure;

	//  retval = ioctl( socket_desc, IOCTL_AVB_GET_RESULT, &ifr );
	retval = ioctl( socket_desc, IOCTL_AVB_TEST, &ifr );
	if( retval < 0 )
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}

	printf("Successfully Retrieved AVB Results \n");

close_socket:
	close(socket_desc);
	return retval;
}

static int run_avb_test(char * ifname,synopGMACavbStruct  * avb_structure)// AVB_RUN_TEST = 3
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h
	struct ifr_data_struct data;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );

	data.unit = AVB_RUN_TEST;
	data.data = 0;
	data.addr = (u32) avb_structure;

	ifr.ifr_data = (void *) &data; 
	//  ifr.ifr_data = (void *) avb_structure;

	//  retval = ioctl( socket_desc, IOCTL_AVB_RUN_TEST, &ifr );
	retval = ioctl( socket_desc, IOCTL_AVB_TEST, &ifr );
	if( retval < 0 )
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}

	printf("AVB Test Started \n");

close_socket:  close(socket_desc);
	       return retval;


}

static int send_avb_params_to_driver (char *ifname, synopGMACavbStruct  * avb_structure)//AVB_SET_CONFIG
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h
	struct ifr_data_struct data;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );

	data.unit = AVB_SET_CONFIG;
	data.data = 0;
	data.addr = (u32) avb_structure;

	ifr.ifr_data = (void *) &data; 

	//  ifr.ifr_data = (void *) avb_structure;

	//  retval = ioctl( socket_desc, IOCTL_AVB_SET_CONFIG, &ifr );
	retval = ioctl( socket_desc, IOCTL_AVB_TEST, &ifr );
	if( retval < 0 )
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}

	printf("Successfully Configured AVB parameters \n");

close_socket:  close(socket_desc);
	       return retval;

}
static int config_hw_for_avb_test(char * ifname)
{
	int socket_desc;
	int retval = 0;
	struct ifreq ifr;  //defined in if.h
	struct ifr_data_struct data;

	socket_desc = socket( PF_INET, SOCK_DGRAM, 0 );
	if( socket_desc < 0 )
	{
		perror("Socket error");
		return -1;
	}

	strcpy( ifr.ifr_name, ifname );

	data.unit = AVB_CONFIG_HW;
	data.data = 0;
	data.addr = 0;

	ifr.ifr_data = (void *) &data; 

	//  ifr.ifr_data = (void *) avb_structure;

	//  retval = ioctl( socket_desc, IOCTL_AVB_SET_CONFIG, &ifr );
	retval = ioctl( socket_desc, IOCTL_AVB_TEST, &ifr );
	if( retval < 0 )
	{
		printf("IOCTL Error\n");
		goto close_socket;
	}

	printf("Successfully Configured The Hardware for AVB \n");

close_socket:  close(socket_desc);
	       return retval;

}
static void print_avb_structure(synopGMACavbStruct * avb_structure)
{
	printf("ChSelMask = %02x\n",avb_structure->ChSelMask);
	printf("DurationOfExp= %02x\n",avb_structure->DurationOfExp);
	printf("AvControlCh= %02x\n",avb_structure->AvControlCh);
	printf("PTPCh= %02x\n",avb_structure->PTPCh);
	printf ("PrioTagForAV= %02x\n",avb_structure->PrioTagForAV);
	printf("AvType= %04x\n",avb_structure->AvType);

	printf("Ch2PrioWts = %01x\n",avb_structure->Ch2PrioWts);
	printf("Ch2Bw= %02x\n",avb_structure->Ch2Bw);
	printf("Ch2_frame_size= %08x\n",avb_structure->Ch2_frame_size);
	printf("Ch2_EnableSlotCheck= %02x\n",avb_structure->Ch2_EnableSlotCheck);
	printf("Ch2_AdvSlotInterval= %02x\n",avb_structure->Ch2_AdvSlotInterval);
	printf("Ch2CrSh= %02x\n",avb_structure->Ch2CrSh);
	printf("Ch2SlotCount= %02x\n",avb_structure->Ch2SlotCount);
	printf("Ch2AvgBitsPerSlot= %08x\n",avb_structure->Ch2AvgBitsPerSlot);
	printf("Ch2AvgBitsPerSlotAccL= %08x\n",avb_structure->Ch2AvgBitsPerSlotAccL);
	printf("Ch2AvgBitsPerSlotAccH= %08x\n",avb_structure->Ch2AvgBitsPerSlotAccH);
	printf("Ch2AvgBitsPerSlotInterrupts= %08x\n",avb_structure->Ch2AvgBitsNoOfInterrupts);
	printf("Ch2CreditControl= %02x\n",avb_structure->Ch2CreditControl);
	printf("Ch2_tx_rx_prio_policy= %02x\n",avb_structure->Ch2_tx_rx_prio_policy);
	printf("Ch2_use_tx_high_prio= %02x\n",avb_structure->Ch2_use_tx_high_prio);
	printf("Ch2_tx_rx_prio_ratio= %02x\n",avb_structure->Ch2_tx_rx_prio_ratio);
	printf("Ch2SendSlope= %08x\n",avb_structure->Ch2SendSlope);
	printf("Ch2IdleSlope= %08x\n",avb_structure->Ch2IdleSlope);
	printf("Ch2HiCredit= %08x\n",avb_structure->Ch2HiCredit);
	printf("Ch2LoCredit= %08x\n",avb_structure->Ch2LoCredit);
	printf("Ch2FramecountTx= %08x\n",avb_structure->Ch2FramecountTx);
	printf("Ch2FramecountRx= %08x\n",avb_structure->Ch2FramecountRx);
	printf("Ch2 Tx Desc Starting Slot No = %01x\n",avb_structure->Ch2_tx_desc_slot_no_start);
	printf("Ch2 Tx Desc Slot No. Skip count = %01x\n",avb_structure->Ch2_tx_desc_slot_no_skip);

	printf("Ch1PrioWts = %01x\n",avb_structure->Ch1PrioWts);
	printf("Ch1Bw= %02x\n",avb_structure->Ch1Bw);
	printf("Ch1_frame_size= %08x\n",avb_structure->Ch1_frame_size);
	printf("Ch1_EnableSlotCheck= %02x\n",avb_structure->Ch1_EnableSlotCheck);
	printf("Ch1_AdvSlotInterval= %02x\n",avb_structure->Ch1_AdvSlotInterval);
	printf("Ch1CrSh= %02x\n",avb_structure->Ch1CrSh);
	printf("Ch1SlotCount= %02x\n",avb_structure->Ch1SlotCount);
	printf("Ch1AvgBitsPerSlot= %08x\n",avb_structure->Ch1AvgBitsPerSlot);
	printf("Ch1AvgBitsPerSlotAccL= %08x\n",avb_structure->Ch1AvgBitsPerSlotAccL);
	printf("Ch1AvgBitsPerSlotAccH= %08x\n",avb_structure->Ch1AvgBitsPerSlotAccH);
	printf("Ch1AvgBitsPerSlotInterrupts= %08x\n",avb_structure->Ch1AvgBitsNoOfInterrupts);
	printf("Ch1CreditControl= %02x\n",avb_structure->Ch1CreditControl);
	printf("Ch1_tx_rx_prio_policy= %02x\n",avb_structure->Ch1_tx_rx_prio_policy);
	printf("Ch1_use_tx_high_prio= %02x\n",avb_structure->Ch1_use_tx_high_prio);
	printf("Ch1_tx_rx_prio_ratio= %02x\n",avb_structure->Ch1_tx_rx_prio_ratio);
	printf("Ch1SendSlope= %08x\n",avb_structure->Ch1SendSlope);
	printf("Ch1IdleSlope= %08x\n",avb_structure->Ch1IdleSlope);
	printf("Ch1HiCredit= %08x\n",avb_structure->Ch1HiCredit);
	printf("Ch1LoCredit= %08x\n",avb_structure->Ch1LoCredit);
	printf("Ch1FramecountTx= %08x\n",avb_structure->Ch1FramecountTx);
	printf("Ch1FramecountRx= %08x\n",avb_structure->Ch1FramecountRx);
	printf("Ch1 Tx Desc Starting Slot No = %01x\n",avb_structure->Ch1_tx_desc_slot_no_start);
	printf("Ch1 Tx Desc Slot No. Skip count = %01x\n",avb_structure->Ch1_tx_desc_slot_no_skip);


	printf("Ch0PrioWts = %01x\n",avb_structure->Ch0PrioWts);
	printf("Ch0_frame_size= %08x\n",avb_structure->Ch0_frame_size);
	printf("Ch0_tx_rx_prio_policy= %02x\n",avb_structure->Ch0_tx_rx_prio_policy);
	printf("Ch0_use_tx_high_prio= %02x\n",avb_structure->Ch0_use_tx_high_prio);
	printf("Ch0_tx_rx_prio_ratio= %02x\n",avb_structure->Ch0_tx_rx_prio_ratio);
	printf("Ch0FramecountTx= %08x\n",avb_structure->Ch0FramecountTx);
	printf("Ch0FramecountRx= %08x\n",avb_structure->Ch0FramecountRx);
}

static int avbtest(char *ifname, int argc, char *argv[] )
{
	struct avb_user_input * avb_params;
	synopGMACavbStruct  * avb_structure;

	int user_input=0;
	char user_string[10];

	avb_params = (struct avb_user_input *) malloc(sizeof(struct avb_user_input));
	if(avb_params == NULL){
		printf("Error in Memory Allocation for AVB structure(User)\n");
		return 1;
	}
	avb_structure = (synopGMACavbStruct *) malloc(sizeof(synopGMACavbStruct));
	if(avb_structure == NULL){
		printf("Error in Memory Allocation for AVB structure(Kernel)\n");
		return 1;
	}

	/* The Following three functions are invoked to avoid if user runs the avb test without configuring the hardware.*/
	initialize_default_avb(avb_params);// Initialize the Default AVB parameters
	populate_avb_structure(avb_params,avb_structure);
	send_avb_params_to_driver(ifname,avb_structure);
	config_hw_for_avb_test(ifname);

	printf("You are about to start AVB Testing....\n");

	do{
		system("clear");
		printf("--------------------------------------------------\n");
		printf("Select the Options Below:\n");
		printf("--------------------------------------------------\n");

		// Minutes for which experiment needs to be run in Seconds
		printf("(01) Show/Set the Avb Parameters  \n");   
		printf("(02) Send Parameters to Driver\n");
		printf("(03) Configure HW for AVB Test\n");
		printf("(04) Run AVB test  \n");   
		printf("(05) Show AVB Test Reports \n");   
		printf("(06) Enter Debug Mode \n");   
		printf("(07) Print AVB Structure\n");
		printf("(99) To quit this menu\n\n");

		printf("Your Option here::");
		fscanf(stdin,"%s",user_string);
		sscanf(user_string,"%d",&user_input);

		switch (user_input){
			case 01:
				//printf("user input = %d\n",user_input);
				// Prompt for change in AVB parameters
				get_avb_params(avb_params);
				// Populate the modifications done
				populate_avb_structure(avb_params,avb_structure);
				break;
			case 02:
				// Populate the modifications done
				populate_avb_structure(avb_params,avb_structure);
				//Send the avb_params_to_driver
				send_avb_params_to_driver(ifname,avb_structure);
				break;
			case 03:
				//Configure the HW for testing
				//Populates the buffers in Tx descriptors
				//Programs the BUS_MODE_REGISTER and OMR registers
				config_hw_for_avb_test(ifname);
				break;
			case 04:
				// Run Avb test
				run_avb_test(ifname,avb_structure);
				break;
			case 05:
				// Get Avb Results from Hardware
				get_avb_result_from_hw(ifname,avb_structure);
				// Geneate Avb test Report
				generate_avb_report(avb_params, avb_structure);
				break;
			case 06:
				Avb_debug_menu(ifname,avb_structure);
				break;
			case 07:
				print_avb_structure(avb_structure);
				break;

			case 99:
				break;
			default:
				printf("Wrong input.......\n");
		}

	}while(user_input !=99);

	return 0;
}
#endif

int main( int argc, char *argv[] )
{
	int retval = 1;

	printf(copyright);


	if( argc>=3 )
	{
		//  if( strcmp( argv[2], "debug" ) == 0 )    retval = debug( argv[1], argc-3, argv+3 );
		if( strcmp( argv[2], "dump" ) == 0 )     retval = dump( argv[1], argc-3, argv+3 );
		if( strcmp( argv[2], "write" ) == 0 )    retval = wcs( wcs_write, argv[1], argv[2], argc-3, argv+3 );
		if( strcmp( argv[2], "read" ) == 0 )     retval = read(  argv[1], argc-3, argv+3 );
		if( strcmp( argv[2], "set" ) == 0 )      retval = wcs( wcs_set, argv[1], argv[2], argc-3, argv+3 );
		if( strcmp( argv[2], "clr" ) == 0 )      retval = wcs( wcs_clr, argv[1], argv[2], argc-3, argv+3 );
		if( strcmp( argv[2], "status" ) == 0 )   retval = status( argv[1], argc-3, argv+3 );
		if( strcmp( argv[2], "powerdown") == 0)  retval = powerdown(argv[1],argc-3,argv+3);
#ifdef AVB_SUPPORT
		if( strcmp( argv[2], "avbtest") == 0)    retval = avbtest(argv[1],argc-3,argv+3);
#endif

	}
	if( retval==1 ) printf( usage );
	return retval;
}
