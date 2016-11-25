/*******************************************************************************
* File Name          : esc_aux_brake.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 11/07/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"
#include "bsp_iocfg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void MainShaftBrakePawlSupervision(void);

/*
#define AUX_BRAKE_FEEDBACK_INPUT   (EscRtData. SafetyInputData[3] & 0x04)
*/

#define AUX_BRAKE_FEEDBACK_INPUT   (EscRtData. Cfg_Input_Level[0] &(0x01u))

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
  static u16 PawlSupervsionTime=0u,RunStatePawlTime=0u;
  
  /*check  AUX feedback in run state*/  
#if 0  
  if((SfBase_EscState == ESC_RUN_STATE))  
#else    
  if((SfBase_EscState == ESC_RUN_STATE) && (!(CMD_FLAG3 & 0x20u)))  /* not in inspection mode */
#endif  
  {
    if( AUX_BRAKE_FEEDBACK_INPUT ) 
    {
      RunStatePawlTime = 0u;        
    }                           
    else                                /* Aux Brake Sensor signal == False */
    {
      if(RunStatePawlTime > 20u)        /* SYSTEMTICK=5, 5*20=100ms */
      {
        EN_ERROR38 |= 0x10u;            /* F300                     */       
      }
      else
      {  
        RunStatePawlTime++;             
      }  
    }    
  } 
  else
  {
    RunStatePawlTime = 0u;      
  }
  
  if((SfBase_EscState!=ESC_RUN_STATE) && (!(CMD_FLAG3 & 0x20u)))  /* not run state, not Inspection mode */ 
  {
    /* K4.0 & K 4.3 closed, but not AUX feedback */
    if((CMD_FLAG7&0X01u) && (CMD_OMC_FLAG7&0X01u) && (!(AUX_BRAKE_FEEDBACK_INPUT)))         
    {
      if(PawlSupervsionTime<10000u) 
      {
        PawlSupervsionTime++;      
      }  
    } 
    else if( ((!(CMD_FLAG7&0X01u)) || (!(CMD_OMC_FLAG7&0X01u))) && (AUX_BRAKE_FEEDBACK_INPUT) ) 
    {
      if(PawlSupervsionTime<10000u) 
      {
        PawlSupervsionTime++;         
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
  CMD_FLAG7 &= ~0x0Cu;   
  
  if( (SfBase_EscState == ESC_READY_STATE) )
  {
    if((CMD_FLAG3 & 0x20u) || (!(AUX_BRAKE_FEEDBACK_INPUT))) /* Inspection mode or AUX brake locked  */ 
    {
      CMD_FLAG7 |= 0x04u;  /* Straing process Enable */  
    }  
  }  
  else if( (SfBase_EscState == ESC_STARTING_PROCESS_STATE) )
  {
    AUX_RELAY_ON();   	
    CMD_FLAG7 |= 0x10u;  /* K4.2 = ture */    
    
    if( (CMD_FLAG3 & 0x20u) && (CMD_FLAG6 & 0x01u) ) /* Inspection mode and upwards */ 
    {  
       CMD_FLAG7 |= 0x08u;  /* Aux RUN Enable */                                 
    } 
    else if( (AUX_BRAKE_FEEDBACK_INPUT) )
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
  static u16 TimeLockDelay=0u, CapacitatorDurationTimer=0u;
  
  if( (EN_ERROR33 & 0x63u) )    /* any other condiction ? */
  {
    AUX_RELAY_OFF();   	
    CMD_FLAG7 |= 0x10u;         /* K4.2 = ture */  
  }  
  else if((SfBase_EscState == ESC_INIT_STATE) || (SfBase_EscState == ESC_READY_STATE) || (SfBase_EscState == ESC_INTERM_STATE) || (SfBase_EscState == ESC_FAULT_STATE) || (SfBase_EscState == ESC_STOPPING_PROCESS_STATE))
  {  
    if( TimeLockDelay++ > 2000u ) /* 2000*5= 10s */
    {  
      AUX_RELAY_OFF();                        
      if( CAPACITOR_TIME_MEASUREMENT ) /* capacitator supervise enable */
      {
        CMD_FLAG7 &= ~0x10u;      /* K4.2 = false */
        
        if( AUX_BRAKE_FEEDBACK_INPUT )   
        {
          if(CapacitatorDurationTimer++>300u)   /* 300*5= 1.5s */
          {
            EN_ERROR3 |= 0x04u;                           
          }  
        }  
      } 
      else    
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
  
  if( (AUX_BRAKE_ENABLE) ) 
  {
   
    AuxBrakeLock();
  
    AuxBrakeUnlock();
  
    AuxBrakeSupervision();
  }
  
}

/******************************  END OF FILE  *********************************/

