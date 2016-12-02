/**
 * Company	: ThyssenKrupp PDC
 * @file	: check_instruction_post_2.s
 * @purpose     :
 * @brief	: Instruction POST test for IEC61508 verification.
 * @version	: 1.0
 * @date	: 11-April-2014
 * @author	: Paul
*/

;Defined data area
  SECTION .text:DATA  ;REORDER:NOROOT    ;:ROOT
;MyData1
;  DCB 0 ; Destination of calculation result
;  DCD 0xE000E400

;  EXPORT  __Vectors  
  ;//DCD¼´Define Const DWORD  
;__Vectors       DCD     __initial_sp              ; Top of Stack      

; [WEAK] option other statements of the same name on the label is preferentially reference numerals.
; EXPORT hint to the compiler that the label can be referenced as an external file.
; Numeral IMPORT tells the compiler to use in other documents.
; Use the "=" indicates LDR pseudo-instruction is currently not a standard instruction.

  SECTION .text:CODE

  PUBLIC  _ITT_Arithmetic  
  PUBLIC  _TBB_Arithmetic  

  EXTERN  InstCheckPOST_struct


;/**************************************/
;/* START of the _TBB_Arithmetic test  3*/
;/**************************************/
_TBB_Arithmetic
;/* Push ALL registers to stack */
  push {r0-r12,r14}
  
  MOV R0, #1
  TBB.W   [PC, R0]               ;Execution of this instruction, the PC value exactly equal __SwiFunction                                     
    DATA
__DcbFunction
    DCB     ( (Donothing - __DcbFunction) / 2 )      ;0   ;Note: Because the value is 8 bits, so the use of DCB pointer
    DCB     ( (Addone  - __DcbFunction) / 2 )        ;1   ;DCB assigned a contiguous bytes of storage units and assign initialization
Tbhw      ;label
  MOV R0, #0    
      
  TBH.W   [PC, R0,LSL #1]                                                    
    DATA
DcbFunctionAdd       
    DC16    ( (Addtwo  - DcbFunctionAdd) / 2 )               ; 0           
;  DC16    ( (ClrvicIrqFunc    - SwiFunctionAdd) / 2 )     ; 1            


Tbbreturn  ;label
  THUMB    ;It instructs the compiler 32-bit Thumb-2 instruction


;CPY Copy the value of a register to another register
  MOV R1, #1
  CPY R0, R1
  CMP R0, R1
  BNE _TBB_Arithmetic_test_fail

  MOV R0, #0xff
  MOV R1, #0xff
  ADDS R1, R0, R1
  CMP R1, #510
  ADC R0, R0, R1  ;766

  MOV R0, #0
  MOV R1,#0xffff
  ADD R0, R0, R1  ;65535
  ADC R0, R1, #0  ;65536
  

;SBC instruction performs the function operand 1 operand minus 2, 
;minus flag C negated value of the result to the destination register
;SUB  R0£¬ R1£¬ #256  £»R0 £½ R1 £­ 256 £¬ The results do not affect the flag
;SUBS  R0£¬ R2£¬R3£¬LSL #1 £»R0 = R2 £­ ( R3 <<1 )£¬Results Affected flag
;SUB  SP £¬ #380   £»SP £½ SP £­ 380
;SBC  R0£¬ R1£¬ R2 £»R0 £½ R1 £­ R2 £­ !C
;RSC  R0£¬ R1£¬ R2 £»R0 £½ R2 £­ R1 £­ !C

  MOV R1, #0
  MOV R2, #0xFF
  SUBS R0, R1, #256
  CMP R0, #256
  SBC R0, R1, R2    ;FFFF FF01
  
  MOV R1, #0
  MOV R2, #0xFF
  SUB R0, R1, #256  ;FFFF FF00
  SBC R0, R1, R2    ;FFFF FF01

;  RSC R0,R1,R2      ;R0 £½ R2 £­ R1 £­ !C not available in THUMB mode

;  ADRL R0, ThumbFun + 1    ;Generate the branch address and the least significant bit is set to 1
;  BLX R0                   ;Jump to the address specified in R0, and switch the processor to Thumb state work

   ; BL {condition} destination address
   ; Instruction before the jump, will save the current address of the next instruction in the register R14,
   ; Therefore, you can reload the R14 into the PC, to return to the instruction after the execution of the jump instruction.
   ; This instruction is a common means of achieving the subroutine call

   ; BKPT 16-bit immediate data
   ; Function: generate software breakpoint interrupt normal instruction execution, enter the appropriate debugging routines

_TBB_Arithmetic_test_pass
  LDR  R1, =InstCheckPOST_struct     
  B    _TBB_Arithmetic_test_exit

_TBB_Arithmetic_test_fail
  LDR  R1, =InstCheckPOST_struct+4     
  
_TBB_Arithmetic_test_exit  
  LDRB R0, [R1]                      
  ADD  R0,R0,#0x01
  STR  R0, [R1]
;/* Pop the stack back */
  pop {r0-r12,r14}
;/* Branch back */
  bx  lr
;/**************************************/
;/* END of the _TBB_Arithmetic test     */
;/**************************************/


ThumbFun
;Thumb Assembly instructions
  MVN R1, #0xFF    ;R1 ¡û 0X FFFF FF00
  ;MVN R1, R2
  MOV PC, R14      ;R14 register value is transferred to the PC, for subroutine return¡£

;/*********************************************************************************************************
;** Function name:           Donothing
;** Descriptions:            Do not call this function
;** input parameters:        none
;** output parameters:       none
;** Returned value:          none                          
;*********************************************************************************************************/
Donothing  ; r0 = 0 Ê±Ö´ÐÐ
    BX LR

;/*********************************************************************************************************
;** Function name:           Addone
;** Descriptions:            Call this function to add a
;** input parameters:        none
;** output parameters:       none
;** Returned value:          none                          
;*********************************************************************************************************/
Addone     ; r0 = 1 execution
  LDR  R1, =InstCheckPOST_struct     /*COUNT*/
  LDRB R0, [R1]                      /*LDRB*/
  ADD  R0,R0,#0x01
  STR  R0, [R1]
  B Tbhw
    
;/*********************************************************************************************************
;** Function name:           Addtwo
;** Descriptions:            Call this function to add a
;** input parameters:        none
;** output parameters:       none
;** Returned value:          none                          
;*********************************************************************************************************/
Addtwo     ; r0 = 0 execution
  LDR  R1, =InstCheckPOST_struct     /*COUNT*/
  LDRB R0, [R1]                      /*LDRB*/
  ADD  R0,R0,#0x01
  STR  R0, [R1]
  B Tbbreturn

;/**************************************/
;/* START of the _ITT_Arithmetic test     5*/
;/**************************************/
_ITT_Arithmetic
;/* Push ALL registers to stack */
  push {r0-r12,r14}

;if  ( a > b )  a++;
;else  b++;
  ;MOV R0, #0
  ;MOV R1, #1
  ;CMP  R0, R1  ; R0 compared with R1, R0-R1 to do an operation; the first operand instruction subtracts the second operand, but the results are not stored, only the change condition flag
  ;Conditional instruction is only allowed inside IT block
  ;ADDHI  R0, R0, #1  ;if R0 > R1, R0 = R0 + 1
  ;ADDLS  R1, R1, #1  ;if R0 <= R1, R1 = R1 + 1

;if (r0==r1)
;{r3=r4+r5; r3=r3/2;}
;else
;{r3=r6+r7; r3=r3/2;}

  MOV R0, #1
  MOV R1, #2
  MOV R4, #5
  MOV R5, #6
  MOV R6, #8
  MOV R7, #9
  CMP R0, R1
  ITTEE EQ           ;if R0==R1, Then-Then-Else-Else
  ADDEQ R3, R4, R5   ;equal add
  ASREQ R3, R3, #1   ;equal shift right
  ADDNE R3, R6, R7   ;no equal add
  ASRNE R3, R3, #1   ;no equal shift right
  CMP R3, #8
  bne _ITT_Arithmetic_test_fail

_ITT_Arithmetic_test_pass
  LDR  R1, =InstCheckPOST_struct     /*COUNT*/
  B    _ITT_Arithmetic_test_exit
  
_ITT_Arithmetic_test_fail
  LDR  R1, =InstCheckPOST_struct+4     /*COUNT*/
  
_ITT_Arithmetic_test_exit
  LDRB R0, [R1]                      /*LDRB*/
  ADD  R0,R0,#0x01
  STR  R0, [R1]
;/* Pop the stack back */
  pop {r0-r12,r14}
;/* Branch back */
  bx  lr
;/**************************************/
;/* END of the _ITT_Arithmetic test     */
;/**************************************/


  END   ;Inform the assembler that it has reached the end of the source file
  