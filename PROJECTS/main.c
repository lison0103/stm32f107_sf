/*******************************************************************************
* File Name          : main.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "initial_devices.h"
#include "esc_error_process.h"
#include "safety_test.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 Tms10Counter=0,Tms20Counter=0,Tms50Counter=0,Tms100Counter=0,Tms500Counter=0,Tms1000Counter=0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 TimingDelay = 0;
 

/*******************************************************************************
* Function Name  : LED_indicator
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void LED_indicator(void)
{
	static u32 led_idr_cnt=0;	 
	
	led_idr_cnt++;
	
	if(led_idr_cnt >= 100 && data_error <= 0)
	{
                led_idr_cnt = 0;
		LED=!LED;                
	}   
}


/*******************************************************************************
* Function Name  : Task_Loop
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void Task_Loop(void)
{          

      if( ++Tms10Counter>=2 ) Tms10Counter=0;
      if( ++Tms20Counter>=4 ) Tms20Counter=0;
      if( ++Tms50Counter>=10 ) Tms50Counter=0;
      if( ++Tms100Counter>=20 ) Tms100Counter=0;
      if( ++Tms500Counter>=100 ) Tms500Counter=0;
      if( ++Tms1000Counter>=200 ) Tms1000Counter=0;      

#if SELF_TEST      
      /* self check */
      STL_DoRunTimeChecks();     
      Safety_RunCheck();
#endif  
      
  

//      if( Tms10Counter == 0 )
//      {
//        CPU_Comm();
//      }
      
      if( Tms20Counter == 0 )      
      {
          CPU_Comm();
      }
      
      if( Tms50Counter == 0 )
      {                       
          Input_Check();                   
#ifdef GEC_SF_MASTER          
          USB_VCP_RecvBufandSend();
#endif          
          /* Reload SF_EWDG / EWDT counter */
          SF_EWDT_TOOGLE();
          EWDT_TOOGLE();
          
      } 
      
      if( Tms100Counter == 0 )
      {         
          SF_CTR_Check();
      }
#ifdef GEC_SF_MASTER            
      if( Tms500Counter == 0 )
      {            
           CAN_Comm();                  
      }
      
      if( Tms1000Counter == 0 )
      {
          Comm_DisplayBoard();      
      }
#endif      

}


/*******************************************************************************
* Function Name  : main
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{        
  
    /** hardware init **/
    Bsp_Init();    
    
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



