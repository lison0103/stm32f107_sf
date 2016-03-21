/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLparam.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2007
* Description        : This file contains the parameters to be customized for
*                      the final application.
********************************************************************************
* History:
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/*Reviewed for IEC 60335-1 Compliance
* This module can be used for self tests according to Table H.11.12.7 of appendix R of the standard ;EN/IEC 60335-1. The implementation of the module has to be tested in each application.
* Reviewed by V.D.E. Testing and Certification Institute ; Merianstrasse 28 ; D-63069 Offenbach
* VDE-Ref:    5008057-9999-0001/91436 FG23/Scb
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SELFTEST_PARAM_H
#define __SELFTEST_PARAM_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define U8_MAX     ((u8)255)
#define S8_MAX     ((s8)127)
#define S8_MIN     ((s8)-128)
#define U16_MAX    ((u16)65535u)
#define S16_MAX    ((s16)32767)
#define S16_MIN    ((s16)-32768)
#define U32_MAX    ((u32)4294967295uL)
#define S32_MAX    ((s32)2147483647)
#define S32_MIN    ((s32)2147483648uL)

/* This is for having self-diagnostic messages reported on a PC via UART */
//#define STL_VERBOSE_POR     /* During Power-on phase only*/
//#define STL_VERBOSE         /* During main program execution */

/* These are the direct and inverted data (pattern) used during the RAM
test, performed using March C- Algorithm */
#define BCKGRND     ((u32)0x00000000uL)
#define INV_BCKGRND ((u32)0xFFFFFFFFuL)

/* Initial value of the CRC */
#define CRC_INIT ((u32)0x00000000uL)

/* Define the time bases for clock frequency measure: RUN and START time bases
must be equal or multiple. RUN time base must take into account SysTick
prescaler and PLL setting (if enabled)*/
#define SYSTICK_2_ms_HSI ((u32)16000uL) /* HSI = 8MHz */

#define SYSTICK_2_ms_PLL ((u32)144000uL) /* PLL = 72MHz, SysTick clock not prescaled */

#define SYSTICK_TB_START  ((u32)SYSTICK_2_ms_HSI)
#define SYSTICK_TB_RUN  ((u32)SYSTICK_2_ms_PLL)

/* This is to provide a time base longer than the SysTick for the main */
/* For instance this is needed to refresh the LSI watchdog and window watchdog */
#define SYSTICK_20ms_TB  ((u32)10uL) // 10*2ms

/* Timeout required to avoid being stuck in while loops during clock circuitry
initialization, in case of problem. Watchdog is also active, but these timeouts
allow to react more quickly */
#define LSI_START_TIMEOUT ((u32)1000uL) /* ~3.5ms (IAR, speed optimized) */

/* ~35ms to be adapted depending on crystal/resonator */
#define HSE_START_TIMEOUT ((u32)10000uL)

/* For clock switch:~3.5ms (IAR, speed optimized) valid for 8MHz and 64MHz
(~3.5/8=450µs)*/
#define CLOCK_SWITCH_TIMEOUT ((u32)1000uL)

/* PLL lock Timeout set to ~10ms (IAR, speed optimized) */
#define PLL_LOCK_TIMEOUT ((u32)3000uL)

/* Value of the Internal oscillator in Hz*/
#define HSI_Freq    ((u32)8000000uL)

/* HSE frequency above this limit considered as harmonics*/
#define HSE_LimitHigh ((u32)(HSE_Value*5)/4) /* (HSEValue + 25%) */

/* HSE frequency below this limit considered as sub-harmonics*/
#define HSE_LimitLow ((u32)(HSE_Value*3)/4)  /* (HSEValue - 25%) */

/* -------------------------------------------------------------------------- */
/* ------------------ CONTROL FLOW TAGS and CHECKPOINTS --------------------- */
/* -------------------------------------------------------------------------- */
/* RAM_TEST_CALLEE is only needed for CtrlFlowCntInv when exiting routine */
/* This is because the RAM test routines destroys the control flow counters */
#define RAM_TEST_CALLEE         ((u32)0xFFFFFFFFuL)

#define CPU_TEST_CALLER         ((u32)2)
#define CPU_TEST_CALLEE         ((u32)3) /* Do not modify: hard coded in assembly file */
#define WDG_TEST_CALLER         ((u32)5)
#define CRC32_INIT_CALLEE       ((u32)7)
#define CRC32_TEST_CALLER       ((u32)11)
#define CRC32_TEST_CALLEE       ((u32)13)
#define CRC16_TEST_CALLER       ((u32)17)
#define CRC16_TEST_CALLEE       ((u32)19)
#define CLOCK_TEST_CALLER       ((u32)23)
#define CLOCK_TEST_CALLEE       ((u32)29)
#define LSI_INIT_CALLEE         ((u32)31)
#define HSE_INIT_CALLEE         ((u32)37)
#define RTC_INIT_CALLEE         ((u32)41)
#define SYSTICK_INIT_CALLEE     ((u32)43)
#define CLOCK_SWITCH_CALLEE     ((u32)47)
#define STACK_OVERFLOW_TEST     ((u32)53)
#define STACK_OVERFLOW_CALLEE   ((u32)59)
#define CLOCKPERIOD_TEST_CALLEE ((u32)61)
#define CRC16_RUN_TEST_CALLEE   ((u32)67)
#define CRC32_RUN_TEST_CALLEE   ((u32)71)
#define FLASH_TEST_CALLER       ((u32)73)

#define CHECKPOINT1 ((u32)CPU_TEST_CALLER + \
                          CPU_TEST_CALLEE + \
                          WDG_TEST_CALLER + \
                          CRC32_INIT_CALLEE + \
                          CRC32_TEST_CALLER + \
                          CRC32_TEST_CALLEE + \
                          CRC16_TEST_CALLER + \
                          CRC16_TEST_CALLEE)

#define CHECKPOINT2 ((u32)CLOCK_TEST_CALLER + \
                          CLOCK_TEST_CALLEE + \
                          LSI_INIT_CALLEE + \
                          HSE_INIT_CALLEE + \
                          RTC_INIT_CALLEE + \
                          SYSTICK_INIT_CALLEE + \
                          CLOCK_SWITCH_CALLEE + \
                          STACK_OVERFLOW_TEST)

/* This is for run-time tests with 16-bit CRC */
#define DELTA_MAIN  ((u32)CPU_TEST_CALLER + \
                          CPU_TEST_CALLEE + \
                          STACK_OVERFLOW_TEST + \
                          STACK_OVERFLOW_CALLEE + \
                          CLOCK_TEST_CALLER + \
                          CLOCK_TEST_CALLEE + \
                          CLOCKPERIOD_TEST_CALLEE + \
                          FLASH_TEST_CALLER + \
                          CRC16_RUN_TEST_CALLEE + \
                          CRC16_TEST_CALLEE)
#define LAST_DELTA_MAIN ((u32) DELTA_MAIN)
#define FULL_FLASH_CHECKED ((u32)DELTA_MAIN * STEPS_NUMBER)

/* This is for run-time tests with 32-bit CRC */
//#define DELTA_MAIN  ((u32)CPU_TEST_CALLER + \
//                          CPU_TEST_CALLEE + \
//                          STACK_OVERFLOW_TEST + \
//                          STACK_OVERFLOW_CALLEE + \
//                          CLOCK_TEST_CALLER + \
//                          CLOCK_TEST_CALLEE + \
//                          CLOCKPERIOD_TEST_CALLEE + \
//                          FLASH_TEST_CALLER + \
//                          CRC32_RUN_TEST_CALLEE + \
//                          CRC32_TEST_CALLEE)
//#define LAST_DELTA_MAIN ((u32) DELTA_MAIN  - CRC32_TEST_CALLEE + CRC32_INIT_CALLEE)
//
//#define FULL_FLASH_CHECKED ((u32)(DELTA_MAIN * STEPS_NUMBER) + LAST_DELTA_MAIN)


#define MEASPERIOD_ISR_CALLER   ((u32)2)
#define MEASPERIOD_ISR_CALLEE   ((u32)3)
#define CLOCKPERIOD_ISR_CALLEE  ((u32)5)
#define RAM_MARCHC_ISR_CALLER   ((u32)7)
#define RAM_MARCHC_ISR_CALLEE   ((u32)11)
#define RAM_MARCHX_ISR_CALLER   ((u32)13)
#define RAM_MARCHX_ISR_CALLEE   ((u32)17)

/* This is for March C tests */
#define DELTA_ISR  ((u32)  MEASPERIOD_ISR_CALLER + \
                           MEASPERIOD_ISR_CALLEE + \
                           RAM_MARCHC_ISR_CALLER + \
                           RAM_MARCHC_ISR_CALLEE)

/* This is for March X tests */
//#define DELTA_ISR  ((u32)  MEASPERIOD_ISR_CALLER + \
//                           MEASPERIOD_ISR_CALLEE + \
//                           RAM_MARCHX_ISR_CALLER + \
//                           RAM_MARCHX_ISR_CALLEE)

#define CLASS_B_ROWS ((u32)(CLASS_B_END - CLASS_B_START)/4)
/* +1 below is for buffer self-test*/
#define RAM_TEST_COMPLETED ((u32)(DELTA_ISR * (CLASS_B_ROWS+1)))

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */



#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */

#ifdef EWARMv5
/* This is the IAR compiler entry point, usually executed right after reset */
  extern void __iar_program_start( void );
  extern u16 __checksum;
/* The 4 variables here above are set by IAR linker */
  extern u32 __ICFEDIT_region_ROM_start__;
  extern u32 __ICFEDIT_region_ROM_end__;
  extern u32 __ICFEDIT_region_RAM_start__;
  extern u32 __ICFEDIT_region_RAM_end__;
  extern u32 __ICFEDIT_region_CLASSB_start__;
  extern u32 __ICFEDIT_region_CLASSB_end__;  

  /* Constants necessary for Flash CRC calculation */
  

  #define ROM_START (unsigned char *)&__ICFEDIT_region_ROM_start__ 
  #define ROM_SIZE  (unsigned int)&__ICFEDIT_region_ROM_end__ - 2 -(unsigned int)&__ICFEDIT_region_ROM_start__ + 1 /* ROM_SIZE in byte */
  #define ROM_SIZEinWORDS ((u32)&__ICFEDIT_region_ROM_end__ -2 -(u32)&__ICFEDIT_region_ROM_start__ +1)/4 /* ...in words */
  #define ROM_END   ((u8 *)(&__ICFEDIT_region_ROM_end__))

  #define STEPS_NUMBER        ((u32)1024uL)
  #define FLASH_BLOCK         (u32)((ROM_SIZE+2) / STEPS_NUMBER)
  #define FLASH_BLOCK_WORDS   ((u32)(ROM_SIZEinWORDS / STEPS_NUMBER))

  /* Constants necessary for RAM test (RAM_END is word aligned) */
  #define RAM_START (u32 *)0x20000000
  #define RAM_END   (u32 *)0x20004FFc

  /* Constants necessary for Transparent March tests */
  #define CLASS_B_START ((u32 *)(&__ICFEDIT_region_CLASSB_start__))
  #define CLASS_B_END ((u32 *)(&__ICFEDIT_region_CLASSB_end__))
  
  #define GotoCompilerStartUp() __iar_program_start();//Reset_Handler();//__iar_program_start();

  #define REF_CRC16 __checksum
#else



  /* This is the IAR compiler entry point, usually executed right after reset */
  extern void __program_start( void );
  extern u16 __checksum;

  /* The 4 variables here above are set by IAR linker */
  extern u32 ROMSTART;
  extern u32 ROMEND;
  extern u32 RAMSTART;
  extern u32 RAMEND;
  extern u32 CLASSB_START;
  extern u32 CLASSB_END;

  /* Constants necessary for Flash CRC calculation */
  #define ROM_START ((u8 *)(&ROMSTART))
  #define ROM_SIZE  ((u32)&ROMEND -2 -(u32)&ROMSTART +1) /* ROM_SIZE in byte */
  #define ROM_SIZEinWORDS ((u32)&ROMEND -2 -(u32)&ROMSTART +1)/4 /* ...in words */
  #define ROM_END   ((u8 *)(&ROMEND))

  #define STEPS_NUMBER        ((u32)1024uL)
  #define FLASH_BLOCK         (u32)((ROM_SIZE+2) / STEPS_NUMBER)
  #define FLASH_BLOCK_WORDS   ((u32)(ROM_SIZEinWORDS / STEPS_NUMBER))

  /* Constants necessary for RAM test (RAM_END is word aligned) */
  #define RAM_START ((u32 *)(&RAMSTART))
  #define RAMSIZEinWORDS ((u32) (&RAMEND - &RAMSTART))
  #define RAM_END   ((u32 *)(&RAMSTART + RAMSIZEinWORDS))

  /* Constants necessary for Transparent March tests */
  #define CLASS_B_START ((u32 *)(&CLASSB_START))
  #define CLASS_B_END ((u32 *)(&CLASSB_END))

  #define GotoCompilerStartUp() __program_start();

  #define REF_CRC16 __checksum
#endif 
#endif  /* __IAR_SYSTEMS_ICC__ */



#ifdef __CC_ARM   /* KEIL Compiler */

  /* This is the KEIL compiler entry point, usually executed right after reset */
  extern void Reset_Handler( void );

  /* Constants necessary for Flash CRC calculation (ROM_SIZE in byte) */
  #define ROM_START ((u8 *) 0x08000000)													
  #define ROM_SIZE  ((u32)  0x0001FFFF) /* byte-aligned address */
  #define ROM_SIZEinWORDS (u32) (ROM_SIZE/4)
  #define ROM_END   ((u8 *) 0x0801FFFF)

  #define STEPS_NUMBER          ((u32)1024uL)
  #define FLASH_BLOCK           (u32)((ROM_SIZE+1) / STEPS_NUMBER)
  #define FLASH_BLOCK_WORDS     (u32)((ROM_SIZEinWORDS) / STEPS_NUMBER)


  /* Constants necessary for Transparent March tests */
  #define CLASS_B_START ((u32 *) 0x20000040)
  #define CLASS_B_END ((u32 *) 0x20000100)

  /* Constants necessary for RAM test (RAM_END is word aligned ) */
  #define RAM_START (u32 *)0x20000000
  #define RAM_END   (u32 *)0x20004FFC /* last word-aligned address */

  #define REF_CRC16 ((u32)0)/* Dummy value (CRC Not available yet from RVMDK) */

  #define GotoCompilerStartUp() Reset_Handler();

#endif  /* __CC_ARM */

#endif /* __SELFTEST_PARAM_H */
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
