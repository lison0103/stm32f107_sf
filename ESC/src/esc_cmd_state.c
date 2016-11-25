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
static void Safety_String_End(void);
static void Safety_String_Begin(void);
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
                    /* FAULT: UP DOWN KEYS ACTIVATE (F377) */
                    ptKEY->InputPreviousState = 0u;
                    EN_ERROR48 |= 0x02u;
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
                        /* FAULT: UP KEY ACTIVATE (F378) */
                        ptKEY->InputPreviousState = 0u;
                        EN_ERROR48 |= 0x04u;
                    }
                }
                else
                {
                    /* Key Up-Down FAULT (F377) */
                    ptKEY->InputPreviousState = 0u;
                    EN_ERROR48 |= 0x02u;
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
        EN_ERROR50 &= ~0x40u;
    }
    else
    {
        EN_ERROR50 |= 0x40u;      
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
                EN_ERROR2 |= 0x04u; 
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
    Safety_String_Begin();
    Safety_String_End();
    Drive_Chain();
    check_key_input();
}




/******************************  END OF FILE  *********************************/