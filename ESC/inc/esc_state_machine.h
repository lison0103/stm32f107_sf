/*******************************************************************************
* File Name          : esc_state_machine.h
* Author             : lison
* Version            : V1.0
* Date               : 04/19/2016
* Description        : The file contains prototype of esc state machine
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_STATE_MACHINE_H
#define __ESC_STATE_MACHINE_H 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
typedef enum  
{
    s_init, 
    s_ready, 
    s_run, 
    s_fault 
}
ESCState;

typedef enum 
{ 
    power_on,
    power_off,
    init_fault,
    no_failure_store,
    failure_store,
    up_down_key, 
    inspec_buttons, 
    remote_run, 
    people_detected,
    aut_mode_stop,
    run_fault,
    run_stop, 
    inspec_buttons_released, 
    stop_from_up_down_key,
    ready_fault,
    reset_button, 
    safety_curtain_ok,
    stop_fault
}
ESCEvent;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */



#endif /* __ESC_STATE_MACHINE_H */


/******************************  END OF FILE  *********************************/


