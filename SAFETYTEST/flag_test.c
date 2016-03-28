/**
 * Company	: ThyssenKrupp PDC
 * @file	: flag_test.c
 * @purpose     :
 * @brief	: c based Register flag bit test POST configure test 
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 13-May-2014
 * @author	: Paul
*/

#include "stm32f10x.h"
#include "flag_test.h"

type_testResult_t flag_test(void)
{
  type_testResult_t Result = IEC61508_testFailed;
  
  //Checks whether the FLASH Read Out Protection Status is set or not
  if ( FLASH_GetReadOutProtectionStatus() == RESET)
  {
    Result = IEC61508_testPassed;  
  }
  else
  {
    Result = IEC61508_testFailed;
  }
  
  //Checks whether the FLASH Prefetch Buffer status is set or not
  if( FLASH_GetPrefetchBufferStatus() != RESET)
  {
    Result = IEC61508_testPassed;  
  }
  else
  {
    Result = IEC61508_testFailed;
  }
    
    
  // Checks whether the specified TIM flag is set or not.
  if( TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == RESET)
  {
    Result = IEC61508_testPassed;  
  }
  else
  {
    Result = IEC61508_testFailed;
  }
    
  //Checks whether the Early Wakeup interrupt flag is set or not
  if (WWDG_GetFlagStatus() == RESET)
  {
    Result = IEC61508_testPassed;  
  }
  else
  {
    Result = IEC61508_testFailed;
  }
  
  //Checks whether the specified RCC interrupt has occurred or not
  //RCC_IT_LSIRDY: LSI ready interrupt
  if (RCC_GetITStatus(RCC_IT_LSIRDY) == RESET)
  {
    Result = IEC61508_testPassed;    
  }
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_IT_LSERDY: LSE ready interrupt
  if (RCC_GetITStatus(RCC_IT_LSERDY) == RESET)
  {
    Result = IEC61508_testPassed;    
  }
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_IT_HSIRDY: HSI ready interrupt
  if (RCC_GetITStatus(RCC_IT_HSIRDY) == RESET)
  {
    Result = IEC61508_testPassed;    
  }
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_IT_HSERDY: HSE ready interrupt
  if (RCC_GetITStatus(RCC_IT_HSERDY) == RESET)
  {
    Result = IEC61508_testPassed;    
  }
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_IT_PLLRDY: PLL ready interrupt
  if (RCC_GetITStatus(RCC_IT_PLLRDY) == RESET)
  {
    Result = IEC61508_testPassed;    
  }
  else
  {
    Result = IEC61508_testFailed;
  }

  //RCC_FLAG_HSIRDY: HSI oscillator clock ready
  if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)
  {
    Result = IEC61508_testPassed;
  }
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_FLAG_HSERDY: HSE oscillator clock ready
  if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
  {
    Result = IEC61508_testPassed;
  }      
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_FLAG_PLLRDY: PLL clock ready
  if (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET)
  {
    Result = IEC61508_testPassed;
  }      
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_FLAG_LSERDY: LSE oscillator clock ready
//  if (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != RESET)
//  {
//    Result = IEC61508_testPassed;
//  }      
//  else
//  {
//    Result = IEC61508_testFailed;
//  }
  //RCC_FLAG_LSIRDY: LSI oscillator clock ready
  //if (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != RESET)
  //{
  //  Result = IEC61508_testPassed;
  //}      
  //RCC_FLAG_PINRST: Pin reset
  if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
  {
    Result = IEC61508_testPassed;
  }      
  else
  {
    Result = IEC61508_testFailed;
  }
  //RCC_FLAG_LPWRRST: Low Power reset
  //if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET)
  //{
  //  Result = IEC61508_testPassed;
  //}      
  
  //unsigned int RestFlag;  
  //看门狗复位标志
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    Result = IEC61508_testPassed;
    //RestFlag = 1;//
    //WdgRestCnt++;//存储在EPPROM
  }      
  //软件复位标志
  else if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
  {
    Result = IEC61508_testPassed;
    //RestFlag =  2;  
    //SftRestCnt++;//存储在EPPROM
  }
  //上电复位标志
  else if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
  {
    Result = IEC61508_testPassed;
    //RestFlag = 3;         
    //PorRestCnt++;//存储在EPPROM 
  }
  //RCC_ClearFlag();//清楚复位状态标志位, 程序后面要用到一些标示，所以不可以执行
  return Result;
}