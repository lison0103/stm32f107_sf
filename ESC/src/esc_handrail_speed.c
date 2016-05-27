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

HDLITEM HDL_Left = 
{
    
  0,0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  0,0,
  (u16*)&EscRTBuff[44]
  
};

HDLITEM HDL_Right = 
{
    
  0,0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  0,0,
  (u16*)&EscRTBuff[46]
  
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
    
    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        psHDL->delay_no_pulse_tms = 0;
    }    

    
    if( SfBase_EscState & ESC_STATE_RUNNING ) 
    {
        Handrail_speed_freq = Measure_handrail_speed(psHDL);
        Escalator_speed_freq = ( EscRTBuff[41] << 8 | EscRTBuff[40] );
        Handrail_speed = (( Handrail_speed_freq * 2 * 314 * ROLLER_HR_RADIUS ) / ( HR_PULSES_PER_REV * 100 ));    
        Escalator_speed = (( Escalator_speed_freq * NOMINAL_SPEED ) / ( ( MOTOR_RPM * MOTOR_PLUSE_PER_REV ) / 60 ) );
        EscRTBuff[80] = Handrail_speed >> 8;
        EscRTBuff[81] = Handrail_speed;
        EscRTBuff[82] = Escalator_speed >> 8;
        EscRTBuff[83] = Escalator_speed;
        
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
                    EscRTBuff[78] = 1;
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
                    EscRTBuff[79] = 1;
                }
            }
        }
        else
        {
            psHDL->HR_Fault_timer = 0;
        }
        
        *(psHDL->ptHDLDataBuff) = Handrail_speed_freq;
    }
    
    escState_old = SfBase_EscState;
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
//    u8 First_HS_edge_detected,Timer_HS_shortcircuit = 0;
    
    
    
}



/******************************  END OF FILE  *********************************/






