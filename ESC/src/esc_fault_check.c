
/*******************************************************************************
* File Name          : 
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

static void CheckUpDownInspectionKey(void);
static void FlyWheelProtect(void);
void Fault_Check(void) ;

/*******************************************************************************
* Function Name  : FlyWheelProtect
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void FlyWheelProtect(void)
{
  if(EscRtData. Cfg_Input_Mask[1] & 0x18u) /* be configured */
  {
    if( (EscRtData. Cfg_Input_Mask[1]&0x08u) && (!(EscRtData. Cfg_Input_Level[1]&0x08u)) )
    {
      EN_ERROR37 |= 0x08u;   
    }
    
    if( (EscRtData. Cfg_Input_Mask[1]&0x10u) && (!(EscRtData. Cfg_Input_Level[1]&0x10u)) )
    {
      EN_ERROR37 |= 0x10u;   
    }
  }   
}

/*******************************************************************************
* Function Name  : CheckUpDownInspectionKey
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void CheckUpDownInspectionKey(void)
{
  static u8 key_button_fault_tms=0u;
    
  if((CMD_FLAG3 & 0x10u) && (CMD_FLAG3 & 0x0Cu)) /* Normal mode, inspection up/down signal */
  {
    if(key_button_fault_tms>100u)
    {
      EN_ERROR50 |= 0x10u; 
    } 
    else
    {
      key_button_fault_tms += 5u;
    }  
  } 
  else if((!(CMD_FLAG3 & 0x10u)) && (CMD_FLAG3 & 0x03u)) /* Inspection mode, key up/down signal */
  {
    if(key_button_fault_tms>100u)
    {
      EN_ERROR46 |= 0x40u; 
    } 
    else
    {
      key_button_fault_tms += 5u;
    }  
  }
  else if( ((CMD_FLAG3 & 0x03u)==0x03u) || ((CMD_FLAG3 & 0x0cu)==0x0cu) )
  {
    if(key_button_fault_tms>100u)
    {
      EN_ERROR46 |= 0x40u;  /* temp fault code */
    } 
    else
    {
      key_button_fault_tms += 5u;
    }  
  }  
  else
  {
    key_button_fault_tms = 0u;    
  }  
}

/*******************************************************************************
* Function Name  : CheckUpDownInspectionKey
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Fault_Check(void) 
{
  Safety_String_Check();
  
  CheckUpDownInspectionKey();
  
  FlyWheelProtect();
}

/******************************  END OF FILE  *********************************/
