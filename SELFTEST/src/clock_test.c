/**
 * Company	: ThyssenKrupp PDC
 * @file	: clock_test.c
 * @purpose     :
 * @brief	: c based clock structure test POST and BIST clock structure test 
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 5-May-2014
 * @author	: Paul
*/

#include "iec61508.h"
#include "ram_test.h"
#include "clock_test.h"
#include "stm32f10x_tim.h"

ClockTest_t ClockTest;  /* Clock/Timer-test requires fixed data */

/*!
 * @brief    internal function to reset all test structures
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC61508_code"
#else
__attribute__((section(".IEC61508_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC61508_resetClockTest(uint32_t ratio, uint32_t tolerance)
{
  /* reset counter */
  if (ratio == 0)
  {
    ClockTest.timerCounter = 0;
    ClockTest.baseTicks    = 0;
    ClockTest.result       = IEC61508_testInProgress + 1;
  }
  else
  {
    ClockTest.Ratio        = ratio;
    ClockTest.Tolerance    = tolerance;
    ClockTest.timerCounter = 0;
    ClockTest.baseTicks    = 0;
  }
}

/*!
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC61508_code"
#else
__attribute__((section(".IEC61508_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC61508_initClockTest(uint32_t ratio, uint32_t tolerance)
{
  /* setup parameter */
  IEC61508_resetClockTest(ratio, tolerance);

  //设置TIM类型
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  //设置TIM1时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_DeInit(TIM2);
  //采用内部时钟给TIM2提供时钟源
  TIM_InternalClockConfig(TIM2);
  TIM_TimeBaseStructure.TIM_Period=2000;        //自动重装载寄存器周期的值(计数值) ARR的值
  //累计 TIM_Period个频率后产生一个更新或者中断
  TIM_TimeBaseStructure.TIM_Prescaler= (36000 - 1);    //时钟预分频数   例如：时钟频率=72MHZ/(时钟预分频+1)
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;    //采样分频
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;   //向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);       //清除溢出中断标志
  //TIM_PrescalerConfig(TIM2,0x8C9F,TIM_PSCReloadMode_Immediate); //时钟分频系数36000，所以定时器时钟为2K
  TIM_ARRPreloadConfig(TIM2, DISABLE);       //禁止ARR预装载缓冲器
  //开启TIM2的中断
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  //选择中断分组1
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  //选择TIM2的中断通道
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
  //抢占式中断优先级设置为0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //响应式中断优先级设置为0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //使能中断
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM2, ENABLE);           //开启时钟
  
  
#if (USE_SYSTICK == 1)
/*! @cond USE_SYSTICK */

  /* Setup SysTick Timer for 10 msec interrupts  */
  /* set reload register */
  /* set Priority for Cortex-M3 System Interrupts */
  /* Load the SysTick Counter Value */
  /* Enable SysTick IRQ and SysTick Timer */

/*! @endcond */
#elif (USE_TIMER0)
/*! @cond USE_TIMER0 */
/* start timer0 */
/* Enable Timer Clock */
  /* Configure TIMER from HFPERCLK */
  /* 32K ticks interrupts */
  /* Clear pending interrupt */
  /* enable interrupt */
/*! @endcond */
#elif (USE_TIMER1)
/*! @cond USE_TIMER1 */
/* start timer1 */
/* TODO : implement timer1 setup here */
/*! @endcond */
#endif
}

void RCC_cfg()
{
  //定义错误状态变量
  ErrorStatus HSEStartUpStatus;
  //将RCC寄存器重新设置为默认值
  RCC_DeInit();
  //打开外部高速时钟晶振
  RCC_HSEConfig(RCC_HSE_ON);
  //等待外部高速时钟晶振工作
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    //设置AHB时钟(HCLK)为系统时钟
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    //设置高速AHB时钟(APB2)为HCLK时钟
    RCC_PCLK2Config(RCC_HCLK_Div1);
    //设置低速AHB时钟(APB1)为HCLK的2分频
    RCC_PCLK1Config(RCC_HCLK_Div2);
    //设置FLASH代码延时
    FLASH_SetLatency(FLASH_Latency_2);
    //使能预取指缓存
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    //设置PLL时钟，为HSE的9倍频 8MHz * 9 = 72MHz
//    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    //使能PLL
    RCC_PLLCmd(ENABLE);
    //等待PLL准备就绪
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    //设置PLL为系统时钟源
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    //判断PLL是否是系统时钟
    while(RCC_GetSYSCLKSource() != 0x08);
  }
   //允许TIM2的时钟
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  //允许GPIO的时钟
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
}

void RCC_HSI_Cfg()
{
  RCC_DeInit();  //将外设RCC寄存器重设为缺省值
  RCC_HSICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET);  //等待HSI就绪
  
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  
  FLASH_SetLatency(FLASH_Latency_2);
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  while(RCC_GetSYSCLKSource() != 0x00);  //0x00: HSI 作为系统时钟

}
/*!
 * This function represents the part of the IEC61508 Class B clock test which has to be executed within the main loop.
 * It checks several thing:
 * @arg If the clock test timer interrupts are triggered
 * @arg If the clock test RTC interrupt is triggered
 * @arg or, If in any of the two interrupts an error was detected...
 *
 * This function must be called once inside the main loop.
 *
 * @attention For this function, it is necessary to estimate the count how often this function could be called. This is important to find
 * valid threshold values, which are used to test timer and RTC interrupt occurrence.
 *
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if (defined (__ICCARM__))
    #pragma section="IEC61508_code"
#else
__attribute__((section(".IEC61508_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
type_testResult_t IEC61508_Clocktest_PollHandler(void)
{
  type_testResult_t result = IEC61508_testFailed;
  uint32_t     absolutUP, absolutLOW, intermediate;
  ClockTest.result = IEC61508_testInProgress;

  /* wait on RTC flag if running */
    //ClockTest.baseTicks++;
  /* calculate tolerance band extremes */
  intermediate = ClockTest.Ratio * ClockTest.Tolerance / 100;
  if (intermediate > (0x80000000 - 1))
  {
    ClockTest.result = IEC61508_testFailed;
    return(result);
  }
  else
  {
    /* absolutLOW must not be negative */
    if (ClockTest.Ratio >= intermediate)
    {
      absolutLOW = ClockTest.Ratio - intermediate;
    }
    else
    {
      absolutLOW = 0;
    }
    absolutUP = intermediate + ClockTest.Ratio;
  }
  /* calculate average counter/baseTicks */
  intermediate = ClockTest.timerCounter / ClockTest.baseTicks;
  if ((intermediate > absolutLOW) && (intermediate < absolutUP))
  {
    result = IEC61508_testPassed;
    /* reset the counters */
    IEC61508_resetClockTest(0, 0);
    /* stop Interrupts */
#if (USE_SYSTICK == 1)
/*! @cond USE_SYSTICK */
    /* Disable SysTick IRQ and SysTick Timer */

  /* Disable the SysTick Counter 禁止SysTick 计数器*/
  //SysTick_CounterCmd(SysTick_Counter_Disable);
  /* Clear the SysTick Counter 清零SysTick 计数器*/
  //SysTick_CounterCmd(SysTick_Counter_Clear);
    
/*! @endcond */
#elif (USE_TIMER0)
/*! @cond USE_TIMER0 */
    //if (TIMER0->CMD != TIMER_CMD_START)
    //{
    //  ClockTest.result = IEC61508_testFailed;
    //  return(IEC61508_testFailed);
    //}
    ClockTest.result = IEC61508_testPassed;
    /* disable interrupt */
    //NVIC->ICER[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
    /* Clear pending interrupt */
    //NVIC->ICPR[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
    //TIMER0->IFC                                = TIMER_IEN_OF;
    //TIMER0->IEN                               &= ~TIMER_IEN_OF;
/*! @endcond */
#elif (USE_TIMER1)
/*! @cond USE_TIMER1 */
/* TODO : implement timer1 stop here */
/*! @endcond */
#endif
  }

  return(result);
}

/*!
 * This function is intended to be called once inside the custom RTC interrupt service handler.
 * It can be used as service handler by itself. The result can be evaluated by checking the
 * counters values are reseted after this.
 *
 * This function tests the timer time frame and in this case the CPU frequency.
 * Also, this function checks if the main loop function was called.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC61508_code"
#else
__attribute__((section(".IEC61508_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC61508_Clocktest_RTCHandler(void)
{
  uint32_t absolutUP, absolutLOW, intermediate;
  ClockTest.result = IEC61508_testInProgress;
  /* TODO: call this vector if RTC is used with interrupt */
  /* check comp0 interrupt */
  //if (RTC->IF & RTC_IF_COMP0)
  //{
    ClockTest.baseTicks++;
  //  RTC->IFC = RTC_IFC_COMP0;               /* clear the interrupt flag */
    /* calculate tolerance band extremes */
    intermediate = ClockTest.Ratio * ClockTest.Tolerance / 100;

      /* absolutLOW must not be negative */
      if (ClockTest.Ratio >= intermediate)
      {
        absolutLOW = ClockTest.Ratio - intermediate;
      }
      else
      {
        absolutLOW = 0;
      }
      absolutUP = intermediate + ClockTest.Ratio;

      /* calculate average counter/baseTicks */
    intermediate = ClockTest.timerCounter / ClockTest.baseTicks;
      if ((intermediate > absolutLOW) && (intermediate < absolutUP))
      {
        /* reset the counters */
        IEC61508_resetClockTest(0, 0);
        /* stop Interrupts */
#if (USE_SYSTICK == 1)
/*! @cond USE_SYSTICK */
        /* Disable SysTick IRQ and SysTick Timer */
        SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
/*! @endcond */
#elif (USE_TIMER0)
/*! @cond USE_TIMER0 */
      if (TIMER0->CMD != TIMER_CMD_START)
      {
        ClockTest.result = IEC61508_testFailed;
        return;
      }
      ClockTest.result = IEC61508_testPassed;
      /* disable interrupt */
      NVIC->ICER[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
      /* Clear pending interrupt */
      NVIC->ICPR[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
      TIMER0->IFC                                = TIMER_IEN_OF;
      TIMER0->IEN                               &= ~TIMER_IEN_OF;
/*! @endcond */
#elif (USE_TIMER1)
/*! @cond USE_TIMER1 */
/* TODO : implement timer1 stop here */
/*! @endcond */
#endif
    //}
  }
}


/*!
 * @}
 */

/************************************** EOF *********************************/
