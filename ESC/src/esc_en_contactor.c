/*******************************************************************************
* File Name          : esc_en_contactor.c
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

/* static u8 Contact_FB_Mask=0x03u; */

static u8 Contact_Output_State=0u,Contact_FB_Input=0u;
static u16 Contact_Error_Tms[10]={0,0,0,0,0,0,0,0,0,0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Feedback_Contactor_Calc( u16 cbfb_mask, u8 seq);

/*******************************************************************************
* Function Name  : Feedback_Contactor_calc
* Description    : 
* Input          : CONTACTOR_FBx_MASK, x = 1~8, conbine contactor contact feedback
* Output         : None
* Return         : None
*******************************************************************************/
static void Feedback_Contactor_Calc( u16 cbfb_mask, u8 seq)
{
  /* if CONTACTOR_FB1_MASK & output true, should be no feedback */
  if( (cbfb_mask & Contact_Output_State) && (!(Contact_FB_Input & (u8)((u8)1u<<(u8)seq) )) )  
  {
    if( Contact_Error_Tms[seq] < CONTACTOR_FEEDBACK_FILTER ) 
    {
      Contact_Error_Tms[seq] += 5u; 
    }
  } 
  else if( (!(cbfb_mask & Contact_Output_State)) && (Contact_FB_Input & (u8)((u8)1u<<(u8)seq))) 
  {
    if( Contact_Error_Tms[seq] < CONTACTOR_FEEDBACK_FILTER ) 
    { 
      Contact_Error_Tms[seq] += 5u;
    }
  }  
  else
  {
    Contact_Error_Tms[seq] = 0u;
  }  
  
  if( Contact_Error_Tms[seq] >= CONTACTOR_FEEDBACK_FILTER)
  {
    switch(seq)
    {
      case 0: EN_ERROR35 |= 0x40u; break; 
      case 1: EN_ERROR35 |= 0x80u; break; 
      case 2: EN_ERROR36 |= 0x01u; break; 
      case 3: EN_ERROR36 |= 0x02u; break; 
      case 4: EN_ERROR36 |= 0x04u; break; 
      case 5: EN_ERROR36 |= 0x08u; break; 
      case 6: EN_ERROR36 |= 0x10u; break; 
      default: EN_ERROR36 |= 0x20u; break; 
    }  
  }    
}

/*******************************************************************************
* Function Name  : Feedback_Contactors_Check
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Feedback_Contactors_Check(void)
{
  u8 i;
  static u8 contactor_state_old=0u; 
  static u16 contactor_check_timeout=0u;

/*  Contact_FB_Mask = (((EscRtData. Cfg_Input_Mask[1])>>3u)&0x1Cu) | ((u8)(EscRtData. Cfg_Input_Mask[2]<<5u)&0xe0u) | 0x03u;           */
  
  Contact_FB_Input = ((EscRtData. Cfg_Input_Level[1]>>3u)&0x1Cu) | ((u8)(EscRtData. Cfg_Input_Level[2]<<5u)&0xe0u) | ((EscRtData.SafetyInputData[1]>>2) & 0x03u);      

  /* Contactor output state */
  Contact_Output_State = EscRtData.DataFromControl[0][4];
 
  /* Time delay */
  if( (SfBase_EscState == ESC_RUN_STATE) && (contactor_state_old != ESC_RUN_STATE) )   
  {
    contactor_check_timeout = 0u;
  }
  else  if( (SfBase_EscState != ESC_RUN_STATE) && (contactor_state_old == ESC_RUN_STATE) )   
  {
    contactor_check_timeout = 0u;
  } 
  else
  {
  } 
  
  contactor_state_old = SfBase_EscState;
    
  if( contactor_check_timeout > CONTACTORS_TIMEOUT )
  {
    if(CONTACTOR_FB1_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB1_MASK, 0u ); }
    if(CONTACTOR_FB2_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB2_MASK, 1u ); }
    if(CONTACTOR_FB3_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB3_MASK, 2u ); }
    if(CONTACTOR_FB4_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB4_MASK, 3u ); }
    if(CONTACTOR_FB5_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB5_MASK, 4u ); }
    if(CONTACTOR_FB6_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB6_MASK, 5u ); }
    if(CONTACTOR_FB7_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB7_MASK, 6u ); }
    if(CONTACTOR_FB8_MASK)  { Feedback_Contactor_Calc( (u16)CONTACTOR_FB8_MASK, 7u ); }
  }  
  else
  {
    contactor_check_timeout += 5u;
    
    for(i=0u;i<10u;i++)  
    {
      Contact_Error_Tms[i] = 0u;
    }  
  } 
}

/******************************  END OF FILE  *********************************/