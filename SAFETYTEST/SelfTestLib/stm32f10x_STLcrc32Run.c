/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLcrc16Run.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/09/2007
* Description        : Contains the functions performing run time invariable
*                      memory checks based on 32-bit CRC
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
#include "stm32f10x_lib.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : STL_FlashCrc32Init
* Description    : Inializes the pointers to the Flash memory required during
*                   run-time
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STL_FlashCrc32Init(void)
{
  p_RunCrc32Chk = (u32*)ROM_START;
  p_RunCrc32ChkInv = ((u32 *)~((u32)ROM_START));

  CRC_Init(); /* Reset the CRC generator */
}

/*******************************************************************************
* Function Name  : STL_crc32Run
* Description    : Computes the crc in multiple steps and compare it with the
*                   ref value when the whole memory has been tested
* Input          : None
* Output         : None
* Return         : ClassBTestStatus {TEST_RUNNING, CLASS_B_DATA_FAIL,
                    CTRL_FLW_ERROR, TEST_FAILURE, TEST_OK}
*******************************************************************************/
ClassBTestStatus STL_crc32Run(void)
{
    ClassBTestStatus Result = CTRL_FLW_ERROR; /* In case of abnormal func exit*/

  CtrlFlowCnt += CRC32_RUN_TEST_CALLEE;

  /* Check Class B var integrity */
  if ((((u32)p_RunCrc32Chk) ^ ((u32)p_RunCrc32ChkInv)) == 0xFFFFFFFFuL)
  {
    if (p_RunCrc32Chk < (u32 *)ROM_END)
    {
      CRC_CalcBlockCrc((uc32 *)p_RunCrc32Chk, (u32)FLASH_BLOCK_WORDS);
      p_RunCrc32Chk += FLASH_BLOCK_WORDS+1;     /* Increment pointer to next block */
      p_RunCrc32ChkInv = ((u32 *)~((u32)p_RunCrc32Chk));
      Result = TEST_RUNNING;
    }
    else
    {
      if ((RefCrc32 ^ RefCrc32Inv) == 0xFFFFFFFFuL)
      {
        /* The reference CRC was stored in a Class B variable */
        if (GetCurrentCRC() == RefCrc32)
        {
          Result = TEST_OK;
        }
        else
        {
          Result = TEST_FAILURE;
        }
        STL_FlashCrc32Init(); /* Prepare next test (or redo it if this one failed) */
      }
      else /* Class B error on RefCrc32 */
      {
        Result = CLASS_B_DATA_FAIL;
      }
    }
  }
  else  /* Class B error p_RunCrc32Chk */
  {
    Result = CLASS_B_DATA_FAIL;
  }

  CtrlFlowCntInv -= CRC32_RUN_TEST_CALLEE;

  return (Result);

}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
