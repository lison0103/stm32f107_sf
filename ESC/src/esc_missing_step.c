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

/* variable */
u8 First_MS_sync_entry,First_MS_edge_detected = 0u;
u16 *const MS_MTR_PULSE = (u16*)&EscRTBuff[67];

STEPMISSINGITEM STPMS_UPPER=
{
    &EscRTBuff[64],

    0,
    0,0,
    {0,0},
    (u16*)&EscRTBuff[60],
    0,
    0,0,0
};

STEPMISSINGITEM STPMS_LOWER=
{
    &EscRTBuff[65],

    0,          
    0,0,
    {0,0},
    (u16*)&EscRTBuff[62],
    0,
    0,0,0
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
        
        if( psSTPMS->rising_edge_detected[0] == 1u )
        {
            psSTPMS->rising_edge_detected[0] = 0u;

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
void Missing_StepRun(STEPMISSINGITEM* psSTPMS)
{
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {       
        
        if( psSTPMS->First_entry_missing_step_detection == 0u )
        {
            psSTPMS->First_entry_missing_step_detection = 1u;
            psSTPMS->Motor_speed_pulse_counter_init = Pulse_counter_sensor_speed(psSTPMS);
        }
        
        psSTPMS->Motor_speed_pulse_counter =  Pulse_counter_sensor_speed(psSTPMS) - psSTPMS->Motor_speed_pulse_counter_init;
        
        *MS_MTR_PULSE = R;
        
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
            if( psSTPMS->rising_edge_detected[0] == 1u )
            {
                
                psSTPMS->Motor_speed_pulse_counter =  Pulse_counter_sensor_speed(psSTPMS) - psSTPMS->Motor_speed_pulse_counter_init;
                
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

                *(psSTPMS->ptStepMtrBuff) = psSTPMS->MtrPulse;
                psSTPMS->rising_edge_detected[0] = 0u;
                
                if(psSTPMS->MtrPulse)
                {
                    psSTPMS->MtrPulse = 0u;
                    psSTPMS->Motor_speed_pulse_counter_init = Pulse_counter_sensor_speed(psSTPMS);
                    psSTPMS->sensor_error_cnt = 0u;
                }
                else if((!(*MTRITEM[0].ptFreqBuff)) && (!(*MTRITEM[1].ptFreqBuff)))
                {
                    psSTPMS->sensor_error_cnt++;
                    *(psSTPMS->pcErrorCodeBuff) |= 0x08u;
                    EN_ERROR3 |= 0x08u;
                }
                else
                {
                
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
    u16 current_pulse_counter_sensor = 0u;    
    
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
    static u8 Upper_MS_counter,Lower_MS_counter = 0u;    
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_MS_sync_entry == 0u )
        {
            First_MS_sync_entry = 1u;
            Upper_MS_counter = 0u;
            Lower_MS_counter = 0u;
        } 
        else
        {
            if( STPMS_UPPER.rising_edge_detected[1] == 1u )
            {
                STPMS_UPPER.rising_edge_detected[1] = 0u;
                
                Upper_MS_counter += 1u;
                Lower_MS_counter = 0u;
                
                if( Upper_MS_counter > 2u )
                {
                    /* Fault */
                    MS_SYNC_RUN_ERROR |= 0x01u;
                    EN_ERROR5 |= 0x01u;
                    
                }
                else
                {
                    MS_SYNC_RUN_ERROR &= ~0x01u;
                }
            }
            
            if( STPMS_LOWER.rising_edge_detected[1] == 1u )
            {
                STPMS_LOWER.rising_edge_detected[1] = 0u;
                
                Lower_MS_counter += 1u;
                Upper_MS_counter = 0u;
                
                if( Lower_MS_counter > 2u )
                {
                    /* Fault */
                    MS_SYNC_RUN_ERROR |= 0x02u;
                    EN_ERROR5 |= 0x02u;
                }
                else
                {
                    MS_SYNC_RUN_ERROR &= ~0x02u;
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
    static u32 Timer_MS_shortcircuit = 0u;
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_MS_edge_detected == 0u )
        {
            First_MS_edge_detected = 1u;
            Timer_MS_shortcircuit = 0u;
            TIM_Cmd(TIM7, DISABLE); 
            TIM7_Int_Init(65535u,71u);
            
        }    
        else
        {
            Timer_MS_shortcircuit = TIM_GetCounter(TIM7);
            if( Timer_MS_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_MS_shortcircuit = 0u;               
                TIM_SetCounter(TIM7,0u); 
                
                /* Fault ¨C Motorspeed Sensor shortcircuited */
                SHORTCIRCUIT_ERROR |= 0x04u;
                EN_ERROR4 |= 0x04u;
            }
            else
            {
                Timer_MS_shortcircuit = 0u;             
                TIM_SetCounter(TIM7,0u);     
                
                SHORTCIRCUIT_ERROR &= ~0x04u;
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
  static u16 escState_old = 0u; 
  
  
  Missing_Step_Ready(&STPMS_UPPER);
  Missing_Step_Ready(&STPMS_LOWER);		
 
  if((SfBase_EscState & ESC_STATE_RUNNING) && (!(escState_old & ESC_STATE_RUNNING))) 
  { 
      STPMS_UPPER.First_entry_missing_step_detection = 0u;
      STPMS_LOWER.First_entry_missing_step_detection = 0u;
      First_MS_sync_entry = 0u;
      First_MS_edge_detected = 0u;
  } 
  
  Missing_StepRun(&STPMS_UPPER);
  Missing_StepRun(&STPMS_LOWER);
  Missing_Step_UpperLower_SyncRun();
  
  escState_old = SfBase_EscState;                                      

}

/******************************  END OF FILE  *********************************/






