/*******************************************************************************
* File Name          : spi.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : The file contains spi driver functions.
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
static void SPI1_DMA_Configuration( void );


static u8 SPI1_TX_Buff[buffersize] = { 0 };
static u8 SPI1_RX_Buff[buffersize] = { 0 };
u8 SPI1_TX_Data[buffersize] = { 0 };
u8 SPI1_RX_Data[buffersize] = { 0 };
static DMA_InitTypeDef     DMA_InitStructure;
static u16 waitus = 0u;



/*******************************************************************************
* Function Name  : SPI1_Configuration
* Description    : Configure spi1 register.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Configuration(void)
{       
        SPI_InitTypeDef  SPI_InitStructure;	
        
        SPI_I2S_DeInit(SPI1);

        /* Setting unidirectional or bidirectional SPI data mode: SPI is set to double two-way full-duplex */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;      
#ifdef GEC_SF_MASTER
        /* Setting SPI mode: setting master SPI */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                
#else
        /* Setting SPI mode: setting slave SPI */
        SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
#endif
        /* SPI data set size: SPI transmission and reception of 8-bit frame structure */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		        
        /* Select the serial clock Steady: Clock floating high */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		              
        /* Data capture on the second clock edge */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                        
        /* NSS signal by hardware (NSS pin) or software (using SSI bit) management: internal NSS-bit control signal has SSI */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                
        /* Defines the baud rate prescaler values: Baud Rate Prescaler is 32, speed is about 72M / 32 = 2.25M / s */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;	
        /* Specifies the data transmission from the MSB or LSB bit first: data transmission start from the MSB */
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
        /* CRC polynomial value calculation */
	SPI_InitStructure.SPI_CRCPolynomial = 7u;	                        
	SPI_Init(SPI1, &SPI_InitStructure);   
        
#ifdef GEC_SF_S_NEW
        SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
#endif

        /* DMA */ 
        SPI1_DMA_Configuration();       
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
           
        /* CRC */
        /*SPI_CalculateCRC(SPI1, ENABLE);*/
        
        /* SPI enable */
	SPI_Cmd(SPI1, ENABLE); 
        
} 


/*******************************************************************************
* Function Name  : SPI1_Init
* Description    : Intialization SPI1 .
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;        	

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
    
        SPI1_Configuration();
}    


/*******************************************************************************
* Function Name  : SPI1_DMA_Configuration
* Description    : Configuring SPI1_RX DMA channel 2, SPI1_TX DMA channel 3
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void SPI1_DMA_Configuration( void )
{  
        
      
      DMA_DeInit(DMA1_Channel2);
      DMA_InitStructure.DMA_PeripheralBaseAddr = SPI1_DR_Addr;
      /* Set the DMA memory address */
      DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_RX_Buff;
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
      /* A transmission number is set to buffersize */
      DMA_InitStructure.DMA_BufferSize = buffersize; 
      
      DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
      DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
      DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
      DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
      DMA_InitStructure.DMA_Priority = DMA_Priority_High;
      DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
      DMA_Init(DMA1_Channel2, &DMA_InitStructure);  
      
      
      DMA_DeInit(DMA1_Channel3);
      DMA_InitStructure.DMA_PeripheralBaseAddr = SPI1_DR_Addr;
      DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SPI1_TX_Buff;
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 
      DMA_InitStructure.DMA_BufferSize = buffersize; 
      
      DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
      DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
      DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
      DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
      DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
      DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
      DMA_Init(DMA1_Channel3, &DMA_InitStructure);         
  
}

/*******************************************************************************
* Function Name  : SPI1_DMA_ReceiveSendByte
* Description    : Enable SPI1 DMA send and receive data.
* Input          : num: the number of send and receive data.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_DMA_ReceiveSendByte( u16 num )
{
    u16 i;
    
    /* copy data to buff */
    for( i = 0u; i < num; i++)
    {
        SPI1_TX_Buff[i] = SPI1_TX_Data[i];
    }    
    
    waitus = 0u;
    while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) && ( waitus < 2000u ) )
    {
        waitus++;
        delay_us(1u);
    }
    
    if( waitus >= 2000u )
    {
        /* TXE timeout!  */
        waitus = 0u;
    }
      
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);      
    
    DMA1_Channel2->CNDTR = 0x0000u;	
    DMA1_Channel2->CNDTR = num;
    DMA1_Channel3->CNDTR = 0x0000u;	
    DMA1_Channel3->CNDTR = num;      
    
    
    DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TC3|DMA1_FLAG_HT3|DMA1_FLAG_TE3);
    DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);
      
    /* Read once before the shuttle SPI1-> DR, ensure that the receiving buffer is empty */
    SPI1->DR ;						
      

    waitus = 0u;
    while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) && ( waitus < 2000u ) )
    {
        waitus++;
        delay_us(1u);
    }
    
    if( waitus >= 2000u )
    {
        /* TXE timeout!  */
        waitus = 0u;
    }
    
    DMA_Cmd(DMA1_Channel2, ENABLE);    
    DMA_Cmd(DMA1_Channel3, ENABLE);
      
    
}

/*******************************************************************************
* Function Name  : DMA_Check_Flag
* Description    : Check the DMA flag register.
*                  Judging whether data transmission and reception is completed
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_Check_Flag(u32 times)
{         
          static u8 stat_u8CheckError = 0u;
          u16 i;
          
          waitus = 0u;
          /* 10us */
          while( ( !DMA_GetFlagStatus(DMA1_IT_TC2) ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1u);
              EWDT_TOOGLE();
              IWDG_ReloadCounter();  
          }
          
          if( waitus >= times )
          {
              /* DMA1_IT_TC2 wait timeout!!! */
              waitus = 0u;
          }
          waitus = 0u;
          while( ( !DMA_GetFlagStatus(DMA1_IT_TC3) ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1u);
          }
          
          if( waitus >= times )
          {
              /* DMA1_IT_TC3 wait timeout!!! */
              waitus = 0u;
          }
          waitus = 0u;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1u);
          }
          
          if( waitus >= times )
          {              
              /* SPI_I2S_FLAG_TXE wait timeout!!! */
              waitus = 0u;
          }
          waitus = 0u;
          while( ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET ) && ( waitus < times ) )
          {
              waitus++;
              delay_us(1u);
          }
          
          if( waitus >= times )
          {             
              /* SPI_I2S_FLAG_BSY wait timeout!!! */
              waitus = 0u;
          } 
 
        DMA_ClearFlag(DMA1_FLAG_GL3|DMA1_FLAG_TC3|DMA1_FLAG_HT3|DMA1_FLAG_TE3);
        DMA_Cmd(DMA1_Channel3, DISABLE); 
        DMA_ClearFlag(DMA1_FLAG_GL2|DMA1_FLAG_TC2|DMA1_FLAG_HT2|DMA1_FLAG_TE2);        
        DMA_Cmd(DMA1_Channel2, DISABLE);                 
              
          if( SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR) != RESET)
          {
              SPI_I2S_ClearFlag(SPI1, SPI_FLAG_CRCERR);
              
              /* SPI CRC ERROR */
              stat_u8CheckError++;
#ifdef GEC_SF_MASTER          
/*
              SPI1_Configuration();
              delay_ms(200);
*/              
#else
              SPI1_Configuration();
#endif              
              if(stat_u8CheckError > 2u)
              {
                ESC_SPI_Error_Process();
              }
          }
          else
          {
              stat_u8CheckError = 0u;
          }        
        
        /* copy buff to data */
        for( i = 0u; i < buffersize; i++)
        {
            SPI1_RX_Data[i] = SPI1_RX_Buff[i];
        }         
}




/******************************  END OF FILE  *********************************/
















