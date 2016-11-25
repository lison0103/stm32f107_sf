/*******************************************************************************
* File Name          : spi.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : The file contains prototype of spi driver functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H
#define __SPI_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define buffersize 500u

/* Exported functions ------------------------------------------------------- */
void SPI1_Init(void);			 
void SPI1_DMA_ReceiveSendByte( u16 num );
void DMA_Check_Flag(u32 times);
#ifdef GEC_SF_S_NEW 
void SPI1_Configuration(void);
#endif

extern u8 SPI1_TX_Data[buffersize] ;
extern u8 SPI1_RX_Data[buffersize] ;
extern u8 SPI_DMA_RECEIVE_FLAG;




#endif


/******************************  END OF FILE  *********************************/



