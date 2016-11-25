/*******************************************************************************
* File Name          : esc_comm_diagnostic2.h
* Author             : lison
* Version            : V1.0
* Date               : 10/26/2016
* Description        : This file contains prototype of esc safety board communication  with
*                      diagnostic level 2 board.                       
*			          
*******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_COMM_DIAGNOSTIC2_H
#define __ESC_COMM_DIAGNOSTIC2_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"
#include "initial_devices.h"
#include "can.h"
#include "crc16.h"

#ifdef DIAGNOSTIC_LEVEL2
/* Exported types ------------------------------------------------------------*/
typedef struct dbl2comm
{
    /* Comm time wait*/
    u16 TimerCommWait; 
    
    /* hand shake */ 
    u16 HandshakeSuccess;
    
    /* SMCT Timer */
    u16 TimerSMCT;
    
    /* SMVT Timer */
    u16 TimerSMVT;
      
}DBL2Comm;

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
void Safety_Request_DBL2(void);
#ifdef GEC_SF_MASTER
void Send_Data_To_DBL2(void);
void Receive_Data_From_DBL2(void);
#endif

#endif /* DIAGNOSTIC_LEVEL2 */
#endif  /* __ESC_COMM_DIAGNOSTIC2_H */


/******************************  END OF FILE  *********************************/



