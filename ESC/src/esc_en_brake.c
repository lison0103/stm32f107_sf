/*******************************************************************************
* File Name          : esc_en_brake.c
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
static u8 Brake_Number=2u,Brake_Mask=0x03u,Brake_Input=0u; 
static u16 Brake_Timeout_Tms=0u, Brake_Run_Tms=10000u,Brake_Stop_Tms=10000u,Brake_Error_Tms[8]={0,0,0,0,0,0,0,0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Brake_Status_Check
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Brake_Status_Check(void)
{
  u8 i = 0u;

  Brake_Timeout_Tms = CONTACTORS_TIMEOUT; 
  Brake_Timeout_Tms *= 1000u;

/*  
  Brake_Mask=0x03u;
  Brake_Input=0u; 
  if(INPUT_PORT9_16 & INPUT_PORT13_MASK) { Brake_Input |= 0x01u; }
  if(INPUT_PORT9_16 & INPUT_PORT14_MASK) { Brake_Input |= 0x02u; }
*/
  
  if( SfBase_EscState == ESC_RUN_STATE ) 
  {
    Brake_Stop_Tms = 0u;
    
    if( Brake_Run_Tms > Brake_Timeout_Tms )  /* run delay time */
    {
      for(i=0u;i<8u;i++)  
      {
        if( Brake_Input & (1u<<i) )
        {
          if( Brake_Error_Tms[i]<10000u) /* Fault filter time increasing*/
          {
            Brake_Error_Tms[i] += 5u;
          }  
        } 
        else
        {
          Brake_Error_Tms[i] = 0u; 
        }  
      }  
    } 
    else
    {
      for(i=0u;i<8u;i++)  
      {
        Brake_Error_Tms[i] = 0u;
      }  
      
      Brake_Run_Tms += 5u; 
    }  
      
  } 
  else  /*  Escalator Stop  */
  {
    Brake_Run_Tms = 0u;
    
    if( Brake_Stop_Tms > Brake_Timeout_Tms )  /* stop delay time */
    {
      for(i=0u;i<8u;i++)  
      {  
        if( (Brake_Mask&((1u<<i))) && (!(Brake_Input & (1u<<i)))  )
        {
          if( Brake_Error_Tms[i] < 10000u)
          {
             Brake_Error_Tms[i] += 5u;
          } 
        } 
        else
        {
          Brake_Error_Tms[i] = 0u; 
        }  
      }    
    } 
    else
    {
      for(i=0u;i<8u;i++)  
      {
        Brake_Error_Tms[i] = 0u;
      }  

      Brake_Stop_Tms += 5u; 
    }  
  }  
  
  for(i=0u;i<8u;i++)  
  {  
    if( Brake_Error_Tms[i] > 1500u)
    {  
      if(CMD_FLAG3 & 0x10u)
      {        
        switch(i)
        {
          case 0x01: EN_ERROR3 |= 0x04u; break; 
          case 0x02: EN_ERROR3 |= 0x04u; break; 
          case 0x04: EN_ERROR3 |= 0x04u; break; 
          case 0x08: EN_ERROR3 |= 0x04u; break; 
          case 0x10: EN_ERROR3 |= 0x04u; break; 
          case 0x20: EN_ERROR3 |= 0x04u; break; 
          case 0x40: EN_ERROR3 |= 0x04u; break; 
          default:   EN_ERROR3 |= 0x04u; break; 
        }   
      }
      else /* inspection mode ,warning only */
      {
        switch(i)
        {
          case 0x01: EN_WARN1 |= 0x04u; break; 
          case 0x02: EN_WARN1 |= 0x04u; break; 
          case 0x04: EN_WARN1 |= 0x04u; break; 
          case 0x08: EN_WARN1 |= 0x04u; break; 
          case 0x10: EN_WARN1 |= 0x04u; break; 
          case 0x20: EN_WARN1 |= 0x04u; break; 
          case 0x40: EN_WARN1 |= 0x04u; break; 
          default:   EN_WARN1 |= 0x04u; break; 
        }   
      }  
    }    
  }
  
  if(CMD_FLAG3 & 0x10u)
  {
    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;} 
    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;} 
    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;} 
    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;} 

    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;} 
    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;} 
    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;} 
    if( EN_WARN1 &0x01u ) { EN_WARN1 &= ~0x01u; EN_ERROR3 |= 0x04u;}     
  }  
}

/******************************  END OF FILE  *********************************/