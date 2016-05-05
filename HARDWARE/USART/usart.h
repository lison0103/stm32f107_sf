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
#define USING_USART3_OVERTIME

#ifndef USART3_BUF_SIZE
#define USART3_BUF_SIZE                512             
#endif

#ifndef USART3_RECEIVE_OVERTIME
#define USART3_RECEIVE_OVERTIME        30              
#endif

#ifndef SYSTEMTICK_PERIOD_MS
#define SYSTEMTICK_PERIOD_MS            1
#endif

/* Exported functions ------------------------------------------------------- */
void BSP_USART_Init(USART_TypeDef* USARTx, uint32_t baud, uint16_t Parity); 
void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);
void BSP_USART_Send(USART_TypeDef* USARTx,uint8_t *buff,uint32_t len);
uint32_t BSP_USART_Receive(USART_TypeDef* USARTx,uint8_t *buff,uint32_t mlen);

void USART3_ISR(void);
void USART_ReceiveOvertimeProcess(void);
extern uint8_t USART3_ready_buf[];
extern __IO uint8_t USART3_ready_buf_ok;
extern __IO uint16_t USART3_ready_buf_len;



#endif  /* __USART_H */

/******************************  END OF FILE  *********************************/



