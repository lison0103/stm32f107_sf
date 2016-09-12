/*******************************************************************************
* File Name          : esc_motor_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 09/12/2016
* Description        : This file contains esc motor speed and brake distance.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_motor_speed.h"
#include "delay.h"
#include "initial_devices.h"
#include "timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u8 g_u8FirstMotorSpeedEdgeDetected = 0u; 
static u32 g_u32TimeRuningTms = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static u16 Measure_motor_speed(MotorSpeedItem* ptMTR);
static void Motor_Speed_Ready(MotorSpeedItem* ptMTR);
static void Motor_Speed_Run_EN115(MotorSpeedItem* ptMTR);
static void Check_Stopping_Distance(MotorSpeedItem* ptMTR);
static void Motor_Senor_Check(void);

MotorSpeedItem MTRITEM[2]=
{
    {
        /* motor speed 1 */
        0,
        0,    
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                 
        (u16*)&EscRTBuff[40], 
        &EscRTBuff[48],
        0,
        0,
        {0,0,0,0,0,0,0,0,0,0},
        0,
        TIM1_Int_Init,
        TIM1,
        
        /* brake distance */
        (u16*)&EscRTBuff[44],
        0,
        0,   
        0,0,1
    },
    {
        /* motor speed 2 */
        0,
        0,    
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        (u16*)&EscRTBuff[42], 
        &EscRTBuff[49],
        0,
        0,
        {0,0,0,0,0,0,0,0,0,0},
        0,
        TIM2_Int_Init,
        TIM2,
            
        /* brake distance */
        (u16*)&EscRTBuff[46],
        0,
        0,   
        0,0,1            
            
    }
};



/*******************************************************************************
* Function Name  : Motor_Speed_Ready
* Description    : Pulse detection when escalator is stopped.
* Input          : ptMTR: motor speed sensor id          
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Speed_Ready(MotorSpeedItem* ptMTR)
{
    u16 Escalator_speed = 0u;
    
    if( SfBase_EscState & ESC_READY_STATE ) 
    {              
        Escalator_speed = Measure_motor_speed(ptMTR);
        
        if( Escalator_speed > 1u )
        {
            /* fault */
            *(ptMTR->pcErrorCodeBuff) |= 0x01u;
            EN_ERROR1 |= 0x01u;
        }            
    } 
}


/*******************************************************************************
* Function Name  : Motor_Speed_Run_EN115
* Description    : Overspeed and underspeed detection (Sensor X)
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Speed_Run_EN115(MotorSpeedItem* ptMTR)
{  
    u16 Escalator_speed = 0u;
    u8 i;	
    
    if(( SfBase_EscState & ESC_RUN_STATE ) || ( SfBase_EscState & ESC_STOPPING_PROCESS_STATE ))
    {      
                
        g_u32TimeRuningTms++;
        Escalator_speed = Measure_motor_speed(ptMTR);
        
        /* for debug */
        MTR_MAX_SPEED = MAX_SPEED;
        MTR_MIN_SPEED = MIN_SPEED;
        

        for( i = 0u; i < ptMTR->between_pulse_counter; i++ )
        {
            /* judge overspeed */
            if( ptMTR->TimerMotorSpeedBetweenPulse[i] < ( 1000000u / MAX_SPEED ))
            {
                ptMTR->NotOkCounter++;
                ptMTR->OkCounter = 0u;
                if( ptMTR->NotOkCounter >= 5u )
                {
                    /* overspeed fault */
                    EN_ERROR1 |= 0x02u;
                    /* reset timer */
                    g_u32TimeRuningTms = 0u;
                }
            }
            else
            {
                ptMTR->OkCounter++;
                if( ptMTR->OkCounter >= 10u )
                {
                    ptMTR->NotOkCounter = 0u;
                }
            }
            
            /* judge underspeed */
            if( ptMTR->NotOkCounter < 5u )
            {
                if( ( g_u32TimeRuningTms * SYSTEMTICK ) > UNDERSPEED_TIME )
                {
                    if( ptMTR->TimerMotorSpeedBetweenPulse[i] > ( 1000000u / MIN_SPEED ))
                    {
                        /* underspeed fault */
                        EN_ERROR1 |= 0x04u; 
                        /* reset timer */
                        g_u32TimeRuningTms = 0u;
                    }
                    else
                    {
                        /* if no motor pulse */
                        if( TIM_GetCounter(ptMTR->TimerNum) > ( 1000000u / MIN_SPEED ))
                        {
                            /* underspeed fault */
                            EN_ERROR1 |= 0x04u; 
                            /* reset timer */
                            g_u32TimeRuningTms = 0u;
                        }
                    }
                }
            }
            
            ptMTR->TimerMotorSpeedBetweenPulse[i] = 0u;          
        }
        
        ptMTR->between_pulse_counter = 0u;
        
    } 
    
    /* record the escalator speed */
    *ptMTR->ptFreqBuff = Escalator_speed;   

}

/*******************************************************************************
* Function Name  : Measure_motor_between_pulse
* Description    : Measure the escalator motor between pulse.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
void Measure_motor_between_pulse(MotorSpeedItem* ptMTR)
{
        /* Get between two pulse Timer counter */
        ptMTR->TimerMotorSpeedBetweenPulse[ptMTR->between_pulse_counter] = TIM_GetCounter(ptMTR->TimerNum);
        /* Clear the Timer counter */
        TIM_SetCounter(ptMTR->TimerNum, 0u); 
        ptMTR->between_pulse_counter++;      
}

/*******************************************************************************
* Function Name  : Measure_motor_speed
* Description    : Measure the escalator motor speed.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : current motor speed pulse
*******************************************************************************/
static u16 Measure_motor_speed(MotorSpeedItem* ptMTR)
{
    u16 u16CurrentMotorSpeedSensor = 0u;
    u8 u8TimeDelayCnt = 100u;
    u8 i;
    
    /* 20 * 5 = 100ms */
    ptMTR->Tms_counter++;
    if( ( ptMTR->Tms_counter * SYSTEMTICK ) >= u8TimeDelayCnt )
    {
        ptMTR->Tms_counter = 0u;
        
        for( i = 9u; i > 0u; i-- )                                                         
        {
            ptMTR->pulseArray[i] = ptMTR->pulseArray[i - 1u];
        }
        ptMTR->pulseArray[0] = ptMTR->rt_pulse;
        ptMTR->rt_pulse = 0u;
        
    }
    u16CurrentMotorSpeedSensor = 0u;
    
    /* 100ms * 10 = 1s */
    for( i = 0u; i < 10u; i++ )
    {
        u16CurrentMotorSpeedSensor += ptMTR->pulseArray[i];
    }
    
    return u16CurrentMotorSpeedSensor;
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
    static u32 stat_u32TimerMotorSpeedShortCircuit = 0u;
    static u32 stat_u32MotorSpeedShortCircuitOkCounter = 0u;
    static u32 stat_u32MotorSpeedShortCircuitNotOkCounter = 0u;
   
    if( SfBase_EscState & ESC_RUN_STATE )
    {  
        if( g_u8FirstMotorSpeedEdgeDetected == 0u )
        {
            g_u8FirstMotorSpeedEdgeDetected = 1u;
            stat_u32TimerMotorSpeedShortCircuit = 0u;
            stat_u32MotorSpeedShortCircuitOkCounter = 0u;
            stat_u32MotorSpeedShortCircuitNotOkCounter = 0u;
            TIM_Cmd(TIM5, DISABLE);
            TIM5_Int_Init(65535u,71u);           
        }    
        else
        {
            /* Get the time between the two signals */
            stat_u32TimerMotorSpeedShortCircuit = TIM_GetCounter(TIM5);
            TIM_SetCounter(TIM5,0u);
            
            if( stat_u32TimerMotorSpeedShortCircuit < SSM_SHORTCIRCUIT_TIME )
            {
                stat_u32TimerMotorSpeedShortCircuit = 0u;                                 
                stat_u32MotorSpeedShortCircuitNotOkCounter++;
                stat_u32MotorSpeedShortCircuitOkCounter = 0u;
                
                /* 100 consecutive pulses with less than SSM_SHORTCIRCUIT_TIME, go to fault */
                if( stat_u32MotorSpeedShortCircuitNotOkCounter >= 100u )
                {
                    /* Fault ¨C Motorspeed Sensor shortcircuited */
                    EN_ERROR4 |= 0x01u;
                }
            }
            else
            {
                stat_u32TimerMotorSpeedShortCircuit = 0u;              
                stat_u32MotorSpeedShortCircuitOkCounter++;
                
                /* counter should be resetted after 10 consecutive different pulses with more than SSM_SHORTCIRCUIT_TIME */
                if( stat_u32MotorSpeedShortCircuitOkCounter >= 10u )
                {
                    stat_u32MotorSpeedShortCircuitNotOkCounter = 0u;
                }  
            }
        }
    }
}

/*******************************************************************************
* Function Name  : Motor_Senor_Check
* Description    : Check the difference in both speed sensors.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Senor_Check(void)
{
    /* The safety board goes to fault when the difference 
    in both speed sensors is more than 5% for more than 1 second */
    
    
    
    /* The safety board goes to fault when the difference in both speed 
    sensors between microprocessors is more than 5% for more than 1 second */
    
    
}


/*******************************************************************************
* Function Name  : Check_Stopping_Distance
* Description    : Stopping distance.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
static void Check_Stopping_Distance(MotorSpeedItem* ptMTR)
{   
    
    if((SfBase_EscState & ESC_STOPPING_PROCESS_STATE ) && ( ptMTR->rt_brake_stop == 0u )) 
    {
        if((ptMTR->rt_brake_pulse) > MAX_DISTANCE)
        {
            /* stopping distance fault */  
            *(ptMTR->pcErrorCodeBuff) |= 0x08u;
            EN_ERROR1 |= 0x08u;
        }   
           
        
        if( !(ptMTR->rt_brake_stop))
        {
            ptMTR->BrakeCalTms++;
            if( ( ptMTR->BrakeCalTms * SYSTEMTICK ) > 1000u ) 
            {
                  
                if( ptMTR->rt_brake_pulse == ptMTR->last_brake_pulse )     
                {
                    /* record the escalator stopping distance */
                    *(ptMTR->ptBrakeDistanceBuff) = ptMTR->rt_brake_pulse;
                    ptMTR->rt_brake_pulse = 0u;
                    ptMTR->rt_brake_stop = 1u;
                    
                }
                else
                {
                    ptMTR->last_brake_pulse = ptMTR->rt_brake_pulse;
                }
                
                ptMTR->BrakeCalTms = 0u;
                
            }    
        }
    } 
    else
    {
        ptMTR->BrakeCalTms = 0u;
        ptMTR->rt_brake_pulse = 0u;
    }   

}


/*******************************************************************************
* Function Name  : ESC_Motor_Check
* Description    : Esc motor speed and brake distance check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Motor_Check(void)
{
    static u16 stat_u16EscStateOld = 0u; 
    
    
    Motor_Speed_Ready(&MTRITEM[0]);
    Motor_Speed_Ready(&MTRITEM[1]);		
    
    if((SfBase_EscState & ESC_RUN_STATE) && (!(stat_u16EscStateOld & ESC_RUN_STATE))) 
    { 
        g_u8FirstMotorSpeedEdgeDetected = 0u;
        g_u32TimeRuningTms = 0u;
        
        /* Clear motor between pulse counter */
        MTRITEM[0].between_pulse_counter = 0u;
        MTRITEM[1].between_pulse_counter = 0u;
        
        /* Timer init, counter is 1us */
        TIM_Cmd(MTRITEM[0].TimerNum, DISABLE);
        TIM_Cmd(MTRITEM[1].TimerNum, DISABLE);
        MTRITEM[0].Timer_Init(65535u, 71u);
        MTRITEM[1].Timer_Init(65535u, 71u);
    } 
    else
    {        
        if(( SfBase_EscState & ESC_STOPPING_PROCESS_STATE ) && ( !(stat_u16EscStateOld & ESC_STOPPING_PROCESS_STATE)))
        {
            MTRITEM[0].last_brake_pulse = MTRITEM[0].rt_brake_pulse;
            MTRITEM[1].last_brake_pulse = MTRITEM[1].rt_brake_pulse;
            MTRITEM[0].rt_brake_stop = 0u;
            MTRITEM[1].rt_brake_stop = 0u;
        }            
    }   
    
    Motor_Speed_Run_EN115(&MTRITEM[0]);
    Motor_Speed_Run_EN115(&MTRITEM[1]);
    
    Check_Stopping_Distance(&MTRITEM[0]);
    Check_Stopping_Distance(&MTRITEM[1]);
    
    
    stat_u16EscStateOld = SfBase_EscState;                                      

}







/******************************  END OF FILE  *********************************/






