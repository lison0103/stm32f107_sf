/*******************************************************************************
* File Name          : esc_safety_check.c
* Author             : lison
* Version            : V1.0
* Date               : 06/20/2016
* Description        : This file contains esc safety module check.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_safety_check.h"
#include "ewdt.h"
#include "esc_error_process.h"
#include "bsp_iocfg.h"
#include "safety_test.h"
#include "delay.h"
#include "initial_devices.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u8 sf_wdt_check_tms = 0;
u8 sf_wdt_check_en = 0;
u8 sf_relay_check_cnt = 0;
//static u8 test_num = 0;
#ifdef GEC_SF_MASTER
u8 R_SF_RL2_FB_CPU1;
#else
u8 R_SF_RL_FB_CPU2;

#endif

u8 SAFETY_SWITCH[4][4];
u8 SAFETY_SWITCH_STATUS[4];

#define UNDEFINED       0
#define OPEN       1
#define CLOSED       2
#define ERROR       3 
#define SYNCLINE        4
#define FEEDBACK_PULSE_OUTPUT   5
/*******************************************************************************
* Function Name  : SafetySwitchStatus
* Description    : Safety Switch.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetySwitchStatus(void)
{
    static u8 Pulse_Generation = 0;
    static u8 SwitchState = 0;
    static u8 Timeout_SYNC_Line = 0;
    static u8 SafetySwitchInit = 0;
    static u8 FAULT = 0;
    
    if( SafetySwitchInit == 0 )
    {
#ifdef GEC_SF_MASTER
        SYNC_SYS_OUT_SET();
#else
        SYNC_SYS_OUT_CLR();
#endif    
        
        PLUSE_OUTPUT_CLR();
        Pulse_Generation = 0;
        for( u8 switch_num = 0; switch_num < 4; switch_num++ )
        {
            SAFETY_SWITCH_STATUS[switch_num] = UNDEFINED;
            pcEscSafetySwitchStatus[switch_num] = UNDEFINED;
        }
        
        
        SafetySwitchInit = 1;
    }   

#if 1   
    switch( SwitchState )
    {
       case 0:
        {
            if( SYNC_SYS_IN == 0 || Pulse_Generation == 1 )
            {
                Timeout_SYNC_Line = 0;
                
                if( Pulse_Generation != 1 )
                {
                    Pulse_Generation = 1;
                    SwitchState = 1;
                }
                
                SYNC_SYS_OUT_SET();
                if( SYNC_SYS_IN != 0 )
                {
                    FAULT = SYNCLINE;
                    EN_ERROR8 |= 0x08;
                    SwitchState = 6;
                }
            }
            else
            {
                if( Timeout_SYNC_Line > 10 )
                {
                    FAULT = SYNCLINE;
                    EN_ERROR8 |= 0x08;
                    SwitchState = 6;            
                }
                else
                {
                    Timeout_SYNC_Line++;
                }
            } 
            break;
        }
       case 1:
        {
            PLUSE_OUTPUT_SET();            
            FAULT = 0;/* for test */
            SwitchState = 2;
            break;
        }
       case 2:
        {
            if( PLUSE_OUTPUT_FB == 0 )
            {
                if( INPUT_PORT24_29 & INPUT_PORT29_MASK )
                {
                    SAFETY_SWITCH[0][0] = CLOSED;
                }
                else
                {
                    SAFETY_SWITCH[0][0] = OPEN;
                }
            }
            else
            {
                FAULT = FEEDBACK_PULSE_OUTPUT;
                EN_ERROR8 |= 0x10;
            }
            
            PLUSE_OUTPUT_CLR();
            SwitchState = 3;
            break;
        }
       case 3:
        {
            if( PLUSE_OUTPUT_FB == 1 )
            {
                if( INPUT_PORT24_29 & INPUT_PORT29_MASK )
                {
                    SAFETY_SWITCH[0][1] = ERROR;
                }
                else
                {
                    SAFETY_SWITCH[0][1] = UNDEFINED;
                }
            }
            else
            {
                FAULT = FEEDBACK_PULSE_OUTPUT;
                EN_ERROR8 |= 0x10;
            }
            
            PLUSE_OUTPUT_CLR();
            SwitchState = 4;
            break;
        }   
       case 4:
        {
            if( PLUSE_OUTPUT_FB == 1 )
            {
                if( INPUT_PORT24_29 & INPUT_PORT29_MASK )
                {
                    SAFETY_SWITCH[0][2] = ERROR;
                }
                else
                {
                    SAFETY_SWITCH[0][2] = UNDEFINED;
                }
            }
            else
            {
                FAULT = FEEDBACK_PULSE_OUTPUT;
                EN_ERROR8 |= 0x10;
            }
            
            PLUSE_OUTPUT_SET();
            SwitchState = 5;
            break;
        }          
       case 5:
        {
            if( PLUSE_OUTPUT_FB == 0 )
            {
                if( INPUT_PORT24_29 & INPUT_PORT29_MASK )
                {
                    SAFETY_SWITCH[0][3] = CLOSED;
                }
                else
                {
                    SAFETY_SWITCH[0][3] = OPEN;
                }
            }
            else
            {
                FAULT = FEEDBACK_PULSE_OUTPUT;
                EN_ERROR8 |= 0x10;
            }
            
            for( u8 switch_num = 0; switch_num < 1; switch_num++ )
            {
                if( FAULT == FEEDBACK_PULSE_OUTPUT )
                {
                    SAFETY_SWITCH_STATUS[switch_num] = UNDEFINED;
                    pcEscSafetySwitchStatus[switch_num] = UNDEFINED;
                }
                else if ( SAFETY_SWITCH[switch_num][0] == ERROR || SAFETY_SWITCH[switch_num][1] == ERROR || SAFETY_SWITCH[switch_num][2] == ERROR || SAFETY_SWITCH[switch_num][3] == ERROR )
                {
                    SAFETY_SWITCH_STATUS[switch_num] = ERROR;
                    pcEscSafetySwitchStatus[switch_num] = ERROR;
                }
                else if ( SAFETY_SWITCH[switch_num][0] == OPEN || SAFETY_SWITCH[switch_num][1] == OPEN || SAFETY_SWITCH[switch_num][2] == OPEN || SAFETY_SWITCH[switch_num][3] == OPEN )
                {
                    SAFETY_SWITCH_STATUS[switch_num]  = OPEN;
                    pcEscSafetySwitchStatus[switch_num] = OPEN;
                }
                else if ( SAFETY_SWITCH[switch_num][0] == CLOSED || SAFETY_SWITCH[switch_num][1] == CLOSED || SAFETY_SWITCH[switch_num][2] == CLOSED || SAFETY_SWITCH[switch_num][3] == CLOSED )
                {
                    SAFETY_SWITCH_STATUS[switch_num]  = CLOSED;
                    pcEscSafetySwitchStatus[switch_num] = CLOSED;
                }
                else
                {
                    SAFETY_SWITCH_STATUS[switch_num] = UNDEFINED;
                    pcEscSafetySwitchStatus[switch_num] = UNDEFINED;
                }
            }           
            
            Pulse_Generation = 0;
            PLUSE_OUTPUT_CLR();
            SYNC_SYS_OUT_CLR();
            SwitchState = 7;
            break;
        }
       case 6:
        {
            SAFETY_SWITCH_STATUS[0] = UNDEFINED;
            pcEscSafetySwitchStatus[0] = UNDEFINED;
            
            Pulse_Generation = 0;
            PLUSE_OUTPUT_CLR();
            SYNC_SYS_OUT_CLR();
            SwitchState = 7;
            break;            
        }
       case 7:
        {
            SwitchState = 0;
            break;            
        }       
       default:
        break;
        
    }
#endif 
    
}

/*******************************************************************************
* Function Name  : SafetyOutputDisable
* Description    : Safety Relay disable.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyOutputDisable(void)
{
    static u8 sf_output_dis_tms = 0; 
    
    if(( sf_wdt_check_en == 0 ) && ( SfBase_EscState & ESC_STATE_STOP )) 
    { 
        SF_RELAY_OFF();
        
        if( sf_relay_check_cnt == 1 )
        {
            sf_output_dis_tms++;
            if( sf_output_dis_tms * SYSTEMTICK > 50 )
            {
                SafetyRelayAuxRelayTest();
                sf_relay_check_cnt = 0;
                sf_output_dis_tms = 0;
            }
        }  
        
    }

}

/*******************************************************************************
* Function Name  : SafetyOutputEnable
* Description    : Safety Relay enable.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyOutputEnable(void)
{

    static u8 sf_output_en_tms = 0; 
    
    
    if( SfBase_EscState & ESC_STATE_RUNNING ) 
    { 
        SF_RELAY_ON();    
        SF_EWDT_TOOGLE();
        
        if( sf_relay_check_cnt == 0 )
        {
            sf_output_en_tms++;
            if( sf_output_en_tms * SYSTEMTICK > 50 )
            {
                SafetyRelayAuxRelayTest();
                sf_relay_check_cnt = 1;
                sf_output_en_tms = 0;
            }
        }   
        
        SafetyCTR_Check();
    }

}


/*******************************************************************************
* Function Name  : SafetyRelayAuxRelayTest
* Description    : Safety Relay and Auxiliary brake Relay Test
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyRelayAuxRelayTest(void)
{
   
    if( SfBase_EscState & ESC_STATE_RUNNING ) 
    { 
        /* safety circuit is  connected */
        if( SF_RL_DRV_FB || SF_PWR_FB_CPU || SF_RL_FB || !AUX_FB )
        {
//            FailSafeTest();
            EN_ERROR8 |= 0x01;
        }   
    }
    else if( SfBase_EscState & ESC_STATE_STOP )  
    {
        /* safety circuit is disconnected */
        if( !SF_RL_DRV_FB || SF_PWR_FB_CPU || !SF_RL_FB )
        {
//            FailSafeTest();
            EN_ERROR8 |= 0x01;
        }  
    }
}

/*******************************************************************************
* Function Name  : SafetyExtWdt_StartUpCheck
* Description    : When powered on,check the safety external watchdog.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyExtWdt_StartUpCheck(void)
{
    
#ifndef GEC_SF_MASTER    
//    delay_ms(700);
#endif

    if( SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB && AUX_FB )
    {
        SF_EWDT_TOOGLE();
        SF_RELAY_ON();   
        SF_EWDT_TOOGLE();
    }   
    
    
    /*  wait 1800ms */
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    
    /** Safety Relay and AuxRelay Test **/
    if( SF_RL_DRV_FB || SF_PWR_FB_CPU || SF_RL_FB || !AUX_FB )
    {
//        FailSafeTest();
        EN_ERROR8 |= 0x01;
    }   
    
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
   
    if( !SF_RL_FB )
    {
//        FailSafeTest();
        EN_ERROR8 |= 0x02;
    }
    else
    {
        SF_RELAY_OFF(); 
    }   
        


}

/*******************************************************************************
* Function Name  : SafetyExtWdt_RunCheck
* Description    : When the escalator stopped,check the safety external watchdog.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyExtWdt_RunCheck(void)
{

    static u16 escState_old = ESC_STATE_READY; 
    
    if(( SfBase_EscState & ESC_STATE_READY ) && ( !(escState_old & ESC_STATE_READY)))
    {
        sf_wdt_check_en = 1;
    }

    
    if( sf_wdt_check_en == 1 )
    {
        if(( sf_wdt_check_tms == 0 ) && ( SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB ))
        {
            SF_EWDT_TOOGLE();
            SF_RELAY_ON();  
            SF_EWDT_TOOGLE();
        }   
        
        sf_wdt_check_tms++;
        /*  (41 - 1)*45ms = 1800ms */
        if( sf_wdt_check_tms >= 41)
        {
            
            sf_wdt_check_tms = 0;
            sf_wdt_check_en = 0;
            
            if( !SF_RL_FB )
            {
//                FailSafeTest();
                EN_ERROR8 |= 0x02;
            }
            else
            {
                SF_RELAY_OFF();
            }   
            
        }
    }
    else
    {
        SF_EWDT_TOOGLE();
    }
    
    escState_old = SfBase_EscState;
}




/*******************************************************************************
* Function Name  : SafetyCTR_Check
* Description    : Coding protection detection in running process
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyCTR_Check(void)
{
    static u8 sf_ctr_check_tms = 0;
    
    if( SfBase_EscState & ESC_STATE_RUNNING ) 
    {    
        sf_ctr_check_tms++;
        /* check period 1s */ 
        if( sf_ctr_check_tms * SYSTEMTICK > 1000 )
        {
            SF_RELAY_OFF();
            delay_us(150);
            if(!SF_RL_DRV_FB)
            {
                EN_ERROR_SYS2++;
                if(EN_ERROR_SYS2 > 2)
                {
                    /* SafetyCTR_Check error */
//                    ESC_SafeRelay_Error_Process();
                    EN_ERROR8 |= 0x04;
                }
            }
            else
            {
                EN_ERROR_SYS2 = 0;
            }
            SF_RELAY_ON();
        }
    }

}







/******************************  END OF FILE  *********************************/