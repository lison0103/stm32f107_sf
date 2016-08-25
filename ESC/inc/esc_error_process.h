/*******************************************************************************
* File Name          : esc_error_process.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of esc error process functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_ERROR_PROCESS_H
#define __ESC_ERROR_PROCESS_H 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define EN_ERROR_SYS1 EscBuff[0]
#define EN_ERROR_SYS2 EscBuff[1]
#define EN_ERROR_SYS3 EscBuff[2]
#define EN_ERROR_SYS4 EscBuff[3]
#define EN_ERROR_SYS5 EscBuff[4]
/* Exported functions ------------------------------------------------------- */
void ESC_EWDT_Error_Process(void);
void ESC_Flash_Error_Process(void);
void ESC_Fram_Error_Process(void);
void ESC_SPI_Error_Process(void);
void ESC_SafeRelay_Error_Process(void);
void fault_code_decode(u8 code_buff[]);
void ESC_Init_Fault(void);

extern u8 EscBuff[10];


#endif /* __ESC_ERROR_PROCESS_H */


/******************************  END OF FILE  *********************************/


