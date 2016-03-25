/*******************************************************************************
* File Name          : spi.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
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
#define buffersize 512

/* Exported functions ------------------------------------------------------- */
void SPI1_Init(void);			 
void SPI1_DMA_Configuration( void );
void SPI1_DMA_ReceiveSendByte( u8 *pbufer, u16 num );
void SPI1_SetSpeed(u8 SpeedSet);   
u8 SPI1_ReadWriteByte(u8 TxData);
void SPI1_WriteByte(u8 TxData);
u8 SPI1_ReadByte(u8 TxData);
void SPI1_NVIC(void);

extern u8 SPI1_TX_Buff[buffersize] ;
extern u8 SPI1_RX_Buff[buffersize] ;
extern u8 SPI_DMA_RECEIVE_FLAG;




#endif


/******************************  END OF FILE  *********************************/



