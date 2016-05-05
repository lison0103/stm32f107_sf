/*******************************************************************************
* File Name          : esc_state_machine.c
* Author             : lison
* Version            : V1.0
* Date               : 04/19/2016
* Description        : The file contains esc state machine.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_state_machine.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

ESCEvent m_escevent = power_on;

/*******************************************************************************
* Function Name  : normal_run
* Description    : None
*                  
* Input          : None
*                  None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
u8 normal_run( void )
{
    return 0;
}


/*******************************************************************************
* Function Name  : esc_init
* Description    : None
*                  
* Input          : None
*                  None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void esc_init( void )
{
    u8 res = 1;
    /* esc init code */
    /*
        .................
    */
    if( 1 == res )
    {
        m_escevent = no_failure_store;
    }
    else if( 2 == res )
    {
        m_escevent = failure_store;
    }
    else
    {
        m_escevent = init_fault;
    }
}

/*******************************************************************************
* Function Name  : esc_ready_run
* Description    : None
*                  
* Input          : None
*                  None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void esc_ready_run( void )
{
    u8 cmd = 1;
    /* esc ready run code */
    /*
        .................
    */
    if( 1 == cmd )
    {
        m_escevent = up_down_key;
    }
    else if( 2 == cmd )
    {
        m_escevent = inspec_buttons;
    }
    else if( 3 == cmd )
    {
        m_escevent = remote_run;
    }
    else if( 4 == cmd )
    {
        m_escevent = people_detected;
    }  
    else if( 5 == cmd )
    {
        m_escevent = ready_fault;
    }      
    else
    {
        /* no cmd */

    }
}

/*******************************************************************************
* Function Name  : esc_run_process
* Description    : None
*                  
* Input          : None
*                  None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void esc_run_process( ESCEvent e )
{
    u8 status = 1;
    /* esc ready run code */
    if( e == up_down_key )
    {
        status = normal_run();
    }
    else if( e == inspec_buttons )
    {
    
    }
    else if( e == remote_run )
    {
    
    }
    else if( e == people_detected )
    {
    
    }   
    
    
    if( 1 == status )
    {
        m_escevent = run_fault;
    }
    else if( 2 == status )
    {
        m_escevent = run_stop;
    }     
    else
    {
        /* no cmd */

    }
}

/*******************************************************************************
* Function Name  : esc_stopping_process
* Description    : None
*                  
* Input          : None
*                  None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void esc_stopping_process( void )
{
    u8 result = 1;
    /* esc ready run code */
    /*
        .................
    */
    if( 1 == result )
    {
        m_escevent = aut_mode_stop;
    }
    else if( 2 == result )
    {
        m_escevent = inspec_buttons_released;
    }     
    else if( 3 == result )
    {
        m_escevent = stop_from_up_down_key;

    }
    else if( 4 == result )
    {
        m_escevent = stop_fault;

    }    
}

/*******************************************************************************
* Function Name  : esc_failure_process
* Description    : None
*                  
* Input          : None
*                  None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void esc_failure_process( void )
{
    u8 cmd = 1;
    /* esc ready run code */
    /*
        .................
    */
    if( 1 == cmd )
    {
        m_escevent = reset_button;
    }
    else if( 2 == cmd )
    {
        m_escevent = safety_curtain_ok;
    }     
    else if( 3 == cmd )
    {
        m_escevent = up_down_key;

    }
}

/*******************************************************************************
* Function Name  : ESCState_Change
* Description    : None
*                  
* Input          : None
*                  None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void ESCState_Change( ESCEvent e )
{
    static ESCState s = s_init;
    
    switch ( s )
    {
       case s_init:
        {
            if( e == power_on )
            {
                s = s_init;
                /* esc init */
                esc_init();
            }            
            else if( e == init_fault )
            {
                s = s_init;
                /* reset the system */
                while(1);
            }
            else if( e == no_failure_store )
            {
                s = s_ready;

            }
            else if( e == failure_store )
            {
                s = s_fault;
               
                
            }
            break;
        }
       case s_ready:
        {
            if( e == up_down_key )
            {
                s = s_run;
                /* check safety curtain if needed */
                
            }
            else if( e == inspec_buttons )
            {
                s = s_run;
                
            }
            else if( e == remote_run )
            {
                s = s_run;
                /* check safety curtain if needed */
                
            }  
            else if( e == people_detected )
            {
                s = s_run;
            }             
            else if( e == ready_fault )
            {
                s = s_fault;
            }     
            else if( e == no_failure_store )
            {
                s = s_ready;
                /* ready status, wait for cmd */
                esc_ready_run();
           
            }
            break;
        }
       case s_run:
        {
            if( e == up_down_key || e == inspec_buttons ||  e == remote_run || e == people_detected )
            {
                s = s_run;
                /* run process */
                esc_run_process(e);
            }
            if( e == aut_mode_stop )
            {
                s = s_ready;
            }
            else if( e == run_fault )
            {
                s = s_fault;
                esc_stopping_process();
                
            }
            else if( e == run_stop )
            {
                s = s_run;
                esc_stopping_process();
            }  
            else if( e == inspec_buttons_released || stop_from_up_down_key)
            {
                s = s_ready;

            }
            else if( e == stop_fault )
            {
                s = s_fault;

            }              
            break;
        }
       case s_fault:
        {
            if( e == failure_store || e == stop_fault )
            {
                s = s_fault;
                /* failure process */
                esc_failure_process();
            }
            else if( e == reset_button || e == safety_curtain_ok || e == up_down_key )
            {
                s = s_ready;
                
            }           
            break;
        }
       default:
        break;
    }
}





/******************************  END OF FILE  *********************************/



