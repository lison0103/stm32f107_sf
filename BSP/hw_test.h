/*******************************************************************************
* File Name          : hw_test.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains prototype of some test function.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_TEST_H
#define __HW_TEST_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "bsp_iocfg.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Input_Check(void);
void Input_Check2(void);
void CrossCommCPUCheck(void);
void HardwareTEST(void);

extern u8 testmode;


#endif  /* __HW_TEST_H */


/******************************  END OF FILE  *********************************/



