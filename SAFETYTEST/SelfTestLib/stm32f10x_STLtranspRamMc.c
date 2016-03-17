/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_STLtranspRamMc.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 09/26/2007
* Description        : This module contains functions for transparent RAM
*                      functional testusing March C- algorithm, to be done
*                      during run-time.
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
#define RT_RAM_BLOCK_OVERLAP  ((u32)1)  /* Min overlap to cover coupling fault
                                           from one tested row to the other */
#define RT_RAMBUF_BLOCKSIZE   ((u32)9)  /* Reserved area for RAM buffer, incl
                                           overlap for test purposes */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* These arrays contain scramblong/descrambling patterns for 6 and 9 words */
static sc8 RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE] = {-2,0,1,3,2,4};
static sc8 RT_RAMBUF_SCRMBL[RT_RAMBUF_BLOCKSIZE] = {-1,0,2,1,3,4,6,5,7};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : STL_TranspMarchCInit
* Description    : Initializes the pointer to the RAM for the run-time
*                   transparent functional test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STL_TranspMarchCInit(void)
{
   p_RunTimeRamChk = CLASS_B_START;
   p_RunTimeRamChkInv = ((u32 *)~((u32)CLASS_B_START));
}


/*******************************************************************************
* Function Name  : STL_TranspMarchC
* Description    : This function verifies that 6 words of RAM are functional,
                    (with overlapping) using the March C- algorithm.
* Input          : None
* Output         : None
* Return         : ClassBTestStatus {TEST_RUNNING, CLASS_B_DATA_FAIL,
                    CTRL_FLW_ERROR, TEST_FAILURE, TEST_OK}
*******************************************************************************/
ClassBTestStatus STL_TranspMarchC(void)
{
      ClassBTestStatus Result = TEST_RUNNING;
      u32 i;        /* Index for RAM physical addressing */
      sc8 *pBlock;  /* Index for addresses scrambling/descrambling */

  ISRCtrlFlowCnt += RAM_MARCHC_ISR_CALLEE;

  /* Check Class B var integrity */
  if ((((u32)p_RunTimeRamChk) ^ ((u32)p_RunTimeRamChkInv)) == 0xFFFFFFFFuL)
  {
    if (p_RunTimeRamChk >= CLASS_B_END)
    {
      /*------------- Apply March C- to the RAM Buffer itself --------------- */
      p_RunTimeRamChk = &RunTimeRamBuf[0];
      p_RunTimeRamChkInv = (u32*)(~(u32)(&RunTimeRamBuf[0]));

      /*---------------------------- STEP 1 --------------------------------- */
      /* Write background with addresses increasing */
      pBlock = &RT_RAMBUF_SCRMBL[0];
      do
      {
        *(p_RunTimeRamChk + *pBlock) = BCKGRND;
        pBlock++;
      }
      while(pBlock < &RT_RAMBUF_SCRMBL[RT_RAMBUF_BLOCKSIZE]);

      /*---------------------------- STEP 2 --------------------------------- */
      /* Verify background and write inverted background addresses increasing */
      pBlock = &RT_RAMBUF_SCRMBL[0];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = INV_BCKGRND;
        pBlock++;
      }
      while(pBlock < &RT_RAMBUF_SCRMBL[RT_RAMBUF_BLOCKSIZE]);

      /*---------------------------- STEP 3 --------------------------------- */
      /* Verify inverted background and write background addresses increasing */
      pBlock = &RT_RAMBUF_SCRMBL[0];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != INV_BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = BCKGRND;
        pBlock++;
      }
      while(pBlock < &RT_RAMBUF_SCRMBL[RT_RAMBUF_BLOCKSIZE]);

      /*---------------------------- STEP 4 --------------------------------- */
      /* Verify background and write inverted background addresses decreasing */
      pBlock = &RT_RAMBUF_SCRMBL[RT_RAMBUF_BLOCKSIZE-1];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = INV_BCKGRND;
        pBlock--;
      }
      while(pBlock >= &RT_RAMBUF_SCRMBL[0]);

      /*---------------------------- STEP 5 --------------------------------- */
      /* Verify inverted background and write background addresses decreasing */
      pBlock = &RT_RAMBUF_SCRMBL[RT_RAMBUF_BLOCKSIZE-1];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != INV_BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = BCKGRND;
        pBlock--;
      }
      while(pBlock >= &RT_RAMBUF_SCRMBL[0]);

      /*---------------------------- STEP 6 --------------------------------- */
      /* Verify background with addresses increasing */
      pBlock = &RT_RAMBUF_SCRMBL[0];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        pBlock++;
      }
      while(pBlock < &RT_RAMBUF_SCRMBL[RT_RAMBUF_BLOCKSIZE]);

      /* Check again Class B variable integrity */
      if ((((u32)p_RunTimeRamChk) ^ ((u32)p_RunTimeRamChkInv)) == 0xFFFFFFFFuL)
      {
        /* Prepare next Tranparent RAM test from the beginning of Class A area */
        p_RunTimeRamChk = CLASS_B_START;
        p_RunTimeRamChkInv = ((u32 *)~((u32)CLASS_B_START));

        if (Result == TEST_RUNNING)
        {
          Result = TEST_OK; /* Means all selected variable memory was scanned */
        }
        else  /* Buffer is not functional */
        {
          Result = TEST_FAILURE;
        }
      }
      else  /* Class B error on p_RunTimeRamChk */
      {
        Result = CLASS_B_DATA_FAIL;
      }

    } /* ------------------ End of Buffer Self-check ------------------------ */
    else
    { /* ------------------ Regular memory Self-check ----------------------- */
      /*---------------------------- STEP 1 --------------------------------- */
      /* Save the content of the 6 words to be tested and start MarchC-
         Write background with addresses increasing */
      pBlock = &RT_RAM_SCRMBL[0]; /* Takes into account RAM scrambling */
      i=0;
      do
      {
        RunTimeRamBuf[i++] = *(p_RunTimeRamChk + *pBlock);
        *(p_RunTimeRamChk + *pBlock) = BCKGRND;
        pBlock++;
      }
      while(pBlock < &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE]);

      /*---------------------------- STEP 2 --------------------------------- */
      /* Verify background and write inverted background addresses increasing */
      pBlock = &RT_RAM_SCRMBL[0];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = INV_BCKGRND;
        pBlock++;
      }
      while(pBlock < &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE]);

      /*---------------------------- STEP 3 --------------------------------- */
      /* Verify inverted background and write background addresses increasing */
      pBlock = &RT_RAM_SCRMBL[0];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != INV_BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = BCKGRND;
        pBlock++;
      }
      while(pBlock < &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE]);

      /*---------------------------- STEP 4 --------------------------------- */
      /* Verify background and write inverted background addresses decreasing */
      pBlock = &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE-1];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = INV_BCKGRND;
        pBlock--;
      }
      while(pBlock >= &RT_RAM_SCRMBL[0]);

      /*---------------------------- STEP 5 --------------------------------- */
      /* Verify inverted background and write background addresses decreasing */
      pBlock = &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE-1];
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != INV_BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = BCKGRND;
        pBlock--;
      }
      while(pBlock >= &RT_RAM_SCRMBL[0]);

      /*---------------------------- STEP 6 --------------------------------- */
      /* Verify background with addresses increasing */
      /* and restore the content of the 6 tested words */
      pBlock = &RT_RAM_SCRMBL[0];
      i=0;
      do
      {
        if ( *(p_RunTimeRamChk + *pBlock) != BCKGRND)
        {
          Result = TEST_FAILURE;
        }
        *(p_RunTimeRamChk + *pBlock) = RunTimeRamBuf[i++];
        pBlock++;
      }
      while(pBlock < &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE]);

      /* Check again Class B variable integrity */
      if ((((u32)p_RunTimeRamChk) ^ ((u32)p_RunTimeRamChkInv)) == 0xFFFFFFFFuL)
      {
        /* Prepare next Row Tranparent RAM test */
        p_RunTimeRamChk += RT_RAM_BLOCKSIZE- (2*RT_RAM_BLOCK_OVERLAP);
        p_RunTimeRamChkInv = (u32 *)(~((u32)p_RunTimeRamChk));

        if (Result != TEST_RUNNING)
        {
          Result = TEST_FAILURE;  /* Word line under test was not functional */
        }
        else
        {
          // Do nothing: keep Result as TEST_RUNNING;
        }
      }
      else  /* Class B error on p_RunTimeRamChk when exiting the function*/
      {
        Result = CLASS_B_DATA_FAIL;
      }
    } /* --------------- End of Regular memory Self-check --------------------- */
  }
  else  /* Class B error on p_RunTimeRamChk when entering the function*/
  {
    Result = CLASS_B_DATA_FAIL;
  }

  ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLEE;

  return (Result);

}


/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
