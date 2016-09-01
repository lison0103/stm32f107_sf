/*******************************************************************************
* File Name          : flag_test.h
* Author             : Paul
* Version            : V1.0
* Date               : 05/13/2014
* Description        : header for c based Register flag bit test POST configure test
*                      IEC61508 verification on ETC-01 mainboard.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLAG_TEST_H
#define __FLAG_TEST_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "iec61508.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
  /* base 0 */
  unsigned int Flag_Pass_Cnt;     /*!< tests passed */
  /* base 4 */
  unsigned int Flag_Err_Cnt;      /*!< tests failed */
}IEC61508_FlagCheck_struct;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
type_testResult_t flag_test(void);


#endif /* __FLAG_TEST_H */


/******************************  END OF FILE  *********************************/

