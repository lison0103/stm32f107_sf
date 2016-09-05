/*******************************************************************************
* File Name          : esc_interrupt.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
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
    MTRITEM[0].SensorPulse++;    
    MTRITEM[0].rt_pulse++; 			
    MTRITEM[0].rt_brake_pulse++; 
    MTRITEM[0].PulseCalCounter++;
    
#ifdef GEC_SF_MASTER      
    STPMS_UPPER.MtrPulse++;
    STPMS_LOWER.MtrPulse++;
#endif
    Motor_Speed_1_2_Shortcircuit_Run();
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
  
    MTRITEM[1].SensorPulse++;    
    MTRITEM[1].rt_pulse++; 	
    MTRITEM[1].rt_brake_pulse++; 
    MTRITEM[1].PulseCalCounter++;
    
#ifndef GEC_SF_MASTER  
    STPMS_UPPER.MtrPulse++;
    STPMS_LOWER.MtrPulse++;
#endif
    
    Motor_Speed_1_2_Shortcircuit_Run();
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

    HDL_Left.hr_pulse++;
    
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

    HDL_Right.hr_pulse++;
    
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
    
    STPMS_UPPER.rising_edge_detected[0] = 1u;
    STPMS_UPPER.rising_edge_detected[1] = 1u;     
    
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
    
    STPMS_LOWER.rising_edge_detected[0] = 1u;
    STPMS_LOWER.rising_edge_detected[1] = 1u;
    
    Missing_Step_UpperLower_Shortcircuit_Run();
}

/******************************  END OF FILE  *********************************/






