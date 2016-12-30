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
/* failure lock */
#define EF       2u
/* standard fault */
#define ES       1u
/* recovery fault */
#define ER       3u
/* Undefined */
#define EU       0x0u

/* Exported functions ------------------------------------------------------- */
void fault_code_decode(u16 code_buff[],u8 fault);
void fault_code_manual_reset(u8 ResetType);
void fault_code_auto_reset(void);
void error_change_check(void);

extern u8 EscFaultType[512];

#endif /* __ESC_ERROR_PROCESS_H */


/******************************  END OF FILE  *********************************/


