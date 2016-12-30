
/*******************************************************************************
* File Name          : esc_isp_normal_mode.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 11/07/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"
#include "esc_cmd_state.h" 

#define CB_AUTCONT_OPTIONS              EscRtData.DataFromControl[0][2]    


static void OperationModeSelection(void);
static void OperationModesReadyState(void);
static void OperationModesRunState(void);
static void OperationModesIntermState(void);
static void OperationModesStoppingProcess(void);
static void OperationalStop(void);
static void OperationModesStartingProcess(void);
static void OperationInspectionRun(void);
void OperationModesProcess(void);

static u16 Reversing_Tms,Reversing_Ts;
static u8 PDS_Upper_Mask, PDS_Upper_input,PDS_Lower_Mask, PDS_Lower_input;
static u8 OP_Old_State = 0xffu,OP_Run_Tms=0u;

static u16 Transportation_Ts,Lowspeed_Ts,Direction_Indication_Ts; 


/*******************************************************************************
* Function Name  : OperationModeSelection
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationModeSelection(void)
{
  u8 cb_mode=0u;
  
  cb_mode = (CB_AUTCONT_OPTIONS>>4) & 0x07u; 
  
  CMD_FLAG8 &= ~0x07u;  /* Continuos */
  
  switch(AUTCONT_OPTIONS)
  {
    case 0: /* Continious */
      
      if(cb_mode)
      {
        /* Warning? */
      } 
                  
      break;             
    case 1:   /* Intermitend */  
      if( cb_mode == 0x02u )
      {
        CMD_FLAG8 |= AUTCONT_OPTIONS;  /* Continuos */
      } 
      else if( cb_mode )
      {
        /* Warning? */
      } 
      else   
      {
        /* NONE */                
      }  
      
      break; 
    case 2:     /* Stdby */
      
      if( cb_mode == 0x01u )
      {
        CMD_FLAG8 |= AUTCONT_OPTIONS;  /* Continuos */
      } 
      else if( cb_mode )
      {
        /* Warning? */
      } 
      else
      {
        /* NONE */
      }  
      
      break; 
    case 3:     /* Intermitend + stby */
      
      if( !(cb_mode & 0x04u) )
      {
        CMD_FLAG8 |= cb_mode;
      }  
      else
      {
         /* Warning? */
      }  
      
      break; 
    case 4:     /* 2 Direction Mode */
      
      if( cb_mode == 0x04u )
      {
        CMD_FLAG8 |= AUTCONT_OPTIONS;
      }  
      else
      {
         /* Warning? */
      }        
      
      break; 
    case 5:     /* Y/D Switchback */
      
      if( cb_mode == 0x01u ) 
      {
        CMD_FLAG8 |= AUTCONT_OPTIONS;
      }  
      else
      {
         /* Warning? */
      }              
      
      break; 
    default:
      break;     
  } 
  
  /*Send mode to control board */
  EscDataToControl[0][2] &= ~ESC_MODE2_MASK;
  EscDataToControl[0][2] |= (u8)((CMD_FLAG8&0x07u)<<5u);
  
}
 
/*******************************************************************************
* Function Name  : OperationModesReadyState
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationModesReadyState(void)
{
  /* In normal mode */
  if ( CMD_ESC_RUN_MODE & ESC_NORMAL ) 
  {
    CheckUpDown_Key(&UpKey);
    CheckUpDown_Key(&DownKey);
  
    if(( CMD_ESC_RUN & ESC_UP ) || ( CMD_ESC_RUN & ESC_DOWN ))
    {
      if((!(AUX_BRAKE_ENABLE)) || (CMD_FLAG7 & 0x04u) ) 
      {  
        CMD_FLAG10 |= 0x02u;                            /*  enable to ESC_STARTING_PROCESS_STATE; */
      }  
      else
      {
        CMD_ESC_RUN &= ~(ESC_UP | ESC_DOWN);
      }  
    }  
  }
  else
  {
    /* Inspection up down key check*/
    Inspection_UpDown_Button();    
    if(( CMD_ESC_RUN & ESC_UP ) || ( CMD_ESC_RUN & ESC_DOWN ))
    {
      CMD_FLAG10 |= 0x02u;                              /*  ESC_STARTING_PROCESS_STATE;*/
    }  
  }  
}

/*******************************************************************************
* Function Name  : OperationModesRunState
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationModesRunState(void)
{
  u16 transportation_set_ts,Lowspeed_set_ts,reversing_set_ts;
  
  /* Data to control board */
  
  if(( EscRtData.DataFromControl[0][2] & ESC_FROM_CB_STATE_MASK ) == ESC_FROM_CB_SF_STATE_RUNNING )
  {
    /* removes the order (=0) when checks that the control board is in RUN state */
    EscDataToControl[0][1] &= ~ORDER_RUN;
  }
  else
  {
    /* activates the order (=1) when changes to RUN state */
    /* ORDER_RUN */ 
    EscDataToControl[0][1] |= ORDER_RUN;            
  }
        
  /* ORDER_SLOW_FAST */
  EscDataToControl[0][1] &= ~ORDER_SLOW_FAST;
        
  if(CMD_FLAG6 & 0x01u)
  {  
    /* DIRECTION_UP */
    EscDataToControl[0][1] |= ORDER_UP_DOWN;
  }
  else
  {  
    /* DIRECTION_DOWN */
    EscDataToControl[0][1] &= ~ORDER_UP_DOWN;
  }  
        
  /* ORDER_CAPACITOR */
  EscDataToControl[0][1] &= ~ORDER_CAPACITOR;                
        
  /* reset ORDER_STOP / ORDER_FAULT / ORDER_RESET / ORDER_CONTACTOR */
  EscDataToControl[0][1] &= ~0x1Eu;    
        
  /* stopping process */
  EscDataToControl[0][3] &= ~ESC_STOPPING_PROCESS_FINISH;
  
  /* run control */
  
  transportation_set_ts = TIME_TRANSPORTATION + ADDITIONAL_TRANSPORTATION_TIME;
  Lowspeed_set_ts = TIME_LOW_SPEED; 
  reversing_set_ts = ADDITIONAL_REVERSING_TIME + TIME_REVERSING;
  
  if( OP_Old_State != SfBase_EscState )
  {
    if( CMD_FLAG6 & 0x20u )  /* Reversing enter flag */
    {      
      /* Transportation timer done */
    }
    else
    {  
      Transportation_Ts = 0u;    /* Transportation timer reset */
      Lowspeed_Ts = 0u;          /* *? Lower speed timer reset  */
    }  
    
    CMD_FLAG6 &= ~0x08u; /* Low_Speed_Activ reset */
    Direction_Indication_Ts = 0u;
  }   
  
  if( !(CMD_FLAG8 & 0x07u) ) /* Not 2 direction or fast only*/
  {
    /* Order to control: ORDER_SLOW_FAST= False */
  } 
  else
  {
    if(((CMD_FLAG6 & 0x01u) && (PDS_Lower_input)) || ((CMD_FLAG6 & 0x02u) && (PDS_Upper_input)))
    {
      Transportation_Ts = 0u;
      
      CMD_FLAG6 &= ~0x08u;        /* Reset: Low_Speed_Activ */
      CMD_FLAG6 &= ~0x20u;        /* Reset: Active_Direction_Indication */
    }
    
    if( Transportation_Ts < transportation_set_ts ) /* Fast run */
    {
      /* Order to control: ORDER_SLOW_FAST= False */      
      if( OP_Run_Tms++ > 200u )
      {
        OP_Run_Tms = 0u;
        Transportation_Ts++;  
      }        
    }  
    else
    {                                                                                                                       
      if( ((CMD_FLAG6 & 0x01u) && (PDS_Upper_input)) || ((CMD_FLAG6 & 0x02u) && (PDS_Lower_input)) ) /* Reversing Enter */
      {
        CMD_FLAG6 |= 0x20u;              /* Set Reversing enter flag */
        Direction_Indication_Ts = 0u;    /* Reset timer */
        CMD_FLAG6 &= ~0x08u;             /* Reset Low speed flag */ 
      }  
      
      if( (Direction_Indication_Ts < reversing_set_ts) && (CMD_FLAG6 & 0x20u) )
      {
        /* Order to control: ORDER_SLOW_FAST= False */
        
        if( OP_Run_Tms++ > 200u )
        {
          OP_Run_Tms = 0u;
          Direction_Indication_Ts++;  
        }        
      }  
      else
      {
        CMD_FLAG6 &= ~0x20u;    /* Reset Reversing enter flag */
        
        if( CMD_FLAG8 & 0x02u ) /* Low speed mode */
        {
          if(!(CMD_FLAG6 & 0x40u)) /* Low speed active, restart timer */
          {
            CMD_FLAG6 |= 0x40u;
            Lowspeed_Ts = 0u;
          }
          else if((CMD_FLAG6 & 0x10u) && (Lowspeed_Ts >= Lowspeed_set_ts))
          {
            /* Interm Stop  */
            
          }  
          else 
          {
            /* Low speed */
            if( OP_Run_Tms++ > 200u )
            {
              OP_Run_Tms = 0u;
              Lowspeed_Ts++;  
            }        
          }
        } 
        else /* Interm Stop  */
        {
          /* Interm Stop  */
          
        }  
      }  
    }
  }     
}                                                   
                            
/*******************************************************************************
* Function Name  : OperationModesIntermState
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationModesIntermState(void) 
{
  u16 reversing_set_ts;
  
  if( (PDS_Upper_input & 0xc0u) || (PDS_Lower_input & 0xc0u) )
  {
    EN_ERROR3 |= 0x04u;   /* COMB SENSOR DETECTED PERSON NEAR THE COMBS */
  }   
  
  reversing_set_ts = ADDITIONAL_REVERSING_TIME + TIME_REVERSING;
  
  if( (!(CMD_FLAG8 &0x04u)) )   /* No intermitent */
  {
    SfBase_EscState = ESC_STARTING_PROCESS_STATE;
  }  
  else if( (CMD_FLAG8 &0x01u) &&  ( Reversing_Ts > reversing_set_ts )) 
  {
    /* 
    **¡¡Two direction run enable mode
    **  reversing start enable 
    **
    */
    if( PDS_Upper_input )       /*  Down demand    */ 
    {   
      CMD_FLAG6 &= ~0x02u;
      CMD_FLAG6 |= 0x01u;
        
      CMD_FLAG8 |= 0x80u;       /* To starting process state*/    
    } 
    else if( PDS_Lower_input )  /*  Up demand    */ 
    {
      CMD_FLAG6 &= ~0x01u;
      CMD_FLAG6 |= 0x02u;
      
      CMD_FLAG8 |= 0x80u;       /* To starting process state*/    
    }  
    else
    {
       /* NONE */
    }  
  }
  else /* */
  {
    if(CMD_FLAG8 &0x01u)
    {
      if(Reversing_Tms++>100u) 
      {
        Reversing_Tms = 0u;  
        if(Reversing_Ts < 0xffffu) 
        {
          Reversing_Ts++; 
        }  
      }    
    }
    
    if(((CMD_FLAG6 & 0x01u) && (PDS_Lower_input)) || ((CMD_FLAG6 & 0x02u) && (PDS_Upper_input)))
    {
      CMD_FLAG8 |= 0x80u; /* Ë³Ïò½øÈë, goto state: STARTING PROCESS  */
    }  
    else if( ((CMD_FLAG6 & 0x01u) && (PDS_Upper_input)) || ((CMD_FLAG6 & 0x02u) && (PDS_Lower_input)) )  /* Wrong direction enter */
    {  
      CMD_FLAG8 |= 0x40u; /* Activate_Direction_Indication */
      CMD_FLAG8 |= 0x80u; /* goto state: STARTING PROCESS  */
    } 
    else
    {
      /*  NONE  */
    }  
  }   
}

/*******************************************************************************
* Function Name  : OperationModesStoppingProcess
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationModesStoppingProcess(void)
{
  /* ORDER_STOP */
  EscDataToControl[0][1] |= ORDER_STOP;       
  
  EscDataToControl[0][3] &= ~0x10u;  
  
  Transportation_Ts = 0u;
  Lowspeed_Ts = 0u;
  Direction_Indication_Ts = 0u;   
  
  /* Flag: Active_Direction_Indication */
  if( (!(CMD_ESC_RUN_MODE & ESC_NORMAL)) && (!(CMD_FLAG5 & ESC_FAULT)) && (!(CMD_OMC_FLAG5 & ESC_FAULT)) )   /* In normal mode and no fault */  
  {
    /* Inspection up down key check*/
    Inspection_UpDown_Button();
    
    if(( CMD_ESC_RUN & ESC_UP ) || ( CMD_ESC_RUN & ESC_DOWN ))
    {
      CMD_FLAG10 |= 0x02u;                              /*  ESC_STARTING_PROCESS_STATE;*/
    }  
  }  
}

/*******************************************************************************
* Function Name  : OperationalStop
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationalStop(void)
{
  CMD_FLAG5 &= ~0x20u; /* Operation stop flag */
  
  /* Stop order from control board */
  if( EscRtData.DataFromControl[0][1] & 0x10u )
  {    
    CMD_FLAG5 |= 0x20u;
    CMD_FLAG6 &= ~0x10u;  /* Interm stop deactived */   
  }
}

/*******************************************************************************
* Function Name  : OperationModesStartingProcess
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationModesStartingProcess(void)
{
  static u16 opms_tcnt=0u;
  
  if ( CMD_ESC_RUN_MODE & ESC_NORMAL )      /* In normal mode */  
  {  
    if( OP_Old_State != SfBase_EscState )
    {
      opms_tcnt = 0u;
    }
    
    if(opms_tcnt++<100u) /* no contact feedback */
    {
      
    }  
    else if(opms_tcnt++<200u) /* no contact feedback */
    {
      /* ACTIVATE_UP_CONTACTOR & STATE_STARTING_PROCESS state */
      EscDataToControl[0][1] |= ORDER_ACTIVATE_UP_CONTACTOR;    
    }
    else if(opms_tcnt++<1000u)
    {
      if((!(AUX_BRAKE_ENABLE)) || (CMD_FLAG7 & 0x08u) )
      {  
         CMD_FLAG10 |= 0x04u; /* Flag set, Enable starting -> run */
      }   
    }  
    else 
    {
      
    }  
  }
  else
  {  
    if(opms_tcnt++<1000u)
    {  
      if( (!(AUX_BRAKE_ENABLE)) || (CMD_FLAG7 & 0x08u) || (CMD_ESC_RUN & ESC_UP) )
      {   
        CMD_FLAG10 |= 0x04u; /* Flag set, Enable starting -> run */
      } 
    }
    else
    {
      
    }  
  }  
}

/*******************************************************************************
* Function Name  : OperationInspectionRun
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void OperationInspectionRun(void)
{
  /* Inspection up down key check*/
  Inspection_UpDown_Button();

  if(( EscRtData.DataFromControl[0][2] & ESC_FROM_CB_STATE_MASK ) == ESC_FROM_CB_SF_STATE_RUNNING )
  {
    /* removes the order (=0) when checks that the control board is in RUN state */
    EscDataToControl[0][1] &= ~ORDER_RUN;
  }
  else
  {
    /* activates the order (=1) when changes to RUN state */
    /* ORDER_RUN */ 
    EscDataToControl[0][1] |= ORDER_RUN;            
  }

  if(CMD_FLAG6 & 0x01u)
  {  
    /* DIRECTION_UP */
    EscDataToControl[0][1] |= ORDER_UP_DOWN;
  }
  else
  {  
    /* DIRECTION_DOWN */
    EscDataToControl[0][1] &= ~ORDER_UP_DOWN;
  }  
  
  /*
  if( (!( CMD_ESC_RUN & ESC_UP )) && (!( CMD_ESC_RUN & ESC_DOWN )) )
  {
                         
  } 
  */
}

/*******************************************************************************
* Function Name  : OperationModesProcess
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void OperationModesProcess(void)
{
  
  /* Flag setting */
  
  /* ORDER_STOP */
  EscDataToControl[0][1] &= ~(ORDER_STOP | ORDER_RUN);          
  /* Deactive UP contactor output order */
  EscDataToControl[0][1] &= ~ORDER_ACTIVATE_UP_CONTACTOR;  
  
  EscDataToControl[0][3] |= 0x10u;  
    
  /* Flag reset:
  ready -> starting, 
  starting -> run 
  */
  CMD_FLAG10 &= ~0x06u; 

  OperationModeSelection();
      
  switch(SfBase_EscState)
  {  
    case ESC_READY_STATE: 
      
      OperationModesReadyState();
      break;
    
    case ESC_STARTING_PROCESS_STATE:
      
      OperationModesStartingProcess();          
      break;
      
    case ESC_RUN_STATE: 
      
      if ( CMD_ESC_RUN_MODE & ESC_NORMAL )      /* In normal mode */    
      {
        OperationModesRunState();               /*  Normal run  */
      }
      else
      {   
        OperationInspectionRun();               /* Inspection run */
      }
      
      break;      
      
    case ESC_INTERM_STATE: 
      
      OperationModesIntermState();
      break;
      
    case ESC_STOPPING_PROCESS_STATE:   
      
      OperationModesStoppingProcess();   
      break;      
      
    default :
      
      break;
  }   
  
  OperationalStop();
  
  OP_Old_State = SfBase_EscState;
}

/******************************  END OF FILE  *********************************/