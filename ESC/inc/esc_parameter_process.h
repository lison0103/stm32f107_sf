/*******************************************************************************
* File Name          : esc_parameter_process.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of esc parameter process.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_PARAMETER_PROCESS_H
#define __ESC_PARAMETER_PROCESS_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define MESSAGE_TO_CPU      0x11u
#define MESSAGE_TO_CONTROL      0x22u
#define USB_DETECTED        0x01u
#define USB_NOT_DETECTED        0x02u
#define SAFETY_PARAMETER_LOADED        0x10u
#define CONTROL_PARAMETER_LOADED        0x20u
#define PARAMETER_ERROR        0x03u
#define PARAMETER_CORRECT        0x04u
#define SAFETY_PARAMETER_NOT_EXIST        0x05u
#define CONTROL_PARAMETER_NOT_EXIST        0x06u
#define SAFETY_PARAMETER_EXIST        0x07u
#define CONTROL_PARAMETER_EXIST        0x08u
#define SEND_PARAMETER        0x09u
#define RECEIVE_PARAMETER        0x0Au
#define PARAMETER_LOADED_FINSH        0x0Bu

#define ESC_PARA_ADR 20u
#define ESC_PARA_NUM 100u

/* Exported functions ------------------------------------------------------- */
#ifdef GEC_SF_MASTER
int USB_LoadParameter(void);
#endif
void ParametersLoading(void);
static u8 Send_State_Message(u8 board, u8 state, u8 buff[], u8 len);

extern u8 ParaLoad;

#endif  /* __ESC_PARAMETER_PROCESS_H */


/******************************  END OF FILE  *********************************/



