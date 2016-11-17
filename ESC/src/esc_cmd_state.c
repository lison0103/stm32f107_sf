/*******************************************************************************
* File Name          : esc_cmd_state.c
* Author             : lison
* Version            : V1.0
* Date               : 06/16/2016
* Last modify date   : 09/28/2016
* Description        : This file contains esc command and state.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_cmd_state.h"  
#include "esc_motor_speed.h"     
#include "initial_devices.h"     
#include "esc_error_process.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void en_key_check(void);
static void check_key_input(void);
static void Safety_String_End(void);
static void Safety_String_Begin(void);
static void Drive_Chain1(void);
void Inspection_Normal_Key_Ready_b(void);
void Inspection_Normal_Key_Run_b(void);

UpDownKeyItem UpKey = 
{ 
    0u,
    ESC_UP_KEY,
    ESC_DOWN_KEY,
    0u
};

UpDownKeyItem DownKey = 
{ 
    0u,
    ESC_DOWN_KEY,
    ESC_UP_KEY,
    0u
};

/*******************************************************************************
* Function Name  : CheckUpDown_Key
* Description    : Check up down key.
* Input          : ptKEY: up or down key           
* Output         : None
* Return         : None
*******************************************************************************/
void CheckUpDown_Key(UpDownKeyItem *ptKEY)
{       
    /* Check allow up key or down key or both */
    if(( UP_DOWN_ALLOWED == 0u ) || ( UP_DOWN_ALLOWED == ptKEY->key ))
    {
        if( ptKEY->InputPreviousState == 0u )
        {
            if( CMD_ESC_KEY & ptKEY->key )
            {
                if( (CMD_ESC_KEY & ptKEY->otherkey) == 0u )
                {
                    ptKEY->TimerKeyOn = 0u;
                    ptKEY->InputPreviousState = 1u;
                }
                else
                {
                    /* Key up-down fault */
                    ptKEY->InputPreviousState = 0u;
                    EN_ERROR10 |= 0x01u;
                }
            }
            else
            {
                ptKEY->InputPreviousState = 0u;
            }
        }
        else
        {
            ptKEY->TimerKeyOn++;
            if( CMD_ESC_KEY & ptKEY->key )
            {
                if( (CMD_ESC_KEY & ptKEY->otherkey) == 0u )
                {
                    if( (ptKEY->TimerKeyOn * SYSTEMTICK) > 5000u )
                    {
                        /* Key up-down fault */
                        ptKEY->InputPreviousState = 0u;
                        EN_ERROR10 |= 0x01u;
                    }
                }
                else
                {
                    /* Key up-down fault */
                    ptKEY->InputPreviousState = 0u;
                    EN_ERROR10 |= 0x01u;
                }
            }
            else
            {
                ptKEY->InputPreviousState = 0u;
                if( (ptKEY->TimerKeyOn * SYSTEMTICK) > KEY_MINIMUM_TIME )
                {
                    ptKEY->TimerKeyOn = 0u;
                    
                    /* In fault state, reset standard fault */
                    if( SfBase_EscState == ESC_FAULT_STATE )
                    {
                        g_u8ResetButton = 1u;
                        /* for test */
                        fault_code_manual_reset();
                    }
                    else
                    {
                        /* In ready state, Order to Run Up or Down*/
                        if( ptKEY->key == ESC_UP_KEY )
                        {
                            CMD_ESC_RUN |= ESC_UP;
                        }
                        else if( ptKEY->key == ESC_DOWN_KEY )
                        {

                          
                          
                          CMD_ESC_RUN |= ESC_DOWN;
                        }
                        else
                        {}
                    
                    }
                }
            }
        }
    }
    
    /* Disable */
    if( !(CMD_FLAG5 & ESC_SAFETY_END_ENABLE) )
    {
      CMD_ESC_RUN &= ~(ESC_UP|ESC_DOWN);
    }  
}

/*******************************************************************************
* Function Name  : Inspection_UpDown_Button
* Description    : Check Inspection up down.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Inspection_UpDown_Button(void)
{
    if( CMD_ESC_KEY & ESC_INSPECT_UP_BUTTON )
    {
        /* Order to inspect Run Up */
        CMD_ESC_RUN |= ESC_UP;
    }
    else if( CMD_ESC_KEY & ESC_INSPECT_DOWN_BUTTON )
    {
        /* Order to inspect Run Down */
        CMD_ESC_RUN |= ESC_DOWN;
    }
    else
    {
        /* In RUN state, when the actual direction input (up or down) is not 
        activated, the sytem exectues the stopping process */
        /* order to stop */
        CMD_ESC_RUN &= ~ESC_UP;
        CMD_ESC_RUN &= ~ESC_DOWN;
    }
    
    /* Disable */
    if( !(CMD_FLAG5 & ESC_SAFETY_END_ENABLE) )
    {
      CMD_ESC_RUN &= ~(ESC_UP|ESC_DOWN);
    }  
}


/*******************************************************************************
* Function Name  : Inspection_Normal_Key_Ready
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Inspection_Normal_Key_Ready_b(void)
{
    if( SfBase_EscState != ESC_RUN_STATE )
    {
        if( CMD_ESC_KEY & ESC_INSPECT_NORMAL_KEY )
        {
            /* normal mode */
            CMD_ESC_RUN_MODE &= ~ESC_INSPECT;
        }
        else
        {
            /* inspection mode */
            CMD_ESC_RUN_MODE |= ESC_INSPECT;
        }
    }
}

/*******************************************************************************
* Function Name  : Inspection_Normal_Key_Run
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Inspection_Normal_Key_Run_b(void)
{
    /* Check the conditions to go to inspection like the inspection run orders are deactived */
    if( SfBase_EscState == ESC_RUN_STATE )
    {
        if( CMD_ESC_KEY & ESC_INSPECT_NORMAL_KEY )
        {
            /* normal mode */
            CMD_ESC_RUN_MODE &= ~ESC_INSPECT;
        }
        else
        {
            /* inspection mode */
            CMD_ESC_RUN_MODE |= ESC_INSPECT;
        }
    }
}
    
/*******************************************************************************
* Function Name  : CheckReset
* Description    : Check reset.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void CheckReset(void)
{
    static u16 stat_u16TimerResetPress = 0u;
    u8 i;
   
    /* reset Button */
    if( CMD_ESC_KEY & ESC_RESET_BUTTON )
    {
        stat_u16TimerResetPress++;
    }
    else
    {       
        if(((stat_u16TimerResetPress * SYSTEMTICK) > RESET_MINIMUM_TIME ) && ((stat_u16TimerResetPress * SYSTEMTICK) < 5000u ))
        {            
            /* Reset a fault */
            if( SfBase_EscState == ESC_FAULT_STATE )
            {
                g_u8ResetButton = 1u;                
            }
            
            /* for test */
            for( i = 0u; i < 64u; i++ )
            {
                EscRtData.ErrorBuff[i] = 0u;
            }
            for( i = 0u; i < 5u; i++ )
            {
                EscRtData.ErrorCode[i] = 0u;
                EscErrorCodeBuff[i] = 0u;
            }
        }
        stat_u16TimerResetPress = 0u;
    }  
}

/*******************************************************************************
* Function Name  : Safety_String_Begin
* Description    : Go to fault if begin safety string is open.      
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Safety_String_Begin(void)
{
    /* Begin safety string is open */
    if( INPUT_PORT9_16 & INPUT_PORT10_MASK ) /* LOW active*/
    {
        EN_ERROR8 &= ~0x20u;
    }
    else
    {
        EN_ERROR8 |= 0x20u;      
    }  
}

/*******************************************************************************
* Function Name  : Safety_String_End
* Description    : Go to fault if end safety string is open.      
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Safety_String_End(void)
{
  static u8 safety_end_off_tcnt=0u,safety_end_on_flag=0u;
    
  if( safety_end_off_tcnt < 255u ) 
  {
      safety_end_off_tcnt++;
  }
  
  /* End safety string is open */
  if( INPUT_FEEDBACK & INPUT_PORT_SF_PWR_FB_CPU_MASK )
  {        
    CMD_FLAG5 &= ~ESC_SAFETY_END_CLOSE;
        
    if(safety_end_on_flag)
    {
        safety_end_off_tcnt = 0u;
    }
    safety_end_on_flag = 0u;
    
    CMD_FLAG5 &= ~ESC_SAFETY_END_ENABLE;
  }
  else
  {
    CMD_FLAG5 |= ESC_SAFETY_END_CLOSE;
       
    safety_end_on_flag = 1u;
 
    if(safety_end_off_tcnt > 100u)
    {  
      CMD_FLAG5 |= ESC_SAFETY_END_ENABLE;
    } 
  }  
         
    /* The system supervises also the voltage of the end safety string. */
    /* This voltage is sent to Control Board */
    /* Only in High configure safety board CPU1*/
#ifdef GEC_SF_MASTER    
     Get_Adc_Average();
#endif    
}


/*******************************************************************************
* Function Name  : Drive_Chain1
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Drive_Chain1(void)
{
    static u16 stat_u16TimerDriverChain = 0u;
    
    /* The safety board goes to fault if a drive chain signal is deactivated during a fixed time */
    if( INPUT_PORT9_16 & INPUT_PORT9_MASK )
    {
        stat_u16TimerDriverChain++;
        if( (stat_u16TimerDriverChain * SYSTEMTICK) > DRIVE_CHAIN_DELAY )
        {
            /* fault: DRIVE CHAIN 1 or 2 */
            
        }
    }    
    else
    {
        stat_u16TimerDriverChain = 0u;
    }
}


/*******************************************************************************
* Function Name  : check_key_input
* Description    : Check key input.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
static void check_key_input(void) 
{
    /* up key */  
#if 1
    if((EscRtData. Cfg_Input_Level[2] &(0x01u))  ) 
    {
        CMD_ESC_KEY |= ESC_UP_KEY;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_UP_KEY;
    }
    
    /* down key */
    if((EscRtData. Cfg_Input_Level[2] &(0x02u))  ) 
    {
        CMD_ESC_KEY |= ESC_DOWN_KEY;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_DOWN_KEY;
    }

#else  
    if((INPUT_PORT17_24 & INPUT_PORT21_MASK)) 
    {
        CMD_ESC_KEY |= ESC_UP_KEY;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_UP_KEY;
    }
    
    /* down key */
    if((INPUT_PORT17_24 & INPUT_PORT22_MASK)) 
    {
        CMD_ESC_KEY |= ESC_DOWN_KEY;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_DOWN_KEY;
    }
#endif
    
    
    
    
    
    
    /* inspect up key */
    if((INPUT_PORT17_24 & INPUT_PORT19_MASK)) 
    {
        CMD_ESC_KEY |= ESC_INSPECT_UP_BUTTON; 
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_INSPECT_UP_BUTTON; 
    }
    
    /* inspect down key */
    if((INPUT_PORT17_24 & INPUT_PORT20_MASK)) 
    {
        CMD_ESC_KEY |= ESC_INSPECT_DOWN_BUTTON;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_INSPECT_DOWN_BUTTON;
    }   
    /* inspect normal key */
/*    
    if((INPUT_PORT9_16 & INPUT_PORT11_MASK)) 
    {
        CMD_ESC_KEY |= ESC_INSPECT_NORMAL_KEY;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_INSPECT_NORMAL_KEY;
    }  
*/
    /* reset button */
    if((INPUT_PORT9_16 & INPUT_PORT10_MASK)) 
    {
        CMD_ESC_KEY |= ESC_RESET_BUTTON;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_RESET_BUTTON;
    }     
}


/*******************************************************************************
* Function Name  : Esc_Safety_Input_Check
* Description    : 
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void Esc_Safety_Input_Check(void)
{
    Safety_String_Begin();
    Safety_String_End();
    Drive_Chain1();
    check_key_input();
}




/******************************  END OF FILE  *********************************/