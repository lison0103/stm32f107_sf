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
#define ESC_ERROR_ADR 0u
#define ESC_ERROR_NUM 100u
#define ESC_PARA_ADR 200u
#define ESC_PARA_NUM 200u
#define ESC_BACKUP_ADR 1024u

/* Exported functions ------------------------------------------------------- */
void esc_data_check(void);
u8 fram_data_read(u16 Adr, u16 len, u8 ReadData[]);
void fram_data_write(u16 Adr, u16 len, u8 WriteData[]);
u8 Check_Error_Present_Memory(void);
void StoreFaultInMemory(void);

#endif  /* __ESC_RECORD_DATA_H */


/******************************  END OF FILE  *********************************/



