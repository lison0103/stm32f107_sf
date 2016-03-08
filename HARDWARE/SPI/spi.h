#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

 
#define buffersize 512

#define SPI1_DR_Addr ( (u32)0x4001300C )

extern u8 SPI1_TX_Buff[buffersize] ;
extern u8 SPI1_RX_Buff[buffersize] ;

extern u8 SPI_DMA_RECEIVE_FLAG;

void SPI1_Init(void);			 
void SPI1_DMA_Configuration( void );
void SPI1_DMA_ReceiveSendByte( u16 num );


void SPI1_SetSpeed(u8 SpeedSet);   
u8 SPI1_ReadWriteByte(u8 TxData);

void SPI1_WriteByte(u8 TxData);
u8 SPI1_ReadByte(u8 TxData);
void SPI1_NVIC(void);


#endif

