/*******************************************************************************
* File Name          : hw_test.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains some test function.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "hw_test.h"
#include "led.h"
#include "can.h"
#include "spi.h"
#include "ewdt.h"
#include "esc_error_process.h"
#include "crc16.h"
#include "safety_test.h"
#include "esc.h"

#ifdef GEC_SF_MASTER
#include "usart.h"
#include "crc16.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 sflag = 0,inputnum = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : Input_Check
* Description    : Monitor the input pin status test.
*                   
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Input_Check(void)
{  
    
    u32 *ulPt_Input,*pc_can_tx;
    u8 i;
    
    ulPt_Input = (u32*)&EscRTBuff[4];
    pc_can_tx = (u32*)&CAN1_TX_Data[0];
    
    for( u8 i = 0; i < 4; i++ )
    {
        CAN1_TX_Data[i]= 0x0;                 
    }
    
    for( i = 0; i < 28; i++ )
    {
        if( ulPt_Input[0] & ( 1 << i ))
        {
            *pc_can_tx |= 1 << i;
        }
    } 

}        
        

/*******************************************************************************
* Function Name  : Input_Check2
* Description    : Monitor the input pin status and test.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Input_Check2(void)
{
  
        u32 *ulPt_Input1,*ulPt_Input2;
        u8 i;
        
        ulPt_Input1 = (u32*)&EscRTBuff[4];       
        ulPt_Input2 = (u32*)&EscRTBuff[8];
        sflag = 0;
        inputnum = 0;        
        
        
        for( i = 0; i < 28; i++ )
        {
            if( *ulPt_Input1 & ((u32)( 1 << i )))
            {
                sflag++;
                inputnum = i + 1;
            }
        }
        
        for( i = 0; i < 17; i++ )
        {
            if( *ulPt_Input2 & ((u32)( 1 << i )))
            {
                sflag++;
                inputnum = i + 29;
            }
        }        
                       

        if (( inputnum == 0 ) || ( sflag > 1 ))
        {
            SF_RELAY_OFF(); 
            AUX_RELAY_OFF();
        }
        else if ( inputnum && ( inputnum % 2 ) )
        {
            SF_RELAY_ON(); 
        }
        else if ( inputnum )
        {
            AUX_RELAY_ON(); 
        }
       
}


/*******************************************************************************
* Function Name  : CrossCommCPUCheck
* Description    : CrossCommCPU and Data Integrity test
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CrossCommCPUCheck(void)
{    

    u16 i = 0;  
    u8 number = 0;
    u8 data_error = 0;
    u32 test_cnt = 100;  
    u16 comm_num = buffersize;
  
#ifdef GEC_SF_MASTER      
    u8 result = 0;
#endif
        
    number++;

    for( i = 0; i < comm_num; i++)
    {
        SPI1_TX_Data[i] = number;
    }
    
    i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
    SPI1_TX_Data[comm_num - 2] = i;
    SPI1_TX_Data[comm_num - 1] = i>>8;
    
    
    SPI1_DMA_ReceiveSendByte(comm_num);

#ifdef GEC_SF_MASTER
    DMA_Check_Flag(100000);
#else
    DMA_Check_Flag(100000000);
#endif
    
        
        while(test_cnt--)
        {
            
            EWDT_TOOGLE(); 
            IWDG_ReloadCounter();             
        
#ifdef GEC_SF_MASTER              
              
              delay_ms(2);
              
              number++;
              if(number >= 255)number = 0;

              for( i = 0; i < comm_num - 2; i++ )
              {
                    SPI1_TX_Data[i] = number;
              }
              
              i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
              SPI1_TX_Data[comm_num - 2] = i;
              SPI1_TX_Data[comm_num - 1] = i>>8;
              
              SPI1_DMA_ReceiveSendByte(comm_num);
                        
              DMA_Check_Flag(40000);
              
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {

                  for( i=0; i < comm_num - 2; i++ )
                  {
                      result = SPI1_RX_Data[i]^(SPI1_TX_Data[i] - 1);
                      if( result )
                      {
                          data_error++;                            
                          break;

                      }
                  
                  }
              } 
              else
              {
                  data_error++;                 
  
              }
              
              if( data_error > 2 )
              {
                  break;
              }               
              
              
#else
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {                 
                  for( i=0; i < comm_num - 2; i++ )
                  {
                      SPI1_TX_Data[i] = SPI1_RX_Data[i];
                  }   
                  
                  i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
                  SPI1_TX_Data[comm_num - 2] = i;
                  SPI1_TX_Data[comm_num - 1] = i>>8;
              } 
              else
              {
                  data_error++;
                  if( data_error > 2 )
                  {
                      break;
                  }
              }
              
              SPI1_DMA_ReceiveSendByte(comm_num);

              DMA_Check_Flag(40000);
#endif              
        }      
        

#ifdef GEC_SF_MASTER 
#else
        SPI1_DMA_ReceiveSendByte(comm_num);
#endif
    if( data_error > 2 )
    {
        LED = 0;
        /* SPI1_DMA_Check error */
        FailSafeTest();
//        while(1)
//        {
//            EWDT_TOOGLE(); 
//            IWDG_ReloadCounter();        
//        }
    }


}

 




/******************************  END OF FILE  *********************************/




