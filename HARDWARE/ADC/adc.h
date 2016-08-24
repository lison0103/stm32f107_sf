/*******************************************************************************
* File Name          : adc.h
* Author             : lison
* Version            : V1.0
* Date               : 04/12/2016
* Description        : This file contains prototype of adc conversion functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Adc_Init(void);
u16  Get_Adc(void); 
u8 Get_Adc_Average(void);
	

#endif  /* __ADC_H */

/******************************  END OF FILE  *********************************/


