/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLfullRamMc.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/01/2007
* Description        : This file contains the RAM functional to be done at
*                      start-up. This test is destructive and will initialize
*                      the whole RAM to zero.
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
#define RAM_BLOCKSIZE         ((u32)16)
static uc8 RAM_SCRMBL[RAM_BLOCKSIZE] = {0,1,3,2,4,5,7,6,8,9,11,10,12,13,15,14};
static uc8 RAM_REVSCRMBL[RAM_BLOCKSIZE] = {1,0,2,3,5,4,6,7,9,8,10,11,13,12,14,15};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : RAM_StarupCheck
* Description    : This function verifies that RAM is functional,
*                  using the March C- algorithm.
* Input          : None
* Output         : The whole RAM is initialized with 0 when exiting this fct,
*                  at the exception of CtrlFlowCntInv, set to 0xFFFFFFFF.
* Return         : ErrorStatus = {ERROR; SUCCESS}
*******************************************************************************/
ErrorStatus RAM_StarupCheck(void)
{
      ErrorStatus Result = SUCCESS;
      u32 *p;       /* RAM pointer */
      u32 j;        /* Index for RAM physical addressing */

   /* CtrlFlowCnt not used at function entry, since it will be cleared by the
      routine. CtrlFlowCntInv is written at the function exit */

   /* ---------------------------- STEP 1 ----------------------------------- */
   /* Write background with addresses increasing */
   for (p = RAM_START; p <= RAM_END; p++)
   {
      /* Scrambling not important when there's no consecutive verify and write */
      *p = BCKGRND;
   }

   /* ---------------------------- STEP 2 ----------------------------------- */
   /* Verify background and write inverted background with addresses increasing */
   for (p = RAM_START; p <= RAM_END; p += RAM_BLOCKSIZE)
   {
      for (j = 0u; j < RAM_BLOCKSIZE; j++)
      {
         if ( *(p + (u32)RAM_SCRMBL[j]) != BCKGRND)
         {
            Result = ERROR;
         }
         *(p + (u32)RAM_SCRMBL[j]) = INV_BCKGRND;
      }
   }

   /* ---------------------------- STEP 3 ----------------------------------- */
   /* Verify inverted background and write background with addresses increasing */
   for (p = RAM_START; p <= RAM_END; p += RAM_BLOCKSIZE)
   {
      for (j = 0u; j < RAM_BLOCKSIZE; j++)
      {
         if ( *(p + (u32)RAM_SCRMBL[j]) != INV_BCKGRND)
         {
            Result = ERROR;
         }
         *(p + (u32)RAM_SCRMBL[j]) = BCKGRND;
      }
   }

   /* ---------------------------- STEP 4 ----------------------------------- */
   /* Verify background and write inverted background with addresses decreasing */
   for (p = RAM_END; p > RAM_START; p -= RAM_BLOCKSIZE)
   {
      for (j = 0u; j < RAM_BLOCKSIZE; j++)
      {
         if ( *(p - (u32)RAM_REVSCRMBL[j]) != BCKGRND)
         {
            Result = ERROR;
         }
         *(p - (u32)RAM_REVSCRMBL[j]) = INV_BCKGRND;
      }
   }

   /* ---------------------------- STEP 5 ----------------------------------- */
   /* Verify inverted background and write background with addresses decreasing */
   for (p = RAM_END; p > RAM_START; p -= RAM_BLOCKSIZE)
   {
      for (j = 0u; j < RAM_BLOCKSIZE; j++)
      {
         if ( *(p - (u32)RAM_REVSCRMBL[j]) != INV_BCKGRND)
         {
            Result = ERROR;
         }
         *(p - (u32)RAM_REVSCRMBL[j]) = BCKGRND;
      }
   }

   /* ---------------------------- STEP 6 ----------------------------------- */
   /* Verify background with addresses increasing */
   for (p = RAM_START; p <= RAM_END; p++)
   {
      if (*p != BCKGRND)
      {
         Result = ERROR;    /* No need to take into account scrambling here */
      }
   }

   CtrlFlowCntInv = 0xFFFFFFFFuL;
   return (Result);

}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
