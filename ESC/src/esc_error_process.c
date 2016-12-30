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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* fault type: Failure lock / standard fault / recovery fault */
u8 EscFaultType[512] = 
{
    /* 1~30,F00~F29 */                      
    EF,EU,EU,EU,EU,EU,EU,EF,EF,ES,  EU,EF,EF,EU,ES,ES,EU,EU,EU,EU,  EU,ES,ES,ES,ES,EU,ER,ER,ER,ER,
    /* 31~60,F30~F59 */                      
    EU,EU,ES,ES,EU,ES,ES,EU,ES,ES,  ES,ES,ES,ES,EF,EF,EF,EF,ES,EU,  EF,EF,EU,EU,EU,EU,EU,EU,EU,EU,  
    /* 61~90 */                      
    EU,EU,ES,EU,EU,EU,EU,ES,EU,EU,  EU,EU,EU,ES,ES,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 91~120 */                      
    EU,EU,EU,EU,EU,ES,ES,ES,ES,EU,  EU,EU,EU,EU,EU,EU,EU,EU,ES,EU,  EU,EU,EU,EU,EU,EU,EU,EU,ES,ES, 
    /* 121~150 */                      
    ES,ES,ES,ES,EF,ES,ES,ES,ES,EF,  EF,ES,ES,EU,EU,EU,EU,ES,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 151~180 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EF, 
    /* 181~210 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  ES,ES,ES,EU,EU,EU,EU,EU,EF,EF, 
    /* 211~240 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 241~270 */                      
    EU,EU,EU,EU,EU,EU,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EF,EF,EF,EF,  ES,ES,ES,EF,EF,EF,EF,EF,EF,EF, 
    /* 271~300 */                      
    ES,ES,ES,ES,ES,ES,ES,ES,EF,EF,  EF,EF,EF,EF,EF,EF,EF,EF,EF,EF,  ES,EF,EF,ER,ER,ES,ES,ES,ES,EF,  
    /* 301~330 */                      
    EF,ES,ES,ES,ES,EF,EF,EF,EF,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,EF,EF,ES,ES, 
    /* 331~360 */                      
    ES,ES,ES,ES,ES,EF,EF,EF,ES,ES,  ES,ES,ES,EF,EF,EF,EU,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, 
    /* 361~390 */                      
    EU,EU,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES, 
    /* 391~420 */                      
    ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,EF,EF,EF,  EF,EF,EF,EF,EF,EF,EF,EF,EF,EF, 
    /* 421~450 */                      
    EF,EF,EF,EF,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES, 
    /* 451~480 */                      
    ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,ES,ES,ES, 
    /* 481~512 */                      
    ES,ES,ES,ES,ES,ES,ES,ES,ES,ES,  ES,ES,ES,ES,ES,ES,ES,EU,EU,EU,  EU,EU,EU,EU,EU,EU,EU,EU,EU,EU, EU,EU   
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
            /*if( EscFaultType[EscErrorCodeBuff[k] - 1u] == ER )*/
            
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
    
    /* The warning can be reset when the escalator changes from fault state to ready state, when a reset is executed */
    if( SfBase_EscState == ESC_READY_STATE )
    {
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.Warn[i] = 0u;
        }
    } 
    
    /* reset safety board */
    if(( EscErrorCodeBuff[0] ) && ( EscFaultType[EscErrorCodeBuff[0] - 1u] <= ResetType ))
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
    
    /* reset control board */
    if( ResetType == 2u )
    {
        if((( EscRtData.DataFromControl[0][3] & ESC_FROM_CB_TYPE_OF_CONTROL_FAULT ) == ESC_FROM_CB_FAULT_FAILURE_LOCK )
            || (( EscRtData.DataFromControl[0][3] & ESC_FROM_CB_TYPE_OF_CONTROL_FAULT ) == ESC_FROM_CB_FAULT_STANDARD_FAULT ))
        {
            EscDataToControl[0][1] |= ORDER_RESET;
        }
    }
    else if( ResetType == 1u )
    {
        if(( EscRtData.DataFromControl[0][3] & ESC_FROM_CB_TYPE_OF_CONTROL_FAULT ) == ESC_FROM_CB_FAULT_STANDARD_FAULT )
        {
             EscDataToControl[0][1] |= ORDER_RESET;
        }
    }
    else
    {}
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
*                  fault :  1: fault 0: warn
* Output         : None
* Return         : None
*******************************************************************************/
void fault_code_decode(u16 code_buff[],u8 fault)
{  
    
    u16 i = 0u,j = 0u,error_counter = 0u,error_code_temp = 0u;
    u16 error_temp[5]={0,0,0,0,0};
    u8 error_num = 0u;
    static u16 stat_u16TimeOrderReset = 0u;
 
    
    if( fault ) 
    {
        /* fault code */
        error_num = 64u;
        for( i = 0u; i < error_num; i++ ) 
        {
            EscErrorBuff[i] = EscRtData.ErrorBuff[i];				
        }  
    }
    else
    {
        /* warn code */
        error_num = 8u;
        for( i = 0u; i < error_num; i++ ) 
        {
            EscErrorBuff[i] = EscRtData.Warn[i] | OmcEscRtData.Warn[i];				
        }         
    }
    
    error_counter = 0u;
    error_code_temp = 0u;
    
    for( i = 0u; i < error_num; i++ )
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
            
            if( fault ) 
            {
                CMD_FLAG5 |= ESC_FAULT;
                g_u8LedFreq = FREQ_3HZ;
            }
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

        if( fault ) 
        {
            CMD_FLAG5 &= ~ESC_FAULT;
            g_u8LedFreq = FREQ_1HZ;
        }
    }  
    
    if( fault ) 
    {
        for( i = 0u; i < 5u; i++ )
        {
            EscErrorCodeBuff[i] = code_buff[i];
        }
    }
    
    /* ORDER_RESET keep 100 ms */ 
    if( EscDataToControl[0][1] & ORDER_RESET )
    {
        if( stat_u16TimeOrderReset < 0xffffu )
        {
            stat_u16TimeOrderReset++;
        }
        
        if( stat_u16TimeOrderReset > 10u )
        {
            stat_u16TimeOrderReset = 0u;
            EscDataToControl[0][1] &= ~ORDER_RESET;
        }
    }
    else
    {
        stat_u16TimeOrderReset = 0u;
    }
}

/******************************  END OF FILE  *********************************/
