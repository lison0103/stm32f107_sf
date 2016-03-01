/**
 * Company	: ThyssenKrupp PDC
 * @file	: cpu_reg_test_post.s
 * @purpose     :
 * @brief	: CPU Register POST test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 22-April-2014
 * @author	: Paul
*/

  SECTION .text:CODE

	PUBLIC	_CPUregTestPOST	
  EXTERN	CPUregTestPOST_struct

;/* Test pattern definition */ 
pattern1 	EQU		0xAAAAAAAA
pattern2 	EQU		0xAAAAAAA8
pattern3 	EQU		0x55555555
pattern4 	EQU		0x55555554
pattern5 	EQU		0xA8000000
pattern6 	EQU		0x50000000
;pattern7 	EQU		0x000000A8
;pattern8 	EQU		0x00000050
pattern7 	EQU		0x00000060
pattern8 	EQU		0x00000020

;/* Test structure offset definitions */
testState  	EQU 	0x0
testPassed 	EQU 	0x4

;/* Start of the CPU test */
_CPUregTestPOST

;   /* Push ALL registers to stack */
    push    {r0-r12,r14}

    mov	r12, lr
    cmp	r12, lr
    bne _cpu_low_test_fail

;/*---- LOW REGISTERS r0-r7 --------------------
; * 	First the low registers r0-r7 are tested
; */

;/* r0 register test:
; * Since r0 is the first register to be tested
; * and no other registers may be used, r0 should be tested
; * by only using immediate instructions.
; */
_cpu_low_test
;	/* Since cmp can only compare with an immediate
;	 * 8-bit value, the bits to be tested needs to
;	 * be shifted to the LSB's.
;	 */
;	/* r0 - stuck at 0 test */
	movw	r0, #0x00AA
;	/* r0 - test r0[7:0] */
	cmp 	r0, #0xAA
	bne		_cpu_low_test_fail
;	/* r0 - test r0[15:8] */
	movw	r0, #0xAA00
	lsr		r0, r0, #8
	cmp 	r0, #0xAA
	bne		_cpu_low_test_fail
;	/* r0 - test r0[23:16] */
	movw	r0, #0x0000
	movt	r0, #0x00AA
	lsr		r0, r0, #16
	cmp 	r0, #0xAA
	bne		_cpu_low_test_fail
;	/* r0 - test r0[31:24] */
	movw	r0, #0x0000
	movt	r0, #0xAA00
	lsr		r0, r0, #24
	cmp 	r0, #0xAA
	bne		_cpu_low_test_fail

;	/* r0 - inverted pattern and
;	 * neighbour stuck test
;	 */
	movw	r0, #0x0055
;	/* r0 - test r0[7:0] */
	cmp 	r0, #0x55
	bne		_cpu_low_test_fail
;	/* r0 - test r0[15:8] */
	movw	r0, #0x5500
	lsr		r0, r0, #8
	cmp 	r0, #0x55
	bne		_cpu_low_test_fail
;	/* r0 - test r0[23:16] */
	movw	r0, #0x0000
	movt	r0, #0x0055
	lsr		r0, r0, #16
	cmp 	r0, #0x55
	bne		_cpu_low_test_fail
;	/* r0 - test r0[31:24] */
	movw	r0, #0x0000
	movt	r0, #0x5500
	lsr		r0, r0, #24
	cmp 	r0, #0x55
	bne		_cpu_low_test_fail

;	/*!! NOW r0 is known good !!*/

_cpu_r0_test_pass
 	b		_cpu_r1_r7_test

_cpu_r0_test_fail
	b		_cpu_low_test_fail


;/* Register r1-r7 test:
; * The registers under test will be written with
; * pattern1 = 0xAAAA.AAAA. Each register will be individually
; * compared to r0 for a pass/fail for the test.
; * The second part of the test will write the inverted pattern
; * in the register under test and pass/fail with a cmp
; */
_cpu_r1_r7_test

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern1

;	/* Put patter1 in r1-r7 */
	mov		r1,r0
	cmp		r1,r0
	bne		_cpu_low_test_fail

	mov		r2,r0
	cmp		r2,r0
	bne		_cpu_low_test_fail

	mov		r3,r0
	cmp		r3,r0
	bne		_cpu_low_test_fail

	mov		r4,r0
	cmp		r4,r0
	bne		_cpu_low_test_fail

	mov		r5,r0
	cmp		r5,r0
	bne		_cpu_low_test_fail

	mov		r6,r0
	cmp		r6,r0
	bne		_cpu_low_test_fail

	mov		r7,r0
	cmp		r7,r0
	bne		_cpu_low_test_fail

;	/* Load pattern3 in r0 */
	ldr		r0,=pattern3

;	/* Put patter3 in r1-r7 */
	mov		r1,r0
	cmp		r1,r0
	bne		_cpu_low_test_fail

	mov		r2,r0
	cmp		r2,r0
	bne		_cpu_low_test_fail

	mov		r3,r0
	cmp		r3,r0
	bne		_cpu_low_test_fail

	mov		r4,r0
	cmp		r4,r0
	bne		_cpu_low_test_fail

	mov		r5,r0
	cmp		r5,r0
	bne		_cpu_low_test_fail

	mov		r6,r0
	cmp		r6,r0
	bne		_cpu_low_test_fail

	mov		r7,r0
	cmp		r7,r0
	bne		_cpu_low_test_fail

_cpu_low_test_pass
;	/* Indicate in the CPUregTestPOST_struct
;	 * that r0-r7 tests passed.
;	 */
	mov		r0, #0xFF
	ldr		r1, =CPUregTestPOST_struct+testState
	str		r0, [r1]
	b		_cpu_high_test

_cpu_low_test_fail
	b		_cpu_high_test_fail

;/*--- HIGH REGISTERS r8-r12 ---*/
_cpu_high_test
;	/* Store the LR to r2 */
	mov	r2, r12

;	/* Clear r0 */
	mov	r0, #0x00

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern1

;	/* Put patter1 in r8-r12 */
	mov		r8,r0
	cmp		r8,r0
	bne		_cpu_high_test_fail

	mov		r9,r0
	cmp		r9,r0
	bne		_cpu_high_test_fail

	mov		r10,r0
	cmp		r10,r0
	bne		_cpu_high_test_fail

	mov		r11,r0
	cmp		r11,r0
	bne		_cpu_high_test_fail

	mov		r12,r0
	cmp		r12,r0
	bne		_cpu_high_test_fail

;	/* Load pattern3 in r0 */
	ldr		r0,=pattern3

;	/* Put patter1 in r8-r12 */
	mov		r8,r0
	cmp		r8,r0
	bne		_cpu_high_test_fail

	mov		r9,r0
	cmp		r9,r0
	bne		_cpu_high_test_fail

	mov		r10,r0
	cmp		r10,r0
	bne		_cpu_high_test_fail

	mov		r11,r0
	cmp		r11,r0
	bne		_cpu_high_test_fail

	mov		r12,r0
	cmp		r12,r0
	bne		_cpu_high_test_fail

_cpu_high_test_pass
;	/* Restore the LR to r2 */
	mov		r12, r2
;	/* Indicate in the CPUregTestPOST_struct
;	 * that r8-r12 tests passed.
;	 */
	movw	r0, #0x1F00
	ldr		r1, =CPUregTestPOST_struct+testState
	ldr     r2, [r1]
	orr     r0, r0, r2
	str		r0, [r1]
	b  cpu_MSP_test
;        b  cpu_PSP_test
_cpu_high_test_fail
;	/* Restore the LR to r2 */
	mov		r12, r2
	b		_cpu_MSP_test_fail


;/* MSP register tests:
; * The MSP consists of two tests, the first test the register
; * through reading/writing r13 using the normal instructions.
; * The second test test the register by using the MSR and MRS
; * special register read/write instructions.
; * For the MSP a different pattern is used because MSP[1:0] are
; * always zero.
; */
cpu_MSP_test
;	/* Store the current CONTROL value */
	mrs	 	r3, CONTROL

;	/* Select default stack (MSP) */
	mov 	r6, #0x00
	msr		CONTROL, r6

;	/* Store the current MSP */
	mov		r4, r13

;	/* Check whether MSP is really selected */
	mrs		r5, MSP
	cmp		r4,r5
	bne		_cpu_MSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern2

;	/* Move pattern1 in the MSP */
	mov		r13, r0

;	/* Compare the pattern with the input pattern */
	cmp		r13, r0
	bne		_cpu_MSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern4

;	/* Move pattern1 in the MSP */
	mov		r13, r0

;	/* Compare the pattern with the input pattern */
	cmp		r13, r0
	bne		_cpu_MSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern2

;	/* Move pattern1 in the MSP */
	msr		MSP, r0

;	/* Copy MSP to r1, since MSP can't be directly cmp'ed */
	mrs 	r1, MSP

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_MSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern4

;	/* Move pattern1 in the MSP */
	msr		MSP, r0

;	/* Copy MSP to r1, since MSP can't be directly cmp'ed */
	mrs 	r1, MSP

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_MSP_test_fail_res

_cpu_MSP_test_pass
;	/* Restore the MSP and CONTROL */
	mov 	r13, r4
	msr     MSP, r4
	msr		CONTROL, r6

;	/* Indicate in the CPUregTestPOST_struct
;	 * that MSP tests passed.
;	 */
    movw    r0, #0x2000
    ldr     r1, =CPUregTestPOST_struct+testState
    ldr     r2, [r1]
    orr     r0, r0, r2
    str     r0, [r1]
	b		cpu_PSP_test

_cpu_MSP_test_fail_res
;	/* Restore the MSP and CONTROL */
	mov 	r13, r4
	msr		CONTROL, r6
_cpu_MSP_test_fail
	b		_cpu_PSP_test_fail

;/* PSP register tests:
; * The PSP consists of two tests, the first test the register
; * through reading/writing r13 using the normal instructions.
; * The second test test the register by using the MSR and MRS
; * special register read/write instructions.
; * For the PSP a different pattern is used because MSP[1:0] are
; * always zero.
; */
cpu_PSP_test
;	/* Store the current CONTROL value */
	mrs	 	r3, CONTROL

;	/* Select alternitive stack (PSP) */
	mov 	r6, #0x02
	msr		CONTROL, r6
	mrs     r4, CONTROL

;	/* Store the current PSP */
	mov		r4, r13

;	/* Check whether PSP is really selected */
	mrs		r5, PSP
	cmp		r4, r5
	bne		_cpu_PSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern2

;	/* Move pattern1 in the PSP */
	mov		r13, r0

;	/* Compare the pattern with the input pattern */
	cmp		r13, r0
	bne		_cpu_PSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern4

;	/* Move pattern1 in the PSP */
	mov		r13, r0

;	/* Compare the pattern with the input pattern */
	cmp		r13, r0
	bne		_cpu_PSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern2

;	/* Move pattern1 in the PSP */
	msr		PSP, r0

;	/* Copy PSP to r1, since PSP can't be directly cmp'ed */
	mrs 	r1, PSP

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_PSP_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern4

;	/* Move pattern1 in the PSP */
	msr		PSP, r0

;	/* Copy PSP to r1, since PSP can't be directly cmp'ed */
	mrs 	r1, PSP

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_PSP_test_fail_res

_cpu_PSP_test_pass
;	/* Restore the PSP and CONTROL */
	mov 	r13, r4
	msr     PSP, r4
	msr		CONTROL, r3

;   /* Indicate in the CPUregTestPOST_struct
;    * that PSP tests passed.
;    */
    movw    r0, #0x4000
    ldr     r1, =CPUregTestPOST_struct+testState
    ldr     r2, [r1]
    orr     r0, r0, r2
    str     r0, [r1]
	b		_cpu_LR_test

_cpu_PSP_test_fail_res
;	/* Restore the PSP and CONTROL */
	mov 	r13, r4
	msr		CONTROL, r6
_cpu_PSP_test_fail
	b 		_cpu_LR_test_fail

;/* LR register test:
; * The link register will be written with pattern1,
; * then compared for a pass/fail. Secondly the inverse
; * of the pattern will be tested.
; */
_cpu_LR_test
;	/* Store the current link register */
	mov		r3, r14
;	/* Load pattern1 in r0 */
	ldr		r0,=pattern1

;	/* Move pattern1 in the MSP */
	mov		r14, r0

;	/* Compare the pattern with the input pattern */
	cmp		r14, r0
	bne		_cpu_LR_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern3

;	/* Move pattern1 in the MSP */
	mov		r14, r0

;	/* Compare the pattern with the input pattern */
	cmp		r14, r0
	bne		_cpu_LR_test_fail_res

_cpu_LR_test_pass
;	/* Restore the LR */
	mov 	r14, r3
;   /* Indicate in the CPUregTestPOST_struct
;    * that LR tests passed.
;    */
    movw    r0, #0x8000
    ldr     r1, =CPUregTestPOST_struct+testState
    ldr     r2, [r1]
    orr     r0, r0, r2
    str     r0, [r1]
	b		_cpu_APSR_test

_cpu_LR_test_fail_res
;	/* Restore the LR */
	mov 	r14, r3
_cpu_LR_test_fail
	b 		_cpu_APSR_test_fail

;/* APSR register tests:
; * Only the APSR register can be test, the other PSR registers,
; * IPSR and EPSR are read-only.
; * The APSR register will be written with pattern5, because only APSR[31:27]
; * are readable and writeable,
; * then compared for a pass/fail. Secondly the inverse
; * of the pattern will be tested.
; */
_cpu_APSR_test
;	/* Store the current link register */
	mrs		r3, APSR

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern5

;	/* Move pattern1 in the APSR */
	msr		APSR, r0

;	/* Read pattern1 from the APSR */
	mrs		r1, APSR

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_APSR_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0,=pattern6

;	/* Move pattern1 in the APSR */
	msr		APSR, r0

;	/* Read pattern1 from the APSR */
	mrs		r1, APSR

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_APSR_test_fail_res

_cpu_APSR_test_pass
;	/* Restore the APSR */
	msr 	APSR, r3

;   /* Indicate in the CPUregTestPOST_struct
;    * that APSR tests passed.
;    */
    movw    r0, #0x0000
    movt    r0, #0x0001
    ldr     r1, =CPUregTestPOST_struct+testState
    ldr     r2, [r1]
    orr     r0, r0, r2
    str     r0, [r1]
	b		_cpu_PRIMASK_test

_cpu_APSR_test_fail_res
;	/* Restore the APSR */
	msr 	APSR, r3
_cpu_APSR_test_fail
	b 		_cpu_PRIMASK_test_fail


;/* PRIMASK register tests:
; * The PRIMASK bit will be tested to 0 and 1.
; */
_cpu_PRIMASK_test
;	/* Store the current link register */
	mrs		r3, PRIMASK

;	/* Load pattern1 in r0 */
	mov		r0, #0

;	/* Move pattern1 in the PRIMASK */
	msr		PRIMASK, r0

;	/* Read pattern1 from the PRIMASK */
	mrs		r1, PRIMASK

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_PRIMASK_test_fail_res

;	/* Load pattern1 in r0 */
	mov		r0, #1

;	/* Move pattern1 in the PRIMASK */
	msr		PRIMASK, r0

;	/* Read pattern1 from the PRIMASK */
	mrs		r1, PRIMASK

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_PRIMASK_test_fail_res

_cpu_PRIMASK_test_pass
;	/* Restore the PRIMASK */
	msr 	PRIMASK, r3
;   /* Indicate in the CPUregTestPOST_struct
;    * that PRIMASK tests passed.
;    */
    movw    r0, #0x0000
    movt    r0, #0x0002
    ldr     r1, =CPUregTestPOST_struct+testState
    ldr     r2, [r1]
    orr     r0, r0, r2
    str     r0, [r1]
	b		_cpu_FAULTMASK_test

_cpu_PRIMASK_test_fail_res
;	/* Restore the PRIMASK */
	msr 	PRIMASK, r3
_cpu_PRIMASK_test_fail
	b 		_cpu_FAULTMASK_test_fail

;/* FAULTMASK register tests:
; * The FAULTMASK bit will be tested to 0 and 1.
; */
_cpu_FAULTMASK_test
;	/* Store the current link register */
	mrs		r3, FAULTMASK

;	/* Load pattern in r0 */
	mov		r0, #0

;	/* Move pattern in the FAULTMASK */
	msr		FAULTMASK, r0

;	/* Read pattern from the FAULTMASK */
	mrs		r1, FAULTMASK

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_FAULTMASK_test_fail_res

;	/* Load pattern in r0 */
	mov		r0, #1

;	/* Move pattern in the FAULTMASK */
	msr		FAULTMASK, r0

;	/* Read pattern from the FAULTMASK */
	mrs		r1, FAULTMASK

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_FAULTMASK_test_fail_res

_cpu_FAULTMASK_test_pass
;	/* Restore the FAULTMASK */
	msr 	FAULTMASK, r3

;   /* Indicate in the CPUregTestPOST_struct
;    * that FAULTMASK tests passed.
;    */
    movw    r0, #0x0000
    movt    r0, #0x0004
    ldr     r1, =CPUregTestPOST_struct+testState
    ldr     r2, [r1]
    orr     r0, r0, r2
    str     r0, [r1]
	b		_cpu_BASEPRI_test

_cpu_FAULTMASK_test_fail_res
;	/* Restore the FAULTMASK */
	msr 	FAULTMASK, r3
_cpu_FAULTMASK_test_fail
	b 		_cpu_BASEPRI_test_fail

;/* BASEPRI register tests:
; * The BASEPRI bit will be tested to 0 and 1.
; */
_cpu_BASEPRI_test
;	/* Store the current link register */
	mrs		r3, BASEPRI

;	/* Load pattern1 in r0 */
	ldr		r0, =pattern7

;	/* Move pattern1 in the BASEPRI */
	msr		BASEPRI, r0

;	/* Read pattern1 from the BASEPRI */
	mrs		r1, BASEPRI

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_BASEPRI_test_fail_res

;	/* Load pattern1 in r0 */
	ldr		r0, =pattern8

;	/* Move pattern1 in the BASEPRI */
	msr		BASEPRI, r0

;	/* Read pattern1 from the BASEPRI */
	mrs		r1, BASEPRI

;	/* Compare the pattern with the input pattern */
	cmp		r1, r0
	bne		_cpu_BASEPRI_test_fail_res

_cpu_BASEPRI_test_pass
;	/* Restore the BASEPRI */
	msr 	BASEPRI, r3

;   /* Indicate in the CPUregTestPOST_struct
;    * that FAULTMASK tests passed.
;    */
    movw    r0, #0x0000
    movt    r0, #0x0008
    ldr     r1, =CPUregTestPOST_struct+testState
    ldr     r2, [r1]
    orr     r0, r0, r2
    str     r0, [r1]
	b		cpu_test_pass

_cpu_BASEPRI_test_fail_res
;	/* Restore the BASEPRI */
	msr 	BASEPRI, r3
_cpu_BASEPRI_test_fail
	b 		cpu_test_fail

cpu_test_pass
	mov		r0, #0x01
	ldr		r1, =CPUregTestPOST_struct+testPassed
	str		r0, [r1]
	b		exit

cpu_test_fail
;	/* Clear r0 as CPU reg test fail */
	movs	r0, #0x00
	ldr     r1, =CPUregTestPOST_struct+testPassed
    str     r0, [r1]

exit
	bx 		lr

	END