/*******************************************************************************
* File Name          : esc_comm_safety_dualcpu.h
* Author             : lison
* Version            : V1.0
* Date               : 06/20/2016
* Description        : This file contains prototype of esc communication between safety board cpu.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_COMM_SAFETY_DUALCPU_H
#define __ESC_COMM_SAFETY_DUALCPU_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Communication_CPU(void);
void Receive_IO_status_from_CPU(void);
void CPU_Exchange_Data( u8 *buffer, u8 len );
void CPU_Data_Check( u8 *buffer, u8 *len );

#endif  /* __ESC_COMM_SAFETY_DUALCPU_H */


/******************************  END OF FILE  *********************************/



