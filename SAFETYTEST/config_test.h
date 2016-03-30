/*******************************************************************************
* File Name          : config_test.h
* Author             : lison
* Version            : V1.0
* Date               : 03/30/2016
* Description        : 
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CONFIG_TEST_H__
#define _CONFIG_TEST_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "iec61508.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RCC_Configuration_8M(void);
void RCC_Configuration_72M(void);
type_testResult_t ConfigurationRegister_Check(void);	

#endif  /* _CONFIG_TEST_H__ */

/******************************  END OF FILE  *********************************/


