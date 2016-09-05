/*******************************************************************************
* File Name          : esc_motor_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Description        : This file contains esc motor speed.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_motor_speed.h"
#include "delay.h"
#include "initial_devices.h"
#include "timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static u16 Measure_motor_speed(MTRFREQITEM* ptMTR);
static void Motor_Speed_Ready(MTRFREQITEM* ptMTR);
static void Motor_Speed_Run_EN115(MTRFREQITEM* ptMTR);
static void Check_Stopping_Distance(MTRFREQITEM* ptMTR);


/* variable */
u32 time_running_tms = 0u;
u16 SfBase_EscState = ESC_STATE_READY;;
static u8 First_motorspeed_edge_detected = 0u;


MTRFREQITEM MTRITEM[2]=
{
    {
        0,0,
        
        0,   
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
        
        0,0,
        
        (u16*)&EscRTBuff[40], 
        (u16*)&EscRTBuff[44],  
        
        0,
        {0,0,0,0,0,0,0,0,0,0,0,0},
        0,
        0,0,1,
        &EscRTBuff[48]
    },
    {
        0,0,
        
        0,  
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
        
        0,0,
        
        (u16*)&EscRTBuff[42], 
        (u16*)&EscRTBuff[46],  
        
        0,
        {0,0,0,0,0,0,0,0,0,0,0,0},
        0,
        0,0,1,
        &EscRTBuff[49]
    }
};




/*******************************************************************************
* Function Name  : Motor_Speed_Ready
* Description    : Pulse detection when escalator is stopped.
* Input          : ptMTR: motor speed sensor id          
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Speed_Ready(MTRFREQITEM* ptMTR)
{

    u16 Escalator_speed = 0u;
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {
               
        Escalator_speed = Measure_motor_speed(ptMTR);
        
        if( Escalator_speed > 1u )
        {
            /* fault */
            *(ptMTR->pcErrorCodeBuff) |= 0x01u;
            EN_ERROR1 |= 0x01u;
        }            
    } 
    

}


/*******************************************************************************
* Function Name  : Motor_Speed_Run_EN115
* Description    : Overspeed and underspeed detection (Sensor X)
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
static void Motor_Speed_Run_EN115(MTRFREQITEM* ptMTR)
{
    
    u16 Escalator_speed = 0u;
    	

    if( ( SfBase_EscState & ESC_STATE_RUN ) || ( (SfBase_EscState & ESC_STATE_STOP) && (!(SfBase_EscState & ESC_STATE_READY))))
    {      
               
        
        Escalator_speed = Measure_motor_speed(ptMTR);
        MTR_MAX_SPEED = MAX_SPEED;
        MTR_MIN_SPEED = MIN_SPEED;
        
        if( Escalator_speed >= MAX_SPEED )
        {

            if( ptMTR->MtrSpeedHigh115Cnt >= 5u )
            {
                /* overspeed fault */
                *(ptMTR->pcErrorCodeBuff) |= 0x02u;
                EN_ERROR1 |= 0x02u;
            } 
            else
            {
                ptMTR->MtrSpeedHigh115Cnt++;
            }  
           
        }
        else
        {
            
            ptMTR->MtrSpeedHigh115Cnt = 0u;
            
            if( SfBase_EscState & ESC_STATE_RUN5S )
            {
                if( Escalator_speed <= MIN_SPEED )
                {
                    /* underspeed fault */
                    *(ptMTR->pcErrorCodeBuff) |= 0x04u;
                    EN_ERROR1 |= 0x04u;
                }
            }            
        }
        

     
        
        /* record the escalator speed */
        *ptMTR->ptFreqBuff = Escalator_speed;   

    }
    
}

/*******************************************************************************
* Function Name  : Measure_motor_speed
* Description    : Measure the escalator motor speed.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : current motor speed pulse
*******************************************************************************/
static u16 Measure_motor_speed(MTRFREQITEM* ptMTR)
{
    u16 current_motor_speed_sensor,i = 0u;
    u8 timeDelayCnt = 100u;    
     
    
    /* 20 * 5 = 100ms */
    ptMTR->Tms_counter++;
    if( ( ptMTR->Tms_counter * SYSTEMTICK ) >= timeDelayCnt) 
    { 
        ptMTR->Tms_counter = 0u; 
        
        for( i = 9u; i > 0u; i-- )                                                   
        {   
            ptMTR->pulseArray[i] = ptMTR->pulseArray[i - 1u];
        }
        ptMTR->pulseArray[0] = ptMTR->rt_pulse;
        ptMTR->rt_pulse = 0u;
        
    }
    current_motor_speed_sensor = 0u;
    /* 100ms * 10 = 1s */
    for( i = 0u; i < 10u; i++ ) 
    {
        current_motor_speed_sensor += ptMTR->pulseArray[i];
    }      

    return current_motor_speed_sensor;
}

/*******************************************************************************
* Function Name  : Motor_Speed_1_2_Shortcircuit_Run
* Description    : Motor speed sensor 1 and 2 shortcircuit checking
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Motor_Speed_1_2_Shortcircuit_Run(void)
{
    static u32 Timer_motorspeed_shortcircuit = 0u;

    
    if( SfBase_EscState & ESC_STATE_RUN )
    {  
        if( First_motorspeed_edge_detected == 0u )
        {
            First_motorspeed_edge_detected = 1u;
            Timer_motorspeed_shortcircuit = 0u;
            TIM_Cmd(TIM5, DISABLE);
            TIM5_Int_Init(65535u,71u);
            
        }    
        else
        {
            Timer_motorspeed_shortcircuit = TIM_GetCounter(TIM5);
            if( Timer_motorspeed_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_motorspeed_shortcircuit = 0u;               
                TIM_SetCounter(TIM5,0u); 
                
                /* Fault ¨C Motorspeed Sensor shortcircuited */
                SHORTCIRCUIT_ERROR |= 0x01u;
                EN_ERROR4 |= 0x01u;
            }
            else
            {
                Timer_motorspeed_shortcircuit = 0u;             
                TIM_SetCounter(TIM5,0u);  
                
                SHORTCIRCUIT_ERROR &= ~0x01u;
            }
        }
    }


}

/*******************************************************************************
* Function Name  : Check_Stopping_Distance
* Description    : Stopping distance.
* Input          : ptMTR: motor speed sensor id            
* Output         : None
* Return         : None
*******************************************************************************/
static void Check_Stopping_Distance(MTRFREQITEM* ptMTR)
{   
    
    if((SfBase_EscState & ESC_STATE_STOP ) && ( ptMTR->rt_brake_stop == 0u )) 
    {
        if((ptMTR->rt_brake_pulse) > MAX_DISTANCE)
        {
            /* stopping distance fault */  
            *(ptMTR->pcErrorCodeBuff) |= 0x08u;
            EN_ERROR1 |= 0x08u;
        }   
           
        
        if( !(ptMTR->rt_brake_stop))
        {
            ptMTR->BrakeCalTms++;
            if( ( ptMTR->BrakeCalTms * SYSTEMTICK ) > 1000u ) 
            {
                  
                if( ptMTR->rt_brake_pulse == ptMTR->last_brake_pulse )     
                {
                    /* record the escalator stopping distance */
                    *(ptMTR->ptBrakeDistanceBuff) = ptMTR->rt_brake_pulse;
                    ptMTR->rt_brake_pulse = 0u;
                    ptMTR->rt_brake_stop = 1u;
                    
                }
                else
                {
                    ptMTR->last_brake_pulse = ptMTR->rt_brake_pulse;
                }
                
                ptMTR->BrakeCalTms = 0u;
                
            }    
        }
    } 
    else
    {
        ptMTR->BrakeCalTms = 0u;
        ptMTR->rt_brake_pulse = 0u;
    }   

}


/*******************************************************************************
* Function Name  : ESC_Motor_Check
* Description    : Esc motor speed check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Motor_Check(void)
{
    static u16 escState_old = ESC_STATE_STOP; 
    
    
    Motor_Speed_Ready(&MTRITEM[0]);
    Motor_Speed_Ready(&MTRITEM[1]);		
    
    if((SfBase_EscState & ESC_STATE_RUN) && (!(escState_old & ESC_STATE_RUN))) 
    { 
        First_motorspeed_edge_detected = 0u;
    } 
    else if(( SfBase_EscState & ESC_STATE_STOP ) && ( !(escState_old & ESC_STATE_STOP)))
    {
        MTRITEM[0].last_brake_pulse = MTRITEM[0].rt_brake_pulse;
        MTRITEM[1].last_brake_pulse = MTRITEM[1].rt_brake_pulse;
        MTRITEM[0].rt_brake_stop = 0u;
        MTRITEM[1].rt_brake_stop = 0u;
    } 
    else
    {
      
    }
    
    Motor_Speed_Run_EN115(&MTRITEM[0]);
    Motor_Speed_Run_EN115(&MTRITEM[1]);
    
    Check_Stopping_Distance(&MTRITEM[0]);
    Check_Stopping_Distance(&MTRITEM[1]);
    
    
    escState_old = SfBase_EscState;                                      

}







/******************************  END OF FILE  *********************************/






