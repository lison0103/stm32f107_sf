/*******************************************************************************
* File Name          : timer.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of mb85rcxx driver functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MB85RCXX_H
#define __MB85RCXX_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void eep_init(void);
uint8_t eeprom_write(uint16_t addr,u16 len,uint8_t *dat);
uint8_t eeprom_read(uint16_t addr,u16 len,uint8_t *dat);
u8 MB85RCXX_Check(void);  


#endif  /* __MB85RCXX_H */


/******************************  END OF FILE  *********************************/


