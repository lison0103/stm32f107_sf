/**
 * Company	: ThyssenKrupp PDC
 * @file	: config_tes.h
 * @purpose     :
 * @brief	: header for c based Configuration Register test POST configure test
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 12-May-2014
 * @author	: Paul
*/

#ifndef _CONFIG_TEST_H__
#define _CONFIG_TEST_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration_8M(void);
void RCC_Configuration_72M(void);
#endif  /* _CONFIG_TEST_H__ */

/************************************** EOF *********************************/
