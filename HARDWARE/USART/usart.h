/*******************************************************************************
* File Name          : usart.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of usart functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H
#define __USART_H	 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
static void BSP_USART_Init(USART_TypeDef* USARTx, uint32_t baud, uint16_t Parity); 
void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);
void BSP_USART_Send(USART_TypeDef* USARTx,uint8_t buff[],uint32_t len);
uint32_t BSP_USART_Receive(USART_TypeDef* USARTx,uint8_t buff[],uint32_t mlen);



#endif  /* __USART_H */

/******************************  END OF FILE  *********************************/



