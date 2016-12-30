/*******************************************************************************
* File Name          : esc_safety_check.c
* Author             : lison
* Version            : V1.0
* Date               : 06/20/2016
* Last modify date   : 09/19/2016
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
#define SAFETY_SWITCH_TIME              20u
#define STATE_ERROR_TIME                200u
#define SAFETY_SWITCH_CHECK_DELAY       1000u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u8 sf_wdt_check_tms = 0u;
static u8 sf_wdt_check_en = 0u;
/* static u8 test_num = 0;*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void SafetySwitchCheck(u8 check);

static u8 SW_T_State = 0u;
static u16 SW_T_Delay_Tms = 0u;


/*******************************************************************************
* Function Name  : SafetySwitchCheck
* Description    : Safety Switch.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetySwitchCheck(u8 check)
{
    u8 state = 0u;
    
    if( check == 1u )
    {     
        /*
        if( OmcEscRtData.SafetySwitchState == EscRtData.SafetySwitchState )
        { */           
            if(( EscRtData.SafetySwitchState == 2u ) || ( EscRtData.SafetySwitchState == 6u ))
            {
#ifdef GEC_SF_MASTER                 
                CPU1_PULSE_OUTPUT_OFF();
#else
                CPU2_PULSE_OUTPUT_OFF();
#endif                
                delay_us(SAFETY_SWITCH_CHECK_DELAY);
                
                if( PULSE_OUTPUT_FB  == 1u )
                {
                    if( SAFETY_SWITCH_INPUT_1 == 1u )
                    {
                        /* Safety switch 1 fault F362 */
                        EN_ERROR46 |= 0x04u;
                    }
                    if( SAFETY_SWITCH_INPUT_2 == 1u )
                    {
                        /* Safety switch 2 fault F363 */
                        EN_ERROR46 |= 0x08u;
                    }
                    if( SAFETY_SWITCH_INPUT_3 == 1u )
                    {
                        /* Safety switch 3 fault F364 */
                        EN_ERROR46 |= 0x10u;
                    }
                    if( SAFETY_SWITCH_INPUT_4 == 1u )
                    {
                        /* Safety switch 4 fault F365 */
                        EN_ERROR46 |= 0x20u;
                    }                  
                }
                else
                {
                    /* Feedback fault pulse output safety switch (F368) */
                    EN_ERROR47 |= 0x01u;            
                }
#ifdef GEC_SF_MASTER                 
                CPU1_PULSE_OUTPUT_ON();
#else
                CPU2_PULSE_OUTPUT_ON();
#endif                
                SW_T_State += 1u;
                SW_T_Delay_Tms = 0u;            
            }
            else
            {
                if( SW_T_Delay_Tms++ > SAFETY_SWITCH_TIME )
                {
                    SW_T_State += 1u;
                    if( SW_T_State > 8u )
                    {
                        SW_T_State = 1u;
                    }
                    SW_T_Delay_Tms = 0u;
                    if(( EscRtData.SafetySwitchState == 1u ) || ( EscRtData.SafetySwitchState == 5u ))
                    {
                        if( PULSE_OUTPUT_FB  == 1u )
                        {
                            /* Feedback fault pulse output safety switch (F368) */
                            EN_ERROR47 |= 0x01u;
                        }
                    }
                }
            }
#if 0
        }
        else
        {
            if( SW_T_Delay_Tms++ > STATE_ERROR_TIME )
            {
                /* SAFETY SWITCH ERROR F361*/
                EN_ERROR46 |= 0x02u;
                SW_T_Delay_Tms = 0u;
                SW_T_State = 0u;                    
            }
        }     
#endif    
    }
    else
    {     
        if( EscRtData.SafetySwitchState == 8u )
        {
            state = 0u;
        }
#ifdef GEC_SF_MASTER         
        else if( EscRtData.SafetySwitchState == 5u )
        {
            state = 6u;
        }
#else
        else if( EscRtData.SafetySwitchState == 1u )
        {
            state = 2u;
        }
#endif        
        else
        {
            state = EscRtData.SafetySwitchState;
        }
        
        if( OmcEscRtData.SafetySwitchState == ( state + 1u ))
        {
            SW_T_State += 1u;
            if( SW_T_State > 8u )
            {
                SW_T_State = 1u;
            }            
            SW_T_Delay_Tms = 0u;                
        }
        else
        {
            if( SW_T_Delay_Tms++ > STATE_ERROR_TIME )
            {
                /* SAFETY SWITCH ERROR F361*/
                EN_ERROR46 |= 0x02u;
                SW_T_Delay_Tms = 0u;
                SW_T_State = 0u;                    
            }
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
#if 1    
    EscRtData.SafetySwitchState = SW_T_State;
    EscRtData.SafetySwitchEnter = 1u;
    
    /* Switch n close or open check in Get_GpioInput() */
  
    if( OmcEscRtData.SafetySwitchEnter )
    {
        /* Safety switch hardware be checked every 800ms */
        switch( SW_T_State )
        {
           case 0:
            {
#ifdef GEC_SF_MASTER            
                CPU1_PULSE_OUTPUT_ON();
                SafetySwitchCheck(1u);           
#else
                CPU2_PULSE_OUTPUT_ON();
                SafetySwitchCheck(0u);
#endif            
                break;
            }
           case 1:
            {
#ifdef GEC_SF_MASTER      
                CPU1_PULSE_OUTPUT_ON();
                SafetySwitchCheck(1u);
#else
                CPU2_PULSE_OUTPUT_OFF();
                SafetySwitchCheck(0u);            
#endif            
                break;
            }
           case 2:
            {
#ifdef GEC_SF_MASTER    
                SafetySwitchCheck(1u);
#else
                CPU2_PULSE_OUTPUT_OFF();
                SafetySwitchCheck(0u);          
#endif 
                break;
            }
           case 3:
            {
#ifdef GEC_SF_MASTER       
                CPU1_PULSE_OUTPUT_ON();
                SafetySwitchCheck(1u);           
#else
                CPU2_PULSE_OUTPUT_OFF();
                SafetySwitchCheck(0u);          
#endif           
                break;
            }   
           case 4:
            {
#ifdef GEC_SF_MASTER            
                CPU1_PULSE_OUTPUT_ON();
                SafetySwitchCheck(1u);          
#else
                CPU2_PULSE_OUTPUT_ON();
                SafetySwitchCheck(0u);              
#endif
                break;
            }          
           case 5:
            {
#ifdef GEC_SF_MASTER            
                CPU1_PULSE_OUTPUT_OFF();
                SafetySwitchCheck(0u);           
#else            
                CPU2_PULSE_OUTPUT_ON();
                SafetySwitchCheck(1u);            
#endif 
                break;
            }
           case 6:
            {
#ifdef GEC_SF_MASTER                       
                CPU1_PULSE_OUTPUT_OFF();
                SafetySwitchCheck(0u);           
#else
                SafetySwitchCheck(1u);            
#endif
                break;            
            }
           case 7:
            {
#ifdef GEC_SF_MASTER            
                CPU1_PULSE_OUTPUT_OFF();
                SafetySwitchCheck(0u);           
#else
                CPU2_PULSE_OUTPUT_ON();
                SafetySwitchCheck(1u);            
#endif
                break;            
            }   
           case 8:
            {
#ifdef GEC_SF_MASTER            
                CPU1_PULSE_OUTPUT_ON();
                SafetySwitchCheck(0u);            
#else
                CPU2_PULSE_OUTPUT_ON();
                SafetySwitchCheck(1u);            
#endif
                break;            
            }         
           default:
            break;
        }
    }
    
    
#else
 
/* for test */    
#ifdef GEC_SF_MASTER 
    CPU1_PULSE_OUTPUT_OFF();
    delay_us(650u);
    
    if( PULSE_OUTPUT_FB  == 1u )
    {
        if( SAFETY_SWITCH_INPUT_1 == 1u )
        {
            /* Safety switch 1 fault F362 */
            EN_ERROR46 |= 0x04u;
        }                  
    }
    else
    {
        /* Feedback fault pulse output safety switch (F368) */
        EN_ERROR47 |= 0x01u;            
    }
    
    CPU1_PULSE_OUTPUT_ON();    
#else
    CPU2_PULSE_OUTPUT_OFF();
#endif    
    
#endif    
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
        /* safety circuit is connected */
        if( SF_RL_DRV_FB )
        {
            EN_ERROR49 |= 0x04u;
        }      
        else if ( SF_RL_FB )
        {
            EN_ERROR49 |= 0x04u;
        }
        else if( !AUX_FB )
        {
            EN_ERROR49 |= 0x04u;
        }
        else
        {
            
        }
    }
    else
    {
        /* safety circuit is disconnected */
        if( !SF_RL_DRV_FB )
        {
            EN_ERROR49 |= 0x04u;
        }      
        else if ( !SF_RL_FB )
        {
            EN_ERROR49 |= 0x04u;
        }
        else
        {
            
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
/*    delay_ms(700);*/
#endif

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
        EN_ERROR49 |= 0x04u;
    }
    else if( SF_PWR_FB_CPU )
    {
        EN_ERROR49 |= 0x04u;
    }
    else if ( SF_RL_FB )
    {
        EN_ERROR49 |= 0x04u;
    }
    else if( !AUX_FB )
    {
        EN_ERROR49 |= 0x04u;
    }
    else
    {
        
    }     
    
    delay_ms(600u);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
   
    if( !SF_RL_FB )
    {
        /*FailSafeTest();*/
        /* Safety relay outp_Ext. watchd error F386 */
        EN_ERROR49 |= 0x04u;
    }

    SF_RELAY_OFF();    
        
    if( EN_ERROR49 & 0x04u )
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
    
    if(( SfBase_EscState != ESC_STOPPING_PROCESS_STATE ) && ( (escState_old == ESC_STOPPING_PROCESS_STATE)))
    {
        sf_wdt_check_en = 1u;
    }

    
    if( sf_wdt_check_en == 1u )
    {    
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
                /* Safety relay outp_Ext. watchd error F386 */
                EN_ERROR49 |= 0x04u;
            }

            SF_RELAY_OFF();   
        }
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
    static u8 stat_u8CheckError = 0u;
    
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
                stat_u8CheckError++;
                if(stat_u8CheckError > 2u)
                {
                    /* SafetyCTR_Check error */
                    /* Safety relay outp_Ext. watchd error F386 */
                    EN_ERROR49 |= 0x04u;
                }
            }
            else
            {
                stat_u8CheckError = 0u;
            }
            SF_RELAY_ON();
        }
    }
}

/*******************************************************************************
* Function Name  : SafetyOutputDisable
* Description    : Safety Relay disable.                 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyOutputDisable(void)
{
    SF_RELAY_OFF();
}

/*******************************************************************************
* Function Name  : SafetyOutputEnable
* Description    : Safety Relay enable.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyOutputEnable(void)
{
    SF_RELAY_ON();    
    SF_EWDT_TOOGLE();     
}

/******************************  END OF FILE  *********************************/


