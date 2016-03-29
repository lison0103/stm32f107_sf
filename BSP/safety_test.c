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

volatile type_testResult_t result = IEC61508_testFailed;   /* variable is located in the stack */
u32 SafetyTestFlowCnt = 0;
u32 SafetyTestFlowCntInv = 0xFFFFFFFFuL;


/*******************************************************************************
* Function Name  : FailSafeTest
* Description    : Contains the Fail Safe routine executed in case of
*                  failure detected during one of the POR self-test routines
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FailSafeTest(void)
{
  while(1)
  {
    NVIC_SystemReset();
  }
}

/*******************************************************************************
* Function Name  : Safety_test_init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int Safety_test_init(void)
{       
             
      /*----------------------------------------------------------------------*/
      /*---------------------- Configuration registers -----------------------*/
      /*----------------------------------------------------------------------*/  
        SafetyTestFlowCnt += CONFIGURATION_REG_TEST_CALLER;
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_Configuration_8M();//HSE - 8MHz
        RCC_GetClocksFreq(&RCC_Clocks); 
        if (RCC_Clocks.SYSCLK_Frequency !=8000000)
        {
            FailSafeTest();
        }
        
        RCC_Configuration_72M(); //PLL - 72MHz
        RCC_GetClocksFreq(&RCC_Clocks); 
        if (RCC_Clocks.SYSCLK_Frequency !=72000000)
        {
            FailSafeTest();
        }       

        
        SafetyTestFlowCntInv -= CONFIGURATION_REG_TEST_CALLER;
        
      /*----------------------------------------------------------------------*/
      /*--------------------------- FLAG registers ---------------------------*/
      /*----------------------------------------------------------------------*/
        SafetyTestFlowCnt += FLAG_TEST_CALLER;
        result = flag_test();
        if (result != IEC61508_testPassed)
        {
            FailSafeTest();                           
        }
        else
        {
            SafetyTestFlowCntInv -= FLAG_TEST_CALLER;
        }

      /*----------------------------------------------------------------------*/
      /*------------------------------- PC Test ------------------------------*/
      /*----------------------------------------------------------------------*/ 
        SafetyTestFlowCnt += PC_TEST_CALLER;
        result = IEC61508_PCTest_POST();
        if (result != IEC61508_testPassed)
        {
            FailSafeTest();                                    
        }   
        else
        {
            SafetyTestFlowCntInv -= PC_TEST_CALLER;
        }

      /*----------------------------------------------------------------------*/
      /*------------------- program execution instruction --------------------*/
      /*------------------------- Accumulator Test ---------------------------*/
      /*----------------------------------------------------------------------*/  
        SafetyTestFlowCnt += PEI_TEST_CALLER;
        /* Do the IEC61508 instruction tests */
        if (iec61508_InstCheck_POST() == IEC61508_testFailed)
        {            
           FailSafeTest();
        }
        else
        {
            SafetyTestFlowCntInv -= PEI_TEST_CALLER;
        }
        
      /*----------------------------------------------------------------------*/
      /*---------------- Check Safety routines Control flow  -----------------*/
      /*----------------------------------------------------------------------*/        
       if (((SafetyTestFlowCnt ^ SafetyTestFlowCntInv) != 0xFFFFFFFFuL)
          ||(SafetyTestFlowCnt != CHECKCNT ))  
       {
          FailSafeTest();
       }
       else
       {
           SafetyTestFlowCnt = 0;
           SafetyTestFlowCntInv = 0xFFFFFFFFuL;          
       }
        
        return 0;

}

/*******************************************************************************
* Function Name  : Safety_test_run
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int Safety_test_run(void)
{       
        
      /*----------------------------------------------------------------------*/
      /*---------------------- Configuration registers -----------------------*/
      /*----------------------------------------------------------------------*/  
        SafetyTestFlowCnt += CONFIGURATION_REG_TEST_CALLER;
        
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_GetClocksFreq(&RCC_Clocks); 
        if (RCC_Clocks.SYSCLK_Frequency !=72000000)
        {
            while(1);
        }
        
        
        SafetyTestFlowCntInv -= CONFIGURATION_REG_TEST_CALLER;
        
      /*----------------------------------------------------------------------*/
      /*--------------------------- FLAG registers ---------------------------*/
      /*----------------------------------------------------------------------*/
        SafetyTestFlowCnt += FLAG_TEST_CALLER;
        result = flag_test();
        if (result != IEC61508_testPassed)
        {
          while(1);                            /* remains if FLAG test fails */
        }
        else
        {
          SafetyTestFlowCntInv -= FLAG_TEST_CALLER;
        }

      /*----------------------------------------------------------------------*/
      /*------------------------------- PC Test ------------------------------*/
      /*----------------------------------------------------------------------*/ 
        SafetyTestFlowCnt += PC_TEST_CALLER;
        result = IEC61508_PCTest_POST();
        if (result != IEC61508_testPassed)
        {
          while(1);                                      /* remains if PC test fails */
        }   
        else
        {
          SafetyTestFlowCntInv -= PC_TEST_CALLER;
        }

      /*----------------------------------------------------------------------*/
      /*------------------- program execution instruction --------------------*/
      /*------------------------- Accumulator Test ---------------------------*/
      /*----------------------------------------------------------------------*/  
        SafetyTestFlowCnt += PEI_TEST_CALLER;
        /* Do the IEC61508 instruction tests */
        if (iec61508_InstCheck_POST() == IEC61508_testFailed)
        {
            /* POST instruction test failed */  /*waitting for WATCHDOG to reset*/
            while (1);
        }
        else
        {
            SafetyTestFlowCntInv -= PEI_TEST_CALLER;
        }
        
       if (((SafetyTestFlowCnt ^ SafetyTestFlowCntInv) != 0xFFFFFFFFuL)
          ||(SafetyTestFlowCnt != CHECKCNT ))  
       {
          while (1);
       }
       else
       {
           SafetyTestFlowCnt = 0;
           SafetyTestFlowCntInv = 0xFFFFFFFFuL;          
       }
        
        return 0;

}


/******************************  END OF FILE  *********************************/








