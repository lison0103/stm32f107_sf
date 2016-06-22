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

/* motor speed */
#define MAX_SPEED       ( ( F1 * 115 ) / 100 )
#define MIN_SPEED       ( ( F1 * 10 ) / 100 )

/* stopping distance */
#define CONVERSION      ( ( NOMINAL_SPEED / MOTOR_RPM ) * MOTOR_PLUSE_PER_REV )
#define MAX_DISTANCE    ( ( 12 * 2 * F1 ) / 10 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
u16 Measure_motor_speed(MTRFREQITEM* ptMTR);


/* variable */
u32 time_running_tms = 0;
u16 SfBase_EscState = ESC_STATE_READY;;
u8 First_motorspeed_edge_detected = 0;


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
        0,0,0,
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
        0,0,0,
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
void Motor_Speed_Ready(MTRFREQITEM* ptMTR)
{

    u16 Escalator_speed = 0;
    
    if( SfBase_EscState & ESC_STATE_READY ) 
    {
               
        Escalator_speed = Measure_motor_speed(ptMTR);
        
        if( Escalator_speed > 1 )
        {
            /* fault */
            *(ptMTR->pcErrorCodeBuff) |= 0x01;
            EN_ERROR1 |= 0x01;
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
void Motor_Speed_Run_EN115(MTRFREQITEM* ptMTR)
{
    
    u16 Escalator_speed = 0;
    	

//    if( SfBase_EscState & ESC_STATE_RUNNING  || SfBase_EscState & ESC_STATE_STOP)
    if( SfBase_EscState & ESC_STATE_RUNNING  || ( (SfBase_EscState & ESC_STATE_STOP) && (!(SfBase_EscState & ESC_STATE_READY))))
    {      
        
//        if( ( time_running_tms * SYSTEMTICK ) > UNDERSPEED_TIME )
//        {
//            SfBase_EscState |= ESC_STATE_RUN5S;                
//        }
//        else
//        {
//            time_running_tms++;
//        }        
        
        Escalator_speed = Measure_motor_speed(ptMTR);
        MTR_MAX_SPEED = MAX_SPEED;
        MTR_MIN_SPEED = MIN_SPEED;
        
        if( Escalator_speed >= MAX_SPEED )
        {
            
            if(ptMTR->MtrSpeedHigh115Cnt < 100) ptMTR->MtrSpeedHigh115Cnt++;
            
            if(ptMTR->MtrSpeedHigh115Cnt >= 5)
            {
                /* overspeed fault */
                *(ptMTR->pcErrorCodeBuff) |= 0x02;
                EN_ERROR1 |= 0x02;
            } 
           
        }
        else
        {
            
            ptMTR->MtrSpeedHigh115Cnt = 0;
            
        }
        
        if( SfBase_EscState & ESC_STATE_RUN5S )
        {
            if( Escalator_speed <= MIN_SPEED )
            {
                /* underspeed fault */
                *(ptMTR->pcErrorCodeBuff) |= 0x04;
                EN_ERROR1 |= 0x04;
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
u16 Measure_motor_speed(MTRFREQITEM* ptMTR)
{
    u16 current_motor_speed_sensor,i = 0;
    u8 timeDelayCnt = 100;    
     
    
    /* 20 * 5 = 100ms */
    ptMTR->Tms_counter++;
    if( ( ptMTR->Tms_counter * SYSTEMTICK ) >= timeDelayCnt) 
    { 
        ptMTR->Tms_counter = 0; 
        
        for(i=9; i>0; i--)                                                   
        {   
            ptMTR->pulseArray[i] = ptMTR->pulseArray[i-1];
        }
        ptMTR->pulseArray[0] = ptMTR->rt_pulse;
        ptMTR->rt_pulse=0;
        
    }
    current_motor_speed_sensor = 0;
    /* 100ms * 10 = 1s */
    for(i=0;i<10;i++) 
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
    static u16 Timer_motorspeed_shortcircuit = 0;

    
    if( SfBase_EscState & ESC_STATE_RUNNING )
    {  
        if( First_motorspeed_edge_detected == 0 )
        {
            First_motorspeed_edge_detected = 1;
            Timer_motorspeed_shortcircuit = 0;
            TIM_Cmd(TIM5, DISABLE);
            TIM5_Int_Init(65535,71);
            
        }    
        else
        {
            Timer_motorspeed_shortcircuit = TIM_GetCounter(TIM5);
            if( Timer_motorspeed_shortcircuit < SSM_SHORTCIRCUIT_TIME )
            {
                Timer_motorspeed_shortcircuit = 0;               
                TIM_SetCounter(TIM5,0); 
                
                /* Fault ¨C Motorspeed Sensor shortcircuited */
                SHORTCIRCUIT_ERROR |= 0x01;
                EN_ERROR4 |= 0x01;
            }
            else
            {
                Timer_motorspeed_shortcircuit = 0;             
                TIM_SetCounter(TIM5,0);  
                
                SHORTCIRCUIT_ERROR &= ~0x01;
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
void Check_Stopping_Distance(MTRFREQITEM* ptMTR)
{   
    
    if(SfBase_EscState & ESC_STATE_STOP) 
    {
        if((ptMTR->rt_brake_pulse) > MAX_DISTANCE)
        {
            /* stopping distance fault */  
            *(ptMTR->pcErrorCodeBuff) |= 0x08;
            EN_ERROR1 |= 0x08;
        }   
           
        
        if( !(ptMTR->rt_brake_stop))
        {
            ptMTR->BrakeCalTms++;
            if( ( ptMTR->BrakeCalTms * SYSTEMTICK ) > 1000 ) 
            {
                
//                if(( ptMTR->rt_brake_pulse ) && ( ptMTR->rt_brake_pulse == ptMTR->last_brake_pulse ))   
                if( ptMTR->rt_brake_pulse == ptMTR->last_brake_pulse )     
                {
                    /* record the escalator stopping distance */
                    *(ptMTR->ptBrakeDistanceBuff) = ptMTR->rt_brake_pulse;
                    ptMTR->rt_brake_pulse = 0;
                    ptMTR->rt_brake_stop = 1;
                    
                }
                else
                {
                    ptMTR->last_brake_pulse = ptMTR->rt_brake_pulse;
                }
                
                ptMTR->BrakeCalTms = 0;
                
            }    
        }
    } 
    else
    {
        ptMTR->BrakeCalTms=0;
        ptMTR->rt_brake_pulse = 0;
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
    static u16 escState_old = 0; 
    
    
    Motor_Speed_Ready(&MTRITEM[0]);
    Motor_Speed_Ready(&MTRITEM[1]);		
    
    if((SfBase_EscState & ESC_STATE_RUNNING) && (!(escState_old & ESC_STATE_RUNNING))) 
    { 
        First_motorspeed_edge_detected = 0;
    } 
    else if(( SfBase_EscState & ESC_STATE_STOP ) && ( !(escState_old & ESC_STATE_STOP)))
    {
        MTRITEM[0].last_brake_pulse = MTRITEM[0].rt_brake_pulse;
        MTRITEM[1].last_brake_pulse = MTRITEM[1].rt_brake_pulse;
        MTRITEM[0].rt_brake_stop = 0;
        MTRITEM[1].rt_brake_stop = 0;
    } 
    
    Motor_Speed_Run_EN115(&MTRITEM[0]);
    Motor_Speed_Run_EN115(&MTRITEM[1]);
    
    Check_Stopping_Distance(&MTRITEM[0]);
    Check_Stopping_Distance(&MTRITEM[1]);
    
    
    escState_old = SfBase_EscState;                                      

}

/*******************************************************************************
* Function Name  : key_run_detect
* Description    : Esc key run.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void key_run_detect(void)
{
    static u16 key_on_tms = 0;   
    
    if( ( INPUT_PORT17_24 & INPUT_PORT22_MASK ) && ( sf_wdt_check_en == 0 ))
    {
        if(key_on_tms < 5000) key_on_tms++;
        
    }
    
    if(( key_on_tms * SYSTEMTICK == 2000))
    {
        CMD_FLAG1 |= 0x01;
    }	
    
    if(!(INPUT_PORT17_24 & INPUT_PORT22_MASK) || ( sf_wdt_check_en == 1 )) 
    {
        key_on_tms = 0;	
    }
}


/*******************************************************************************
* Function Name  : sfEscStateCheck
* Description    : Esc state check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void sfEscStateCheck(void)
{
  static u16 sf_running_tms,sf_stopping_tms = 0;
  static u16 sf_reset_tms = 0;
  static u8 key_on = 0;
 
  
  /* esc running */
  if( ( INPUT_PORT17_24 & INPUT_PORT22_MASK ) && (SfBase_EscState & ESC_STATE_READY) )
  {
      key_on = 1;
  }
  

  key_run_detect();
  
  if( ( INPUT_PORT17_24 & INPUT_PORT22_MASK ) && (CMD_FLAG1 & 0x01) && ( key_on == 1 ) )
  {
      SfBase_EscState &= ~ESC_STATE_STOP;
      SfBase_EscState &= ~ESC_STATE_READY;
      
      SfBase_EscState |= ESC_STATE_RUNNING;
      
      if(( (sf_running_tms * SYSTEMTICK) > 2500 ) && (*(MTRITEM[0].ptFreqBuff) > MIN_SPEED ) && (*(MTRITEM[1].ptFreqBuff) > MIN_SPEED ))
      {
          SfBase_EscState |= ESC_STATE_SPEEDUP;
      }
      
      if( (sf_running_tms * SYSTEMTICK) > 6000 )
      {
          SfBase_EscState |= ESC_STATE_RUN5S;
      }
      else
      {
          sf_running_tms++;
      }
      
      sf_stopping_tms = 0;
      
      sf_reset_tms = 0;
  }
  else
  {
      SfBase_EscState &= ~ESC_STATE_RUNNING;
      SfBase_EscState &= ~ESC_STATE_SPEEDUP;
      SfBase_EscState &= ~ESC_STATE_RUN5S;
      
      SfBase_EscState |= ESC_STATE_STOP;
      
      key_on = 0;
      
      if(( (sf_stopping_tms * SYSTEMTICK) > 3000 ) && (MTRITEM[0].rt_brake_stop == 1) && (MTRITEM[1].rt_brake_stop == 1) )
      {
          SfBase_EscState |= ESC_STATE_READY;
      }
      else
      {        
          sf_stopping_tms++;
      }
      
      /* for test reset the value-------------------------------------*/
      if( (sf_reset_tms * SYSTEMTICK) > 20000 )
      {
          for(u8 i = 30; i < 200; i++ )
          {
              EscRTBuff[i] = 0;
          }
          sf_reset_tms = 0;
      }
      else
      {
          sf_reset_tms++;
      }
      
      sf_running_tms = 0;
      
      CMD_FLAG1 &= ~0x01;
  }
    
   
  
}





/******************************  END OF FILE  *********************************/






