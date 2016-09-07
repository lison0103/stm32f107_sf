/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLlib.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 09/26/2007
* Description        : This file references all header files of the STM32F10x
*                      Self Test Library.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_STL_LIB_H
#define __STM32F10x_STL_LIB_H

/* Includes ------------------------------------------------------------------*/
/* All user customizable parameters */
#include "stm32f10x_STLparam.h"

/* Examples of Self Test library demo routines use */
#include "stm32f10x_STLstartup.h"
#include "stm32f10x_STLmain.h"

/* Cortex-M3 CPU test */
#include "stm32f10x_STLcpu.h"

/* Clock frequency test */
#include "stm32f10x_STLclockstart.h"
#include "stm32f10x_STLclockrun.h"

/* Invariable memory test */
#include "stm32f10x_STLcrc16.h"
#include "stm32f10x_STLcrc32.h"
#include "stm32f10x_STLcrc16Run.h"
#include "stm32f10x_STLcrc32Run.h"

/* Variable memory test */
#include "stm32f10x_STLfullRamMc.h"
#include "stm32f10x_STLtranspRamMc.h"
#include "stm32f10x_STLtranspRamMx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FailSafePOR(void);

extern u32 g_u32InitTestError;
extern u16 g_u16RunTestError;

#endif /* __STM32F10x_STL_LIB_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
