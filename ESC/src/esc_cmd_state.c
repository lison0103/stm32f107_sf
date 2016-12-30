/*******************************************************************************
* File Name          : esc_cmd_state.c
* Author             : lison
* Version            : V1.0
* Date               : 06/16/2016
* Last modify date   : 11/21/2016
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
/* Drive chain 1, function id 36 */
#define DRIVE_CHAIN1_ENABLE   (EscRtData. Cfg_Input_Mask[4] & (0x08u))
#define DRIVE_CHAIN1_INPUT    (EscRtData.Cfg_Input_Level[4] & (0x08u))
/* Drive chain 1, function id 37 */
#define DRIVE_CHAIN2_ENABLE   (EscRtData. Cfg_Input_Mask[4] & (0x10u))
#define DRIVE_CHAIN2_INPUT    (EscRtData.Cfg_Input_Level[4] & (0x10u))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void en_key_check(void);
static void check_key_input(void);
static void Drive_Chain(void);


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
                    /* FAULT: UP DOWN KEYS ACTIVATE (F330) */
                    ptKEY->InputPreviousState = 0u;
                    EN_ERROR42 |= 0x04u;
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
                        ptKEY->InputPreviousState = 0u;
                        if( ptKEY->key == ESC_UP_KEY )
                        {
                            /* FAULT: UP KEY ACTIVATE (F331) */
                            EN_ERROR42 |= 0x08u;
                        }
                        else if( ptKEY->key == ESC_DOWN_KEY )
                        {
                            /* FAULT: DOWN KEY ACTIVATE (F332) */
                            EN_ERROR42 |= 0x10u;
                        }
                        else
                        {}
                    }
                }
                else
                {
                    /* FAULT: UP DOWN KEYS ACTIVATE (F330) */
                    ptKEY->InputPreviousState = 0u;
                    EN_ERROR42 |= 0x04u;
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
                        g_u8ResetType = 1u;

                        fault_code_manual_reset(g_u8ResetType);
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
    if( (CMD_ESC_KEY & ESC_INSPECT_UP_BUTTON) && (!(CMD_ESC_KEY & ESC_INSPECT_DOWN_BUTTON)) )
    {
        /* Order to inspect Run Up */
        CMD_ESC_RUN |= ESC_UP;
    }
    else if( (!(CMD_ESC_KEY & ESC_INSPECT_UP_BUTTON)) && (CMD_ESC_KEY & ESC_INSPECT_DOWN_BUTTON) )
    {
        /* Order to inspect Run Down */
        CMD_ESC_RUN |= ESC_DOWN;
    }
    else
    {
        /* In RUN state, when the actual direction input (up or down) is not activated, the sytem exectues the stopping process */
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
* Function Name  : Inspection_UpDown_Button_Reset
* Description    : Check Inspection up down.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Inspection_UpDown_Button_Reset(void)
{
    static u16 stat_u16TimeUpButton = 0u,stat_u16TimeDownButton = 0u;
    
    if( RESET_FROM_INSPECTION_CONTROL )
    {
        if( (CMD_ESC_KEY & ESC_INSPECT_UP_BUTTON) && (!(CMD_ESC_KEY & ESC_INSPECT_DOWN_BUTTON)) )
        {
            stat_u16TimeUpButton++;
            stat_u16TimeDownButton = 0u;
            
            
        }
        else if( (!(CMD_ESC_KEY & ESC_INSPECT_UP_BUTTON)) && (CMD_ESC_KEY & ESC_INSPECT_DOWN_BUTTON) )
        {
            stat_u16TimeDownButton++;
            stat_u16TimeUpButton = 0u;
        }
        else
        {
            if(((( stat_u16TimeUpButton * SYSTEMTICK ) < 5000u ) && (( stat_u16TimeUpButton * SYSTEMTICK ) > KEY_MINIMUM_TIME ))
               || ((( stat_u16TimeDownButton * SYSTEMTICK ) < 5000u ) && (( stat_u16TimeDownButton * SYSTEMTICK ) > KEY_MINIMUM_TIME )))
            {
                g_u8ResetType = 1u;
                
                fault_code_manual_reset(g_u8ResetType);
            }
            stat_u16TimeUpButton = 0u;
            stat_u16TimeDownButton = 0u;
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
   
    /* reset Button */
    if( CMD_ESC_KEY & ESC_RESET_BUTTON )
    {
        stat_u16TimerResetPress++;
    }
    else
    {       
        if(((stat_u16TimerResetPress * SYSTEMTICK) > KEY_MINIMUM_TIME ) && ((stat_u16TimerResetPress * SYSTEMTICK) < 5000u ))
        {            
            /* Reset a fault */
            if( SfBase_EscState == ESC_FAULT_STATE )
            {
                g_u8ResetType = 2u;                
                
                fault_code_manual_reset(g_u8ResetType);                
            }                    
        }
        stat_u16TimerResetPress = 0u;
    }  
}

/*******************************************************************************
* Function Name  : Drive_Chain
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void Drive_Chain(void)
{
    static u16 stat_u16TimerDriverChain1 = 0u;
    static u16 stat_u16TimerDriverChain2 = 0u;
    
    /* configure Drive chain 1 function enable */
    if( DRIVE_CHAIN1_ENABLE )
    {
        /* The safety board goes to fault if a drive chain signal is deactivated during a fixed time */
        if( !DRIVE_CHAIN1_INPUT )
        {
            stat_u16TimerDriverChain1++;
            if(( stat_u16TimerDriverChain1 * SYSTEMTICK ) > DRIVE_CHAIN_DELAY )
            {
                /* Drive chain (Du-, Triplex) F09 */
                EN_ERROR2 |= 0x02u; 
            }
        }    
        else
        {
            stat_u16TimerDriverChain1 = 0u;
        }
    }
    
    /* configure Drive chain 2 function enable */
    if( DRIVE_CHAIN2_ENABLE )
    {
        /* The safety board goes to fault if a drive chain signal is deactivated during a fixed time */
        if( !DRIVE_CHAIN2_INPUT )
        {
            stat_u16TimerDriverChain2++;
            if(( stat_u16TimerDriverChain2 * SYSTEMTICK ) > DRIVE_CHAIN_DELAY )
            {
                /* Drive chain-2 (Du-, Triplex) F137 */
                EN_ERROR18 |= 0x02u; 
            }
        }    
        else
        {
            stat_u16TimerDriverChain2 = 0u;
        }    
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
    if( (EscRtData. Cfg_Input_Level[3] &(0x02u)) ) 
    {
        CMD_ESC_KEY |= ESC_UP_KEY;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_UP_KEY;
    }
    
    /* down key */
    if( (EscRtData. Cfg_Input_Level[3] &(0x04u)) ) 
    {
        CMD_ESC_KEY |= ESC_DOWN_KEY;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_DOWN_KEY;
    }
      
    /* inspect up key */
    if( (EscRtData. Cfg_Input_Level[0] &(0x20u)) ) 
    {
        CMD_ESC_KEY |= ESC_INSPECT_UP_BUTTON; 
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_INSPECT_UP_BUTTON; 
    }
    
    /* inspect down key */
    if( (EscRtData. Cfg_Input_Level[0] &(0x40u))  ) 
    {
        CMD_ESC_KEY |= ESC_INSPECT_DOWN_BUTTON;
    }
    else
    {
        CMD_ESC_KEY &= ~ESC_INSPECT_DOWN_BUTTON;
    }   

    /* reset button */
    if((INPUT_PORT9_16 & INPUT_PORT9_MASK)) 
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
    Drive_Chain();
    check_key_input();
}




/******************************  END OF FILE  *********************************/