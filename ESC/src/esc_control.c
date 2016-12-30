/*******************************************************************************
* File Name          : esc_control.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 11/07/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"
#include "esc_safety_check.h"
/* 
#include "esc_motor_speed.h"
#include "initial_devices.h"
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void En_Check(void);
static void Esc_Control_Starting_Prpcess(void);
static void Esc_Control_Run(void);
static void Esc_Control_Stop(void);

static u8 Start_Process_State=0u;

/*******************************************************************************
* Function Name  : En_Check
* Description    : En_Check.             
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void En_Check(void)
{
  
}

/*******************************************************************************
* Function Name  : Esc_Control_Starting_Prpcess
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Esc_Control_Starting_Prpcess(void)
{
  static u8 delay_cnt=0u;
  

  if( CMD_ESC_RUN_MODE & ESC_NORMAL )
  {
/*    g_u8SafetyRelayStartCheck = 1u;    */
    /* return; */
  }
  else
  {
      switch(Start_Process_State)
      {
         case 0u:
          
/*          g_u8SafetyRelayStartCheck = 0u;*/
          delay_cnt = 0u;
          Start_Process_State = 1u;
          
          break;
         case 1u:
          
          SafetyOutputDisable();
          /* ACTIVATE_UP_CONTACTOR & STATE_STARTING_PROCESS state */
          EscDataToControl[0][1] |= ORDER_ACTIVATE_UP_CONTACTOR;
          EscDataToControl[0][2] &= ~ESC_STATE_MASK;
          EscDataToControl[0][2] |= ESC_STATE_STARTING_PROCESS;
          
          if(delay_cnt++>100u)
          {
              Start_Process_State = 2u;
              delay_cnt = 0u;
          }  
          break;
          
         case 2u:
          
          SafetyOutputDisable();
          /* STATE_STARTING_PROCESS state */
          EscDataToControl[0][1] &= ~ORDER_ACTIVATE_UP_CONTACTOR;
          EscDataToControl[0][2] &= ~ESC_STATE_MASK;
          EscDataToControl[0][2] |= ESC_STATE_STARTING_PROCESS;
          
          if(delay_cnt++>20u)
          {
              Start_Process_State = 3u;
              delay_cnt = 0u;
          }  
          break;
          
         case 3u:      
          
          SafetyOutputEnable();
          /* STATE_STARTING_PROCESS state */
          EscDataToControl[0][1] &= ~ORDER_ACTIVATE_UP_CONTACTOR;
          EscDataToControl[0][2] &= ~ESC_STATE_MASK;
          EscDataToControl[0][2] |= ESC_STATE_STARTING_PROCESS;
          
          if(delay_cnt++>100u)
          {
              Start_Process_State = 4u;
              delay_cnt = 0u;
          }         
          break;   
          
         case 4u:
          
          /*SfBase_EscState = ESC_RUN_STATE;*/
          /* g_u8SafetyRelayStartCheck = 1u; */
          
          break;
         default:
          /*  Fault  */
          break;      
      }  
  }
}

/*******************************************************************************
* Function Name  : Esc_Control_Stop
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Esc_Control_Stop(void) 
{
    
    EscDataToControl[0][1] &= ~0x0Eu;
    
    if( SfBase_EscState == ESC_FAULT_STATE )
    {
        if(( EscRtData.DataFromControl[0][2] & ESC_FROM_CB_STATE_MASK ) == ESC_FROM_CB_SF_STATE_FAULT )
        {
            /* removes the order (=0) when checks that the control board is in FAULT state */
            EscDataToControl[0][1] &= ~ORDER_FAULT;
        }
        else
        {
            /* Safety activates the order (=1) when a fault is detected  */
            /* ORDER_FAULT */ 
            EscDataToControl[0][1] |= ORDER_FAULT;            
        }
    }
    else if( SfBase_EscState == ESC_STOPPING_PROCESS_STATE )
    {
        /* the order remains actived during the STOPPING PROCESS state */
        /* ORDER_STOP */
        EscDataToControl[0][1] |= ORDER_STOP;       
    }    
    else if( SfBase_EscState == ESC_READY_STATE )
    {
        if(( EscRtData.DataFromControl[0][2] & ESC_FROM_CB_STATE_MASK ) == ESC_FROM_CB_STATE_READY )
        {
            /* Safety board activates reset order when the last active fault is reset */
            EscDataToControl[0][1] &= ~ORDER_RESET;
        }
        else
        {
            /* removes the order (=0) when checks that the control board is in READY state, maximum 200ms  */
            /* ORDER_RESET */ 
            EscDataToControl[0][1] |= ORDER_RESET;            
        }        
    }
    else
    {

    }
    
    /* stopping process */
    if( SfBase_EscState != ESC_STOPPING_PROCESS_STATE )
    {    
        EscDataToControl[0][3] |= ESC_STOPPING_PROCESS_FINISH;
    }

    /* ORDER_RUN / ORDER_CONTACTOR / ORDER_CAPACITOR */
    EscDataToControl[0][1] &= ~0x91u;  
}

#if 0
/*******************************************************************************
* Function Name  : Esc_Control_Run
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Esc_Control_Run(void)
{
  /* Normal mode*/
  EscDataToControl[0][2] |=0x08u; 

  
  if(CMD_FLAG6 & 0x03u)
  {
      /* ORDER_RUN */ 
    EscDataToControl[0][1] |= 0x01u;
    
    if(CMD_FLAG6 & 0x01u)
    {  
        /* DIRECTION_UP */
      EscDataToControl[0][2] |=0x02u;
    }
    else
    {  /* DIRECTION_DOWN */
      EscDataToControl[0][2] |=0x01u;
    }  

    /* clear sf state */
    EscDataToControl[0][1] &= ~0XE0u;
    /* STATE_RUN */
    EscDataToControl[0][1] |= 0X20u;
    
    /* STOP */
    EscDataToControl[0][1] &= ~0X02u;
    
    /* SfBase_EscState = ESC_RUN_STATE; */
    
    /* ESC_MOTOR_DELTA */
    EscDataToControl[0][2] |= 0x80u;
  }  
} 

/*******************************************************************************
* Function Name  : Esc_Control_Stop
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Esc_Control_Stop(void) 
{
    /* ORDER_STOP */
    EscDataToControl[0][1] &= ~0x01u;
    
    /* DIRECTION_MASK  */
    EscDataToControl[0][2] &=~0x03u;
    
    /* clear sf state */
    EscDataToControl[0][1] &= ~0XE0u;
    
    /* SF_STATE_READY */
    EscDataToControl[0][1] |= 0X80u;
    
    /* STOP order*/
    EscDataToControl[0][1] |= 0X02u;
    
    /* ESC_MOTOR_DELTA */
     EscDataToControl[0][2] &= ~0x80u; 
}
#endif
/*******************************************************************************
* Function Name  : Esc_Control
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Esc_Control(void)
{
  /*
  Inspection_Normal_Key_Check();
  */ 
  Feedback_Contactors_Check();
  
  Floorplate_Check();
  
  Brake_Status_Check();
  
  Aux_Brake_CS();
  
  if( (!(CMD_FLAG5 & ESC_SAFETY_END_CLOSE)) || (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT)  )  /* || (!(CMD_FLAG3 &0x10))*/
  {
    CMD_ESC_RUN &= ~ESC_UP; 
    CMD_ESC_RUN &= ~ESC_DOWN;
      
  }  
      
  if ( CMD_ESC_RUN_MODE & ESC_NORMAL ) 
  {
      /* Normal mode */
      EscDataToControl[0][2] &= ~ESC_MODE1_MASK;
      EscDataToControl[0][2] |= ESC_MODE1_NORMAL;
  }
  else
  {
      /* INSPECTION mode */
      EscDataToControl[0][2] &= ~ESC_MODE1_MASK;
      EscDataToControl[0][2] |= ESC_MODE1_INSPECTION;      
  }
   
#if 0  
  if(SfBase_EscState == ESC_STARTING_PROCESS_STATE)
  {
    Esc_Control_Starting_Prpcess();
  } 
  else
  {
    Start_Process_State=0u;
    
    if((SfBase_EscState == ESC_RUN_STATE) && ((CMD_FLAG5&0x20u) || (!(CMD_FLAG5&0x10u))))
    {
      Esc_Control_Run();
    }
    else
    {
      Esc_Control_Stop();
    }  
  }
#endif 
}

/******************************  END OF FILE  *********************************/



