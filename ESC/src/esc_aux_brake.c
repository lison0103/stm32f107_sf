/*******************************************************************************
* File Name          : esc_aux_brake.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 12/20/2016
* Description        : 12/20/2016 adding UNINTENTIONAL CHANGE DIRECTION to aux lock
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"
#include "bsp_iocfg.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define AUX_DELYA_FB_MASK               (EscRtData. Cfg_Input_Mask[14] &(0x40u))
#define AUX_DELAY_FB_INPUT              (EscRtData. Cfg_Input_Level[14] &(0x40u))

#define AUX_RELAY_FB_MASK               (EscRtData. Cfg_Input_Mask[14] &(0x40u))
#define AUX_RELAY_FB_INPUT              (EscRtData. Cfg_Input_Level[14] &(0x40u))

#define AUX_BRAKE_TEST_MASK             (EscRtData. Cfg_Input_Mask[14] &(0x40u))
#define AUX_BRAKE_TEST_INPUT            (EscRtData. Cfg_Input_Level[14] &(0x40u))

#define AUX_BRAKE_FEEDBACK_MASK1        (EscRtData. Cfg_Input_Mask[4] &(0x02u))
#define AUX_BRAKE_FEEDBACK_INPUT1       (EscRtData. Cfg_Input_Level[4] &(0x02u))

#define AUX_BRAKE_FEEDBACK_MASK2        (EscRtData. Cfg_Input_Mask[4] &(0x04u))
#define AUX_BRAKE_FEEDBACK_INPUT2       (EscRtData. Cfg_Input_Level[4] &(0x04u))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u8 aux_sensor_fb_mask,aux_sensor_fb_input;

/* Private function prototypes -----------------------------------------------*/
static void AuxBrakeSupervision(void);
static void AuxBrakeLock(void);
static void AuxBrakeUnlock(void);

/*******************************************************************************
* Function Name  : AuxBrakeSupervision
* Description    : Check Aux output,comparing with aux brake/relay feedback         
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void AuxBrakeSupervision(void)
{
  u16 aux_brake_error_tms=0u; 
  static u16 PawlSupervsionTime=0u,RunStatePawlTime=0u;
  static u16 aux_delay_fb_error_tms=0u,aux_relay_fb_error_tms=0u;
  
  
  aux_brake_error_tms = AUX_BRAKE_SUPERVISION_TIME; 
  aux_brake_error_tms = aux_brake_error_tms*1000u;
  
  /*
  ** Aux relay k4.0 and K4.3 drive feedback 
  */
  if(AUX_RELAY_FB_MASK)
  {  
    /* Relay output, feedback;  Relay deactive, no feedback*/
    if( ((CMD_FLAG7 & 0x01u) && AUX_RELAY_FB_INPUT)    ||    ((!(CMD_FLAG7 & 0x01u)) && (!AUX_RELAY_FB_INPUT)) )
    {
       if(aux_relay_fb_error_tms > CONTACTOR_FEEDBACK_FILTER ) 
       {
          EN_ERROR37 |= 0x01u;          /* F288  Supervision aux brake contactor  */                 
       } 
       else
       {
         aux_relay_fb_error_tms += 5u;
       }  
    }  
    else
    {
      aux_relay_fb_error_tms = 0u;  
    }  
  }

  /*
  ** Aux relay k4.2 drive feedback 
  */
  if(AUX_DELYA_FB_MASK)
  {  
    /* Relay output, feedback;  Relay deactive, no feedback*/
    if( ((CMD_FLAG7 & 0x10u) && AUX_DELAY_FB_INPUT) || ((!(CMD_FLAG7 & 0x10u)) && (!AUX_DELAY_FB_INPUT)) )
    {
       if(aux_delay_fb_error_tms > CONTACTOR_FEEDBACK_FILTER) 
       {
         EN_ERROR37 |= 0x02u;       /* F289 Supervision aux brake delay contactor  */  
       } 
       else
       {
         aux_delay_fb_error_tms += 5u;
       }  
    }  
    else
    {
      aux_delay_fb_error_tms = 0u;  
    }  
  }
    
  /*check  AUX feedback in run state,  
  ** Run state 
  ** normal or running down
  */
  if((SfBase_EscState == ESC_RUN_STATE) && ((CMD_FLAG3 & 0x10u) || (CMD_FLAG6 & 0x02u)) )  
  {
    if( (aux_sensor_fb_input == aux_sensor_fb_mask) )       /* aux feedback actived */
    {
      RunStatePawlTime = 0u;        
    }                           
    else                                /* Aux Brake Sensor signal == False */
    {
      if(RunStatePawlTime > 100u)        /* 100ms */
      { 
        if(!(aux_sensor_fb_input&0x01u)) 
        {
          EN_ERROR36 |= 0x40u;          /* F286  Aux bake status 1  */                 
        }   
          
        if((aux_sensor_fb_mask&0x02u) != (aux_sensor_fb_input&0x02u)) 
        {  
          EN_ERROR36 |= 0x80u;          /* F287  Aux bake status 2 */       
        }   
      }
      else
      {  
        RunStatePawlTime += 5u;             
      }  
    }    
  } 
  else
  {
    RunStatePawlTime = 0u;      
  }
  
  /*
  ** not run state, 
  ** not Inspection mode 
  ** not Capacitor testing
  */ 
  if((SfBase_EscState!=ESC_RUN_STATE) && (CMD_FLAG3 & 0x10u) && (!(CMD_FLAG7 & 0x20u)))  
  {
    /* K4.0 & K 4.3 closed, but not AUX feedback */
    
    if(((CMD_FLAG7&0X01u) &&  (aux_sensor_fb_input != aux_sensor_fb_mask))  || \
      ((!(CMD_FLAG7&0X01u)) && (aux_sensor_fb_input)))         
    {
      if(PawlSupervsionTime > aux_brake_error_tms)
      {
        /* Aux unlock */
        if( CMD_FLAG7&0X01u ) 
        {
          if(!(aux_sensor_fb_input & 0x01u)) 
          {
            EN_ERROR36 |= 0x40u;          /* F286  Aux bake status 1  */             
          }  
          
          if((aux_sensor_fb_mask & 0x02u) && (!(aux_sensor_fb_input & 0x02u)))
          {
            EN_ERROR36 |= 0x80u;          /* F287  Aux bake status 2 */       
          }  
        }
        else /* Aux lock */
        {
          if(aux_sensor_fb_input & 0x01u) 
          {
            EN_ERROR36 |= 0x40u;          /* F286  Aux bake status 1  */                 
          }   
            
          if(aux_sensor_fb_input & 0x02u) 
          {  
            EN_ERROR36 |= 0x80u;          /* F287  Aux bake status 2 */       
          }   
        }        
      }
      else
      {
        PawlSupervsionTime += 5u;     
      }  
    } 
    else
    {
      PawlSupervsionTime = 0u; 
    }  
  }
  else
  {
    PawlSupervsionTime = 0u;
  }   
  
}

/*******************************************************************************
* Function Name  : AuxBrakeUnlock
* Description    :        
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void AuxBrakeUnlock(void)
{
  /* reset enanble ( run ) flag */
  CMD_FLAG7 &= ~0x0Cu;   
  
  if( (SfBase_EscState == ESC_READY_STATE) )
  {
    /* Inspection mode or AUX brake locked  */ 
    if((!(CMD_FLAG3 & 0x10u)) || (aux_sensor_fb_input == 0u)) 
    {
      CMD_FLAG7 |= 0x04u;  /* Aux Straing process Enable flag */  
    }  
  }  
  else if( (SfBase_EscState == ESC_STARTING_PROCESS_STATE) )
  {
    /*
    ** MCU1 will be K4.0 = true
    ** MCU2 will be K4.3 = true
    */
    CMD_FLAG7 |= 0x01u;   	
    
    CMD_FLAG7 |= 0x10u;  /* K4.2 = ture */    
    
    if( (!(CMD_FLAG3 & 0x10u)) && (CMD_FLAG6 & 0x01u) ) /* Inspection mode and upwards */ 
    {  
       CMD_FLAG7 |= 0x08u;  /* Aux RUN Enable */                                 
    } 
    else if( (aux_sensor_fb_input == aux_sensor_fb_mask) ) /* Aux Brake feedback active */
    {
       CMD_FLAG7 |= 0x08u;  /* Aux RUN Enable */         
    }  
    else
    {
    }  
  }
  else
  {  
  }      
}

/*******************************************************************************
* Function Name  : AuxBrakeLock
* Description    :        
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void AuxBrakeLock(void)
{
  static u8 aux_run_flag_old=0u;
                               
  static u16 TimeLockDelay=0u, CapacitatorDurationTimer=0u;
  
  CMD_FLAG7 &= ~0x20u;

  /* 
  ** Total fault number: 
  ** Overspeed Testmotor Sensor 1, 2
  ** Overspeed Mainshaft Sensor 1, 2, 3
  ** Lowspeed Testmotor Sensor 1, 2
  ** Lowspeed Mainshaft Sensor 1, 2, 3
  ** Timeout stopping Process Testmotor Sensor 1, 2
  ** Timeout stopping Process Mainshaft Sensor 1, 2, 3
  ** UNINTENTIONAL CHANGE DIRECTION F343/F344
  **   
  **  
  */ 
  if( (EN_ERROR33 & 0x0fu) || (EN_ERROR34 & 0x01u) || (EN_ERROR43 & 0x80u) ||  (EN_ERROR44 & 0x01u) || (EN_ERROR52 & 0x06u) || (EN_ERROR53 & 0x7eu))    
  {
    CMD_FLAG7 &= ~0x01u;        /* K4.0 and K4.3 = off */   	
    CMD_FLAG7 |= 0x10u;         /* K4.2 = true */  
  }  
  else if( (AUX_BRAKE_TEST_INPUT) && (!(CMD_FLAG3 & 0x10u)) && (aux_run_flag_old&0x02u) && (!(CMD_FLAG6&0x02u)) )  
  {
    /*
    ** Test enable ,inspection mode, run downwards and stop
    */
    CMD_FLAG7 &= ~0x01u;      	/* K4.0 and K4.3 = off */   
    CMD_FLAG7 |= 0x10u;         /* K4.2 = true */  
  }  
  else if((SfBase_EscState == ESC_INIT_STATE) || (SfBase_EscState == ESC_READY_STATE) || (SfBase_EscState == ESC_INTERM_STATE) || \
          (SfBase_EscState == ESC_FAULT_STATE) || (SfBase_EscState == ESC_STOPPING_PROCESS_STATE))
  {  
     TimeLockDelay += 5u;
    if( TimeLockDelay++ > 10000u ) /* 10s after stop */
    {  
      CMD_FLAG7 &= ~0x01u;      	/* K4.0 and K4.3 = off */   
      
      if( CAPACITOR_TIME_MEASUREMENT ) /* capacitator supervise enable */
      {       
        CapacitatorDurationTimer++;
        if(CapacitatorDurationTimer>2000u)  /* 10s */
        {
          CMD_FLAG7 |= 0x10u;      /* K4.2 = ture */
        }  
        else 
        {
          CMD_FLAG7 &= ~0x10u;      /* K4.2 = false */
          CMD_FLAG7 |= 0x20u;       /* Capacitor testing */
          
          if( (CapacitatorDurationTimer<300u) && (aux_sensor_fb_input != aux_sensor_fb_mask) )
          {  
            EN_ERROR37 |= 0x02u;       /* F289   */  
          }  
        }
      } 
      else  /* No K4.2*/   
      {  
        CMD_FLAG7 |= 0x10u;      /* K4.2 = ture */                                      
      }     
    }              
  }   
  else
  {
    TimeLockDelay = 0u;    
    CapacitatorDurationTimer = 0u;   
  }    
     
  aux_run_flag_old = CMD_FLAG6;
}

/*******************************************************************************
* Function Name  : Aux_Brake_CS
* Description    :              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Aux_Brake_CS(void)
{
  aux_sensor_fb_mask=0u;
  aux_sensor_fb_input=0u;
  
  
  if( (AUX_BRAKE_ENABLE) ) 
  {
    if(AUX_BRAKE_FEEDBACK_MASK2) 
    {
      aux_sensor_fb_mask = 0x03u;
      if(AUX_BRAKE_FEEDBACK_INPUT1) 
      {
        aux_sensor_fb_input |= 0x01u;
      }  
      
      if(AUX_BRAKE_FEEDBACK_INPUT2) 
      {
        aux_sensor_fb_input |= 0x02u;
      }  
    }
    else
    {
      aux_sensor_fb_mask = 0x01u;
      
      if(AUX_BRAKE_FEEDBACK_INPUT1) 
      {
        aux_sensor_fb_input |= 0x01u;
      }  
    }  
    
    AuxBrakeLock();
  
    AuxBrakeUnlock();
  
    AuxBrakeSupervision();
  }
  
  if( CMD_FLAG7 & 0x01u )
  {
    AUX_RELAY_ON();   
  } 
  else
  {
    AUX_RELAY_OFF();   
  }  
}

/******************************  END OF FILE  *********************************/

