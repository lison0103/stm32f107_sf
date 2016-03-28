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

#include "check_instruction.h"
#include "check_instruction_2.h"
#include "ram_test.h"
#include "pc_test.h"
#include "flag_test.h"
#include "config_test.h"

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

        volatile type_testResult_t result = IEC61508_testFailed;   /* variable is located in the stack */
        
        result = flag_test();
        if (result != IEC61508_testPassed)
        {
          while(1);                            /* remains if FLAG test fails */
        }
        
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_Configuration_8M();//HSE - 8MHz
        RCC_GetClocksFreq(&RCC_Clocks); 
        if (RCC_Clocks.SYSCLK_Frequency !=8000000)
        {
          while(1);
        }
        
        RCC_Configuration_72M(); //PLL - 72MHz
        RCC_GetClocksFreq(&RCC_Clocks); 
        if (RCC_Clocks.SYSCLK_Frequency !=72000000)
        {
          while(1);
        }
        
         /* Configure PA.00 in interrupt mode */
        EXTI0_Config();
        /* Generate software interrupt: simulate a falling edge applied on EXTI0 line */
        EXTI_GenerateSWInterrupt(EXTI_Line0);//中断函数中加入标志 IEC61508_testPassed，判断成功触发
       
        result = IEC61508_PCTest_POST();
        if (result != IEC61508_testPassed)
        {
          while(1);                                      /* remains if PC test fails */
        }        

        /* Do the IEC61508 instruction tests */
        if (iec61508_InstCheck_POST() == IEC61508_testFailed)
        {
            /* POST instruction test failed */  /*waitting for WATCHDOG to reset*/
            while (1);
        }
        
        
        return 0;

}




/******************************  END OF FILE  *********************************/








