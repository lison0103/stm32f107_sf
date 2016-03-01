/**
 * Company	: ThyssenKrupp PDC
 * @file	: check_instruction.c
 * @purpose
 * @brief	: main code for applications for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 26-March-2014
 * @author	: Paul
*/

#include "stm32f10x.h"
#include "check_instruction.h"
#include "check_instruction_2.h"

IEC61508_InstCheck_struct InstCheckPOST_struct;
IEC61508_InstCheck_struct InstCheckBIST_struct;

//type_testResult iec61508_InstCheck_POST (void)
type_testResult_t iec61508_InstCheck_POST (void)
{
  _TBB_Arithmetic();   //no ok  ->remove ADRL R0, ThumbFun + 1 生成分支地址并置最低位为 1 ->ok
  _ITT_Arithmetic();   //ok 
  _DMB_Arithmetic();   //no ok  ->remove lock_mutex -> ok
  _CMP_Arithmetic();  //ok
  _BIT_Arithmetic();  //ok
  _STMDB_LDMIA();
  _InstCheckPOST();
  if (InstCheckPOST_struct.Instr_Err_Cnt >= IEC61508_testPassed) return (IEC61508_testFailed);
  return IEC61508_testPassed;
}

void Check_Instruction_Set()
{
  //uint32_t instruction_err=0x0;
  //uint32_t u16gvIndex=0x0;
  
  struct inst__type
  { 
    long instruction_err; 
    long u16gvIndex; 
  }; 
  /*TEST INSTRUCTION*/
int i=0;

  __asm 
  (
  "MOV R0, #0x1" "\n\t"
  "ADD R0, R0, #0x0" "\n\t"
  "MOV R0,#55H" "\n\t"                        /*MOVB*/
  "ADD R0, R0, #55H" "\n\t"                        /*ADDB*/
  "SUB R0, R0, #11H" "\n\t"                        /*SUBB*/
  "MOV R1, #0xFF" "\n\t"                        /*MOVB*/
  "AND R0, R0, R1" "\n\t"                       /*ANDB*/
  "MOV R1, #0x33" "\n\t"                        /*MOVB*/
  "ORR R0, R0, R1" "\n\t"                        /*ORB*/
  "MOV R1, #0x0F" "\n\t"                        /*MOVB*/
  "EOR R0, R0, R1" "\n\t"                       /*XORB*/
  "CMP R0, #0xB4" "\n\t"                       /*CMPB*/
  );
i++;
  asm 
  (
  "MOV R0, #0x1 \n"
  "ADD R0, R0, #0x0 \n"
  "CBZ R0, JNB_TEST \n"
    "NOP \n"
//  "ADD inst__type.instruction_err, inst__type.instruction_err, #0x1" "\n\t" 
  "JNB_TEST: \n"
//  "ADD [bx]inst__type.u16gvIndex, #0x1"     
    "NOP \n"
  "MOV R0,#55H" "\n\t"                        /*MOVB*/
  "ADD R0, R0, #55H" "\n\t"                        /*ADDB*/
  "SUB R0, R0, #11H" "\n\t"                        /*SUBB*/
  "MOV R1, #0xFF" "\n\t"                        /*MOVB*/
  "AND R0, R0, R1" "\n\t"                       /*ANDB*/
  "MOV R1, #0x33" "\n\t"                        /*MOVB*/
  "ORR R0, R0, R1" "\n\t"                        /*ORB*/
  "MOV R1, #0x0F" "\n\t"                        /*MOVB*/
  "EOR R0, R0, R1" "\n\t"                       /*XORB*/
  "CMP R0, #0xB4" "\n\t"                       /*CMPB*/
  );
/*
  i++;  
a++;
   asm 
  (
      "DMB  \n"                   //; 保证资源的访问已经结束
      "MOV R1, #0 \n"      //; 向锁定域写"unlocked"
      "STR R1, [R0] \n"
      "DSB \n"                    //; 保证在CPU唤醒前完成互斥量状态更新
      "SEV \n"                   //; 像其他CPU发送事件，唤醒任何等待事件的CPU 

   );

__asm
(
   //"IMPORT gVar \n" 
 // "ldr r0, [in] \n" 
  //push eax 

  "MOV r0, r15 \n" 
  "MOV R0, #0x1 \n"
  "ADD R0, R0, #0x0 \n"
);
i++;
__asm 
(
//  "ADD [bx]inst__type.u16gvIndex, #0x1"     
  "MOV R0, #0x1 \n"
  "ADD R0, R0, #0x0 \n"
//  "CBZ R0, JNB_TEST \n"           
//  "NOP \n"
//  "JNB_TEST: \n"
);
*/

}
