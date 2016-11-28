/*******************************************************************************
* File Name          : esc_error_process.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Last modify date   : 11/22/2016
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
/* failure lock */
#define EF       2u
/* standard fault */
#define ES       1u
/* recovery fault */
#define ER       3u
/* Undefined */
#define EU       0x0u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* fault type: Failure lock / standard fault / recovery fault */
static u8 EscFaultType[512] = 
{
    /* 1~30,F00~F29 */                      
    EF,EU,EU,EU,EU,EU,EU,EF,EF,ES,  EU,EF,EF,ES,ES,ES,EU,EU,ES,EU,  EU,ES,ES,ES,ES,EU,ER,ER,ER,ER,
    /* 31~60,F30~F59 */                      
    EU,EU,ES,ES,EU,ES,ES,EU,ES,ES,  ES,ES,ER,ER,EF,EF,EF,EF,ES,EU,  EF,EF,EU,EU,EU,EU,EU,EU,ES,EU,  
    /* 61~90 */                      
    EU,EU,ES,EU,EU,EU,EU,ES,EU,EU,  EU,EU,EU,ES,ES,EU,EU,EU,ES,EU,  EU,EU,EU,EU,EU,ES,EU,EU,ES,EU, 
    /* 91~120 */                      
    EU,EU,ES,EU,EU,ES,ES,ES,ES,EU,  EU,EU,EU,EU,EU,EU,EU,EU,ES,EU,  EU,EU,EU,EU,EU,EU,EU,EU,ES,ES, 
    /* 121~150 */                      
    ES,ES,ES,ES,EF,ES,ES,ES,ES,EF,  EF,ES,ES,ES,ES,EU,EU,ES,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 151~180 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EF, 
    /* 181~210 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,ES,  ES,ES,ES,EU,EU,EU,EU,EU,ES,ES, 
    /* 211~240 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 241~270 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EF,EF,EF,EF,EF,EF,ES,EF,EF,ES, 
    /* 271~300 */                      
    ES,ES,ES,ES,ES,EF,ES,ES,ES,ES,  ES,ES,EF,EF,ES,ES,ES,ES,ES,ES,  ES,ES,EU,EU,EU,EU,EU,ES,ES,EU,  
    /* 301~330 */                      
    ES,ES,ES,EF,EF,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,EU, 
    /* 331~360 */                      
    EU,ES,ES,ES,ES,ES,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EF,ES,EU,EU,EU,EU,EU,EU, 
    /* 361~390 */                      
    EU,EU,EU,EU,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES, 
    /* 391~420 */                      
    ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,EF,EF,EF, 
    /* 421~450 */                      
    ES,ES,ES,ES,ES,ES,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 451~480 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 481~512 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, EU,EU   
};


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
            /* recovery fault type */
            /*if( EscFaultType[EscErrorCodeBuff[k]] == ER )*/
            
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
* Input          : ResetType: 1: statand fault reset, 2: failure lock reset            
* Output         : None
* Return         : None
*******************************************************************************/
void fault_code_manual_reset(u8 ResetType)
{  
    u16 i,j;
    
    if(( EscErrorCodeBuff[0] ) && ( EscFaultType[EscErrorCodeBuff[0]] <= ResetType ))
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
            g_u8LedFreq = FREQ_2HZ;
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
        g_u8LedFreq = FREQ_0_5HZ;
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
