#include "spi.h"

							  
#ifdef GEC_SF_MASTER
extern u8 Master_Temp[10];
#else
extern u8 Slave_Temp[10];
extern u8 R_SF_RL1_FB_CPU2;
#endif

SPI_InitTypeDef  SPI_InitStructure;

void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
                
 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
        
        SPI_I2S_DeInit(SPI1);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;      //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
#ifdef GEC_SF_MASTER
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                //设置SPI工作模式:设置为主SPI
#else
        SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
#endif
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		        //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                        //数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                        //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);                                     
 
        //DMA 
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
      
//        SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE, ENABLE);
        
        //CRC
        SPI_CalculateCRC(SPI1, ENABLE);
        
	SPI_Cmd(SPI1, ENABLE); 
        
        #ifdef GEC_SF_MASTER
            //发送时钟，开始传输
//            SPI1_ReadWriteByte(0xff);        
        #endif
}   

/** SPI SpeedSet:
SPI_BaudRatePrescaler_2   2 division   (SPI 36M@sys 72M)
SPI_BaudRatePrescaler_8   8 division   (SPI 9M@sys 72M)
SPI_BaudRatePrescaler_16  16 division  (SPI 4.5M@sys 72M)
SPI_BaudRatePrescaler_256 256 division (SPI 281.25K@sys 72M)
**/  
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
} 

/** SPIx read and write a byte
TxData:Bytes to be written
return :Read bytes
**/
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)          //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData);                                         //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)         //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1);                                       //返回通过SPIx最近接收的数据					    
}


void SPI1_WriteByte(u8 TxData)
{  
     u8 retry=0;     
     while((SPI1->SR&1<<1)==0) 
     {
      retry++;
      if(retry>200)return;
     }     
     SPI1->DR=TxData;            
}


u8 SPI1_ReadByte(u8 TxData)
{  
     u8 retry=0;     

     while((SPI1->SR&1<<0)==0)  
     {
      retry++;
      if(retry>200)return 0;
     }             
     return SPI1->DR;                
}

extern u8 Master_Temp[10];

u8 Recive_buf[20] = {0x00};
u8 cnt,flag1,recv_len= 0;

void SPI1_IRQHandler(void)
{
//      if((SPI1->SR&1<<0)==1) 
//     {    
      if (SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE) != RESET)     
      {
          SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
          
                while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
      
      DMA_Cmd(DMA1_Channel3, DISABLE);
      DMA_Cmd(DMA1_Channel2, DISABLE);
#ifdef GEC_SF_MASTER
//          Master_Temp[0] = SPI1_ReadByte(0x00); 
//          SPI1_ReadWriteByte(0xaa); 
#else
//          Recive_buf[cnt++] = SPI1_ReadByte(0x00); 
//          if(Recive_buf[cnt - 3] == 0xAA && Recive_buf[cnt - 2] == 0x55)
//          {
//              flag1 = 1;
//              recv_len = Recive_buf[cnt - 1];
//          }
//          if(flag1)
//          {
//              if(Recive_buf[cnt - 2] == 0x33 && Recive_buf[cnt - 1] == 0xCC)
//              {
//                  flag1 = 0;
//                  SPI1_ReadWriteByte(0xaa);
//              }              
//          }
//          if(cnt > 19)
//          {
//              cnt = 0;
//          }
          Slave_Temp[0] = SPI1_ReadByte(0x00); 
          SPI1_ReadWriteByte(Slave_Temp[1]); 
#endif
     }  
}

void SPI1_NVIC(void)
{
  
      NVIC_InitTypeDef NVIC_InitStructure;
      
      NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
      NVIC_Init(&NVIC_InitStructure);	
      
      NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
      NVIC_Init(&NVIC_InitStructure);      
      
      NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
      NVIC_Init(&NVIC_InitStructure);      
}


/******************************************************************************/



u8 SPI1_TX_Buff[buffersize] = { 0 };
u8 SPI1_RX_Buff[buffersize] = { 0 };

void SPI1_Configuration( void )
{	 
	RCC->APB2ENR |= 1<<2  ;       //PORTA时钟使能   
	RCC->APB2ENR |= 1<<12 ;       //SPI1时钟使能 

	GPIOA->CRL &= 0X000FFFFF ; 
	GPIOA->CRL |= 0XBBB00000 ;    //PA567复用	    
	GPIOA->ODR |= 7<<5 ;          //PA567上拉	
	
	SPI1->CR1 = 0x0000 ;          //SPI1->CR1复位
		
	SPI1->CR1 |= 0<<10 ;          //全双工模式	
	SPI1->CR1 |= 1<<9  ;          //nss软件管理
        
#ifdef GEC_SF_MASTER        
	SPI1->CR1 |= 1<<8  ;          //nss高电平 
	SPI1->CR1 |= 1<<2  ;          //SPI主机
#else
	SPI1->CR1 &= ~(1<<8)  ;       //nss低电平 
	SPI1->CR1 &= ~(1<<2)  ;       //SPI从机
#endif
	SPI1->CR1 |= 0<<11 ;          //数据格式:8bit	
	SPI1->CR1 |= 1<<1  ;          //CPOL=1:空闲模式下SCK为高电平 
	SPI1->CR1 |= 1<<0  ;          //CPHA=1:数据采样从第二个时钟沿开始  
	SPI1->CR1 |= 7<<3  ;          //Fsck=Fcpu/256
	SPI1->CR1 |= 0<<7  ;          //MSBfirst   
	
	SPI1->CR2 |= 1<<1  ;	      //发送缓冲区DMA使能
	SPI1->CR2 |= 1<<0  ;	      //接收缓冲区DMA使能
	
	SPI1->CR1 |= 1<<6  ;          //SPI设备使能		 
} 

  


/*******************************************************************************
* Function Name  : SPI1_DMA_Configuration
* Description    : 配置SPI1_RX的DMA通道2，SPI1_TX的DMA通道3
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI1_DMA_Configuration( void )
{

#if 1  
    DMA_InitTypeDef     DMA_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure.DMA_PeripheralBaseAddr = SPI1_DR_Addr;//设置外设地址，注意PSIZE
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_RX_Buff;//设置DMA存储器地址，注意MSIZE
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //目的
  DMA_InitStructure.DMA_BufferSize = buffersize; //传输数量设置为buffersize个
  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//不执行外设地址增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器地址增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据宽度8bit
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据宽度8bit
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //执行循环操作
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//通道优先级高
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //非存储器到存储器模式
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);  //
  
  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
  
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = SPI1_DR_Addr;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_TX_Buff;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //目的
  DMA_InitStructure.DMA_BufferSize = buffersize; //缓存长度
  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//一个外设
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//缓存地址增加
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //字节传输
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);  //  
  
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
  
#ifndef GEC_SF_MASTER   
//  DMA_Cmd(DMA1_Channel2, DISABLE);
//  DMA1_Channel2->CNDTR = 0x0000;	
//  DMA1_Channel2->CNDTR = 512;
//  DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);
//  SPI1->DR ;
//  DMA_Cmd(DMA1_Channel2, ENABLE);
//            while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//          DMA_Cmd(DMA1_Channel2, DISABLE);
//          DMA_Cmd(DMA1_Channel3, DISABLE);
//          SPI1_ReceiveSendByte(512);
#endif
  
#else
  
    RCC->AHBENR |= 1<<0 ;                     //DMA1时钟使能

	/*------------------配置SPI1_RX_DMA通道Channel2---------------------*/

    DMA1_Channel2->CCR &= ~( 1<<14 ) ;        //非存储器到存储器模式
	DMA1_Channel2->CCR |=    2<<12   ;        //通道优先级高
	DMA1_Channel2->CCR &= ~( 3<<10 ) ;        //存储器数据宽度8bit
	DMA1_Channel2->CCR &= ~( 3<<8  ) ;        //外设数据宽度8bit
	DMA1_Channel2->CCR |=    1<<7    ;        //存储器地址增量模式
	DMA1_Channel2->CCR &= ~( 1<<6  ) ;        //不执行外设地址增量模式
	DMA1_Channel2->CCR &= ~( 1<<5  ) ;        //执行循环操作
	DMA1_Channel2->CCR &= ~( 1<<4  ) ;        //从外设读

	DMA1_Channel2->CNDTR &= 0x0000   ;        //传输数量寄存器清零
	DMA1_Channel2->CNDTR = buffersize ;       //传输数量设置为buffersize个

	DMA1_Channel2->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE
	DMA1_Channel2->CMAR = (u32)SPI1_RX_Buff ; //设置DMA存储器地址，注意MSIZE

	/*------------------配置SPI1_TX_DMA通道Channel3---------------------*/

	DMA1_Channel3->CCR &= ~( 1<<14 ) ;        //非存储器到存储器模式
	DMA1_Channel3->CCR |=    0<<12   ;        //通道优先级最低
	DMA1_Channel3->CCR &= ~( 3<<10 ) ;        //存储器数据宽度8bit
	DMA1_Channel3->CCR &= ~( 3<<8 )  ;        //外设数据宽度8bit
	DMA1_Channel3->CCR |=    1<<7    ;        //存储器地址增量模式
	DMA1_Channel3->CCR &= ~( 1<<6 )  ;        //不执行外设地址增量模式
	DMA1_Channel3->CCR &= ~( 1<<5 ) ;         //不执行循环操作
	DMA1_Channel3->CCR |=    1<<4    ;        //从存储器读

	DMA1_Channel3->CNDTR &= 0x0000   ;        //传输数量寄存器清零
	DMA1_Channel3->CNDTR = buffersize ;       //传输数量设置为buffersize个
	
	DMA1_Channel3->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE
	DMA1_Channel3->CMAR = (u32)SPI1_TX_Buff ; //设置DMA存储器地址，注意MSIZE	
        
#endif
}

void SPI1_ReceiveSendByte( u16 num )
{
  
#if   1
      
      DMA1_Channel2->CNDTR = 0x0000;	
      DMA1_Channel2->CNDTR = num;
      DMA1_Channel3->CNDTR = 0x0000;	
      DMA1_Channel3->CNDTR = num;      
      
      
      DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TC3|DMA1_FLAG_HT3|DMA1_FLAG_TE3);
      DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);
      
      SPI1->DR ;						//接送前读一次SPI1->DR，保证接收缓冲区为空
      
      while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
      
      DMA_Cmd(DMA1_Channel3, ENABLE);
      DMA_Cmd(DMA1_Channel2, ENABLE);
      
//      while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//      
//      DMA_Cmd(DMA1_Channel3, DISABLE);
//      DMA_Cmd(DMA1_Channel2, DISABLE);
     
#else  
      
	DMA1_Channel2->CNDTR = 0x0000   ;           //传输数量寄存器清零
	DMA1_Channel2->CNDTR = num ;         //传输数量设置为buffersize个

	DMA1_Channel3->CNDTR = 0x0000   ;           //传输数量寄存器清零
	DMA1_Channel3->CNDTR = num ;         //传输数量设置为buffersize个

	DMA1->IFCR = 0xF0 ;                         //清除通道3的标志位
	DMA1->IFCR = 0xF00 ;                        //清除通道3的标志位

	SPI1->DR ;									//接送前读一次SPI1->DR，保证接收缓冲区为空

	while( ( SPI1->SR & 0x02 ) == 0 ); //发送缓冲区非空
	
	DMA1_Channel3->CCR |= 1 << 0 ;              //开启DMA通道3
	DMA1_Channel2->CCR |= 1 << 0 ;              //开启DMA通道2	

	while( ( DMA1->ISR & 0x20 ) == 0 ); //通道2,即接收传输完成

	DMA1_Channel3->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道3
	DMA1_Channel2->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道2
#endif
}


u8 flag,number = 0;
void DMA1_Channel2_IRQHandler(void)
{

      if ( ( DMA_GetITStatus( DMA1_IT_TC2 ) ) != RESET )
      {
          DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);
          
          if( SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR) != RESET)
          {
              SPI_I2S_ClearFlag(SPI1, SPI_FLAG_CRCERR);
              //SPI CRC ERROR
              while(1);
          }
          
      #ifdef GEC_SF_MASTER
          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
          DMA_Cmd(DMA1_Channel2, DISABLE);
          DMA_Cmd(DMA1_Channel3, DISABLE);
          
      #else
//          R_SF_RL1_FB_CPU2 = SPI1_RX_Buff[0];
//          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//          DMA_Cmd(DMA1_Channel2, DISABLE);
//          DMA_Cmd(DMA1_Channel3, DISABLE);         
//          
//          
//          SPI1_ReceiveSendByte(512);
          
      #endif
          
          flag = 1;
      }

}

void DMA1_Channel3_IRQHandler(void)
{
      if ( ( DMA_GetITStatus( DMA1_IT_TC3 ) ) != RESET )
      {
        DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TC3|DMA1_FLAG_HT3|DMA1_FLAG_TE3);

//          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  
//          DMA_Cmd(DMA1_Channel3, DISABLE);
//          DMA_Cmd(DMA1_Channel2, DISABLE);   
          
          if( SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR) != RESET)
          {
              SPI_I2S_ClearFlag(SPI1, SPI_FLAG_CRCERR);
              //SPI CRC ERROR
              while(1);
          }
          
          

      }

}

















