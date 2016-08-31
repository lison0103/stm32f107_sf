/*******************************************************************************
* File Name          : esc_state.c
* Author             : lison
* Version            : V1.0
* Date               : 08/16/2016
* Description        : Esc state machine.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_state.h"
#include "esc_motor_speed.h"
#include "esc_handrail_speed.h"
#include "esc_missing_step.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Esc_State_Machine
* Description    :                
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void Esc_State_Machine(void)
{ 
    
    switch (EscState) {
        
      case INIT_STATE:
        {
            if( self_test_error || parameter_loading_error )
            {
                Init_Fault();
            }
            else
            {
                Last_PowerOff_State = Check_Error_Present_Memory();
                EscState = LastPowerOffState;
            }
            break;
        }
      case FAULT_STATE:
        {
            
            StoreFaultInMemory();

            /* falut increase */
            
            
            //check the type of fault
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
                EscState = READY_STATE;
            }
            else
            {
                
            }
                
            if( no_fault )
            {
                EscState = READY_STATE;
            }
            break;    
        }
      case READY_STATE:
        {
            if (fault) 
            {  
                EscState = FAULT
                break;
            }         
              
            if (local && not_fault) 
            {
                check_up_down_key();
                if( up_down_order_check_ok )
                {
                    EscState = STARTING_PROCESS_STATE;
                }
            }
            else if(inspec && not_fault) 
            {
                check_inpspec_up_down_button();
                if( inspec_up_down_order_check_ok )
                {
                    EscState = STARTING_PROCESS_STATE;
                }                
            }
   /*        
            if (remote && not_fault)
            {
                check_remote_order();
                if( remote_up_down_order_check_ok )
                {
                    EscState = STARTING_PROCESS_STATE;
                }                 
            }
   */
              break;      
        }

      case STARTING_PROCESS_STATE:
       {
        if (fault) EscState = FAULT
         
            
            
         if (parameter safety curtain) check safety curtain
         if (parameter aux brake) chek aux brake has opened
             control board drivers: go to run: closed safety relay();     

            
             
           If conditions ok > RUN_STATE 
            
            break;
       }
      case RUN_STATE:
        {
            if ( fault || programmed_stop || fault_order_from_control || 
                up_down_maintenance_button_released || stop_order_from_control ) 
            {
                EscState = STOPPING_PROCESS_STATE;
            }
            
            if (finish count of interm mode) EscState = INTERM_STATE;
            if  (change from every mode to contious mode) set all timers to 0
            if  (change from interm to standby mode) if lightbarier sensor is false fo the duration of stand by time
            ...
                  
            break;
        } 
      case STOPPING_PROCESS_STATE: 
        {
            
            stopping_process(); 
            
            
            break;
        }
        case INTERM_STATE:
        {
            if ( not fault) check people detection 
            if  (change to contious mode) EscState = STARTING_PROCESS_STATE
            if  (change to standby mode) EscState = STARTING_PROCESS_STATE
                
                if (fault) EscState = FAULT
                    break;
        }      
      default:
        
        /* set fault */
        
        
        break;   
    }

}





/******************************  END OF FILE  *********************************/

