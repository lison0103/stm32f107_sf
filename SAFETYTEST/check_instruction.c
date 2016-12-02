/*******************************************************************************
* File Name          : check_instruction.c
* Author             : Paul
* Version            : V1.0
* Date               : 03/26/2014
* Description        : main code for applications for IEC61508 verification.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "check_instruction.h"
#include "check_instruction_2.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

IEC61508_InstCheck_struct InstCheckPOST_struct;
IEC61508_InstCheck_struct InstCheckBIST_struct;


/*******************************************************************************
* Function Name  : iec61508_InstCheck_POST
* Description    : Check instruction.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
type_testResult_t iec61508_InstCheck_POST (void)
{
    type_testResult_t testResult = IEC61508_testPassed;
    
    _TBB_Arithmetic();   
    _ITT_Arithmetic();    
    _DMB_Arithmetic();   
    _CMP_Arithmetic();  
    _BIT_Arithmetic();  
    _STMDB_LDMIA();
    _InstCheckPOST();
    if (InstCheckPOST_struct.Instr_Err_Cnt >= IEC61508_testPassed)
    {   
        testResult = IEC61508_testFailed;
    }
    
    return testResult;
}

/******************************  END OF FILE  *********************************/
