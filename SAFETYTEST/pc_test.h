/*******************************************************************************
* File Name          : pc_tes.h
* Author             : Paul
* Version            : V1.0
* Date               : 05/08/2014
* Description        : header for c based PC test 
*                      POST and BIST PC test for IEC61508 verification on ETC-01 mainboard
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PC_TEST_H
#define __PC_TEST_H

/* Includes ------------------------------------------------------------------*/
#include "iec61508.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
type_testResult_t IEC61508_PCTest_POST(void);
type_testResult_t IEC61508_PCTest_BIST(void);


#endif /* __PC_TEST_H */


/******************************  END OF FILE  *********************************/

