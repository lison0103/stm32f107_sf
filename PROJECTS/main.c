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
u8 g_u8LedFreq = FREQ_1HZ;

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
      Safety_Receive_Data_Process();
#ifdef GEC_SF_S_NEW           
      Safety_Request_DBL2();
#endif          
#endif      
      
      Communication_CPU();
      
#ifdef GEC_SF_MASTER       
      Safety_Comm_Diag();
#endif           
            
      if( Tms10Counter == 0u )
      {
          fault_code_decode(EscRtData.ErrorCode);    
#ifdef GEC_SF_MASTER           
          Communication_To_Control();  
#endif          
      }   
      
      if( Tms20Counter == 0u )
      {                       
          
      }  
        
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
      
#ifdef GEC_SF_MASTER  
      error_change_check();
      StoreFaultInMemory();      
      fram_store_data();
#endif
      
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
    
    /* Power up delay */
    for( i = 0u; i < 10000u; i++ )
    {
               
    }
    
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



