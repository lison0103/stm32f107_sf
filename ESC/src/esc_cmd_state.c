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
    static u16 key_on_tms = 0,key_up_tms = 0,key_down_tms = 0; 
    static u16 auto_cmd_differ_tms=0;
    
    if(!(CMD_FLAG1 & 0x02) && ( sf_wdt_check_en == 0 ))
    {   
        if( ( CMD_FLAG6 & 0x03 ))
        {
            if( key_on_tms < 5000 ) key_on_tms++;
            
            if(CMD_FLAG6 & 0x01) 
            {
                if( key_up_tms < 5000 ) key_up_tms++;
            }		
            
            if(CMD_FLAG6 & 0x02) 
            {
                if( key_down_tms < 5000 ) key_down_tms++;
            }
        }

        if((!(CMD_FLAG6 & 0x03)) && (key_on_tms * SYSTEMTICK >= 1500) && (key_on_tms * SYSTEMTICK < 6000)) 
        {
            if(key_on_tms == key_up_tms)  
            {
                CMD_FLAG1 |= (1<<2)|(1<<1);
            }
            else if(key_on_tms == key_down_tms)  
            {
                CMD_FLAG1 |= (1<<3)|(1<<1); 
            }
            
        }	
    }
	
    
    if( sf_wdt_check_en == 1 )
    {
        key_on_tms = 0;	
        key_up_tms=0;
        key_down_tms=0;
    }
    
    if(!(CMD_FLAG6 & 0x03)) key_on_tms = 0;	
    if(!(CMD_FLAG6 & 0x01)) key_up_tms=0;
    if(!(CMD_FLAG6 & 0x02)) key_down_tms=0;

    /* dual cpu check */
    if( CMD_FLAG1 & 0x0E )
    {
        if((CMD_FLAG1&0x0f) != (CMD_OMC_FLAG1&0x0f))
        {
            if(auto_cmd_differ_tms > 20)
            {
                CMD_FLAG1 = 0;      
            } 
            else
            {
                auto_cmd_differ_tms++;      
            }  
        }
        else
        {
            auto_cmd_differ_tms = 0;
        }  
    }  
    else
    {
        auto_cmd_differ_tms = 0;
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
    static u32 auto_running_tms=0,key_on_tms=0,key_off_tms=0,key_stop_tms=0;

    
    /* has key signal */
    if(CMD_FLAG6 & 0x03) 
    {
        if( key_on_tms * SYSTEMTICK < 10000 ) 
        {
            key_on_tms++;
        }
        else
        {
            /* key stick */
            EN_ERROR6 |= 0x40; 
        }  
        
        key_off_tms = 0;
    }
    else 
    {  
        if( key_off_tms * SYSTEMTICK < 2000 ) 
        {
            key_off_tms++;
        }
        else
        {
            EN_ERROR6 &= ~0x40;  
        }
        
        key_on_tms = 0;
    }
    
    /* escalator running, key stop escalator */
    if(CMD_FLAG1 & 0x0c) 
    {
        /* 1 minute */
        if(auto_running_tms * SYSTEMTICK < 60000) 
        {
            auto_running_tms++;
        }
    }
    else
    {
        auto_running_tms = 0;
    }        
    
    /* after running 10s, 3s key signal */
    if((key_on_tms * SYSTEMTICK == 3000) && (auto_running_tms * SYSTEMTICK > 10000)) 
    {
        EN_ERROR6 |= 0x20; 
        CMD_FLAG1 &= ~0x0e;
    }  
    
    if(EN_ERROR6 & 0x20)
    {
        if( (key_stop_tms++)  * SYSTEMTICK > 2000 ) 
        {
            EN_ERROR6 &= ~0x20;
        }
    }  
    else
    {
        key_stop_tms = 0; 
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
        CMD_FLAG6 |= 0x01;
    }
    else
    {
        CMD_FLAG6 &= ~0x01;
    }
    
    /* run down */
    if((INPUT_PORT17_24 & INPUT_PORT22_MASK)) 
    {
        CMD_FLAG6 |= 0x02;
    }
    else
    {
        CMD_FLAG6 &= ~0x02;
    }
    
    /* inspect run up */
    if((INPUT_PORT17_24 & INPUT_PORT19_MASK)) 
    {
        CMD_FLAG6 |= 0x04; 
    }
    else
    {
        CMD_FLAG6 &= ~0x04; 
    }
    
    /* inspect run down */
    if((INPUT_PORT17_24 & INPUT_PORT20_MASK)) 
    {
        CMD_FLAG6 |= 0x08;
    }
    else
    {
        CMD_FLAG6 &= ~0x08;
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
    static u16 sf_running_tms,sf_stopping_tms = 0;
    static u16 sf_reset_tms = 0;
    static u8 key_on = 0;
    u8 i;
    
    
    if( (SfBase_EscState & ESC_STATE_READY) )
    {
        key_on = 1;
    }
    
    CheckUpDown_Key();
    
    /* esc running */
    if( (CMD_FLAG1 & 0x0c) && (CMD_OMC_FLAG1 & 0x0c) && ( key_on == 1 ) )
    {
        SfBase_EscState &= ~ESC_STATE_STOP;
        SfBase_EscState &= ~ESC_STATE_READY;
        
        SfBase_EscState |= ESC_STATE_RUNNING;
        
        SfBase_EscState |= ESC_STATE_NORMAL;
        
        if(( (sf_running_tms * SYSTEMTICK) > 2500 ) && (*(MTRITEM[0].ptFreqBuff) > MIN_SPEED ) && (*(MTRITEM[1].ptFreqBuff) > MIN_SPEED ))
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
        
        sf_stopping_tms = 0;
        
        sf_reset_tms = 0;
    }
    else
    {
        SfBase_EscState &= ~ESC_STATE_RUNNING;
        SfBase_EscState &= ~ESC_STATE_SPEEDUP;
        SfBase_EscState &= ~ESC_STATE_RUN5S;
        
        SfBase_EscState &= ~ESC_STATE_NORMAL;
        
        SfBase_EscState |= ESC_STATE_STOP;
        
        key_on = 0;
        
        if(( (sf_stopping_tms * SYSTEMTICK) > 3000 ) && (MTRITEM[0].rt_brake_stop == 1) && (MTRITEM[1].rt_brake_stop == 1) )
        {
            SfBase_EscState |= ESC_STATE_READY;
        }
        else
        {        
            sf_stopping_tms++;
        }
        
        /* for test reset the value-------------------------------------*/
        if( (sf_reset_tms * SYSTEMTICK) > 20000 )
        {
            for( i = 30; i < 200; i++ )
            {
                EscRTBuff[i] = 0;
            }
            sf_reset_tms = 0;
        }
        else
        {
            sf_reset_tms++;
        }
        
        sf_running_tms = 0;
        
        
    }
     
}



/******************************  END OF FILE  *********************************/