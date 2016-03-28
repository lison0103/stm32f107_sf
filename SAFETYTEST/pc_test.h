/**
 * Company	: ThyssenKrupp PDC
 * @file	: pc_tes.h
 * @purpose     :
 * @brief	: header for c based PC test 
 *                  POST and BIST PC test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 8-May-2014
 * @author	: Paul
*/

#ifndef __cb_pc_
#define __cb_pc_

#include "iec61508.h"


type_testResult_t IEC61508_PCTest_POST(void);
type_testResult_t IEC61508_PCTest_BIST(void);

#endif
