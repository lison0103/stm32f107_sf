/**
 * Company	: ThyssenKrupp PDC
 * @file	: wdt_test.c
 * @purpose     :
 * @brief	: c based WDT test 
 *                  POST and BIST WDT test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 28-April-2014
 * @author	: Paul
*/

#include "stm32f10x.h"
//#include "platform_config.h"
#include "wdt_test.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_wwdg.h"


//#include "stm32f10x_exti.h"
//#include "stm32f10x_wwdg.h"
//#include "stm32f10x_flash.h"

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC61508_code"
#else
__attribute__((section(".IEC61508_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
type_testResult_t IEC61508_initWDT(void)
{
  type_testResult_t result = IEC61508_testFailed;
  //独立看门狗初始化
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   //启动寄存器读写
  IWDG_SetPrescaler(IWDG_Prescaler_32);           //40K时钟32分频
  IWDG_SetReload(349);                            //计数器数值
  IWDG_ReloadCounter();                           //重启计数器
  IWDG_Enable();                                  //启动看门狗
  return result;
}

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC61508_code"
#else
__attribute__((section(".IEC61508_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC61508_Refresh_WDT(void)
{
  /* If a previous clearing is being synchronized to LF domain, then there
   * is no point in waiting for it to complete before clearing over again.
   * This avoids stalling the core in the typical use case where some idle loop
   * keeps clearing the watchdog. */
  //if (SyncWDOGCMD())
  //{
  //  return;
  //}

  //WDOG->CMD = WDOG_CMD_CLEAR;
}

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC61508_code"
#else
__attribute__((section(".IEC61508_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
type_testResult_t IEC61508_Force_WDT_Reset(void)
{
  type_testResult_t result = IEC61508_testFailed;
  /* no debugger allowed here */
  result = IEC61508_initWDT();

  return result;
}

/***************************/
void WWDG_Configuration(void)
{
    /* 开启窗口看门狗时钟 */
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);
//   /* Deinitialize the WWDG registers将外设 WWDG寄存器重设为缺省值  */ 
//   从其底层函数可以看出，初始化关闭窗口看门狗时钟
//   WWDG_DeInit();
   /* Set WWDG prescaler to 8 设置 WWDG 预分频值 即PCLK1/4096/8=1098.6 */ 
   WWDG_SetPrescaler(WWDG_Prescaler_8);
   /* Set WWDG window value to 0x41指定的窗口值，该参数取值必须在 0x40 与 0x7F之间。  */ 
   WWDG_SetWindowValue(0x41); 
  /* Enable WWDG and set counter value to 0x7F 使能窗口看门狗，并把看门狗计数器的值设为0x7f*/
    WWDG_Enable(0x7f);
   /* Clear EWI flag 清除早期唤醒中断标志位 */ 
   WWDG_ClearFlag();  
   /* Enable WWDG Early wakeup interrupt 使能 WWDG 早期唤醒中断（EWI）  */ 
   WWDG_EnableIT();  
}


/*!
 * @}
 */

/************************************** EOF *********************************/
