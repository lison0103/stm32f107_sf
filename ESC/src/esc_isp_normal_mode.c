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
static void Inspection_Normal_Key_Ready(void);
static void Inspection_Normal_Key_Run(void);


/*******************************************************************************
* Function Name  : Inspection_Normal_Key_Ready
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Inspection_Normal_Key_Ready(void)
{
  if(IspNormal_Input_High_Counter > 250u)    /* Normal running */
  {
    CMD_FLAG3 &= ~0x10u;             
  }   
  else if(IspNormal_Input_Low_Counter > 250u)  /* Inspection running */
  {
    CMD_FLAG3 |= 0x10u;                             
  }  
  else
  {}
}

/*******************************************************************************
* Function Name  : Inspection_Normal_Key_Run
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Inspection_Normal_Key_Run(void)
{
   if( CMD_FLAG3 & 0x10u )  /* Inspection running */
   {
     if( IspNormal_Input_High_Counter > 100u )
     {
       EN_ERROR31 |= 0x01u;
     }  
   } 
   else if((CMD_FLAG3 & 0x10u) != 0x10u) /* Normal running */
   {
     if( IspNormal_Input_Low_Counter > 100u )
     {
       EN_ERROR31 |= 0x01u;
     }  
   }  
   else
   {}
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
  static u16 INE_tms=0u;
  
  if(ISP_NORMAL_INPUT)
  {
    IspNormal_Input_Low_Counter = 0u;
     
    if(IspNormal_Input_High_Counter < 255u)
    {
      IspNormal_Input_High_Counter++;
    } 
  }  
  else
  {
    IspNormal_Input_High_Counter = 0u;
     
    if(IspNormal_Input_Low_Counter < 255u)
    {
      IspNormal_Input_Low_Counter++;
    } 
  } 
  
  if( SfBase_EscState == ESC_RUN_STATE ) 
  {
    Inspection_Normal_Key_Run();
  }  
  else
  {
    Inspection_Normal_Key_Ready();
  }  
  
  if(EN_ERROR31 & 0x01u)
  {
      if(INE_tms++>500u) { EN_ERROR31 &= ~0x01u;}
  }  
  else
  {
    INE_tms=0u;
  }  
}

/******************************  END OF FILE  *********************************/