/*******************************************************************************
* File Name          : esc_motor_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 11/22/2016
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
#define MOTOR_SSM_SHORTCIRCUIT_TIME     300u

/* Private variables ---------------------------------------------------------*/
static u8 g_u8FirstMotorSpeedEdgeDetected = 0u; 
static u8 g_u8FirstMotorSpeedSensorCheck = 0u; 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static u16 Measure_motor_speed(MotorSpeedItem* ptMTR);
static void Motor_Speed_Run_EN115(MotorSpeedItem* ptMTR);
static void Check_Stopping_Distance(MotorSpeedItem* ptMTR, u16 EscStateOld);
static void Motor_Speed_Senor_Check(void);
static void Motor_Speed_Direction_Check(void);


static u16 stat_u16MaxDistace = 0u;
static u16 stat_u16ShoppingDistance = 0u;

MotorSpeedItem MTRITEM[2]=
{
    {
        /* motor speed 1 */
        1u,
        0u,
        0u,
        0u,    
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},                 
        (u16*)&MOTOR_SPEED1, 
        0u,
        0u,
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
        0u,
        0u,
        0u,
        TIM1_Int_Init,
        TIM1,
        
        /* brake distance */
        (u16*)&STOPPING_DISTANCE1,
        0u,
        1u
    },
    {
        /* motor speed 2 */
        2u,
        0u,
        0u,
        0u,    
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u}, 
        (u16*)&MOTOR_SPEED2, 
        0u,
        0u,
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
        0u,
        0u,
        0u,
        TIM2_Int_Init,
        TIM2,
            
        /* brake distance */
        (u16*)&STOPPING_DISTANCE2,
        0u,
        1u                     
    }
};




/*******************************************************************************
* Function Name  : Motor_Speed_Run_EN115
* Description    : Overspeed and underspeed detection (Sensor X)
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Speed_Run_EN115(MotorSpeedItem* ptMTR)
{  
    u8 i;	
    
    /* System in State: STOPPING PROCESS or READY or STARTING PROCESS or FAULT or INTERM */
    if( SfBase_EscState != ESC_RUN_STATE )
    {
        ptMTR->TimeoutTimerMotorSpeed++;
        
        if(( ptMTR->TimeoutTimerMotorSpeed * SYSTEMTICK ) > 8000u )
        {
            /* prevent overflow */
            ptMTR->TimeoutTimerMotorSpeed = 8000u;
            
            *ptMTR->ptFreqBuff = Measure_motor_speed(ptMTR);
            
            if( *ptMTR->ptFreqBuff > 1u )
            {
                /* Fault: SPEED SENSOR TIMEOUT STOPPING PROCESS (F340) */
                EN_ERROR43 |= 0x10u;
            }         
        } 
    }
    
    /* System in run state or stopping process state */
    if(( SfBase_EscState == ESC_RUN_STATE ) || ( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ))
    { 
        ptMTR->TimerRuningTms++;
        
        /* record the escalator speed */ 
        *ptMTR->ptFreqBuff = Measure_motor_speed(ptMTR);  
        
        if( ptMTR->between_pulse_counter > 0u )
        {
            ptMTR->TimerNoPulse = 0u;
            
            for( i = 0u; i < ptMTR->between_pulse_counter; i++ )
            {
                /* judge overspeed, ptMTR->TimerMotorSpeedBetweenPulse[i] unit is 10 microsecond */
                if( ptMTR->TimerMotorSpeedBetweenPulse[i] < ( 100000u / MAX_SPEED ))
                {
                    ptMTR->NotOkCounter++;
                    ptMTR->OkCounter = 0u;
                    if( ptMTR->NotOkCounter >= 5u )
                    {
                        /* Fault: OVERSPEED (F264) */
                        EN_ERROR34 |= 0x01u;
                        /* reset timer */
                        ptMTR->TimerRuningTms = 0u;
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
                
                /* In stopping process state not need to judge the underspeed */
                if( SfBase_EscState == ESC_RUN_STATE )
                {
                    /* judge underspeed */
                    if( ptMTR->NotOkCounter < 5u )
                    {
                        if( ( ptMTR->TimerRuningTms * SYSTEMTICK ) > UNDERSPEED_TIME )
                        {
                            /* prevent overflow */
                            ptMTR->TimerRuningTms = UNDERSPEED_TIME;
                            
                            if( ptMTR->TimerMotorSpeedBetweenPulse[i] > ( 100000u / MIN_SPEED ))
                            {
                                /* Fault: LOW SPEED (F265) */
                                EN_ERROR34 |= 0x02u;
                                /* reset timer */
                                ptMTR->TimerRuningTms = 0u;
                            }
                        }
                    }
                }
                
                ptMTR->TimerMotorSpeedBetweenPulse[i] = 0u;          
            }
            
            ptMTR->between_pulse_counter = 0u;
        }
        else
        {
            if( SfBase_EscState == ESC_RUN_STATE )
            {
                ptMTR->TimerNoPulse++;
                if( ( ptMTR->TimerRuningTms * SYSTEMTICK ) > UNDERSPEED_TIME )
                {
                    /* prevent overflow */
                    ptMTR->TimerRuningTms = UNDERSPEED_TIME;      
                    
                    /* if no motor pulse */
                    if(( TIM_GetCounter(ptMTR->TimerNum) > ( 100000u / MIN_SPEED ))  /* 10us */
                       || (( ptMTR->TimerNoPulse * SYSTEMTICK ) > ( 1000u / MIN_SPEED ))) /* ms */ 
                    {
                        /* Fault: LOW SPEED (F265) */
                        EN_ERROR34 |= 0x02u;
                        /* reset timer */
                        ptMTR->TimerRuningTms = 0u;
                    }
                }
            }
        } 
        
        if( SfBase_EscState == ESC_STOPPING_PROCESS_STATE )
        {
            /* Speed is zero */
            if( *ptMTR->ptFreqBuff < 1u )
            {
                /* stopping process finish */
                ptMTR->rt_brake_stop = 1u;
            }   
        }
    }
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
    /* Get between two pulse Timer counter, unit is microsecond */
    ptMTR->TimerMotorSpeedBetweenPulse[ptMTR->between_pulse_counter] = TIM_GetCounter(ptMTR->TimerNum);
    /* Reset the Timer counter */
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
   
    /* System in run state */
    if( SfBase_EscState == ESC_RUN_STATE )
    {  
    
        if( g_u8FirstMotorSpeedEdgeDetected == 0u )
        {
            g_u8FirstMotorSpeedEdgeDetected = 1u;
            stat_u32TimerMotorSpeedShortCircuit = 0u;
            stat_u32MotorSpeedShortCircuitOkCounter = 0u;
            stat_u32MotorSpeedShortCircuitNotOkCounter = 0u; 
            
            /* Timer init, counter is 1us */
            TIM_Cmd(TIM5, DISABLE);
            TIM5_Int_Init(65535u,71u);           
        }    
        else
        {
            /* Get the time between the two signals */
            stat_u32TimerMotorSpeedShortCircuit = TIM_GetCounter(TIM5);
            /* Reset the timer */
            TIM_SetCounter(TIM5,0u);
            
            if( stat_u32TimerMotorSpeedShortCircuit < MOTOR_SSM_SHORTCIRCUIT_TIME )
            {
                stat_u32TimerMotorSpeedShortCircuit = 0u;                                 
                stat_u32MotorSpeedShortCircuitNotOkCounter++;
                stat_u32MotorSpeedShortCircuitOkCounter = 0u;
                
                /* 100 consecutive pulses with less than SSM_SHORTCIRCUIT_TIME, go to fault */
                if( stat_u32MotorSpeedShortCircuitNotOkCounter >= 100u )
                {
                    /* Fault ¨C Motorspeed Sensor shortcircuited (F267) */
                    EN_ERROR34 |= 0x08u;
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
* Function Name  : Motor_Speed_Senor_Check
* Description    : Check the difference in both speed sensors.
*                  Check the difference in both speed sensors between micro.     
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Speed_Senor_Check(void)
{
    static u16 stat_u16TimerMotorSpeedSensor = 0u;
    static u16 stat_u16Timer1Cpu2MotorSpeedSensor = 0u;
    static u16 stat_u16Timer2Cpu2MotorSpeedSensor = 0u;
    u16 u16MotorSpeedSensorDiff = 0u;

     /* System in run state */
    if( SfBase_EscState == ESC_RUN_STATE )
    {    
        if( g_u8FirstMotorSpeedSensorCheck == 0u )
        {
            g_u8FirstMotorSpeedSensorCheck = 1u;
            stat_u16TimerMotorSpeedSensor = 0u;
            stat_u16Timer1Cpu2MotorSpeedSensor = 0u;
            stat_u16Timer2Cpu2MotorSpeedSensor = 0u;         
        } 
        
        /* The safety board goes to fault when the difference 
        in both speed sensors is more than 5% for more than 1 second */        
        if( *MTRITEM[0].ptFreqBuff > *MTRITEM[1].ptFreqBuff )
        {
            u16MotorSpeedSensorDiff = ( *MTRITEM[0].ptFreqBuff - *MTRITEM[1].ptFreqBuff ) * 100u / *MTRITEM[1].ptFreqBuff;
        }
        else if( *MTRITEM[0].ptFreqBuff < *MTRITEM[1].ptFreqBuff )
        {
            u16MotorSpeedSensorDiff = ( *MTRITEM[1].ptFreqBuff - *MTRITEM[0].ptFreqBuff ) * 100u / *MTRITEM[0].ptFreqBuff;
        }
        else
        {
            stat_u16TimerMotorSpeedSensor = 0u;
        }
        
        if( u16MotorSpeedSensorDiff > 5u )
        {
            stat_u16TimerMotorSpeedSensor++;
            if(( stat_u16TimerMotorSpeedSensor * SYSTEMTICK ) > 1000u )
            {
                /* Fault: DISCRESPANCE_SPEED_SENSOR */
                /* Discrepance motor speed sensor F407 */
                EN_ERROR51 |= 0x80u;
            }
        }
        
        
        /* The safety board goes to fault when the difference in both speed 
        sensors between microprocessors is more than 5% for more than 1 second */
        
        /* wait the speed stable */
        if( ( MTRITEM[0].TimerRuningTms * SYSTEMTICK ) > UNDERSPEED_TIME )
        {
            /* Motor speed sensor 1 */
            if( OMC_MOTOR_SPEED1 > *MTRITEM[0].ptFreqBuff )
            {
                u16MotorSpeedSensorDiff = ( OMC_MOTOR_SPEED1 - *MTRITEM[0].ptFreqBuff ) * 100u / *MTRITEM[0].ptFreqBuff;
            }
            else if( OMC_MOTOR_SPEED1 < *MTRITEM[0].ptFreqBuff )
            {
                u16MotorSpeedSensorDiff = ( *MTRITEM[0].ptFreqBuff - OMC_MOTOR_SPEED1 ) * 100u / OMC_MOTOR_SPEED1;
            }
            else
            {
                stat_u16Timer1Cpu2MotorSpeedSensor = 0u;
            }  
            
            if( u16MotorSpeedSensorDiff > 5u )
            {
                stat_u16Timer1Cpu2MotorSpeedSensor++;
                if(( stat_u16Timer1Cpu2MotorSpeedSensor * SYSTEMTICK ) > 1000u )
                {
                    /* Fault: DISCREPANCE_SPEED_CPU */
                    /* Discrepance motor speed CPU F408 */
                    EN_ERROR52 |= 0x01u;
                }
            }            
            
            /* Motor speed sensor 2 */
            if( OMC_MOTOR_SPEED2 > *MTRITEM[1].ptFreqBuff )
            {
                u16MotorSpeedSensorDiff = ( OMC_MOTOR_SPEED2 - *MTRITEM[1].ptFreqBuff ) * 100u / *MTRITEM[1].ptFreqBuff;
            }
            else if( OMC_MOTOR_SPEED2 < *MTRITEM[1].ptFreqBuff )
            {
                u16MotorSpeedSensorDiff = ( *MTRITEM[1].ptFreqBuff - OMC_MOTOR_SPEED2 ) * 100u / OMC_MOTOR_SPEED2;
            }
            else
            {
                stat_u16Timer2Cpu2MotorSpeedSensor = 0u;
            }  
            
            if( u16MotorSpeedSensorDiff > 5u )
            {
                stat_u16Timer2Cpu2MotorSpeedSensor++;
                if(( stat_u16Timer2Cpu2MotorSpeedSensor * SYSTEMTICK ) > 1000u )
                {
                    /* Fault: DISCREPANCE_SPEED_CPU */
                    /* Discrepance motor speed CPU F408 */
                    EN_ERROR51 |= 0x80u;
                }
            }
        }       
    }
}

/*******************************************************************************
* Function Name  : Motor_Speed_Direction_Check
* Description    : Check the motor speed direction.    
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Speed_Direction_Check(void)
{
    /* how to measure the motor speed direction? */
    
}


/*******************************************************************************
* Function Name  : Check_Stopping_Distance
* Description    : Stopping distance.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
static void Check_Stopping_Distance(MotorSpeedItem* ptMTR, u16 EscStateOld)
{       
    
    if( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ) 
    {        
        if(( SPEED_SENSOR_INSTALLATION == 0u ) || ( SPEED_SENSOR_INSTALLATION == 3u ))
        {
            /* Stopping_distance = Motor_speed_counter * CONVERSION_MOTORSPEED */
            stat_u16ShoppingDistance = (ptMTR->rt_brake_pulse) * CONVERSION_MOTORSPEED;
        }
        else
        {
            /* Stopping_distance = Mainshaft_speed_counter * CONVERSION_MAINSHAFTSPEED */
            /*stat_u16ShoppingDistance = Mainshaft_speed_counter * CONVERSION_MAINSHAFTSPEED; */           
        }
        
        if( stat_u16ShoppingDistance > stat_u16MaxDistace )
        {
            /* if no Fault: OVERSPEED  or Fault: LOW SPEED, Excessive distance FAULT */
            if( (!( EN_ERROR34 & 0x01u )) && (!( EN_ERROR34 & 0x02u )))
            {
                /* Excessive distance FAULT (F275) */  
                EN_ERROR35 |= 0x08u;
            }
        } 
    } 
    else
    {
        ptMTR->rt_brake_pulse = 0u;
        /* Mainshaft_speed_counter =0; */
    }        
    
    if(( EscStateOld == ESC_STOPPING_PROCESS_STATE ) && ( !(SfBase_EscState == ESC_STOPPING_PROCESS_STATE))) 
    {
        /* Stopping process finished */
        if( ptMTR->rt_brake_stop == 1u )  
        {
            /* Send the stopping distance to control board */
            *(ptMTR->ptBrakeDistanceBuff) = stat_u16ShoppingDistance;
        } 
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
 
    /* esc turn to run state, init */
    if((SfBase_EscState == ESC_RUN_STATE) && (!(stat_u16EscStateOld == ESC_RUN_STATE))) 
    { 
        g_u8FirstMotorSpeedEdgeDetected = 0u;
        g_u8FirstMotorSpeedSensorCheck = 0u;
        
        /* Reset the value when escalator run */
        MTRITEM[0].TimerRuningTms = 0u;
        MTRITEM[0].NotOkCounter = 0u;
        MTRITEM[0].OkCounter = 0u;
        MTRITEM[0].between_pulse_counter = 0u;    
        
        MTRITEM[1].TimerRuningTms = 0u;
        MTRITEM[1].NotOkCounter = 0u;
        MTRITEM[1].OkCounter = 0u;
        MTRITEM[1].between_pulse_counter = 0u; 
        
        /* Timer init, counter is 10us */
        TIM_Cmd(MTRITEM[0].TimerNum, DISABLE);
        TIM_Cmd(MTRITEM[1].TimerNum, DISABLE);
        MTRITEM[0].Timer_Init(65535u, 719u);
        MTRITEM[1].Timer_Init(65535u, 719u);
    } 
    else
    {   
        /* esc enter stopping process state, init */
        if(( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ) && ( !(stat_u16EscStateOld == ESC_STOPPING_PROCESS_STATE)))
        {
            MTRITEM[0].rt_brake_stop = 0u;
            MTRITEM[1].rt_brake_stop = 0u;
            
            /* Restart: TimeoutTimerMotorSpeed */
            MTRITEM[0].TimeoutTimerMotorSpeed = 0u;
            MTRITEM[1].TimeoutTimerMotorSpeed = 0u;
            
            /* Calculate MAX_DISTANCE */
            if( NOMINAL_SPEED <= 75u )
            {
                stat_u16MaxDistace = MAX_DISTANCE;
            }
            else if( NOMINAL_SPEED <= 90u )
            {
                stat_u16MaxDistace = MAX_DISTANCE2;
            }   
            else
            {
                /* Warning: Wrong Parameter NOMINAL_SPEED */
                stat_u16MaxDistace = 0u;
            }
        }            
    }   
        
    Motor_Speed_Run_EN115(&MTRITEM[0]);
    Motor_Speed_Run_EN115(&MTRITEM[1]);
    
    Motor_Speed_Senor_Check();          
    
    Check_Stopping_Distance(&MTRITEM[0],stat_u16EscStateOld);
    Check_Stopping_Distance(&MTRITEM[1],stat_u16EscStateOld);
    
    
    stat_u16EscStateOld = SfBase_EscState; 
}



/******************************  END OF FILE  *********************************/






