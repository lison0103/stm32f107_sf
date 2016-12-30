/*******************************************************************************
* File Name          : esc_main_shaft_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 12/20/2016
* Last modify date   : 12/23/2016
* Description        : This file contains esc main shaft speed.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_main_shaft_speed.h"
#include "esc_motor_speed.h"
#include "delay.h"
#include "initial_devices.h"
#include "timer.h"
#include "esc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 g_u8FirstMainShaftSpeedSensorCheck = 0u;
u8 g_u8FirstMainShaftSpeedSensor1Detected = 0u;
u8 g_u8FirstMainSaftSpeedEdgeDetected = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void MainShaftSpeedEN115(MotorSpeedItem* ptMTR);
static void MainShaft_Speed_Senor_Check(void);


MotorSpeedItem MAINSHAFTITEM[3] =
{
    {
        /* mainshaft speed 1 */
        2u,
        1u,
        0u,
        0u,
        0u,    
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},                 
        &MAIN_SHAFT_SPEED1, 
        0u,
        0u,
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
        0u,
        0u,
        0u,
        0u,
        MAIN_SHAFT_SPEED1_CHECK_TIMER_INIT,
        MAIN_SHAFT_SPEED1_CHECK_TIMER, 
        
        /* brake distance */
        &STOPPING_DISTANCE1,
        0u,
        1u
    },
    {
        /* mainshaft speed 2 */
        2u,
        2u,
        0u,
        0u,
        0u,    
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u}, 
        &MAIN_SHAFT_SPEED2, 
        0u,
        0u,
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
        0u,
        0u,
        0u,
        0u,
        MAIN_SHAFT_SPEED1_CHECK_TIMER_INIT,
        MAIN_SHAFT_SPEED1_CHECK_TIMER,           
            
        /* brake distance */
        &STOPPING_DISTANCE2,
        0u,
        1u                     
    },
    {
        /* mainshaft speed 3 */
        2u,
        3u,
        0u,
        0u,
        0u,    
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u}, 
        &MAIN_SHAFT_SPEED3, 
        0u,
        0u,
        {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
        0u,
        0u,
        0u,
        0u,
        MAIN_SHAFT_SPEED3_CHECK_TIMER_INIT,
        MAIN_SHAFT_SPEED3_CHECK_TIMER,     
            
        /* brake distance */
        NULL,
        0u,
        1u                     
    }    
};

/*******************************************************************************
* Function Name  : MainShaftSpeedEN115
* Description    : The same to the Motor_Speed_Run_EN115()
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void MainShaftSpeedEN115(MotorSpeedItem* ptMTR)
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
                    /* Main shaft speed sensor 1 timeout stopping process F420 */
                    EN_ERROR53 |= 0x10u;
                }
                else if( ptMTR->SensorX == 2u )
                {
                    /* Main shaft speed sensor 1 timeout stopping process F421 */
                    EN_ERROR53 |= 0x20u;                
                }    
                else if( ptMTR->SensorX == 3u )
                {
                    /* Main shaft speed sensor 1 timeout stopping process F422 */
                    EN_ERROR53 |= 0x40u;                
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
                if( ptMTR->TimerMotorSpeedBetweenPulse[i] < ( 100000u / MAX_SPEED_MAINSHAFT ))
                {
                    if( ptMTR->NotOkCounter < 0xffu )
                    {
                        ptMTR->NotOkCounter++;
                    }
                    ptMTR->OkCounter = 0u;
                    if( ptMTR->NotOkCounter >= 5u )
                    {
                        /* Fault: mainshaft overspeed (F256) */
                        EN_ERROR33 |= 0x01u;                                                 
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
                            if( ptMTR->TimerMotorSpeedBetweenPulse[i] > ( 100000u / MIN_SPEED_MAINSHAFT ))
                            {
                                /* Fault: mainshaft underspeed (F257) */
                                EN_ERROR33 |= 0x02u;                        
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
                    if(( ptMTR->TimerNoPulse * SYSTEMTICK ) > ( 1000u / MIN_SPEED_MAINSHAFT )) /* ms */ 
                    {
                        /* Fault: mainshaft underspeed (F257) */
                        EN_ERROR33 |= 0x02u;                        
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
* Function Name  : MainShaftWrongDirection
* Description    : enable only parameter SPEED_SENSOR_INSTALLATION = 1,2,3
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void MainShaftWrongDirection(MotorSpeedItem* ptMTR)
{
    static u32 stat_u32Time12MotorSpeedDirection = 0u;
    static u32 stat_u32Time21MotorSpeedDirection = 0u;
    static u8 stat_u8MotorSpeedDirectionOkCounter = 0u;
    static u8 stat_u8MotorSpeedDirectionNotOkCounter = 0u;
    static u8 stat_u8LastMotorSensor = 0u;
    
    /* System in run state */
    if(( SfBase_EscState == ESC_RUN_STATE ) || ( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ) || ( SfBase_EscState == ESC_FAULT_STATE ))
    {  
        if( g_u8FirstMainShaftSpeedSensor1Detected == 0u )
        {
            g_u8FirstMainShaftSpeedSensor1Detected = 1u;
            stat_u32Time12MotorSpeedDirection = 0u;
            stat_u32Time21MotorSpeedDirection = 0u;
            stat_u8MotorSpeedDirectionOkCounter = 0u;
            stat_u8MotorSpeedDirectionNotOkCounter = 0u; 
            stat_u8LastMotorSensor = ptMTR->SensorX;
            
            /* Timer init, counter is 10 us */
            TIM_Cmd(MAIN_SHAFT_DIRECTION_TIMER, DISABLE);
            TIM5_Int_Init(65535u, 719u);            
        }    
        else
        {
            if(( ptMTR->SensorX == 2u ) & ( stat_u8LastMotorSensor == 1u ))
            {
                /* Get the time between the two signals */
                stat_u32Time12MotorSpeedDirection = TIM_GetCounter(MAIN_SHAFT_DIRECTION_TIMER);
                /* Reset the timer */
                TIM_SetCounter(MAIN_SHAFT_DIRECTION_TIMER,0u);
            }
            else if(( ptMTR->SensorX == 1u ) & ( stat_u8LastMotorSensor == 2u ))
            {
                /* Get the time between the two signals */
                stat_u32Time21MotorSpeedDirection = TIM_GetCounter(MAIN_SHAFT_DIRECTION_TIMER);
                /* Reset the timer */
                TIM_SetCounter(MAIN_SHAFT_DIRECTION_TIMER,0u);                
            }
            else
            {
                if( stat_u8MotorSpeedDirectionNotOkCounter < 255u )
                {
                    stat_u8MotorSpeedDirectionNotOkCounter++;
                }
                /* Reset the timer */
                TIM_SetCounter(MAIN_SHAFT_DIRECTION_TIMER,0u);                
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
                    /* Fault 每 MAINSHAFT UNINTENTIONAL CHANGE DIRECTION (F343) */
                    /* In this case the aux brake is lock. */
                    EN_ERROR43 |= 0x80u;                     
                }
                /* between 2 and 5 seconds after going to run state */
                else if(( ptMTR->TimerRuningTms >= 400u ) && ( ptMTR->TimerRuningTms <= 1000u ))
                {
                    /* Fault 每 MAINSHAFT WRONG DIRECTION (F423) */
                    EN_ERROR53 |= 0x80u;  
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
* Function Name  : MainShaft_Speed_Shortcircuit_Run
* Description    : Motor speed sensor 1 and 2 shortcircuit checking
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void MainShaft_Speed_Shortcircuit_Run(void)
{
    static u32 stat_u32TimerMotorSpeedShortCircuit = 0u;
    static u32 stat_u32MotorSpeedShortCircuitOkCounter = 0u;
    static u32 stat_u32MotorSpeedShortCircuitNotOkCounter = 0u;
   
    /* System in run state */
    if( SfBase_EscState == ESC_RUN_STATE )
    {  
        if( g_u8FirstMainSaftSpeedEdgeDetected == 0u )
        {
            g_u8FirstMainSaftSpeedEdgeDetected = 1u;
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
                    if( SPEED_SENSOR_INSTALLATION == 1u )
                    {
                        /* Fault 每 Shortcircuit main shaft speed sensor 1+2 (F417) */
                        EN_ERROR53 |= 0x02u;  
                    }
                    else if( SPEED_SENSOR_INSTALLATION == 2u )
                    {
                        /* check main shaft 1-2 */
                        if( g_u8SensorShortCircuitCheck == 1u )
                        {
                            /* Fault 每 Shortcircuit main shaft speed sensor 1+2 (F417) */
                            EN_ERROR53 |= 0x02u; 
                        }
                        /* check main shaft 1-3 */
                        else if( g_u8SensorShortCircuitCheck == 2u )
                        {
                            /* Fault 每 Shortcircuit main shaft speed sensor 1+3 (F418) */
                            EN_ERROR53 |= 0x04u;                         
                        }
                        /* check main shaft 2-3 */
                        else if( g_u8SensorShortCircuitCheck == 3u )
                        {
                            /* Fault 每 Shortcircuit main shaft speed sensor 2+3 (F419) */
                            EN_ERROR53 |= 0x08u;                         
                        }
                        else
                        {}
                    }
                    else if( SPEED_SENSOR_INSTALLATION == 3u )
                    {
                        /* Fault 每 Shortcircuit main shaft speed sensor 1+2 (F417) */
                        EN_ERROR53 |= 0x02u;                          
                    }
                    else
                    {}
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
* Function Name  : MainShaft_Speed_Senor_Check
* Description    : Check the difference in 2 or 3 speed sensors.
*                  Check the difference in 2 or 3 speed sensors between micro.     
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void MainShaft_Speed_Senor_Check(void)
{
    static u16 stat_u16Timer1MotorSpeedSensor = 0u;
    static u16 stat_u16Timer2MotorSpeedSensor = 0u;
    static u16 stat_u16Timer3MotorSpeedSensor = 0u;
    static u16 stat_u16Timer1Cpu2MotorSpeedSensor = 0u;
    static u16 stat_u16Timer2Cpu2MotorSpeedSensor = 0u;
    static u16 stat_u16Timer3Cpu2MotorSpeedSensor = 0u;
    u16 u16MotorSpeedSensorDiff = 0u;

     /* System in run state */
    if( SfBase_EscState == ESC_RUN_STATE )
    {    
        if( g_u8FirstMainShaftSpeedSensorCheck == 0u )
        {
            g_u8FirstMainShaftSpeedSensorCheck = 1u;
            stat_u16Timer1MotorSpeedSensor = 0u;
            stat_u16Timer2MotorSpeedSensor = 0u;
            stat_u16Timer3MotorSpeedSensor = 0u;
            stat_u16Timer1Cpu2MotorSpeedSensor = 0u;
            stat_u16Timer2Cpu2MotorSpeedSensor = 0u;  
            stat_u16Timer3Cpu2MotorSpeedSensor = 0u; 
        } 
        
        /* The safety board goes to fault when the difference 
        in both speed sensors is more than 5% for more than 1 second */    
        /* main shaft speed 1-2 */
        if( MAIN_SHAFT_SPEED1 > MAIN_SHAFT_SPEED2 )
        {
            u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED1 - MAIN_SHAFT_SPEED2 ) * 100u / MAIN_SHAFT_SPEED2;
        }
        else if( MAIN_SHAFT_SPEED1 < MAIN_SHAFT_SPEED2 )
        {
            u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED2 - MAIN_SHAFT_SPEED1 ) * 100u / MAIN_SHAFT_SPEED1;
        }
        else
        {
            stat_u16Timer1MotorSpeedSensor = 0u;
        }
        
        if( u16MotorSpeedSensorDiff > 5u )
        {
            if( stat_u16Timer1MotorSpeedSensor < 0xffffu )
            {
                stat_u16Timer1MotorSpeedSensor++;
            }
            if(( stat_u16Timer1MotorSpeedSensor * SYSTEMTICK ) > 1000u )
            {
                /* Discrepance main shaft speed sensor 1-2 F411*/
                EN_ERROR52 |= 0x08u;
            }
        }           
        
        /* 3 MAIN SHAFT SENSOR */
        if( SPEED_SENSOR_INSTALLATION == 2u )
        {
            /* main shaft speed 2-3 */
            if( MAIN_SHAFT_SPEED2 > MAIN_SHAFT_SPEED3 )
            {
                u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED2 - MAIN_SHAFT_SPEED3 ) * 100u / MAIN_SHAFT_SPEED3;
            }
            else if( MAIN_SHAFT_SPEED2 < MAIN_SHAFT_SPEED3 )
            {
                u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED3 - MAIN_SHAFT_SPEED2 ) * 100u / MAIN_SHAFT_SPEED2;
            }
            else
            {
                stat_u16Timer2MotorSpeedSensor = 0u;
            }
            
            if( u16MotorSpeedSensorDiff > 5u )
            {
                if( stat_u16Timer2MotorSpeedSensor < 0xffffu )
                {
                    stat_u16Timer2MotorSpeedSensor++;
                }
                if(( stat_u16Timer2MotorSpeedSensor * SYSTEMTICK ) > 1000u )
                {
                    /* Discrepance main shaft speed sensor 2-3 F412 */
                    EN_ERROR52 |= 0x10u;
                }
            }
            
            /* main shaft speed 1-3 */
            if( MAIN_SHAFT_SPEED1 > MAIN_SHAFT_SPEED3 )
            {
                u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED1 - MAIN_SHAFT_SPEED3 ) * 100u / MAIN_SHAFT_SPEED3;
            }
            else if( MAIN_SHAFT_SPEED1 < MAIN_SHAFT_SPEED3 )
            {
                u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED3 - MAIN_SHAFT_SPEED1 ) * 100u / MAIN_SHAFT_SPEED1;
            }
            else
            {
                stat_u16Timer3MotorSpeedSensor = 0u;
            }
            
            if( u16MotorSpeedSensorDiff > 5u )
            {
                if( stat_u16Timer3MotorSpeedSensor < 0xffffu )
                {
                    stat_u16Timer3MotorSpeedSensor++;
                }
                if(( stat_u16Timer3MotorSpeedSensor * SYSTEMTICK ) > 1000u )
                {
                    /* Discrepance main shaft speed sensor 1-3 F413 */
                    EN_ERROR52 |= 0x20u;
                }
            }        
        }
        
        /* The safety board goes to fault when the difference in both speed 
        sensors between microprocessors is more than 5% for more than 1 second */
        
        /* wait the speed stable */
        if( ( MTRITEM[0].TimerRuningTms * SYSTEMTICK ) > UNDERSPEED_TIME )
        {
            /* Mainshaft speed sensor 1 */
            if( OMC_MAIN_SHAFT_SPEED1 > MAIN_SHAFT_SPEED1 )
            {
                u16MotorSpeedSensorDiff = ( OMC_MAIN_SHAFT_SPEED1 - MAIN_SHAFT_SPEED1 ) * 100u / MAIN_SHAFT_SPEED1;
            }
            else if( OMC_MAIN_SHAFT_SPEED1 < MAIN_SHAFT_SPEED1 )
            {
                u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED1 - OMC_MAIN_SHAFT_SPEED1 ) * 100u / OMC_MAIN_SHAFT_SPEED1;
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
                    /* Discrepance main shaft 1 speed CPU F414 */
                    EN_ERROR52 |= 0x40u;
                }
            }            
            
            /* Mainshaft speed sensor 2 */
            if( OMC_MAIN_SHAFT_SPEED2 > MAIN_SHAFT_SPEED2 )
            {
                u16MotorSpeedSensorDiff = ( OMC_MAIN_SHAFT_SPEED2 - MAIN_SHAFT_SPEED2 ) * 100u / MAIN_SHAFT_SPEED2;
            }
            else if( OMC_MOTOR_SPEED2 < MAIN_SHAFT_SPEED2 )
            {
                u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED2 - OMC_MAIN_SHAFT_SPEED2 ) * 100u / OMC_MAIN_SHAFT_SPEED2;
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
                    /* Discrepance main shaft 2 speed CPU F415 */
                    EN_ERROR52 |= 0x80u;
                }
            }
            
            /* 3 MAIN SHAFT SENSOR */
            if( SPEED_SENSOR_INSTALLATION == 2u )
            {
                /* Mainshaft speed sensor 3 */
                if( OMC_MAIN_SHAFT_SPEED3 > MAIN_SHAFT_SPEED3 )
                {
                    u16MotorSpeedSensorDiff = ( OMC_MAIN_SHAFT_SPEED3 - MAIN_SHAFT_SPEED3 ) * 100u / MAIN_SHAFT_SPEED3;
                }
                else if( OMC_MAIN_SHAFT_SPEED3 < MAIN_SHAFT_SPEED3 )
                {
                    u16MotorSpeedSensorDiff = ( MAIN_SHAFT_SPEED3 - OMC_MAIN_SHAFT_SPEED3 ) * 100u / OMC_MAIN_SHAFT_SPEED3;
                }
                else
                {
                    stat_u16Timer3Cpu2MotorSpeedSensor = 0u;
                } 

                if( u16MotorSpeedSensorDiff > 5u )
                {
                    if( stat_u16Timer3Cpu2MotorSpeedSensor < 0xffffu )
                    {
                        stat_u16Timer3Cpu2MotorSpeedSensor++;
                    }
                    if(( stat_u16Timer3Cpu2MotorSpeedSensor * SYSTEMTICK ) > 1000u )
                    {
                        /* Discrepance main shaft 3 speed CPU F416 */
                        EN_ERROR53 |= 0x01u;
                    }
                }                
            }                
        }       
    }
}

/*******************************************************************************
* Function Name  : ESC_Mainshaft_Check
* Description    : Esc mainshaft speed and brake distance check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Mainshaft_Check(void)
{
    static u16 stat_u16EscStateOld = 0u; 
 
    if(( SPEED_SENSOR_INSTALLATION == 1u ) || ( SPEED_SENSOR_INSTALLATION == 2u ) || ( SPEED_SENSOR_INSTALLATION == 3u ))
    {
        /* esc turn to run state, init */
        if((SfBase_EscState == ESC_RUN_STATE) && (!(stat_u16EscStateOld == ESC_RUN_STATE))) 
        {             
            g_u8FirstMainSaftSpeedEdgeDetected = 0u;
            g_u8FirstMainShaftSpeedSensorCheck = 0u;
            g_u8FirstMainShaftSpeedSensor1Detected = 0u;
            if(( SPEED_SENSOR_INSTALLATION == 1u ) || ( SPEED_SENSOR_INSTALLATION == 2u ))
            {
                g_u8SensorShortCircuitCheck = 0u;
            }
            /* Reset the value when escalator run */
            MAINSHAFTITEM[0].TimerRuningTms = 0u;
            MAINSHAFTITEM[0].NotOkCounter = 0u;
            MAINSHAFTITEM[0].OkCounter = 0u;
            MAINSHAFTITEM[0].between_pulse_counter = 0u; 
            *MAINSHAFTITEM[0].ptBrakeDistanceBuff = 0u;
            
            MAINSHAFTITEM[1].TimerRuningTms = 0u;
            MAINSHAFTITEM[1].NotOkCounter = 0u;
            MAINSHAFTITEM[1].OkCounter = 0u;
            MAINSHAFTITEM[1].between_pulse_counter = 0u; 
            *MAINSHAFTITEM[1].ptBrakeDistanceBuff = 0u;
            
            MAINSHAFTITEM[2].TimerRuningTms = 0u;
            MAINSHAFTITEM[2].NotOkCounter = 0u;
            MAINSHAFTITEM[2].OkCounter = 0u;
            MAINSHAFTITEM[2].between_pulse_counter = 0u; 
            *MAINSHAFTITEM[2].ptBrakeDistanceBuff = 0u;        
            
            /* Timer init, counter is 10us */
            TIM_Cmd(MAINSHAFTITEM[0].TimerSensorNum, DISABLE);
            TIM_Cmd(MAINSHAFTITEM[1].TimerSensorNum, DISABLE);
            TIM_Cmd(MAINSHAFTITEM[2].TimerSensorNum, DISABLE);
            MAINSHAFTITEM[0].Timer_Init_Sensor(65535u, 719u);
            MAINSHAFTITEM[1].Timer_Init_Sensor(65535u, 719u);
            MAINSHAFTITEM[2].Timer_Init_Sensor(65535u, 719u);
        } 
        else
        {   
            /* esc enter stopping process state, init */
            if(( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ) && ( !(stat_u16EscStateOld == ESC_STOPPING_PROCESS_STATE)))
            {
                MAINSHAFTITEM[0].rt_brake_stop = 0u;
                MAINSHAFTITEM[1].rt_brake_stop = 0u;
                MAINSHAFTITEM[2].rt_brake_stop = 0u;
                
                /* Restart: TimeoutTimerMotorSpeed */
                MAINSHAFTITEM[0].TimeoutTimerMotorSpeed = 0u;
                MAINSHAFTITEM[1].TimeoutTimerMotorSpeed = 0u;  
                MAINSHAFTITEM[2].TimeoutTimerMotorSpeed = 0u;
            }            
        }   
        
        MainShaftSpeedEN115(&MAINSHAFTITEM[0]);
        MainShaftSpeedEN115(&MAINSHAFTITEM[1]);  
        if( SPEED_SENSOR_INSTALLATION == 2u )
        {        
            MainShaftSpeedEN115(&MAINSHAFTITEM[2]);
        }
        
        if(( SPEED_SENSOR_INSTALLATION == 1u ) || ( SPEED_SENSOR_INSTALLATION == 2u ))
        {
            Check_Stopping_Distance(&MAINSHAFTITEM[0],stat_u16EscStateOld);
            Check_Stopping_Distance(&MAINSHAFTITEM[1],stat_u16EscStateOld);
        }
        
        MainShaft_Speed_Senor_Check();
          
        stat_u16EscStateOld = SfBase_EscState; 
    }
    
    /* check escalator stop */
    if( SfBase_EscState == ESC_STOPPING_PROCESS_STATE )
    {
        if( SPEED_SENSOR_INSTALLATION == 0u )
        {
            /* Escalator speed pulse freqency < 1Hz */
            if( (MTRITEM[0].rt_brake_stop == 1u) && (MTRITEM[1].rt_brake_stop == 1u) )
            {  
                g_u8EscStoppingFinish = 1u;
            }
        }
        else if(( SPEED_SENSOR_INSTALLATION == 1u ) || ( SPEED_SENSOR_INSTALLATION == 2u ))
        {
            /* Escalator speed pulse freqency < 1Hz */
            if( (MAINSHAFTITEM[0].rt_brake_stop == 1u) && (MAINSHAFTITEM[1].rt_brake_stop == 1u) )
            {  
                g_u8EscStoppingFinish = 1u;
            }            
        }
        else if( SPEED_SENSOR_INSTALLATION == 3u )
        {
            /* Escalator speed pulse freqency < 1Hz */
            if( (MTRITEM[0].rt_brake_stop == 1u) && (MTRITEM[1].rt_brake_stop == 1u) 
               && (MAINSHAFTITEM[0].rt_brake_stop == 1u) && (MAINSHAFTITEM[1].rt_brake_stop == 1u))
            {  
                g_u8EscStoppingFinish = 1u;
            }           
        }
        else
        {}    
    }
    else
    {
        g_u8EscStoppingFinish = 0u;
    }         
}



/******************************  END OF FILE  *********************************/






