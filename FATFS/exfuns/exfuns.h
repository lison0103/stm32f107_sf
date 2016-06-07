/*******************************************************************************
* File Name          : exfuns.h
* Author             : lison
* Version            : V1.0
* Date               : 03/23/2016
* Description        : This file contains prototype of fatfs externel functions.
*                      
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXFUNS_H
#define __EXFUNS_H 

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "ff.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// Return type definition f_typetell
// Table FILETYPE TBL obtained. Exfuns.c defined inside
#define T_BIN		0X00	//bin
#define T_LRC		0X10	//lrc
#define T_NES		0X20	//nes

#define T_TEXT		0X30	//.txt
#define T_C		0X31	//.c
#define T_H		0X32    //.h

#define T_WAV		0X40	//WAV
#define T_MP3		0X41	//MP3 
#define T_APE		0X42	//APE
#define T_FLAC		0X43	//FLAC

#define T_BMP		0X50	//bmp
#define T_JPG		0X51	//jpg
#define T_JPEG		0X52	//jpeg		 
#define T_GIF		0X53	//gif 

#define T_AVI		0X60	//avi  

/* Exported functions ------------------------------------------------------- */
u8 exfuns_init(void);							
u8 f_typetell(u8 *fname);						
u8 exf_getfree(u8 *drv,u32 *total,u32 *free);	
u32 exf_fdsize(u8 *fdname);	

extern FATFS *fs[1];  
//extern FIL *file;	 	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;

#endif


/******************************  END OF FILE  *********************************/

					   



