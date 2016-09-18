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
void SafetySwitchCheck( u8 pulse_on, u8 test_num );

static u8 sf_wdt_check_tms = 0u;
u8 sf_wdt_check_en = 0u;
static u8 sf_relay_check_cnt = 0u;
/* static u8 test_num = 0;*/
#ifdef GEC_SF_MASTER
u8 R_SF_RL2_FB_CPU1;
#else
u8 R_SF_RL_FB_CPU2;

#endif

u8 SAFETY_SWITCH[4][4];
u8 SAFETY_SWITCH_STATUS[4];
u8 FAULT = 0u;

#ifdef GEC_SF_MASTER
#define TEST_PATTERN    0x09u
#else
#define TEST_PATTERN    0x06u
#endif

#define UNDEFINED       0u
#define OPEN       1u
#define CLOSED       2u
#define ERROR       3u 
#define SYNCLINE        4u
#define FEEDBACK_PULSE_OUTPUT   5u

/*******************************************************************************
* Function Name  : SafetySwitchCheck
* Description    : Safety Switch.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetySwitchCheck( u8 pulse_on, u8 test_num )
{

    if( pulse_on != 0u )
    {
        if( PULSE_OUTPUT_FB == 0u )
        {
            if( SAFETY_SWITCH_INPUT_1 )
            {
                SAFETY_SWITCH[0][test_num] = CLOSED;
            }
            else
            {
                SAFETY_SWITCH[0][test_num] = OPEN;
            }
            
            if( SAFETY_SWITCH_INPUT_2 )
            {
                SAFETY_SWITCH[1][test_num] = CLOSED;
            }
            else
            {
                SAFETY_SWITCH[1][test_num] = OPEN;
            }  
            
            if( SAFETY_SWITCH_INPUT_3 )
            {
                SAFETY_SWITCH[2][test_num] = CLOSED;
            }
            else
            {
                SAFETY_SWITCH[2][test_num] = OPEN;
            }
            
            if( SAFETY_SWITCH_INPUT_4 )
            {
                SAFETY_SWITCH[3][test_num] = CLOSED;
            }
            else
            {
                SAFETY_SWITCH[3][test_num] = OPEN;
            }                 
        }
        else
        {
            FAULT = FEEDBACK_PULSE_OUTPUT;
            EN_ERROR8 |= 0x10u;
        }    
    }
    else
    {
        if( PULSE_OUTPUT_FB == 1u )
        {
            if( SAFETY_SWITCH_INPUT_1 )
            {
                SAFETY_SWITCH[0][test_num] = ERROR;
            }
            else
            {
                SAFETY_SWITCH[0][test_num] = UNDEFINED;
            }
            if( SAFETY_SWITCH_INPUT_2 )
            {
                SAFETY_SWITCH[1][test_num] = ERROR;
            }
            else
            {
                SAFETY_SWITCH[1][test_num] = UNDEFINED;
            }  
            
            if( SAFETY_SWITCH_INPUT_3 )
            {
                SAFETY_SWITCH[2][test_num] = ERROR;
            }
            else
            {
                SAFETY_SWITCH[2][test_num] = UNDEFINED;
            }
            
            if( SAFETY_SWITCH_INPUT_4 )
            {
                SAFETY_SWITCH[3][test_num] = ERROR;
            }
            else
            {
                SAFETY_SWITCH[3][test_num] = UNDEFINED;
            }                 
        }
        else
        {
            FAULT = FEEDBACK_PULSE_OUTPUT;
            EN_ERROR8 |= 0x10u;
        }   
    }
}

/*******************************************************************************
* Function Name  : SafetySwitchStatus
* Description    : Safety Switch.                 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetySwitchStatus(void)
{
    static u8 Pulse_Generation = 0u;
    static u8 SwitchState = 0u;
    static u8 Timeout_SYNC_Line = 0u;
    static u8 SafetySwitchInit = 0u;
    static u8 test_pattern = 0u;
    u8 switch_num;
    
    if( SafetySwitchInit == 0u )
    {
#ifdef GEC_SF_MASTER
        SYNC_SYS_OUT_SET();
#else
        SYNC_SYS_OUT_CLR();
#endif    
        
        PULSE_OUTPUT_CLR();
        Pulse_Generation = 0u;
        for( switch_num = 0u; switch_num < 4u; switch_num++ )
        {
            SAFETY_SWITCH_STATUS[switch_num] = UNDEFINED;
        }
        
        test_pattern = TEST_PATTERN;
        
        SafetySwitchInit = 1u;
    }   

#if 1   
    switch( SwitchState )
    {
       case 0:
        {
            if( (SYNC_SYS_IN == 0u) || (Pulse_Generation == 1u) )
            {
                Timeout_SYNC_Line = 0u;
                
                if( Pulse_Generation != 1u )
                {
                    Pulse_Generation = 1u;
                    SwitchState = 1u;
                }
                
                SYNC_SYS_OUT_SET();
                if( SYNC_SYS_IN != 0u )
                {
                    FAULT = SYNCLINE;
                    EN_ERROR8 |= 0x08u;
                    SwitchState = 6u;
                }
            }
            else
            {
                if( Timeout_SYNC_Line > 10u )
                {
                    FAULT = SYNCLINE;
                    EN_ERROR8 |= 0x08u;
                    SwitchState = 6u;            
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
            if( test_pattern & 0x01u )
            {
                PULSE_OUTPUT_SET(); 
            }
            else
            {
                PULSE_OUTPUT_CLR(); 
            }
            SwitchState = 2u;
            break;
        }
       case 2:
        {
            SafetySwitchCheck( (test_pattern & 0x01u), 0u );
            
            if( test_pattern & 0x02u )
            {
                PULSE_OUTPUT_SET(); 
            }
            else
            {
                PULSE_OUTPUT_CLR(); 
            }
            SwitchState = 3u;
            break;
        }
       case 3:
        {

            SafetySwitchCheck( (test_pattern & 0x02u), 1u );
            
            if( test_pattern & 0x04u )
            {
                PULSE_OUTPUT_SET(); 
            }
            else
            {
                PULSE_OUTPUT_CLR(); 
            }
            SwitchState = 4u;
            break;
        }   
       case 4:
        {

            SafetySwitchCheck( (test_pattern & 0x04u), 2u );
            
            if( test_pattern & 0x08u )
            {
                PULSE_OUTPUT_SET(); 
            }
            else
            {
                PULSE_OUTPUT_CLR(); 
            }
            SwitchState = 5u;
            break;
        }          
       case 5:
        {

            SafetySwitchCheck( (test_pattern & 0x08u), 3u );
            
            for( switch_num = 0u; switch_num < 4u; switch_num++ )
            {
                if( FAULT == FEEDBACK_PULSE_OUTPUT )
                {
                    SAFETY_SWITCH_STATUS[switch_num] = UNDEFINED;
                }
                else if ( (SAFETY_SWITCH[switch_num][0] == ERROR) || (SAFETY_SWITCH[switch_num][1] == ERROR) || (SAFETY_SWITCH[switch_num][2] == ERROR) || (SAFETY_SWITCH[switch_num][3] == ERROR) )
                {
                    SAFETY_SWITCH_STATUS[switch_num] = ERROR;
                }
                else if ( (SAFETY_SWITCH[switch_num][0] == OPEN) || (SAFETY_SWITCH[switch_num][1] == OPEN) || (SAFETY_SWITCH[switch_num][2] == OPEN) || (SAFETY_SWITCH[switch_num][3] == OPEN) )
                {
                    SAFETY_SWITCH_STATUS[switch_num]  = OPEN;
                }
                else if ( (SAFETY_SWITCH[switch_num][0] == CLOSED) || (SAFETY_SWITCH[switch_num][1] == CLOSED) || (SAFETY_SWITCH[switch_num][2] == CLOSED) || (SAFETY_SWITCH[switch_num][3] == CLOSED) )
                {
                    SAFETY_SWITCH_STATUS[switch_num]  = CLOSED;
                }
                else
                {
                    SAFETY_SWITCH_STATUS[switch_num] = UNDEFINED;
                }
            }           
            
            Pulse_Generation = 0u;
            PULSE_OUTPUT_CLR();
            SYNC_SYS_OUT_CLR();
            SwitchState = 7u;
            break;
        }
       case 6:
        {
            for( switch_num = 0u; switch_num < 4u; switch_num++ )
            {
                SAFETY_SWITCH_STATUS[switch_num] = UNDEFINED;
            }
            
            Pulse_Generation = 0u;
            PULSE_OUTPUT_CLR();
            SYNC_SYS_OUT_CLR();
            SwitchState = 7u;
            break;            
        }
       case 7:
        {
            SwitchState = 0u;
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
    static u8 sf_output_dis_tms = 0u; 
    
    if(( sf_wdt_check_en == 0u ) && ( SfBase_EscState & ESC_STATE_STOP )) 
    { 
        SF_RELAY_OFF();
        
        if( sf_relay_check_cnt == 1u )
        {
            sf_output_dis_tms++;
            if( sf_output_dis_tms * SYSTEMTICK > 50u )
            {
                SafetyRelayAuxRelayTest();
                sf_relay_check_cnt = 0u;
                sf_output_dis_tms = 0u;
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

    static u8 sf_output_en_tms = 0u; 
    
    
    if( SfBase_EscState == ESC_RUN_STATE ) 
    { 
        SF_RELAY_ON();    
        SF_EWDT_TOOGLE();
        
        if( sf_relay_check_cnt == 0u )
        {
            sf_output_en_tms++;
            if( sf_output_en_tms * SYSTEMTICK > 50u )
            {
                SafetyRelayAuxRelayTest();
                sf_relay_check_cnt = 1u;
                sf_output_en_tms = 0u;
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
static void SafetyRelayAuxRelayTest(void)
{
   
    if( SfBase_EscState == ESC_RUN_STATE ) 
    { 
        /* safety circuit is  connected */
        if( SF_RL_DRV_FB )
        {
            EN_ERROR8 |= 0x01u;
        }
        else if( SF_PWR_FB_CPU )
        {
            EN_ERROR8 |= 0x01u;
        }
        else if ( SF_RL_FB )
        {
            EN_ERROR8 |= 0x01u;
        }
        else if( !AUX_FB )
        {
            EN_ERROR8 |= 0x01u;
        }
        else
        {
            
        }
        
        /* MISRA C 2004 rule 12.4/12.5 can not compile */
/*
        if( SF_RL_DRV_FB || SF_PWR_FB_CPU || SF_RL_FB || !AUX_FB )
        {
//            FailSafeTest();
            EN_ERROR8 |= 0x01u;
        }   
*/
    }
    else if( SfBase_EscState & ESC_STATE_STOP )  
    {
        /* safety circuit is disconnected */
        if( !SF_RL_DRV_FB )
        {
            EN_ERROR8 |= 0x01u;
        }
        else if( SF_PWR_FB_CPU )
        {
            EN_ERROR8 |= 0x01u;
        }
        else if ( !SF_RL_FB )
        {
            EN_ERROR8 |= 0x01u;
        }
        else
        {
            
        }        
        
        /* MISRA C 2004 rule 12.4/12.5 can not compile */
/*                
        if( !SF_RL_DRV_FB || SF_PWR_FB_CPU || !SF_RL_FB )
        {
//            FailSafeTest();
            EN_ERROR8 |= 0x01u;
        } 
*/    
    }
    else
    {
    
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
/*    delay_ms(700);*/
#endif

    /* MISRA C 2004 rule 12.4/12.5 can not compile */
/*    
    if( SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB && AUX_FB )
    {
        SF_EWDT_TOOGLE();
        SF_RELAY_ON();   
        SF_EWDT_TOOGLE();
    }   
*/
    if( !SF_RL_DRV_FB )
    {

    }
    else if( SF_PWR_FB_CPU )
    {

    }
    else if ( !SF_RL_FB )
    {

    }
    else if( !AUX_FB )
    {

    }    
    else
    {
        SF_EWDT_TOOGLE();
        SF_RELAY_ON();   
        SF_EWDT_TOOGLE();        
    }  
    
    
    /*  wait 1800ms */
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
    delay_ms(600u);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    delay_ms(600u);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    
    /** Safety Relay and AuxRelay Test **/
    if( SF_RL_DRV_FB )
    {
        EN_ERROR8 |= 0x01u;
    }
    else if( SF_PWR_FB_CPU )
    {
        EN_ERROR8 |= 0x01u;
    }
    else if ( SF_RL_FB )
    {
        EN_ERROR8 |= 0x01u;
    }
    else if( !AUX_FB )
    {
        EN_ERROR8 |= 0x01u;
    }
    else
    {
        
    }    
    
    /* MISRA C 2004 rule 12.4/12.5 can not compile */
/*     
    if( (SF_RL_DRV_FB) || (SF_PWR_FB_CPU) || (SF_RL_FB) || (!AUX_FB) )
    {
//        FailSafeTest();
        EN_ERROR8 |= 0x01u;
    }   
*/    
    
    delay_ms(600u);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
   
    if( !SF_RL_FB )
    {
        /*FailSafeTest();*/
        EN_ERROR8 |= 0x02u;
    }
    else
    {
        SF_RELAY_OFF(); 
    }   
        
    if( EN_ERROR8 & 0x03u )
    {
        g_u32InitTestError = 1u;
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

    static u16 escState_old = ESC_READY_STATE; 
    
    if(( SfBase_EscState == ESC_READY_STATE ) && ( !(escState_old == ESC_READY_STATE)))
    {
        sf_wdt_check_en = 1u;
    }

    
    if( sf_wdt_check_en == 1u )
    {

        /* MISRA C 2004 rule 12.4/12.5 can not compile */
/*            
        if(( sf_wdt_check_tms == 0u ) && ( SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB ))
        {
            SF_EWDT_TOOGLE();
            SF_RELAY_ON();  
            SF_EWDT_TOOGLE();
        }   
*/     
        if( sf_wdt_check_tms == 0u )
        {
            if( !SF_RL_DRV_FB )
            {
                
            }
            else if( SF_PWR_FB_CPU )
            {
                
            }
            else if ( !SF_RL_FB )
            {
                
            }   
            else
            {
                SF_EWDT_TOOGLE();
                SF_RELAY_ON();   
                SF_EWDT_TOOGLE();        
            }                    
        }
        
        sf_wdt_check_tms++;
        /*  (41 - 1)*45ms = 1800ms */
        if( sf_wdt_check_tms >= 41u)
        {
            
            sf_wdt_check_tms = 0u;
            sf_wdt_check_en = 0u;
            
            if( !SF_RL_FB )
            {
                /*FailSafeTest();*/
                EN_ERROR8 |= 0x02u;
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
static void SafetyCTR_Check(void)
{
    static u16 sf_ctr_check_tms = 0u;
    
    if( SfBase_EscState == ESC_RUN_STATE ) 
    {    
        sf_ctr_check_tms++;
        /* check period 1s */ 
        if( sf_ctr_check_tms * SYSTEMTICK > 1000u )
        {
            SF_RELAY_OFF();
            delay_us(150u);
            if(!SF_RL_DRV_FB)
            {
                EN_ERROR_SYS2++;
                if(EN_ERROR_SYS2 > 2u)
                {
                    /* SafetyCTR_Check error */
                    /*ESC_SafeRelay_Error_Process();*/
                    EN_ERROR8 |= 0x04u;
                }
            }
            else
            {
                EN_ERROR_SYS2 = 0u;
            }
            SF_RELAY_ON();
        }
    }

}







/******************************  END OF FILE  *********************************/