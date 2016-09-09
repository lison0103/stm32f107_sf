/*******************************************************************************
* File Name          : esc_state.c
* Author             : lison
* Version            : V1.0
* Date               : 08/16/2016
* Last modify date   : 09/07/2016
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u16 SfBase_EscState = ESC_INIT_STATE;
u32 g_u32InitTestError = 0u;
u16 g_u16RunTestError = 0u;
u16 g_u16ParameterLoadingError = 0u;

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
            if( g_u32InitTestError || g_u16ParameterLoadingError )
            {
                /* Init fault do what??? specfication don't describe detail ???? */
                
                /* informs to the CPU2 the reason */
                
                /* informs to the control board the reason */
                
                /* goes to init fault state */       
                ESC_Init_Fault();
            }
            else 
            {
                /* everything is OK, the system goes to the previous state (before the power off)*/
#ifdef GEC_SF_MASTER
                SfBase_EscState = Check_Error_Present_Memory();
#else
                SfBase_EscState = pcOMC_SfBase_EscState;
#endif
            }
            
            break;
        }
      case ESC_FAULT_STATE:
        {
#if 0            
            StoreFaultInMemory();

            /* falut increase */
            
            
            /* check the type of fault */
            if( failure_lock )
            {
                if( g_u8ResetButton )
                {
                    /* falut decrease */
                }
            }
            else if( standard_fault )
            {
                if( normal_mode )
                {
                    if( remote_reset || up_down_key_reset || reset_button )
                    {
                        /* falut decrease */

                        
                    }    
                }
                else if( inspec_mode )
                {
                    if( up_down_maintenance button || reset_button )
                    {
                         /* falut decrease */
                    }                      
                }
            }
            else if( recovery_fault )
            {
                /* check stopping distance and no other fault */
                SfBase_EscState = READY_STATE;
            }
            else
            {
                
            }
                
            if( no_fault )
            {
                SfBase_EscState = READY_STATE;
            }
#endif            
            break;    
        }
      case ESC_READY_STATE:
        {
/*            
            if (fault) 
            {  
                SfBase_EscState = FAULT
                break;
            }         
              
            if (local && not_fault) 
            {
                check_up_down_key();
                if( up_down_order_check_ok )
                {
                    SfBase_EscState = STARTING_PROCESS_STATE;
                }
            }
            else if(inspec && not_fault) 
            {
                check_inpspec_up_down_button();
                if( inspec_up_down_order_check_ok )
                {
                    SfBase_EscState = STARTING_PROCESS_STATE;
                }                
            }
*/            
   /*        
            if (remote && not_fault)
            {
                check_remote_order();
                if( remote_up_down_order_check_ok )
                {
                    SfBase_EscState = STARTING_PROCESS_STATE;
                }                 
            }
   */
              break;      
        }

      case ESC_STARTING_PROCESS_STATE:
       {
/*           
        if (fault) SfBase_EscState = FAULT
         
            
            
         if (parameter safety curtain) check safety curtain
         if (parameter aux brake) chek aux brake has opened
             control board drivers: go to run: closed safety relay();     

            
             
           If conditions ok > RUN_STATE 
*/            
            break;
       }
      case ESC_RUN_STATE:
        {
/*            
            if ( fault || programmed_stop || fault_order_from_control || 
                up_down_maintenance_button_released || stop_order_from_control ) 
            {
                SfBase_EscState = STOPPING_PROCESS_STATE;
            }
            
            if (finish count of interm mode) SfBase_EscState = INTERM_STATE;
            if  (change from every mode to contious mode) set all timers to 0
            if  (change from interm to standby mode) if lightbarier sensor is false fo the duration of stand by time
            ...
*/                  
            break;
        } 
      case ESC_STOPPING_PROCESS_STATE: 
        {
/*            
            stopping_process(); 
*/            
            
            break;
        }
        case ESC_INTERM_STATE:
        {
/*            
            if ( not fault) check people detection 
            if  (change to contious mode) SfBase_EscState = STARTING_PROCESS_STATE
            if  (change to standby mode) SfBase_EscState = STARTING_PROCESS_STATE
                
                if (fault) SfBase_EscState = FAULT
*/            
                    break;
        }      
      default:
        
        /* set fault */
        
        
        break;   
    }

}





/******************************  END OF FILE  *********************************/


