/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLcpu.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/01/2007
* Description        : This file contains Start-up Cortex-M3 CPU test function
*                      prototype.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CORTEXM3_CPU_TEST_H
#define __CORTEXM3_CPU_TEST_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
// This is to be sure that tested value is in-line with code returned by the
// with assembly routine.
#define CPUTEST_SUCCESS ((u32)0x00000001uL)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
ErrorStatus STL_StartUpCPUTest(void);
ErrorStatus STL_RunTimeCPUTest(void);

#endif /* __CORTEXM3_CPU_TEST_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
