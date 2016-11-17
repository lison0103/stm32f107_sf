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
/* Exported functions ------------------------------------------------------- */
void ESC_EWDT_Error_Process(void);
void ESC_Flash_Error_Process(void);
void ESC_Fram_Error_Process(void);
void ESC_SPI_Error_Process(void);
void ESC_SafeRelay_Error_Process(void);
void ESC_Init_Fault(void);
void fault_code_decode(u16 code_buff[]);
void fault_code_manual_reset(void);
void fault_code_auto_reset(void);
void error_change_check(void);


#endif /* __ESC_ERROR_PROCESS_H */


/******************************  END OF FILE  *********************************/


