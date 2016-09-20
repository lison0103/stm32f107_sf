/*******************************************************************************
* File Name          : esc_cmd_state.c
* Author             : lison
* Version            : V1.0
* Date               : 06/16/2016
* Date               : 09/19/2016
* Description        : This file contains esc command and state.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_cmd_state.h"
#include "esc_motor_speed.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ESC_UP_KEY                 0x01u
#define ESC_DOWN_KEY               0x02u
#define ESC_INSPECT_UP_BUTTON      0x04u
#define ESC_INSPECT_DOWN_BUTTON    0x08u
#define ESC_RESET_BUTTON           0x10u
#define ESC_INSPECT_NORMAL_KEY     0x20u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void key_run_detect(void);
void en_key_check(void);
void check_key_input(void);
void CheckUpDown_Key(UpDownKeyItem *ptKEY);
void Inspection_UpDown_Button(void);
void CheckReset(void);


UpDownKeyItem UpKey = 
{ 
    0u,
    0x01u,
    0x02u,
    0u
};

UpDownKeyItem DownKey = 
{ 
    0u,
    0x02u,
    0x01u,
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
            if( CMD_FLAG6 & ptKEY->key )
            {
                if( (CMD_FLAG6 & ptKEY->otherkey) == 0u )
                {
                    ptKEY->TimerKeyOn = 0u;
                    ptKEY->InputPreviousState = 1u;
                }
                else
                {
                    /* Key up-down fault */
                    
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
            if( CMD_FLAG6 & ptKEY->key )
            {
                if( (CMD_FLAG6 & ptKEY->otherkey) == 0u )
                {
                    if( (ptKEY->TimerKeyOn * SYSTEMTICK) > 5000u )
                    {
                        /* Key up-down fault */
                        
                    }
                }
                else
                {
                    /* Key up-down fault */
                    
                }
            }
            else
            {
                ptKEY->InputPreviousState = 0u;
                if( (ptKEY->TimerKeyOn * SYSTEMTICK) > KEY_MINIMUM_TIME )
                {
                    ptKEY->TimerKeyOn = 0u;
                    
                    /* In ready state, Order to Run Up or Down*/
                    
                    
                    /* In fault state, reset standard fault */
                    
                }
            }
        }
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
    if( CMD_FLAG6 & ESC_INSPECT_UP_BUTTON )
    {
        if(( CMD_FLAG6 & ESC_INSPECT_UP_BUTTON ) == 0u )
        {
            /* Order to inspect Run Up */
            
        }
        else
        {
            /* fault UP DOWN MAINTENANCE ACTIVATE */
            
        }
    }
    else if( CMD_FLAG6 & ESC_INSPECT_DOWN_BUTTON )
    {
        if(( CMD_FLAG6 & ESC_INSPECT_DOWN_BUTTON ) == 0u )
        {
            /* Order to inspect Run Down */
            
        }
        else
        {
            /* fault UP DOWN MAINTENANCE ACTIVATE */
            
        }
    }
    else
    {
        /* In RUN state, when the actual direction input (up or down) is not 
        activated, the sytem exectues the stopping process */
        
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
    if( CMD_FLAG6 & ESC_RESET_BUTTON )
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
        }
        stat_u16TimerResetPress = 0u;
    }
}


/*******************************************************************************
* Function Name  : Brake_Status_Ready
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Brake_Status_Ready(void)
{
    
    
}

/*******************************************************************************
* Function Name  : Brake_Status_Run
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Brake_Status_Run(void)
{
    
    
}




/*******************************************************************************
* Function Name  : en_key_check
* Description    : Enable key check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void en_key_check(void)
{
    static u32 auto_running_tms = 0u,key_on_tms = 0u,key_off_tms = 0u,key_stop_tms = 0u;

    
    /* has key signal */
    if(CMD_FLAG6 & 0x03u) 
    {
        if( key_on_tms * SYSTEMTICK < 10000u ) 
        {
            key_on_tms++;
        }
        else
        {
            /* key stick */
            EN_ERROR6 |= 0x40u; 
        }  
        
        key_off_tms = 0u;
    }
    else 
    {  
        if( key_off_tms * SYSTEMTICK < 2000u ) 
        {
            key_off_tms++;
        }
        else
        {
            EN_ERROR6 &= ~0x40u;  
        }
        
        key_on_tms = 0u;
    }
    
    /* escalator running, key stop escalator */
    if(CMD_FLAG1 & 0x0cu) 
    {
        /* 1 minute */
        if(auto_running_tms * SYSTEMTICK < 60000u) 
        {
            auto_running_tms++;
        }
    }
    else
    {
        auto_running_tms = 0u;
    }        
    
    /* after running 10s, 3s key signal */
    if((key_on_tms * SYSTEMTICK == 3000u) && (auto_running_tms * SYSTEMTICK > 10000u)) 
    {
        EN_ERROR6 |= 0x20u; 
        CMD_FLAG1 &= ~0x0eu;
    }  
    
    if(EN_ERROR6 & 0x20u)
    {
        if( (key_stop_tms++)  * SYSTEMTICK > 2000u ) 
        {
            EN_ERROR6 &= ~0x20u;
        }
    }  
    else
    {
        key_stop_tms = 0u; 
    }  
    
}



/*******************************************************************************
* Function Name  : check_key_input
* Description    : Check key input.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void check_key_input(void) 
{
    /* up key */  
    if((INPUT_PORT17_24 & INPUT_PORT21_MASK)) 
    {
        CMD_FLAG6 |= ESC_UP_KEY;
    }
    else
    {
        CMD_FLAG6 &= ~ESC_UP_KEY;
    }
    
    /* down key */
    if((INPUT_PORT17_24 & INPUT_PORT22_MASK)) 
    {
        CMD_FLAG6 |= ESC_DOWN_KEY;
    }
    else
    {
        CMD_FLAG6 &= ~ESC_DOWN_KEY;
    }
    
    /* inspect up key */
    if((INPUT_PORT17_24 & INPUT_PORT19_MASK)) 
    {
        CMD_FLAG6 |= ESC_INSPECT_UP_BUTTON; 
    }
    else
    {
        CMD_FLAG6 &= ~ESC_INSPECT_UP_BUTTON; 
    }
    
    /* inspect down key */
    if((INPUT_PORT17_24 & INPUT_PORT20_MASK)) 
    {
        CMD_FLAG6 |= ESC_INSPECT_DOWN_BUTTON;
    }
    else
    {
        CMD_FLAG6 &= ~ESC_INSPECT_DOWN_BUTTON;
    }
    
    /* reset button */
    if((INPUT_PORT9_16 & INPUT_PORT10_MASK)) 
    {
        CMD_FLAG6 |= ESC_RESET_BUTTON;
    }
    else
    {
        CMD_FLAG6 &= ~ESC_RESET_BUTTON;
    }    
    
    /* inspect normal key */
    if((INPUT_PORT9_16 & INPUT_PORT11_MASK)) 
    {
        CMD_FLAG6 |= ESC_INSPECT_NORMAL_KEY;
    }
    else
    {
        CMD_FLAG6 &= ~ESC_INSPECT_NORMAL_KEY;
    }      
}


/*******************************************************************************
* Function Name  : sfEscStateCheck
* Description    : Esc state check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void sfEscStateCheck(void)
{
    static u16 sf_running_tms,sf_stopping_tms = 0u;
    static u16 sf_reset_tms = 0u;
    static u8 key_on = 0u;
    u8 i;
    
    
    if( (SfBase_EscState & ESC_READY_STATE) )
    {
        key_on = 1u;
    }
    
/*    CheckUpDown_Key();*/
    
    /* esc running */
    if( (CMD_FLAG1 & 0x0cu) && (CMD_OMC_FLAG1 & 0x0cu) && ( key_on == 1u ) )
    {
        SfBase_EscState &= (u16)(~ESC_STATE_STOP);
        SfBase_EscState &= (u16)(~ESC_READY_STATE);
        
        SfBase_EscState |= ESC_RUN_STATE;
        
        SfBase_EscState |= ESC_STATE_NORMAL;
        
        if(( (sf_running_tms * SYSTEMTICK) > 2500u ) && (*(MTRITEM[0].ptFreqBuff) > MIN_SPEED ) && (*(MTRITEM[1].ptFreqBuff) > MIN_SPEED ))
        {
            SfBase_EscState |= ESC_STATE_SPEEDUP;
        }
        
        if( (sf_running_tms * SYSTEMTICK) > UNDERSPEED_TIME )
        {
            SfBase_EscState |= ESC_RUN_STATE5S;
        }
        else
        {
            sf_running_tms++;
        }
        
        sf_stopping_tms = 0u;
        
        sf_reset_tms = 0u;
    }
    else
    {
        SfBase_EscState &= (u16)(~ESC_RUN_STATE);
        SfBase_EscState &= (u16)(~ESC_STATE_SPEEDUP);
        SfBase_EscState &= (u16)(~ESC_RUN_STATE5S);
        
        SfBase_EscState &= (u16)(~ESC_STATE_NORMAL);
        
        SfBase_EscState |= ESC_STATE_STOP;
        
        key_on = 0u;
        
        if(( (sf_stopping_tms * SYSTEMTICK) > 3000u ) && (MTRITEM[0].rt_brake_stop == 1u) && (MTRITEM[1].rt_brake_stop == 1u) )
        {
            SfBase_EscState |= ESC_READY_STATE;
        }
        else
        {        
            sf_stopping_tms++;
        }
        
        /* for test reset the value-------------------------------------*/
        if( (sf_reset_tms * SYSTEMTICK) > 20000u )
        {
            for( i = 30u; i < 200u; i++ )
            {
                EscRTBuff[i] = 0u;
            }
            sf_reset_tms = 0u;
        }
        else
        {
            sf_reset_tms++;
        }
        
        sf_running_tms = 0u;
        
        
    }
     
}



/******************************  END OF FILE  *********************************/