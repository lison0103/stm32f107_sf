/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLcrc16Run.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/09/2007
* Description        : Contains the functions performing run time invariable
*                      memory checks based on 16-bit CRC
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
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : STL_FlashCrc16Init
* Description    : Inializes the pointers to the Flash memory required during
*                   run-time
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STL_FlashCrc16Init(void)
{
  p_RunCrc16Chk = ROM_START;
  p_RunCrc16ChkInv = ((u8 *)~((u32)ROM_START));

  CurrentCrc16 = 0u;
  CurrentCrc16Inv = 0xFFFFu;
}

/*******************************************************************************
* Function Name  : STL_crc16Run
* Description    : Computes the crc in multiple steps and compare it with the
*                   ref value when the whole memory has been tested
* Input          : None
* Output         : None
* Return         : ClassBTestStatus {TEST_RUNNING, CLASS_B_DATA_FAIL,
                    CTRL_FLW_ERROR, TEST_FAILURE, TEST_OK}
*******************************************************************************/
ClassBTestStatus STL_crc16Run(void)
{
    ClassBTestStatus Result = CTRL_FLW_ERROR; /* In case of abnormal func exit*/

  CtrlFlowCnt += CRC16_RUN_TEST_CALLEE;

  /* Check Class B var integrity */
  if ((((u32)p_RunCrc16Chk) ^ ((u32)p_RunCrc16ChkInv)) == 0xFFFFFFFFuL)
  { /* Check Class B var integrity */
    if ((CurrentCrc16 ^ CurrentCrc16Inv) == 0xFFFFu)
    {
      /* Last block is managed differently */
      if (p_RunCrc16Chk < ROM_END - FLASH_BLOCK)
      {
        CurrentCrc16 = STL_crc16(CurrentCrc16, p_RunCrc16Chk, FLASH_BLOCK);
        CurrentCrc16Inv = ~CurrentCrc16;  /* Store crc for next run */
        p_RunCrc16Chk += FLASH_BLOCK;     /* Increment pointer to next block */
        p_RunCrc16ChkInv = ((u8 *)~((u32)p_RunCrc16Chk));
        Result = TEST_RUNNING;
      }
      else
      {
        /* Do not include the ref checksum in the last block */
        CurrentCrc16 = STL_crc16(CurrentCrc16, p_RunCrc16Chk, FLASH_BLOCK-2u);
        if (CurrentCrc16 == REF_CRC16)
        {
          Result = TEST_OK;
        }
        else
        {
          Result = TEST_FAILURE;
        }
        STL_FlashCrc16Init(); /* Prepare next test (or redo it if this one failed) */
      }
    }
    else  /* Class B error CurrentCrc16 */
    {
      Result = CLASS_B_DATA_FAIL;
    }
  }
  else  /* Class B error p_RunCrc16Chk */
  {
    Result = CLASS_B_DATA_FAIL;
  }

  CtrlFlowCntInv -= CRC16_RUN_TEST_CALLEE;

  return (Result);

}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
