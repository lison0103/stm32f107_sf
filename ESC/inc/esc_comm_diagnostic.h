/*******************************************************************************
* File Name          : esc_comm_diagnostic.h
* Author             : lison
* Version            : V1.0
* Date               : 06/23/2016
* Description        : This file contains prototype of esc safety board communication  
*                      with diagnostic board.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_COMM_DIAGNOSTIC_H
#define __ESC_COMM_DIAGNOSTIC_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"
#include "initial_devices.h"

/* Exported types ------------------------------------------------------------*/
typedef struct dbl1comm
{
    /* DBL1 type */
    u8 Type;
    
    /* Comm time wait*/
    u16 TimerCommWait; 
    
    /* hand shake */ 
    u16 HandshakeSuccess;
    
    /* Comm Timeout */
    u16 CommTimeout;
    
      
}DBL1Comm;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define DBL1_TYPE_UPPER   0x01u  
#define DBL1_TYPE_LOWER   0x02u
#define DBL1_TYPE_INTERM1   0x04u    
#define DBL1_TYPE_INTERM2   0x08u

/* Exported functions ------------------------------------------------------- */
void Safety_Comm_Diag(void);


#endif  /* __ESC_COMM_DIAGNOSTIC_H */


/******************************  END OF FILE  *********************************/



