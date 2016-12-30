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
#include "esc_main_shaft_speed.h"
#ifdef GEC_SF_S_NEW
#include "usb_virtual_com_port.h"
#endif

#include "esc_control.h"


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
/* 5 fault code */
u16 EscErrorCodeBuff[5];
u8 EscErrorBuff[64];

#ifdef GEC_SF_MASTER
/* 4 warn code */
u16 EscWarnCodeBuff[5];
#endif

/* ESC rt data */
SafetyEscData EscRtData;
SafetyEscData OmcEscRtData;

/* Control board data */
u8 EscDataToControl[SF_TO_CONTROL_DATA_LEN][8];

/* DBL1 data */
u8 EscDataToDBL1[SF_TO_DBL1_DATA_LEN][8];
u8 EscDataFromDBL1[DATA_FROM_DBL1_LEN][8];

#ifdef DIAGNOSTIC_LEVEL2
/* DBL2 data */
u8 EscDataToDBL2[SF_TO_DBL2_DATA_LEN][8];
u8 EscDataFromDBL2[DATA_FROM_DBL2_LEN][8];
#endif

/* Parameter data */
u8 ParaDataToControl[2][8];
u8 ParaDataFromControl[5][8];

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
      
      Inspection_Normal_Key_Check();
      
      /*  ESC  */
      if( testmode == 0u )
      {
          Esc_State_Machine();
          
          ESC_Tandem_Check();
          
          ESC_Motor_Check();
          
          ESC_Mainshaft_Check();
                 
          ESC_Handrail_Check();
          
          ESC_Missingstep_Check();
       
          Esc_Safety_Input_Check();
          
          Esc_Control(); 
          
          SafetySwitchStatus();
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
          fault_code_decode(EscRtData.ErrorCode,1u);    
#ifdef GEC_SF_MASTER       
          fault_code_decode(EscWarnCodeBuff,0u);
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
      
      OperationModesProcess();
      
      
      /* Parameters Loading */
      ParametersLoading();
      
#ifdef GEC_SF_MASTER     
      StoreFaultInMemory();          
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



