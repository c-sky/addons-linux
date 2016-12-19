#ifndef __SILAN_DEF_H__
#define __SILAN_DEF_H__

/***************************************************************************
 * Configuration related to memory
***************************************************************************/

/* Start address of the register space */
#define SILAN_REG_SPACE_START		0x1FA00000
/* Size of register space */
#define SILAN_REG_SPACE_SIZE		0x600000
/* Android pmem */
#define SILAN_PMEM0_SIZE			0x4000000
#define SILAN_PMEM1_SIZE			0x4000000
/* Size of memory region aviable to DSP */
#define SIZE_DSP_MEM_REGION         (768*1024*1024)
#define ADUIO_FIRMWARE_SIZE         (0x1000000)
#define IVS_FIRMWARE_SIZE           (0x1000000)
#define DSP_PRINT_BUFF_RSV_SIZE     (1024)
#define DSP_PRINT_BUFF_SIZE         (1024*3 + DSP_PRINT_BUFF_RSV_SIZE)
#define DSP_PRINT_RSV_SIZE          (DSP_PRINT_BUFF_SIZE)

/***************************************************************************
 * Configuration related to clock
***************************************************************************/

/* Frequency of external oscillator (Unit: HZ) */	
#define EXTERNAL_OSC_FREQ			27000000

/*normal uart number*/
#define UART_NORMAL_NR		3

/*
 * Clock debug on or off
 * define it:  debug on 
 * comment it: debuf off
 */
/*#define SILAN_CLK_DEBUG			1*/

/* 
 * Use perfect PLL setting follow or not
 * 0: disable
 * 1: enable
 */
#define USE_PERFECT_PLL_SET_FLOW	0

/***************************************************************************
 * Utilities
***************************************************************************/

/*
 * Check if varaible 'mid' is more near to varaible 'big' than to 'smal'
 * Note: 'smal' should be smaller than or equal to 'big'
 */
#define IS_NEAR_BIG(smal, mid, big)	((((big) - (mid)) < ((mid) - (smal))) ? 1 : 0)

/* Get the min or the max value */
#ifndef MIN                              
#define MIN(x, y)       (((x) > (y))?(y):(x))
#endif 

#ifndef MAX                              
#define MAX(x, y)       (((x) > (y))?(x):(y))
#endif 

unsigned long get_silan_sys_pllclk(void);
unsigned long get_silan_cpu_pllclk(void);

#endif
