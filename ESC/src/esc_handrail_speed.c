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
u16 ROLLER_HR_RADIUS = ( 0.1 * 1000 ); //mm
u16 HR_PULSES_PER_REV = 2;

HDLITEM HDL_Left = 
{
    
  0,0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  
};

HDLITEM HDL_Right = 
{
    
  0,0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  
};


/*******************************************************************************
* Function Name  : HR_Speed_Ready
* Description    : Check the escalator handrail speed ready.
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
* Description    : Check the escalator handrail speed run.
* Input          : psHDL: handrail speed sensor id                
* Output         : None
* Return         : None
*******************************************************************************/
void HR_Speed_Run_EN115(HDLITEM* psHDL)
{
    u16 Escalator_speed,Handrail_speed,Handrail_speed_freq,Escalator_speed_freq = 0;
    static u16 HR_Fault_timer = 0;
    static u16 delay_no_pulse_tms = 0;
    
    if( ( SfBase_EscState & ESC_STATE_RUNNING ) && ( !(escState_old & ESC_STATE_RUNNING) ) )
    {
        delay_no_pulse_tms = 0;
    }    

    
    if( SfBase_EscState & ESC_STATE_RUNNING ) 
    {
        Handrail_speed_freq = Measure_handrail_speed(psHDL);
        Escalator_speed_freq = ( EscRTBuff[41] << 8 | EscRTBuff[40] );
        Handrail_speed = (( Handrail_speed_freq * 2 * ( 314 / 100 ) * ROLLER_HR_RADIUS ) / HR_PULSES_PER_REV );    
        Escalator_speed = (( Escalator_speed_freq * NOMINAL_SPEED * 1000 ) / ( ( MOTOR_RPM * MOTOR_PLUSE_PER_REV ) / 60 ) );

        if( ( delay_no_pulse_tms * SYSTEMTICK ) > DELAY_NO_PULSE_CHECKING )
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
                }            
            }          
        }
        else
        {
            delay_no_pulse_tms++;
        }
        

        
        HR_Fault_timer++;
        if( (Handrail_speed <= (( Escalator_speed * 87 ) / 100 )) || (Handrail_speed >= (( Escalator_speed * 113 ) / 100 )) )
        {
            if( ( HR_Fault_timer * SYSTEMTICK ) >= HR_FAULT_TIME )
            {
                if( SfBase_EscState & ESC_STATE_INSP ) 
                {         
                    /* handrail speed warning */
                }
                else
                {
                    /* handrail speed fault */
                }
            }
        }
        else
        {
            HR_Fault_timer = 0;
        }
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



/******************************  END OF FILE  *********************************/






