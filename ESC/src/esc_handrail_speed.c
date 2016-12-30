/*******************************************************************************
* File Name          : esc_handrail_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 12/09/2016
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
#define Handrail_speed_Max_pulse     ( ( Handrail_speed_Rate_pulse * 113u ) / 100u )
#define Handrail_speed_Min_pulse     ( ( Handrail_speed_Rate_pulse * 87u ) / 100u )

#define HandrailFreq                (( (u16)NOMINAL_SPEED * 10u * HR_PULSES_PER_REV ) / (( 2u * 314u * ROLLER_HR_RADIUS ) / 100u )) 	/* [1/s] */
#define Handrail_speed_Rate_pulse   (( HDL_CAL_PULSE_NUMBER * F1 ) / HandrailFreq )	/*  motor pulse amounts for 4 handrail pulses. */

#define HANDRAIL_LEFT_INPUT   (INPUT_PORT1_8 & INPUT_PORT3_MASK)
#define HANDRAIL_RIGHT_INPUT   (INPUT_PORT1_8 & INPUT_PORT4_MASK)

/* Private macro -------------------------------------------------------------*/
#define HDL_CAL_PULSE_NUMBER   4u

/* Private variables ---------------------------------------------------------*/
static u8 g_u8FirstHandrailSpeedSensorCheck = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void HR_Speed_Ready(HandrailSpeedItem* psHDL);
static void HR_Speed_Run_EN115(HandrailSpeedItem* psHDL);
static void Handrail_RisingEdge_check(void);
static void Handrail_Speed_Senor_Check(void);

u8 g_u8FirstHandrailSpeedEdgeDetected = 0u;

HandrailSpeedItem HDL_Left = 
{ 
    1u,
    200u,
    0u,
    0u,
    0u,
    {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
    0u,
    &HANDRAIL_SPEED_LEFT,
    &HANDRAIL_SPEED_LEFT_MOTOR_PULSE,
    0u
};

HandrailSpeedItem HDL_Right = 
{    
    2u,
    200u,
    0u,
    0u,
    0u,
    {0u,0u,0u,0u,0u,0u,0u,0u,0u,0u},
    0u,
    &HANDRAIL_SPEED_RIGHT,
    &HANDRAIL_SPEED_RIGHT_MOTOR_PULSE,
    0u
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
    u8 i;
    
    /* reset value */
    psHDL->hdl_pulse_counter = 0u;
    for( i = 0u; i < HDL_CAL_PULSE_NUMBER; i++ )
    {
        psHDL->hdl_mp_array[i] = 0u;
    }
    
    /* prevent overflow */
    if( psHDL->hdl_pulse_tms < 10000u )
    {
        psHDL->hdl_pulse_tms++;
    }

    if( psHDL->rising_edge_detected == 1u )
    {
        psHDL->rising_edge_detected = 0u;
        
        /* Handrail pulse detected with freq > 1 Hz? */
        if( psHDL->hdl_pulse_tms < 200u )
        {
            if(!( CMD_ESC_RUN_MODE & ESC_NORMAL )) 
            {         
                /* HR speed Warning left (W63) */
                if( psHDL->SensorX == 1u )
                {
                    EN_WARN8 |= 0x80u;
                }
                else if( psHDL->SensorX == 2u )
                {
                    /* HR speed Warning right (W63) */
                    EN_WARN8 |= 0x80u;
                }
                else
                {}  
            }
            else
            {               
                if( psHDL->SensorX == 1u )
                {
                    /* Handrail left pulse detected while stop F338 */
                    EN_ERROR43 |= 0x04u;
                }
                else if( psHDL->SensorX == 2u )
                {
                    /* Handrail right pulse detected while stop F339 */
                    EN_ERROR43 |= 0x08u;
                }
                else
                {}
            }
        }
        else
        {
            psHDL->hdl_pulse_tms = 0u;
        }
    }   
    
    /* handrail speed (pulse of motor) */
    *psHDL->ptHDLMotorOfPulse = 0u;
    
    /* handrail speed (HZ * 100) */
    *psHDL->ptHDLSpeed = 0u;
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
    u16 u16PulseSum = 0u;
    u8 u8fault_flag = 0u;
    u16 i;
    
    psHDL->hdl_pulse_tms = 200u;
    
    /* Handrail rising edge detected ? */
    if( psHDL->rising_edge_detected == 1u )
    {
        psHDL->rising_edge_detected = 0u;
        
        if( psHDL->hdl_pulse_counter <= HDL_CAL_PULSE_NUMBER )
        { 
            psHDL->hdl_pulse_counter++;
        }

        for( i = (HDL_CAL_PULSE_NUMBER - 1u); i > 0u; i-- )
        {
            psHDL->hdl_mp_array[i] = psHDL->hdl_mp_array[i - 1u];
        }
        psHDL->hdl_mp_array[0] = psHDL->handrail_speed_rt_pulse;
        psHDL->handrail_speed_rt_pulse = 0u;
        
        psHDL->HDL_Pulse_Value = 0u;
        for( i = 0u; i < HDL_CAL_PULSE_NUMBER; i++ )
        {
            psHDL->HDL_Pulse_Value += psHDL->hdl_mp_array[i];
        }
        
        /* handrail speed (pulse of motor) */
        *psHDL->ptHDLMotorOfPulse = psHDL->HDL_Pulse_Value;
        
        /* handrail speed (HZ * 100) */
        *psHDL->ptHDLSpeed = (( HDL_CAL_PULSE_NUMBER * F1 * 100u ) / psHDL->HDL_Pulse_Value);        
    }
    else
    {      
        /* stuck sensor detection */
        for( i = 0u; i < (HDL_CAL_PULSE_NUMBER - 1u); i++ )
        {
            u16PulseSum += psHDL->hdl_mp_array[i];
        }
        u16PulseSum += psHDL->handrail_speed_rt_pulse;
    }

    if( psHDL->hdl_pulse_counter > HDL_CAL_PULSE_NUMBER )
    {       
        /* handrail speed faster than motor speed */
        if( psHDL->HDL_Pulse_Value < Handrail_speed_Min_pulse )
        {
            u8fault_flag = 3u;
        }
    } 
    
    if( !u8fault_flag )
    {
        if( u16PulseSum < Handrail_speed_Max_pulse )
        {
            if( psHDL->HDL_Pulse_Value < Handrail_speed_Max_pulse )
            {
                psHDL->HR_Fault_timer = 0u;
            }  
            else
            {
                /* handrail speed slower than motor speed */
                u8fault_flag = 2u; 
            }
        }
        else
        {
            /* stuck sensor detection */
            u8fault_flag = 1u;   
        }
    }
 
    if( u8fault_flag )
    {
        psHDL->HR_Fault_timer++;
        if( ( psHDL->HR_Fault_timer * SYSTEMTICK ) >= ( HR_FAULT_TIME * 1000u ))
        {
            if(!( CMD_ESC_RUN_MODE & ESC_NORMAL )) 
            {         
                /* HR speed Warning left (W63) */
                if( psHDL->SensorX == 1u )
                {
                    EN_WARN8 |= 0x80u;
                }
                else if( psHDL->SensorX == 2u )
                {
                    /* HR speed Warning right (W63) */
                    EN_WARN8 |= 0x80u;
                }
                else
                {}                
            }
            else
            {          
                if( psHDL->SensorX == 1u )
                {
                    if( u8fault_flag == 1u )
                    {
                        /* Handrail speed stuck sensor left F260 */
                        EN_ERROR33 |= 0x10u;
                    }
                    else
                    {
                        /* Speed supervision: handrail left F73 */
                        EN_ERROR10 |= 0x02u;
                    }
                }
                else if( psHDL->SensorX == 2u )
                {
                    if( u8fault_flag == 1u )
                    {
                        /* Handrail speed stuck sensor right F261 */
                        EN_ERROR33 |= 0x20u;                    
                    }
                    else
                    {                    
                        /* Speed supervision: handrail right F74 */
                        EN_ERROR10 |= 0x04u;
                    }
                }
                else
                {}
            }
        }            
    }  
}

/*******************************************************************************
* Function Name  : Handrail_Speed_Senor_Check
* Description    : Check the difference in both handrail speed sensors between CPU.                      
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Handrail_Speed_Senor_Check(void)
{
    static u16 stat_u16TimerHandrailSpeedLeftSensor = 0u;
    static u16 stat_u16TimerHandrailSpeedRightSensor = 0u;
    u16 u16HandrailSpeedSensorDiff = 0u;

     /* System in run state */
    if( SfBase_EscState == ESC_RUN_STATE )
    {    
        if( g_u8FirstHandrailSpeedSensorCheck == 0u )
        {
            g_u8FirstHandrailSpeedSensorCheck = 1u;
            stat_u16TimerHandrailSpeedLeftSensor = 0u;
            stat_u16TimerHandrailSpeedRightSensor = 0u;         
        } 

        /* Handrail speed left sensor pulse of motor  */
        if( OMC_HANDRAIL_SPEED_LEFT_MOTOR_PULSE > HANDRAIL_SPEED_LEFT_MOTOR_PULSE )
        {
            u16HandrailSpeedSensorDiff = ( OMC_HANDRAIL_SPEED_LEFT_MOTOR_PULSE - HANDRAIL_SPEED_LEFT_MOTOR_PULSE ) * 100u / HANDRAIL_SPEED_LEFT_MOTOR_PULSE;
        }
        else if( OMC_HANDRAIL_SPEED_LEFT_MOTOR_PULSE < HANDRAIL_SPEED_LEFT_MOTOR_PULSE )
        {
            u16HandrailSpeedSensorDiff = ( HANDRAIL_SPEED_LEFT_MOTOR_PULSE - OMC_HANDRAIL_SPEED_LEFT_MOTOR_PULSE ) * 100u / OMC_HANDRAIL_SPEED_LEFT_MOTOR_PULSE;
        }
        else
        {
            stat_u16TimerHandrailSpeedLeftSensor = 0u;
        }  
        
        /* If these values are different in more than 10% during a period of time of 5s safety board goes to fault */
        if( u16HandrailSpeedSensorDiff > 10u )
        {
            stat_u16TimerHandrailSpeedLeftSensor++;
            if(( stat_u16TimerHandrailSpeedLeftSensor * SYSTEMTICK ) > 5000u )
            {
                /* DISCRESPANCE_HANDRAIL_LEFT_CPU  F376 */
                EN_ERROR48 |= 0x01u;
            }
        }            
        
        /* Handrail speed right sensor pulse of motor */
        if( OMC_HANDRAIL_SPEED_RIGHT_MOTOR_PULSE > HANDRAIL_SPEED_RIGHT_MOTOR_PULSE )
        {
            u16HandrailSpeedSensorDiff = ( OMC_HANDRAIL_SPEED_RIGHT_MOTOR_PULSE - HANDRAIL_SPEED_RIGHT_MOTOR_PULSE ) * 100u / HANDRAIL_SPEED_RIGHT_MOTOR_PULSE;
        }
        else if( OMC_HANDRAIL_SPEED_RIGHT_MOTOR_PULSE < HANDRAIL_SPEED_RIGHT_MOTOR_PULSE )
        {
            u16HandrailSpeedSensorDiff = ( HANDRAIL_SPEED_RIGHT_MOTOR_PULSE - OMC_HANDRAIL_SPEED_RIGHT_MOTOR_PULSE ) * 100u / OMC_HANDRAIL_SPEED_RIGHT_MOTOR_PULSE;
        }
        else
        {
            stat_u16TimerHandrailSpeedRightSensor = 0u;
        }  
        
        /* If these values are different in more than 10% during a period of time of 5s safety board goes to fault */
        if( u16HandrailSpeedSensorDiff > 10u )
        {
            stat_u16TimerHandrailSpeedRightSensor++;
            if(( stat_u16TimerHandrailSpeedRightSensor * SYSTEMTICK ) > 5000u )
            {
                /* DISCRESPANCE_HANDRAIL_RIGHT_CPU F377 */
                EN_ERROR48 |= 0x02u;
            }
        }       
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
            
            /* Timer init, counter is 100us */
            TIM_Cmd(SENSOR_SHORTCIRCUIT_CHECK_TIMER, DISABLE);  
            TIM6_Int_Init(65535u,7199u);         
        }    
        else
        {
            /* Get the time between the two signals */
            stat_u32TimerHandrailSpeedShortcircuit = TIM_GetCounter(SENSOR_SHORTCIRCUIT_CHECK_TIMER);
            /* Reset the timer */
            TIM_SetCounter(SENSOR_SHORTCIRCUIT_CHECK_TIMER,0u);
            
            if(( stat_u32TimerHandrailSpeedShortcircuit * 100u ) < PULSE_SIGNALS_MINIMUM_LAG )
            {
                stat_u32TimerHandrailSpeedShortcircuit = 0u;  
                if( stat_u32HandrailShortcircuitNotOkCounter < 0xffffu )
                {
                    stat_u32HandrailShortcircuitNotOkCounter++;
                }
                stat_u32HandrailShortcircuitOkCounter = 0u; 

                /* 100 consecutive pulses with less than SSM_SHORTCIRCUIT_TIME, go to fault */
                if( stat_u32HandrailShortcircuitNotOkCounter >= 100u )
                {                
                    if(!( CMD_ESC_RUN_MODE & ESC_NORMAL )) 
                    {         
                        /* HR speed Warning (W63) */
                        EN_WARN8 |= 0x80u;
                    }
                    else
                    {
                        /* Fault ¨C HS Sensor shortcircuited (F262) */
                        EN_ERROR33 |= 0x40u;  
                    }
                }
            }
            else
            {
                stat_u32TimerHandrailSpeedShortcircuit = 0u;   
                if( stat_u32HandrailShortcircuitOkCounter < 0xffffu )
                {
                    stat_u32HandrailShortcircuitOkCounter++;     
                }
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
* Function Name  : Handrail_RisingEdge_check
* Description    : Handrail rising edge check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
static void Handrail_RisingEdge_check(void)
{
    static u8 stat_u8HandrailLeftInputPre = 1u,stat_u8HandrailRightInputPre = 1u;
    
    if( stat_u8HandrailLeftInputPre == 0u )
    {
        if( HANDRAIL_LEFT_INPUT )
        {
            stat_u8HandrailLeftInputPre = 1u;
            HDL_Left.rising_edge_detected = 1u;
        }
    }
    else if( stat_u8HandrailLeftInputPre == 1u )
    {
        if( !HANDRAIL_LEFT_INPUT )
        {
            stat_u8HandrailLeftInputPre = 0u;
        }
    }
    else
    {}

    if( stat_u8HandrailRightInputPre == 0u )
    {
        if( HANDRAIL_RIGHT_INPUT )
        {
            stat_u8HandrailRightInputPre = 1u;
            HDL_Right.rising_edge_detected = 1u;
        }
    }
    else if( stat_u8HandrailRightInputPre == 1u )
    {
        if( !HANDRAIL_RIGHT_INPUT )
        {
            stat_u8HandrailRightInputPre = 0u;
        }
    }
    else
    {}   
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

    Handrail_RisingEdge_check();
    
    /* esc turn to run state, init */
    if((SfBase_EscState == ESC_RUN_STATE) && (!(stat_u16EscStateOld == ESC_RUN_STATE))) 
    { 
        g_u8FirstHandrailSpeedEdgeDetected = 0u;
        g_u8FirstHandrailSpeedSensorCheck = 0u;
        
        /* Reset the value when escalator run */
        HDL_Left.handrail_speed_rt_pulse = 0u;
        HDL_Left.HR_Fault_timer = 0u;
        HDL_Left.rising_edge_detected = 0u;
        HDL_Left.HDL_Pulse_Value = 0u;
  
        HDL_Right.handrail_speed_rt_pulse = 0u;
        HDL_Right.HR_Fault_timer = 0u;
        HDL_Right.rising_edge_detected = 0u;
        HDL_Right.HDL_Pulse_Value = 0u;
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
    
    Handrail_Speed_Senor_Check();
    
    stat_u16EscStateOld = SfBase_EscState;                                      
}

/******************************  END OF FILE  *********************************/






