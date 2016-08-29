/*******************************************************************************
* File Name          : sys.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains assembly functions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : WFI_SET
* Description    : The THUMB assembler does not support inline.
*                  Use the following methods to achieve the implementation of assembly instructions WFI
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/ 
void WFI_SET(void)
{
	__ASM("WFI");		  
}

/*******************************************************************************
* Function Name  : INTX_DISABLE
* Description    : Close all interrupts.
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/ 
void INTX_DISABLE(void)
{
	__ASM("CPSID I");    		  
}

/*******************************************************************************
* Function Name  : INTX_ENABLE
* Description    : Open all interrupts.
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/ 
void INTX_ENABLE(void)
{
	__ASM("CPSIE I");		  
}

/*******************************************************************************
* Function Name  : MSR_MSP
* Description    : Set stack address.
* Input          : addr: address of the stack                
* Output         : None
* Return         : None
*******************************************************************************/ 
void MSR_MSP(u32 addr) 
{
    __ASM("MSR MSP, r0"); 			/* set Main Stack value */
    __ASM("BX r14");
}


/******************************  END OF FILE  *********************************/



