/*******************************************************************************
* File Name          : esc_state.c
* Author             : lison
* Version            : V1.0
* Date               : 08/16/2016
* Last modify date   : 09/23/2016
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
u8 g_u8ResetButton = 0u;
u8 g_u8SafetyRelayStartCheck = 0u;

/*******************************************************************************
* Function Name  : Esc_State_Machine
* Description    :                
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void Esc_State_Machine(void)
{ 
    
    switch (SfBase_EscState) 
    {
        
      case ESC_INIT_STATE:
        {  
#if 0            
            if( g_u32InitTestError || g_u16ParameterLoadingError )
            {
                /* Init fault do what??? specfication don't describe detail ???? */
                
                /* informs to the CPU2 the reason */
                
                /* informs to the control board the reason */
                
                /* goes to init fault state */       
                /*ESC_Init_Fault();*/
            }
            else 
#endif                
            {
                /* for test */
                SfBase_EscState = ESC_READY_STATE;
                
                /* everything is OK, the system goes to the previous state (before the power off)*/
#ifdef GEC_SF_MASTER
                /*SfBase_EscState = Check_Error_Present_Memory();*/
#else
                SfBase_EscState = OmcSfBase_EscState;
#endif
            }

            break;
        }
      case ESC_FAULT_STATE:
        {
          /* Safety relay output disable when fault */
            SafetyOutputDisable();  
                        
            /* check reset */
            CheckReset();
            CheckUpDown_Key(&UpKey);
            CheckUpDown_Key(&DownKey);
            fault_code_auto_reset();
            
                
            
#ifdef GEC_SF_MASTER   
            error_change_check();
            /*StoreFaultInMemory();*/
#endif            
            
            if ( !((CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT)) ) 
            {  
                SfBase_EscState = ESC_READY_STATE;
            }             
            break;    
        }
      case ESC_READY_STATE:
        {
          /* Safety relay output disable when ready */
          SafetyOutputDisable();  
           
            if ( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) ) 
            {  
                SfBase_EscState = ESC_FAULT_STATE;
                break;
            }         
              
            /* In local mode */
            if ( ( CMD_ESC_RUN_MODE & ESC_INSPECT ) == ESC_NORMAL ) 
            {
                CheckUpDown_Key(&UpKey);
                CheckUpDown_Key(&DownKey);
            }
            /* In Inspection mode */
            else if( CMD_ESC_RUN_MODE & ESC_INSPECT ) 
            {
                Inspection_UpDown_Button();          
            }
            else
            {
/*                
                if( remote )
                {
                    check_remote_order();
                    if( remote_up_down_order_check_ok )
                    {
                        SfBase_EscState = STARTING_PROCESS_STATE;
                    }                 
                }
*/                
            }
           
            if(( CMD_ESC_RUN & ESC_UP ) || ( CMD_ESC_RUN & ESC_DOWN ))
            {
              SfBase_EscState = ESC_STARTING_PROCESS_STATE;  /*  ESC_STARTING_PROCESS_STATE;*/
            }

            break;      
        }

      case ESC_STARTING_PROCESS_STATE:
       {          
           
            if ( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) ) 
            {  
                SfBase_EscState = ESC_FAULT_STATE;
                break;
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
/*           if( CMD_ESC_RUN_MODE & ESC_INSPECT ) 
           {
               
           }
           
           else if ( The starting is due to people detected in Intermitted operation  or Two direction operation )
           {
           
           }
          
           else
           {
                Safety_Relay_Shortcircuit_Check();      
           }
*/               

           if( g_u8SafetyRelayStartCheck == 1u )
           {
               SfBase_EscState = ESC_RUN_STATE;
           }
            
           break;
       }
      case ESC_RUN_STATE:
        {
          /* Safety relay output enable when run control */
          SafetyOutputEnable();  
          
            if( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) )
            {
                SfBase_EscState = ESC_STOPPING_PROCESS_STATE;
                break;
            }
            
            if(  ((CMD_ESC_RUN & (ESC_UP | ESC_DOWN)) == 0u) ) 
            {
                SfBase_EscState = ESC_STOPPING_PROCESS_STATE;
            }            
            /* if( programmed_stop || fault_order_from_control || stop_order_from_control )*/ 
            
            /* When UP/DOWN maintenance buttons are released: if there is no fault during the stopping process system goes to READY state.*/
            /*
            if( (CMD_ESC_RUN_MODE & ESC_INSPECT) && ((CMD_ESC_INSP & (ESC_UP | ESC_DOWN)) == 0u) ) 
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

          EscDataToControl[0][2] &= ~ESC_STOPPING_PROCESS_FINISH;
          
          if( (MTRITEM[0].rt_brake_stop == 1u) && (MTRITEM[1].rt_brake_stop == 1u) )
            {                  
                EscDataToControl[0][2] |= ESC_STOPPING_PROCESS_FINISH;
                
                if( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) ) 
                {  
                    SfBase_EscState = ESC_FAULT_STATE;
                }
                else
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
            
            if ( (CMD_FLAG5 & ESC_FAULT) || (CMD_OMC_FLAG5 & ESC_FAULT) ) 
            {  
                SfBase_EscState = ESC_FAULT_STATE;
                break;
            } 
/*            
            if ( not fault) check people detection 
            if  (change to contious mode) SfBase_EscState = STARTING_PROCESS_STATE
            if  (change to standby mode) SfBase_EscState = STARTING_PROCESS_STATE
                
*/            
                    break;
        }      
      default:
        
        /* set fault */
          /* Safety relay output disable */
          SafetyOutputDisable();  
        
        
        break;   
    }  
}





/******************************  END OF FILE  *********************************/


