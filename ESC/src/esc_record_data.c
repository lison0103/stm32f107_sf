/*******************************************************************************
* File Name          : esc_record_data.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Last modify date   : 11/22/2016
* Description        : This file contains esc record and store data.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_record_data.h"
#include "mb85rcxx.h"
#include "crc16.h"
#include "delay.h"
#include "esc_error_process.h"
#include "esc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ESC_RECORD_NUM 1024u
#define ERROR_BACKUP_STORE_TIME 1000u
#define FRAM_WRITE_LENGTH       10u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u8 g_u8FaultCodeStore = 0u;
u16 g_u16FramWriteAdr = 0u;
u16 g_u16FramWriteLen = 0u;
u8 FramWriteData;
u8 FramWriteBuffer[ESC_ERROR_NUM];
u8 FramNeedStore = 0u;

/*******************************************************************************
* Function Name  : Check_Error_Present_Memory
* Description    : esc check error in memory.                 
* Input          : None          
* Output         : None
* Return         : return last power off esc state
*******************************************************************************/
u8 Check_Error_Present_Memory(void)
{
    u8 readbuffer[ESC_ERROR_NUM];
    u8 result = 0u;
    u8 i;
    u16 temp;
    u8 state = 0u;
    
    result = fram_data_read(ESC_ERROR_ADR, ESC_ERROR_NUM, readbuffer);
    if( !result )
    {
        if( (readbuffer[0] == 0xfau) && (readbuffer[1] == 0xedu))
        {
            /* 5 Error Code */
            for( i = 0u; i < 5u; i++ )
            {
                EscRtData.ErrorCode[i] |= readbuffer[i*2u + 2u];
                temp = (u16)readbuffer[i*2u + 3u] << 8u;
                EscRtData.ErrorCode[i] |= temp;
                EscErrorCodeBuff[i] = EscRtData.ErrorCode[i];
            }
            
            /* Error Buffer */
            for( i = 0u; i < 64u; i++ )
            {
                EscRtData.ErrorBuff[i] = readbuffer[i + 12u];
                if( EscRtData.ErrorBuff[i] )
                {
                    result = 1u;
                }
            }     
        }
    }
    else
    {
        /* epprom read error */
        EN_ERROR11 |= 0x01u;
    }
    
    if(( EscRtData.ErrorCode[0] ) || ( result ))
    {
        state = ESC_FAULT_STATE;
    }
    else
    {
        state = ESC_READY_STATE;
    }
    
    return state;
}

/*******************************************************************************
* Function Name  : StoreFaultInMemory
* Description    : Store Fault code In Memory.
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void StoreFaultInMemory(void)
{
    static u16 stat_u16TimerStoreFault = 0u;
    u8 u8DataStore = 0u;
    u16 i;
    
    if(( SfBase_EscState == ESC_FAULT_STATE ) || ( SfBase_EscState == ESC_READY_STATE ))
    {
        /* check error code change */
        error_change_check();
        
        if( g_u8FaultCodeStore == 1u )
        {
            u8DataStore = 1u;
            g_u8FaultCodeStore = 2u;
            stat_u16TimerStoreFault = 0u;
        }
        else
        {
            if( g_u8FaultCodeStore == 2u )
            {
                stat_u16TimerStoreFault++;
                if( stat_u16TimerStoreFault * 5u > ERROR_BACKUP_STORE_TIME )
                {
                    u8DataStore = 1u;
                    g_u8FaultCodeStore = 0u;
                }
            } 
        }
        
        if( u8DataStore == 1u )
        {
            /* first, clear buffer */
            for( i = 0u; i < ESC_ERROR_NUM; i++ )
            {
                FramWriteBuffer[i] = 0u;
            }
            
            /* Header */
            FramWriteBuffer[0] = 0xfau;
            FramWriteBuffer[1] = 0xedu;
            
            /* 5 Error Code */
            for( i = 0u; i < 5u; i++ )
            {
                FramWriteBuffer[i*2u + 2u] = (u8)(EscRtData.ErrorCode[i] & 0xffu);
                FramWriteBuffer[i*2u + 3u] = (u8)((EscRtData.ErrorCode[i] >> 8u) & 0xffu);
            }
            
            /* Error FramWriteBuffer */
            for( i = 0u; i < 64u; i++ )
            {
                FramWriteBuffer[i + 12u] = EscRtData.ErrorBuff[i] | OmcEscRtData.ErrorBuff[i];
            }
            
            /* CRC16 */
            i = MB_CRC16( FramWriteBuffer, ESC_ERROR_NUM - 2u );
            FramWriteBuffer[ESC_ERROR_NUM - 2u] = (u8)i;
            FramWriteBuffer[ESC_ERROR_NUM - 1u] = (u8)(i >> 8u);
            
            /* record in fram */
            if( g_u8FaultCodeStore )
            {
                /*fram_data_write(ESC_ERROR_ADR, ESC_ERROR_NUM, FramWriteBuffer, 0u);  */
                g_u16FramWriteAdr = ESC_ERROR_ADR;
                g_u16FramWriteLen = ESC_ERROR_NUM;
                FramWriteData = 0u;       
                FramNeedStore = 1u;
            }
            else
            {
                /* backup */
                /*fram_data_write(ESC_BACKUP_ADR + ESC_ERROR_ADR, ESC_ERROR_NUM, FramWriteBuffer, 0u);  */
                g_u16FramWriteAdr = ESC_BACKUP_ADR + ESC_ERROR_ADR;
                g_u16FramWriteLen = ESC_ERROR_NUM;
                FramWriteData = 0u;        
                FramNeedStore = 1u;
            }
            
            u8DataStore = 0u;
        } 
        
        /* store to Fram 10byte/5ms to reduce times */ 
        fram_store_data();
    }
}

/*******************************************************************************
* Function Name  : fram_store_data
* Description    : Store the data to Fram.                  
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void fram_store_data(void)
{
    static u16 stat_u16WriteLength = 0u;
    
    if( FramNeedStore == 1u )
    {
        FramNeedStore = 2u;
        stat_u16WriteLength = FRAM_WRITE_LENGTH;
    }
    
    if( FramNeedStore )
    {
        if( eeprom_write(g_u16FramWriteAdr, stat_u16WriteLength, &FramWriteBuffer[FramWriteData]))
        {
            /* FAULT: eeprom write error */
            
        }  
        else
        {
            g_u16FramWriteLen -= stat_u16WriteLength;
            if( g_u16FramWriteLen > FRAM_WRITE_LENGTH )
            {
                stat_u16WriteLength = FRAM_WRITE_LENGTH;
                g_u16FramWriteAdr += FRAM_WRITE_LENGTH;
                FramWriteData += FRAM_WRITE_LENGTH;
            }            
            else if( g_u16FramWriteLen == 0u )
            {
                FramNeedStore = 0u;
            }
            else
            {                
                stat_u16WriteLength = g_u16FramWriteLen;
                g_u16FramWriteAdr += FRAM_WRITE_LENGTH;   
                FramWriteData += FRAM_WRITE_LENGTH;
            }
        }
    }
}

/*******************************************************************************
* Function Name  : fram_data_read
* Description    : Read the data from the Fram.                  
* Input          : Adr: fram address, ReadData: read buffer, len: the length of data          
* Output         : None
* Return         : 0: success   1: fail
*******************************************************************************/
u8 fram_data_read(u16 Adr, u16 len, u8 ReadData[])
{
    u16 i;
    u8 Fram_Data[ESC_PARA_NUM] = {0};
    u8 Fram_Data_Backup[ESC_PARA_NUM] = {0};
    u8 errorflag = 0u,result = 0u;
        
    if( Adr == ESC_ERROR_ADR )
    {
        if(!eeprom_read(Adr, len, Fram_Data))
        {
            if(MB_CRC16(Fram_Data, len))
            {
                errorflag = 1u;
            }
        }
        else
        {
            errorflag = 1u;
        }
        
        if( errorflag == 1u )
        {
            if(!eeprom_read(ESC_BACKUP_ADR + Adr, len, Fram_Data))
            {
                if(!MB_CRC16(Fram_Data, len))
                {
                    errorflag = 0u;
                }
            }
        }
    }
    else if( Adr == ESC_PARA_ADR )
    {
        if(!eeprom_read(Adr, len, Fram_Data))
        {
            if(!MB_CRC32(Fram_Data, len, PARAMETER_POLYNOMIALS))
            {
                delay_ms(10u);
                
                if(!eeprom_read(ESC_BACKUP_ADR + Adr, len, Fram_Data_Backup))
                {
                    if(!MB_CRC32(Fram_Data_Backup, len, PARAMETER_POLYNOMIALS))
                    {                       
                        for( i = 0u; i < len; i++ )
                        {
                            result = Fram_Data[i]^Fram_Data_Backup[i];
                            if( result )
                            {
                                errorflag = 1u;
                                break;
                            }                
                        }
                    } 
                    else
                    {
                        errorflag = 1u;
                    }
                }
                else
                {
                    errorflag = 1u;
                }
            } 
            else
            {
                errorflag = 1u;
            }
        }   
        else
        {
            errorflag = 1u;
        }  
    }
    else
    {}
    
    if( errorflag == 0u )
    {
        for( i = 0u; i < len; i++ )
        {
            ReadData[i] = Fram_Data[i];
        }
    }
    else
    {
        for(i = 0u; i < len; i++)
        {
            Fram_Data[i] = 0u;
        } 
        if( Adr == ESC_ERROR_ADR )
        {
            fram_data_write(Adr, len, Fram_Data, 0u);
            fram_data_write(ESC_BACKUP_ADR + Adr, len, Fram_Data, 0u);        
        }
        else if( Adr == ESC_PARA_ADR )
        {
            fram_data_write(Adr, len, Fram_Data, PARAMETER_POLYNOMIALS);
            fram_data_write(ESC_BACKUP_ADR + Adr, len, Fram_Data, PARAMETER_POLYNOMIALS);
        }
        else
        {}
    }
    
    return errorflag;
}

/*******************************************************************************
* Function Name  : fram_data_write
* Description    : write the data to the Fram.                  
* Input          : Adr: fram address, WriteData: write buffer, len: the length of data          
* Output         : None
* Return         : None
*******************************************************************************/
void fram_data_write(u16 Adr, u16 len, u8 WriteData[], u32 Polynomials)
{
    u32 i;
        
    if( Polynomials )
    {
        /* CRC32 */
        i = MB_CRC32( WriteData, (len - 4u) , Polynomials);
        WriteData[len - 4u] = (u8)(i >> 24u);
        WriteData[len - 3u] = (u8)(i >> 16u);     
        WriteData[len - 2u] = (u8)(i >> 8u);
        WriteData[len - 1u] = (u8)i;         
    }
    else
    {
        /* CRC16 */
        i = MB_CRC16( WriteData, len - 2u );
        WriteData[len - 2u] = (u8)i;
        WriteData[len - 1u] = (u8)(i >> 8u);
    }
    
    eeprom_write(Adr, len, WriteData);    
}


/*******************************************************************************
* Function Name  : DataIntegrityInFRAMCheck
* Description    : FRAM initialization and test data integrity.
*                  Respectively on two parameters CRC check memory area, such as CRC check is passed,
*                  And then one by one using zone parameters (original code) and backup zone parameters (complement) XOR,
*                  Two storage areas to ensure that the correct parameters, such as parameter error occurs, 
*                  the process proceeds to the error handling routine                   
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void DataIntegrityInFRAMCheck(void)
{
    u8 result = 0u;
    u8 Fram_Data[ESC_RECORD_NUM] = {0};

    result = fram_data_read(ESC_PARA_ADR, ESC_PARA_NUM, Fram_Data);
    if(result)
    {
        g_u32InitTestError = 1u;
    }
}


/******************************  END OF FILE  *********************************/






