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
static u16 Brake_Error_Tms[8]={0,0,0,0,0,0,0,0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Brake_Status_Ready(void);
static void Brake_Status_Run(void);



/*******************************************************************************
* Function Name  : Brake_Status_Ready
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Brake_Status_Ready(void)
{
  u8 i;
  
  for(i=0u;i<Brake_Number;i++)  
  {
    if( Brake_Input & (1u<<i) )
    {
      Brake_Error_Tms[i] = 0u;
    } 
    else
    {
        if( Brake_Error_Tms[i]<1000u)  { Brake_Error_Tms[i]++; }
    }  
    
    if( Brake_Error_Tms[i]>300u)
    {
      EN_ERROR29 |= 0x01u; 
    } 
  }
}

/*******************************************************************************
* Function Name  : Brake_Status_Run
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Brake_Status_Run(void)
{
  u8 i;
  
  for(i=0u;i<Brake_Number;i++)  
  {
    if( !(Brake_Input & (1u<<i)) )
    {
      Brake_Error_Tms[i] = 0u;
    } 
    else
    {
        if( Brake_Error_Tms[i]<1000u)  { Brake_Error_Tms[i]++; }
    }  
    
    if( Brake_Error_Tms[i]>300u)
    {
      EN_ERROR29 |= 0x01u; 
    } 
  }
}

/*******************************************************************************
* Function Name  : Brake_Status_Check
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Brake_Status_Check(void)
{
  Brake_Number=2u;
  Brake_Mask=0x03u;
  Brake_Input=0u; 
  
  if(INPUT_PORT9_16 & INPUT_PORT13_MASK) { Brake_Input |= 0x01u; }
  if(INPUT_PORT9_16 & INPUT_PORT14_MASK) { Brake_Input |= 0x02u; }
                                            
  if( SfBase_EscState == ESC_RUN_STATE ) 
  {
    Brake_Status_Run();
  } 
  else
  {
    Brake_Status_Ready();
  }  
}

/******************************  END OF FILE  *********************************/