/*******************************************************************************
* File Name          : sys.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of assembly functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_H
#define __SYS_H	

/* Includes ------------------------------------------------------------------*/
#ifdef GEC_SF_S_NEW
#include "stm32f30x.h"
#else
#include "stm32f10x.h"
#endif
#include "stdio.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef GEC_SF_MASTER
#define DEBUG_PRINTF    0
#else
#define DEBUG_PRINTF    0
#endif

/* Exported functions ------------------------------------------------------- */	
void WFI_SET(void);	
void INTX_DISABLE(void);
void INTX_ENABLE(void);	
void MSR_MSP(u32 addr);	


#endif  /* __SYS_H */


/******************************  END OF FILE  *********************************/







