/*******************************************************************************
* File Name          : esc_motor_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Description        : This file contains esc motor speed.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_motor_speed.h"
#include "delay.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define F1      ( ( MOTOR_RPM * MOTOR_PLUSE_PER_REV ) / 60 )
#define MAX_SPEED       ( F1 * 1.15 )
#define MIN_SPEED       ( F1 * 0.1 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
u32 Measure_motor_speed(MTRFREQITEM* ptMTR);
 

/* parameters */ 
u16 MOTOR_RPM = 1000;
u16 MOTOR_PLUSE_PER_REV = 10;
u16 UNDERSPEED_TIME = 0;
u16 DELAY_NO_PULSE_CHECKING = 0;

/* variable */
u32 Time_running_tms = 0;
u16 SfBase_EscState = 0;
u8 EscRTBuff[200]; 

MTRFREQITEM MTRITEM[2]=
{
  {
    0,0,0,0,
    
    0,//实时飞轮频率计数   
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //脉冲周期数组 
    
    0,0,//制动距离计数 
   
    (u16*)&EscRTBuff[40],  //频率值
    (u16*)&EscRTBuff[48],  //制动距离
    
    0,
    {0,0,0,0,0,0,0,0,0,0,0,0},
		0
  },
  {
    0,0,0,0,
    
    0,//实时飞轮频率计数   
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //脉冲周期数组
    
    0,0,//制动距离计数 
   
    (u16*)&EscRTBuff[42],  //频率值
    (u16*)&EscRTBuff[50],  //制动距离
    
    0,
    {0,0,0,0,0,0,0,0,0,0,0,0},
		0
  }
};



/*******************************************************************************
* Function Name  : Motor_Speed_Ready
* Description    : Check the escalator motor speed ready.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Motor_Speed_Ready(MTRFREQITEM* ptMTR)
{
    static u16 escState_old = 0; 
    u16 Escalator_speed = 0;
    static u16 delay_no_pulse_tms = 0;
    
    if( ( SfBase_EscState & ESC_STATE_READY ) && ( !(escState_old & ESC_STATE_READY) ) )
    {
        
    }
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {
        
        if( ( delay_no_pulse_tms * SYSTEMTICK ) > DELAY_NO_PULSE_CHECKING )
        {
            Escalator_speed = Measure_motor_speed(ptMTR);
            
            if( Escalator_speed > 1 )
            {
                /* fault */
            }            
        }
        else
        {
            delay_no_pulse_tms++;
        }
        

    } 
}


/*******************************************************************************
* Function Name  : Motor_Speed_Ready
* Description    : Check the escalator motor speed run.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Motor_Speed_Run_EN115(MTRFREQITEM* ptMTR)
{
    static u16 escState_old = 0; 
    u16 Escalator_speed = 0;
    	
    
    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        Time_running_tms = 0;
    } 

    if( SfBase_EscState & ESC_STATE_RUNNING )
    {      
        
        if( ( Time_running_tms * SYSTEMTICK ) > UNDERSPEED_TIME )
        {
            SfBase_EscState |= ESC_STATE_RUN5S;                
        }
        else
        {
            Time_running_tms++;
        }        
        
        Escalator_speed = Measure_motor_speed(ptMTR);
        
        if( Escalator_speed >= MAX_SPEED )
        {
            /* overspeed fault */
        }
        else
        {

            if( SfBase_EscState & ESC_STATE_RUN5S )
            {
                if( Escalator_speed <= MIN_SPEED )
                {
                    /* underspeed fault */
                }
            }
        }

    }
    escState_old = SfBase_EscState;
}

/*******************************************************************************
* Function Name  : Measure_motor_speed
* Description    : Measure the escalator motor speed.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
u32 Measure_motor_speed(MTRFREQITEM* ptMTR)
{
    u32 current_motor_speed_sensor = 0;

    return current_motor_speed_sensor;
}


/*******************************************************************************
* Function Name  : Check_Stopping_Distance
* Description    : Check the escalator Stopping Distance.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
//u32 Check_Stopping_Distance(Sensor)
//{
//    
//    if( SfBase_EscState & ESC_STATE_STOP )
//    {
//        if( Motor_speed_signal == low_state )
//        {
//            timer_between_pulses = 0;
//                
//            Motor_speed_HR_counter = Motor_speed_HR_counter + 1;
//            
//            Stopping_distance = Motor_speed_HR_counter * CONVERSION;
//            
//            store_to_fram(Stopping_distance);
//            
//            if( Stopping_distance > Max_distance ) 
//            {
//                /* stopping distance fault */
//            }
//            else
//            {
//                if( timer_between_pulses > 1s )
//                {
//                    return;
//                } 
//            }
//        }
//        else
//        {
//            if( timer_between_pulses > 1s )
//            {
//                return;
//            }
//        }
//        
//    }
//    else
//    {
//      
//    }
//
//}


/******************************  END OF FILE  *********************************/






