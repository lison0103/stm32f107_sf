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
#include "timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* motor speed */
#define MAX_SPEED       ( ( F1 * 115 ) / 100 )
#define MIN_SPEED       ( ( F1 * 10 ) / 100 )

/* stopping distance */
#define CONVERSION      ( ( NOMINAL_SPEED / MOTOR_RPM ) * MOTOR_PLUSE_PER_REV )
#define MAX_DISTANCE    ( ( 12 * 2 * F1 ) / 10 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
u16 Measure_motor_speed(MTRFREQITEM* ptMTR);
 

/* parameters */ 
u16 MOTOR_RPM = 1000;
u16 MOTOR_PLUSE_PER_REV = 10;
u16 UNDERSPEED_TIME = 5000;
u16 DELAY_NO_PULSE_CHECKING = 1500;
u16 NOMINAL_SPEED = 500;
u16 SSM_SHORTCIRCUIT_TIME = 750;

/* variable */
u32 time_running_tms = 0;
u16 SfBase_EscState = 0;
u16 escState_old = 0;
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
		0,
                0,0,0
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
		0,
                0,0,0
  }
};



/*******************************************************************************
* Function Name  : Motor_Speed_Ready
* Description    : Pulse detection when escalator is stopped.
* Input          : ptMTR: motor speed sensor id          
* Output         : None
* Return         : None
*******************************************************************************/
void Motor_Speed_Ready(MTRFREQITEM* ptMTR)
{

    u16 Escalator_speed = 0;
    
    if( ( SfBase_EscState & ESC_STATE_READY ) && ( !(escState_old & ESC_STATE_READY) ) )
    {
        ptMTR->delay_no_pulse_tms = 0;
    }
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {
        
        if( ( ptMTR->delay_no_pulse_tms * SYSTEMTICK ) > DELAY_NO_PULSE_CHECKING )
        {
            Escalator_speed = Measure_motor_speed(ptMTR);
            
            if( Escalator_speed > 1 )
            {
                /* fault */
            }            
        }
        else
        {
            ptMTR->delay_no_pulse_tms++;
        }
        

    } 
    
    escState_old = SfBase_EscState;
}


/*******************************************************************************
* Function Name  : Motor_Speed_Run_EN115
* Description    : Overspeed and underspeed detection (Sensor X)
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
void Motor_Speed_Run_EN115(MTRFREQITEM* ptMTR)
{
    
    u16 Escalator_speed = 0;
    	
    
    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        time_running_tms = 0;
    } 

    if( SfBase_EscState & ESC_STATE_RUNNING )
    {      
        
        if( ( time_running_tms * SYSTEMTICK ) > UNDERSPEED_TIME )
        {
            SfBase_EscState |= ESC_STATE_RUN5S;                
        }
        else
        {
            time_running_tms++;
        }        
        
        Escalator_speed = Measure_motor_speed(ptMTR);
        EscRTBuff[75] = MAX_SPEED;
        EscRTBuff[77] = MIN_SPEED;
        
        if( Escalator_speed >= MAX_SPEED )
        {
            
            if(ptMTR->MtrSpeedHigh115Cnt < 100) ptMTR->MtrSpeedHigh115Cnt++;
            
            if(ptMTR->MtrSpeedHigh115Cnt >= 5)
            {
                /* overspeed fault */
                EscRTBuff[74] = 1;
                
            } 
           
        }
        else
        {
            
            ptMTR->MtrSpeedHigh115Cnt = 0;
            
        }
        
        if( SfBase_EscState & ESC_STATE_RUN5S )
        {
            if( Escalator_speed <= MIN_SPEED )
            {
                /* underspeed fault */
                EscRTBuff[76] = 1;
                
            }
        }
     
        
        /* record the escalator speed */
        *ptMTR->ptFreqBuff = Escalator_speed;   

    }
    
    escState_old = SfBase_EscState;
}

/*******************************************************************************
* Function Name  : Measure_motor_speed
* Description    : Measure the escalator motor speed.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : current motor speed pulse
*******************************************************************************/
u16 Measure_motor_speed(MTRFREQITEM* ptMTR)
{
    u16 current_motor_speed_sensor,i = 0;
    u8 timeDelayCnt = 100;    
     
    
    /* 20 * 5 = 100ms */
    ptMTR->Tms_counter++;
    if( ( ptMTR->Tms_counter * SYSTEMTICK ) >= timeDelayCnt) 
    { 
        ptMTR->Tms_counter = 0; 
        
        for(i=9; i>0; i--)                                                   
        {   
            ptMTR->pulseArray[i] = ptMTR->pulseArray[i-1];
        }
        ptMTR->pulseArray[0] = ptMTR->rt_pulse;
        ptMTR->rt_pulse=0;
        
    }
    current_motor_speed_sensor = 0;
    /* 100ms * 10 = 1s */
    for(i=0;i<10;i++) 
    {
        current_motor_speed_sensor += ptMTR->pulseArray[i];
    }      

    return current_motor_speed_sensor;
}

/*******************************************************************************
* Function Name  : Motor_Speed_1_2_Shortcircuit_Run
* Description    : Motor speed sensor 1 and 2 shortcircuit checking
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Motor_Speed_1_2_Shortcircuit_Run(void)
{
    static u16 First_motorspeed_edge_detected,Timer_motorspeed_shortcircuit = 0;

    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        First_motorspeed_edge_detected = 0;
    } 
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_motorspeed_edge_detected == 0 )
        {
            First_motorspeed_edge_detected = 1;
            Timer_motorspeed_shortcircuit = 0;
            TIM5_Int_Init(65535,71);
            
        }    
        else
        {
            Timer_motorspeed_shortcircuit = TIM_GetCounter(TIM5);
            if( Timer_motorspeed_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_motorspeed_shortcircuit = 0;               
                TIM_SetCounter(TIM5,0); 
                
                /* Fault – Motorspeed Sensor shortcircuited */
                EscRTBuff[66] = 1;
            }
            else
            {
                Timer_motorspeed_shortcircuit = 0;             
                TIM_SetCounter(TIM5,0);  
                
                EscRTBuff[66] = 0;
            }
        }
    }


}

/*******************************************************************************
* Function Name  : Check_Stopping_Distance
* Description    : Stopping distance.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
void Check_Stopping_Distance(MTRFREQITEM* ptMTR)
{   

    if( ( SfBase_EscState & ESC_STATE_STOP ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        ptMTR->last_brake_pulse = ptMTR->rt_brake_pulse;
    } 
    
    if(SfBase_EscState & ESC_STATE_STOP) 
    {
        if((ptMTR->rt_brake_pulse) > MAX_DISTANCE)
        {
            /* stopping distance fault */                     
        }   
           
        
        if( !(ptMTR->rt_brake_stop))
        {
            ptMTR->BrakeCalTms++;
            if( ( ptMTR->BrakeCalTms * SYSTEMTICK ) > 1000 ) 
            {
                
                if(( ptMTR->rt_brake_pulse ) && ( ptMTR->rt_brake_pulse == ptMTR->last_brake_pulse )) 
                {
                    /* record the escalator stopping distance */
                    *(ptMTR->ptBrakeDistanceBuff) = ptMTR->rt_brake_pulse;
                    ptMTR->rt_brake_pulse = 0;
                    ptMTR->rt_brake_stop = 1;
                    
                }
                else
                {
                    ptMTR->last_brake_pulse = ptMTR->rt_brake_pulse;
                }
                
                ptMTR->BrakeCalTms = 0;
                
            }    
        }
    } 
    else
    {
        ptMTR->BrakeCalTms=0;
        ptMTR->rt_brake_pulse = 0;
    }   

}


/******************************  END OF FILE  *********************************/






