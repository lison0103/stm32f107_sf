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
#define CPU_COMM_TIMEOUT  5u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void CPU_Comm(void);
static void Send_state_to_CPU(void);
static void Receive_state_from_CPU(void);
static void Receive_IO_status_from_CPU(void);

static u16 comm_num = 0u;


#ifdef GEC_SF_MASTER
static u8 onetime = 0u;
#else
static u16 comm_timeout = 100u;
#endif

static u8 cpu_senddata_buffer[250];
static u8 cpu_recvdata_buffer[250];
static u8 recvlen = 0u;

/*******************************************************************************
* Function Name  : Communication_CPU
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Communication_CPU(void)
{
    static u8 comm_cpu_tms = 0u;
    
    CPU_Comm();
    
    comm_cpu_tms++;
#ifdef GEC_SF_MASTER
    if( comm_cpu_tms * ( SYSTEMTICK * 4u ) >= 200u )
    {
        comm_cpu_tms = 0u;
        Receive_IO_status_from_CPU();
    }
#else
    if( comm_cpu_tms * ( SYSTEMTICK * 2u ) >= 200u )
    {
        comm_cpu_tms = 0u;
        Receive_IO_status_from_CPU();
    }    
    
#endif
}

/*******************************************************************************
* Function Name  : Send_state_to_CPU
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Send_state_to_CPU(void)
{
    u8 i;
    
    /* 1. esc Rtdata --------------------------*/
    for( i = 0u; i < 100u; i++)
    {
        cpu_senddata_buffer[i] = EscRTBuff[i];
    }    
    
    /* 2. esc state --------------------------*/        
    cpu_senddata_buffer[100] = (u8)SfBase_EscState;
    cpu_senddata_buffer[101] = (u8)(SfBase_EscState >> 8);

#ifdef GEC_SF_MASTER    
    /* 3. esc para, just for test */
/*
    for( i = 102; i < 162; i++)
    {
        cpu_senddata_buffer[i] = Modbuff[1120 + i - 102];
    }    
*/    
#endif
}

/*******************************************************************************
* Function Name  : Receive_state_from_CPU
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Receive_state_from_CPU(void)
{
    u8 i;
    
#ifdef GEC_SF_MASTER 
    if( recvlen == 102u )       
    {
        /* 1. esc Rtdata receive--------------------------*/
        for( i = 0u; i < 100u; i++)
        {
            McRxBuff[i] = cpu_recvdata_buffer[i];
        }
        
        /* 2. esc state */
        pcOMC_SfBase_EscState = (u16)cpu_recvdata_buffer[101] << 8 ; 
        pcOMC_SfBase_EscState |= (u16)cpu_recvdata_buffer[100];     
    }
#else
    if( recvlen == 102u )       
    {
        /* 1. esc Rtdata receive--------------------------*/
        for( i = 0u; i < 100u; i++)
        {
            McRxBuff[i] = cpu_recvdata_buffer[i];
        }
        
        /* 2. esc state */
        pcOMC_SfBase_EscState = (u16)cpu_recvdata_buffer[101] << 8 ; 
        pcOMC_SfBase_EscState |= (u16)cpu_recvdata_buffer[100]; 
        
        /* 3. esc para receive, just for test --------------*/
/*        
        for( i = 102; i < 162; i++)
        {
            Modbuff[1120 + i - 102] = cpu_recvdata_buffer[i];
        }        
*/        
    }
#endif    
}

/*******************************************************************************
* Function Name  : Receive_IO_status_from_CPU
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Receive_IO_status_from_CPU(void)
{
      static u8 receive_io_error = 0u;
      u8 i;
      
      for( i = 4u; i < 12u; i++ )
      {
          if( McRxBuff[i] != EscRTBuff[i] )
          {
              receive_io_error++;
              break;
          }
      }
      
      if( receive_io_error > 5u )
      {
          NVIC_SystemReset();
      }
      else
      {
          receive_io_error = 0u;
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
static void CPU_Comm(void)
{

#ifdef GEC_SF_MASTER
    if( onetime == 0u )
    {
        onetime++;

        Send_state_to_CPU();
        CPU_Exchange_Data(cpu_senddata_buffer, 102u);
    }
    else
    {
        CPU_Data_Check(cpu_recvdata_buffer, &recvlen);
        Receive_state_from_CPU();
               
        Send_state_to_CPU();        
        CPU_Exchange_Data(cpu_senddata_buffer, 102u);
    }
#else  
    comm_timeout--;

    if( comm_timeout == 0u )
    {
        /* CPU_Comm---comm_timeout */
        EN_ERROR7 |= 0x01u;
        ESC_SPI_Error_Process();
        comm_timeout = CPU_COMM_TIMEOUT;
    }
    if ( ( DMA_GetFlagStatus(DMA1_IT_TC2) ) != RESET )
    {
        
        comm_timeout = CPU_COMM_TIMEOUT;

        CPU_Data_Check(cpu_recvdata_buffer, &recvlen);
        Receive_state_from_CPU();
                
        Send_state_to_CPU();
        CPU_Exchange_Data(cpu_senddata_buffer, 102u);
        
        EN_ERROR7 &= ~0x01u;
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
void CPU_Data_Check( u8 buffer[], u8 *len )
{ 
     u8 i;
     
    /* communication buffer */
    comm_num = buffersize;  
       
    DMA_Check_Flag(10000000u);    
        
    if(!MB_CRC16(SPI1_RX_Data, comm_num))
    {
        
        EN_ERROR_SYS3 = 0u;    
        EN_ERROR7 &= ~0x02u;
        
        *len = SPI1_RX_Data[0];       
        for( i = 0u; i < *len; i++ )
        {
            buffer[i] = SPI1_RX_Data[ i + 1u ];
        }
                  
    }
    else
    {
        EN_ERROR_SYS3++;     
        /* MB_CRC16 error */
                       
    }
    
    if(EN_ERROR_SYS3 > 2u)
    {
        EN_ERROR_SYS3 = 0u;
        ESC_SafeRelay_Error_Process();
        /* CPU_Exchange_Data_Check error */
        EN_ERROR7 |= 0x02u;
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
void CPU_Exchange_Data( u8 buffer[], u8 len )
{   
    u16 i;
    
    /* communication buffer */
    comm_num = buffersize;  
    for(i = 0u; i < comm_num - 2u; i++)
    {
        SPI1_TX_Data[i] = 0u;
    }
    
    SPI1_TX_Data[0] = len;
    for( i = 0u; i < len; i++ )
    {
        SPI1_TX_Data[i + 1u] = buffer[i];
    }
    
    i = MB_CRC16( SPI1_TX_Data, comm_num - 2u );
    SPI1_TX_Data[comm_num - 2u] = (u8)i;
    SPI1_TX_Data[comm_num - 1u] = (u8)(i >> 8u); 
    
    SPI1_DMA_ReceiveSendByte(comm_num);
           
}


/******************************  END OF FILE  *********************************/