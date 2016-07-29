/*******************************************************************************
* File Name          : esc_record_data.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of esc record and store data.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_RECORD_DATA_H
#define __ESC_RECORD_DATA_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void esc_data_check(void);
u8 fram_data_read(u8 Adr, u8 len, u8 *ReadData);
void fram_data_write(u8 Adr, u8 len, u8 *WriteData);

#endif  /* __ESC_RECORD_DATA_H */


/******************************  END OF FILE  *********************************/



