/**
 * Company	: ThyssenKrupp PDC
 * @file	: cpu_reg_test.h
 * @purpose     :
 * @brief	: CPU Register POST test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 22-April-2014
 * @author	: Paul
*/

#ifndef CPUREGS_H_
#define CPUREGS_H_

#include "iec61508.h"

  #define __ASM           __asm                                       /*!< asm keyword for IAR Compiler           */
  #define __INLINE        inline                                      /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */

typedef struct {
  /* base 0 */
  unsigned int testState;       /*!< CPU register POST tests passed */
  /* base 4 */
  unsigned int testPassed;      /*!< CPU register POST test status (PASS/FAIL) */
}CPUreg_struct;

/*!
 * @brief IEC607335 CPU register test POST (Pre Operation Self Test)
 */
extern void _CPUregTestPOST (void);


#endif /* CPUREGS_H_ */
