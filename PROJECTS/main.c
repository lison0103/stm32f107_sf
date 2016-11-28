/*******************************************************************************
* File Name          : main.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Last modify date   : 09/27/2016
* Description        : Main program body.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "initial_devices.h"
#include "esc_error_process.h"
#include "safety_test.h"
#include "stm32f10x_STLlib.h"
#include "esc_motor_speed.h"
#include "esc_handrail_speed.h"
#include "esc_missing_step.h"
#include "bsp_input.h"
#include "esc_comm_safety_dualcpu.h"
#include "esc_comm_control.h"
#include "esc_safety_check.h"
#include "esc_cmd_state.h"
#include "esc_tandem.h"
#include "esc_comm_diagnostic.h"
#include "comm_display_board.h"
#include "esc_state.h"
#include "esc_comm_diagnostic2.h"
#ifdef GEC_SF_S_NEW
#include "usb_virtual_com_port.h"
#endif
#include "mb85rcxx.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 Tms10Counter = 0u,Tms20Counter = 0u,Tms50Counter = 0u,Tms100Counter = 0u,Tms500Counter = 0u,Tms1000Counter = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void LED_indicator(u8 freq);
static void Task_Loop(void);

u32 TimingDelay = 0u;
u32 SysRunTime = 0u; 
u8 testmode = 0u;
u8 g_u8CanCommunicationToCotrolID = 0u,g_u8CanCommunicationToCotrolLen = 0u,g_u8CanCommunicationToCotrolOk = 1u;
u8 g_u8LedFreq = FREQ_0_5HZ;

/* ESC -----------------------*/
/* 5 fault code, 1 alarm code */
u16 EscErrorCodeBuff[6];
u8 EscErrorBuff[64];

#ifdef GEC_SF_MASTER
/* state for display */
u8 StateDisplay[8];
#endif

/* ESC rt data */
SafetyEscData EscRtData;
SafetyEscData OmcEscRtData;

/* Control board data */
u8 EscDataToControl[20][8];
u8 EscDataFromControl[3][8];

/* DBL1 data */
u8 EscDataToDBL1[3][8];
u8 EscDataFromDBL1[4][8];

/* DBL2 data */
u8 EscDataToDBL2[8][8];
u8 EscDataFromDBL2[16][8];


/*******************************************************************************
* Function Name  : LED_indicator
* Description    : LED indicator flashes Timing                 
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
static void LED_indicator(u8 freq)
{
	static u32 led_idr_cnt = 0u;	 
	
	led_idr_cnt++;
	 
	if(led_idr_cnt >= freq)   
	{
            led_idr_cnt = 0u;
            LED_FLASH();       
	}   
}


/*******************************************************************************
* Function Name  : Task_Loop
* Description    : This function contains the task to be executed in a loop.               
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
static void Task_Loop(void)
{          

    if( ++Tms10Counter >= 2u ){ Tms10Counter = 0u;}
    if( ++Tms20Counter >= 4u ){ Tms20Counter = 0u;}
    if( ++Tms50Counter >= 9u ){ Tms50Counter = 0u;}
    if( ++Tms100Counter >= 19u ){ Tms100Counter = 0u;}
    if( ++Tms500Counter >= 99u ){ Tms500Counter = 0u;}
    if( ++Tms1000Counter >= 200u ){ Tms1000Counter = 0u;}      

#if SELF_TEST      
      /* self check */
      Safety_RunCheck1();     
      Safety_RunCheck2();
#endif  
      

      Get_GpioInput(&EscRtData.SafetyInputData[0]);
      
      pga_input_decode();
      
      
      /*  ESC  */
      if( testmode == 0u )
      {
          Esc_State_Machine();
          
          ESC_Tandem_Check();
          
          ESC_Motor_Check();
          
          ESC_Handrail_Check();
          
          ESC_Missingstep_Check();
          
          Esc_Safety_Input_Check();
          
          Esc_Control();
          
/*          SafetySwitchStatus();*/
      }
      
      
#ifdef DIAGNOSTIC_LEVEL2      
      if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
      {
          Safety_Receive_Data_Process();
#ifdef GEC_SF_S_NEW           
          Safety_Request_DBL2();
#endif          
      }
#endif      

      
#ifdef GEC_SF_MASTER 
      Communication_CPU();
      Safety_Comm_Diag();
#else     
      Communication_CPU();
#endif
      
      
      
      
#ifdef GEC_SF_MASTER 
      if( Tms10Counter == 0u )
      {
          fault_code_decode(EscRtData.ErrorCode);               
          /*Communication_CPU();*/
          
          Communication_To_Control();  
      }      
      if( Tms20Counter == 0u )
      {                       
          
      }  
#else
      if( Tms10Counter == 0u )
      { 
          fault_code_decode(EscRtData.ErrorCode);

      }
      if( Tms20Counter == 0u )
      {
         
      }       
#endif     
      
      if( Tms50Counter == 0u )
      {                                 
          /* Reload EWDT counter */          
          EWDT_TOOGLE();
          
          /*SafetyExtWdt_RunCheck();*/
      } 
      
      if( Tms100Counter == 0u )
      {   
#ifdef GEC_SF_MASTER 
          Comm_DisplayBoard();
          SW_SPDT_KEY;         
#else
          USB_Receive_Data_Send();
#endif
      }
           
      if( Tms500Counter == 0u )
      {             
          Input_Check();
      }
      
      if( Tms1000Counter == 0u )
      {  

      }
     

}

u8 testinput[5] = {1u,2u,3u,4u};
typedef struct rty
{
    /* input data */
    u8 (*InputData)[5];


      
}RTY;

RTY rrrtt = {&testinput};


void STM_CRC_Init(void)
{
  /* Enable CRC module clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC ,ENABLE);

  /* Reset CRC generator */
  CRC->CR = CRC_RESET;
}

uint32_t STM_CRC32(uint32_t pBuffer[], uint32_t BufferLength)
{
  uint32_t index = 0;
  
  for(index = 0; index < BufferLength; index++)
  {
    CRC->DR = pBuffer[index];
  }
  return (CRC->DR);
}


u32 revbit(u32 data)  
{  
  asm("rbit r0,r0");  
  return data;  
};  
#define CRC32_POLYNOMIAL                        ((uint32_t)0xEDB88320)
/*================================================================== 
* Function  : CRC32_ForBytes 
* Description   : CRC32输入为8bits buffer的指针及长度 
* Input Para    :  
* Output Para   :  
* Return Value:  
==================================================================*/  
u32 CRC32_ForBytes(u8 *pData,u32 uLen)  
{  
    u32 uIndex= 0,uData = 0,i;  
    uIndex = uLen >> 2;  
      
    STM_CRC_Init(); 
        
    while(uIndex--)  
    {    
        memcpy((u8*)&uData,pData,4);  
        
        pData += 4;  
        uData = revbit(uData);  
        CRC->DR = uData;  
    }  
    uData = revbit(CRC->DR);  
    uIndex = uLen & 0x03;  
    while(uIndex--)  
    {  
        uData ^= (u32)*pData++;  
        for(i = 0;i < 8;i++)  
          if (uData & 0x1)  
            uData = (uData >> 1) ^ CRC32_POLYNOMIAL;  
          else  
            uData >>= 1;  
    }  
    return uData^0xFFFFFFFF;  
} 
/*******************************************************************************
* Function Name  : main
* Description    : Main program.             
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{        

    u16 i; 
    u32 crc,len;
    
    u8 testlen[100] = {0u};
    
    len = 100u;
    crc = MB_CRC32( testlen, len - 4u, 0x04C11DB7 );
    crc = CRC32_ForBytes( testlen, len - 4u );
    testlen[len - 4u] = (u8)(crc >> 24u);
    testlen[len - 3u] = (u8)(crc >> 16u);     
    testlen[len - 2u] = (u8)(crc >> 8u);
    testlen[len - 1u] = (u8)crc;
    
    crc = CRC32_ForBytes( testlen, len );
    
    
    *rrrtt.InputData[0u] = 5u;
    /* Power up delay */
    for( i = 0u; i < 10000u; i++ )
    {
               
    }
    eep_write(0x03u);
    /** hardware init **/
    Initial_Device();    
    
    while(1)
    {
      
        /* 5ms */
        while ( !TimingDelay )
        {
        }
        TimingDelay = 0u;

        Task_Loop();
        LED_indicator(g_u8LedFreq);
   
    }          
          
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/******************************  END OF FILE  *********************************/



