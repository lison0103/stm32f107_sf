/**
 * Company	: ThyssenKrupp PDC
 * @file	: config_tes.h
 * @purpose     :
 * @brief	: header for c based Register flag bit test POST configure test
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 13-May-2014
 * @author	: Paul
*/

#ifndef _FLAG_TEST_H__
#define _FLAG_TEST_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "ram_test.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
type_testResult_t flag_test(void);
#endif  /* _FLAG_TEST_H__ */

/************************************** EOF *********************************/
