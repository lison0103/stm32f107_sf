/**
 * Company	: ThyssenKrupp PDC
 * @file	: check_instruction_post_2.s
 * @purpose     :
 * @brief	: Instruction POST test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 11-April-2014
 * @author	: Paul
*/
;定义数据区
  SECTION .text:DATA  ;REORDER:NOROOT    ;:ROOT
;MyData1
;  DCB 0 ; Destination of calculation result
;  DCD 0xE000E400

;  EXPORT  __Vectors  
  ;//DCD即Define Const DWORD  
;__Vectors       DCD     __initial_sp              ; Top of Stack      

;//[WEAK]选项声明其他的同名标号优先于该标号被引用。  
;//EXPORT提示编译器该标号可以为外部文件引用。  
;//IMPORT通知编译器要使用的标号在其他文件。  
;//使用“=”表示LDR目前是伪指令不是标准指令。 

  SECTION .text:CODE

  PUBLIC  _ITT_Arithmetic  
  PUBLIC  _TBB_Arithmetic  

  EXTERN  InstCheckPOST_struct

;/* Test pattern definition */ 
;pattern1 EQU  0x50000000
;__VALID_PRI_LBIT    EQU     0x03                                  ;/* 优先级最低有效位,与芯片相关 */
;__SYSTICK_PRI       EQU     0xFF                                  ;/* SysTick的优先级，默认为最低 */
;__INT_BASE_PRI      EQU     (0x02 << __VALID_PRI_LBIT)            ;/* 屏蔽优先级阈值-非零，默认为2*/

;;;;;;;;;;;----NO TEST----;;;;;;;;;;;
;BKPT BL CPS CPY LDRSH LDRSB LDRH
;STMIA STRB STRH UXTB CLZ ISB 
;MLA.W MLS.W MOVT.W MOVW.W 
;TEQ.W TST.W 


;TBB
;TBB.W [Rn, Rm] ; PC+= Rn[Rm]*2  ;Rn 指向跳转表的基址，Rm 则给出表中元素的下标
;TBB.W [pc, r0] ; 执行此指令时，PC 的值正好等于branchtable
;branchtable
;  DCB ((dest0 – branchtable)/2) ; 注意：因为数值是8 位的，故使用DCB 指示字
;  DCB ((dest1 – branchtable)/2)
;  DCB ((dest2 – branchtable)/2)
;  DCB ((dest3 – branchtable)/2)
;dest0
;  MOV R1, #0  ; r0 = 0 时执行
;dest1
;  MOV R1, #1  ; r0 = 1 时执行
;dest2
;  MOV R1, #2  ; r0 = 2 时执行
;dest3
;  MOV R1, #3  ; r0 = 3 时执行  

;/**************************************/
;/* START of the _TBB_Arithmetic test  3*/
;/**************************************/
_TBB_Arithmetic
;/* Push ALL registers to stack */
  push {r0-r12,r14}
  
  MOV R0, #1
  TBB.W   [PC, R0]               ; 执行此指令时，PC 的值正好等于__SwiFunction                                     
    DATA
__DcbFunction
    DCB     ( (Donothing - __DcbFunction) / 2 )      ;0   ;注意：因为数值是8 位的，故使用DCB 指示字
    DCB     ( (Addone  - __DcbFunction) / 2 )        ;1   ;DCB 分配一片连续的字节存储单元并赋值初始化
Tbhw      ;label
  MOV R0, #0    
      
  TBH.W   [PC, R0,LSL #1]                                                    
    DATA
DcbFunctionAdd       
    DC16    ( (Addtwo  - DcbFunctionAdd) / 2 )               ; 0           
;  DC16    ( (ClrvicIrqFunc    - SwiFunctionAdd) / 2 )     ; 1            


Tbbreturn  ;label
  THUMB    ;指示编译器32 位Thumb-2 指令

;CPS <effect>, <iflags>    ;改变处理器状态  CM3 没有“A”位

;CPY 把一个寄存器的值拷贝到另一个寄存器中
  MOV R1, #1
  CPY R0, R1
  CMP R0, R1
  BNE _TBB_Arithmetic_test_fail
  
;LDRSH  ;加载半字到寄存器，再带符号扩展到32位
  ;LDRSH sXi, [PssX, #2]        
  ;LDRSH sXr, [PssX]
  ;ADD PssX, PssX, offset

;LDRSB ;从存储器中加载字节，再经过带符号扩展后存储在一个寄存器中
 ;LDRSB    R0,[R0, #+0]

;LDRH  ;从存储器中加载半字到一个寄存器中


;STMIA ;存储多个字，并且在存储后自增基址寄存器 
;STRB  ;存储寄存器中的 低字节
 ;LDR      R1,??DataTable74  
 ;STRB     R0,[R1, #+0]

;STRH  ;存储寄存器中的 低半字
  ;STRH     R5,[R0, #+0]

;STRH半字数据存储指令；
;STRB字节数据存储指令

;UXTB 
;CLZ 
;ISB 
;MLA.W 
;MLS.W 
;MOVT.W 
;MOVW.W 
;TEQ.W 
;TST.W 

;ADC 带进位加法指令
;ADC指令完成的功能是将操作数1加上操作数2，再加上标志位C的值，结果送到目的寄存器
;ADDS  R1， R1，#10   ;结果影响标志位
;ADD   R1， R1， R2   ;结果不影响标志位
;ADD   R3， R1， R2， LSL #2   ; R3 = R1 + ( R2 << 2 )

  MOV R0, #0xff
  MOV R1, #0xff
  ADDS R1, R0, R1
  CMP R1, #510
  ADC R0, R0, R1  ;766

  MOV R0, #0
  MOV R1,#0xffff
  ADD R0, R0, R1  ;65535
  ADC R0, R1, #0  ;65536
  

;SBC指令完成的功能是将操作数1减去操作数2，再减去标志位C的取反值，结果送到目的寄存器
;SUB  R0， R1， #256  ；R0 ＝ R1 － 256 ， 结果不影响标志位
;SUBS  R0， R2，R3，LSL #1 ；R0 = R2 － ( R3 <<1 )，结果影响标志位
;SUB  SP ， #380   ；SP ＝ SP － 380
;SBC  R0， R1， R2 ；R0 ＝ R1 － R2 － !C
;RSC  R0， R1， R2 ；R0 ＝ R2 － R1 － !C

  MOV R1, #0
  MOV R2, #0xFF
  SUBS R0, R1, #256
  CMP R0, #256
  SBC R0, R1, R2    ;FFFF FF01
  
  MOV R1, #0
  MOV R2, #0xFF
  SUB R0, R1, #256  ;FFFF FF00
  SBC R0, R1, R2    ;FFFF FF01

;  RSC R0,R1,R2      ;R0 ＝ R2 － R1 － !C not available in THUMB mode

;  ADRL R0, ThumbFun + 1    ;生成分支地址并置最低位为 1
;  BLX R0                    ;跳转到R0所指定的地址，并切换处理器到Thumb工作状态

  ;BL｛条件｝  目标地址
  ;指令在跳转之前，会在寄存器R14中保存当前的下一条指令的地址，
  ;因此，可以通过将R14重新加载到PC中，来返回到跳转指令之后的那条指令处执行。
  ;该指令是实现子程序调用的一种常用手段

  ;BKPT  16位的立即数
  ;功能：用于产生软件断点中断，执行时中断正常指令，进入相应的调试子程序

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
;Thumb汇编指令
  MVN R1, #0xFF    ;R1 ← 0X FFFF FF00
  ;MVN R1, R2
  MOV PC, R14      ;将寄存器R14的值传送给PC，用于子程序返回。

;/*********************************************************************************************************
;** Function name:           Donothing
;** Descriptions:            不调用此函数
;** input parameters:        none
;** output parameters:       none
;** Returned value:          none                          
;*********************************************************************************************************/
Donothing  ; r0 = 0 时执行
    BX LR

;/*********************************************************************************************************
;** Function name:           Addone
;** Descriptions:            调用此函数，增一
;** input parameters:        none
;** output parameters:       none
;** Returned value:          none                          
;*********************************************************************************************************/
Addone     ; r0 = 1 时执行
  LDR  R1, =InstCheckPOST_struct     /*COUNT*/
  LDRB R0, [R1]                      /*LDRB*/
  ADD  R0,R0,#0x01
  STR  R0, [R1]
  B Tbhw
    
;/*********************************************************************************************************
;** Function name:           Addtwo
;** Descriptions:            调用此函数，再增一
;** input parameters:        none
;** output parameters:       none
;** Returned value:          none                          
;*********************************************************************************************************/
Addtwo     ; r0 = 0 时执行
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
  ;CMP  R0, R1  ; R0与R1比较，做R0-R1的操作 ;指令将第一操作数减去第二操作数，但不存储结果，只更改条件标志位
  ;Conditional instruction is only allowed inside IT block
  ;ADDHI  R0, R0, #1  ;若R0 > R1, 则R0 = R0 + 1
  ;ADDLS  R1, R1, #1  ; 若R0 <= R1, 则R1 = R1 + 1

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


  END   ;通知汇编程序它已达到源文件的末尾
  