/*******************************************************************************
* File Name          : esc_interrupt.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 12/23/2016
* Description        : This file contains esc interrupt process function.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_interrupt.h"
#include "esc.h"
#include "esc_motor_speed.h"
#include "esc_handrail_speed.h"
#include "esc_missing_step.h"
#include "esc_main_shaft_speed.h"

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
    Measure_between_pulse_Time(&MTRITEM[0]);
    if( g_u8SensorShortCircuitCheck == 1u )
    {
        Motor_Speed_1_2_Shortcircuit_Run();
    }
    /*Motor_Speed_Direction_Check(&MTRITEM[0]);*/
    
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
    if(( SPEED_SENSOR_INSTALLATION == 0u ) || ( SPEED_SENSOR_INSTALLATION == 3u )) 
    {
        Measure_between_pulse_Time(&MTRITEM[1]);
        if( g_u8SensorShortCircuitCheck == 1u )
        {
            Motor_Speed_1_2_Shortcircuit_Run();
        }
        /*Motor_Speed_Direction_Check(&MTRITEM[1]);*/
        
        MTRITEM[1].rt_pulse++; 	
        MTRITEM[1].rt_brake_pulse++; 
        HDL_Right.handrail_speed_rt_pulse++;
        
#ifndef GEC_SF_MASTER  
        STPMS_UPPER.MtrPulse++;
        STPMS_LOWER.MtrPulse++;
#endif
    }
}

/*******************************************************************************
* Function Name  : mainshaft_X1_int
* Description    : mainshaft pulse counter x1 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void mainshaft_X1_int(void)
{   
    Measure_between_pulse_Time(&MAINSHAFTITEM[0]);
    if( SPEED_SENSOR_INSTALLATION == 1u )
    {
        if( g_u8SensorShortCircuitCheck == 1u )
        {
            MainShaft_Speed_Shortcircuit_Run();
        }
    }
    else if( SPEED_SENSOR_INSTALLATION == 2u )
    {
        /* check main shaft 1-2 */
        if( g_u8SensorShortCircuitCheck == 1u )
        {
            MainShaft_Speed_Shortcircuit_Run();
        }
        /* check main shaft 1-3 */
        else if( g_u8SensorShortCircuitCheck == 2u )
        {
            MainShaft_Speed_Shortcircuit_Run();                     
        }
        /* check main shaft 2-3 */
        else if( g_u8SensorShortCircuitCheck == 3u )
        {
                       
        }
        else
        {}
    }
    else if( SPEED_SENSOR_INSTALLATION == 3u )
    {    
        if( g_u8SensorShortCircuitCheck == 2u )
        {
            MainShaft_Speed_Shortcircuit_Run();                     
        }        
    }
    else
    {}

    /*MainShaftWrongDirection(&MAINSHAFTITEM[0]);*/
    MAINSHAFTITEM[0].rt_pulse++;     
    
    if(( SPEED_SENSOR_INSTALLATION == 1u ) || ( SPEED_SENSOR_INSTALLATION == 2u ))
    {			
        MAINSHAFTITEM[0].rt_brake_pulse++; 
        HDL_Left.handrail_speed_rt_pulse++;
        
#ifdef GEC_SF_MASTER      
        STPMS_UPPER.MtrPulse++;
        STPMS_LOWER.MtrPulse++;
#endif
    }
}

/*******************************************************************************
* Function Name  : mainshaft_X2_int
* Description    : mainshaft pulse counter x2 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void mainshaft_X2_int(void)
{      
    Measure_between_pulse_Time(&MAINSHAFTITEM[1]);
    if( SPEED_SENSOR_INSTALLATION == 1u )
    {
        if( g_u8SensorShortCircuitCheck == 1u )
        {
            MainShaft_Speed_Shortcircuit_Run();
        }
    }
    else if( SPEED_SENSOR_INSTALLATION == 2u )
    {
        /* check main shaft 1-2 */
        if( g_u8SensorShortCircuitCheck == 1u )
        {
            MainShaft_Speed_Shortcircuit_Run();
        }
        /* check main shaft 1-3 */
        else if( g_u8SensorShortCircuitCheck == 2u )
        {
                                
        }
        /* check main shaft 2-3 */
        else if( g_u8SensorShortCircuitCheck == 3u )
        {
            MainShaft_Speed_Shortcircuit_Run();             
        }
        else
        {}
    }
    else if( SPEED_SENSOR_INSTALLATION == 3u )
    {    
        if( g_u8SensorShortCircuitCheck == 2u )
        {
            MainShaft_Speed_Shortcircuit_Run();                     
        }        
    }
    else
    {}
 
    /*MainShaftWrongDirection(&MAINSHAFTITEM[1]);*/
    MAINSHAFTITEM[1].rt_pulse++; 	
    
    if(( SPEED_SENSOR_INSTALLATION == 1u ) || ( SPEED_SENSOR_INSTALLATION == 2u ))
    {
        MAINSHAFTITEM[1].rt_brake_pulse++; 
        HDL_Right.handrail_speed_rt_pulse++;
        
#ifndef GEC_SF_MASTER  
        STPMS_UPPER.MtrPulse++;
        STPMS_LOWER.MtrPulse++;
#endif
    }
}

/*******************************************************************************
* Function Name  : mainshaft_X3_int
* Description    : mainshaft pulse counter x3 interrupt.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void mainshaft_X3_int(void)
{      
    if( SPEED_SENSOR_INSTALLATION == 2u )
    {
        Measure_between_pulse_Time(&MAINSHAFTITEM[2]);
        if( SPEED_SENSOR_INSTALLATION == 2u )
        {
            /* check main shaft 1-2 */
            if( g_u8SensorShortCircuitCheck == 1u )
            {
                
            }
            /* check main shaft 1-3 */
            else if( g_u8SensorShortCircuitCheck == 2u )
            {
                MainShaft_Speed_Shortcircuit_Run();
            }
            /* check main shaft 2-3 */
            else if( g_u8SensorShortCircuitCheck == 3u )
            {
                MainShaft_Speed_Shortcircuit_Run();             
            }
            else
            {}
        }
        
        MAINSHAFTITEM[2].rt_pulse++; 	
    }
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
    if( g_u8SensorShortCircuitCheck == 4u )
    {    
        Handrail_Speed_Right_Left_Shortcircuit_Run();
    }
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
    if( g_u8SensorShortCircuitCheck == 4u )
    {    
        Handrail_Speed_Right_Left_Shortcircuit_Run();
    }
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
    if( g_u8SensorShortCircuitCheck == 5u )
    {       
        Missing_Step_UpperLower_Shortcircuit_Run();
    }
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
    if( g_u8SensorShortCircuitCheck == 5u )
    {       
        Missing_Step_UpperLower_Shortcircuit_Run();
    }
}

/******************************  END OF FILE  *********************************/






