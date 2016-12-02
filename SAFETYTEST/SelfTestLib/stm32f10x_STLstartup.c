/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLstartup.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/01/2007
* Description        : This file contains all calls to the routines to be
*                      executed at start.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/*Reviewed for IEC 60335-1 Compliance
* This module can be used for self tests according to Table H.11.12.7 of appendix R of the standard ;EN/IEC 60335-1. The implementation of the module has to be tested in each application.
* Reviewed by V.D.E. Testing and Certification Institute ; Merianstrasse 28 ; D-63069 Offenbach
* VDE-Ref:    5008057-9999-0001/91436 FG23/Scb
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_STLlib.h"
#include "ewdt.h"

/*#define ALLOC_GLOBALS*/
#include "stm32f10x_STLclassBvar.h"

#ifdef STL_VERBOSE_POR
  #include <stdio.h>
#endif  /* STL_VERBOSE_POR */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 g_u32InitTestTemp = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#ifdef STL_VERBOSE_POR
  static void STL_VerbosePORInit(void);
#endif  /* STL_VERBOSE_POR */

static ErrorStatus RCC_Config64MHzOnHSI(void);
static ErrorStatus RCC_SwitchOffPLL(void);
static void ExtWdtCheck(void);
static void GeneralRegister_StartupCheck(void);
static void IWDTCheck(void);
static void DataIntegrityInFlash_StartupCheck(void);
static void ClockFrequency_StartupCheck(void);
static void Stack_StartupCheck(void);

#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */

  /* Temporary RAM buffer used during transparent run-time tests */
  /* WARNING: Real reserved RAM location from 0x20000000 to 0x20000024*/
    #pragma location = "RUN_TIME_RAM_BUF"
    u32 RunTimeRamBuf[RT_RAM_BLOCKSIZE] ;

  /* RAM pointer for run-time tests */
    #pragma location = "RUN_TIME_RAM_PNT"
    u32 *p_RunTimeRamChk  ;
    #pragma location = "RUN_TIME_RAM_PNT"
    u32 *p_RunTimeRamChkInv ;

  /* Counter for verifying correct program execution at start */
    #pragma location = "CLASS_B_RAM"
    u32 CtrlFlowCnt;
    #pragma location = "CLASS_B_RAM_REV"
    u32 CtrlFlowCntInv;

  /* Counter for verifying correct program execution in interrupt */
    #pragma location = "CLASS_B_RAM"
    u32 ISRCtrlFlowCnt ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 ISRCtrlFlowCntInv ;

  /* First measure stored as reference for run-time checks */
    #pragma location = "CLASS_B_RAM"
    u32 StartUpClockFreq ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 StartUpClockFreqInv ;

  /* First period measure stored as reference for run-time checks */
    #pragma location = "CLASS_B_RAM"
    u32 RefHSEPeriod ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 RefHSEPeriodInv ;

  /* Last period measure stored as reference for run-time checks */
    #pragma location = "CLASS_B_RAM"
    u32 CurrentHSEPeriod ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 CurrentHSEPeriodInv;

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
    #pragma location = "CLASS_B_RAM"
    u32 TickCounter ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 TickCounterInv ;

  /* Indicates to the main routine a 100ms tick */
    #pragma location = "CLASS_B_RAM"
    vu32 TimeBaseFlag ;
    #pragma location = "CLASS_B_RAM_REV"
    vu32 TimeBaseFlagInv  ;

  /* Stores the Control flow counter from one main loop to the other */
    #pragma location = "CLASS_B_RAM"
    u32 LastCtrlFlowCnt ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 LastCtrlFlowCntInv ;

  /* Pointer to FLASH for crc16 run-time tests */
    #pragma location = "CLASS_B_RAM"
    u8 *p_RunCrc16Chk ;
    #pragma location = "CLASS_B_RAM_REV"
    u8 *p_RunCrc16ChkInv ;

  /* Pointer to FLASH for crc32 run-time tests */
    #pragma location = "CLASS_B_RAM"
    u32 *p_RunCrc32Chk ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 *p_RunCrc32ChkInv  ;

  /* Reference 32-bit CRC for run-time tests */
    #pragma location = "CLASS_B_RAM"
    u32 RefCrc32  ;
    #pragma location = "CLASS_B_RAM_REV"
    u32 RefCrc32Inv;

  /* Current FLASH 16-bit Crc */
    #pragma location = "CLASS_B_RAM"
    u16 CurrentCrc16 ;
    #pragma location = "CLASS_B_RAM_REV"
    u16 CurrentCrc16Inv ;

  /* Magic pattern for Stack overflow in this array */
    #pragma location = "STACK_BOTTOM"
    vu32 StackOverFlowPtrn[4];

#endif  /* __IAR_SYSTEMS_ICC__ */


/*******************************************************************************
* Function Name  : FailSafePOR
* Description    : Contains the Fail Safe routine executed in case of
*                  failure detected during one of the POR self-test routines
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FailSafePOR(void)
{
/*  SysTick_ITConfig(DISABLE);*/
  #ifdef STL_VERBOSE_POR
  printf(" >>>>>>>>>> POR FailSafe Mode <<<<<<<<<<\n\r");
  #endif  /* STL_VERBOSE_POR */
/*  
  while(1)
  {
    NVIC_SystemReset();
  }
*/
      /* Reocrd the init test error */
      /* Set 2 flag, because the test has bkp reset and ram reset in startup */
#ifdef GEC_SF_S_NEW
      write_bkp(RTC_BKP_DR3, 1u);  
#else
      write_bkp(BKP_DR3, 1u);
#endif  
      g_u32InitTestTemp = 1u;      
}

/*******************************************************************************
* Function Name  : ExtWdtCheck
* Description    : Check the external watchdong.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ExtWdtCheck(void)
{

    EWDT_Drv_pin_config();
    ExternalWatchdogCheck(); 

}

/*******************************************************************************
* Function Name  : GeneralRegister_StartupCheck
* Description    : Check the general register after startup.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GeneralRegister_StartupCheck(void)
{
    /* Initializes counter for control flow monitoring */
    CtrlFlowCnt = CPU_TEST_CALLER;
    CtrlFlowCntInv = 0xFFFFFFFFuL;
    
    if (STL_StartUpCPUTest() != CPUTEST_SUCCESS)
    {
#ifdef STL_VERBOSE_POR
        printf("Start-up CPU Test Failure\n\r");
#endif /* STL_VERBOSE_POR */
        FailSafePOR();
    }
    else  /* Test OK */
    {
        /* If else statement is not executed, it will be detected by control flow monitoring */
        CtrlFlowCntInv -= CPU_TEST_CALLER;
#ifdef STL_VERBOSE_POR
        printf("Start-up CPU Test OK\n\r");
#endif /* STL_VERBOSE_POR */
    }

}

/*******************************************************************************
* Function Name  : IWDTCheck
* Description    : Check the internal watchdong.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void IWDTCheck(void)
{
    CtrlFlowCnt += IWDG_TEST_CALLER;
    STL_WDGSelfTest();
    EWDT_TOOGLE();
    CtrlFlowCntInv -= IWDG_TEST_CALLER;

}

/*******************************************************************************
* Function Name  : DataIntegrityInFlashCheck
* Description    : Check the clock frequency after startup.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void DataIntegrityInFlash_StartupCheck(void)
{
    CtrlFlowCnt += CRC32_TEST_CALLER;
    {
        u32 TmpCRC;
        /* Initialize the internal 32-bit CRC generator */
        CRC_Init();
        /* Compute the 32-bit crc of the whole Flash */
        TmpCRC = CRC_CalcBlockCrc((uc32 *)ROM_START, (u32)ROM_SIZEinWORDS);
        /* Store the inverted least significant byte of the CRC in the peripheral */
        SetIDRegister(~((u8)TmpCRC));
        CtrlFlowCntInv -= CRC32_TEST_CALLER;
    }
    
    /* Reload IWDG / EWDT counter */
    IWDG_ReloadCounter();
    EWDT_TOOGLE();
    
    /* Regular 16-bit crc computation */
    CtrlFlowCnt += CRC16_TEST_CALLER;
    if(STL_crc16((u16)CRC_INIT,(u8 *)ROM_START, ROM_SIZE) != REF_CRC16)
    {
#ifdef STL_VERBOSE_POR
        printf(" FLASH 16-bit CRC Error at Start-up\n\r");
#endif  /* STL_VERBOSE_POR */
        FailSafePOR();
    }
    else  /* Test OK */
    {
        /* If else statement is not executed, it will be detected by control flow monitoring */
        CtrlFlowCntInv -= CRC16_TEST_CALLER;
#ifdef STL_VERBOSE_POR
        printf(" Start-up FLASH 16-bit CRC OK\n\r");
#endif  /* STL_VERBOSE_POR */
    }
    
    /* Reload IWDG / EWDT counter */
    IWDG_ReloadCounter();
    EWDT_TOOGLE();

}

/*******************************************************************************
* Function Name  : ClockFrequency_StartupCheck
* Description    : Check the data integrity in flash after startup.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ClockFrequency_StartupCheck(void)
{
    CtrlFlowCnt += CLOCK_TEST_CALLER;
    switch ( STL_ClockStartUpTest() )
    {
       case FREQ_OK:
#ifdef STL_VERBOSE_POR
        /* Switch back to HSI to be sure to have a valid display */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
        printf("Clock frequency OK \n\r");
#endif  /* STL_VERBOSE_POR */
        break;
        
       case LSI_START_FAIL:
#ifdef STL_VERBOSE_POR
        printf("LSI start-up failure \n\r");
#endif  /* STL_VERBOSE_POR */
        FailSafePOR();
        break;
        
       case HSE_START_FAIL:
#ifdef STL_VERBOSE_POR
        printf("HSE start-up failure \n\r");
#endif  /* STL_VERBOSE_POR */
        FailSafePOR();
        break;
        
       case HSI_HSE_SWITCH_FAIL:
#ifdef STL_VERBOSE_POR
        printf("Clock switch failure \n\r");
#endif  /* STL_VERBOSE_POR */
        FailSafePOR();
        break;
        
       case EXT_SOURCE_FAIL:
#ifdef STL_VERBOSE_POR
        printf("Clock Source failure \n\r");
#endif  /* STL_VERBOSE_POR */
        FailSafePOR();
        break;
        
       case TEST_ONGOING:
       default:
#ifdef STL_VERBOSE_POR
        printf("Abnormal Clock Test routine termination \n\r");
#endif  /* STL_VERBOSE_POR */
        FailSafePOR();
        break;
    }
    /* Either switch back to HSI or start PLL on HSE asap */
    CtrlFlowCntInv -= CLOCK_TEST_CALLER;
    
    /* Reload IWDG / EWDT counter */
    IWDG_ReloadCounter();
    EWDT_TOOGLE();

}

/*******************************************************************************
* Function Name  : Stack_StartupCheck
* Description    : Verify Control flow before Starting main program execution.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Stack_StartupCheck(void)
{
    CtrlFlowCnt += STACK_OVERFLOW_TEST;
    StackOverFlowPtrn[0] = 0xAAAAAAAAuL;
    StackOverFlowPtrn[1] = 0xBBBBBBBBuL;
    StackOverFlowPtrn[2] = 0xCCCCCCCCuL;
    StackOverFlowPtrn[3] = 0xDDDDDDDDuL;
    CtrlFlowCntInv -= STACK_OVERFLOW_TEST;
}


/*******************************************************************************
* Function Name  : Safety_StartupCheck1
* Description    : Contains the very first test routines executed right after
*                  the reset
* Input          : None
* Output         : Zero initialized RAM, CPU clock provided by the HSE
*                  Flash interface initialized, Systick timer ON (2ms timebase)
* Return         : None
*******************************************************************************/
void Safety_StartupCheck1(void)
{
  #ifdef STL_VERBOSE_POR
    /* NOTE: Depending on compiler optimization level, printf may not operate
    correctly. In this case, STL_StartUpCPUTest and STL_FullRamMarchC must be
    commented (this functions are destroying R4 and stack content) */
    STL_VerbosePORInit();
    printf("\n\n\r *******  Self Test Library Init  *******\n\r");
  #endif
   
   /* Clear the init test error flag */ 
   g_u32InitTestTemp = 0u;
   
  /*--------------------------------------------------------------------------*/ 
  /*--------------------------- EWDT check -----------------------------------*/  
  /*--------------------------------------------------------------------------*/ 
  ExtWdtCheck();
  
  /*--------------------------------------------------------------------------*/
  /*------------------- CPU registers and Flags Self Test --------------------*/
  /*--------------------------------------------------------------------------*/
  /* WARNING: all registers destroyed when exiting this function (including
  preserved registers R4 to R11) and excluding stack point R13) */

  GeneralRegister_StartupCheck();

  /*--------------------------------------------------------------------------*/ 
  /*--------------------------- IWDT check -----------------------------------*/  
  /*--------------------------------------------------------------------------*/  
  IWDTCheck();

  /*--------------------------------------------------------------------------*/
  /*-------------------- Switch ON PLL for 64MHz operation--------------------*/
  /*--------------------------------------------------------------------------*/
  /* No Control flow check here (not safety critical) */
  /* Switch on the PLL to speed-up Flash and RAM tests */
  if (RCC_Config64MHzOnHSI() != SUCCESS)
  {
  #ifdef STL_VERBOSE_POR
    printf("PLL Switch on failure\n\r");
    FailSafePOR();
  #endif  /* STL_VERBOSE_POR */
  }

  #ifdef STL_VERBOSE_POR
    USART_Configuration();  // Re-init USART with modified clock setting
  #endif  /* STL_VERBOSE_POR */


  /*--------------------------------------------------------------------------*/
  /*--------------------- Invariable memory CRC check ------------------------*/
  /*--------------------------------------------------------------------------*/
  DataIntegrityInFlash_StartupCheck();

  
  /*--------------------------------------------------------------------------*/
  /*   Verify Control flow before RAM init (which clears Ctrl flow counters)  */
  /*--------------------------------------------------------------------------*/
  if (((CtrlFlowCnt ^ CtrlFlowCntInv) != 0xFFFFFFFFuL)
      ||(CtrlFlowCnt != CHECKPOINT1 ))
  {
     #ifdef STL_VERBOSE_POR
       printf("Control Flow Error Checkpoint 1\n\r");
     #endif  /* STL_VERBOSE_POR */
     FailSafePOR();
  }
  else
  {
   #ifdef STL_VERBOSE_POR
    printf("Control Flow Checkpoint 1 OK \n\r");
   #endif  /* STL_VERBOSE_POR */
  }

  /*--------------------------------------------------------------------------*/
  /* --------------------- Variable memory functional test -------------------*/
  /* --------------------------  Address Computation  ------------------------*/
  /*--------------------------------------------------------------------------*/
  /* WARNING: Stack is zero-initialized when exiting from this routine */

  if (RAM_StarupCheck() != SUCCESS)
  {
#ifdef STL_VERBOSE_POR
      printf("RAM Test Failure\n\r");
#endif  /* STL_VERBOSE_POR */
      FailSafePOR();
  }
#ifdef STL_VERBOSE_POR
  printf(" Full RAM Test OK\n\r");
#endif /* STL_VERBOSE_POR */
  
  /* Reload IWDG / EWDT counter */
  IWDG_ReloadCounter();
  EWDT_TOOGLE();

  /* Both CtrlFlowCnt and CtrlFlowCntInv are zeroed then re-initialized inside
  the test routine to have inverse values */

  /*--------------------------------------------------------------------------*/
  /*-- Store reference 32-bit CRC in RAM after RAM test (if not corrupted) ---*/
  /*--------------------------------------------------------------------------*/
  if ((GetIDRegister() ^ ((u8)GetCurrentCRC())) == 0xFF)
  {
    RefCrc32 = GetCurrentCRC();
    RefCrc32Inv = ~RefCrc32;
    #ifdef STL_VERBOSE_POR
      printf("Ref 32-bit CRC OK\n\r");
    #endif  /* STL_VERBOSE_POR */

  }
  else
  {
    #ifdef STL_VERBOSE_POR
      printf("Ref 32-bit CRC corrupted\n\r");
    #endif  /* STL_VERBOSE_POR */
    FailSafePOR();

  }
  
  /* Reload IWDG / EWDT counter */
  IWDG_ReloadCounter();
  EWDT_TOOGLE();

  /*--------------------------------------------------------------------------*/
  /*--------------- Switch Off PLL to test external clock source--------------*/
  /*--------------------------------------------------------------------------*/
  /* No Control flow check here (not safety critical) */
  if (RCC_SwitchOffPLL() != SUCCESS)
  {
  #ifdef STL_VERBOSE_POR
    printf("Switch back to HSI failed\n\r");
    FailSafePOR();
  #endif  /* STL_VERBOSE_POR */
  }

  #ifdef STL_VERBOSE_POR
    USART_Configuration();  // Re-init USART with modified clock setting
  #endif  /* STL_VERBOSE_POR */

  /* Reload IWDG / EWDT counter */
  IWDG_ReloadCounter();
  EWDT_TOOGLE();
  
  /*--------------------------------------------------------------------------*/
  /*----------------------- Clock Frequency Self Test ------------------------*/
  /*--------------------------------------------------------------------------*/

#ifdef GEC_SF_S_NEW
      g_u32InitTestTemp = RTC_ReadBackupRegister(RTC_BKP_DR3);
#else
      g_u32InitTestTemp = BKP_ReadBackupRegister(BKP_DR3);
#endif
  ClockFrequency_StartupCheck();
  
  /*--------------------------------------------------------------------------*/
  /*--------------------------- Stack overflow -------------------------------*/
  /*--------------------------------------------------------------------------*/

  Stack_StartupCheck();

  /*--------------------------------------------------------------------------*/
  /* -----  Verify Control flow before Starting main program execution ------ */
  /*--------------------------------------------------------------------------*/
   if (((CtrlFlowCnt ^ CtrlFlowCntInv) != 0xFFFFFFFFuL)
      ||(CtrlFlowCnt != CHECKPOINT2 ))
   {
     #ifdef STL_VERBOSE_POR
       printf("Control Flow Error Checkpoint 2\n\r");
     #endif  /* STL_VERBOSE_POR */
     FailSafePOR();
   }
   #ifdef STL_VERBOSE_POR
    printf("Control Flow Checkpoint 2 OK \n\r");
   #endif  /* STL_VERBOSE_POR */
   
   if( g_u32InitTestTemp )
   {
      FailSafePOR();
   }
   
  /* Reload IWDG / EWDT counter */
  IWDG_ReloadCounter();
  EWDT_TOOGLE();
  
   GotoCompilerStartUp()
      ;

}


#ifdef STL_VERBOSE_POR
/*******************************************************************************
* Function Name  : STL_VerbosePORInit
* Description    : Initializes the USART1 and few I/Os for test purposes
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STL_VerbosePORInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_AFIO, ENABLE);

  /* Configure PC.06 and PC.07 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_Configuration();

}


/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_DeInit(USART1);

/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate = 115200uL;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Clock = USART_Clock_Disable;
  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

  USART_Init(USART1, &USART_InitStructure);

  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
}

#endif  /* STL_VERBOSE_POR */

/*******************************************************************************
* Function Name  : RCC_Config64MHzOnHSI
* Description    : Switch on the PLL to have the maximum avialable speed on
*                  internal high speed oscillator
* Input          : None
* Output         : None
* Return         : ErrorStatus = {ERROR; SUCCESS}
*******************************************************************************/
static ErrorStatus RCC_Config64MHzOnHSI(void)
{
    ErrorStatus Result = ERROR;
    u32 TimeOut = PLL_LOCK_TIMEOUT;

  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);
  /* PCLK1 = HCLK/2 */
  RCC_PCLK1Config(RCC_HCLK_Div2);

  /* Flash 2 wait state */
  FLASH_SetLatency(FLASH_Latency_2);
  /* Enable Prefetch Buffer */
#ifdef GEC_SF_S_NEW
    FLASH_PrefetchBufferCmd(ENABLE);
#else
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
#endif

  /* PLLCLK = 4MHz * 16 = 64 MHz */
/*  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);*/
  /* Enable PLL */
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL is ready */
  do
  {
    TimeOut--;
  }
  while((RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) && (TimeOut != 0u));

  if (TimeOut == 0u)
  {
    Result = ERROR;     /* Clock switch failure */
  }
  else
  {
    /* Set timeout for clock switch */
    TimeOut = CLOCK_SWITCH_TIMEOUT;

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    do
    {
      TimeOut--;
    }
    while((RCC_GetSYSCLKSource() != 0x08u) && (TimeOut != 0u));

    if (TimeOut == 0u)
    {
      Result = ERROR;     /* Clock switch failure */
    }
    else
    {
      Result = SUCCESS;
    }
  }

  return (Result);

}

/*******************************************************************************
* Function Name  : RCC_SwitchOffPLL
* Description    : Switch off the PLL to be able to measure external clock
*                  source frequency
* Input          : None
* Output         : None
* Return         : ErrorStatus = {ERROR; SUCCESS}
*******************************************************************************/
static ErrorStatus RCC_SwitchOffPLL(void)
{
    ErrorStatus Result = ERROR;
    u32 TimeOut = CLOCK_SWITCH_TIMEOUT;
    /* Even if product runs at 64MHz, timeout is still much higher than the
    few clock cycles required for clock switch */

  /* Select HSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  /* Wait till HSI is used again as a system clock source */
  do
  {
    TimeOut--;
  }
  while ((RCC_GetSYSCLKSource() != 0x00u) && (TimeOut != 0u));

  if (TimeOut == 0u)
  {
    Result = ERROR;     /* Clock switch failure */
  }
  else
  {
    /* We can safely switch off the PLL */
    RCC_PLLCmd(DISABLE);
    Result = SUCCESS;
  }

  return (Result);


}


/*******************************************************************************
* Function Name  : STL_IWDGSelfTest
* Description    : Verifies the watchdog by forcing watchdog resets
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void STL_WDGSelfTest(void)
{
    u8 GetResetFlag = 0u;
    
  #ifdef STL_VERBOSE_POR
    if (RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET) printf("Pin reset \r\n");
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET) printf("POR reset \r\n");
    if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET) printf("SW reset \r\n");
    if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET) printf("LP reset \r\n");
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) printf("IWDG reset \r\n");
    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET) printf("WWDG reset \r\n");
  #endif /* STL_VERBOSE_POR */

  /* In one of the 4 conditions below, start watchdogs test */
  if (RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET)   /* Power on reset */
  {
      GetResetFlag = 1u;
  }
  else if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET)   /* or Software reset */
  {
      GetResetFlag = 1u;
  }
  else if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET)  /* or Low Power reset */
  {
      GetResetFlag = 1u;
  }
  else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET)  /* or triggered by */
  {     
      GetResetFlag = 1u;
  }     
  else
  {
      GetResetFlag = 0u;
  }

  if( GetResetFlag == 1u )
  {
      if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == RESET)  /* external pin solely */
      {
#ifdef STL_VERBOSE_POR
          printf("... Power-on or software reset, testing IWDG ... \r\n");
#endif  /* STL_VERBOSE_POR */
          
          /* Enable write access to IWDG_PR and IWDG_RLR registers */
          IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
          /* IWDG clock: 40KHz(LSI) / 4 = 10KHz  */
          IWDG_SetPrescaler(IWDG_Prescaler_4);
          /* Set counter reload value to 1 (125¦Ìs */
          IWDG_SetReload(1u);
          /* Reload IWDG counter */
          IWDG_ReloadCounter();
          /* Enable IWDG (LSI automatically enabled by HW) */
          IWDG_Enable();
          
          RCC_ClearFlag();        /* Clear all flags before reuming test */
          /* Wait for an independant watchdog reset */
          /* set the flag,don't check the ewdt */
#ifdef GEC_SF_S_NEW
          write_bkp(RTC_BKP_DR2, 1u);  
#else
          write_bkp(BKP_DR2, 1u);
#endif 
          while(1)
          {}
      }
  }
  else  /* Watchdog test or software reset triggered by application failure */
  {
    /* If WWDG only was set, re-start the complete test (indicates a reset triggered by safety routines */
      GetResetFlag = 0u;
      if(RCC_GetFlagStatus(RCC_FLAG_PINRST)  == SET)
      {
          GetResetFlag++;
      }
      if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == RESET)
      {
          GetResetFlag++;
      }
      
    if( GetResetFlag == 2u )
    {
      RCC_ClearFlag();
      #ifdef STL_VERBOSE_POR
        printf("... WWDG reset, re-start WDG test ... \r\n");
      #endif  /* STL_VERBOSE_POR */
      NVIC_SystemReset();
    }
    else  /* If IWDG only was set, continue the test with WWDG test*/
    {
        
      GetResetFlag = 0u;
      if(RCC_GetFlagStatus(RCC_FLAG_PINRST)  == SET)
      {
          GetResetFlag++;
      }
      if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
      {
          GetResetFlag++;
      }
      
      if( GetResetFlag == 2u )          
      { 
        
          /* Enable write access to IWDG_PR and IWDG_RLR registers */
          IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
          /* IWDG clock: 40KHz(LSI)  max:0xFFF / 4095    Tout = ((4*2^prer)*count) / 40*/
          IWDG_SetPrescaler(IWDG_Prescaler_64);
          /* Set counter reload value to 1250 ,  64 * 1250 / 40 = 2s */
          IWDG_SetReload(1250u);
          /* Reload IWDG counter */
          IWDG_ReloadCounter();
          /* Enable IWDG (LSI automatically enabled by HW) */
          IWDG_Enable();        
        
        
/*          RCC_ClearFlag();*/
#ifdef GEC_SF_S_NEW
          write_bkp(RTC_BKP_DR2, 0u);  
#else
          write_bkp(BKP_DR2, 0u);
#endif 
          #ifdef STL_VERBOSE_POR
            printf("... WWDG reset, WDG test completed ... \r\n");
          #endif  /* STL_VERBOSE_POR */

      }
      else  /* If both flags are set, means that watchdog test is completed */
      {

          RCC_ClearFlag();
          #ifdef STL_VERBOSE_POR
            printf("...Unexpected Flag configuration, re-start WDG test... \r\n");
          #endif  /* STL_VERBOSE_POR */
          /*NVIC_SystemReset();*/
          FailSafePOR();
      } /* End of Unexpected Flag configuration */

    } /* End of partial WDG test (IWDG test done) */
  } /* End of part where 1 or 2 Watchdog flags are set */
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/