/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLstartup.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/01/2007
* Description        : This file contains the prototype of the FailSafe routines
*                      and the very first elf-test function to be executed after
*                      the reset.
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
#ifndef __SELF_TEST_START_UP_H
#define __SELF_TEST_START_UP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Safety_StartupCheck1(void);
void FailSafe(void);
void FailSafeCPU(void);
void STL_WDGSelfTest(void);


#endif /* __SELF_TEST_START_UP_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
