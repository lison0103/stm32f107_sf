/*******************************************************************************
* File Name          : esc_comm_safety_dualcpu.c
* Author             : lison
* Version            : V1.0
* Date               : 06/20/2016
* Description        : This file contains esc communication between safety board cpu.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_comm_safety_dualcpu.h"
#include "spi.h"
#include "can.h"
#include "crc16.h"
#include "esc_error_process.h"
#include "bsp_iocfg.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CPU_COMM_TIMEOUT  5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void CPU_Data_Check(void);
void CPU_Exchange_Data(void);
void CPU_Comm(void);

u16 comm_num = 0;
u8 onetime = 0;
#ifndef GEC_SF_MASTER
static u16 comm_timeout = 100;
#endif


/*******************************************************************************
* Function Name  : Communication_CPU
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Communication_CPU(void)
{
    static u8 comm_cpu_tms = 0;
    
    CPU_Comm();
    
    comm_cpu_tms++;
#ifdef GEC_SF_MASTER
    if( comm_cpu_tms * ( SYSTEMTICK * 4 ) >= 200 )
    {
        comm_cpu_tms = 0;
        Receive_IO_status_from_CPU();
    }
#else
    if( comm_cpu_tms * ( SYSTEMTICK * 2 ) >= 200 )
    {
        comm_cpu_tms = 0;
        Receive_IO_status_from_CPU();
    }    
    
#endif
}

/*******************************************************************************
* Function Name  : Send_state_to_CPU2
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Send_state_to_CPU(void)
{

}

/*******************************************************************************
* Function Name  : Send_state_to_CPU2
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Receive_state_from_CPU(void)
{

}

/*******************************************************************************
* Function Name  : Send_state_to_CPU2
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Receive_IO_status_from_CPU(void)
{
      static u8 receive_io_error = 0;
    
      for( u8 i = 4; i < 12; i++ )
      {
          if( pcOMC_EscRTBuff[i] != EscRTBuff[i] )
          {
              receive_io_error++;
              break;
          }
      }
      
      if( receive_io_error > 5 )
      {
          NVIC_SystemReset();
      }
      else
      {
          receive_io_error = 0;
      }
}

/*******************************************************************************
* Function Name  : CPU_Comm
* Description    : Cpu1 and cpu2 communication.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CPU_Comm(void)
{

#ifdef GEC_SF_MASTER
    if( onetime == 0 )
    {
        onetime++;
        CPU_Exchange_Data();
    }
    else
    {
        CPU_Data_Check();
        CPU_Exchange_Data();
    }
#else  
    comm_timeout--;

    if( comm_timeout == 0 )
    {
        /* CPU_Comm---comm_timeout */
        
        ESC_SPI_Error_Process();
        comm_timeout = CPU_COMM_TIMEOUT;
    }
    if ( ( DMA_GetFlagStatus(DMA1_IT_TC2) ) != RESET )
    {

        comm_timeout = CPU_COMM_TIMEOUT;

        CPU_Data_Check();
        CPU_Exchange_Data();
    }   
#endif
}

/*******************************************************************************
* Function Name  : CPU_Data_Check
* Description    : Check the receive data.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CPU_Data_Check(void)
{ 
   
    
    /* communication buffer */
    comm_num = buffersize;  
    
    
    DMA_Check_Flag(2000);    
    
    
    if(!MB_CRC16(SPI1_RX_Data, comm_num))
    {
        
        EN_ERROR_SYS3 = 0;       
        
        /* esc Rtdata receive--------------------------*/
        for( u8 i = 0; i < 100; i++)
        {
            McRxBuff[i] = SPI1_RX_Data[9 + i];
        }
        
        /* esc parameter */
#ifndef GEC_SF_MASTER
        for( u8 i = 0; i < 100; i++)
        {
            Sys_Data[i] = SPI1_TX_Data[109 + i];
        }
#endif                 
        
    }
    else
    {
        EN_ERROR_SYS3++;     
        /* MB_CRC16 error */
                       
    }
    
    if(EN_ERROR_SYS3 > 2)
    {
        EN_ERROR_SYS3 = 0;
        ESC_SafeRelay_Error_Process();
        /* CPU_Exchange_Data_Check error */
    }
          

             
}

/*******************************************************************************
* Function Name  : CPU_Exchange_Data
* Description    : Cpu send data by spi.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CPU_Exchange_Data(void)
{   
    u16 i;
    
    /* communication buffer */
    comm_num = buffersize;  
    for(i = 0; i < comm_num - 2; i++)
    {
        SPI1_TX_Data[i] = 0;
    }
    
    SPI_DMA_RECEIVE_FLAG = 0;
    
    
    /* esc Rtdata --------------------------*/
    for( i = 0; i < 100; i++)
    {
        SPI1_TX_Data[9 + i] = EscRTBuff[i];
    }
    
    /* esc parameter */
#ifdef GEC_SF_MASTER
    for( i = 0; i < 100; i++)
    {
        SPI1_TX_Data[109 + i] = Sys_Data[i];
    }
#endif          
    
    i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
    SPI1_TX_Data[comm_num - 2] = i;
    SPI1_TX_Data[comm_num - 1] = i>>8; 
    
    SPI1_DMA_ReceiveSendByte(comm_num);
           
}


/******************************  END OF FILE  *********************************/