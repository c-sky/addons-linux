#ifndef __SILAN_GENERIC_H__
#define __SILAN_GENERIC_H__
#include <linux/types.h>

void static inline sl_writeb(u8 val, unsigned long reg)
{
    *(volatile u8 *)(reg) = val;
#ifdef CONFIG_MIPS
    __asm__ volatile ("ehb");
#endif
}

void static inline sl_writew(u16 val, unsigned long reg)
{
    *(volatile u16 *)(reg) = val;
#ifdef CONFIG_MIPS
    __asm__ volatile ("ehb");
#endif
}

void static inline sl_writel(u32 val, unsigned long reg)
{
    *(volatile u32 *)(reg) = val;
#ifdef CONFIG_MIPS
    __asm__ volatile ("ehb");
#endif
}

static inline u8 sl_readb(unsigned long reg)
{
    return (*(volatile u8 *)reg);
}

static inline u16 sl_readw(unsigned long reg)
{
    return (*(volatile u16 *)reg);
}

static inline u32 sl_readl(unsigned long reg)
{
    return (*(volatile u32 *)reg);
}

#endif

