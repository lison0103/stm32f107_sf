/**
 * Company	: ThyssenKrupp PDC
 * @file	: pc_test.c
 * @purpose     :
 * @brief	: c based PC test POST and BIST PC test for  
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 8-May-2014
 * @author	: Paul
*/
#include <stdint.h>
#include "iec61508.h"
#include "pc_test.h"

static uint32_t IEC61508_PCTest_FctA(void);
static uint32_t IEC61508_PCTest_FctB(void);
static uint32_t IEC61508_PCTest_FctC(void);
static uint32_t IEC61508_PCTest_FctD(void);
static uint32_t IEC61508_PCTest_FctE(void);
static uint32_t IEC61508_PCTest_FctF(void);

#define RET_FCT_A	1u
#define RET_FCT_B	2u
#define RET_FCT_C	3u
#define RET_FCT_D	5u
#define RET_FCT_E	7u
#define RET_FCT_F	11u

uint32_t IEC61508_PCTest_lastFctTested = RET_FCT_F;

/*!
	\return 
*/
#pragma location = ".IEC61508_behindVectors"
static uint32_t IEC61508_PCTest_FctA(void)
{
	return RET_FCT_A;
}

/*!
	\return 
*/
#pragma location = ".IEC61508_beforeText"
static uint32_t IEC61508_PCTest_FctB(void)
{
	return RET_FCT_B;
}

/*!
	\return 
*/
#pragma location = ".IEC61508_behindText"
static uint32_t IEC61508_PCTest_FctC(void)
{
	return RET_FCT_C;
}

/*!
	\return 
*/
#pragma location = ".IEC61508_lastFashSection"
static uint32_t IEC61508_PCTest_FctD(void)
{
	return RET_FCT_D;
}

/*!
	\return 
*/
#pragma location = ".IEC61508_beforeRAMcode"
static uint32_t IEC61508_PCTest_FctE(void)
{
	return RET_FCT_E;
}

/*!
	\return 
*/
#pragma location = ".IEC61508_behindRAMcode"
static uint32_t IEC61508_PCTest_FctF(void)
{
	return RET_FCT_F;
}

/*!
	\return 
*/
type_testResult_t IEC61508_PCTest_POST(void)
{
  type_testResult_t Result = IEC61508_testFailed;

  if(IEC61508_PCTest_FctA() == RET_FCT_A)
  {
    Result = IEC61508_testPassed;
  }	
  if((Result == IEC61508_testPassed))
  {
      if(IEC61508_PCTest_FctB() != RET_FCT_B)
      {
          Result = IEC61508_testFailed;
      }
  }
  if((Result == IEC61508_testPassed) )
  {
      if(IEC61508_PCTest_FctC() != RET_FCT_C)
      {
          Result = IEC61508_testFailed;
      }
  }
  if((Result == IEC61508_testPassed) )
  {
      if(IEC61508_PCTest_FctD() != RET_FCT_D)
      {
          Result = IEC61508_testFailed;
      }
  }
  if((Result == IEC61508_testPassed) )
  {
      if(IEC61508_PCTest_FctE() != RET_FCT_E)
      {
          Result = IEC61508_testFailed;
      }
  }
  if((Result == IEC61508_testPassed) )
  {
      if(IEC61508_PCTest_FctF() != RET_FCT_F)
      {
          Result = IEC61508_testFailed;
      }
  }
  
  return Result;
}

/*!
	\return 	
*/
type_testResult_t IEC61508_PCTest_BIST(void)
{
	type_testResult_t Result = IEC61508_testFailed;

	switch(IEC61508_PCTest_lastFctTested)
	{
		case RET_FCT_A:
			if(IEC61508_PCTest_FctB() == RET_FCT_B)
                        {
				Result = IEC61508_testPassed;
			}
			IEC61508_PCTest_lastFctTested = RET_FCT_B;
			break;

		case RET_FCT_B:
			if(IEC61508_PCTest_FctC() == RET_FCT_C)
                        {
				Result = IEC61508_testPassed;
			}
			IEC61508_PCTest_lastFctTested = RET_FCT_C;
			break;

		case RET_FCT_C:
			if(IEC61508_PCTest_FctD() == RET_FCT_D)
                        {
				Result = IEC61508_testPassed;
			}
			IEC61508_PCTest_lastFctTested = RET_FCT_D;
			break;

		case RET_FCT_D:
			if(IEC61508_PCTest_FctE() == RET_FCT_E)
                        {
				Result = IEC61508_testPassed;
			}
			IEC61508_PCTest_lastFctTested = RET_FCT_E;
			break;

		case RET_FCT_E:
			if(IEC61508_PCTest_FctF() == RET_FCT_F)
                        {
				Result = IEC61508_testPassed;
			}
			IEC61508_PCTest_lastFctTested = RET_FCT_F;
			break;

		case RET_FCT_F:
		default:
			if(IEC61508_PCTest_FctA() == RET_FCT_A)
                        {
				Result = IEC61508_testPassed;
			}
			IEC61508_PCTest_lastFctTested = RET_FCT_A;
			break;

	}

	return Result;
}
