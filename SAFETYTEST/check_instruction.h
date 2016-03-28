#ifndef __CHECK_INS_H
#define __CHECK_INS_H
#include "iec61508.h"

//__no_init int  b@0x1510;
//extern char a; 
__no_init int a @0x1500;
//int a @0x1500;
//int a=1;
//register __no_init char g @ 15;
//__regvar __no_init static UINT8 glDeviceType @5; 

typedef struct {
  /* base 0 */
  unsigned int Instr_Proc_Cnt;     /*!< Instruction Check POST tests passed */
  /* base 4 */
  unsigned int Instr_Err_Cnt;      /*!< Instruction Check POST test status (PASS/FAIL) */
}IEC61508_InstCheck_struct;

/*!
 * @brief IEC607335 CPU register test POST (Pre Operation Self Test)
 */
extern void _InstCheckPOST (void);
extern void _STMDB_LDMIA (void);
extern void _BIT_Arithmetic (void);
extern void _CMP_Arithmetic (void);
extern void _DMB_Arithmetic (void);


//type_testResult iec61508_InstCheck_POST (void);
type_testResult_t  iec61508_InstCheck_POST (void);
void Check_Instruction_Set();

#endif /* __CHECK_INS_H */