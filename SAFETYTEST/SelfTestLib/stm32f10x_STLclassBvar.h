/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLclassBvar.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/03/2007
* Description        : Contains all safety critical variables; they must have
*                      predefined addresses and inverse redundant storage
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
#ifndef __STL_CLASS_B_VAR_H
#define __STL_CLASS_B_VAR_H

/* This avoids having mutiply defined global variables */
#ifdef ALLOC_GLOBALS
#define EXTERN
#else
#define EXTERN extern
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define RT_RAM_BLOCKSIZE      ((u32)6)  /* Number of RAM words tested at once */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */

  /* Temporary RAM buffer used during transparent run-time tests */
  /* WARNING: Real reserved RAM location from 0x20000000 to 0x20000024*/
   EXTERN u32 RunTimeRamBuf[RT_RAM_BLOCKSIZE] ;

  /* RAM pointer for run-time tests */
   EXTERN u32 *p_RunTimeRamChk  ;
   EXTERN u32 *p_RunTimeRamChkInv ;

  /* Counter for verifying correct program execution at start */
   EXTERN u32 CtrlFlowCnt;
   EXTERN u32 CtrlFlowCntInv;

  /* Counter for verifying correct program execution in interrupt */
   EXTERN u32 ISRCtrlFlowCnt ;
   EXTERN u32 ISRCtrlFlowCntInv ;

  /* First measure stored as reference for run-time checks */
   EXTERN u32 StartUpClockFreq ;
   EXTERN u32 StartUpClockFreqInv ;

  /* First period measure stored as reference for run-time checks */
   EXTERN u32 RefHSEPeriod ;
   EXTERN u32 RefHSEPeriodInv ;

  /* Last period measure stored as reference for run-time checks */
   EXTERN u32 CurrentHSEPeriod ;
   EXTERN u32 CurrentHSEPeriodInv;

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
   EXTERN u32 TickCounter ;
   EXTERN u32 TickCounterInv ;

  /* Indicates to the main routine a 100ms tick */
   EXTERN vu32 TimeBaseFlag ;
   EXTERN vu32 TimeBaseFlagInv  ;

  /* Stores the Control flow counter from one main loop to the other */
   EXTERN u32 LastCtrlFlowCnt ;
   EXTERN u32 LastCtrlFlowCntInv ;

  /* Pointer to FLASH for crc16 run-time tests */
   EXTERN u8 *p_RunCrc16Chk ;
   EXTERN u8 *p_RunCrc16ChkInv ;

  /* Pointer to FLASH for crc32 run-time tests */
   EXTERN u32 *p_RunCrc32Chk ;
   EXTERN u32 *p_RunCrc32ChkInv  ;

  /* Reference 32-bit CRC for run-time tests */
   EXTERN u32 RefCrc32  ;
   EXTERN u32 RefCrc32Inv;

  /* Current FLASH 16-bit Crc */
   EXTERN u16 CurrentCrc16 ;
   EXTERN u16 CurrentCrc16Inv ;

  /* Magic pattern for Stack overflow in this array */
   EXTERN vu32 StackOverFlowPtrn[4];

#endif  /* __IAR_SYSTEMS_ICC__ */


#ifdef __CC_ARM   /* KEIL Compiler */

  /* Temporary RAM buffer used during transparent run-time tests */
  /* WARNING: Uses RAM location from 0x20000000 to 0x20000020 included */
  EXTERN u32 RunTimeRamBuf[RT_RAM_BLOCKSIZE] __attribute__((section("RUN_TIME_RAM_BUF")));

  /* RAM pointer for run-time tests */
  EXTERN u32 *p_RunTimeRamChk        __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN u32 *p_RunTimeRamChkInv     __attribute__((section("RUN_TIME_RAM_PNT")));

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables would be cleared by the C startup */

  /* Counter for verifying correct program execution at start */
  EXTERN u32 CtrlFlowCnt             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 CtrlFlowCntInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Counter for verifying correct program execution in interrupt */
  EXTERN u32 ISRCtrlFlowCnt          __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 ISRCtrlFlowCntInv       __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* First frequency measure stored as reference for run-time checks */
  EXTERN u32 StartUpClockFreq        __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 StartUpClockFreqInv     __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* First period measure stored as reference for run-time checks */
  EXTERN u32 RefHSEPeriod            __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 RefHSEPeriodInv         __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Last period measure stored as reference for run-time checks */
  EXTERN u32 CurrentHSEPeriod        __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 CurrentHSEPeriodInv     __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  EXTERN u32 TickCounter             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 TickCounterInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Indicates to the main routine a 100ms tick */
  EXTERN vu32 TimeBaseFlag           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN vu32 TimeBaseFlagInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Stores the Control flow counter from one main loop to the other */
  EXTERN u32 LastCtrlFlowCnt         __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 LastCtrlFlowCntInv      __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Pointer to FLASH for crc16 run-time tests */
  EXTERN u8 *p_RunCrc16Chk           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u8 *p_RunCrc16ChkInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Pointer to FLASH for crc32 run-time tests */
  EXTERN u32 *p_RunCrc32Chk          __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 *p_RunCrc32ChkInv       __attribute__((section("CLASS_B_RAM_REV"), zero_init));

/* Reference 32-bit CRC for run-time tests */
  EXTERN u32 RefCrc32                __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u32 RefCrc32Inv             __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Current FLASH 16-bit Crc */
  EXTERN u16 CurrentCrc16            __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN u16 CurrentCrc16Inv         __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Magic pattern for Stack overflow in this array */
  EXTERN vu32 StackOverFlowPtrn[]   __attribute__((section("STACK_BOTTOM"), zero_init));

#endif  /* __CC_ARM */


#endif /* __STL_CLASS_B_VAR_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
