/*******************************************************************************
* File Name          : timer.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of stm32 timer funcions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef GEC_SF_S_NEW
#define TIM5    TIM15
#endif

/* Exported functions ------------------------------------------------------- */
void TIM7_Int_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);
void TIM5_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);


#endif  /* __TIMER_H */


/******************************  END OF FILE  *********************************/


