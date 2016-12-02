/*******************************************************************************
* File Name          : flag_test.c
* Author             : Paul
* Version            : V1.0
* Date               : 05/13/2014
* Description        : c based Register flag bit test POST configure test 
*                       IEC61508 verification.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "flag_test.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static IEC61508_FlagCheck_struct  FlagCheck;
/*static unsigned int RestFlag;*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : flag_test
* Description    : Check flag register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
type_testResult_t flag_test(void)
{
    type_testResult_t testResult = IEC61508_testPassed;
    
    FlagCheck.Flag_Pass_Cnt = 0u;
    FlagCheck.Flag_Err_Cnt = 0u;
    
    /* Checks whether the FLASH Read Out Protection Status is set or not */
#ifdef GEC_SF_S_NEW  
        if ( FLASH_OB_GetRDP() == RESET )
#else
        if ( FLASH_GetReadOutProtectionStatus() == RESET)
#endif
        {  
            FlagCheck.Flag_Pass_Cnt++;
        }
        else
        {
            FlagCheck.Flag_Err_Cnt++;
        }
    
    /* Checks whether the FLASH Prefetch Buffer status is set or not */
#ifndef GEC_SF_S_NEW
    if( FLASH_GetPrefetchBufferStatus() != RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
#endif  
    /* Checks whether the specified RCC interrupt has occurred or not */
    /* RCC_IT_LSIRDY: LSI ready interrupt */
    if (RCC_GetITStatus(RCC_IT_LSIRDY) == RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    /* RCC_IT_LSERDY: LSE ready interrupt */
    if (RCC_GetITStatus(RCC_IT_LSERDY) == RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    /* RCC_IT_HSIRDY: HSI ready interrupt */
    if (RCC_GetITStatus(RCC_IT_HSIRDY) == RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    /* RCC_IT_HSERDY: HSE ready interrupt */
    if (RCC_GetITStatus(RCC_IT_HSERDY) == RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    /* RCC_IT_PLLRDY: PLL ready interrupt */
    if (RCC_GetITStatus(RCC_IT_PLLRDY) == RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    
    /* RCC_FLAG_HSIRDY: HSI oscillator clock ready */
    if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    /* RCC_FLAG_HSERDY: HSE oscillator clock ready */
    if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    /* RCC_FLAG_PLLRDY: PLL clock ready */
    if (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }
    /* RCC_FLAG_LSIRDY: LSI oscillator clock ready */
    if (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    } 
#if 0     
    /* RCC_FLAG_PINRST: Pin reset */
    if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {  
        FlagCheck.Flag_Pass_Cnt++;
    }
    else
    {
        FlagCheck.Flag_Err_Cnt++;
    }    
   
    /* RCC_FLAG_IWDGRST:  IWDG RESET */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        RestFlag = 1u;
    }      
    /* RCC_FLAG_SFTRST: Software reset */
    else if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
    {
        RestFlag =  2u;  
    }
    /* RCC_FLAG_PORRST: Power on reset */
    else if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
        RestFlag = 3u;         
    }
    else
    {}
#endif  
    if (FlagCheck.Flag_Err_Cnt >= IEC61508_testPassed) 
    {
        testResult = IEC61508_testFailed;
    }
    
    return testResult;
    
    
}

/******************************  END OF FILE  *********************************/

