/*******************************************************************************
* File Name          : safety_test.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains prototype of the Self-test functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SAFETY_TEST_H
#define __SAFETY_TEST_H

/* Includes ------------------------------------------------------------------*/
#include "iec61508.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define CONFIGURATION_REG_TEST_CALLER         ((u32)2)
#define FLAG_TEST_CALLER         ((u32)3) /* Do not modify: hard coded in assembly file */
#define PEI_TEST_CALLER       ((u32)5)


#define CHECKCNT ((u32)CONFIGURATION_REG_TEST_CALLER + \
                          FLAG_TEST_CALLER + \
                          PEI_TEST_CALLER)

#define CHECKCNTRUN ((u32)CONFIGURATION_REG_TEST_CALLER + \
                          FLAG_TEST_CALLER + \
                          PEI_TEST_CALLER)


/* Exported functions ------------------------------------------------------- */
void Safety_StartupCheck2(void);
void Safety_RunCheck2(void);
static void FailSafeTest(void);



#endif  /* __SAFETY_TEST_H */


/******************************  END OF FILE  *********************************/





