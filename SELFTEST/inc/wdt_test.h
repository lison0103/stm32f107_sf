/**
 * Company	: ThyssenKrupp PDC
 * @file	: wdt_test.h
 * @purpose     :
 * @brief	: header for c based WDT test 
 *                  POST and BIST WDT test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 28-April-2014
 * @author	: Paul
*/
#ifndef _WDT_TEST_H__
#define _WDT_TEST_H__

#include "iec61508.h"
#include "ram_test.h"

/*! @addtogroup IEC61508_WDOG_TEST
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*!
 * @brief   This function initializes the IEC61508 WDT test
 *          for POST usage\n
 *          the function will be called after reset and checks the reset conditions\n
 * The function will enable the WDOG module if the last reset condition was POR or EXT.\n
 * In this case the WDOG start counting and will cause a WDOG reset after a period of time.\n
 * Residing in the POST function the routine now will check for WDOG reset and return a pass result.
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @note        debugging can affect the WDT behavior defined in the DEBUGRUN bit in the WDOG_CTRL register.
 * @attention   This function must not be called outside the POST tests.
 * @image html WDOG-init.jpg
 */
type_testResult_t IEC61508_initWDT(void);

/*!
 * @brief   This function represents the part of the IEC61508 WDT test
 *          which has to be executed within a loop. It refreshes the WDT counter.
 *
 * @attention This function must be called periodically inside a loop.
 *
 * @attention For this function, it is necessary to estimate the count how often this function could be called.
 * @image html WDOG-refresh.jpg
 */
void IEC61508_Refresh_WDT(void);

/*!
 * @brief   This function starts the Watchdog and waits for positive reset conditions.
 * @return  passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @note    POST use only! The function will lead to a reset.
 * @attention The function may not work in debugging mode. The reset cause of the debugge is not obtained and\n
 *            debugger interface may affect the WDOG runing conditions. If the reset occures the debugger may\n
 *            fail reconnect the target without resetting it again.
 * @image html WDOG-force.jpg
 */
type_testResult_t IEC61508_Force_WDT_Reset(void);

void WWDG_Configuration(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* _WDT_TEST_H__ */

/************************************** EOF *********************************/