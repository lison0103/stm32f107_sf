/*******************************************************************************
* File Name          : esc_error_process.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains esc error process functions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_error_process.h"
#include "hw_test.h"
#include "sys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


int EscBuff[10] = {0};



/*******************************************************************************
* Function Name  : ESC_EWDT_Error_Process
* Description    : External watchdog checked fail
*                  In the error handling routine in an infinite loop, disconnect the safety relay
*                  Escalator stops running, enter the fault state, waiting for manual reset fault.
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_EWDT_Error_Process(void)
{
 
    /** Disconnect the safety relay **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;
   
    while(1)
    {
        /** In the error handling routine in an infinite loop **/
          /**  Wait manual reset fault **/
          if( IN10 )
          {
                __set_FAULTMASK(1);   
                /** soft reset **/
                NVIC_SystemReset();
          
          }
    }

}


/*******************************************************************************
* Function Name  : ESC_Flash_Error_Process
* Description    : Fram checked fail
*                  Error handling routine, safety disconnect relay,
*                  Escalator stops running, enter the fault state, 
*                  waiting for manual correction parameters
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Flash_Error_Process(void)
{
 
    /** Disconnect the safety relay **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;  

}



/*******************************************************************************
* Function Name  : ESC_Flash_Error_Process
* Description    : Flash checked fail
*                  In the error handling routine in an infinite loop, waiting watchdog reset
*                  Disconnect the safety relay, the escalator stops running, enter the fault state, 
*                  waiting for the fault reset.
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Fram_Error_Process(void)
{
#ifdef GEC_SF_MASTER  
    /** Disconnect the safety relay **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;    
#endif    

}


/*******************************************************************************
* Function Name  : ESC_SPI_Error_Process
* Description    : SPI checked fail
*                  Entering the error handling routine, safety disconnect relay,
*                  Escalator stops running into the fault state
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_SPI_Error_Process(void)
{
  
    /** Disconnect the safety relay **/
//    SF_RL_CTR = 0;
//    SF_RL_WDT = 0;
    printf("ESC_SPI_Error_Process \r\n");
}


/*******************************************************************************
* Function Name  : ESC_SafeRelay_Error_Process
* Description    : ESC SafeRelay checked fail
*                  Entering the error handling routine, safety disconnect relay,
*                  Escalator stops running into the fault state
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_SafeRelay_Error_Process(void)
{
 
    /** Disconnect the safety relay **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;
    printf("ESC_SafeRelay_Error_Process \r\n");
}