/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLcrc32.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : This file contains all the functions prototypes for the
*                      ADC firmware library.
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
#ifndef __STM32F10x_STL_CRC32_H
#define __STM32F10x_STL_CRC32_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_lib.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* DR is the read/write data register which holds the 32-bit CRC. Each write in
this register re-computes a CRC with the new data and the previous CRC. The
result is written back into DR. (kind of CRC) */

/* IDR is a 8-bit storage register which allows to keep a test ID or the 8 LSBs
of a previous CRC for instance */

/* CR is the Control Register. It contains the CRC reset bit (other bits are
reserved). The CRC reset value is 0xFFFFFFFF */
//typedef struct
//{
//  vu32  DR;
//  vu8   IDR;
//  vu8   RESERVED0;
//  vu16  RESERVED1;
//  vu32  CR;
//  vu32  RESERVED2;
//} CRC_TypeDef;
//#define CRC                 ((CRC_TypeDef *) (0x40023000uL))

#define CRC_RESET           ((u32)0x00000001uL)

//#define RCC_AHBPeriph_CRC   ((u32)0x00000040uL)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#ifndef GEC_SF_S_NEW
void CRC_DeInit(void);
#endif
void CRC_Init(void);

u32 CRC_CalcBlockCrc(uc32 *p, u32 len);
u32 GetCurrentCRC(void);

void SetIDRegister(u8 Value);
u8 GetIDRegister(void);

#endif /*__STM32F10x_CRC_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
