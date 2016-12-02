/*******************************************************************************
* File Name          : esc_coverplate.c
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
#define FLOOR_PLATE_U_INPUT1   (EscRtData. Cfg_Input_Level[0] &(0x80u))
#define FLOOR_PLATE_U_INPUT2   (EscRtData. Cfg_Input_Level[1] &(0x01u))

#define FLOOR_PLATE_L_INPUT1   (EscRtData. Cfg_Input_Level[1] &(0x02u))
#define FLOOR_PLATE_L_INPUT2   (EscRtData. Cfg_Input_Level[1] &(0x04u))

#define CP_INPUT_OPEN           0x00u
#define CP_INPUT_CLOSE          0x03u
#define CP_INPUT_FAULT1         0x01u
#define CP_INPUT_FAULT2         0x02u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Floorplate_Check
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Floorplate_Check(void)
{
  u8 i=0u,cp_input[2]={0,0};
  static u16 input_sensor_fault_counter[2]={0,0};
  
  /*
  *  cp_input[0].0 Upper coverplate input1
  *  cp_input[0].1 Upper coverplate input2
  *  cp_input[1].0 Lower coverplate input1
  *  cp_input[1].1 Lower coverplate input2
  */
  if(FLOOR_PLATE_U_INPUT1) { cp_input[0] |= 0x01u; }
  if(FLOOR_PLATE_U_INPUT2) { cp_input[0] |= 0x02u; }
  
  if(FLOOR_PLATE_L_INPUT1) { cp_input[1] |= 0x01u; }
  if(FLOOR_PLATE_L_INPUT2) { cp_input[1] |= 0x02u; }
  
  for(i=0u;i<2u;i++)
  {
    if( (cp_input[i] == CP_INPUT_FAULT1)   ||   (cp_input[i] == CP_INPUT_FAULT2) )
    {
      if(input_sensor_fault_counter[i]>100u) /* 500ms delay*/
      {
        EN_ERROR5 |= (1u<<i);
      } 
      else  
      { 
        input_sensor_fault_counter[i]++; 
      }
    }  
    else
    {
      input_sensor_fault_counter[i] = 0u;
    }   
  }  
  
  if( ISP_NORMAL_INPUT )  /* ISP_NORMAL input pin is high( to be in normal mode) */
  {
    if((cp_input[0] != CP_INPUT_CLOSE))
    {
      EN_ERROR5 |= 0x01u;   /* Upper floorplate open */
    }  
    
    if((cp_input[1] != CP_INPUT_CLOSE))
    {
      EN_ERROR5 |= 0x02u;  /* Lower floorplate open */
    }  
  }  
  else  /* to be inspection mode */
  {
    if((cp_input[0] == CP_INPUT_CLOSE) && (cp_input[1] == CP_INPUT_CLOSE))
    {
      EN_ERROR50 |= 0x08u;   /* At least should be open one floorplate */
    }  
  }
}

/******************************  END OF FILE  *********************************/