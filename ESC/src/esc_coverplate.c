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
#define FLOOR_PLATE_U_INPUT1   (EscRtData. SafetyInputData[2] & 0x40u)
#define FLOOR_PLATE_U_INPUT2   (EscRtData. SafetyInputData[2] & 0x80u)

#define FLOOR_PLATE_L_INPUT1   (EscRtData. SafetyInputData[3] & 0x01u)
#define FLOOR_PLATE_L_INPUT2   (EscRtData. SafetyInputData[3] & 0x02u)

#define CP_INPUT_OPEN           0x02u
#define CP_INPUT_CLOSE          0x01u
#define CP_INPUT_FAULT1         0x00u
#define CP_INPUT_FAULT2         0x03u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Floorplate_Open
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Floorplate_Open(void)
{
  u8 i=0u,cp_input[2]={0,0};
  static u16 input_sensor_fault_counter[2]={0,0},input_sensor_ok_counter[2]={0,0};
  static u16 cp_close_counter[2]={0,0},cp_open_counter[2]={0,0};
  
  if(FLOOR_PLATE_U_INPUT1) { cp_input[0] |= 0x01u; }
  if(FLOOR_PLATE_U_INPUT2) {cp_input[0] |= 0x02u;}
  
  if(FLOOR_PLATE_L_INPUT1) {cp_input[1] |= 0x01u;}
  if(FLOOR_PLATE_L_INPUT2) {cp_input[1] |= 0x02u;}
  
  for(i=0u;i<2u;i++)
  {
    if( (cp_input[i] == CP_INPUT_FAULT1)   ||   (cp_input[i] == CP_INPUT_FAULT2) )
    {
      input_sensor_ok_counter[i] = 0u;
      if(input_sensor_fault_counter[i]<1000u) { input_sensor_fault_counter[i]++; }
    }  
    else
    {
      input_sensor_fault_counter[i] = 0u;
      if(input_sensor_ok_counter[i]<1000u) { input_sensor_ok_counter[i]++; }
    }  
  }  
   
  if(input_sensor_ok_counter[0]>200u)  /* Upper plate sensor no fault */
  {
     EN_ERROR30 &= ~0x01u; 
  }  
  else if(input_sensor_fault_counter[0]>200u) /* Upper plate sensor fault */
  {
    EN_ERROR30 |= 0x01u; 
  }  
  else
  {}
    
  if(input_sensor_ok_counter[1]>200u)  /* Lower plate sensor no fault */
  {
     EN_ERROR30 &= ~0x02u; 
  }  
  else if(input_sensor_fault_counter[1]>200u)  /* Lower plate sensor fault */
  {
    EN_ERROR30 |= 0x02u; 
  }    
  else       
  {}
  
  /* 
  
  * OPEN/ CLOSE check 
  *
  *
  */
  for(i=0u;i<2u;i++)
  {
    if(cp_input[i] == CP_INPUT_CLOSE)
    {
      cp_open_counter[i] = 0u;
      if(cp_close_counter[i]<1000u) {cp_close_counter[i]++; }
    }  
    else /* if(cp_input[i] == CP_INPUT_OPEN) */
    {
      cp_close_counter[i] = 0u;
      if(cp_open_counter[i]<1000u) {cp_open_counter[i]++; }
    }  
  } 
  
  if(CMD_FLAG3 & 0x10u)  /* Inspection running */
  {
    /* No plate opened */
    
    if((cp_open_counter[0]>200u) || (cp_open_counter[1]>200u))
    {
       EN_ERROR30 &= ~0x10u; 
    }  
    else if((cp_close_counter[0]>200u) && (cp_close_counter[1]>200u))
    {
      EN_ERROR30 |= 0x10u; 
    }  
    else
    {}
    
    EN_ERROR30 &= ~0x0cu;  
  }  
  else /* Normal running */
  {
    if(cp_close_counter[0]>200u)  /* Upper plate sensor no fault */
    {
     EN_ERROR30 &= ~0x04u;  
    }  
    else if(cp_open_counter[0]>200u) /* Upper plate sensor fault */
    {
       EN_ERROR30 |= 0x04u;
    }  
    else
    {}
      
    if(cp_close_counter[1]>200u)  /* Lower plate sensor no fault */
    {
      EN_ERROR30 &= ~0x08u;
    }  
    else if(cp_open_counter[1]>200u)  /* Lower plate sensor fault */
    {
      EN_ERROR30 |= 0x08u;  
    }
    else
    {}
  }
}

/******************************  END OF FILE  *********************************/