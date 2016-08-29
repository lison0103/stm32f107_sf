/*******************************************************************************
* File Name          : initial_devices.h
* Author             : lison
* Version            : V1.0
* Date               : 03/23/2016
* Description        : This file contains prototype of fatfs operation functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FATFS_H
#define __FATFS_H

/* Includes ------------------------------------------------------------------*/
#include "ff.h"  
#include "delay.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 isFileExist(char *filename);
u8 DeleteFile(char *filename);
u8 CopyFile(char *readfilename, char *newfilename);
u8 fatfs_init(void);	
u16 ReadFile(char *readfilename, u8 buffer[]);

extern FATFS *fs; 

#endif /* __FATFS_H */


/******************************  END OF FILE  *********************************/

