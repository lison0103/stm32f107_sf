/*******************************************************************************
* File Name          : main.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Main program body.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "initial_devices.h"
#include "esc_error_process.h"
#include "safety_test.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 Tms10Counter = 0,Tms20Counter = 0,Tms50Counter = 0,Tms100Counter = 0,Tms500Counter = 0,Tms1000Counter = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 TimingDelay = 0;
u32 SysRunTime = 0; 
u32 timeprintf = 0;

u8 Modbuff[3000];
u8 EscRTBuff[200];
u8 McRxBuff[1000];
u8 *const Sys_Data = &Modbuff[1100];
u16 *const pt_SysBuff = (u16*)&Modbuff[1100];
u8 *const pcOMC_EscRTBuff = &McRxBuff[0]; 

/*******************************************************************************
* Function Name  : LED_indicator
* Description    : LED indicator flashes Timing                 
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void LED_indicator(void)
{
	static u32 led_idr_cnt = 0;	 
	
	led_idr_cnt++;
	
	if( led_idr_cnt >= 100 )
	{
                led_idr_cnt = 0;
		LED =! LED;                
	}   
}


/*******************************************************************************
* Function Name  : Task_Loop
* Description    : This function contains the task to be executed in a loop.               
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void Task_Loop(void)
{          

      if( ++Tms10Counter >= 2 ) Tms10Counter = 0;
      if( ++Tms20Counter >= 4 ) Tms20Counter = 0;
      if( ++Tms50Counter >= 9 ) Tms50Counter = 0;
      if( ++Tms100Counter >= 19 ) Tms100Counter = 0;
      if( ++Tms500Counter >= 99 ) Tms500Counter = 0;
      if( ++Tms1000Counter >= 200 ) Tms1000Counter = 0;      

#if SELF_TEST      
      /* self check */
      Safety_RunCheck1();     
      Safety_RunCheck2();
#endif  
      

      Get_GpioInput(&EscRTBuff[4]);
      
      /*  ESC  */
      sfEscStateCheck();  
      ESC_Tandem_Check();
      ESC_Motor_Check();
      ESC_Handrail_Check();
      ESC_Missingstep_Check();
      SafetyOutputDisable();
      SafetyOutputEnable();
      

      
#ifdef GEC_SF_MASTER 
      if( Tms10Counter == 0 )
      {
        
      }      
      if( Tms20Counter == 0 )
      {                
          Communication_CPU();
          Communication_To_Control();  
      }  
#else
      if( Tms10Counter == 0 )
      {
          Communication_CPU();         
      }
      if( Tms20Counter == 0 )
      {

      }       
#endif     
      
      if( Tms50Counter == 0 )
      {                                 
          /* Reload EWDT counter */          
          EWDT_TOOGLE();
          Safety_Comm_Diag();
          
          SafetyExtWdt_RunCheck();
      } 
      
      if( Tms100Counter == 0 )
      {         
          
      }
           
      if( Tms500Counter == 0 )
      {             
//          Input_Check2();
      }
      
      if( Tms1000Counter == 0 )
      {  
          if( ++timeprintf >= 600 )
          {
              timeprintf = 0;
              printf("%d\r\n",SysRunTime);
          }
      }
     

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

    /** hardware init **/
    Initial_Device();    
    
    while(1)
    {
      
        /* 5ms */
        while ( !TimingDelay );
        TimingDelay = 0;

        Task_Loop();
        LED_indicator();
   
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



