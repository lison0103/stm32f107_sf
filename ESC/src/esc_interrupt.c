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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : mtr_X1_int
* Description    : motor pulse counter x1.
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
//    HDL_Left.MtrPulse++;
//    HDL_Right.MtrPulse++;
    
//    STPMS_UPPER.MtrPulse++;
//    STPMS_LOWER.MtrPulse++;
#endif
  
}

/*******************************************************************************
* Function Name  : mtr_X2_int
* Description    : motor pulse counter x2.
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
//    HDL_Left.MtrPulse++;
//    HDL_Right.MtrPulse++;
    
//    STPMS_UPPER.MtrPulse++;
//    STPMS_LOWER.MtrPulse++;
#endif
    
 
}


/*******************************************************************************
* Function Name  : handrail_X1_int
* Description    : handrail pulse counter x1.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void handrail_X1_int(void)
{  

    HDL_Left.hr_pulse++;
    
}


/*******************************************************************************
* Function Name  : handrail_X2_int
* Description    : handrail pulse counter x2.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void handrail_X2_int(void)
{  

    HDL_Right.hr_pulse++;
    
}



/******************************  END OF FILE  *********************************/






