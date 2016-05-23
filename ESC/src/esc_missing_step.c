/*******************************************************************************
* File Name          : esc_missing_step.c
* Author             : lison
* Version            : V1.0
* Date               : 05/10/2016
* Description        : This file contains esc missing step check.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_missing_step.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 Motor_speed_pulse_counter = 0;


/*******************************************************************************
* Function Name  : Missing_Step_Ready
* Description    : Check the escalator missing step ready.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Missing_Step_Ready(void)
{

}

/*******************************************************************************
* Function Name  : Missing_StepRun
* Description    : Check the escalator missing step run.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Missing_StepRun(void)
{
    static u16 escState_old = 0; 
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {
        if( missing_step_pluse_freq > 1 )
        {
            /* fault */
        }
    }	
    
    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        Motor_speed_pulse_counter = 0;
    }
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {       
        Motor_speed_pulse_counter =  Pulse_counter_sensor_speed1() - Motor_speed_pulse_counter;
        
        if( Motor_speed_pulse_counter > R*1.1 )
        {
            if( SfBase_EscState & ESC_STATE_INSP )
            {     
                /* Missing step warning */
                Waring_Missing_Step++;
            }
            else
            {
                /* Missing step fault */
                Fault_Missing_Step++;
            }
        }
        else
        {
            if( missing_step_signal_check() )
            {
                Motor_speed_pulse_counter = 0;
                
                Motor_speed_pulse_counter =  Pulse_counter_sensor_speed1() - Motor_speed_pulse_counter;
                
                if( Motor_speed_pulse_counter > R*1.1 )
                {
                    if( SfBase_EscState & ESC_STATE_INSP )
                    {     
                        /* Missing step warning */
                        Waring_Missing_Step++;
                    }
                    else
                    {
                        /* Missing step fault */
                        Fault_Missing_Step++;
                    }
                }
                else
                {
                    if( missing_step_signal_check() )
                    {
                        if( Motor_speed_pulse_counter < R*0.9 )
                        {
                            if( SfBase_EscState & ESC_STATE_INSP )
                            {     
                                /* Missing step warning */
                                Waring_Missing_Step++;
                            }
                            else
                            {
                                /* Missing step fault */
                                Fault_Missing_Step++;
                            }
                        } 
                        else
                        {
                            Motor_speed_signal_first_edge_detected = 1;
                        }
                    }
                }
            }
        }
 
    }
    
    if( Fault_Missing_Step )
    {
    
    }
    else if ( Waring_Missing_Step )
    {
        
    }
    else
    {
        Fault_Missing_Step = 0;
        Waring_Missing_Step = 0;
    }
    
    
    escState_old = SfBase_EscState; 
    
    
}

/*******************************************************************************
* Function Name  : Pulse_counter_sensor_speed
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Pulse_counter_sensor_speed(Sensor)
{

    return current_pulse_counter_sensor;
}


/******************************  END OF FILE  *********************************/






