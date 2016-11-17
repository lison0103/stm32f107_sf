/*******************************************************************************
* File Name          : esc_aux_brake.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 11/07/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"
#include "bsp_iocfg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void MainShaftBrakePawlSupervision(void);

/*
#define AUX_BRAKE_FEEDBACK_INPUT   (EscRtData. SafetyInputData[3] & 0x04)
*/

#define AUX_BRAKE_FEEDBACK_INPUT   (EscRtData. Cfg_Input_Level[0] &(0x01u))

/*******************************************************************************
* Function Name  : MainShaftBrakePawlSupervision
* Description    : Check main shaft feedbcak,comparing main shaft driver             
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void MainShaftBrakePawlSupervision(void)
{
  static u16 PawlSupervsionTime=0u,RunStatePawlTime=0u;

  if(SfBase_EscState == ESC_RUN_STATE) 
  {
    PawlSupervsionTime = 0u;
    
    if( AUX_BRAKE_FEEDBACK_INPUT ) /* Aux brake feedback actived */
    {
      RunStatePawlTime  = 0u;
    }    
    else 
    {  
      /* No feedback, Fault counter increased by one, 5ms */
      RunStatePawlTime ++;
      
      /*if fault time > 100ms, go fault*/
      if(RunStatePawlTime >20u) /* fault time: 100ms */
      {
        EN_ERROR28 |= 0x01u;       
      }  
    }  
  }  
  else 
  {
    RunStatePawlTime  = 0u;

    if( ( CMD_FLAG5 & 0x04u ) )   /* Aux relay unlock flag   */
    {
      if( AUX_BRAKE_FEEDBACK_INPUT ) /* Aux feedback active */
      {
        PawlSupervsionTime = 0u;
      }  
      else
      {
        if(PawlSupervsionTime>1000u) /* Fault time > 5s*/
        {
          EN_ERROR28 |= 0x01u; 
        }
        else
        {
          PawlSupervsionTime++;
        }  
      }  
    }  
    else   /* Aux relay lock flag */
    {
      if( AUX_BRAKE_FEEDBACK_INPUT ) /* Aux feedback active */
      {
        if(PawlSupervsionTime>1000u) /* Fault time > 5s*/
        {
          EN_ERROR28 |= 0x02u; 
        }
        else 
        { 
          PawlSupervsionTime++; 
        }  
      }  
      else
      { 
        PawlSupervsionTime = 0u; 
      }   
    }   
  }  
}  
 
/*******************************************************************************
* Function Name  : Aux_Brake_CS
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Aux_Brake_CS(void)
{
  static u16 AR_on_counter=0u,AR_stop_counter=0u;
 
  CMD_FLAG5 |= 0x10u;
  
  if( (SfBase_EscState == ESC_RUN_STATE) || (SfBase_EscState == ESC_STARTING_PROCESS_STATE) ) 
  {
    AR_stop_counter = 0u;
    AUX_RELAY_ON();   	
    
    CMD_FLAG5 |= 0x04u;
  } 
  else
  {
    if(AR_stop_counter++>1000u)
    {  
       AUX_RELAY_OFF();
       
       CMD_FLAG5 &= ~0x04u;
    }
  }   
  
  if( AUX_BRAKE_FEEDBACK_INPUT )
  {  
    CMD_FLAG5 |= 0x20u;  
  }
  else
  {
    CMD_FLAG5 &= ~0x20u;  
  } 
  
  MainShaftBrakePawlSupervision();
}

/******************************  END OF FILE  *********************************/

