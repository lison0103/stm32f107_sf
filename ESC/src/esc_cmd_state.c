/*******************************************************************************
* File Name          : esc_cmd_state.c
* Author             : lison
* Version            : V1.0
* Date               : 06/16/2016
* Description        : This file contains esc command and state.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_cmd_state.h"
#include "esc_motor_speed.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void key_run_detect(void);
void en_key_check(void);
void run_key_check(void);
void CheckUpDown_Key(void);


/*******************************************************************************
* Function Name  : CheckUpDown_Key
* Description    : Check up down key state.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void CheckUpDown_Key(void)
{
    
    run_key_check();
    key_run_detect();
    en_key_check();
   
}

/*******************************************************************************
* Function Name  : key_run_detect
* Description    : Esc key run.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void key_run_detect(void)
{
    static u16 key_on_tms = 0u,key_up_tms = 0u,key_down_tms = 0u; 
    static u16 auto_cmd_differ_tms = 0u;
    
    if( (!(CMD_FLAG1 & 0x02u)) && ( sf_wdt_check_en == 0u ))
    {   
        if( ( CMD_FLAG6 & 0x03u ))
        {
            if( key_on_tms < 5000u ) 
            {
                key_on_tms++;
            }
            
            if(CMD_FLAG6 & 0x01u) 
            {
                if( key_up_tms < 5000u ) 
                {
                    key_up_tms++;
                }
            }		
            
            if(CMD_FLAG6 & 0x02u) 
            {
                if( key_down_tms < 5000u ) 
                {
                    key_down_tms++;
                }
            }
        }

        if((!(CMD_FLAG6 & 0x03u)) && (key_on_tms * SYSTEMTICK >= 1500u) && (key_on_tms * SYSTEMTICK < 6000u)) 
        {
            if(key_on_tms == key_up_tms)  
            {
                CMD_FLAG1 |= (1u<<2u)|(1u<<1u);
            }
            else if(key_on_tms == key_down_tms)  
            {
                CMD_FLAG1 |= (1u<<3u)|(1u<<1u); 
            }
            else
            {}
            
        }	
    }
	
    
    if( sf_wdt_check_en == 1u )
    {
        key_on_tms = 0u;	
        key_up_tms = 0u;
        key_down_tms = 0u;
    }
    
    if(!(CMD_FLAG6 & 0x03u)) 
    {
        key_on_tms = 0u;	
    }
    if(!(CMD_FLAG6 & 0x01u))
    {
        key_up_tms = 0u;
    }
    if(!(CMD_FLAG6 & 0x02u)) 
    {
        key_down_tms = 0u;
    }
    
    /* dual cpu check */
    if( CMD_FLAG1 & 0x0Eu )
    {
        if((CMD_FLAG1&0x0fu) != (CMD_OMC_FLAG1&0x0fu))
        {
            if(auto_cmd_differ_tms > 20u)
            {
                CMD_FLAG1 = 0u;      
            } 
            else
            {
                auto_cmd_differ_tms++;      
            }  
        }
        else
        {
            auto_cmd_differ_tms = 0u;
        }  
    }  
    else
    {
        auto_cmd_differ_tms = 0u;
    }  
      
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
* Function Name  : run_key_check
* Description    : Run key check.
* Input          : None               
* Output         : None
* Return         : None
*******************************************************************************/
void run_key_check(void) 
{
    /* run up */  
    if((INPUT_PORT17_24 & INPUT_PORT21_MASK)) 
    {
        CMD_FLAG6 |= 0x01u;
    }
    else
    {
        CMD_FLAG6 &= ~0x01u;
    }
    
    /* run down */
    if((INPUT_PORT17_24 & INPUT_PORT22_MASK)) 
    {
        CMD_FLAG6 |= 0x02u;
    }
    else
    {
        CMD_FLAG6 &= ~0x02u;
    }
    
    /* inspect run up */
    if((INPUT_PORT17_24 & INPUT_PORT19_MASK)) 
    {
        CMD_FLAG6 |= 0x04u; 
    }
    else
    {
        CMD_FLAG6 &= ~0x04u; 
    }
    
    /* inspect run down */
    if((INPUT_PORT17_24 & INPUT_PORT20_MASK)) 
    {
        CMD_FLAG6 |= 0x08u;
    }
    else
    {
        CMD_FLAG6 &= ~0x08u;
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
    
    
    if( (SfBase_EscState & ESC_STATE_READY) )
    {
        key_on = 1u;
    }
    
    CheckUpDown_Key();
    
    /* esc running */
    if( (CMD_FLAG1 & 0x0cu) && (CMD_OMC_FLAG1 & 0x0cu) && ( key_on == 1u ) )
    {
        SfBase_EscState &= (u16)(~ESC_STATE_STOP);
        SfBase_EscState &= (u16)(~ESC_STATE_READY);
        
        SfBase_EscState |= ESC_STATE_RUN;
        
        SfBase_EscState |= ESC_STATE_NORMAL;
        
        if(( (sf_running_tms * SYSTEMTICK) > 2500u ) && (*(MTRITEM[0].ptFreqBuff) > MIN_SPEED ) && (*(MTRITEM[1].ptFreqBuff) > MIN_SPEED ))
        {
            SfBase_EscState |= ESC_STATE_SPEEDUP;
        }
        
        if( (sf_running_tms * SYSTEMTICK) > UNDERSPEED_TIME )
        {
            SfBase_EscState |= ESC_STATE_RUN5S;
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
        SfBase_EscState &= (u16)(~ESC_STATE_RUN);
        SfBase_EscState &= (u16)(~ESC_STATE_SPEEDUP);
        SfBase_EscState &= (u16)(~ESC_STATE_RUN5S);
        
        SfBase_EscState &= (u16)(~ESC_STATE_NORMAL);
        
        SfBase_EscState |= ESC_STATE_STOP;
        
        key_on = 0u;
        
        if(( (sf_stopping_tms * SYSTEMTICK) > 3000u ) && (MTRITEM[0].rt_brake_stop == 1u) && (MTRITEM[1].rt_brake_stop == 1u) )
        {
            SfBase_EscState |= ESC_STATE_READY;
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