/*******************************************************************************
* File Name          : esc_interrupt.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 09/06/2016
* Description        : This file contains esc interrupt process function.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_interrupt.h"
#include "esc.h"
#include "esc_motor_speed.h"
#include "esc_handrail_speed.h"
#include "esc_missing_step.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : mtr_X1_int
* Description    : motor pulse counter x1 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void mtr_X1_int(void)
{   
    Measure_motor_between_pulse(&MTRITEM[0]);
    Motor_Speed_1_2_Shortcircuit_Run();
    
    MTRITEM[0].rt_pulse++; 			
    MTRITEM[0].rt_brake_pulse++; 
    HDL_Left.handrail_speed_rt_pulse++;
    
#ifdef GEC_SF_MASTER      
    STPMS_UPPER.MtrPulse++;
    STPMS_LOWER.MtrPulse++;
#endif
}

/*******************************************************************************
* Function Name  : mtr_X2_int
* Description    : motor pulse counter x2 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void mtr_X2_int(void)
{      
    Measure_motor_between_pulse(&MTRITEM[1]);
    Motor_Speed_1_2_Shortcircuit_Run();
    
    MTRITEM[1].rt_pulse++; 	
    MTRITEM[1].rt_brake_pulse++; 
    HDL_Right.handrail_speed_rt_pulse++;
    
#ifndef GEC_SF_MASTER  
    STPMS_UPPER.MtrPulse++;
    STPMS_LOWER.MtrPulse++;
#endif
}


/*******************************************************************************
* Function Name  : handrail_X1_int
* Description    : handrail pulse counter x1 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void handrail_X1_int(void)
{  
    Handrail_Speed_Right_Left_Shortcircuit_Run();
}


/*******************************************************************************
* Function Name  : handrail_X2_int
* Description    : handrail pulse counter x2 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void handrail_X2_int(void)
{  
    Handrail_Speed_Right_Left_Shortcircuit_Run();
}

/*******************************************************************************
* Function Name  : missingstep_X1_int
* Description    : missingstep pulse counter x1 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void missingstep_X1_int(void)
{    
    Missing_Step_UpperLower_Shortcircuit_Run();
}

/*******************************************************************************
* Function Name  : missingstep_X2_int
* Description    : missingstep pulse counter x2 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void missingstep_X2_int(void)
{     
    Missing_Step_UpperLower_Shortcircuit_Run();
}

/******************************  END OF FILE  *********************************/






