/*******************************************************************************
* File Name          : esc_handrail_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 09/20/2016
* Description        : This file contains esc handrail speed.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_handrail_speed.h"
#include "esc.h"
#include "esc_motor_speed.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HANDRAIL_PULSE_NUMBER   4u

/* Private macro -------------------------------------------------------------*/
#define Handrail_speed_Max_pulse     ( ( HANDRAIL_MOTOR_PULSE * 113u ) / 100u )
#define Handrail_speed_Min_pulse     ( ( HANDRAIL_MOTOR_PULSE * 87u ) / 100u )

/* Private variables ---------------------------------------------------------*/
static u8 g_u8FirstHandrailSpeedEdgeDetected = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void HR_Speed_Ready(HandrailSpeedItem* psHDL);
static void HR_Speed_Run_EN115(HandrailSpeedItem* psHDL);


HandrailSpeedItem HDL_Left = 
{ 
    200u,
    0u,
    0u,
    0u,
    {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
    0u,
    (u16*)&EscRTBuff[50],
    &EscRTBuff[54]
};

HandrailSpeedItem HDL_Right = 
{    
    200u,
    0u,
    0u,
    0u,
    {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
    0u,
    (u16*)&EscRTBuff[52],
    &EscRTBuff[55]
};


/*******************************************************************************
* Function Name  : HR_Speed_Ready
* Description    : Pulse detection when escalator is stopped
* Input          : psHDL: handrail speed sensor id          
* Output         : None
* Return         : None
*******************************************************************************/
static void HR_Speed_Ready(HandrailSpeedItem* psHDL)
{                          
    
    psHDL->hdl_pulse_tms++;
    
    if( psHDL->rising_edge_detected == 1u )
    {
        psHDL->rising_edge_detected = 0u;
        
        /* Handrail pulse detected with freq > 1 Hz? */
        if( psHDL->hdl_pulse_tms * SYSTEMTICK < 1000u )
        {
            /* fault */
            *(psHDL->pcErrorCodeBuff) |= 0x01u;
            EN_ERROR2 |= 0x01u;
        }
        else
        {
            psHDL->hdl_pulse_tms = 0u;
        }
    }                
}


/*******************************************************************************
* Function Name  : HR_Speed_Run_EN115
* Description    : Handrail Speed desviation detection,Stuck sensor detection.
* Input          : psHDL: handrail speed sensor id                
* Output         : None
* Return         : None
*******************************************************************************/
static void HR_Speed_Run_EN115(HandrailSpeedItem* psHDL)
{    
    u16 u16PulseSum = 0u, u8FaultFlag = 0u;
    u16 i;
    
    psHDL->hdl_pulse_tms = 200u;
    
    /* Handrail rising edge detected ? */
    if( psHDL->rising_edge_detected == 1u )
    {
        psHDL->rising_edge_detected = 0u;
        
        for( i = (HANDRAIL_PULSE_NUMBER - 1u); i > 0u; i-- )
        {
            psHDL->hdl_mp_array[i] = psHDL->hdl_mp_array[i - 1u];
        }
        psHDL->hdl_mp_array[0] = psHDL->handrail_speed_rt_pulse;
        psHDL->handrail_speed_rt_pulse = 0u;
        
        /* Handrail pulse counter > HANDRAIL_PULSE_NUMBER ? */
        if( psHDL->hdl_pulse_counter > HANDRAIL_PULSE_NUMBER )
        {
            for( i = 0u; i < HANDRAIL_PULSE_NUMBER; i++ )
            {
                u16PulseSum += psHDL->hdl_mp_array[i];
            }
            
            if(( u16PulseSum <= Handrail_speed_Min_pulse ) || ( u16PulseSum >= Handrail_speed_Max_pulse ))
            {
                u8FaultFlag = 1u;
            }
        }
        else
        {
            psHDL->hdl_pulse_counter++;
        }
    }
    
    if( u8FaultFlag == 0u)
    {
        for( i = 0u; i < (HANDRAIL_PULSE_NUMBER - 1u); i++ )
        {
            u16PulseSum += psHDL->hdl_mp_array[i];
        }
        u16PulseSum += psHDL->handrail_speed_rt_pulse;
    }
    
    if( ( u16PulseSum > Handrail_speed_Max_pulse ) || ( u8FaultFlag == 1u ) )
    {
        psHDL->HR_Fault_timer++;
        if( ( psHDL->HR_Fault_timer * SYSTEMTICK ) >= HR_FAULT_TIME )
        {
            if( SfBase_EscState & ESC_STATE_INSP ) 
            {         
                /* handrail speed warning */
                
            }
            else
            {
                /* handrail speed fault */
                *(psHDL->pcErrorCodeBuff) |= 0x04u;
                EN_ERROR2 |= 0x04u;
            }
        }            
    }
    else
    {
        psHDL->HR_Fault_timer = 0u;
    }    
}


/*******************************************************************************
* Function Name  : Handrail_Speed_Right_Left_Shortcircuit_Run
* Description    : Handrail Speed Right Left shortcircuit checking
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void Handrail_Speed_Right_Left_Shortcircuit_Run(void)
{
    static u32 stat_u32TimerHandrailSpeedShortcircuit = 0u;
    static u32 stat_u32HandrailShortcircuitOkCounter = 0u;
    static u32 stat_u32HandrailShortcircuitNotOkCounter = 0u;
    
    /* System in run state */
    if( SfBase_EscState == ESC_RUN_STATE )
    {  
        if( g_u8FirstHandrailSpeedEdgeDetected == 0u )
        {
            g_u8FirstHandrailSpeedEdgeDetected = 1u;
            stat_u32TimerHandrailSpeedShortcircuit = 0u;
            stat_u32HandrailShortcircuitOkCounter = 0u;
            stat_u32HandrailShortcircuitNotOkCounter = 0u;   
            /* Timer init, counter is 1us */
            TIM_Cmd(TIM6, DISABLE);  
            TIM6_Int_Init(65535u,71u);         
        }    
        else
        {
            /* Get the time between the two signals */
            stat_u32TimerHandrailSpeedShortcircuit = TIM_GetCounter(TIM6);
            /* Reset the timer */
            TIM_SetCounter(TIM6,0u);
            
            if( stat_u32TimerHandrailSpeedShortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                stat_u32TimerHandrailSpeedShortcircuit = 0u;               
                stat_u32HandrailShortcircuitNotOkCounter++;
                stat_u32HandrailShortcircuitOkCounter = 0u; 

                /* 100 consecutive pulses with less than SSM_SHORTCIRCUIT_TIME, go to fault */
                if( stat_u32HandrailShortcircuitNotOkCounter >= 100u )
                {                
                    /* Fault ¨C Motorspeed Sensor shortcircuited */
                    EN_ERROR4 |= 0x02u;
                }
            }
            else
            {
                stat_u32TimerHandrailSpeedShortcircuit = 0u;             
                stat_u32HandrailShortcircuitOkCounter++;     

                /* counter should be resetted after 10 consecutive different pulses with more than SSM_SHORTCIRCUIT_TIME */
                if( stat_u32HandrailShortcircuitOkCounter >= 10u )
                {
                    stat_u32HandrailShortcircuitNotOkCounter = 0u;
                }
            }
        }
    }        
}


/*******************************************************************************
* Function Name  : ESC_Handrail_Check
* Description    : Esc handrail speed check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Handrail_Check(void)
{
    static u16 stat_u16EscStateOld = 0u; 
    u8 i;

    /* esc turn to run state, init */
    if((SfBase_EscState == ESC_RUN_STATE) && (!(stat_u16EscStateOld == ESC_RUN_STATE))) 
    { 
        g_u8FirstHandrailSpeedEdgeDetected = 0u;
        
        /* Clear pulse counter */
        HDL_Left.hdl_pulse_counter = 0u;
        HDL_Right.hdl_pulse_counter = 0u;
        for( i = 0u; i < HANDRAIL_PULSE_NUMBER; i ++ )
        {
            HDL_Left.hdl_mp_array[i] = 0u;
            HDL_Right.hdl_mp_array[i] = 0u;
        }
    } 
    
    /* System in run or stopping process state */
    if(( SfBase_EscState == ESC_RUN_STATE ) || ( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ))
    {      
        HR_Speed_Run_EN115(&HDL_Right);
        HR_Speed_Run_EN115(&HDL_Left);
    }
    else /* System in Ready or 0Hz state , except run and stopping process state */
    {
        HR_Speed_Ready(&HDL_Right);
        HR_Speed_Ready(&HDL_Left);  
    }
    
    stat_u16EscStateOld = SfBase_EscState;                                      
}

/******************************  END OF FILE  *********************************/






