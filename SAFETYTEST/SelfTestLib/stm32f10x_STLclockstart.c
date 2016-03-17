/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLclockstart.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/03/2007
* Description        : This file contains the test function for clock circuitry
*                      and wrong frequency detection at start-up.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/*Reviewed for IEC 60335-1 Compliance
* This module can be used for self tests according to Table H.11.12.7 of appendix R of the standard ;EN/IEC 60335-1. The implementation of the module has to be tested in each application.
* Reviewed by V.D.E. Testing and Certification Institute ; Merianstrasse 28 ; D-63069 Offenbach
* VDE-Ref:    5008057-9999-0001/91436 FG23/Scb
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_lib.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static ErrorStatus STL_LSIinit(void);
static ErrorStatus STL_HSE_CSSinit(void);
static ErrorStatus STL_SwitchToExtClockSrc(void);
static void STL_RTCinit(void);
static void STL_SysTickInit(void);

/*******************************************************************************
* Function Name  : STL_ClockStartUpTest
* Description    : Start up the internal and external oscillators and verifies
*                  that clock source is within the expected range
* Input          : None
* Output         : None
* Return         : ClockStatus = {LSI_START_FAIL; HSE_START_FAIL;
*                  HSI_HSE_SWITCH_FAIL; TEST_ONGOING; EXT_SOURCE_FAIL;
*                  CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK}
*******************************************************************************/
ClockStatus STL_ClockStartUpTest(void)
{
      vu32 LSIPeriod, ClockFrequency;
      ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  CtrlFlowCnt += CLOCK_TEST_CALLEE;

  /* Start low speed internal (LSI) oscillator */
  if (STL_LSIinit() != SUCCESS)
  {
    Result = LSI_START_FAIL;
  }
  else  /* LSI started correctly */
  {
    /* Start High-speed external oscillator (HSE) and Clock Security System (CSS) */
    if (STL_HSE_CSSinit() != SUCCESS)
    {
      Result = HSE_START_FAIL;
    }
    else  /* HSE started correctly */
    {
      STL_RTCinit();      /* Start Real Time Clock to measure LSI period */
      STL_SysTickInit();  /* Start SysTick timer to measure HSI then HSE */

      /* Wait Systick underflow before measurement */
      while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
      {
      }
      /*-------------- Reference Measurement ---------------------------------*/
      LSIPeriod = RTC_GetCounter(); /* LSI frequency measurement */

      /* Switch CPU clock source from internal RC to oscillator and check it */
      if (STL_SwitchToExtClockSrc() != SUCCESS)
      {
        Result = HSI_HSE_SWITCH_FAIL;
      }
      else    /* Clock switched correctly */
      {
        RTC_SetCounter(0);                            /* Reset RTC */
        RTC_WaitForLastTask();
        SysTick->VAL =0X00;//SysTick_CounterCmd(SysTick_Counter_Clear);    /* Reset SysTick counter */

        /* Wait Systick underflow before measurement */
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
        {
        }

        /*-------------------- HSE Measurement -------------------------------*/
        RefHSEPeriod = RTC_GetCounter();   /* HSE frequency measurement */
        RefHSEPeriodInv = ~RefHSEPeriod;   /* Redundant storage */

        ClockFrequency = HSI_Freq * LSIPeriod;

        /* Store first measure as reference for run-time check */
        StartUpClockFreq = ClockFrequency;
        StartUpClockFreqInv = ~ClockFrequency;  /* Redundant storage */

        if (RefHSEPeriod !=0)  /* Test for divide by zero */
        {
            u32 TmpHSEPEriod = RefHSEPeriod;  /* Added for MISRA compliance */
          ClockFrequency /= TmpHSEPEriod;
        }
        else  /* Set frequency to an unrealistic high value */
        {
          ClockFrequency = U32_MAX;
        }

        if (ClockFrequency < HSE_LimitLow)
        {
          /* Switch back to internal clock */
          RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
          Result = EXT_SOURCE_FAIL;	/* Sub-harmonics: HSE -25% below expected */
        }
        else
        {
          if (ClockFrequency > HSE_LimitHigh)
          {
            /* Switch back to internal clock */
            RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
            Result = EXT_SOURCE_FAIL;	/* Harmonics: HSE +25% above expected */
          }
          else
          {
            /* Clock OK means < 16MHz (external osc max frequency), therefore
            no wait state are necessary */
            FLASH_SetLatency(FLASH_Latency_0);
            Result = FREQ_OK;         /* Crystal or Resonator started correctly */
          } /* No harmonics */
        } /* No sub-harmonics */
      } /* Clock switched correctly */
    } /* HSE started correctly */
  } /* LSI started correctly */

  CtrlFlowCntInv -= CLOCK_TEST_CALLEE;

  return(Result);

}


/*******************************************************************************
* Function Name  : STL_LSIinit
* Description    : Clock the APB domain 1, reset the circuitry of the battery
*                  backuped domain and start-up the low speed internal
*                  RC oscillator (30 < F < 60 kHz)
* Input          : None
* Output         : None
* Return         : ErrorStatus = {ERROR; SUCCESS}
*******************************************************************************/
ErrorStatus STL_LSIinit(void)
{
    ErrorStatus Result = ERROR;
    u32 TimeOut = LSI_START_TIMEOUT;

  CtrlFlowCnt += LSI_INIT_CALLEE;

  /* RTC clock selection -----------------------------------------------------*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSI */
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */
  do
  {
    TimeOut--;
  }
  while((RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) && (TimeOut != 0));

  if (TimeOut == 0)
  {
    Result = ERROR;     /* Internal low speed oscillator failure */
  }
  else
  {
    Result = SUCCESS;
  }

  CtrlFlowCntInv -= LSI_INIT_CALLEE;

  return (Result);

}


/*******************************************************************************
* Function Name  : STL_HSE_CSSinit
* Description    : Start-up the high speed external oscillator (based on crystal
*                  or resonator), enable the clock security system
* Input          : None
* Output         : None
* Return         : ErrorStatus = {ERROR; SUCCESS}
*******************************************************************************/
ErrorStatus STL_HSE_CSSinit(void)
{
    ErrorStatus Result = ERROR;
    u32 TimeOut = HSE_START_TIMEOUT;

  CtrlFlowCnt += HSE_INIT_CALLEE;

  /* Start-up the oscillator (HSE: High-speed External) */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  do
  {
    TimeOut--;
  }
  while((RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) && (TimeOut != 0));

  if (TimeOut == 0)
  {
    Result = ERROR;     /* Internal low speed oscillator failure */
  }
  else
  {
    /* Enable the Clock Security System (CSS): any failure on HSE will cause
       a Non Maskable Interrupt and switch back to internal oscillator */
    /* At this time the CPU clock source is still the internal oscillator */
    RCC_ClockSecuritySystemCmd(ENABLE);
    Result = SUCCESS;
  }

  CtrlFlowCntInv -= HSE_INIT_CALLEE;

  return (Result);

}


/*******************************************************************************
* Function Name  : STL_RTCinit
* Description    : Start-up the Real Time clock
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STL_RTCinit(void)
{

  CtrlFlowCnt += RTC_INIT_CALLEE;

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); /* Select LSI as RTC Clock Source */

  RCC_RTCCLKCmd(ENABLE);                  /* Start RTC counter */

 /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  RTC_SetPrescaler(0);    /* Do not prescale to have the highest precision */

  CtrlFlowCntInv -= RTC_INIT_CALLEE;

}


/*******************************************************************************
* Function Name  : STL_SysTickInit
* Description    : Initializes and starts the SysTick timer to have the
*                  Start-up measurement time base
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STL_SysTickInit(void)
{
  CtrlFlowCnt += SYSTICK_INIT_CALLEE;

  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  SysTick->LOAD = ((u32)16000uL);//SysTick_SetReload(SYSTICK_TB_START);          /* Set reload rate (Ref period) */
  SysTick->VAL =0X00;//SysTick_CounterCmd(SysTick_Counter_Clear);    /* Reset counter */
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;//SysTick_CounterCmd(SysTick_Counter_Enable);   /* Start down-counting */

  CtrlFlowCntInv -= SYSTICK_INIT_CALLEE;

}


/*******************************************************************************
* Function Name  : STL_SwitchToExtClockSrc
* Description    : Switch the main clock from internal RC oscillator to external
*                  oscillator.
* Input          : None
* Output         : None
* Return         : ErrorStatus = {ERROR; SUCCESS}
*******************************************************************************/
ErrorStatus STL_SwitchToExtClockSrc(void)
{
    ErrorStatus Result = ERROR;
    u32 TimeOut = CLOCK_SWITCH_TIMEOUT;

  CtrlFlowCnt += CLOCK_SWITCH_CALLEE;

  /* Set 2 wait states for Flash Access, so that core can fetch instruction even
  if  HSE frequency exceeds 24MHz */
  FLASH_SetLatency(FLASH_Latency_2);

  /* Switch the main clock to HSE and verify afterwards */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
  do
  {
    TimeOut--;
  }
  while ((RCC_GetSYSCLKSource() != 0x04) && (TimeOut != 0));

  if (TimeOut == 0)
  {
    Result = ERROR;     /* Clock switch failure */
  }
  else
  {
    Result = SUCCESS;
  }

  CtrlFlowCntInv -= CLOCK_SWITCH_CALLEE;

  return (Result);

}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
