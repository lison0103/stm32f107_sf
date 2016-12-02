/**
 * Company	: ThyssenKrupp PDC
 * @file	: iec61508.h
 * @purpose     :
 * @brief	: IEC61508 verification.
 * @version	: 1.0
 * @date	: 22-April-2014
 * @author	: Paul
*/

#ifndef __IEC61508_
#define __IEC61508_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum testResult_enum
{
  IEC61508_testFailed     = 0,              /*!< test result failed replacement */
  IEC61508_testPassed     = 1,              /*!< test result passed replacement */
  IEC61508_testInProgress = 2               /*!< test is still in progress replacement */
} type_testResult_t;


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
