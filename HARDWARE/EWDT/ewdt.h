/*******************************************************************************
* File Name          : can.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of the external watchdog functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EWDT_H
#define __EWDT_H 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef GEC_SF_MASTER
#define EWDT_TOOGLE() 	GPIOA->ODR ^= 0X0002
#define EWDT_ON     PAout(1)
#else
#define EWDT_TOOGLE() 	GPIOA->ODR ^= 0X0001
#endif

/* Exported functions ------------------------------------------------------- */
void ExtWdtCheck(void);
void EWDT_Drv_pin_config(void);

extern u8 iwdg_check_flag;

#endif  /* __EWDT_H */


/******************************  END OF FILE  *********************************/




