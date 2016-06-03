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
u16 Measure_handrail_speed(HDLITEM* psHDL);

/* parameters */ 
u16 HR_FAULT_TIME = 10000;
u16 ROLLER_HR_RADIUS = ( 0.050 * 1000 ); //mm
u16 HR_PULSES_PER_REV = 2;


/* variable */
u8 First_HS_edge_detected = 0;
u16 *const HR_SPEED = (u16*)&EscRTBuff[56];
u16 *const MOTOR_SPEED = (u16*)&EscRTBuff[58];

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
void HR_Speed_Ready(HDLITEM* psHDL)
{
    u16 Escalator_handrail_speed = 0;       
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {                
        Escalator_handrail_speed = Measure_handrail_speed(psHDL);
        
        if( Escalator_handrail_speed > 1 )
        {
            /* fault */
            *(psHDL->pcErrorCodeBuff) |= 0x01;
            EN_ERROR2 |= 0x01;
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
void HR_Speed_Run_EN115(HDLITEM* psHDL)
{
    u16 Escalator_speed,Handrail_speed,Handrail_speed_freq,Escalator_speed_freq = 0;
    
    if( SfBase_EscState & ESC_STATE_RUNNING  || ( (SfBase_EscState & ESC_STATE_STOP) && (!(SfBase_EscState & ESC_STATE_READY))))
    {
        
        Handrail_speed_freq = Measure_handrail_speed(psHDL);
        Escalator_speed_freq = ( EscRTBuff[41] << 8 | EscRTBuff[40] );
        Handrail_speed = (( Handrail_speed_freq * 2 * 314 * ROLLER_HR_RADIUS ) / ( HR_PULSES_PER_REV * 100 ));    
        Escalator_speed = (( Escalator_speed_freq * NOMINAL_SPEED ) / ( ( MOTOR_RPM * MOTOR_PLUSE_PER_REV ) / 60 ) );
        
        *HR_SPEED = Handrail_speed;
        *MOTOR_SPEED = Escalator_speed;
        
        if( SfBase_EscState & ESC_STATE_RUNNING )
        {
        
            if( ( psHDL->delay_no_pulse_tms * SYSTEMTICK ) > DELAY_NO_PULSE_CHECKING )
            {
                if( Handrail_speed_freq < 1 )
                {
                    if( SfBase_EscState & ESC_STATE_INSP ) 
                    {         
                        /* handrail speed warning */
                    }
                    else
                    {
                        /* handrail speed fault */
                        *(psHDL->pcErrorCodeBuff) |= 0x02;
                        EN_ERROR2 |= 0x02;
                    }            
                }          
            }
            else
            {
                psHDL->delay_no_pulse_tms++;
            }
            
            
            
            psHDL->HR_Fault_timer++;
            if( (Handrail_speed <= (( Escalator_speed * 87 ) / 100 )) || (Handrail_speed >= (( Escalator_speed * 113 ) / 100 )) )
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
                        *(psHDL->pcErrorCodeBuff) |= 0x04;
                        EN_ERROR2 |= 0x04;
                    }
                }
            }
            else
            {
                psHDL->HR_Fault_timer = 0;
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
u16 Measure_handrail_speed(HDLITEM* psHDL)
{
    u16 current_hanrail_speed_sensor,i = 0;  
    u8 timeDelayCnt = 100;    
    
    /* 20 * 5 = 100ms */
    psHDL->Tms_counter++;
    if( ( psHDL->Tms_counter * SYSTEMTICK ) >= timeDelayCnt) 
    { 
        psHDL->Tms_counter = 0; 
        
        for(i=9; i>0; i--)                                                   
        {   
            psHDL->pulseArray[i] = psHDL->pulseArray[i-1];
        }
        psHDL->pulseArray[0] = psHDL->hr_pulse;
        psHDL->hr_pulse=0;
        
    }
    current_hanrail_speed_sensor = 0;
    /* 100ms * 10 = 1s */
    for(i=0;i<10;i++) 
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
    static u16 Timer_HS_shortcircuit = 0;
    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_HS_edge_detected == 0 )
        {
            First_HS_edge_detected = 1;
            Timer_HS_shortcircuit = 0;
            TIM_Cmd(TIM6, DISABLE);  
            TIM6_Int_Init(65535,71);
            
        }    
        else
        {
            Timer_HS_shortcircuit = TIM_GetCounter(TIM6);
            if( Timer_HS_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_HS_shortcircuit = 0;               
                TIM_SetCounter(TIM6,0); 
                
                /* Fault ¨C Motorspeed Sensor shortcircuited */
                SHORTCIRCUIT_ERROR |= 0x02;
                EN_ERROR4 |= 0x02;
            }
            else
            {
                Timer_HS_shortcircuit = 0;             
                TIM_SetCounter(TIM6,0);     
                
                SHORTCIRCUIT_ERROR &= ~0x02;
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
  static u16 escState_old = 0; 
  
  
  HR_Speed_Ready(&HDL_Right);
  HR_Speed_Ready(&HDL_Left);		
 
  if((SfBase_EscState & ESC_STATE_RUNNING) && (!(escState_old & ESC_STATE_RUNNING))) 
  { 
      First_HS_edge_detected = 0;
      HDL_Left.delay_no_pulse_tms = 0;
      HDL_Right.delay_no_pulse_tms = 0;
  } 
  
  HR_Speed_Run_EN115(&HDL_Right);
  HR_Speed_Run_EN115(&HDL_Left);
  
  
  escState_old = SfBase_EscState;                                      

}

/******************************  END OF FILE  *********************************/






