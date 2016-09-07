/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLmain.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : Contains the Self-test functions executed during main
*                      program execution.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

#ifdef STL_VERBOSE
  #include <stdio.h>
#endif  /* STL_VERBOSE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u8 g_u8RomCheckOkFlag = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static ClockStatus STL_MainClockTest(void);
static ErrorStatus STL_CheckStack(void);
static void GeneralRegister_RunCheck(void);
static void Stack_RunCheck(void);
static void ClockFrequency_RunCheck(void);
static void DataIntegrityInFlash_RunCheck(u32* RomTest);

/*******************************************************************************
* Function Name  : Safety_InitRunTimeChecks
* Description    : Initializes the Class B variables and their inverted
*                  redundant counterparts. Init also the Timer and RTC timer
*                  for clock frequency monitoring.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Safety_InitRunTimeChecks(void)
{
  /* Init Class B variables required in main routine and Timer interrupt
  service routine for timing purposes */
  TickCounter = 0u;
  TickCounterInv = 0xFFFFFFFFuL;

  TimeBaseFlag = 0u;
  TimeBaseFlagInv = 0xFFFFFFFFuL;

  LastCtrlFlowCnt = 0u;
  LastCtrlFlowCntInv = 0xFFFFFFFFuL;

  /* Initialize variables for Timer interrupt routine control flow monitoring */
  ISRCtrlFlowCnt = 0u;
  ISRCtrlFlowCntInv = 0xFFFFFFFFuL;

  /* Initialize variables for invariable memory check */
  STL_TranspMarchCInit();
/*  STL_TranspMarchXInit();*/

  /* Initialize variable for clock memory check */
  CurrentHSEPeriod = 0u;
  CurrentHSEPeriodInv = 0xFFFFFFFFuL;

  /* Initialize Timer for clock frequency measurement and main time base */
  /* The RTC is also reset and synchronized to do measures in Timer ISR */
  STL_SysTickRTCSync();

  /* Initialize variables for invariable memory check */
  STL_FlashCrc16Init();
  STL_FlashCrc32Init();

  /* Initialize variables for main routine control flow monitoring */
  CtrlFlowCnt = 0u;
  CtrlFlowCntInv = 0xFFFFFFFFuL;

}

/*******************************************************************************
* Function Name  : GeneralRegister_RunCheck
* Description    : Check the general register during running.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GeneralRegister_RunCheck(void)
{
    CtrlFlowCnt += CPU_TEST_CALLER;
    if (STL_RunTimeCPUTest() != CPUTEST_SUCCESS)
    {
#ifdef STL_VERBOSE
        printf("Start-up CPU Test Failure\n\r");
#endif /* STL_VERBOSE */
        FailSafePOR();
    }
    else
    {
        CtrlFlowCntInv -= CPU_TEST_CALLER;
    }

}

/*******************************************************************************
* Function Name  : Stack_RunCheck
* Description    : Check the stack.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Stack_RunCheck(void)
{
    CtrlFlowCnt += STACK_OVERFLOW_TEST;
    if (STL_CheckStack() != SUCCESS)
    {
#ifdef STL_VERBOSE
        printf("Stack overflow\n\r");
#endif /* STL_VERBOSE */
        FailSafePOR();
    }
    else
    {
        CtrlFlowCntInv -= STACK_OVERFLOW_TEST;
    }

}

/*******************************************************************************
* Function Name  : ClockFrequency_RunCheck
* Description    : Check the clock frequency during running.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ClockFrequency_RunCheck(void)
{
    CtrlFlowCnt += CLOCK_TEST_CALLER;
    switch ( STL_MainClockTest() )
    {
       case FREQ_OK:
        CtrlFlowCntInv -= CLOCK_TEST_CALLER;
        break;
        
       case EXT_SOURCE_FAIL:
#ifdef STL_VERBOSE
        /* Loop until the end of current transmission */
        /* Re-config USART baudrate FOR 115200 bds with HSI clock (8MHz) */
        printf("\n\r Clock Source failure (Run-time)\n\r");
#endif /* STL_VERBOSE */
        FailSafePOR();
        break;
        
       case CLASS_B_VAR_FAIL:
#ifdef STL_VERBOSE
        printf("\n\r Class B variable error (clock test)\n\r");
#endif /* STL_VERBOSE */
        FailSafePOR();
        break;
        
       case LSI_START_FAIL:
       case HSE_START_FAIL:
       case HSI_HSE_SWITCH_FAIL:
       case TEST_ONGOING:
       case CTRL_FLOW_ERROR:
       default:
#ifdef STL_VERBOSE
        printf("Abnormal Clock Test routine termination \n\r");
#endif  /* STL_VERBOSE */
        FailSafePOR();
        break;
    }
    
}

/*******************************************************************************
* Function Name  : DataIntegrityInFlash_RunCheck
* Description    : Check the data integrity in flash during running.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void DataIntegrityInFlash_RunCheck(u32* RomTest)
{
    CtrlFlowCnt += FLASH_TEST_CALLER;
    *RomTest = STL_crc16Run();
    switch ( *RomTest )
    {
       case TEST_RUNNING:
        CtrlFlowCntInv -= FLASH_TEST_CALLER;
        break;
        
       case TEST_OK:
#ifdef STL_VERBOSE
        printf("Run-time FLASH CRC OK\n\r");
#endif /* STL_VERBOSE */
        CtrlFlowCntInv -= FLASH_TEST_CALLER;
        break;
        
       case TEST_FAILURE:
       case CLASS_B_DATA_FAIL:
       default:
#ifdef STL_VERBOSE
        printf(" Run-time FLASH CRC Error\n\r");
#endif  /* STL_VERBOSE */
        FailSafePOR();
        break;
    }

}

/*******************************************************************************
* Function Name  : Safety_RunCheck1
* Description    : Provide a short description of the function
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Safety_RunCheck1(void)
{
    static u32 stat_u32CheckTimePeriod = 0u;
    
    stat_u32CheckTimePeriod++;
    if( ( stat_u32CheckTimePeriod * 5u ) >= RUNCHECK_TIME_PERIOD )
    {
        stat_u32CheckTimePeriod = 0u;
        Safety_InitRunTimeChecks();
    }
    
    
  /* Is the time base duration elapsed? */
  if (TimeBaseFlag == 0xAAAAAAAAuL)
  {
        u32 TmpFlag = TimeBaseFlagInv;

    /* Verify its integrity (class B variable) */
    if ((TimeBaseFlag ^ TmpFlag) == 0xFFFFFFFFuL)
    {
        u32 RomTest;
        
        stat_u32CheckTimePeriod = 0u;

      /* Reset Flag (no need to reset the redundant: it is not tested if
      TimeBaseFlag != 0xAAAAAAAA, it means that 100ms elapsed */
      TimeBaseFlag = 0u;


      /*----------------------------------------------------------------------*/
      /*---------------------------- CPU registers ----------------------------*/
      /*----------------------------------------------------------------------*/
      GeneralRegister_RunCheck();

      /*----------------------------------------------------------------------*/
      /*------------------------- Stack overflow -----------------------------*/
      /*----------------------------------------------------------------------*/
      Stack_RunCheck();

      /*----------------------------------------------------------------------*/
      /*------------------------- Clock monitoring ---------------------------*/
      /*----------------------------------------------------------------------*/
      ClockFrequency_RunCheck();


      /*----------------------------------------------------------------------*/
      /*------------------ Invariable memory CRC check -----------------------*/
      /*----------------------------------------------------------------------*/
      DataIntegrityInFlash_RunCheck(&RomTest);

      /*----------------------------------------------------------------------*/
      /*---------------- Check Safety routines Control flow  -----------------*/
      /*------------- Refresh Window and independent watchdogs ---------------*/
      /*----------------------------------------------------------------------*/
      /* Reload IWDG counter */
      IWDG_ReloadCounter();

      if (((CtrlFlowCnt ^ CtrlFlowCntInv) == 0xFFFFFFFFuL)
        &&((LastCtrlFlowCnt ^ LastCtrlFlowCntInv) == 0xFFFFFFFFuL))
      {
        if (RomTest == TEST_OK)
        {
            if(CtrlFlowCnt == FULL_FLASH_CHECKED)
            {
                if((CtrlFlowCnt - LastCtrlFlowCnt) == (LAST_DELTA_MAIN))
                {
                    CtrlFlowCnt = 0u;
                    CtrlFlowCntInv = 0xFFFFFFFFuL;
                    
                    /* Rom test ok */
                    g_u8RomCheckOkFlag = 1u;
                }
                else  /* Return value form crc check was corrupted */
                {
                  #ifdef STL_VERBOSE
                    printf("Control Flow Error (main loop, Flash CRC)\n\r");
                  #endif  /* STL_VERBOSE */
                    FailSafePOR();
                }
            }                
            else  /* Return value form crc check was corrupted */
            {
              #ifdef STL_VERBOSE
                printf("Control Flow Error (main loop, Flash CRC)\n\r");
              #endif  /* STL_VERBOSE */
                FailSafePOR();
            }
        }
        else  /* Flash test not completed yet*/
        {
          if ((CtrlFlowCnt - LastCtrlFlowCnt) != DELTA_MAIN)
          {
            #ifdef STL_VERBOSE
              printf("Control Flow Error (main loop, Flash CRC on-going)\n\r");
            #endif  /* STL_VERBOSE */
            FailSafePOR();
          }
        }

        LastCtrlFlowCnt = CtrlFlowCnt;
        LastCtrlFlowCntInv = CtrlFlowCntInv;
      }
      else
      {
        #ifdef STL_VERBOSE
          printf("Control Flow Error (main loop)\n\r");
        #endif  /* STL_VERBOSE */
        FailSafePOR();
      }
    } /* End of periodic Self-test routine */
    else  /* Class B variable error (can be Systick interrupt lost) */
    {
      #ifdef STL_VERBOSE
        printf("\n\r Class B variable error (clock test)\n\r");
      #endif  /* STL_VERBOSE */
      FailSafePOR();
    }
  } /* End of periodic Self-test routine */

}

/*******************************************************************************
* Function Name  : STL_MainClockTest
* Description    : This function verifies the frequency using the measurement
*                  done in Systick interrupt.
* Input          : None
* Output         : None
* Return         : ClockStatus = {LSI_START_FAIL; HSE_START_FAIL;
*                  HSI_HSE_SWITCH_FAIL; TEST_ONGOING; EXT_SOURCE_FAIL;
*                  CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK}
*******************************************************************************/
static ClockStatus STL_MainClockTest(void)
{
    ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  CtrlFlowCnt += CLOCK_TEST_CALLEE;
  

  if ((CurrentHSEPeriod ^ CurrentHSEPeriodInv) == 0xFFFFFFFFuL)
  {
    if (STL_CheckPeriod(CurrentHSEPeriod/SYSTICK_20ms_TB) == FREQ_OK)
    {
      Result = FREQ_OK;
    }
    else
    {
      Result = EXT_SOURCE_FAIL;
    }
  }
  else  /* Inverse redundant variables failure */
  {
    Result = CLASS_B_VAR_FAIL;
  }

  CtrlFlowCntInv -= CLOCK_TEST_CALLEE;

  return (Result);

}


/*******************************************************************************
* Function Name  : STL_CheckStack
* Description    : This function verifies that Stack didn't overflow
* Input          : None
* Output         : None
* Return         : ErrorStatus = {ERROR; SUCCESS}
*******************************************************************************/
static ErrorStatus STL_CheckStack(void)
{
    ErrorStatus Result = ERROR;

  CtrlFlowCnt += STACK_OVERFLOW_CALLEE;

  if (StackOverFlowPtrn[0] != 0xAAAAAAAAuL)
  {
    Result = ERROR;
  }
  else /* StackOverFlowPtrn[0] == 0xAAAAAAAA */
  {
    if (StackOverFlowPtrn[1] != 0xBBBBBBBBuL)
    {
      Result = ERROR;
    }
    else /* StackOverFlowPtrn[1] == 0xBBBBBBBB */
    {
      if (StackOverFlowPtrn[2] != 0xCCCCCCCCuL)
      {
        Result = ERROR;
      }
      else /* StackOverFlowPtrn[2] == 0xCCCCCCCC */
      {
        if (StackOverFlowPtrn[3] != 0xDDDDDDDDuL)
        {
          Result = ERROR;
        }
        else
        {
          Result = SUCCESS;
        }
      }
    }
  }

  CtrlFlowCntInv -= STACK_OVERFLOW_CALLEE;

  return (Result);

}

/*******************************************************************************
* Function Name  : Safety_TimingCheck
* Description    : This function for safety running check.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Safety_TimingCheck(void)
{
        
    /* Verify TickCounter integrity */
    if ((TickCounter ^ TickCounterInv) == 0xFFFFFFFFuL)
    {
        TickCounter++;
        TickCounterInv = ~TickCounter;
        
        if (TickCounter >= SYSTICK_20ms_TB)
        {
            u32 RamTestResult;
            
            /* Reset timebase counter */
            TickCounter = 0u;
            TickCounterInv = 0xFFFFFFFFu;
            
            /* Set Flag read in main loop */
            TimeBaseFlag = 0xAAAAAAAAu;
            TimeBaseFlagInv = 0x55555555u;
            
            if ((CurrentHSEPeriod ^ CurrentHSEPeriodInv) == 0xFFFFFFFFuL)
            {
                ISRCtrlFlowCnt += MEASPERIOD_ISR_CALLER;
                CurrentHSEPeriod = STL_MeasurePeriod();
                CurrentHSEPeriodInv = ~CurrentHSEPeriod;
                ISRCtrlFlowCntInv -= MEASPERIOD_ISR_CALLER;
            }
            else  /* Class B Error on CurrentHSEPeriod */
            {
#ifdef STL_VERBOSE
                printf("\n\r Class B Error on CurrentHSEPeriod \n\r");
#endif  /* STL_VERBOSE */
            }
            
            ISRCtrlFlowCnt += RAM_MARCHC_ISR_CALLER;
            RamTestResult = RAM_RunCheck();
            ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLER;
            /*
            ISRCtrlFlowCnt += RAM_MARCHX_ISR_CALLER;
            RamTestResult = STL_TranspMarchX();
            ISRCtrlFlowCntInv -= RAM_MARCHX_ISR_CALLER;
            */
            switch ( RamTestResult )
            {
               case TEST_RUNNING:
                break;
               case TEST_OK:
#ifdef STL_VERBOSE                
                printf("\n\r Full RAM verified (Run-time)\n\r");                 
#endif  /* STL_VERBOSE */
                break;
               case TEST_FAILURE:
               case CLASS_B_DATA_FAIL:
               default:
#ifdef STL_VERBOSE
                /* >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check) */
#endif  /* STL_VERBOSE */
                FailSafePOR();
                break;
            } /* End of the switch */
            
            /* Do we reached the end of RAM test? */
            /* Verify 1st ISRCtrlFlowCnt integrity */
            if ((ISRCtrlFlowCnt ^ ISRCtrlFlowCntInv) == 0xFFFFFFFFuL)
            {
                if (RamTestResult == TEST_OK)
                {
                    if (ISRCtrlFlowCnt != RAM_TEST_COMPLETED)
                    {
#ifdef STL_VERBOSE
                        /* Control Flow error (RAM test) */
#endif  /* STL_VERBOSE */
                        FailSafePOR();
                    }
                    else  /* Full RAM was scanned */
                    {
                        ISRCtrlFlowCnt = 0u;
                        ISRCtrlFlowCntInv = 0xFFFFFFFFu;
                        
                        /* Rom test is ok ? */
                        if( g_u8RomCheckOkFlag == 1u )
                        {
                            /* Full RAM check ok, disable Timer */
                            TIM_Cmd(TIM4, DISABLE);
                            RCC_RTCCLKCmd(DISABLE);
                            g_u8RomCheckOkFlag = 0u;
                        }
                    }
                } /* End of RAM completed if*/
            } /* End of control flow monitoring */
            else
            {
#ifdef STL_VERBOSE
                /* Control Flow error in ISR */
#endif  /* STL_VERBOSE */
                FailSafePOR();
            }
        } /* End of the 20 ms timebase interrupt */
    }
    else  /* Class error on TickCounter */
    {
#ifdef STL_VERBOSE
        printf("\n\r Class B Error on TickCounter\n\r");
#endif  /* STL_VERBOSE */
        FailSafePOR();
    }          

}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
