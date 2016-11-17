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

u8 Start_Process_State=0u;

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
  

  if( CMD_ESC_RUN_MODE & ESC_INSPECT )
  {
    g_u8SafetyRelayStartCheck = 1u;    
    /* return; */
  }
  else
  {
      switch(Start_Process_State)
      {
         case 0u:
          
          g_u8SafetyRelayStartCheck = 0u;
          delay_cnt = 0u;
          Start_Process_State = 1u;
          
          break;
         case 1u:
          
          SafetyOutputDisable();
          EscDataToControl[0][1] = 0x50u;
          
          if(delay_cnt++>100u)
          {
              Start_Process_State = 2u;
              delay_cnt = 0u;
          }  
          break;
          
         case 2u:
          
          SafetyOutputDisable();
          EscDataToControl[0][1] = 0x40u;
          
          if(delay_cnt++>20u)
          {
              Start_Process_State = 3u;
              delay_cnt = 0u;
          }  
          break;
          
         case 3u:      
          
          SafetyOutputEnable();
          EscDataToControl[0][1] = 0x40u;
          
          if(delay_cnt++>100u)
          {
              Start_Process_State = 4u;
              delay_cnt = 0u;
          }         
          break;   
          
         case 4u:
          
          /*SfBase_EscState = ESC_RUN_STATE;*/
          g_u8SafetyRelayStartCheck = 1u;
          
          break;
         default:
          /*  Fault  */
          break;      
      }  
  }
}

/*******************************************************************************
* Function Name  : Esc_Control_Run
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Esc_Control_Run(void)
{
  /* Normal */
  EscDataToControl[0][2] |=0x08u; 
  
  if(CMD_FLAG6 & 0x03u)
  {
    EscDataToControl[0][1] |= 0x01u;
    
    if(CMD_FLAG6 & 0x01u)
    {  
      EscDataToControl[0][2] |=0x02u;
    }
    else
    {  
      EscDataToControl[0][2] |=0x01u;
    }  

    EscDataToControl[0][1] &= ~0XE0u;
    EscDataToControl[0][1] |= 0X20u;
    
    /* STOP */
    EscDataToControl[0][1] &= ~0X02u;
    
    /* SfBase_EscState = ESC_RUN_STATE; */
    
    /* Start */
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
    EscDataToControl[0][1] &= ~0x01u;
    EscDataToControl[0][2] &=~0x03u;
    
    EscDataToControl[0][1] &= ~0XE0u;
    EscDataToControl[0][1] |= 0X80u;
    
    /* STOP */
    EscDataToControl[0][1] |= 0X02u;
    
    /* Start */
    EscDataToControl[0][2] &= ~0x80u;  
}

/*******************************************************************************
* Function Name  : Esc_Control
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Esc_Control(void)
{
  Inspection_Normal_Key_Check();
   
  Feedback_Contactors_Check();
  
  Floorplate_Open();
  
  Brake_Status_Check();
  
  Aux_Brake_CS();

  
  if( (!(CMD_FLAG5 & ESC_SAFETY_END_CLOSE)) || (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT)  )  /* || (!(CMD_FLAG3 &0x10))*/
  {
    CMD_ESC_RUN &= ~ESC_UP; 
    CMD_ESC_RUN &= ~ESC_DOWN;
      
  }  
      
   
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

}

/******************************  END OF FILE  *********************************/



