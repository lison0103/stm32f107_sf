/*******************************************************************************
* File Name          : esc_missing_step.c
* Author             : lison
* Version            : V1.0
* Date               : 05/10/2016
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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS);

/* parameters */ 
u16 STEP_WIDTH = 400;

/* variable */
u32 Motor_speed_pulse_counter = 0;
u8 First_entry_missing_step_detection,First_MS_sync_entry,First_MS_edge_detected = 0;
u16 *const MS_MTR_PULSE = (u16*)&EscRTBuff[67];

STEPMISSINGITEM STPMS_UPPER=
{
    &EscRTBuff[64],

    0,
    0,0,
    {0,0},
    (u16*)&EscRTBuff[60],
    0
};

STEPMISSINGITEM STPMS_LOWER=
{
    &EscRTBuff[65],

    0,          
    0,0,
    {0,0},
    (u16*)&EscRTBuff[62],
    0
};


/*******************************************************************************
* Function Name  : Missing_Step_Ready
* Description    : Check the escalator missing step ready.
* Input          : psSTPMS: missingstep speed sensor id           
* Output         : None
* Return         : None
*******************************************************************************/
void Missing_Step_Ready(STEPMISSINGITEM* psSTPMS)
{      
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {                
        psSTPMS->ms_ready_delay++;
        
        if( psSTPMS->rising_edge_detected[0] == 1 )
        {
            psSTPMS->rising_edge_detected[0] = 0;

            if( psSTPMS->ms_ready_delay * SYSTEMTICK < 1000 )
            {
                /* fault */ 
                *(psSTPMS->pcErrorCodeBuff) |= 0x01;
            }
            else
            {
                psSTPMS->ms_ready_delay = 0;
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
void Missing_StepRun(STEPMISSINGITEM* psSTPMS)
{
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {       
        
        if( First_entry_missing_step_detection == 0 )
        {
            First_entry_missing_step_detection = 1;
//            Motor_speed_pulse_counter = Pulse_counter_sensor_speed(psSTPMS);
        }
        
//        Motor_speed_pulse_counter =  Pulse_counter_sensor_speed(psSTPMS) - Motor_speed_pulse_counter;
        
        *MS_MTR_PULSE = R;
        
        if( psSTPMS->MtrPulse > R*1.1 )
        {
            if( SfBase_EscState & ESC_STATE_INSP )
            {     
                /* Missing step warning */

            }
            else
            {
                /* Missing step fault */
                *(psSTPMS->pcErrorCodeBuff) |= 0x02;

            }
        }
        else
        {
            if( psSTPMS->rising_edge_detected[0] == 1 )
            {
                
//                Motor_speed_pulse_counter =  Pulse_counter_sensor_speed(psSTPMS) - Motor_speed_pulse_counter;
                
                if( psSTPMS->MtrPulse > R*1.1 )
                {
                    if( SfBase_EscState & ESC_STATE_INSP )
                    {     
                        /* Missing step warning */

                    }
                    else
                    {
                        /* Missing step fault */
                        *(psSTPMS->pcErrorCodeBuff) |= 0x04;;
                    }
                }
                else
                {
//                    *(psSTPMS->ptStepMtrBuff) = psSTPMS->MtrPulse;
                    psSTPMS->rising_edge_detected[0] = 0;
//                    psSTPMS->MtrPulse = 0;
//                    Motor_speed_pulse_counter = Pulse_counter_sensor_speed(psSTPMS);
                }
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
u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS)
{
    u16 current_pulse_counter_sensor = 0;    
    
    current_pulse_counter_sensor = psSTPMS->MtrPulse;
    
    return current_pulse_counter_sensor;
}


/*******************************************************************************
* Function Name  : Missing_Step_UpperLower_SyncRun
* Description    : Missing step upper lower syncronization checking
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Missing_Step_UpperLower_SyncRun(void)
{
    static u8 Upper_MS_counter,Lower_MS_counter = 0;    
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_MS_sync_entry == 0 )
        {
            First_MS_sync_entry = 1;
            Upper_MS_counter = 0;
            Lower_MS_counter = 0;
        } 
        else
        {
            if( STPMS_UPPER.rising_edge_detected[1] == 1 )
            {
                STPMS_UPPER.rising_edge_detected[1] = 0;
                
                Upper_MS_counter += 1;
                Lower_MS_counter = 0;
                
                if( Upper_MS_counter > 2 )
                {
                    /* Fault */
                    MS_SYNC_RUN_ERROR |= 0x01;
                    
                }
                else
                {
                    MS_SYNC_RUN_ERROR &= ~0x01;
                }
            }
            
            if( STPMS_LOWER.rising_edge_detected[1] == 1 )
            {
                STPMS_LOWER.rising_edge_detected[1] = 0;
                
                Lower_MS_counter += 1;
                Upper_MS_counter = 0;
                
                if( Lower_MS_counter > 2 )
                {
                    /* Fault */
                    MS_SYNC_RUN_ERROR |= 0x02;
                }
                else
                {
                    MS_SYNC_RUN_ERROR &= ~0x02;
                }
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
    static u16 Timer_MS_shortcircuit = 0;
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_MS_edge_detected == 0 )
        {
            First_MS_edge_detected = 1;
            Timer_MS_shortcircuit = 0;
            TIM_Cmd(TIM7, DISABLE); 
            TIM7_Int_Init(65535,71);
            
        }    
        else
        {
            Timer_MS_shortcircuit = TIM_GetCounter(TIM7);
            if( Timer_MS_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_MS_shortcircuit = 0;               
                TIM_SetCounter(TIM7,0); 
                
                /* Fault ¨C Motorspeed Sensor shortcircuited */
                SHORTCIRCUIT_ERROR |= 0x04;
            }
            else
            {
                Timer_MS_shortcircuit = 0;             
                TIM_SetCounter(TIM7,0);     
                
                SHORTCIRCUIT_ERROR &= ~0x04;
            }
        }
    }    
}


/*******************************************************************************
* Function Name  : ESC_Missingstep_Check
* Description    : Esc Missingstep check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Missingstep_Check(void)
{
  static u16 escState_old = 0; 
  
  
  Missing_Step_Ready(&STPMS_UPPER);
  Missing_Step_Ready(&STPMS_LOWER);		
 
  if((SfBase_EscState & ESC_STATE_RUNNING) && (!(escState_old & ESC_STATE_RUNNING))) 
  { 
      First_entry_missing_step_detection = 0;
      First_MS_sync_entry = 0;
      First_MS_edge_detected = 0;
  } 
  
  Missing_StepRun(&STPMS_UPPER);
  Missing_StepRun(&STPMS_LOWER);
  Missing_Step_UpperLower_SyncRun();
  
  escState_old = SfBase_EscState;                                      

}

/******************************  END OF FILE  *********************************/






