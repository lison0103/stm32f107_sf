/*******************************************************************************
* File Name          : esc_comm_safety.h
* Author             : lison
* Version            : V1.0
* Date               : 10/26/2016
* Description        : This file contains prototype of esc safety communication protocol process.                       
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_COMM_SAFETY_H
#define __ESC_COMM_SAFETY_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"
#include "initial_devices.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define CONNECTION_DBL2_UPPER   0x00u  
#define CONNECTION_DBL2_LOWER   0x01u
#define CONNECTION_DBL2_INTERM1   0x02u    
#define CONNECTION_DBL2_INTERM2   0x03u

#define DBL2_TYPE_UPPER   0x01u  
#define DBL2_TYPE_LOWER   0x04u
#define DBL2_TYPE_INTERM1   0x10u    
#define DBL2_TYPE_INTERM2   0x40u

/* Exported functions ------------------------------------------------------- */
void Safety_Send_Data_Process(DBL2Esc *SFData, u8 request);
void Safety_Receive_Data_Process(void);
void Safety_Request_Data(void);
void Safety_Receive_Diagnostic_Validate( DBL2Esc *SFData, u8 DBL2ReceiveData[] );
void Safety_Comm_Data_Init(DBL2Esc *SFData, u8 connection, u8 boardtype, u8 SEQN);

#endif  /* __ESC_COMM_SAFETY_H */


/******************************  END OF FILE  *********************************/



