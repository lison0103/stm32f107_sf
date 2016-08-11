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
#include "esc.h"
#include "ewdt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u8 EscBuff[10] = {0};


/*******************************************************************************
* Function Name  : fault_code_decode
* Description    : The maxium number of decode is 5.
* Input          : code_buff : recode fault parameter.            
* Output         : None
* Return         : None
*******************************************************************************/
void fault_code_decode(u8* code_buff)
{  
    
    u8 i=0,j=0,error_counter=0,error_code_temp=0;
    u8 error_temp[5]={0,0,0,0,0};
    
       
    for( i = 0; i < 10; i++ ) 
    {
        pcEscErrorBuff[i] = pcErrorBuff[i] | pcOmcErrorBuff[i];				
    }  
    
    error_counter = 0;
    error_code_temp = 0;
    
    for( i = 0; i < 10; i++ )
    {
        error_code_temp = i*8;
        
        if(pcEscErrorBuff[i])
        {
            for( j = 0; j < 8; j++ )
            {
                error_code_temp++;
                if(pcEscErrorBuff[i] & (1 << j)) 
                {
                    error_temp[error_counter] = error_code_temp; 
                    error_counter++;
                    
                    if(error_counter >= 5) break;
                }  
            }    
        } 
        
        if(error_counter >= 5) break;
    }    
    
    
    if(error_temp[0])
    {
        code_buff[0] = error_temp[0];
        code_buff[1] = error_temp[1];
        code_buff[2] = error_temp[2];
        code_buff[3] = error_temp[3];
        code_buff[4] = error_temp[4];
    }
    else
    {
        code_buff[0] = 0;	
        code_buff[1] = 0;	
        code_buff[2] = 0;	
        code_buff[3] = 0;	
        code_buff[4] = 0;	
    }  
}


/*******************************************************************************
* Function Name  : ESC_Init_Fault
* Description    : Esc init fault.
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Init_Fault(void)
{
 
    /** Disconnect the safety relay **/
    SF_RL_CTR_CLR();
    SF_RL_WDT_CLR();
   
    while(1)
    {
        EWDT_TOOGLE();
        IWDG_ReloadCounter();
    }

}

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
    SF_RL_CTR_CLR();
    SF_RL_WDT_CLR();
   
    while(1)
    {
        /** In the error handling routine in an infinite loop **/
          /**  Wait manual reset fault **/
//          if( IN10 )
//          {
//                __set_FAULTMASK(1);   
//                /** soft reset **/
//                NVIC_SystemReset();
//          
//          }
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
    SF_RL_CTR_CLR();
    SF_RL_WDT_CLR();  

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
    SF_RL_CTR_CLR();
    SF_RL_WDT_CLR();    
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
//    SF_RL_CTR_CLR();
//    SF_RL_WDT_CLR();
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
    SF_RL_CTR_CLR();
    SF_RL_WDT_CLR();
    printf("ESC_SafeRelay_Error_Process \r\n");
}