/**
 * Company	: ThyssenKrupp PDC
 * @file	: cpu_reg_test.c
 * @purpose     :
 * @brief	: CPU Register POST tests for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 22-April-2014
 * @author	: Paul
*/

#include <stdint.h>
#include "cpu_reg_test.h"
#include "ram_test.h"

CPUreg_struct CPUregTestPOST_struct;

//type_testResult CPUregTest_POST (void)
type_testResult_t CPUregTest_POST (void)
{
	_CPUregTestPOST();
	if (CPUregTestPOST_struct.testPassed != IEC61508_testPassed) return (IEC61508_testFailed);

	return IEC61508_testPassed;
}

