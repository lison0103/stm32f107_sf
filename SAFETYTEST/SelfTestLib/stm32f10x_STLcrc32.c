/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLcrc32.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : This file provides all the internal CRC generator firmware
*                      functions.
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
//#include "stm32f10x_lib.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : CRC_DeInit
* Description    : Deinitializes the CRC peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CRC_DeInit(void)
{
  /* Reset CRC generator */
  CRC->CR = CRC_RESET;
}

/*******************************************************************************
* Function Name  : CRC_Init
* Description    : Initializes CRC peripheral (enable its clock and reset CRC)
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void CRC_Init(void)
{
    /* This is for control flow monitoring */
  CtrlFlowCnt += CRC32_INIT_CALLEE;

  /* Enable CRC module clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC ,ENABLE);

  /* Reset CRC generator */
  CRC->CR = CRC_RESET;

  CtrlFlowCntInv -= CRC32_INIT_CALLEE;

}

/*******************************************************************************
* Function Name  : CRC_CalcBlockCrc
* Description    : Computes the 16-bit CRC of a given part of the memory (words
*                  by words).
* Input          : - p: points to the first memory byte to be taken into account
*                  - len: length (in bytes) of the memory to be computed					
* Output         : None
* Return         : 32-bit CRC using the 0x4C11DB7 polynomial
*******************************************************************************/
u32 CRC_CalcBlockCrc(uc32 *p, u32 len)
{

  /* This is for control flow monitoring */
  CtrlFlowCnt += CRC32_TEST_CALLEE;

  /* Load memory content into the CRC generator data register */
  do
  {
    CRC->DR = *p++;
  }
  while (len--);

  CtrlFlowCntInv -= CRC32_TEST_CALLEE;

  return (CRC->DR);

}


/*******************************************************************************
* Function Name  : SetIDRegister
* Description    : Store a 8-bit data in the peripheral. Allows to keep
*                  a test ID or the 8 LSbits of a previous CRC for instance.
* Input          : The 8-bit value to be stored in this register					
* Output         : None
* Return         : None
*******************************************************************************/
void SetIDRegister(u8 Value)
{
  CRC->IDR = Value;
}

/*******************************************************************************
* Function Name  : GetIDRegister
* Description    : Get the 8-bit data stored in the peripheral
* Input          : None
* Output         : None
* Return         : 8-bit value stored in the ID register of the CRC peripheral
*******************************************************************************/
u8 GetIDRegister(void)
{
  return(CRC->IDR);
}

/*******************************************************************************
* Function Name  : GetCurrentCRC
* Description    : Get the current CRC value
* Input          : None
* Output         : None
* Return         : 32-bit CRC
*******************************************************************************/
u32 GetCurrentCRC(void)
{
  return(CRC->DR);
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
