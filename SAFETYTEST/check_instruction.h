/*******************************************************************************
* File Name          : check_instruction.h
* Author             : Paul
* Version            : V1.0
* Date               : 03/26/2014
* Description        : main code for applications for IEC61508 verification.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CHECK_INS_H
#define __CHECK_INS_H

/* Includes ------------------------------------------------------------------*/
#include "iec61508.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
  /* base 0 */
  unsigned int Instr_Proc_Cnt;     /*!< Instruction Check POST tests passed */
  /* base 4 */
  unsigned int Instr_Err_Cnt;      /*!< Instruction Check POST test status (PASS/FAIL) */
}IEC61508_InstCheck_struct;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*!
 * @brief IEC607335 CPU register test POST (Pre Operation Self Test)
 */
extern void _InstCheckPOST (void);
extern void _STMDB_LDMIA (void);
extern void _BIT_Arithmetic (void);
extern void _CMP_Arithmetic (void);
extern void _DMB_Arithmetic (void);

type_testResult_t  iec61508_InstCheck_POST (void);


#endif /* __CHECK_INS_H */


/******************************  END OF FILE  *********************************/
