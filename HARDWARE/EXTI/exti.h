/*******************************************************************************
* File Name          : timer.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of the external interrupt functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXTI_H
#define __EXIT_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void EXTIX_Init(void);

extern u8 g_u8SPISlaveDataPrepare;

#endif  /* __EXTI_H */

/******************************  END OF FILE  *********************************/


