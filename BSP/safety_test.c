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

/* variable is located in the stack */
volatile type_testResult_t result = IEC61508_testFailed;   
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
* Function Name  : ConfigurationRegister_StartupCheck
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
void ConfigurationRegister_StartupCheck(void)
{
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
}

/*******************************************************************************
* Function Name  : ConfigurationRegister_RunCheck
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ConfigurationRegister_RunCheck(void)
{
    SafetyTestFlowCnt += CONFIGURATION_REG_TEST_CALLER;
    
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks); 
    if (RCC_Clocks.SYSCLK_Frequency !=72000000)
    {
        FailSafeTest();
    }
    /* Wait till PLL is used as system clock source */ 
    if (RCC_GetSYSCLKSource() != 0x08) 
    {
        FailSafeTest();
    } 
    result = ConfigurationRegister_Check();
    if (result != IEC61508_testPassed)
    {
        FailSafeTest();                           
    }
    else
    {
        SafetyTestFlowCntInv -= CONFIGURATION_REG_TEST_CALLER;
    }  
}

/*******************************************************************************
* Function Name  : FlagRegisterCheck
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FlagRegisterCheck(void)
{
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
}

/*******************************************************************************
* Function Name  : ProgramCounterCheck
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ProgramCounterCheck(void)
{
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
}

/*******************************************************************************
* Function Name  : ProgramExecutionInstructionCheck
* Description    : Program Execution Instruction and Accumulator test
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ProgramExecutionInstructionCheck(void)
{
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
}



/*******************************************************************************
* Function Name  : Safety_StartupCheck
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int Safety_StartupCheck(void)
{       
             
      /*----------------------------------------------------------------------*/
      /*---------------------- Configuration registers -----------------------*/
      /*----------------------------------------------------------------------*/  
      ConfigurationRegister_StartupCheck();
        
      /*----------------------------------------------------------------------*/
      /*--------------------------- FLAG registers ---------------------------*/
      /*----------------------------------------------------------------------*/
      FlagRegisterCheck();

      /*----------------------------------------------------------------------*/
      /*------------------------------- PC Test ------------------------------*/
      /*----------------------------------------------------------------------*/ 
      ProgramCounterCheck();

      /*----------------------------------------------------------------------*/
      /*------------------- program execution instruction --------------------*/
      /*------------------------- Accumulator Test ---------------------------*/
      /*----------------------------------------------------------------------*/  
      ProgramExecutionInstructionCheck();

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
int Safety_RunCheck(void)
{       
        
      /*----------------------------------------------------------------------*/
      /*---------------------- Configuration registers -----------------------*/
      /*----------------------------------------------------------------------*/  
      ConfigurationRegister_RunCheck();            
        
      /*----------------------------------------------------------------------*/
      /*--------------------------- FLAG registers ---------------------------*/
      /*----------------------------------------------------------------------*/
        FlagRegisterCheck();

      /*----------------------------------------------------------------------*/
      /*------------------------------- PC Test ------------------------------*/
      /*----------------------------------------------------------------------*/ 
        ProgramCounterCheck();

      /*----------------------------------------------------------------------*/
      /*------------------- program execution instruction --------------------*/
      /*------------------------- Accumulator Test ---------------------------*/
      /*----------------------------------------------------------------------*/  
        ProgramExecutionInstructionCheck();

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


/******************************  END OF FILE  *********************************/








