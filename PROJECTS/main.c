/*******************************************************************************
* File Name          : main.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Last modify date   : 09/07/2016
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
static void LED_indicator(void);
static void Task_Loop(void);

u32 TimingDelay = 0u;
u32 SysRunTime = 0u; 
u8 testmode = 0u;

/* ESC */
u8 Modbuff[3000];
u8 EscRTBuff[200];
u8 McRxBuff[1000];
u8 *const Sys_Data = &Modbuff[1100];
u16 *const pt_SysBuff = (u16*)&Modbuff[1100];
u8 *const pcOMC_EscRTBuff = &McRxBuff[0]; 
u8 *const pcOmcErrorBuff = &McRxBuff[70];
u8 *const pcErrorBuff = &EscRTBuff[70];
#ifdef GEC_SF_MASTER 
static u8 *const pcEscErrorCodeBuff = &Modbuff[454];
#endif
u8 *const pcEscErrorBuff = &Modbuff[468];
u8 *const pcSafetyInputToControl = &EscRTBuff[4];
u8 *const pcEscDataToControl = &EscRTBuff[12];
u8 *const pcEscDataFromControl = &EscRTBuff[81];
u16 pcOMC_SfBase_EscState = 0u;
/* cpu2 motor speed */
u16 *const g_u16ptCpu2MotorFreqSensor1 = (u16*)&McRxBuff[40];
u16 *const g_u16ptCpu2MotorFreqSensor2 = (u16*)&McRxBuff[42];

/*******************************************************************************
* Function Name  : LED_indicator
* Description    : LED indicator flashes Timing                 
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
static void LED_indicator(void)
{
	static u32 led_idr_cnt = 0u;	 
	
	led_idr_cnt++;
	 
	if(led_idr_cnt >= 100u)   
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
      

      Get_GpioInput(&EscRTBuff[4]);
      
      /*  ESC  */
      if( testmode == 0u )
      {
/*          sfEscStateCheck(); */
          Esc_State_Machine();
          ESC_Tandem_Check();
          ESC_Motor_Check();
          ESC_Handrail_Check();
          ESC_Missingstep_Check();
          SafetyOutputDisable();
          SafetyOutputEnable();
/*          SafetySwitchStatus();*/
      }


      
#ifdef GEC_SF_MASTER 
      if( Tms10Counter == 0u )
      {
          fault_code_decode(pcEscErrorCodeBuff);
      }      
      if( Tms20Counter == 0u )
      {                
          Communication_CPU();
          Communication_To_Control();  
      }  
#else
      if( Tms10Counter == 0u )
      {
          Communication_CPU();         
      }
      if( Tms20Counter == 0u )
      {
         
      }       
#endif     
      
      if( Tms50Counter == 0u )
      {                                 
          /* Reload EWDT counter */          
          EWDT_TOOGLE();
          Safety_Comm_Diag();
          
          SafetyExtWdt_RunCheck();
      } 
      
      if( Tms100Counter == 0u )
      {   
#ifdef GEC_SF_MASTER 
          Comm_DisplayBoard();
          CAN1_TX_Data[2] = SW_SPDT_KEY;
          CAN1_TX_Data[3] = Get_Adc_Average();
#else
          USB_Receive_Data_Send();
#endif
      }
           
      if( Tms500Counter == 0u )
      {             
          Input_Check2();
      }
      
      if( Tms1000Counter == 0u )
      {  

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



