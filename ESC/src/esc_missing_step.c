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
#include "timer.h"

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
//void Missing_Step_Ready(void)
//{  
//    u16 Escalator_handrail_speed = 0;       
//    
//    if( SfBase_EscState & ESC_STATE_READY ) 
//    {                
////        Escalator_handrail_speed = Measure_handrail_speed(psHDL);
//        
//        if( Escalator_handrail_speed > 1 )
//        {
//            /* fault */
//        }            
//         
//    } 
//
//}
//
///*******************************************************************************
//* Function Name  : Missing_StepRun
//* Description    : Check the escalator missing step run.
//* Input          : None          
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void Missing_StepRun(void)
//{
//    static u16 escState_old = 0; 
//    
//    if( SfBase_EscState & ESC_STATE_READY ) 
//    {
//        if( missing_step_pluse_freq > 1 )
//        {
//            /* fault */
//        }
//    }	
//    
//    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
//    {
//        Motor_speed_pulse_counter = 0;
//    }
//    
//    if( SfBase_EscState & ESC_STATE_RUNNING )
//    {       
//        Motor_speed_pulse_counter =  Pulse_counter_sensor_speed1() - Motor_speed_pulse_counter;
//        
//        if( Motor_speed_pulse_counter > R*1.1 )
//        {
//            if( SfBase_EscState & ESC_STATE_INSP )
//            {     
//                /* Missing step warning */
//                Waring_Missing_Step++;
//            }
//            else
//            {
//                /* Missing step fault */
//                Fault_Missing_Step++;
//            }
//        }
//        else
//        {
//            if( missing_step_signal_check() )
//            {
//                Motor_speed_pulse_counter = 0;
//                
//                Motor_speed_pulse_counter =  Pulse_counter_sensor_speed1() - Motor_speed_pulse_counter;
//                
//                if( Motor_speed_pulse_counter > R*1.1 )
//                {
//                    if( SfBase_EscState & ESC_STATE_INSP )
//                    {     
//                        /* Missing step warning */
//                        Waring_Missing_Step++;
//                    }
//                    else
//                    {
//                        /* Missing step fault */
//                        Fault_Missing_Step++;
//                    }
//                }
//                else
//                {
//                    if( missing_step_signal_check() )
//                    {
//                        if( Motor_speed_pulse_counter < R*0.9 )
//                        {
//                            if( SfBase_EscState & ESC_STATE_INSP )
//                            {     
//                                /* Missing step warning */
//                                Waring_Missing_Step++;
//                            }
//                            else
//                            {
//                                /* Missing step fault */
//                                Fault_Missing_Step++;
//                            }
//                        } 
//                        else
//                        {
//                            Motor_speed_signal_first_edge_detected = 1;
//                        }
//                    }
//                }
//            }
//        }
// 
//    }
//    
//    if( Fault_Missing_Step )
//    {
//    
//    }
//    else if ( Waring_Missing_Step )
//    {
//        
//    }
//    else
//    {
//        Fault_Missing_Step = 0;
//        Waring_Missing_Step = 0;
//    }
//    
//    
//    escState_old = SfBase_EscState; 
//    
//    
//}

/*******************************************************************************
* Function Name  : Pulse_counter_sensor_speed
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
u16 Pulse_counter_sensor_speed(void)
{
    u16 current_pulse_counter_sensor = 0;
    
    
    return current_pulse_counter_sensor;
}


/*******************************************************************************
* Function Name  : Missing_Step_UpperLower_SyncRun
* Description    : Missing step upper lower syncronization checking
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Missing_Step_UpperLower_SyncRun(void)
{

}

/*******************************************************************************
* Function Name  : Missing_Step_UpperLower_Shortcircuit_Run
* Description    : Missing step upper lower shortcircuit checking
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Missing_Step_UpperLower_Shortcircuit_Run(void)
{
    static u16 First_MS_edge_detected,Timer_MS_shortcircuit = 0;
    
    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        First_MS_edge_detected = 0;
    } 
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_MS_edge_detected == 0 )
        {
            First_MS_edge_detected = 1;
            Timer_MS_shortcircuit = 0;
            TIM7_Int_Init(65535,71);
            
        }    
        else
        {
            Timer_MS_shortcircuit = TIM_GetCounter(TIM7);
            if( Timer_MS_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_MS_shortcircuit = 0;               
                TIM_SetCounter(TIM7,0); 
                
                /* Fault ¨C Motorspeed Sensor shortcircuited */
                EscRTBuff[68] = 1;
            }
            else
            {
                Timer_MS_shortcircuit = 0;             
                TIM_SetCounter(TIM7,0);     
                
                EscRTBuff[68] = 0;
            }
        }
    }    
}

/******************************  END OF FILE  *********************************/






