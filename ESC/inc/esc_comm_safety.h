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

#define DBL2_UPPER_VALIDATE   0x01u  
#define DBL2_LOWER_VALIDATE   0x02u
#define DBL2_INTERM1_VALIDATE   0x04u    
#define DBL2_INTERM2_VALIDATE   0x08u

/* Exported functions ------------------------------------------------------- */
void Safety_Send_Data_Process(u8 connection, u8 *SEQN, u8 DBL2SendData[], u8 request);
void Safety_Receive_Data_Process(void);
void Safety_Request_Data(void);
void Safety_ReceiveB_Diagnostic(u8 Connection, u8 SEQN, u8 DBL2Type, u8 DBL2InputData[], u8 DBL2ReceiveData[] );

#ifdef GEC_SF_MASTER
extern u8 SEQN_UPPER_A,SEQN_LOWER_A,SEQN_INTERM1_A,SEQN_INTERM2_A;
#else

#endif

#endif  /* __ESC_COMM_SAFETY_H */


/******************************  END OF FILE  *********************************/



