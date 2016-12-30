/*******************************************************************************
* File Name          : esc_missing_step.c
* Author             : lison
* Version            : V1.0
* Date               : 05/10/2016
* Last modify date   : 12/09/2016
* Description        : This file contains esc missing step check.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_missing_step.h"
#include "timer.h"
#include "esc_motor_speed.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define    F2           (( (u16)NOMINAL_SPEED * 10u ) / ( STEP_WIDTH ))
#define    R_MS         (( F1 ) / ( F2 ))

#define MISSINGSTEP_LOWER_INPUT   (INPUT_PORT1_8 & INPUT_PORT5_MASK)
#define MISSINGSTEP_UPPER_INPUT   (INPUT_PORT1_8 & INPUT_PORT6_MASK)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u8 g_u8FirstMissingStepSyncEntry = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS);
static u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS);
static void Missing_StepRun(STEPMISSINGITEM* psSTPMS);
static void Missing_Step_UpperLower_SyncRun(void);
static void Missing_Step_Ready(STEPMISSINGITEM* psSTPMS);
static void MissingStep_RisingEdge_check(void);

u8 g_u8FirstMissingStepEdgeDetected = 0u;

STEPMISSINGITEM STPMS_UPPER=
{
    1u,
    0u,
    {0u,0u},
    &MISSINGSTEP_MTR_PULSE1,
    200u,
    0u,
    0u
};

STEPMISSINGITEM STPMS_LOWER=
{
    2u,
    0u,          
    {0u,0u},
    &MISSINGSTEP_MTR_PULSE2,
    200u,
    0u,
    0u
};


/*******************************************************************************
* Function Name  : Missing_Step_Ready
* Description    : Check the escalator missing step ready.
* Input          : psSTPMS: missingstep speed sensor id           
* Output         : None
* Return         : None
*******************************************************************************/
static void Missing_Step_Ready(STEPMISSINGITEM* psSTPMS)
{      
    /* prevent overflow */
    if( psSTPMS->ms_ready_delay < 10000u )
    {
        psSTPMS->ms_ready_delay++;
    }

    if( psSTPMS->rising_edge_detected[0] == 1u )
    {
        psSTPMS->rising_edge_detected[0] = 0u;
        
        /* Missing Step pulse detected with freq > 1 Hz? */
        if( psSTPMS->ms_ready_delay * SYSTEMTICK < 1000u )
        {
            if(!( CMD_ESC_RUN_MODE & ESC_NORMAL ))
            {     
                /* Missing Step Warning (W60) */
                EN_WARN8 |= 0x10u;
            }
            else
            {
                if( psSTPMS->SensorX == 1u )
                {
                    /* Missing step supervision top F50 */
                    EN_ERROR7 |= 0x04u;  
                }
                else if( psSTPMS->SensorX == 2u )
                {
                    /* Missing step supervision bottom F51 */
                    EN_ERROR7 |= 0x08u;
                }
                else
                {}
            }
        }
        else
        {
            psSTPMS->ms_ready_delay = 0u;
        }
    }  
    
    /* missing step (pulse of motor) */
    *psSTPMS->ptStepMtrBuff = 0u;     
}

/*****************************************************************************
* Function Name  : Missing_StepRun
* Description    : Check the escalator missing step run.
* Input          : psSTPMS: missingstep speed sensor id             
* Output         : None
* Return         : None
*****************************************************************************/
static void Missing_StepRun(STEPMISSINGITEM* psSTPMS)
{
    
    /* First_entry_missing_step_detection == 0? */
    if( psSTPMS->First_entry_missing_step_detection == 0u )
    {
        psSTPMS->First_entry_missing_step_detection = 1u;
        psSTPMS->MtrPulse = 0u;
    }
    
    psSTPMS->Motor_speed_pulse_counter =  Pulse_counter_sensor_speed(psSTPMS);
    
    /* Motor_speed_pulse_ counter > R*1.1? */
    if( psSTPMS->Motor_speed_pulse_counter > (( R_MS * 11u ) / 10u ) )
    {
        if(!( CMD_ESC_RUN_MODE & ESC_NORMAL ))
        {     
            /* Missing Step Warning (W60) */
            EN_WARN8 |= 0x10u;
        }
        else
        {
            /* Missing Step Fault (F50, F51) */
            if( psSTPMS->SensorX == 1u )
            {
                /* Missing step supervision top F50 */
                EN_ERROR7 |= 0x04u;  
            }
            else if( psSTPMS->SensorX == 2u )
            {
                /* Missing step supervision bottom F51 */
                EN_ERROR7 |= 0x08u; 
            }
            else
            {}
        }
    }
    else
    {
        /* Missing step signal rising edge detected ? */
        if( psSTPMS->rising_edge_detected[0] == 1u )
        {
            psSTPMS->rising_edge_detected[0] = 0u;            
            
            psSTPMS->MtrPulse = 0u;
            
            /* missing step (pulse of motor) */
            *psSTPMS->ptStepMtrBuff = psSTPMS->Motor_speed_pulse_counter;            
        }
    }       
}

/*******************************************************************************
* Function Name  : Pulse_counter_sensor_speed
* Description    : 
* Input          : psSTPMS: missingstep speed sensor id             
* Output         : None
* Return         : None
*******************************************************************************/
static u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS)
{
    u16 u16CurrentPulseCounterSensor = 0u;    
    
    u16CurrentPulseCounterSensor = psSTPMS->MtrPulse;
    
    return u16CurrentPulseCounterSensor;
}


/*******************************************************************************
* Function Name  : Missing_Step_UpperLower_SyncRun
* Description    : Missing step upper lower syncronization checking
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Missing_Step_UpperLower_SyncRun(void)
{
    static u8 stat_u8UpperMissingStepCounter = 0u,stat_u8LowerMissingStepCounter = 0u;    
     
    if( g_u8FirstMissingStepSyncEntry == 0u )
    {
        g_u8FirstMissingStepSyncEntry = 1u;
        stat_u8UpperMissingStepCounter = 0u;
        stat_u8LowerMissingStepCounter = 0u;
    } 
    
    /* Upper Missing step signal: rising edge detected ? */
    if( STPMS_UPPER.rising_edge_detected[1] == 1u )
    {
        STPMS_UPPER.rising_edge_detected[1] = 0u;
        
        stat_u8UpperMissingStepCounter += 1u;
        stat_u8LowerMissingStepCounter = 0u;
        
        if( stat_u8UpperMissingStepCounter >= 2u )
        {            
            if(!( CMD_ESC_RUN_MODE & ESC_NORMAL ))
            {     
                /* Missingstep Sync Warning top W58 */
                EN_WARN8 |= 0x04u;
            }
            else
            {
                /* Missing step supervision top F50 */
                EN_ERROR7 |= 0x04u;  
            }
        }
    }
    
    /* Lower Missing step signal: rising edge detected ? */
    if( STPMS_LOWER.rising_edge_detected[1] == 1u )
    {
        STPMS_LOWER.rising_edge_detected[1] = 0u;
        
        stat_u8LowerMissingStepCounter += 1u;
        stat_u8UpperMissingStepCounter = 0u;
        
        if( stat_u8LowerMissingStepCounter >= 2u )
        {
            if(!( CMD_ESC_RUN_MODE & ESC_NORMAL ))
            {     
                /* Missingstep sync warning bottom W59 */
                EN_WARN8 |= 0x08u;
            }
            else
            {
                /* Missing step supervision bottom F51 */
                EN_ERROR7 |= 0x08u; 
            }
        }
    }
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
    static u32 stat_u32TimerMissingStepShortcircuit = 0u;
    static u32 stat_u32MissingStepSCOkCounter = 0u;
    static u32 stat_u32MissingStepSCNotOkCounter = 0u;    
    
    /* System in run state */
    if( SfBase_EscState == ESC_RUN_STATE )
    {

        if( g_u8FirstMissingStepEdgeDetected == 0u )
        {
            g_u8FirstMissingStepEdgeDetected = 1u;
            stat_u32TimerMissingStepShortcircuit = 0u;
            stat_u32MissingStepSCOkCounter = 0u;
            stat_u32MissingStepSCNotOkCounter = 0u;  
            
            /* Timer init, counter is 100us */
            TIM_Cmd(SENSOR_SHORTCIRCUIT_CHECK_TIMER, DISABLE); 
            TIM6_Int_Init(65535u,7199u);           
        }    
        else
        {
            /* Get the time between the two signals */
            stat_u32TimerMissingStepShortcircuit = TIM_GetCounter(SENSOR_SHORTCIRCUIT_CHECK_TIMER);
            /* Reset the timer */
            TIM_SetCounter(SENSOR_SHORTCIRCUIT_CHECK_TIMER,0u);            
            
            if(( stat_u32TimerMissingStepShortcircuit * 100u ) < PULSE_SIGNALS_MINIMUM_LAG )
            {
                stat_u32TimerMissingStepShortcircuit = 0u;  
                if( stat_u32MissingStepSCNotOkCounter < 0xffffu )
                {
                    stat_u32MissingStepSCNotOkCounter++;
                }
                stat_u32MissingStepSCOkCounter = 0u;
                
                /* 100 consecutive pulses with less than SSM_SHORTCIRCUIT_TIME, go to fault */
                if( stat_u32MissingStepSCNotOkCounter >= 100u )
                {                
                    if(!( CMD_ESC_RUN_MODE & ESC_NORMAL ))
                    {     
                        /* Missing step warning while inspection */
                        EN_WARN8 |= 0x10u;
                    }
                    else
                    {
                        /* Fault ¨C MS Sensor shortcircuited (F263) */
                        EN_ERROR33 |= 0x80u;  
                    }
                }
            }
            else
            {
                stat_u32TimerMissingStepShortcircuit = 0u;   
                if( stat_u32MissingStepSCOkCounter < 0xffffu )
                {
                    stat_u32MissingStepSCOkCounter++;
                }
                /* counter should be resetted after 10 consecutive different pulses with more than SSM_SHORTCIRCUIT_TIME */
                if( stat_u32MissingStepSCOkCounter >= 10u )
                {
                    stat_u32MissingStepSCNotOkCounter = 0u;                   
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name  : MissingStep_RisingEdge_check
* Description    : Missing step rising edge check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
static void MissingStep_RisingEdge_check(void)
{
    static u8 stat_u8MissingStepLowerInputPre = 1u,stat_u8MissingStepUpperInputPre = 1u;
        
    if( stat_u8MissingStepLowerInputPre == 0u )
    {
        if( MISSINGSTEP_LOWER_INPUT )
        {
            stat_u8MissingStepLowerInputPre = 1u;     
            STPMS_LOWER.rising_edge_detected[0] = 1u;
            STPMS_LOWER.rising_edge_detected[1] = 1u;
            
        }
    }
    else if( stat_u8MissingStepLowerInputPre == 1u )
    {
        if( !MISSINGSTEP_LOWER_INPUT )
        {
            stat_u8MissingStepLowerInputPre = 0u;
        }
    }
    else
    {}
    
    if( stat_u8MissingStepUpperInputPre == 0u )
    {
        if( MISSINGSTEP_UPPER_INPUT )
        {
            stat_u8MissingStepUpperInputPre = 1u;
            STPMS_UPPER.rising_edge_detected[0] = 1u;
            STPMS_UPPER.rising_edge_detected[1] = 1u; 
        }
    }
    else if( stat_u8MissingStepUpperInputPre == 1u )
    {
        if( !MISSINGSTEP_UPPER_INPUT )
        {
            stat_u8MissingStepUpperInputPre = 0u;
        }
    }
    else
    {}    
}

/*******************************************************************************
* Function Name  : ESC_Missingstep_Check
* Description    : Esc Missingstep check, 5 ms enter one time.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Missingstep_Check(void)
{
    static u16 stat_u16EscStateOld = 0u; 
    
    MissingStep_RisingEdge_check();
    
    /* esc turn to run state, init */
    if((SfBase_EscState == ESC_RUN_STATE) && (!(stat_u16EscStateOld == ESC_RUN_STATE))) 
    { 
        /* Reset the value when escalator run */
        STPMS_UPPER.rising_edge_detected[0] = 0u;
        STPMS_UPPER.rising_edge_detected[1] = 0u;        
        STPMS_UPPER.ms_ready_delay = 200u;
        STPMS_UPPER.First_entry_missing_step_detection = 0u;

        STPMS_LOWER.rising_edge_detected[0] = 0u;
        STPMS_LOWER.rising_edge_detected[1] = 0u;        
        STPMS_LOWER.ms_ready_delay = 200u;
        STPMS_LOWER.First_entry_missing_step_detection = 0u;
        
        g_u8FirstMissingStepSyncEntry = 0u;
        g_u8FirstMissingStepEdgeDetected = 0u;

    } 
    
    /* System in run or stopping process state */
    if(( SfBase_EscState == ESC_RUN_STATE ) || ( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ))
    { 
        Missing_StepRun(&STPMS_UPPER);
        Missing_StepRun(&STPMS_LOWER);
        Missing_Step_UpperLower_SyncRun();
    }
    else /* System in Ready or 0Hz state , except run and stopping process state */
    {
        Missing_Step_Ready(&STPMS_UPPER);
        Missing_Step_Ready(&STPMS_LOWER);        
    }
    
    stat_u16EscStateOld = SfBase_EscState;                                      
}

/******************************  END OF FILE  *********************************/






