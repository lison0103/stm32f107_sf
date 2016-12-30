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
#include "esc_comm_diagnostic2.h"
#include "exti.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CPU_COMM_TIMEOUT  50u
#define CAN_COMM_HAND_TIME      8000u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void CPU_Comm(void);
static void Send_state_to_CPU(void);
static void Receive_state_from_CPU(void);
static void Receive_IO_status_from_CPU(void);



static u16 comm_num = 0u;



static u8 onetime = 0u;
#ifndef GEC_SF_MASTER
static u16 comm_timeout = 100u;
#endif

static u8 *cpu_senddata_buffer;
static u8 *cpu_recvdata_buffer;
static u16 recvlen = 0u;

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
    if( comm_cpu_tms * ( SYSTEMTICK * 2u ) >= 200u )
    {
        comm_cpu_tms = 0u;
        Receive_IO_status_from_CPU();
    }
#else
    if( comm_cpu_tms * ( SYSTEMTICK ) >= 200u )
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
    u8 i,j;
    cpu_senddata_buffer = (u8*)&EscRtData;
    
#ifndef GEC_SF_MASTER    
    for( j = 0u; j < 3u; j++ )
    {
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DataFromControl[j][i] = OmcEscRtData.DataFromControl[j][i];
        }
    }

    /* DBL1 INPUT */
    for( i = 0u; i < 4u; i++ )
    {
        EscRtData.DBL1Upper.InputData[i] = OmcEscRtData.DBL1Upper.InputData[i];
        EscRtData.DBL1Lower.InputData[i] = OmcEscRtData.DBL1Lower.InputData[i];
        EscRtData.DBL1Interm1.InputData[i] = OmcEscRtData.DBL1Interm1.InputData[i];
        EscRtData.DBL1Interm2.InputData[i] = OmcEscRtData.DBL1Interm2.InputData[i];
    }
    
    /* OUTPUTS (DBL UPPER) */
    EscRtData.DBL1Upper.OutputData = EscRtData.DataFromControl[1][0];
    /* OUTPUTS (DBL LOWER) */
    EscRtData.DBL1Lower.OutputData = EscRtData.DataFromControl[1][1];
    /* OUTPUTS (DBL INTERM. 1) */
    EscRtData.DBL1Interm1.OutputData = EscRtData.DataFromControl[1][2];
    /* OUTPUTS (DBL INTERM. 2) */
    EscRtData.DBL1Interm2.OutputData = EscRtData.DataFromControl[1][3];
    
    /* CONTROL STANDARD INPUT (1-8) */
    EscRtData.ControlInputData[0] = EscRtData.DataFromControl[1][4];        
    /* CONTROL STANDARD INPUT (9-16) */ 
    EscRtData.ControlInputData[1] = EscRtData.DataFromControl[1][5];  
    
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
    cpu_recvdata_buffer = (u8*)&OmcEscRtData;
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
      
      for( i = 0u; i < 8u; i++ )
      {
          if( OmcEscRtData.SafetyInputData[i] != EscRtData.SafetyInputData[i] )
          {
              receive_io_error++;
              break;
          }
      }
      
      if( receive_io_error > 5u )
      {
          /* Crosscomp. CPU-CPU-input discrep. F375 */
          EN_ERROR47 |= 0x80u;
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
    static u16 stat_u16TimerCommWait = 0u, stat_u16HandshakeSuccess = 0u;
    u8 i;
    
#ifdef GEC_SF_MASTER
    
    
    static u8 stat_u8TimerSend = 0u;

    stat_u8TimerSend++;
    /* CPU2 data prepare or 10ms, send data */
    if(( g_u8SPISlaveDataPrepare == 1u ) || ( stat_u8TimerSend == 2u ))
    {
        g_u8SPISlaveDataPrepare = 0u;
        stat_u8TimerSend = 0u;
        g_u8ParameterSendToCPU2 = 0u;
        
        Send_state_to_CPU();        
        CPU_Exchange_Data(cpu_senddata_buffer, ESC_RT_DATA_LEN);
        
        Receive_state_from_CPU();
        CPU_Data_Check(cpu_recvdata_buffer, &recvlen, 500u );    

#ifdef DIAGNOSTIC_LEVEL2        
        /* clear receive data */
        for( i = 0u; i < 12u; i++ )
        {
            EscRtData.DBL2Upper.ReceiveDataB[i] = 0u;
            EscRtData.DBL2Lower.ReceiveDataB[i] = 0u; 
            EscRtData.DBL2Interm1.ReceiveDataB[i] = 0u;
            EscRtData.DBL2Interm2.ReceiveDataB[i] = 0u;
        }    
#endif        
    }
#else  

    if( stat_u16HandshakeSuccess == 1u )
    {
        if( --comm_timeout == 0u )
        {
            /* CPU_Comm---comm_timeout */
            /* Timeout CPU1 F371 */
            EN_ERROR47 |= 0x08u;
            comm_timeout = CPU_COMM_TIMEOUT;
            
        }
    }
    else
    {
        stat_u16TimerCommWait++;
        if( ( stat_u16TimerCommWait * SYSTEMTICK ) > CAN_COMM_HAND_TIME )
        {
            /*  can communication handshake timeout when power on */ 
            /* Timeout CPU1 F371 */
            EN_ERROR47 |= 0x08u;
        }         
    }
    
    if( onetime == 0u )
    {
        onetime++;
        
        Send_state_to_CPU();
        CPU_Exchange_Data(cpu_senddata_buffer, ESC_RT_DATA_LEN);
    }
    
    if ( ( DMA_GetFlagStatus(DMA1_IT_TC2) ) != RESET )
    {
        stat_u16HandshakeSuccess = 1u;
        comm_timeout = CPU_COMM_TIMEOUT;

        Receive_state_from_CPU();
        CPU_Data_Check(cpu_recvdata_buffer, &recvlen, 500u );        
                
        Send_state_to_CPU();
        CPU_Exchange_Data(cpu_senddata_buffer, ESC_RT_DATA_LEN);
        
#ifdef DIAGNOSTIC_LEVEL2        
        /* clear send data */
        Safety_Send_Data_Process(&EscRtData.DBL2Upper, 0u);
        Safety_Send_Data_Process(&EscRtData.DBL2Lower, 0u);
        Safety_Send_Data_Process(&EscRtData.DBL2Interm1, 0u);
        Safety_Send_Data_Process(&EscRtData.DBL2Interm2, 0u);
#endif
        
        /* interrupt to CPU1 */
        SYNC_SYS_OUT_SET();
        delay_us(5u);
        SYNC_SYS_OUT_CLR();
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
void CPU_Data_Check( u8 buffer[], u16 *len, u32 times )
{      
     static u8 stat_u8CheckError = 0u;
     u16 i;
   
    /* communication buffer */
    comm_num = buffersize;  

    DMA_Check_Flag(times);    

#if 0    
    if(!MB_CRC16(SPI1_RX_Data, comm_num))
#else
    if(!STM_CRC32( (u32*)&SPI1_RX_Data, ( (u32)comm_num >> 2u )))
#endif        
    {
        
        stat_u8CheckError = 0u;    
        
        *len |= SPI1_RX_Data[0];  
        *len |= (u16)((u16)SPI1_RX_Data[1] << 8u);
        for( i = 0u; i < *len; i++ )
        {
            buffer[i] = SPI1_RX_Data[ i + 2u ];
        }
                  
    }
    else
    {
        stat_u8CheckError++;     
        /* MB_CRC16 error */
#ifdef GEC_SF_S_NEW         
        SPI1_Configuration();               
#endif        
        for(i = 0u; i < comm_num; i++)
        {
            SPI1_RX_Data[i] = 0u;
        }
    }
    
    if(stat_u8CheckError > 3u) /* 2 ? */
    {
        stat_u8CheckError = 0u;
        /* CPU_Exchange_Data_Check error */
        /* Timeout CPU2 F372 */
        EN_ERROR47 |= 0x10u;
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
void CPU_Exchange_Data( u8 buffer[], u16 len )
{   
    u32 i;
    
    /* communication buffer */
    comm_num = buffersize;  
    for(i = 0u; i < comm_num - 2u; i++)
    {
        SPI1_TX_Data[i] = 0u;
    }
    
    SPI1_TX_Data[0] = (u8)(len & 0xffu);
    SPI1_TX_Data[1] = (u8)((len >> 8u)& 0xffu);
    for( i = 0u; i < len; i++ )
    {
        SPI1_TX_Data[i + 2u] = buffer[i];
    }
    
#if 0    
    i = MB_CRC16( SPI1_TX_Data, comm_num - 2u );
    SPI1_TX_Data[comm_num - 2u] = (u8)i;
    SPI1_TX_Data[comm_num - 1u] = (u8)(i >> 8u);       
#else
    i = STM_CRC32( (u32*)&SPI1_TX_Data, (( comm_num - 4u ) >> 2u ) );
    SPI1_TX_Data[comm_num - 1u] = (u8)(i >> 24u);
    SPI1_TX_Data[comm_num - 2u] = (u8)(i >> 16u);
    SPI1_TX_Data[comm_num - 3u] = (u8)(i >> 8u);
    SPI1_TX_Data[comm_num - 4u] = (u8)i;

#endif
    
    SPI1_DMA_ReceiveSendByte(comm_num);
}


/******************************  END OF FILE  *********************************/