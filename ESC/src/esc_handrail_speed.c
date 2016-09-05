/*******************************************************************************
* File Name          : esc_handrail_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
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
#define HDL_PULSE_CYCLE_FINISH    0x01
#define HDL_PULSE_CAL_DIFF_EN     0x02

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static u16 Measure_handrail_speed(HDLITEM* psHDL);
static void HR_Speed_Ready(HDLITEM* psHDL);
static void HR_Speed_Run_EN115(HDLITEM* psHDL);

/* variable */
static u8 First_HS_edge_detected = 0u;
static u16 *const HR_SPEED = (u16*)&EscRTBuff[56];
static u16 *const MOTOR_SPEED = (u16*)&EscRTBuff[58];

HDLITEM HDL_Left = 
{
    
    0,0,
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    0,0,
    (u16*)&EscRTBuff[50],
    &EscRTBuff[54]
};

HDLITEM HDL_Right = 
{
    
    0,0,
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    0,0,
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
static void HR_Speed_Ready(HDLITEM* psHDL)
{
    u16 Escalator_handrail_speed = 0u;       
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {                
        Escalator_handrail_speed = Measure_handrail_speed(psHDL);
        
        if( Escalator_handrail_speed > 1u )
        {
            /* fault */
            *(psHDL->pcErrorCodeBuff) |= 0x01u;
            EN_ERROR2 |= 0x01u;
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
static void HR_Speed_Run_EN115(HDLITEM* psHDL)
{
    u16 Escalator_speed,Handrail_speed,Handrail_speed_freq,Escalator_speed_freq = 0u;
    
    if( ( SfBase_EscState & ESC_STATE_RUN ) || ( (SfBase_EscState & ESC_STATE_STOP) && (!(SfBase_EscState & ESC_STATE_READY))))
    {
        
        Handrail_speed_freq = Measure_handrail_speed(psHDL);
        Escalator_speed_freq = *(u16*)&EscRTBuff[40]; /* ( ( EscRTBuff[41] << 8u ) | EscRTBuff[40] );*/
        Handrail_speed = (( Handrail_speed_freq * 2u * 314u * ROLLER_HR_RADIUS ) / ( HR_PULSES_PER_REV * 100u ));    
        Escalator_speed = (( Escalator_speed_freq * NOMINAL_SPEED ) / ( ( MOTOR_RPM * MOTOR_PLUSE_PER_REV ) / 60u ) );
        
        *HR_SPEED = Handrail_speed;
        *MOTOR_SPEED = Escalator_speed;
        
        if( SfBase_EscState & ESC_STATE_RUN )
        {
        
            if( ( psHDL->delay_no_pulse_tms * SYSTEMTICK ) > DELAY_NO_PULSE_CHECKING )
            {
                if( Handrail_speed_freq < 1u )
                {
                    if( SfBase_EscState & ESC_STATE_INSP ) 
                    {         
                        /* handrail speed warning */
                    }
                    else
                    {
                        /* handrail speed fault */
                        *(psHDL->pcErrorCodeBuff) |= 0x02u;
                        EN_ERROR2 |= 0x02u;
                    }            
                }          
            }
            else
            {
                psHDL->delay_no_pulse_tms++;
            }
            
            
            
            psHDL->HR_Fault_timer++;
            if( (Handrail_speed <= (( Escalator_speed * 87u ) / 100u )) || (Handrail_speed >= (( Escalator_speed * 113u ) / 100u )) )
            {
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
        
        *(psHDL->ptHDLDataBuff) = Handrail_speed_freq;
    }
    
}

/*******************************************************************************
* Function Name  : Measure_handrail_speed
* Description    : Measure the escalator hanrail speed.
* Input          : psHDL: handrail speed sensor id                
* Output         : None
* Return         : None
*******************************************************************************/
static u16 Measure_handrail_speed(HDLITEM* psHDL)
{
    u16 current_hanrail_speed_sensor,i = 0u;  
    u8 timeDelayCnt = 100u;    
    
    /* 20 * 5 = 100ms */
    psHDL->Tms_counter++;
    if( ( psHDL->Tms_counter * SYSTEMTICK ) >= timeDelayCnt) 
    { 
        psHDL->Tms_counter = 0u; 
        
        for(i = 9u; i > 0u; i--)                                                   
        {   
            psHDL->pulseArray[i] = psHDL->pulseArray[i-1u];
        }
        psHDL->pulseArray[0] = psHDL->hr_pulse;
        psHDL->hr_pulse = 0u;
        
    }
    current_hanrail_speed_sensor = 0u;
    /* 100ms * 10 = 1s */
    for(i = 0u; i < 10u; i++) 
    {
        current_hanrail_speed_sensor += psHDL->pulseArray[i];
    }  
    
    return current_hanrail_speed_sensor;
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
    static u32 Timer_HS_shortcircuit = 0u;
    
    if( SfBase_EscState & ESC_STATE_RUN )
    {  
        if( First_HS_edge_detected == 0u )
        {
            First_HS_edge_detected = 1u;
            Timer_HS_shortcircuit = 0u;
            TIM_Cmd(TIM6, DISABLE);  
            TIM6_Int_Init(65535u,71u);
            
        }    
        else
        {
            Timer_HS_shortcircuit = TIM_GetCounter(TIM6);
            if( Timer_HS_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_HS_shortcircuit = 0u;               
                TIM_SetCounter(TIM6,0u); 
                
                /* Fault ¨C Motorspeed Sensor shortcircuited */
                SHORTCIRCUIT_ERROR |= 0x02u;
                EN_ERROR4 |= 0x02u;
            }
            else
            {
                Timer_HS_shortcircuit = 0u;             
                TIM_SetCounter(TIM6,0u);     
                
                SHORTCIRCUIT_ERROR &= ~0x02u;
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
  static u16 escState_old = 0u; 
  
  
  HR_Speed_Ready(&HDL_Right);
  HR_Speed_Ready(&HDL_Left);		
 
  if((SfBase_EscState & ESC_STATE_RUN) && (!(escState_old & ESC_STATE_RUN))) 
  { 
      First_HS_edge_detected = 0u;
      HDL_Left.delay_no_pulse_tms = 0u;
      HDL_Right.delay_no_pulse_tms = 0u;
  } 
  
  HR_Speed_Run_EN115(&HDL_Right);
  HR_Speed_Run_EN115(&HDL_Left);
  
  
  escState_old = SfBase_EscState;                                      

}

/******************************  END OF FILE  *********************************/






