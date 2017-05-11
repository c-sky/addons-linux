/*
 * drivers/net/cskymac.c
 *
 * cskymac.c: Driver for CSKY CK1000EVB MAC 10/100baseT ethernet adapters.
 *
 * Copyright (C) 2005, Li Chunqiang (chunqiang_li@c-sky.com)
 * Copyright (C) 2009, Ye Yun (yun_ye@c-sky.com)
 */

#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/in.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/crc32.h>
#include <linux/errno.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <asm/cacheflush.h>
#include "cskymac.h"

//#define DEBUG_CSKY_MAC

static struct cskymac *root_cskymac_dev;

static char version[] __initdata = "CSKY MAC driver v1.1\n";

const  unsigned long CSKYMACADDR[2] = { 0x00800000, 0x0041 };
/*
#undef DEBUG_PROBE
#undef DEBUG_TX
#undef DEBUG_IRQ
#undef DEBUG_RX
#undef DEBUG_CSKY_MAC
*/

//#define DEBUG_PROBE
//#define DEBUG_TX
//#define DEBUG_IRQ
//#define DEBUG_RX
//#define DEBUG_CSKY_MAC

#ifdef DEBUG_PROBE
#define DP(x)  printk x
#else
#define DP(x)
#endif

#ifdef DEBUG_TX
#define DTX(x)  printk x
#else
#define DTX(x)
#endif

#ifdef DEBUG_IRQ
#define DIRQ(x)  printk x
#define DIRQ_BUSY
#else
#define DIRQ(x)
#undef  DIRQ_BUSY
#endif

#ifdef DEBUG_RX
#define DRX(x) printk x
#else
#define DRX(x)
#endif

static void cskymac_get_macaddr (unsigned char macaddr[])
{
	unsigned long  temp[2];

	temp[1] = htonl(CSKYMACADDR[1]);
	temp[0] = htonl(CSKYMACADDR[0]);

	memcpy(macaddr, (unsigned char *)&temp[1]+2, 0x02);
	memcpy(&macaddr[2], (unsigned char *)&temp[0], 0x04);

	return;
}

static void cskymac_set_macaddr (void)
{
	unsigned char	macaddr[6];
	unsigned long  temp[2];
	CKMAC_REG_BASEADDR[CKMAC_ADDR1] = (CSKYMACADDR[1]);
	CKMAC_REG_BASEADDR[CKMAC_ADDR0] = (CSKYMACADDR[0]);

#ifdef __ckcoreBE__
	temp[1] = (CKMAC_REG_BASEADDR[CKMAC_ADDR0]);
	temp[0] = (CKMAC_REG_BASEADDR[CKMAC_ADDR1]);

	memcpy(macaddr, (unsigned char *)&temp[0]+2, 0x02);
	memcpy(&macaddr[2], (unsigned char *)&temp[1], 0x04);
	printk ("Mac Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
			macaddr[0], macaddr[1], macaddr[2],
                      macaddr[3], macaddr[4], macaddr[5]);
#else

	temp[0]= CKMAC_REG_BASEADDR[CKMAC_ADDR0];
	temp[1]= CKMAC_REG_BASEADDR[CKMAC_ADDR1];

	macaddr[0] = (temp[1] >> 8) & 0xff;
	macaddr[1] = temp[1] & 0xff;

	macaddr[2] = (temp[0] >> 24) & 0xff;
	macaddr[3] = (temp[0] >> 16) & 0xff;
	macaddr[4] = (temp[0] >> 8 ) & 0xff;
	macaddr[5] = (temp[0] & 0xff);

	printk ("Mac Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", macaddr[0],
		macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
#endif

	return;
}

static PTXDESC GetTxDescBaseAddr ( void )
{
	return (PTXDESC)CKMAC_DESC_BASEADDR;
}

static PRXDESC GetRxDescBaseAddr ( void )
{
	return (PRXDESC)CKMAC_DESC_BASEADDR + TX_RING_SIZE;
}

static void cskymac_tx_reset(void)
{
	int	txbdindex;
	PTXDESC ptxbd;

	ptxbd = GetTxDescBaseAddr();
	for (txbdindex = 0; txbdindex < TX_RING_SIZE; txbdindex++)
	{
		ClrBit(ptxbd->status, TX_RD_BIT);
		ptxbd++;
	}
	return;
}

static void cskymac_rx_reset(void)
{
	int	rxbdindex;
	PRXDESC	prxbd;

	prxbd  = GetRxDescBaseAddr();
	for (rxbdindex = 0; rxbdindex < RX_RING_SIZE; rxbdindex++)
	{
		prxbd->status |=  RX_ETY_BIT;
		prxbd++;
	}
	return;
}

/* Reset the transmitter and receiver. */
static void cskymac_stop(void)
{
	unsigned long   tmp;

	/* Disable tr & rx */
	tmp = CKMAC_REG_BASEADDR[CKMAC_MODE];
	tmp &= ~CKMAC_MODE_TXEN;
	tmp &= ~CKMAC_MODE_RXEN;
	CKMAC_REG_BASEADDR[CKMAC_MODE] = tmp;
	/* Reset the tx&rx BD */
	cskymac_tx_reset();
	cskymac_rx_reset();
	return;
}


static void cskymac_clean_stats(struct cskymac *dp)
{
    memset(&dp->enet_stats, 0, sizeof (struct net_device_stats));
}


static void cskymac_clean_rings(struct net_device *dev)
{
	int i;
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);

	for (i = 0; i < RX_RING_SIZE; i++)
	{
		if (dp->rx_skbs[i] != NULL)
		{
			dev_kfree_skb_any(dp->rx_skbs[i]);
			dp->rx_skbs[i] = NULL;
		}
	}

	for (i = 0; i < TX_RING_SIZE; i++)
	{
		if (dp->tx_skbs[i] != NULL)
		{
			dev_kfree_skb_any(dp->tx_skbs[i]);
			dp->tx_skbs[i] = NULL;
		}
	}
	return;
}

static void cskymac_init_rings(struct net_device *dev, int from_irq)
{
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);
	int	 i, gfp_flags = GFP_KERNEL;
	PRXDESC  prxbd;
	PTXDESC	 ptxbd;

	if (from_irq || in_interrupt())
		gfp_flags = GFP_ATOMIC;

	dp->rx_new = dp->rx_old = dp->tx_new = dp->tx_old = 0;

	/* Free any skippy bufs left around in the rings. */
	cskymac_clean_rings(dev);

	/* Now get new skbufs for the receive ring. */
	prxbd  = GetRxDescBaseAddr();
	for (i = 0; i < RX_RING_SIZE; i++,prxbd++)
	{
		struct sk_buff *skb;

		skb = alloc_skb(RX_BUF_ALLOC_SIZE, gfp_flags);
		if (!skb)
		{
			continue;
		}
		skb->dev = dev;

		/* Because we reserve afterwards. */
		skb_reserve(skb, 34);
		skb_put(skb, ETH_FRAME_LEN);
		prxbd->buf = __pa(skb->data);
		/*
		 * Alloc skbs for mac receive, and they are immovable untill
		 * closing mac. Then map skb->data where mac dma store rx data
		 * to uncachable virtual memory. So that, when copy rx data in rx
		 * func it's coherent to memory.
		 */
		dp->rx_skbs[i] = skb;
#ifdef CONFIG_MMU
		dp->rx_skbs[i]->data = UNCACHE_ADDR(skb->data);
#endif

		if (i == RX_RING_SIZE - 1)
		{
			prxbd->status = RX_ETY_BIT | RX_IRQ_BIT | RX_WR_BIT;
			prxbd->status &=0xffff;
		}
		else
		{
			prxbd->status = RX_ETY_BIT | RX_IRQ_BIT;
			prxbd->status &=0xffff;
		}

	}

	ptxbd = GetTxDescBaseAddr();
	for (i = 0; i < TX_RING_SIZE; i++,ptxbd++)
	{
		if (i == TX_RING_SIZE - 1)
		{
			ptxbd->status = TX_IRQ_BIT | TX_WR_BIT | TX_PAD_BIT;
		}
		else
		{
			ptxbd->status = TX_IRQ_BIT | TX_PAD_BIT;
		}
		ptxbd->buf = 0;
	}
	return;
}

static int cskymac_init(struct net_device *dev, int from_irq)
{
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);

	/* Set mac working mode: 0x0002a400 */
	CKMAC_REG_BASEADDR[CKMAC_MODE]   = CKMAC_MODE_LPMD_RXEN |
			CKMAC_MODE_PAD | CKMAC_MODE_CRCEN | CKMAC_MODE_FULLD;
#if 0
	//Initial PHY
	CKMAC_REG_BASEADDR[CKMAC_MIIADD] = 0x00000102; // mii address
        /*  mii command (read status register phy) */
	CKMAC_REG_BASEADDR[CKMAC_MIICMD] = CKMAC_MIICMD_RSTAT;
	CKMAC_REG_BASEADDR[CKMAC_MIIADD] = 0x00000002;
	CKMAC_REG_BASEADDR[CKMAC_MIICMD] &= ~(1<<13);
	/* waiting for set PHY Addr and REG ADDR */
	while (CKMAC_REG_BASEADDR[CKMAC_MIISTS] & 0x01);
#endif

	/* set transmit buffer descriptor */
	CKMAC_REG_BASEADDR[CKMAC_TXBD]   = TX_RING_SIZE;

	/* set interrupt mask. */
	CKMAC_REG_BASEADDR[CKMAC_IMASK]  = CKMAC_IMASK_RXC | CKMAC_IMASK_TXC |
			CKMAC_IMASK_BUSY | CKMAC_IMASK_RXE | CKMAC_IMASK_RXB |
			CKMAC_IMASK_TXE | CKMAC_IMASK_TXB;
	/* set packet length, Min length = 0x40, Max length = 0x600 */
	CKMAC_REG_BASEADDR[CKMAC_SIZE]  = 0x00400600;
	/* clear the interrupt source register */
	CKMAC_REG_BASEADDR[CKMAC_INT] = 0xff;

	/* Latch current counters into statistics. */
	cskymac_clean_stats(dp);
	/* Alloc and reset the tx/rx descriptor chains. */
	cskymac_init_rings(dev, from_irq);
	/* Stop transmitter and receiver. */
	cskymac_stop();
	/* Set hardware ethernet address. */
	cskymac_set_macaddr();
	/* Clear the hash table until mc upload occurs. */
	CKMAC_REG_BASEADDR[CKETH_HASH0] = 0;
	CKMAC_REG_BASEADDR[CKETH_HASH1] = 0;
	dp->rx_new = dp->rx_old = dp->tx_new = dp->tx_old = 0;
	CKMAC_REG_BASEADDR[CKMAC_MODE] |= CKMAC_MODE_TXEN | CKMAC_MODE_RXEN;

	return 0;
}

/* CSKY MAC transmit complete interrupt service routines. */
static void cskymac_tx(struct net_device *dev)
{
	PTXDESC			txbase = GetTxDescBaseAddr();
	int			elem;
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);

	spin_lock(&dp->lock);

	elem = dp->tx_old;
	DTX(("cskymac_tx: tx_old[%d] ", elem));
	while (elem != dp->tx_new)
	{
		struct sk_buff	*skb = NULL;
		PTXDESC 	this = (PTXDESC)(&txbase[elem]);

		DTX(("this(%p) [flags(%08x)addr(%08x)]",
		     this, this->status, this->buf));

		skb = dp->tx_skbs[elem];
		if (this->status & TX_RD_BIT)
		{
			break;
		}
		if (unlikely(this->status & TXBD_ERR_BITS))
		{
			this->status &= ~TXBD_ERR_BITS;
			dp->enet_stats.tx_errors++;
			goto next;
		}
#ifndef __ckcoreBE__
		skb = dp->tx_skbs[elem];
#endif
		dp->enet_stats.tx_packets++;
		dp->enet_stats.tx_bytes += skb->len;
		DTX(("skb(%p) ", skb));
next:
		dev_kfree_skb_irq(skb);
		dp->tx_skbs[elem] = NULL;
		elem = NEXT_TX(elem);
	}
	DTX((" DONE, tx_old=%d\n", elem));
	dp->tx_old = elem;

	if (netif_queue_stopped(dev))
	{
		netif_wake_queue(dev);
	}
	spin_unlock(&dp->lock);

	return;
}

/* CSKY MAC receive int service routines. */
static void cskymac_rx(struct net_device *dev)
{
	PRXDESC		rxbase = GetRxDescBaseAddr();
	PRXDESC		this;
	int 		elem;
	u32 		flags;
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);

	spin_lock(&dp->lock);
	elem = dp->rx_new;
	this = &rxbase[elem];
	while (!((flags = this->status) & RX_ETY_BIT))
	{
		DRX(("cskymac_rx: bd %d : 0x%x length = %d\n", elem, this, len));

		/*
		 * Check for errors, if descripter error, make it empty
		 * and reuse it; else receive it to upper layer and reuse it.
		 */
		if (unlikely(flags & RXBD_ERR_BITS))
		{
			this->status &= ~(RXBD_ERR_BITS | RX_ETY_BIT);
		}
		else
		{
			int len;
			struct sk_buff *skb, *copy_skb;

			len = (flags & RXD_LENGTH) >> 16;
			copy_skb = dev_alloc_skb(len + 2);
			if (!copy_skb)
			{
				dp->enet_stats.rx_dropped++;
				goto next;
			}
			skb = dp->rx_skbs[elem];
			skb_reserve(copy_skb, 2);    /* 16 byte align */
			skb_put(copy_skb, len);
			skb_copy_to_linear_data(copy_skb,
						(unsigned char *)skb->data, len);

			copy_skb->dev = dev;
			copy_skb->protocol = eth_type_trans(copy_skb, dev);
			netif_rx(copy_skb);
			dev->last_rx = jiffies;

			dp->enet_stats.rx_packets++;
			dp->enet_stats.rx_bytes += len;
		}
next:
		/* Reuse original ring buffer. */
		this->status |= RX_ETY_BIT;
		elem = NEXT_RX(elem);
		this = &rxbase[elem];
		if (elem == dp->rx_new)
		{
			break;
		}
	}
	dp->rx_new = elem;
	spin_unlock(&dp->lock);

	return;
}

static irqreturn_t cskymac_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	u32		mac_status = 0;

	DIRQ(("cskymac_interrupt: "));

	DIRQ(("mac_status = %08x, ", mac_status));

	/* Latch status registers now. */
	mac_status = CKMAC_REG_BASEADDR[CKMAC_INT];

	/* Having received a control frame */
	if (mac_status & CKMAC_INT_RXC)
	{
		DIRQ(("cskymac_interrupt: recieve a control frame!\n"));
		CKMAC_REG_BASEADDR[CKMAC_INT] |= CKMAC_INT_RXC;
	}

	/* A control frame is sent */
	if (mac_status & CKMAC_INT_TXC)
	{
		DIRQ(("cskymac_interrupt: have sent a control frame!\n"));
		CKMAC_REG_BASEADDR[CKMAC_INT] |= CKMAC_INT_TXC;
	}

	/* A data frame is sent or a tx error hanpens. */
	if (mac_status & (CKMAC_INT_TXB | CKMAC_INT_TXE))
	{
		DIRQ(("cskymac_interrupt: have sent a frame!\n"));
		CKMAC_REG_BASEADDR[CKMAC_INT] |= (CKMAC_INT_TXB | CKMAC_INT_TXE);
		cskymac_tx(dev);
	}

	/* A data frame is received, receive error or receive busy hanpens. */
	if (mac_status & (CKMAC_INT_RXB | CKMAC_INT_BUSY | CKMAC_INT_RXE))
	{
		DIRQ(("cskymac_interrupt: recieve a frame!\n"));
		CKMAC_REG_BASEADDR[CKMAC_INT] |= (CKMAC_INT_RXB|
									CKMAC_INT_BUSY | CKMAC_INT_RXE);
		cskymac_rx(dev);
	}
	return IRQ_HANDLED;
}

static int cskymac_open(struct net_device *dev)
{
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);
	int 		ret;

	init_timer(&dp->cskymac_timer);
	ret = cskymac_init(dev, 0);
	if (ret)
	{
		return ret;
	}

	ret = request_irq(dev->irq, &cskymac_interrupt,
				IRQF_SHARED, dev->name, (void *)dev);
	if (ret) {
		del_timer(&dp->cskymac_timer);
		dp->timer_state = asleep;
		dp->timer_ticks = 0;
		cskymac_stop();
		cskymac_clean_rings(dev);
		printk(KERN_ERR
			"CSKY MAC: Can't order irq %d to go.\n", dev->irq);
	}

	netif_start_queue(dev);

	return ret;
}

static int cskymac_close(struct net_device *dev)
{
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);

	netif_stop_queue(dev);
	del_timer(&dp->cskymac_timer);
	dp->timer_state = asleep;
	dp->timer_ticks = 0;

	cskymac_stop();
	cskymac_clean_rings(dev);
	free_irq(dev->irq, dev);
	return 0;
}

static void cskymac_tx_timeout(struct net_device *dev)
{
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);

	//DTX(("tx timeout happens.\n"));
	printk("tx timeout happens.\n");
	dp->enet_stats.tx_errors++;
	cskymac_init(dev, 0);
	netif_wake_queue(dev);

	return;
}

/*
 * Put a packet on the wire.
 * Initial a transmision for device.
 */
static int cskymac_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);
	int		len, entry;
	PTXDESC		ptxbd;

	len = skb->len;

	ptxbd = GetTxDescBaseAddr();

	/* Avoid a race... */
	spin_lock_irq(&dp->lock);
	entry = dp->tx_new;
	/*
	 * If the buffer is still busy, means that the tx BDs
	 * are full. So we stop the net queue and return a nonzero
	 * value, then the kernel will retry later.
	 */
	if (ptxbd[entry].status & TX_RD_BIT)
	{
		netif_stop_queue(dev);
		spin_unlock_irq(&dp->lock);
		return 1;
	}
	DTX(("cskymac_start_xmit: len(%d) entry(%d) buffer(%x)\n",
             len, entry, (unsigned int)skb->data));
	dp->tx_skbs[entry] = skb;
	ptxbd[entry].buf = __pa(skb->data);
	/*
	 * Below, mac will start a trasmit, before this, we use
	 * clear_dcache_all() to make coherent between data cache
	 * and memory. That make sure the mac transmit right data.
	 */
//	clear_dcache_all();

	if (entry == TX_RING_SIZE - 1)
	{
		ptxbd[entry].status = (TX_RD_BIT | TX_IRQ_BIT | TX_PAD_BIT |
			TX_WR_BIT | TX_CRC_BIT | TX_RTY_BIT | (len << 16));
	}
	else
	{
		ptxbd[entry].status = (TX_RD_BIT | TX_IRQ_BIT | TX_PAD_BIT |
			TX_CRC_BIT | TX_RTY_BIT |(len << 16));
	}

//	dev->trans_start = jiffies;
	dp->tx_new = NEXT_TX(entry);
	DTX(("\nfinish xmit.\n"));

	if (TX_BUFFS_AVAIL(dp) < 0) {
		DTX(("warning!the buffer is not available\n"));
		netif_stop_queue(dev);
	}

	spin_unlock_irq(&dp->lock);
	return 0;
}

static void cskymac_set_multicast(struct net_device *dev)
{
	struct cskymac	*dp = (struct cskymac *)netdev_priv(dev);
	struct netdev_hw_addr *ha;
	char			*addrs;
	int 			i;
	u32 			tmp, crc;

	/*
	 * Disable the receiver.  The bit self-clears when
	 * the operation is complete.
	 */
	tmp = CKMAC_REG_BASEADDR[CKMAC_MODE];
	tmp &= ~(CKMAC_MODE_RXEN);
	CKMAC_REG_BASEADDR[CKMAC_MODE] = tmp;

	if ((dev->flags & IFF_ALLMULTI) || (netdev_mc_count(dev) > CSKYMAC_TABLE_SIZE))
	{
		CKMAC_REG_BASEADDR[CKETH_HASH0] = 0xffffffff;
		CKMAC_REG_BASEADDR[CKETH_HASH1] = 0xffffffff;
		CKMAC_REG_BASEADDR[CKMAC_MODE] &= ~CKMAC_MODE_PRO;
		CKMAC_REG_BASEADDR[CKMAC_MODE] |= CKMAC_MODE_IAM;
	}
	else if (dev->flags & IFF_PROMISC)
	{
		tmp = CKMAC_REG_BASEADDR[CKMAC_MODE];
		tmp |= CKMAC_MODE_PRO;
		CKMAC_REG_BASEADDR[CKMAC_MODE] = tmp;
	}
	else if (netdev_mc_count(dev) == 0)
	{
		tmp = CKMAC_REG_BASEADDR[CKMAC_MODE];
		tmp &= ~CKMAC_MODE_PRO;
		tmp &= ~CKMAC_MODE_IAM;
		CKMAC_REG_BASEADDR[CKMAC_MODE] = tmp;
	}
	else
	{
		u16 hash_table[2];

		for (i = 0; i < 2; i++)
		{
			hash_table[i] = 0;
		}

		netdev_for_each_mc_addr(ha, dev) {
			addrs = ha->addr;

			if (!(*addrs & 1))
			{
				continue;
			}

			crc = ether_crc_le(6, addrs);
			crc >>= 26;
			hash_table[crc >> 5] |= 1 << (crc & 0x1f);
		}
		CKMAC_REG_BASEADDR[CKETH_HASH0] = hash_table[0];
		CKMAC_REG_BASEADDR[CKETH_HASH1] = hash_table[1];
		CKMAC_REG_BASEADDR[CKMAC_MODE] &= ~CKMAC_MODE_PRO;
		CKMAC_REG_BASEADDR[CKMAC_MODE] |= CKMAC_MODE_IAM;
	}

	/* Re-enable the receiver. */
	dp->rx_new = dp->rx_old = 0;
	tmp = CKMAC_REG_BASEADDR[CKMAC_MODE];
	tmp |= CKMAC_MODE_RXEN;
	CKMAC_REG_BASEADDR[CKMAC_MODE] = tmp;
}

static struct net_device_stats *cskymac_get_stats(struct net_device *dev)
{
	struct cskymac  *dp = (struct cskymac *)netdev_priv(dev);

	return &dp->enet_stats;
}

static const struct net_device_ops cskymac_ops = {
	.ndo_open               = cskymac_open,
	.ndo_stop               = cskymac_close,
	.ndo_start_xmit         = cskymac_start_xmit,
//	.ndo_set_multicast_list = cskymac_set_multicast,
	.ndo_tx_timeout         = cskymac_tx_timeout,
	.ndo_get_stats          = cskymac_get_stats,
};

static int __init cskymac_ether_init(void)
{
	static int		version_printed;
	struct cskymac		*dp;
        struct net_device	*dev;
	int			i;

	/* Get a new device struct for this interface. */
	dev = alloc_etherdev(sizeof(struct cskymac));
	if (!dev)
	{
		return -ENOMEM;
	}
	if (version_printed++ == 0)
	{
		printk(KERN_INFO "%s", version);
	}

	/* Report what we have found to the user. */
	printk(KERN_INFO "%s: CSKY MAC 10/100baseT Ethernet ", dev->name);
	dev->base_addr = (unsigned long)CKMAC_REG_BASEADDR;
	cskymac_get_macaddr(dev->dev_addr);
	dev->addr_len = 6;
	for (i = 0; i < 6; i++)
	{
		printk("%2.2x%c", dev->dev_addr[i], i == 5 ? ' ' : ':');
	}
	printk("\n");

	/*
	 * Setup softc, with backpointers to QEC and CSKY MAC
	 * SBUS device structs.
	 */
	dp = netdev_priv(dev);

	spin_lock_init(&dp->lock);

	/* Init auto-negotiation timer state. */
	init_timer(&dp->cskymac_timer);
	dp->timer_state = asleep;
	dp->timer_ticks = 0;

	dp->dev = dev;
	/*
	 * In new kernel, the net device options are all in struct
	 * net_device_ops, and the net_device own the ops by member
	 * netdev_ops.
	 */
	dev->netdev_ops = &cskymac_ops;

	dev->watchdog_timeo = 5 * HZ;

	/* Finish net device registration. */
	dev->irq = CSKYMAC_IRQBASE;
	dev->dma = 0;

    if (register_netdev(dev)) {
        printk(KERN_ERR "Cskymac: Cannot register device.\n");
        goto fail_and_cleanup;
    }
	root_cskymac_dev = dp;

	return 0;

fail_and_cleanup:
	/*
	 * Something went wrong, undo whatever we did so far.
	 * Free register mappings if any.
	 */
	unregister_netdev(dev);
	free_netdev(dev);
	return -ENODEV;
}

static void __exit cskymac_cleanup(void)
{
	struct cskymac *dp = root_cskymac_dev;

	unregister_netdev(dp->dev);
	free_netdev(dp->dev);
}


module_init(cskymac_ether_init);
module_exit(cskymac_cleanup);
MODULE_LICENSE("GPL");
