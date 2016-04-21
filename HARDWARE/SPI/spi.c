/*******************************************************************************
* File Name          : spi.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include "esc_error_process.h"
#include "delay.h"
#include "ewdt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SPI1_DR_Addr ( (u32)0x4001300C )

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void SPI1_DMA_Configuration( void );

u8 SPI1_TX_Buff[buffersize] = { 0 };
u8 SPI1_RX_Buff[buffersize] = { 0 };
u8 SPI1_TX_Data[buffersize] = { 0 };
u8 SPI1_RX_Data[buffersize] = { 0 };
u8 SPI_DMA_RECEIVE_FLAG = 0;
DMA_InitTypeDef     DMA_InitStructure;
static u16 waitus = 0;

/*******************************************************************************
* Function Name  : SPI1_Init
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;        
        SPI_InitTypeDef  SPI_InitStructure;	

#ifdef GEC_SF_MASTER        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
        GPIO_Init(GPIOA, &GPIO_InitStructure);
                
#else
        
#ifdef GEC_SF_S_NEW        
        /* SPI pin mappings */
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_5);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_5);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);
        
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        /* SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
        GPIO_Init(GPIOA, &GPIO_InitStructure);                        
#endif
#endif
           
// 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
        
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
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;	//定义波特率预分频的值:波特率预分频值为32,速度约为72M/32 = 2.25M/s
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                        //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);                                     
 
        //DMA 
        SPI1_DMA_Configuration();       
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
           
        //CRC
        SPI_CalculateCRC(SPI1, ENABLE);
        
        //SPI enable
	SPI_Cmd(SPI1, ENABLE); 
        
        printf("SPI INIT \r\n");
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
      
//      NVIC_InitTypeDef NVIC_InitStructure;   
      
      DMA_DeInit(DMA1_Channel2);
      DMA_InitStructure.DMA_PeripheralBaseAddr = SPI1_DR_Addr;//设置外设地址，注意PSIZE
      DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_RX_Buff;//设置DMA存储器地址，注意MSIZE
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //目的
      DMA_InitStructure.DMA_BufferSize = buffersize; //传输数量设置为buffersize个
      
      DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//不执行外设地址增量模式
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器地址增量模式
      DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据宽度8bit
      DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据宽度8bit
      DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //执行不循环操作
      DMA_InitStructure.DMA_Priority = DMA_Priority_High;//通道优先级高
      DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //非存储器到存储器模式
      DMA_Init(DMA1_Channel2, &DMA_InitStructure);  //
      
//      DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
      
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
      
//      DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
      
      
      
//      NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
//      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
//      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
//      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//      NVIC_Init(&NVIC_InitStructure);      
//      
//      NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
//      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
//      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		
//      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//      NVIC_Init(&NVIC_InitStructure);   
  
}

/*******************************************************************************
* Function Name  : SPI1_DMA_ReceiveSendByte
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_DMA_ReceiveSendByte( u16 num )
{
  
//      while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    
          /* copy data to buff */
          for(u16 i = 0; i < num; i++)
          {
              SPI1_TX_Buff[i] = SPI1_TX_Data[i];
          }    
    
          waitus = 0;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) && ( waitus < 2000 ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= 2000 )
          {
              printf("1 TXE timeout! \r\n");
          }
      
      DMA_Cmd(DMA1_Channel2, DISABLE);
      DMA_Cmd(DMA1_Channel3, DISABLE);      
      
      DMA1_Channel2->CNDTR = 0x0000;	
      DMA1_Channel2->CNDTR = num;
      DMA1_Channel3->CNDTR = 0x0000;	
      DMA1_Channel3->CNDTR = num;      
      
      
      DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TC3|DMA1_FLAG_HT3|DMA1_FLAG_TE3);
      DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);
      
      //接送前读一次SPI1->DR，保证接收缓冲区为空
      SPI1->DR ;						
      
//      while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
          waitus = 0;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) && ( waitus < 2000 ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= 2000 )
          {
              printf("2 TXE timeout! \r\n");
          }
          
      DMA_Cmd(DMA1_Channel2, ENABLE);    
      DMA_Cmd(DMA1_Channel3, ENABLE);
      
          

//    /* DMA channel Tx of SPI Configuration */
//    DMA_InitStructure.DMA_BufferSize = buffersize;
//    DMA_InitStructure.DMA_PeripheralBaseAddr = SPI1_DR_Addr;
//    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_TX_Buff;
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
//    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
//
//    /* Enable the SPI Rx and Tx DMA requests */
//    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
//
//    /* Enable the SPI peripheral */
//    SPI_Cmd(SPI1, ENABLE);
//        
//
//    /* Enable the DMA channels */
//    DMA_Cmd(DMA1_Channel3, ENABLE);      
      
      
     
}

/*******************************************************************************
* Function Name  : DMA_Check_Flag
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_Check_Flag(u32 times)
{         
     
          waitus = 0;
          while( ( !DMA_GetFlagStatus(DMA1_IT_TC2) ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= times )
          {
//              EWDT_TOOGLE(); 
//              IWDG_ReloadCounter();
              printf("DMA1_IT_TC2 wait timeout!!! \r\n");
          }
          waitus = 0;
          while( ( !DMA_GetFlagStatus(DMA1_IT_TC3) ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= times )
          {
//              EWDT_TOOGLE(); 
//              IWDG_ReloadCounter();
              printf("DMA1_IT_TC3 wait timeout!!! \r\n");
          }
          waitus = 0;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= times )
          {
//              EWDT_TOOGLE(); 
//              IWDG_ReloadCounter();              
              printf("SPI_I2S_FLAG_TXE wait timeout!!! \r\n");
          }
          waitus = 0;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= times )
          {
//              EWDT_TOOGLE(); 
//              IWDG_ReloadCounter();              
              printf("SPI_I2S_FLAG_BSY wait timeout!!! \r\n");
          } 
 
        DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TC3|DMA1_FLAG_HT3|DMA1_FLAG_TE3);
        DMA_Cmd(DMA1_Channel3, DISABLE); 
        DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);        
        DMA_Cmd(DMA1_Channel2, DISABLE);                 
              
          if( SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR) != RESET)
          {
              SPI_I2S_ClearFlag(SPI1, SPI_FLAG_CRCERR);
              
              //SPI CRC ERROR
              printf("Channel2 CRCERR \r\n");
              EN_ERROR_SYS4++;
//              SPI1_Init();
              
              if(EN_ERROR_SYS4 > 2)
              {
                ESC_SPI_Error_Process();
              }
          }
          else
          {
              EN_ERROR_SYS4 = 0;
          }        
        
        /* copy buff to data */
        for(u16 i = 0; i < buffersize; i++)
        {
            SPI1_RX_Data[i] = SPI1_RX_Buff[i];
        }         
}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : receive
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{

      if ( ( DMA_GetITStatus( DMA1_IT_TC2 ) ) != RESET )
      {
          DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);
          
          DMA_Cmd(DMA1_Channel2, DISABLE);
          
//      #ifdef GEC_SF_MASTER
//          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET);
//          DMA_Cmd(DMA1_Channel2, DISABLE);
//          DMA_Cmd(DMA1_Channel3, DISABLE);
//          
//      #else
//          
//          if( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR) != RESET)
//          {        
//              printf("SPI_I2S_FLAG_OVR \r\n");
//          }
//      #endif          
          
          
//          if( SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR) != RESET)
//          {
//              SPI_I2S_ClearFlag(SPI1, SPI_FLAG_CRCERR);
//              
//              //SPI CRC ERROR
//              printf("Channel2 CRCERR \r\n");
//              EN_ERROR_SYS4++;
////              SPI1_Init();
//              
//              if(EN_ERROR_SYS4 > 2)
//              {
//                ESC_SPI_Error_Process();
//              }
//          }
//          else
//          {
//              EN_ERROR_SYS4 = 0;
//          }
          
          
          SPI_DMA_RECEIVE_FLAG = 1;
      }

}
/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : send
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
      if ( ( DMA_GetITStatus( DMA1_IT_TC3 ) ) != RESET )
      {
          
          
          
//          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET);
          waitus = 0;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) && ( waitus < 2000 ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= 2000 )
          {
              printf("Channel3 TXE timeout! \r\n");
          }
          waitus = 0;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET ) && ( waitus < 2000 ) )
          {
              waitus++;
              delay_us(1);
          }
          
          if( waitus >= 2000 )
          {
              printf("Channel3 BSY timeout! \r\n");
          }          
          
          DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TC3|DMA1_FLAG_HT3|DMA1_FLAG_TE3);
          
          DMA_Cmd(DMA1_Channel3, DISABLE);                               
      }

}



/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/


/** SPI SpeedSet:
SPI_BaudRatePrescaler_2   2 division   (SPI 36M@sys 72M)
SPI_BaudRatePrescaler_8   8 division   (SPI 9M@sys 72M)
SPI_BaudRatePrescaler_16  16 division  (SPI 4.5M@sys 72M)
SPI_BaudRatePrescaler_256 256 division (SPI 281.25K@sys 72M)
**/  
void SPI1_SetSpeed(u8 SpeedSet)
{
        SPI_InitTypeDef  SPI_InitStructure;
  
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
  
      if (SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE) != RESET)     
      {
          SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
          
                while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
      

#ifdef GEC_SF_MASTER

#else

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
           
}






/******************************  END OF FILE  *********************************/
















