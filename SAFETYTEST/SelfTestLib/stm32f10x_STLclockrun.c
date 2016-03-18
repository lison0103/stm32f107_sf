/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLclockrun.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/03/2007
* Description        : Contains routines required to monitor CPU frequency during
*                      run-time.
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
#define SysTick_Counter_Disable        ((u32)0xFFFFFFFE)
#define SysTick_Counter_Enable         ((u32)0x00000001)
#define SysTick_Counter_Clear          ((u32)0x00000000)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : STL_SysTickRTCSync
* Description    : Initializes the SysTick for clock frequency measurement and
*                  main time base. The RTC is reset and synchronized with the
*                  SysTick to have a direct comparison with reference period
*                  (RTC counter is read during each SysTick interrupt).
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void STL_SysTickRTCSync(void)
{

  /* Initializes the Systick for run-time clock measurement */
//  SysTick_CounterCmd(SysTick_Counter_Disable);  /* Stop SysTick timer */
//  SysTick_CounterCmd(SysTick_Counter_Clear);    /* Reset counter */
//  /* Set reload rate. Corresponds to reference period for clock measurement */
//  SysTick_SetReload(SYSTICK_TB_RUN);
//  /* Enable the SysTick Interrupt */
//  SysTick_ITConfig(ENABLE);
    SysTick_Config(SYSTICK_TB_RUN);
  
  /* Reset RTC */
  RTC_SetCounter(0);
  /* Start down-counting */
//  SysTick_CounterCmd(SysTick_Counter_Enable);
  SysTick->CTRL |= SysTick_Counter_Enable;

}


/*******************************************************************************
* Function Name  : STL_ClockFreqTest
* Description    : This function verifies that external clock source frequency
*                  did not change vs initial measurement (done with internal
*                  oscillators only.
* Input          : None
*                  None
* Output         : None
* Return         : ClockStatus = {LSI_START_FAIL; HSE_START_FAIL;
*                  HSI_HSE_SWITCH_FAIL; TEST_ONGOING; EXT_SOURCE_FAIL;
*                  CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK}
*******************************************************************************/
ClockStatus STL_ClockFreqTest(void)
{
    u32 HSEPeriod, ClockFrequency;
    ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  ISRCtrlFlowCnt += CLOCKPERIOD_ISR_CALLEE;

  /*-------------------------- HSE Measurement -------------------------------*/
  /* Assuming this routine is called on each and every SysTick interrupt
    and that SysTick interrupt rate is constant */
  RTC_WaitForLastTask();          /* Verify last write operation completed*/
  HSEPeriod = RTC_GetCounter();   /* HSE frequency measurement */
  RTC_SetCounter(0);              /* Reset RTC for next measure */

  /* Verify first measure integrity and use it as reference */
  if ((StartUpClockFreq ^ StartUpClockFreqInv) == 0xFFFFFFFFuL)
  {
    ClockFrequency = StartUpClockFreq;

    if (HSEPeriod !=0)  /* Test for divide by zero */
    {
      u32 TmpHSEPEriod = HSEPeriod;  /* for MISRA compliance */
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
        Result = FREQ_OK;         /* Crystal or Resonator started correctly */
      } /* No harmonics */
    } /* No sub-harmonics */
  } /* Control Flow error */
  else
  {
    Result = CLASS_B_VAR_FAIL;
  }

  ISRCtrlFlowCntInv -= CLOCKPERIOD_ISR_CALLEE;

  return (Result);

}

/*******************************************************************************
* Function Name  : STL_MeasurePeriod
* Description    : This function only measures the clock period, to be compared
*                  later on
* Input          : None
* Output         : Current Frequency measure (arbitray unit)
* Return         : None
*******************************************************************************/
u32 STL_MeasurePeriod(void)
{
    u32 Period;

  ISRCtrlFlowCnt += MEASPERIOD_ISR_CALLEE;

  /*-------------------------- HSE Measurement -------------------------------*/
  /* Assuming this routine can only be called in a SysTick interrupt
    and that SysTick interrupt rate is constant */
  RTC_WaitForLastTask();          /* Verify last write operation completed*/
  Period = RTC_GetCounter();   /* HSE frequency measurement */
  RTC_SetCounter(0);              /* Reset RTC for next measure */

  ISRCtrlFlowCntInv -= MEASPERIOD_ISR_CALLEE;

  return (Period);   /* Store inverse redundant value */

}


/*******************************************************************************
* Function Name  : STL_CheckPeriod
* Description    : This function verifies the frequency from the last clock
*                  period measurement
* Input          : Period (u32), with the same unit as CurrentHSEPeriod (could
*                  be an average for instance)
* Output         : None
* Return         : ClockStatus = {LSI_START_FAIL; HSE_START_FAIL;
*                  HSI_HSE_SWITCH_FAIL; TEST_ONGOING; EXT_SOURCE_FAIL;
*                  CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK}
*******************************************************************************/
ClockStatus STL_CheckPeriod(u32 Period)
{
    u32 ClockFrequency;
    ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  CtrlFlowCnt += CLOCKPERIOD_TEST_CALLEE;
  /* First measure as reference for run-time check */
  if ((StartUpClockFreq ^ StartUpClockFreqInv) == 0xFFFFFFFFuL)
  {
    ClockFrequency = StartUpClockFreq;

    if (Period !=0)  /* Test for divide by zero */
    {
      ClockFrequency /= Period;
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
        Result = FREQ_OK;         /* Crystal or Resonator started correctly */
      } /* No harmonics */
    } /* No sub-harmonics */
  } /* Control Flow error */
  else
  {
    Result = CLASS_B_VAR_FAIL;
  }

  CtrlFlowCntInv -= CLOCKPERIOD_TEST_CALLEE;

  return (Result);

}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
