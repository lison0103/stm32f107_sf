/**
 * Company	: ThyssenKrupp PDC
 * @file	: clock_tes.h
 * @purpose     :
 * @brief	: header for c based Clock structure test POST and BIST clock structure test  
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 5-May-2014
 * @author	: Paul
*/

#ifndef _CLOCK_TEST_H__
#define _CLOCK_TEST_H__

/*! @addtogroup IEC60335_TIMER_RTC_TEST
 * @{
 */

#include "iec61508.h"

extern ClockTest_t ClockTest;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*!
 * @brief   This function initializes the IEC60335 Class B clock test
 *          for BIST usage
 *          all test structures are reset to defined values
 *
 * @param   ratio           setup value for timer test ratio to RTC in counter ticks
 * @param   tolerance       setup value for deviation tolerance in percent
 * @attention  This function must be called once inside the main loop.
 */
void IEC61508_initClockTest(uint32_t ratio, uint32_t tolerance);
/*!
 * @brief   This function represents the part of the IEC60335 Class B clock test
 *          which has to be executed within the main loop.
 *  It checks several thing:
 *  - If the clock test timer interrupts are triggered
 *  - If the clock test RTC interrupt is triggered
 *  - or, If in any of the two interrupts an error was detected...
 *
 * @attention   This function must be called once inside the main loop.
 *
 * @attention For this function, it is necessary to estimate the count how often this function could be called. 
 * This is important to find
 *  valid threshold values, which are used to test timer and RTC interrupt occurrence.
 *
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 */
type_testResult_t IEC61508_Clocktest_PollHandler(void);

/*!
 * @brief   This function is intended to use as timer interrupt service handler or to be
 *          called once inside the timer interrupt service handler.
 */
__INLINE static void IEC61508_Clocktest_TimerHandler(void)
{
  /* Increment counter necessary in test*/
  ClockTest.timerCounter++;
}

/*!
 * @brief   This function is intended to be called once inside the custom RTC interrupt service handler.
 *
 *  This function tests the timer time frame and in this case the CPU frequency.
 *
 *  Also, this function checks if the main loop function was called.
 */
void IEC61508_Clocktest_RTCHandler(void);

void RCC_cfg();
void RCC_HSI_Cfg();


#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* _CLOCK_TEST_H__ */

/************************************** EOF *********************************/
