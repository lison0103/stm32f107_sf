/*******************************************************************************
* File Name          : safety_test.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "safety_test.h"
#include "sys.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LED_indicator1(void);


/*******************************************************************************
* Function Name  : Safety_test
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int Safety_test(void)
{

  /* System Clocks Configuration ---------------------------------------------*/
  RCC_Configuration();

  /* LED configuration ------------------------------------------------------*/
  LED_Init();
  
  /* Self test routines initialization ---------------------------------------*/
  STL_InitRunTimeChecks();

  while(1)
  {
    /* This is where the main self-test routines are executed ----------------*/
    STL_DoRunTimeChecks();

    LED_indicator1();
    
    /* This is to monitor transparent RAM test activity */
//    RamPntr = (u32)(p_RunTimeRamChk) - 0x20000000;
//    printf("RamPntr= %d\r", RamPntr);

    /* Test if PB.09 level is low (Key push-button on Eval Board pressed) */
//    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0x00)
//    {
//      /* WWDG configuration --------------------------------------------------------*/
//      /* Enable WWDG clock */
//      RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
//      /* WWDG clock counter = (PCLK1/4096)/8 = 244 Hz (~4 ms)  */
//      WWDG_SetPrescaler(WWDG_Prescaler_8);
//      /* Set Window value to 0x7F */
//      WWDG_SetWindowValue(0x7F);
//      /* Enable WWDG and set counter value to 0x7F, WWDG timeout = ~4 ms * 64 = 262 ms */
//      WWDG_Enable(0x7F);
//    }

  } /* while(1) */

}

void LED_indicator1(void)
{
	static u32 led_idr_cnt=0;	 
	
	led_idr_cnt++;
	
	if(led_idr_cnt >= 100000 && data_error <= 0)
	{
                led_idr_cnt = 0;
		LED=!LED;                
	}   
}


/******************************  END OF FILE  *********************************/








