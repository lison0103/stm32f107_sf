/*******************************************************************************
* File Name          : esc_motor_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 12/23/2016
* Description        : This file contains esc motor speed and brake distance.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_motor_speed.h"
#include "delay.h"
#include "initial_devices.h"
#include "timer.h"
#include "esc_missing_step.h"
#include "esc_handrail_speed.h"
#include "esc_main_shaft_speed.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define MOTOR_SHORTCIRCUIT_CHECK_TIME           5000u
#define MAINSHAFT_SHORTCIRCUIT_CHECK_TIME       5000u
#define HANDRAIL_SHORTCIRCUIT_CHECK_TIME        50000u
#define MISSINGSTEP_SHORTCIRCUIT_CHECK_TIME     150000u

/* Private variables ---------------------------------------------------------*/
static u8 g_u8FirstMotorSpeedEdgeDetected = 0u; 
static u8 g_u8FirstMotorSpeedSensor1Detected = 0u; 
static u8 g_u8FirstMotorSpeedSensorCheck = 0u; 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void Motor_Speed_Senor_Check(void);
static void Sensor_Shortcircuit_Check(void);


u8 g_u8SensorShortCircuitCheck = 0u;
u8 g_u8EscStoppingFinish = 0u;
static u16 stat_u16MaxDistace = 0u;
static u16 stat_u16ShoppingDistance = 0u;


MotorSpeedItem MTRITEM[2]=
{
    {
        /* motor speed 1 */
        1u,
        1u,
        0u,
        0u,
        0u,    
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},                 
        &MOTOR_SPEED1, 
        0u,
        0u,
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
        0u,
        0u,
        0u,
        0u,
        MOTOR_SPEED1_CHECK_TIMER_INIT,
        MOTOR_SPEED1_CHECK_TIMER,
        
        /* brake distance */
        &STOPPING_DISTANCE1,
        0u,
        1u
    },
    {
        /* motor speed 2 */
        1u,
        2u,
        0u,
        0u,
        0u,    
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u}, 
        &MOTOR_SPEED2, 
        0u,
        0u,
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
        0u,
        0u,
        0u,
        0u,
        MOTOR_SPEED1_CHECK_TIMER_INIT,
        MOTOR_SPEED1_CHECK_TIMER,       
            
        /* brake distance */
        &STOPPING_DISTANCE2,
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
void Motor_Speed_Run_EN115(MotorSpeedItem* ptMTR)
{  
    u8 i;
    
    /* System in State: STOPPING PROCESS or READY or STARTING PROCESS or FAULT or INTERM */
    if( SfBase_EscState != ESC_RUN_STATE )
    {
        /* prevent overflow */
        if( ptMTR->TimeoutTimerMotorSpeed < 10000u )
        {
            ptMTR->TimeoutTimerMotorSpeed++;
        }
        
        /* TimeoutTimerMotorSpeed > 8 sec */
        if(( ptMTR->TimeoutTimerMotorSpeed * SYSTEMTICK ) > 8000u )
        {           
            *ptMTR->ptFreqBuff = Measure_motor_speed(ptMTR);
            
            if( *ptMTR->ptFreqBuff > 1u )
            {
                if( ptMTR->SensorX == 1u )
                {
                    /* Motor speed sensor 1 timeout stopping process F409 */
                    EN_ERROR52 |= 0x02u;
                }
                else if( ptMTR->SensorX == 2u )
                {
                    /* Motor speed sensor 2 timeout stopping process F410 */
                    EN_ERROR52 |= 0x04u;                
                }    
                else
                {}
            }         
        } 
    }
    
    /* System in run state or stopping process state */
    if(( SfBase_EscState == ESC_RUN_STATE ) || ( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ))
    { 
        /* prevent overflow */
        if( ptMTR->TimerRuningTms < 20000u )
        {
            ptMTR->TimerRuningTms++;
        }
 
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
                    if( ptMTR->NotOkCounter < 0xffu )
                    {
                        ptMTR->NotOkCounter++;
                    }
                    ptMTR->OkCounter = 0u;
                    if( ptMTR->NotOkCounter >= 5u )
                    {
                        /* Fault: motor overspeed (F258) */
                        EN_ERROR33 |= 0x04u;                         
                        /* reset timer */
                        ptMTR->TimerRuningTms = 0u;
                    }
                }
                else
                {
                    if( ptMTR->OkCounter < 0xffu )
                    {
                        ptMTR->OkCounter++;
                    }
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
                            if( ptMTR->TimerMotorSpeedBetweenPulse[i] > ( 100000u / MIN_SPEED ))
                            {
                                /* Fault: motor underspeed (F259) */
                                EN_ERROR33 |= 0x08u;
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
                if( ptMTR->TimerNoPulse < 200u )
                {
                    ptMTR->TimerNoPulse++;
                }
                if( ( ptMTR->TimerRuningTms * SYSTEMTICK ) > UNDERSPEED_TIME )
                {     
                    if(( ptMTR->TimerNoPulse * SYSTEMTICK ) > ( 1000u / MIN_SPEED )) /* ms */ 
                    {
                        /* Fault: motor underspeed (F259) */
                        EN_ERROR33 |= 0x08u;
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

#if 0
/*******************************************************************************
* Function Name  : Measure_between_pulse_Time
* Description    : Measure the escalator motor between pulse.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
void Measure_between_pulse_Time(MotorSpeedItem* ptMTR)
{
    /* Get between two pulse Timer counter, unit is microsecond */
    ptMTR->TimerMotorSpeedBetweenPulse[ptMTR->between_pulse_counter] = TIM_GetCounter(ptMTR->TimerSensorNum);
    /* Reset the Timer counter */
    TIM_SetCounter(ptMTR->TimerNum, 0u); 
    ptMTR->between_pulse_counter++;      
}

#else
/*******************************************************************************
* Function Name  : Measure_between_pulse_Time
* Description    : Measure the escalator motor between pulse.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
void Measure_between_pulse_Time(MotorSpeedItem* ptMTR)
{   
    u32 u32MotorPulseTime = 0u;
    
    if(( SfBase_EscState == ESC_RUN_STATE ) || ( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ))
    {
        u32MotorPulseTime = TIM_GetCounter(ptMTR->TimerSensorNum);
        if( u32MotorPulseTime > ptMTR->LastMotorSpeedPulseTime )
        {
            /* Get between two pulse Timer counter, unit is microsecond */
            ptMTR->TimerMotorSpeedBetweenPulse[ptMTR->between_pulse_counter] = u32MotorPulseTime - ptMTR->LastMotorSpeedPulseTime;
        }
        else
        {
            ptMTR->TimerMotorSpeedBetweenPulse[ptMTR->between_pulse_counter] = ((0xFFFFu - ptMTR->LastMotorSpeedPulseTime) + u32MotorPulseTime);
        }
        
        if( ptMTR->between_pulse_counter < 0xffu )
        {
            ptMTR->between_pulse_counter++;          
        }
        if( ptMTR->between_pulse_counter > 9u )
        {
            ptMTR->between_pulse_counter = 9u;
        }
        ptMTR->LastMotorSpeedPulseTime = u32MotorPulseTime;
    }
}
#endif

/*******************************************************************************
* Function Name  : Measure_motor_speed
* Description    : Measure the escalator motor speed.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : current motor speed pulse
*******************************************************************************/
u16 Measure_motor_speed(MotorSpeedItem* ptMTR)
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
            TIM_Cmd(SENSOR_SHORTCIRCUIT_CHECK_TIMER, DISABLE);
            TIM6_Int_Init(65535u,71u);           
        }    
        else
        {
            /* Get the time between the two signals */
            stat_u32TimerMotorSpeedShortCircuit = TIM_GetCounter(SENSOR_SHORTCIRCUIT_CHECK_TIMER);
            /* Reset the timer */
            TIM_SetCounter(SENSOR_SHORTCIRCUIT_CHECK_TIMER,0u);
            
            if( stat_u32TimerMotorSpeedShortCircuit < PULSE_SIGNALS_MINIMUM_LAG )
            {
                stat_u32TimerMotorSpeedShortCircuit = 0u;   
                if( stat_u32MotorSpeedShortCircuitNotOkCounter < 0xffffu )
                {
                    stat_u32MotorSpeedShortCircuitNotOkCounter++;
                }
                stat_u32MotorSpeedShortCircuitOkCounter = 0u;
                
                /* 100 consecutive pulses with less than SSM_SHORTCIRCUIT_TIME, go to fault */
                if( stat_u32MotorSpeedShortCircuitNotOkCounter >= 100u )
                {
                    /* Fault ¨C Motorspeed Sensor shortcircuited (F264) */
                    EN_ERROR34 |= 0x01u;  
                }
            }
            else
            {
                stat_u32TimerMotorSpeedShortCircuit = 0u; 
                if( stat_u32MotorSpeedShortCircuitOkCounter < 0xffffu )
                {
                    stat_u32MotorSpeedShortCircuitOkCounter++;
                }
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
* Function Name  : Sensor_Shortcircuit_Run
* Description    : Motor/handrail/missing step shortcircuit use the same timer, check one by one.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Sensor_Shortcircuit_Check(void)
{
    static u32 stat_u32TimerShortCircuit = 0u;
    
    if( SfBase_EscState == ESC_RUN_STATE )
    {
        stat_u32TimerShortCircuit++;       
        if( g_u8SensorShortCircuitCheck == 0u )
        {
            /* First enter */
            g_u8SensorShortCircuitCheck = 1u;
            stat_u32TimerShortCircuit = 0u;
            g_u8FirstMotorSpeedEdgeDetected = 0u;
            g_u8FirstMainSaftSpeedEdgeDetected = 0u;
        }
        else if( g_u8SensorShortCircuitCheck == 1u )
        {
            if( SPEED_SENSOR_INSTALLATION == 0u )
            {
                /* check motor */
                if(( stat_u32TimerShortCircuit * 5u ) > MOTOR_SHORTCIRCUIT_CHECK_TIME )
                {
                    g_u8SensorShortCircuitCheck = 4u;
                    stat_u32TimerShortCircuit = 0u;
                    g_u8FirstHandrailSpeedEdgeDetected = 0u;
                }
            }
            else if( SPEED_SENSOR_INSTALLATION == 1u )
            {
                /* check main shaft */
                if(( stat_u32TimerShortCircuit * 5u ) > MAINSHAFT_SHORTCIRCUIT_CHECK_TIME )
                {
                    g_u8SensorShortCircuitCheck = 4u;
                    stat_u32TimerShortCircuit = 0u;
                    g_u8FirstHandrailSpeedEdgeDetected = 0u;
                }            
            }
            else if( SPEED_SENSOR_INSTALLATION == 2u )
            {
                /* check main shaft 1-2 */
                if(( stat_u32TimerShortCircuit * 5u ) > MAINSHAFT_SHORTCIRCUIT_CHECK_TIME )
                {
                    g_u8SensorShortCircuitCheck = 2u;
                    stat_u32TimerShortCircuit = 0u;
                    g_u8FirstMainSaftSpeedEdgeDetected = 0u;
                }             
            }
            else if( SPEED_SENSOR_INSTALLATION == 3u )
            {
                /* check motor */
                if(( stat_u32TimerShortCircuit * 5u ) > MOTOR_SHORTCIRCUIT_CHECK_TIME )
                {
                    g_u8SensorShortCircuitCheck = 2u;
                    stat_u32TimerShortCircuit = 0u;
                    g_u8FirstMainSaftSpeedEdgeDetected = 0u;
                }             
            }
            else
            {}         
        }
        else if( g_u8SensorShortCircuitCheck == 2u )
        {
            if( SPEED_SENSOR_INSTALLATION == 2u )
            {
                /* check main shaft 1-3 */
                if(( stat_u32TimerShortCircuit * 5u ) > MAINSHAFT_SHORTCIRCUIT_CHECK_TIME )
                {
                    g_u8SensorShortCircuitCheck = 3u;
                    stat_u32TimerShortCircuit = 0u;
                    g_u8FirstMainSaftSpeedEdgeDetected = 0u;
                } 
            }
            else if( SPEED_SENSOR_INSTALLATION == 3u )
            {
                /* check main shaft */
                if(( stat_u32TimerShortCircuit * 5u ) > MAINSHAFT_SHORTCIRCUIT_CHECK_TIME )
                {
                    g_u8SensorShortCircuitCheck = 4u;
                    stat_u32TimerShortCircuit = 0u;
                    g_u8FirstHandrailSpeedEdgeDetected = 0u;
                } 
            }
            else
            {}      
        }   
        else if( g_u8SensorShortCircuitCheck == 3u )
        {
            if( SPEED_SENSOR_INSTALLATION == 2u )
            {
                /* check main shaft 2-3 */
                if(( stat_u32TimerShortCircuit * 5u ) > MAINSHAFT_SHORTCIRCUIT_CHECK_TIME )
                {
                    g_u8SensorShortCircuitCheck = 4u;
                    stat_u32TimerShortCircuit = 0u;
                    g_u8FirstHandrailSpeedEdgeDetected = 0u;
                } 
            }           
        }          
        else if( g_u8SensorShortCircuitCheck == 4u )
        {
            /* check handrail */
            if(( stat_u32TimerShortCircuit * 5u ) > HANDRAIL_SHORTCIRCUIT_CHECK_TIME )
            {
                g_u8SensorShortCircuitCheck = 5u;
                stat_u32TimerShortCircuit = 0u;
                g_u8FirstMissingStepEdgeDetected = 0u;
            }  
        }
        else if( g_u8SensorShortCircuitCheck == 5u )
        {
            /* check missing step */
            if(( stat_u32TimerShortCircuit * 5u ) > MISSINGSTEP_SHORTCIRCUIT_CHECK_TIME )
            {
                g_u8SensorShortCircuitCheck = 1u;
                stat_u32TimerShortCircuit = 0u;
                g_u8FirstMotorSpeedEdgeDetected = 0u;
                g_u8FirstMainSaftSpeedEdgeDetected = 0u;
            }    
        }
        else
        {}
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
        if( MOTOR_SPEED1 > MOTOR_SPEED2 )
        {
            u16MotorSpeedSensorDiff = ( MOTOR_SPEED1 - MOTOR_SPEED2 ) * 100u / MOTOR_SPEED2;
        }
        else if( MOTOR_SPEED1 < MOTOR_SPEED2 )
        {
            u16MotorSpeedSensorDiff = ( MOTOR_SPEED2 - MOTOR_SPEED1 ) * 100u / MOTOR_SPEED1;
        }
        else
        {
            stat_u16TimerMotorSpeedSensor = 0u;
        }
        
        if( u16MotorSpeedSensorDiff > 5u )
        {
            if( stat_u16TimerMotorSpeedSensor < 0xffffu )
            {
                stat_u16TimerMotorSpeedSensor++;
            }
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
            if( OMC_MOTOR_SPEED1 > MOTOR_SPEED1 )
            {
                u16MotorSpeedSensorDiff = ( OMC_MOTOR_SPEED1 - MOTOR_SPEED1 ) * 100u / MOTOR_SPEED1;
            }
            else if( OMC_MOTOR_SPEED1 < MOTOR_SPEED1 )
            {
                u16MotorSpeedSensorDiff = ( MOTOR_SPEED1 - OMC_MOTOR_SPEED1 ) * 100u / OMC_MOTOR_SPEED1;
            }
            else
            {
                stat_u16Timer1Cpu2MotorSpeedSensor = 0u;
            }  
            
            if( u16MotorSpeedSensorDiff > 5u )
            {
                if( stat_u16Timer1Cpu2MotorSpeedSensor < 0xffffu )
                {
                    stat_u16Timer1Cpu2MotorSpeedSensor++;
                }
                if(( stat_u16Timer1Cpu2MotorSpeedSensor * SYSTEMTICK ) > 1000u )
                {
                    /* Fault: DISCREPANCE_SPEED_CPU */
                    /* Discrepance motor speed CPU F408 */
                    EN_ERROR52 |= 0x01u;
                }
            }            
            
            /* Motor speed sensor 2 */
            if( OMC_MOTOR_SPEED2 > MOTOR_SPEED2 )
            {
                u16MotorSpeedSensorDiff = ( OMC_MOTOR_SPEED2 - MOTOR_SPEED2 ) * 100u / MOTOR_SPEED2;
            }
            else if( OMC_MOTOR_SPEED2 < MOTOR_SPEED2 )
            {
                u16MotorSpeedSensorDiff = ( MOTOR_SPEED2 - OMC_MOTOR_SPEED2 ) * 100u / OMC_MOTOR_SPEED2;
            }
            else
            {
                stat_u16Timer2Cpu2MotorSpeedSensor = 0u;
            }              
            
            if( u16MotorSpeedSensorDiff > 5u )
            {
                if( stat_u16Timer2Cpu2MotorSpeedSensor < 0xffffu )
                {
                    stat_u16Timer2Cpu2MotorSpeedSensor++;
                }
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
void Motor_Speed_Direction_Check(MotorSpeedItem* ptMTR)
{
    static u32 stat_u32Time12MotorSpeedDirection = 0u;
    static u32 stat_u32Time21MotorSpeedDirection = 0u;
    static u8 stat_u8MotorSpeedDirectionOkCounter = 0u;
    static u8 stat_u8MotorSpeedDirectionNotOkCounter = 0u;
    static u8 stat_u8LastMotorSensor = 0u;
    
    /* System in run state */
    if(( SfBase_EscState == ESC_RUN_STATE ) || ( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ) || ( SfBase_EscState == ESC_FAULT_STATE ))
    {  
        if( g_u8FirstMotorSpeedSensor1Detected == 0u )
        {
            g_u8FirstMotorSpeedSensor1Detected = 1u;
            stat_u32Time12MotorSpeedDirection = 0u;
            stat_u32Time21MotorSpeedDirection = 0u;
            stat_u8MotorSpeedDirectionOkCounter = 0u;
            stat_u8MotorSpeedDirectionNotOkCounter = 0u; 
            stat_u8LastMotorSensor = ptMTR->SensorX;
            
            /* Timer init, counter is 10 us */
            TIM_Cmd(MOTOR_DIRECTION_CHECK_TIMER, DISABLE);
            TIM2_Int_Init(65535u, 719u);            
        }    
        else
        {
            if(( ptMTR->SensorX == 2u ) & ( stat_u8LastMotorSensor == 1u ))
            {
                /* Get the time between the two signals */
                stat_u32Time12MotorSpeedDirection = TIM_GetCounter(MOTOR_DIRECTION_CHECK_TIMER);
                /* Reset the timer */
                TIM_SetCounter(MOTOR_DIRECTION_CHECK_TIMER,0u);
            }
            else if(( ptMTR->SensorX == 1u ) & ( stat_u8LastMotorSensor == 2u ))
            {
                /* Get the time between the two signals */
                stat_u32Time21MotorSpeedDirection = TIM_GetCounter(MOTOR_DIRECTION_CHECK_TIMER);
                /* Reset the timer */
                TIM_SetCounter(MOTOR_DIRECTION_CHECK_TIMER,0u);                
            }
            else
            {
                if( stat_u8MotorSpeedDirectionNotOkCounter < 255u )
                {
                    stat_u8MotorSpeedDirectionNotOkCounter++;
                }
                /* Reset the timer */
                TIM_SetCounter(MOTOR_DIRECTION_CHECK_TIMER,0u);                
            }
                
            
            /* system order up */
            if( CMD_FLAG6 & 0x01u )
            {
                if( stat_u32Time12MotorSpeedDirection < stat_u32Time21MotorSpeedDirection )
                {
                    if( stat_u8MotorSpeedDirectionNotOkCounter < 255u )
                    {
                        stat_u8MotorSpeedDirectionNotOkCounter++;
                    }
                    stat_u8MotorSpeedDirectionOkCounter = 0u; 
                }
                else
                {
                    if( stat_u8MotorSpeedDirectionOkCounter < 255u )
                    {
                        stat_u8MotorSpeedDirectionOkCounter++;
                    }
                }
            }
            /* system order down */
            else
            {
                if( stat_u32Time12MotorSpeedDirection > stat_u32Time21MotorSpeedDirection )
                {
                    if( stat_u8MotorSpeedDirectionNotOkCounter < 255u )
                    {
                        stat_u8MotorSpeedDirectionNotOkCounter++;
                    }
                    stat_u8MotorSpeedDirectionOkCounter = 0u;                   
                }
                else
                {
                    if( stat_u8MotorSpeedDirectionOkCounter < 255u )
                    {
                        stat_u8MotorSpeedDirectionOkCounter++;
                    }
                }                
            }
            
            /* 5 consecutive pulses not ok, go to fault */
            if( stat_u8MotorSpeedDirectionNotOkCounter >= 5u )
            {
                /* 5 seconds after going to run state */
                if( ptMTR->TimerRuningTms > 1000u )
                {
                    /* Fault ¨C MOTOR UNINTENTIONAL CHANGE DIRECTION (F344) */
                    /* In this case the aux brake is lock. */
                    EN_ERROR44 |= 0x01u;                     
                }
                /* between 2 and 5 seconds after going to run state */
                else if(( ptMTR->TimerRuningTms >= 400u ) && ( ptMTR->TimerRuningTms <= 1000u ))
                {
                    /* Fault ¨C MOTOR WRONG DIRECTION (F335) */
                    EN_ERROR42 |= 0x80u;  
                }
                else
                {}
            }
            /* counter should be resetted after 10 consecutive pulses with ok */
            else if( stat_u8MotorSpeedDirectionOkCounter >= 10u )
            {
                stat_u8MotorSpeedDirectionNotOkCounter = 0u;
            }
            else
            {}
        }
        
        stat_u8LastMotorSensor = ptMTR->SensorX;
    }          
}


/*******************************************************************************
* Function Name  : Check_Stopping_Distance
* Description    : Stopping distance.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
void Check_Stopping_Distance(MotorSpeedItem* ptMTR, u16 EscStateOld)
{       
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
            stat_u16ShoppingDistance = (ptMTR->rt_brake_pulse) * CONVERSION_MAINSHAFTSPEED;            
        }
        
        if( stat_u16ShoppingDistance > stat_u16MaxDistace )
        {
            /* if no Fault: OVERSPEED  or Fault: LOW SPEED, Excessive distance FAULT */
            if( (!( EN_ERROR34 & 0x01u )) && (!( EN_ERROR34 & 0x02u )))
            {
                /* Excessive distance FAULT (F269) */  
                EN_ERROR34 |= 0x20u;
            }
        } 
    } 
    else
    {
        ptMTR->rt_brake_pulse = 0u;
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

    if(( SPEED_SENSOR_INSTALLATION == 0u ) || ( SPEED_SENSOR_INSTALLATION == 3u ))
    {
        /* esc turn to run state, init */
        if((SfBase_EscState == ESC_RUN_STATE) && (!(stat_u16EscStateOld == ESC_RUN_STATE))) 
        { 
            g_u8FirstMotorSpeedEdgeDetected = 0u;
            g_u8FirstMotorSpeedSensor1Detected = 0u;
            g_u8FirstMotorSpeedSensorCheck = 0u;
            g_u8SensorShortCircuitCheck = 0u;
            
            /* Reset the value when escalator run */
            MTRITEM[0].TimerRuningTms = 0u;
            MTRITEM[0].NotOkCounter = 0u;
            MTRITEM[0].OkCounter = 0u;
            MTRITEM[0].between_pulse_counter = 0u; 
            MTRITEM[0].LastMotorSpeedPulseTime = 0u;
            *MTRITEM[0].ptBrakeDistanceBuff = 0u;
            
            MTRITEM[1].TimerRuningTms = 0u;
            MTRITEM[1].NotOkCounter = 0u;
            MTRITEM[1].OkCounter = 0u;
            MTRITEM[1].between_pulse_counter = 0u; 
            MTRITEM[1].LastMotorSpeedPulseTime = 0u;
            *MTRITEM[1].ptBrakeDistanceBuff = 0u;
            
            /* Timer init, counter is 10us */
            TIM_Cmd(MTRITEM[0].TimerSensorNum, DISABLE);
            TIM_Cmd(MTRITEM[1].TimerSensorNum, DISABLE);
            MTRITEM[0].Timer_Init_Sensor(65535u, 719u);
            MTRITEM[1].Timer_Init_Sensor(65535u, 719u);
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
            }            
        }   
        
        
        Motor_Speed_Run_EN115(&MTRITEM[0]);
        Motor_Speed_Run_EN115(&MTRITEM[1]);
        
        Motor_Speed_Senor_Check();          
        
        Check_Stopping_Distance(&MTRITEM[0],stat_u16EscStateOld);
        Check_Stopping_Distance(&MTRITEM[1],stat_u16EscStateOld);
    }
    
    Sensor_Shortcircuit_Check();    
    
    stat_u16EscStateOld = SfBase_EscState; 
}



/******************************  END OF FILE  *********************************/






