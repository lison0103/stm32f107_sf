/*******************************************************************************
* File Name          : esc_missing_step.c
* Author             : lison
* Version            : V1.0
* Date               : 05/10/2016
* Last modify date   : 09/12/2016
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
#define    F2      (( NOMINAL_SPEED ) / ( STEP_WIDTH ))
#define    R       (( F1 ) * ( STEP_WIDTH ) / ( NOMINAL_SPEED ))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u8 g_u8FirstMissingStepSyncEntry = 0u;
static u8 g_u8FirstMissingStepEdgeDetected = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS);
static u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS);
static void Missing_StepRun(STEPMISSINGITEM* psSTPMS);
static void Missing_Step_UpperLower_SyncRun(void);
static void Missing_Step_Ready(STEPMISSINGITEM* psSTPMS);


STEPMISSINGITEM STPMS_UPPER=
{
    &EscRTBuff[64],
    0,
    {0,0},
    (u16*)&EscRTBuff[60],
    0,
    0,
    0,
    0
};

STEPMISSINGITEM STPMS_LOWER=
{
    &EscRTBuff[65],
    0,          
    {0,0},
    (u16*)&EscRTBuff[62],
    0,
    0,
    0,
    0
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
    /* System in Ready or 0Hz state */
    if( SfBase_EscState & ESC_READY_STATE ) 
    {                
        psSTPMS->ms_ready_delay++;
        
        if( psSTPMS->rising_edge_detected[0] == 1u )
        {
            psSTPMS->rising_edge_detected[0] = 0u;
  
            /* Missing Step pulse detected with freq > 1 Hz? */
            if( psSTPMS->ms_ready_delay * SYSTEMTICK < 1000u )
            {
                /* fault */ 
                *(psSTPMS->pcErrorCodeBuff) |= 0x01u;
                EN_ERROR3 |= 0x01u;
            }
            else
            {
                psSTPMS->ms_ready_delay = 0u;
            }
        }         
    } 
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
    /* System in run or stopping process state */
    if(( SfBase_EscState & ESC_RUN_STATE ) || ( SfBase_EscState & ESC_STOPPING_PROCESS_STATE ))
    {       
        psSTPMS->ms_ready_delay = 200u;
        
        /* First_entry_missing_step_detection == 0? */
        if( psSTPMS->First_entry_missing_step_detection == 0u )
        {
            psSTPMS->First_entry_missing_step_detection = 1u;
            psSTPMS->Motor_speed_pulse_counter_init = Pulse_counter_sensor_speed(psSTPMS);
        }
        
        psSTPMS->Motor_speed_pulse_counter =  Pulse_counter_sensor_speed(psSTPMS) - psSTPMS->Motor_speed_pulse_counter_init;
        
        /* Motor_speed_pulse_ counter > R*1.1? */
        if( psSTPMS->Motor_speed_pulse_counter > (( R * 11u ) / 10u ) )
        {
            if( SfBase_EscState & ESC_STATE_INSP )
            {     
                /* Missing step warning */

            }
            else
            {
                /* Missing step fault */
                *(psSTPMS->pcErrorCodeBuff) |= 0x02u;
                EN_ERROR3 |= 0x02u;
            }
        }
        else
        {
            /* Missing step signal rising edge detected ? */
            if( psSTPMS->rising_edge_detected[0] == 1u )
            {
                psSTPMS->rising_edge_detected[0] = 0u;
                
                psSTPMS->Motor_speed_pulse_counter =  Pulse_counter_sensor_speed(psSTPMS) - psSTPMS->Motor_speed_pulse_counter_init;
                
                /* Motor_speed_pulse_ counter > R*1.1? */
                if( psSTPMS->Motor_speed_pulse_counter > (( R * 11u ) / 10u ))
                {
                    if( SfBase_EscState & ESC_STATE_INSP )
                    {     
                        /* Missing step warning */

                    }
                    else
                    {
                        /* Missing step fault */
                        *(psSTPMS->pcErrorCodeBuff) |= 0x04u;
                        EN_ERROR3 |= 0x04u;
                    }
                }

                psSTPMS->MtrPulse = 0u;
                psSTPMS->Motor_speed_pulse_counter_init = Pulse_counter_sensor_speed(psSTPMS);                                                       
            }
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
    
    if(( SfBase_EscState & ESC_RUN_STATE ) || ( SfBase_EscState & ESC_STOPPING_PROCESS_STATE ))
    {  
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
                /* Fault */
                EN_ERROR5 |= 0x01u;         
            }
        }
        
        /* Upper Missing step no fault and Lower Missing step signal: rising edge detected ? */
        if( ( (u8)(EN_ERROR5 & 0x01u)  == 0u ) && ( STPMS_LOWER.rising_edge_detected[1] == 1u ) )
        {
            STPMS_LOWER.rising_edge_detected[1] = 0u;
            
            stat_u8LowerMissingStepCounter += 1u;
            stat_u8UpperMissingStepCounter = 0u;
            
            if( stat_u8LowerMissingStepCounter >= 2u )
            {
                /* Fault */
                EN_ERROR5 |= 0x02u;
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
    
    if( SfBase_EscState & ESC_RUN_STATE )
    {  
        if( g_u8FirstMissingStepEdgeDetected == 0u )
        {
            g_u8FirstMissingStepEdgeDetected = 1u;
            stat_u32TimerMissingStepShortcircuit = 0u;
            stat_u32MissingStepSCOkCounter = 0u;
            stat_u32MissingStepSCNotOkCounter = 0u;            
            TIM_Cmd(TIM7, DISABLE); 
            TIM7_Int_Init(65535u,71u);           
        }    
        else
        {
            /* Get the time between the two signals */
            stat_u32TimerMissingStepShortcircuit = TIM_GetCounter(TIM7);
            TIM_SetCounter(TIM7,0u);            
            
            if( stat_u32TimerMissingStepShortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                stat_u32TimerMissingStepShortcircuit = 0u;               
                stat_u32MissingStepSCNotOkCounter++;
                stat_u32MissingStepSCOkCounter = 0u;
                
                /* 100 consecutive pulses with less than SSM_SHORTCIRCUIT_TIME, go to fault */
                if( stat_u32MissingStepSCNotOkCounter >= 100u )
                {                
                    /* Fault ¨C Motorspeed Sensor shortcircuited */
                    EN_ERROR4 |= 0x04u;
                }
            }
            else
            {
                stat_u32TimerMissingStepShortcircuit = 0u;             
                stat_u32MissingStepSCOkCounter++;
                
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
* Function Name  : ESC_Missingstep_Check
* Description    : Esc Missingstep check, 5 ms enter one time.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Missingstep_Check(void)
{
    static u16 stat_u16EscStateOld = 0u; 
    
    Missing_Step_Ready(&STPMS_UPPER);
    Missing_Step_Ready(&STPMS_LOWER);		
    
    /* esc turn to run state, init */
    if((SfBase_EscState & ESC_RUN_STATE) && (!(stat_u16EscStateOld & ESC_RUN_STATE))) 
    { 
        STPMS_UPPER.First_entry_missing_step_detection = 0u;
        STPMS_LOWER.First_entry_missing_step_detection = 0u;
        g_u8FirstMissingStepSyncEntry = 0u;
        g_u8FirstMissingStepEdgeDetected = 0u;
    } 
    
    Missing_StepRun(&STPMS_UPPER);
    Missing_StepRun(&STPMS_LOWER);
    Missing_Step_UpperLower_SyncRun();
    
    stat_u16EscStateOld = SfBase_EscState;                                      
}

/******************************  END OF FILE  *********************************/






