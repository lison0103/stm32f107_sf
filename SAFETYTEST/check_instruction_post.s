/**
 * Company	: ThyssenKrupp PDC
 * @file	: check_instruction_post.s
 * @purpose     :
 * @brief	: Instruction POST test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 26-March-2014
 * @author	: Paul
*/

; Defined data area
;  AREA | Header Data|, DATA
;  ALIGN 4
  SECTION .text:DATA(2)  ;REORDER:NOROOT    ;:ROOT     ;:NOROOT
MY_NUMBER
  DCD 0x12345678  ;It is a data record
  DCD 0xABCDEF00

  ;PUBLIC MY_NUMBER
  ;SECTION .text:CODE:NOROOT:REORDER(1)
  ;SECTION .text:CODE:REORDER:NOROOT(2)  
  SECTION .text:CODE

  PUBLIC  _InstCheckPOST	         ;/*POST (Pre Operation Self Test)*/
                                         ;/*BIST (Build in Self Test)*/
  PUBLIC  _STMDB_LDMIA  
  PUBLIC  _BIT_Arithmetic  
  PUBLIC  _CMP_Arithmetic  
  PUBLIC  _DMB_Arithmetic  
  
  EXTERN  InstCheckPOST_struct

;/* Test pattern definition */ 
pattern1 EQU  0x50000000
patternnum1 EQU 0x11223344
patternnum2 EQU 0x44332211
patternnum3 EQU 0x22114433
patternnum4 EQU 0xffffffaa

;patternnum5 EQU 0x11111110  ;0x1111110FEEEEEEF0 -> FFFFFFFF,EEEEEEF0
patternnum5 EQU 0xaaaaaaa0   ;0xAAAAAA9F55555560 -> 00000000,55555560
patternnum6 EQU 0xffffffff
patternnum7 EQU 0xAAAAAA9F
patternnum8 EQU 0x55555560
patternnum9 EQU 0xAAAAAAC0
patternum10 EQU 0x5555553E
patternum11 EQU 0xFFFFFFE0
;/* Test structure offset definitions */
testState  EQU 0x0
testPassed EQU 0x4
LOCKED   EQU 1
UNLOCKED EQU 0

;;;;;;;;;;;----NO TEST----;;;;;;;;;;;
; ADDS.W ADD.W
; Isolation instruction
; DMB DSB ISB
; Saturation operation instruction, # imm5 saturation boundary is used to specify the range 1-32
; SSAT.W Rd, # imm5, Rn, {, shift}
; CBZ; Jump narrower, only 0-126
; SDIV
; LDRD STRD

;;;;;;;;;;;----TEST----;;;;;;;;;;;
;ADD ADDS 
;CMP CMN TST TEQ MRS MSR  CBNZ UDIV REV REVH REV16 REVSH
;SXTB SXTH SXTB UXTH   BFC BFI UBFX SBFX 

;/**************************************/
;/* START of the _DMB_Arithmetic test     5*/
;/**************************************/
_DMB_Arithmetic
;/* Push ALL registers to stack */
  push {r0-r12,r14}

  LDR R3, =MY_NUMBER    ;R3= MY_NUMBER
  LDRD.W R0, R1,[R3]    ;R1= 0xABCD_EF00, R0=0x1234_5678
  STRD.W R1, R0,[R3]    ;(MY_NUMBER)=0xABCDEF00_12345678  ;Memory 64-bit integer; to achieve a double word word order inversion operation
  ;DMB                  ;Ensure complete STR     
  ;LDRD.W R0, R1,[R3]   ;R0= 0xABCD_EF00, R1=0x1234_5678
  LDR R4, [R3]          ;R4= *R3 R4= 0xABCD_EF00 
  CMP R4, R1
  bne _DMB_Arithmetic_test_fail
  LDR R5, [R3,#0x04]    ;R5=0x1234_5678
  CMP R5, R0
  bne _DMB_Arithmetic_test_fail

  ;STM r0, {r2,r3}
  ;LDRD.W R0, R1,[R2]  ;R0= 0xABCD_EF00, R1=0x1234_5678
  ;STRD.W R1, R0,[R2]  ;(0x1000)=0xABCD_EF00_1234_5678 : 78 56 34 12 00 EF CD AB 
  ;STRD Memory 64-bit integer
  ;STRD.W R1, R0, [R2]  ;After the execution, (0x1000) = 0xABCD_EF00_1234_5678, in order to achieve the double word word order inversion operation.

;lock_mutex
;       MOV R1, #LOCKED               ; and "locked" Comparison
;       WFE
;       DMB                           ; ensure access to resources has ended
;       MOV R1, #UNLOCKED             ; write to the lock region "unlocked"
;       DSB                           ; ensure completion before the CPU wake mutex status updates
;       SEV                           ; send events like the other CPU, the CPU waits for an event to wake any
;       CMP R1, #UNLOCKED             ; and "locked" Comparison  
;      BNE lock_mutex         
    
; There is no middle DMB STR and BL instructions, we can not guarantee this STR instructions before you copy the code in the end portion of the memory has been completed,
; Because replication routines may be written by STR also write data buffer (Write Buffer) to run when
; DMB DMB instruction before forcing all data access is complete.
; ISB instruction to prevent the instruction fetch from RAM copy the code before the end of
; Flash memory mapped address 0x0800 0000 ~ 0x0801 FFFF, namely the size of 128KB
;     MOV R0, #0
; LDR R1, = 0x08000000  ; #REMAP_REG
; STR R0, [R1]          ; close the flash map
; DMB                   ; ensure complete STR
      // BL block_copy_routine (); copy the code into RAM
; ISB                   ; ensure pipeline Empty
      // BL copied_routine (); run after copying the code (RAM in)
    
; LDR R0, = 0x00000008; = GIC_CPUIF_BASE; obtain the base address of the interrupt controller
; LDR R1, [R0, # 0x0c]  ; read IAR, while lifting nIRQ signal
; DSB                   ; ensure that the memory access is complete, and no other instruction execution

_DMB_Arithmetic_test_pass
  LDR  R1, =InstCheckPOST_struct     
  B     _DMB_Arithmetic_test_exit
 
_DMB_Arithmetic_test_fail  
  LDR  R1, =InstCheckPOST_struct 
  
_DMB_Arithmetic_test_exit  
  LDRB R0, [R1]                      
  ADD  R0,R0,#0x01
  STR  R0, [R1]
;/* Pop the stack back */
  pop {r0-r12,r14}
;/* Branch back */
  bx  lr
;/**************************************/
;/* END of the _DMB_Arithmetic test     */
;/**************************************/


;/**************************************/
;/* START of the _CMP_Arithmetic test     11*/
;/**************************************/
_CMP_Arithmetic
;/* Push ALL registers to stack */
  push {r0-r12,r14}

  LDR R0, =0xeeff5599
  LDR R2, =0x1100aa44
  ADDS.W R0, R0, R2   ;32-bit Thumb-2 instruction, and update flag; R0 = FFFFFFDD
  ADD.W R0, R0, R2    ;32-bit Thumb-2 instructions, but do not update flags

  LDR R0, =0x2000
  SSAT.W R1, #12, R0    ;R0 = 0x2000 (8192) output R1 = 0x7FF (2047) Q flag 112-bit signed integer (-2048 to 2047)
  LDR R4, =0x7ff  
  CMP R4, R1
  bne _CMP_Arithmetic_test_fail
  
  USAT.W R1, #12, R0    ;12-bit unsigned integer (0-4095) R0 = 0x2000 (8192) output R1 = 0xFFF (4095) Q flag 1
  LDR R4, =0xfff  
  CMP R4, R1
  bne _CMP_Arithmetic_test_fail
  
;SSAT{.W} R1, #12, R0  ;R0 = 0x2000 (8192) output R1 = 0x7FF (2047) CPSR flag 112 in the Q-bit signed integer (-2048 to 2047)
;USAT{.W} R1, #12, R0  ;12-bit unsigned integer (0-4095) R0 = 0x2000 (8192) output R1 = 0xFFF (4095) Q flag 1

  MOV R0, #1
  CBZ R0, _CMP_Arithmetic_test_fail
;CBZ <Rn>, <label>

  LDR R0, =300
  MOV R1, #5
  SDIV.W R2, R0, R1   ;R2= 300/5 =60
  MOV R4, #60
  CMP R4, R2
  bne _CMP_Arithmetic_test_fail

  MOV R3, #0xa955
  RSB R1, R3, #0xff00        ;R1=0xff00 - R3         /*RSB*/   ;R1=55ab
  RSBS R2, R1, R1, LSL #2    ;R2=R1<<2-R1 = R1*3     /*RSBS*/  ;R2=10101

  LDR R4, =0x10101
  CMP R4, R2
  bne _CMP_Arithmetic_test_fail
  
  RSBS R0,R1,#0              ;R0=-R1                 /*RSBS*/  ;R0=FFFFAA55
  LDR R4, =0xffffaa55  
  CMP R4, R0
  bne _CMP_Arithmetic_test_fail

;Use ADC to achieve 64-bit adder£¬(R1,R0)=(R1,R0)+(R3,R2)
  LDR R0, =0xeeff5599
  LDR R2, =0x1100aa44
  ADDS R0, R0, R2            /*ADDS*/    ;R0=FFFFFFDD
  ADC  R1, R1, R3            /*ADC*/     ;R1=ff00
  LDR R4, =0xff00
  CMP R4, R1
  bne _CMP_Arithmetic_test_fail

;Use SBC achieve 64-bit subtraction£¬(R1,R0)=(R1,R0)-(R3,R2)
  SUBS R0, R0, R2            /*SUBS*/    ;R0-R2=EEFF5599
  SBC R1, R1, R3             /*SBC*/     ;R1-R3=55AB
  LDR R4, =0x55ab
  CMP R4, R1
  bne _CMP_Arithmetic_test_fail

;Implement using RPC request value of negative 64£¬(R3,R2)  R3=0-R1 R2=0-R0
  ;RSBS R2, R0, #0
  ;RSC R3, R1, #0
  
;R0 + 1, determine complement R0 is 1, and if Z is set
  MOV R0, #1
  NEG R0, R0                 /*r1=0x0*/       /*NEG*/
  CMN R0, #1                 /*CMN*/
  bne _CMP_Arithmetic_test_fail
;Low four judges R1 is 0
  MOV R1, #0xf0
  TST R1, #0x0f              /*TST*/ 
  bne _CMP_Arithmetic_test_fail
;Compare R0 and R1 are equal
  MOV R0, #0xaa55
  MOV R1, #0xaa55
  TEQ R0, R1                 /*TEQ*/
  bne _CMP_Arithmetic_test_fail

_CMP_Arithmetic_test_pass
  LDR  R1, =InstCheckPOST_struct 
  B     _CMP_Arithmetic_test_exit

_CMP_Arithmetic_test_fail
  LDR  R1, =InstCheckPOST_struct+4
  
_CMP_Arithmetic_test_exit  
  LDRB R0, [R1]                      
  ADD  R0,R0,#0x01
  STR  R0, [R1]
;/* Pop the stack back */
  pop {r0-r12,r14}
;/* Branch back */
  bx  lr
;/**************************************/
;/* END of the _CMP_Arithmetic test     */
;/**************************************/


;/**************************************/
;/* START of the _BIT_Arithmetic test     21*/
;/**************************************/
_BIT_Arithmetic
;/* Push ALL registers to stack */
  push {r0-r12,r14}
;/*Bitwise AND
  MOV R0, #0xaaa
  MOV R1, #0x555
  AND R0, R0, R1          ; R0 &= R1
  AND.W R0, R1, #0x55     ; R0 = R1 & imm12  /*AND.W imm12*/
  CMP R0, #0x55
  bne _BIT_Arithmetic_test_fail
  
  MOV R3, #0xaaa
  AND.W R1, R0, R3        ; R1 = R0 & R3     /*AND.W*/
  CMP R1, #0
  bne _BIT_Arithmetic_test_fail
  
;/*Bitwise or
  MOV R0, #0xaaa
  MOV R1, #0x555
  ORR R2, R0, R1           ; Rd |= Rn          /*ORR*/
  ORR.W R2, R1, #0x55      ; Rd = Rn | imm12   /*ORR.W imm12*/
  LSR R2, R2, #0x4
  CMP R2, #0x55
  bne _BIT_Arithmetic_test_fail
  
  LDR R0, =patternnum5
  LDR R1, =patternnum8
  ORR.W R2, R1, R0         ; Rd = Rm | Rn      /*ORR.W*/
  LDR R3, =patternum11
  CMP R2, R3
  bne _BIT_Arithmetic_test_fail

;/*Bit segment is cleared
  LDR R0, =0x1234FFFF
  BFC R0, #4, #10            ;R0= 0x1234C00F  /*BFC*/
  LDR R1, =0x1234C00F
  CMP R0, R1
  bne _BIT_Arithmetic_test_fail

  LDR R0, =0x12345678
  LDR R1, =0xAABBCCDD
  BFI.W R1, R0, #8, #16      ;R1= 0xAA5678DD  /*BFI.W*/
  LDR R2, =0xAA5678DD
  CMP R1, R2
  bne _BIT_Arithmetic_test_fail

  LDR R0, =0x5678ABCD
  UBFX.W R1, R0, #12,#16     ;R1=0x0000678A    /*UBFX.W*/
  LDR R2, =0x0000678A
  CMP R1, R2
  bne _BIT_Arithmetic_test_fail
  
  LDR R0, =0x5678ABCD
  SBFX.W R1, R0, #8,#4       ;R1=0xFFFFFFFB   /*SBFX.W*/
  LDR R2, =0xFFFFFFFB
  CMP R1, R2
  bne _BIT_Arithmetic_test_fail

  BIC R0, R0, #1             /*BIC*/          /*R0 &= ~1*/ 
  BIC.W R0, R0, #0x1E        /*BIC.W imm12*/  /*R0 = R0 & ~imm12 0x5678ABC0*/
  LDR R2, =0x5678ABC0
  CMP R0, R2
  bne _BIT_Arithmetic_test_fail
  BIC.W R1, R1, R0           /*BIC.W*/        /*R1 = R1 & ~R0    0xA987543B*/
  LDR R2, =0xA987543B
  CMP R1, R2
  bne _BIT_Arithmetic_test_fail
  
;/*Bitwise or inverted the source operand bit negated, then performs a bitwise OR
  LDR R0, =0xaa55aa55
  ORN.W R0, R0, #0xaa      ; R0 = R0 | ~imm12    /*ORN.W imm12*/
  LDR R2, =0xffffff55
  CMP R0, R2
  bne _BIT_Arithmetic_test_fail

  LDR R1, =0xffffffa0
  ORN.W R0, R0, R1         ; R0 = R0 | ~R1     /*ORN.W*/  /*0xffffff5f*/
  LDR R2, =0xffffff5f
  CMP R0, R2
  bne _BIT_Arithmetic_test_fail


;/*(Bit) exclusive OR, XOR are always bits
  LDR R0, =0xff0055aa
  LDR R1, =0xf0f0aa55
  EOR R1, R1, R0                    ; Rd ^= Rn          /*EOR*/
  LDR R2, =0xff0ffff
  CMP R1, R2
  bne _BIT_Arithmetic_test_fail

  EOR.W R0, R1, #0xf0               ; Rd = Rn ^ imm12   /*EOR.W*/
  LDR R2, =0xff0ff0f
  CMP R0, R2
  bne _BIT_Arithmetic_test_fail
  EOR.W R3, R1, R0                  ; Rd = Rm ^ Rn      /*EOR.W*/
  MOV R2, #0xf0
  CMP R3, R2
  bne _BIT_Arithmetic_test_fail

;/* Register offset addressing
  MOV R2, #1
  MOV R0, R2, LSL #3           /*R0=R2*8*/
  MOV R3, #3
;  ANDS.W R1, R1, R2, LSL R3
  ANDS.W R1, R1, R2, LSL #3
;  ADD R1, R1, R2, LSL #3
  SUB R1, R1, R2, LSR#2        /*R1=R1-R2*4*/

_BIT_Arithmetic_test_pass
  LDR  R1, =InstCheckPOST_struct     /*COUNT*/
  B     _BIT_Arithmetic_test_exit
  
_BIT_Arithmetic_test_fail
  LDR  R1, =InstCheckPOST_struct+4     
  
_BIT_Arithmetic_test_exit  
  LDRB R0, [R1]                      /*LDRB*/
  ADD  R0,R0,#0x01
  STR  R0, [R1]
;/* Restore the APSR */
  msr APSR, r3
;/* Pop the stack back */
  pop {r0-r12,r14}
;/* Branch back */
  bx  lr
;/**************************************/
;/* END of the _BIT_Arithmetic test     */
;/**************************************/

;/**************************************/
;/* START of the _STMDB_LDMIA test     18*/
;/**************************************/
_STMDB_LDMIA
;/* Push ALL registers to stack */
  push {r0-r12,r14}

;/*Multiply Accumulate, and Multiply Subtract
  MOV R0, #10
  MOV R1, #2
  MOV R2, #3
  MLA R3, R2, R1, R0  ; R3 = R0+R2*R1  /*MLA*/
  CMP R3, #16
  bne _stmdb_ldmia_test_fail
  MLS R3, R2, R1, R0  ; R3 = R0-R2*R1  /*MLS*/
  CMP R3, #4
  bne _stmdb_ldmia_test_fail
  
;/*64-bit signed multiplication
  LDR R0, =patternnum5
  LDR R1, =patternnum6
  SMULL.W R2, R3, R0, R1  ;[R3:R2]= R0*R1     /*SMULL.W*/   #0xAAAAAA9F55555560 
  ;LDR R4, =patternnum7
  CMP R3, #0;
  bne _stmdb_ldmia_test_fail
  LDR R4, =patternnum8
  CMP R2, R4
  bne _stmdb_ldmia_test_fail
  
  SMLAL.W R2, R3, R0, R1  ;[R3:R2]+= R0*R1    /*SMLAL.W*/  *2= #0x5555553EAAAAAAC0
  CMP R3, #0
  bne _stmdb_ldmia_test_fail
  LDR R4, =patternnum9
  CMP R2, R4
  bne _stmdb_ldmia_test_fail
  
;/*Unsigned 64-bit multiplication
  LDR R0, =patternnum5
  LDR R1, =patternnum6
  UMULL.W R2, R3, R0, R1  ;[R3:R2]= R0*R1     /*UMULL.W*/   #0xAAAAAA9F55555560 
  LDR R4, =patternnum7
  CMP R3, R4
  bne _stmdb_ldmia_test_fail
  LDR R4, =patternnum8
  CMP R2, R4
  bne _stmdb_ldmia_test_fail
  
  UMLAL.W R2, R3, R0, R1  ;[R3:R2]+= R0*R1    /*UMLAL.W*/  *2= #0x5555553EAAAAAAC0
  LDR R4, =patternum10
  CMP R3, R4
  bne _stmdb_ldmia_test_fail
  LDR R4, =patternnum9
  CMP R2, R4
  bne _stmdb_ldmia_test_fail

  MOV R0, #0xaa
  SXTB R2, R0                   /*SXTB*/
  LDR R0, =patternnum4
  CMP R0, R2
  bne _stmdb_ldmia_test_fail
  
  LSL R2, R2, #0x8
  MOV R0, #0xaa00
  SXTH R3, R0                  /*SXTH*/
  CMP R3, R2
  bne _stmdb_ldmia_test_fail
  
  LDR R0, =patternnum1    ;0x11223344
  REV.W R1, R0                             /*REV.W*/
  LDR R2, =patternnum2    ;#0x44332211
  CMP R1, R2
  bne _stmdb_ldmia_test_fail
  
;/* Reverse byte order in unsigned short value
  LDR R0, =patternnum1    ;#0x11223344
  REV16.W R1, R0                          /*REV16.W*/
  LDR R2, =patternnum3    ;#0x22114433
  CMP R1, R2
  bne _stmdb_ldmia_test_fail

;/* Reverse byte order in signed short value with sign extension to integer
  MOV R0, #0x005a
  REVSH R0, R0            /*REVSH*/
  MOV R3, R0

  MOV R0, #0x005a 
  UXTH R0, R0             /*UXTH*/ 
  LSLS R0, R0, #16        /*LSLS*/
  RBIT R0, R0             /*RBIT*/
  CMP R3, R0
  bne _stmdb_ldmia_test_fail

;/* Store the current link register */
  mrs r3, APSR                            
;/* Load pattern1 in r0 */
  ldr r0,=pattern1
;/* Move pattern1 in the APSR */
  msr APSR, r0                            /*MSR*/
;/* Read pattern1 from the APSR */
  mrs r1, APSR                            /*MRS*/
;/* Compare the pattern with the input pattern */
  cmp r1, r0
  bne _stmdb_ldmia_test_fail
;/* Restore the APSR */
  msr APSR, r3

  movw r0, #0x00AA
;   /* Load pattern1 in r1 */
  ldr  r1,=pattern1
;   /* Put patter1 in r2-r3 */
  mov r2,r1
  mov r3,r1
    
;  STMDB SP!, {R0-R3, LR} ;PUSH {R0-R3, LR}
  STMDB SP!, {R0-R3} ;PUSH {R0-R3}              /*STMDB*/
  NOP
;  LDMIA SP!, {R0-R3, PC} ;POP {R0-R3, PC}
  LDMIA SP!, {R0-R3} ;POP {R0-R3}               /*LDMIA*/ 
  cmp     r0, #0xAA
  bne     _stmdb_ldmia_test_fail
  cmp     r2,r1
  bne     _stmdb_ldmia_test_fail
  cmp     r3,r1
  bne     _stmdb_ldmia_test_fail  

_stmdb_ldmia_test_pass
  LDR  R1, =InstCheckPOST_struct     /*COUNT*/
  B     _stmdb_ldmia_test_exit
  
_stmdb_ldmia_test_fail
  LDR  R1, =InstCheckPOST_struct+4     /*COUNT*/  
   
_stmdb_ldmia_test_exit  
  LDRB R0, [R1]                      /*LDRB*/
  ADD  R0,R0,#0x01
  STR  R0, [R1]
;/* Restore the APSR */
  msr APSR, r3
;/* Pop the stack back */
  pop {r0-r12,r14}
;/* Branch back */
  bx lr
;/**************************************/
;/* END of the _STMDB_LDMIA test     41*/
;/**************************************/

;/* Start of the Instruction test */
_InstCheckPOST
  MOV R0, #0x1
  ADD R0, R0, #0x0
  CBNZ R0, JNB_TEST

  MOVS R0, #0x01
  LDR  R1, =InstCheckPOST_struct+testPassed  /*ERROR*/
  STR  R0, [R1]  

JNB_TEST
  MOV R0, #0x0
  LDR R1, =InstCheckPOST_struct      /*COUNT*/ /*LDR*/
  ADD R0,R0,#0x01
  STR R0, [R1]                       /*STR*/

  MOV R0,#55H                        /*MOV*/
  ADD R0, R0, #55H                   /*ADD*/
  SUB R0, R0, #11H                   /*SUB*/
  MOV R1, #0xFF                      /*MOV*/
  AND R0, R0, R1                     /*AND*/
  MOV R1, #0x33                      /*MOV*/
  ORR R0, R0, R1                     /*ORR*/
  MOV R1, #0x0F                      /*MOV*/
  EOR R0, R0, R1                     /*EOR*/
  CMP R0, #0xB4                      /*CMP*/
  BNE INS_TEST_FAIL

  LDR  R1, =InstCheckPOST_struct     /*COUNT*/
  LDRB R0, [R1]                      /*LDRB*/
  ADD  R0,R0,#0x01
  STR  R0, [R1]

  MOV R10, #0x00b4
  MOV R0, #0x0002
  UDIV R0, R10,R0
  CMP R0, #0x005a
  BNE INS_TEST_FAIL

  MOV R0, #0x005a
  LSR R0, R0, #0x04      /*0x0005*/         /*LSR*/
  LSL R0, R0, #0x04      /*0x0050*/         /*LSL*/
  MOV R1, #0x04
  ROR R0, R0, R1        /*0x0005*/         /*ROR*/
  MOV R1, #0x0002
  MUL R0, R0, R1        /*0x000A*/         /*MUL*/
  CMP R0, #0x000a                          /*CMP*/
  BNE INS_TEST_FAIL

  LDR  R1, =InstCheckPOST_struct           /*COUNT*/
  LDRB R0, [R1]
  ADD  R0,R0,#0x01
  STR  R0, [R1]

  MOV R0, #0xaaaa
  LSL R0, R0, #0x10        /*0x0050*/        /*LSL*/
  MOV R11, #0x5556 
  LSL R11, R11, #0x10      /*0x0050*/        /*LSL*/
  ADC.W R0, R0, R11        /*r0=1*/          /*ADDC*/

  MOV R11, #0xaa
  MOV R2, #0x55
  SBC.W  R11, R11, R2     /*rl1=0x55*/       /*SUBC*/
  MOVS.W R1, R11                             /*MOVS*/
  SBC.W  R1, R1, R0       /*r1=0x0054 */     /*SUBC*/
  MUL R1, R1, R0                             /*MUL*/
  CMP R1, #0x0054                            /*CMP*/
  BNE INS_TEST_FAIL

  MOV R2,#0x00a8
  UDIV R2, R2, R1                            /*DIV*/
  CMP R2, #0x0002                           /*CMP*/
  BNE INS_TEST_FAIL                         /*0x0002*/

  MOV R1, #0x22222222
  ASR R1, R1, #0x4           /*r1=0x02222222*/       /*ASR*/
  MVN R1, R1                 /*r1=0xfddddddd*/       /*MVN(not)*/
  NEG R1, R1                 /*r1=0x02222223*/       /*NEG*/
  MOV R0, #0x0fff                                    /*MOV*/
  AND R1, R1, R0                                     /*AND*/
  PUSH {R1,LR}                                       /*PUSH*/
  POP {R0,LR}                                        /*POP*/
  NOP                                                /*NOP*/          
  MOV R2, #0x0223
  CMP.W R0, R2                                       /*CMP.W*/
  BNE INS_TEST_FAIL

  MOV R10,#0xaaaaaaaa
  MVN R10, R10                  /*rl0=0x55555555*/            /*MVN*/
  NEG R10, R10                  /*rl0=0xaaaaaaab*/            /*NEG*/
  CMP R10,#0xaaaaaaab
  BNE INS_TEST_FAIL

INS_TEST_PASS
  LDR  R1, =InstCheckPOST_struct           /*COUNT*/
  LDRB R0, [R1]
  ADD  R0,R0,#0x01
  STR  R0, [R1]
  B    EXIT
  
INS_TEST_FAIL
;/* INC ERROR as Inst check fail */
  LDR  R1, =InstCheckPOST_struct+testPassed  /*ERROR*/
  LDRB R0, [R1]
  ADD  R0,R0,#0x01
  STR  R0, [R1]  

EXIT
  BX lr

  END
  