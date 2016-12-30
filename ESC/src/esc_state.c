/*******************************************************************************
* File Name          : esc_state.c
* Author             : lison/Tu
* Version            : V1.0
* Date               : 08/16/2016
* Last modify date   : 12/17/2016
* Description        : Esc state machine.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_state.h"
#include "esc_motor_speed.h"
#include "esc_handrail_speed.h"
#include "esc_missing_step.h"
#include "esc_error_process.h"
#include "esc_parameter_process.h"
#include "esc_cmd_state.h"
#include "esc_safety_check.h"
#include "esc_record_data.h"

#include "esc_control.h" 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 g_u32InitTestError = 0u;
u16 g_u16RunTestError = 0u;
u16 g_u16ParameterLoadingError = 0u;
u8 g_u8ParameterLoadingFinish = 0u;
u8 g_u8ResetType = 0u;
/* u8 g_u8SafetyRelayStartCheck = 0u; */
u8 g_u8ParameterLoading = 0u;

/*******************************************************************************
* Function Name  : Esc_State_Machine
* Description    :                
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void Esc_State_Machine(void)
{ 
    Fault_Check();
    Esc_State_Check();
    
    switch (SfBase_EscState) 
    {
        
      case ESC_INIT_STATE:
        {  
            
            /* init state*/
            EscDataToControl[0][2] &= ~ESC_STATE_MASK;
            EscDataToControl[0][2] |= ESC_STATE_INIT;
            
#ifdef GEC_SF_S_NEW   
            /* CPU2 wait for CPU1 status */           
            if( !OmcEscRtData.ParaStatus )
            {
                
                break;
            }    
            /*g_u8ParameterLoadingFinish = 1u;*/
#endif
            
            
            if( g_u8ParameterLoading == 1u )
            {
                /* Power On with USB Stick */
                /* wait for restart */
                break;
            }        
            

            
            if( g_u8ParameterLoadingFinish )
            {
                if( g_u32InitTestError || g_u16ParameterLoadingError )
                {
                    /* Init fault do what??? specfication don't describe detail ???? */
                    
                    /* informs to the CPU2 the reason */
                    
                    /* informs to the control board the reason */
                    
                    /* goes to init fault state */       
                    /*ESC_Init_Fault();*/
                    
                    EN_ERROR49 |= 0x08u;
                }           
                else                
                {
                    
                    /* everything is OK, the system goes to the previous state (before the power off)*/
#ifdef GEC_SF_MASTER
                    SfBase_EscState = Check_Error_Present_Memory();
#else
                    SfBase_EscState = OmcSfBase_EscState;
#endif
                }
            }

            break;
        }
      case ESC_FAULT_STATE:
        {
          /* Safety relay output disable when fault */
            SafetyOutputDisable();         
            
            /* fault state*/
            EscDataToControl[0][2] &= ~ESC_STATE_MASK;
            EscDataToControl[0][2] |= ESC_STATE_FAULT;            
 
            /* check reset */
            CheckReset();
            CheckUpDown_Key(&UpKey);
            CheckUpDown_Key(&DownKey);
            Inspection_UpDown_Button_Reset();
            fault_code_auto_reset();
            
                
            
           
            
            if (!((CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) || (EscRtData.DataFromControl[0][3] & ESC_FROM_CB_TYPE_OF_CONTROL_FAULT))) 
            {  
                SfBase_EscState = ESC_READY_STATE;
            }             
            break;    
        }
      case ESC_READY_STATE:
        {
            /* Safety relay output disable when ready */
            SafetyOutputDisable();  
                                  
            if ((CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) || (EscRtData.DataFromControl[0][3] & ORDER_FROM_CB_FAULT))
            {  
                SfBase_EscState = ESC_FAULT_STATE;
            }  
            else
            {  
              /* ready state*/
              EscDataToControl[0][2] &= ~ESC_STATE_MASK;
              EscDataToControl[0][2] |= ESC_STATE_READY;             
            
              if((CMD_FLAG10 & 0x02u))
              {
                SfBase_EscState = ESC_STARTING_PROCESS_STATE;                           /*  ESC_STARTING_PROCESS_STATE;*/
              }  
            }
            
            break;      
        }

      case ESC_STARTING_PROCESS_STATE:
       {          
           
            if ( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) || (EscRtData.DataFromControl[0][3] & ORDER_FROM_CB_FAULT)) 
            {  
              SfBase_EscState = ESC_FAULT_STATE; /* Fault */
            }
            else if( CMD_FLAG10 & 0x04u  )
            {
              SfBase_EscState = ESC_RUN_STATE;  /* Go to run state */
            }  
            else
            {  
              /* starting process state*/
              EscDataToControl[0][2] &= ~ESC_STATE_MASK;
              EscDataToControl[0][2] |= ESC_STATE_STARTING_PROCESS;              
            }
            
/*            
         if (parameter safety curtain) 
         {
             CheckSafetyCurtain();
         }
           
         if ( AUX_BRAKE_ENABLE ) 
         {
             chek aux brake has opened
         }
*/       
           
           /* In Inspection mode */
/*           if(!( CMD_ESC_RUN_MODE & ESC_NORMAL )) 
           {
               
           }
           
           else if ( The starting is due to people detected in Intermitted operation  or Two direction operation )
           {
           
           }
          
           else
           {
                Safety_Relay_Shortcircuit_Check();      
           }

           if( g_u8SafetyRelayStartCheck == 1u )
           {
              if( CMD_FLAG7 & 0x08u ) 
              {    
                SfBase_EscState = ESC_RUN_STATE;
              }    
           }
                SfBase_EscState = ESC_RUN_STATE;
*/               
           
            
           break;
       }
      case ESC_RUN_STATE:
        {
           
            /* Fault ,or Operation stop flag actived */
            if( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) || (CMD_FLAG5 & 0x20u) || ((CMD_ESC_RUN & (ESC_UP | ESC_DOWN)) == 0u)
               || (EscRtData.DataFromControl[0][3] & ORDER_FROM_CB_FAULT) )
            {
                SfBase_EscState = ESC_STOPPING_PROCESS_STATE;
                
                /* Safety relay output disable when stopping */
                SafetyOutputDisable();  
            }
            else
            {  
              /* run state*/
              EscDataToControl[0][2] &= ~ESC_STATE_MASK;
              EscDataToControl[0][2] |= ESC_STATE_RUN;             

              /* Safety relay output enable when run control */
              SafetyOutputEnable();  
            }
            
            /* if( programmed_stop || fault_order_from_control || stop_order_from_control )*/ 
            
            /* When UP/DOWN maintenance buttons are released: if there is no fault during the stopping process system goes to READY state.*/
            /*
            if( !( CMD_ESC_RUN_MODE & ESC_NORMAL ) && ((CMD_ESC_INSP & (ESC_UP | ESC_DOWN)) == 0u) ) 
            {
                SfBase_EscState = ESC_STOPPING_PROCESS_STATE;
            }
            */
/*            
            if (finish count of interm mode) SfBase_EscState = INTERM_STATE;
            if  (change from every mode to contious mode) set all timers to 0
            if  (change from interm to standby mode) if lightbarier sensor is false fo the duration of stand by time
            ...
*/                  
            break;
        } 
        case ESC_STOPPING_PROCESS_STATE: 
        {
          /* Safety relay output disable when stopping */
          SafetyOutputDisable();  
          
          /* stopping process state */
          EscDataToControl[0][2] &= ~ESC_STATE_MASK;
          EscDataToControl[0][2] |= ESC_STATE_STOPPING_PROCESS;           
                    
          EscDataToControl[0][3] &= ~ESC_STOPPING_PROCESS_FINISH;
          
          /* Escalator speed pulse freqency < 1Hz */
          if( g_u8EscStoppingFinish == 1u )
          {                  
              EscDataToControl[0][3] |= ESC_STOPPING_PROCESS_FINISH;
              
              if( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) ) 
              {  
                  SfBase_EscState = ESC_FAULT_STATE;
              }
              else if( CMD_FLAG6 & 0x10u )  /* Interm stop */
              {
                  SfBase_EscState = ESC_INTERM_STATE;
              }  
              else /* Operation stop */
              {
                  SfBase_EscState = ESC_READY_STATE;
              }
          }

          break;
        }
        case ESC_INTERM_STATE:
        {
          /* Safety relay output disable when interm */
          SafetyOutputDisable();  
                      
          /* stopping process state*/
          EscDataToControl[0][2] &= ~ESC_STATE_MASK;
            
          EscDataToControl[0][2] |= ESC_STATE_INTERM;
            
          if ( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) || (EscRtData.DataFromControl[0][3] & ORDER_FROM_CB_FAULT)) 
          {  
            SfBase_EscState = ESC_FAULT_STATE;
          }
          else
          {    
            if(( CMD_FLAG5 & 0x20u) )
            {
              SfBase_EscState = ESC_READY_STATE;
            }  
            else if( CMD_FLAG8 & 0x80u )
            {
              SfBase_EscState = ESC_STARTING_PROCESS_STATE;
            }
            else
            {
              
            }  
              
              /*            
              
              if ( not fault) check people detection 
              if  (change to contious mode) SfBase_EscState = STARTING_PROCESS_STATE
              if  (change to standby mode) SfBase_EscState = STARTING_PROCESS_STATE
                  
              */       
          }
            
          break;
        }   
        
        default:
        
          /* set fault */
          /* Safety relay output disable */
          SafetyOutputDisable();  
        
        
        break;   
    }  
}


/*******************************************************************************
* Function Name  : Esc_State_Check
* Description    :                
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void Esc_State_Check(void)
{ 
    static u16 stat_u16TimeFaultInternalState = 0u;
    static u16 stat_u16TimeFaultExternalState = 0u;
    u8 u8SafetyState = 0u;
    
    /* Both cpu state check */
    if( SfBase_EscState != OmcSfBase_EscState )
    {
        if( stat_u16TimeFaultInternalState < 0xffffu )
        {
            stat_u16TimeFaultInternalState++;
        }
        
        /* if during 500ms they don't have the same state then the system goes to fault: INCOHERENCE STATE INTERNAL. */
        if( stat_u16TimeFaultInternalState > 100u )
        {
            /* INCOHERENCE STATE INTERNAL F390 */
            EN_ERROR49 |= 0x40u; 
        }
    }
    else
    {
        stat_u16TimeFaultInternalState = 0u;
    }
    
    
    /* Safety board and control board state check */
    /* The states INTERM, READY and STARTING PROCESS of the safety board are equivalent to READY state in the Control Board. */
    if(( SfBase_EscState == ESC_STARTING_PROCESS_STATE ) || ( SfBase_EscState == ESC_INTERM_STATE )) 
    {
        u8SafetyState = ESC_READY_STATE;
    }
    else
    {
        u8SafetyState = SfBase_EscState;
    }
    if( u8SafetyState != ( EscRtData.DataFromControl[0][2] & ESC_FROM_CB_STATE_MASK ))
    {
        if( stat_u16TimeFaultExternalState < 0xffffu )
        {
            stat_u16TimeFaultExternalState++;
        }
        
        /* if during 500ms they don't have the same state then the system goes to fault: INCOHERENCE STATE EXTERNAL. */
        if( stat_u16TimeFaultExternalState > 100u )
        {
            /* Incoherence state external F389 */
            EN_ERROR49 |= 0x40u; 
        }
    }
    else
    {
        stat_u16TimeFaultExternalState = 0u;
    }
}




/******************************  END OF FILE  *********************************/


