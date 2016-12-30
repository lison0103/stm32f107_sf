/*******************************************************************************
* File Name          : esc_isp_normal_mode.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 11/07/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 IspNormal_Input_High_Counter=0u,IspNormal_Input_Low_Counter=0u;
/* static u8 IspNormal_Error_Counter=0,IspNormal_OFF_Counter=0; */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Inspection_Normal_Key(void);

/*******************************************************************************
* Function Name  : Inspection_Normal_Key
* Description    : 
* Input          : ISP_NORMAL_INPUT, inspection/normal key input
* Output         : None
* Return         : None
*******************************************************************************/
static void Inspection_Normal_Key(void)
{
  static u8 isp_nrm_key_on_tms=0u,isp_nrm_key_off_tms=0u;

  if((!(ISP_NORMAL_INPUT)) && ( CMD_FLAG3 & 0x10u )) /* Normal/isp key input low level, but current mode is normal mode */
  {
    isp_nrm_key_on_tms =0u; /* Reset */
    
    if(isp_nrm_key_off_tms<20u) /* 20*5ms 100ms */
    {
      isp_nrm_key_off_tms++;
    }
    else
    {  
      if((SfBase_EscState == ESC_STARTING_PROCESS_STATE) ||  (SfBase_EscState == ESC_RUN_STATE) || \
         (SfBase_EscState == ESC_STOPPING_PROCESS_STATE) || (SfBase_EscState == ESC_INTERM_STATE))       /* Fault */
      {
        EN_ERROR50 |= 0x02u;   /* F393 */
      }  
      else if( CMD_FLAG2 & 0x0Cu )  /* if UP/DOWN maintenance button press, go Fault */
      {
        EN_ERROR50 |= 0x10u;  /* F396 */
      }
      else
      {
        CMD_FLAG3 &= ~0x10u;  /* Inspection mode */
      }  
    }  
  }  
  else if((ISP_NORMAL_INPUT) && (!(CMD_FLAG3 & 0x10u)) )/* Normal/isp key input high level, current mode is inspection mode */
  {
    isp_nrm_key_off_tms =0u;

    if(isp_nrm_key_on_tms<20u)  /* 20*5ms 100ms */
    {
      isp_nrm_key_on_tms++; 
    }
    else
    {  
      if((SfBase_EscState == ESC_STARTING_PROCESS_STATE) ||  (SfBase_EscState == ESC_RUN_STATE) || (SfBase_EscState == ESC_STOPPING_PROCESS_STATE))       /* Fault */
      {
        EN_ERROR50 |= 0x02u;/* F393 */
      }  
      else
      {
        CMD_FLAG3 |= 0x10u; /* Normal mode */
      }  
    }  
  } 
  else
  {
  }  
}

/*******************************************************************************
* Function Name  : Inspection_Normal_Key_Check
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Inspection_Normal_Key_Check(void)
{
  
  Inspection_Normal_Key();
  
}

/******************************  END OF FILE  *********************************/