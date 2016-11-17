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
static u8 Contact_Number=2u,Contact_Mask=0x03u,Contact_Input=0u; 
static u16 Contact_Error_Tms[8]={0,0,0,0,0,0,0,0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Feedback_Contactors_Ready(void); 
static void Feedback_Contactors_Run(void); 



/*******************************************************************************
* Function Name  : Feedback_Contactors_Ready
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Feedback_Contactors_Ready(void)
{
  u8 i;
  
  for(i=0u;i<Contact_Number;i++)  
  {
    if( Contact_Input & (1u<<i) )
    {
        if( Contact_Error_Tms[i]<1000u)  { Contact_Error_Tms[i]++; }
    } 
    else
    {
      Contact_Error_Tms[i] = 0u; 
    }  
    
    if( Contact_Error_Tms[i]>200u )
    {
      EN_ERROR29 |= 0x02u; 
    } 
    else if( (SfBase_EscState == ESC_STARTING_PROCESS_STATE) && ( Contact_Error_Tms[i]>5u ))
    {
      EN_ERROR29 |= 0x02u; 
    }  
    else
    {}
  }
}

/*******************************************************************************
* Function Name  : Feedback_Contactors_Run
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Feedback_Contactors_Run(void)
{
  u8 i;
  
  for(i=0u;i<Contact_Number;i++)  
  {
    if( (Contact_Input & (1u<<i)) )
    {
      Contact_Error_Tms[i] = 0u;
    } 
    else
    {
        if( Contact_Error_Tms[i]<1000u) { Contact_Error_Tms[i]++; }
    }  
    
    if( Contact_Error_Tms[i]>200u)
    {
      EN_ERROR29 |= 0x02u; 
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
  Contact_Number=2u;
  Contact_Mask=0x03u;
  Contact_Input=0u; 
  
  if(INPUT_PORT9_16 & INPUT_PORT11_MASK) { Contact_Input |= 0x01u; }
  if(INPUT_PORT9_16 & INPUT_PORT12_MASK) { Contact_Input |= 0x02u; }
 
  if( SfBase_EscState == ESC_RUN_STATE )
  {
    Feedback_Contactors_Run();
  }  
  else
  {
    Feedback_Contactors_Ready();
  } 

}

/******************************  END OF FILE  *********************************/