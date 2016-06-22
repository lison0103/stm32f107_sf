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

u8 switch_flag = 1;
u8 sfwdt_checkflag = 0;
u8 sf_wdt_check_tms = 0;
u8 sf_wdt_check_en = 0;
u8 sf_relay_check_cnt = 0;
//static u8 test_num = 0;
#ifdef GEC_SF_MASTER
u8 R_SF_RL2_FB_CPU1;
#else
u8 R_SF_RL_FB_CPU2;

#endif


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
            FailSafeTest();
        }   
    }
    else if( SfBase_EscState & ESC_STATE_STOP )  
    {
        /* safety circuit is disconnected */
        if( !SF_RL_DRV_FB || SF_PWR_FB_CPU || !SF_RL_FB )
        {
            FailSafeTest();
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
    delay_ms(700);
#endif

    if( SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB && AUX_FB )
    {
        SF_EWDT_TOOGLE();
//        AUX_CTR = 1;
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
//    SafetyRelayAuxRelayTest();
    if( SF_RL_DRV_FB || SF_PWR_FB_CPU || SF_RL_FB || !AUX_FB )
    {
        FailSafeTest();
    }   
    
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
   
    if( !SF_RL_FB )
    {
        FailSafeTest();
    }
    else
    {
//        AUX_CTR = 0;
        SF_RELAY_OFF(); 
//        sfwdt_checkflag = 2;
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
                FailSafeTest();
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
                    printf("SafetyCTR_Check error \n");
                    ESC_SafeRelay_Error_Process();
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