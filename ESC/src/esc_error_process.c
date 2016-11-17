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
#include "crc16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : fault_code_auto_reset
* Description    : 
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void fault_code_auto_reset(void)
{  
    u16 i,j,k,resetflag;
    
    for( k = 0u; k < 5u; k++ )
    {
        if( resetflag == 1u )
        {
            k = 0u;
            resetflag = 0u;
        }
        
        if( EscErrorCodeBuff[k] )
        {
            i = (EscErrorCodeBuff[k]-1u)/8u;
            j = (EscErrorCodeBuff[k]-1u)%8u;
            
            if(!(EscRtData.ErrorBuff[i] & ( 1u << j )))
            {
                EscErrorCodeBuff[k] = 0u;
                EscRtData.ErrorCode[k] = 0u;
                for( i = 0u; i < 5u; i++ )
                {
                    EscErrorCodeBuff[i] = EscErrorCodeBuff[i + 1u];
                    EscRtData.ErrorCode[i] = EscErrorCodeBuff[i + 1u];
                } 
                resetflag = 1u;
            }
        }
        else
        {
            break;
        }
    }
}

/*******************************************************************************
* Function Name  : fault_code_manual_reset
* Description    : 
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void fault_code_manual_reset(void)
{  
    u16 i,j;
    
    if( EscErrorCodeBuff[0] ) 
    {
        i = (EscErrorCodeBuff[0]-1u)/8u;
        j = (EscErrorCodeBuff[0]-1u)%8u;
        
        EscRtData.ErrorBuff[i] &= ~( 1u << j );
        
        EscErrorCodeBuff[4] = 0u;
        EscRtData.ErrorCode[4] = 0u;
        for( i = 0u; i < 4u; i++ )
        {
            EscErrorCodeBuff[i] = EscErrorCodeBuff[i + 1u];
            EscRtData.ErrorCode[i] = EscRtData.ErrorCode[i + 1u];
        }
    }
}

/*******************************************************************************
* Function Name  : error_change_check
* Description    : 
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void error_change_check(void)
{ 
    static u16 ErrorBuffCRC = 0u;
    u8 errorbuff[64];
    u16 i;
    
    
    for( i = 0u; i < 64u; i++ ) 
    {
        errorbuff[i] = EscRtData.ErrorBuff[i] | OmcEscRtData.ErrorBuff[i];
        /* for test, only cpu1 */
        /*errorbuff[i] = EscRtData.ErrorBuff[i];*/
    }     
        
    i = MB_CRC16( errorbuff, 64u );
    
    if( i != ErrorBuffCRC )
    {
        ErrorBuffCRC = i;
        g_u8FaultCodeStore = 1u;
    }   
}


/*******************************************************************************
* Function Name  : fault_code_decode
* Description    : The maxium number of decode is 5.
* Input          : code_buff : recode fault parameter.            
* Output         : None
* Return         : None
*******************************************************************************/
void fault_code_decode(u16 code_buff[])
{  
    
    u16 i = 0u,j = 0u,error_counter = 0u,error_code_temp = 0u;
    u16 error_temp[5]={0,0,0,0,0};
    
       
    for( i = 0u; i < 64u; i++ ) 
    {
        EscErrorBuff[i] = EscRtData.ErrorBuff[i];				
    }  
    
    error_counter = 0u;
    error_code_temp = 0u;
    
    for( i = 0u; i < 64u; i++ )
    {
        error_code_temp = i*8u;
        
        if(EscErrorBuff[i])
        {
            for( j = 0u; j < 8u; j++ )
            {
                error_code_temp++;
                if(EscErrorBuff[i] & (1u << j)) 
                {
                    error_temp[error_counter] = error_code_temp; 
                    error_counter++;
                    
                    if(error_counter >= 5u) 
                    {
                        break;
                    }
                }  
            } 
            
            CMD_FLAG5 |= ESC_FAULT;
        } 
        
        if(error_counter >= 5u) 
        {
            break;
        }
    }    
    
    if( error_counter > 0u )
    {
        for( i = 0u; i < error_counter; i++ )
        {
            if(error_temp[i])
            {
                for( j = 0u; j < 5u; j++ )
                {
                    if( ((code_buff[j]) && ( code_buff[j] == error_temp[i] )) || ( code_buff[j] == 0u ))
                    {
                        if( code_buff[j] == 0u )
                        {
                            code_buff[j] = error_temp[i];
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        code_buff[0] = 0u;	
        code_buff[1] = 0u;	
        code_buff[2] = 0u;	
        code_buff[3] = 0u;	
        code_buff[4] = 0u;

        CMD_FLAG5 &= ~ESC_FAULT;
    }  
    
    
    for( i = 0u; i < 5u; i++ )
    {
        EscErrorCodeBuff[i] = code_buff[i];
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
/*   
    while(1)
    {
*/    
        /** In the error handling routine in an infinite loop **/
          /**  Wait manual reset fault **/
/*        
          if( IN10 )
          {
                __set_FAULTMASK(1);   
                NVIC_SystemReset();
          
          }
      
    }
*/  
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
/*    
    SF_RL_CTR_CLR();
    SF_RL_WDT_CLR();
*/    
    /* ESC_SPI_Error_Process */
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
    /* ESC_SafeRelay_Error_Process */
}
