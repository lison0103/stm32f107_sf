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

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define DBL1_UPPER_VALIDATE   0x01u  
#define DBL1_LOWER_VALIDATE   0x02u
#define DBL1_INTERM1_VALIDATE   0x04u    
#define DBL1_INTERM2_VALIDATE   0x08u

/* Exported functions ------------------------------------------------------- */
void Safety_Comm_Diag(void);


#endif  /* __ESC_COMM_DIAGNOSTIC_H */


/******************************  END OF FILE  *********************************/



