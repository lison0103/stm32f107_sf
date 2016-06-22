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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


u8 sflag,inputnum = 0;


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
    
    
    for(u8 i=0;i<4;i++)
    {
        CAN1_TX_Data[i]= 0x0;                 
    }
    
    
    
    if ( !IN1 ) 
    { 
        CAN1_TX_Data[0] |= 1 << 0;                 
    }
    if ( !IN2 ) 
    {       
        CAN1_TX_Data[0] |= 1 << 1;
    }
    if ( !IN3 ) 
    {                   
        CAN1_TX_Data[0] |= 1 << 2; 
        
    }
    if ( !IN4 ) 
    {
        CAN1_TX_Data[0] |= 1 << 3;
    } 
    if ( !IN5 ) 
    {           
        CAN1_TX_Data[0] |= 1 << 4;
    }
    if ( !IN6 ) 
    {          
        CAN1_TX_Data[0] |= 1 << 5;
        
    }
    if ( !IN7 ) 
    {         
        CAN1_TX_Data[0] |= 1 << 6;
        
    }        
    if ( !IN8 ) 
    {         
        CAN1_TX_Data[0] |= 1 << 7;
        
    }
    if ( IN9 ) 
    {     
        CAN1_TX_Data[1] |= 1 << 0;
        
    }
    if ( IN10 ) 
    {                
        CAN1_TX_Data[1] |= 1 << 1;
        
    } 
    if ( IN11 ) 
    {          
        CAN1_TX_Data[1] |= 1 << 2;
        
    }
    if ( IN12 ) 
    {
        CAN1_TX_Data[1] |= 1 << 3;
        
    }
    if ( IN13 ) 
    {   
        CAN1_TX_Data[1] |= 1 << 4; 
        
    }         
    if ( IN14 ) 
    {    
        CAN1_TX_Data[1] |= 1 << 5; 
        
    }
    if ( IN15 ) 
    { 
        CAN1_TX_Data[1] |= 1 << 6;
        
    }
    if ( IN16 ) 
    {
        CAN1_TX_Data[1] |= 1 << 7; 
        
    }    
    if ( IN17 ) 
    {          
        CAN1_TX_Data[2] |= 1 << 0;
        
    }
    if ( IN18 ) 
    {
        CAN1_TX_Data[2] |= 1 << 1;
        
    }
    if ( IN19 ) 
    {   
        CAN1_TX_Data[2] |= 1 << 2;
        
    }         
    if ( IN20 ) 
    {    
        CAN1_TX_Data[2] |= 1 << 3; 
        
    }
    if ( IN21 ) 
    { 
        CAN1_TX_Data[2] |= 1 << 4;
        
    }
    if ( IN22 ) 
    {
        CAN1_TX_Data[2] |= 1 << 5;
        
    } 
    if ( IN23 ) 
    {
        CAN1_TX_Data[2] |= 1 << 6;
        
    }
    if ( IN24 ) 
    {   
        CAN1_TX_Data[2] |= 1 << 7;
        
    }         
    if ( IN25 ) 
    {    
        CAN1_TX_Data[3] |= 1 << 0;
        
    }
    if ( IN26 ) 
    { 
        CAN1_TX_Data[3] |= 1 << 1;
        
    }
    if ( IN27 ) 
    {
        CAN1_TX_Data[3] |= 1 << 2; 
        
    }  
    if ( IN28 ) 
    {
        CAN1_TX_Data[3] |= 1 << 3; 
        
    } 
    
#if EXTERNAL_INPUT_TEST
    
    if ( EX_IN1 )
    {
        inputnum = 29;
        sflag++;
    }
    if ( EX_IN2 ) 
    {       
        inputnum = 30;
        sflag++;
    }
    if ( EX_IN3 ) 
    {                   
        inputnum = 31;
        sflag++;
        
    }
    if ( EX_IN4 ) 
    {
        inputnum = 32;
        sflag++;
    } 
    if ( EX_IN5 ) 
    {           
        inputnum = 33;
        sflag++;
    }
    if ( EX_IN6 ) 
    {          
        inputnum = 34;
        sflag++;
        
    }
    if ( EX_IN7 ) 
    {         
        inputnum = 35;
        sflag++;
        
    }        
    if ( EX_IN8 ) 
    {         
        inputnum = 36;
        sflag++;
        
    }
    if ( EX_IN9 ) 
    {     
        inputnum = 37;
        sflag++;
        
    }
    if ( EX_IN10 ) 
    {                
        inputnum = 38;
        sflag++;
        
    } 
    if ( EX_IN11 ) 
    {          
        inputnum = 39;
        sflag++;
        
    }
    if ( EX_IN12 ) 
    {
        inputnum = 40;
        sflag++;
        
    }
    if ( EX_IN13 ) 
    {   
        inputnum = 41;
        sflag++;
        
    }         
    if ( EX_IN14 ) 
    {    
        inputnum = 42;
        sflag++;
        
    }
    if ( EX_IN15 ) 
    { 
        inputnum = 43;
        sflag++;
        
    }
    if ( EX_IN16 ) 
    {
        inputnum = 44;
        sflag++;
        
    }    
    if ( EX_IN17 ) 
    {          
        inputnum = 45;
        sflag++;
        
    }  
    
#endif
    
    
    
    
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
  
        sflag = 0;
        inputnum = 0;        
        
        if ( !IN1 )
        {
            inputnum = 1;
            sflag++;
        }
        if ( !IN2 ) 
        {       
            inputnum = 2;
            sflag++;
        }
        if ( !IN3 ) 
        {                   
            inputnum = 3;
            sflag++;

        }
        if ( !IN4 ) 
        {
            inputnum = 4;
            sflag++;
        } 
        if ( !IN5 ) 
        {           
            inputnum = 5;
            sflag++;
        }
        if ( !IN6 ) 
        {          
            inputnum = 6;
            sflag++;

        }
        if ( !IN7 ) 
        {         
            inputnum = 7;
            sflag++;

        }        
        if ( !IN8 ) 
        {         
            inputnum = 8;
            sflag++;

        }
        if ( IN9 ) 
        {     
            inputnum = 9;
            sflag++;

        }
        if ( IN10 ) 
        {                
            inputnum = 10;
            sflag++;

        } 
        if ( IN11 ) 
        {          
            inputnum = 11;
            sflag++;

        }
        if ( IN12 ) 
        {
            inputnum = 12;
            sflag++;

        }
        if ( IN13 ) 
        {   
            inputnum = 13;
            sflag++;

        }         
        if ( IN14 ) 
        {    
            inputnum = 14;
            sflag++;

        }
        if ( IN15 ) 
        { 
            inputnum = 15;
            sflag++;

        }
        if ( IN16 ) 
        {
            inputnum = 16;
            sflag++;

        }    
        if ( IN17 ) 
        {          
            inputnum = 17;
            sflag++;

        }
        if ( IN18 ) 
        {
            inputnum = 18;
            sflag++;

        }
        if ( IN19 ) 
        {   
            inputnum = 19;
            sflag++;

        }         
        if ( IN20 ) 
        {    
            inputnum = 20;
            sflag++;

        }
        if ( IN21 ) 
        { 
            inputnum = 21;
            sflag++;

        }
        if ( IN22 ) 
        {
            inputnum = 22;
            sflag++;

        } 
        if ( IN23 ) 
        {
            inputnum = 23;
            sflag++;

        }
        if ( IN24 ) 
        {   
            inputnum = 24;
            sflag++;

        }         
        if ( IN25 ) 
        {    
            inputnum = 25;
            sflag++;

        }
        if ( IN26 ) 
        { 
            inputnum = 26;
            sflag++;

        }
        if ( IN27 ) 
        {
            inputnum = 27;
            sflag++;

        }  
        if ( IN28 ) 
        {
            inputnum = 28;
            sflag++;

        } 

#if EXTERNAL_INPUT_TEST        
        
        if ( EX_IN1 )
        {
            inputnum = 29;
            sflag++;
        }
        if ( EX_IN2 ) 
        {       
            inputnum = 30;
            sflag++;
        }
        if ( EX_IN3 ) 
        {                   
            inputnum = 31;
            sflag++;

        }
        if ( EX_IN4 ) 
        {
            inputnum = 32;
            sflag++;
        } 
        if ( EX_IN5 ) 
        {           
            inputnum = 33;
            sflag++;
        }
        if ( EX_IN6 ) 
        {          
            inputnum = 34;
            sflag++;

        }
        if ( EX_IN7 ) 
        {         
            inputnum = 35;
            sflag++;

        }        
        if ( EX_IN8 ) 
        {         
            inputnum = 36;
            sflag++;

        }
        if ( EX_IN9 ) 
        {     
            inputnum = 37;
            sflag++;

        }
        if ( EX_IN10 ) 
        {                
            inputnum = 38;
            sflag++;

        } 
        if ( EX_IN11 ) 
        {          
            inputnum = 39;
            sflag++;

        }
        if ( EX_IN12 ) 
        {
            inputnum = 40;
            sflag++;

        }
        if ( EX_IN13 ) 
        {   
            inputnum = 41;
            sflag++;

        }         
        if ( EX_IN14 ) 
        {    
            inputnum = 42;
            sflag++;

        }
        if ( EX_IN15 ) 
        { 
            inputnum = 43;
            sflag++;

        }
        if ( EX_IN16 ) 
        {
            inputnum = 44;
            sflag++;

        }    
        if ( EX_IN17 ) 
        {          
            inputnum = 45;
            sflag++;

        }       
#endif        

        if ( inputnum == 0 )
        {
            AUX_CTR = 0; 
            SF_RL_CTR = 0;
        }
        else if ( sflag > 1 )
        {
            AUX_CTR = 0; 
            SF_RL_CTR = 0;
        }
        else if ( inputnum && ( inputnum % 2 ) )
        {
            AUX_CTR = 1; 
        }
        else if ( inputnum )
        {
            SF_RL_CTR = 1; 
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
  
    DMA_Check_Flag(100000);
    
    
        
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
        while(1)
        {
            EWDT_TOOGLE(); 
            IWDG_ReloadCounter();        
        }
    }


}

 




/******************************  END OF FILE  *********************************/




