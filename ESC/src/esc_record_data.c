/*******************************************************************************
* File Name          : esc_record_data.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
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

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u8 g_u8FaultCodeStore = 0u;


/*******************************************************************************
* Function Name  : Check_Error_Present_Memory
* Description    : esc check error in memory.                 
* Input          : None          
* Output         : None
* Return         : return last power off esc state
*******************************************************************************/
u8 Check_Error_Present_Memory(void)
{
    u8 readbuffer[100];
    u8 result = 0u;
    u8 i;
    u16 temp;
    
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
            }     
        }
    }
    else
    {
        /* epprom read error */
        EN_ERROR11 |= 0x01u;
    }
    
    return ESC_READY_STATE;
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
    u8 buffer[100];
    u8 i;

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
        for( i = 0u; i < 100u; i++ )
        {
            buffer[i] = 0u;
        }
        
        /* Header */
        buffer[0] = 0xfau;
        buffer[1] = 0xedu;
        
        /* 5 Error Code */
        for( i = 0u; i < 5u; i++ )
        {
            buffer[i*2u + 2u] = (u8)(EscRtData.ErrorCode[i] & 0xffu);
            buffer[i*2u + 3u] = (u8)((EscRtData.ErrorCode[i] >> 8u) & 0xffu);
        }
        
        /* Error Buffer */
        for( i = 0u; i < 64u; i++ )
        {
            buffer[i + 12u] = EscRtData.ErrorBuff[i] | OmcEscRtData.ErrorBuff[i];
            /* for test, only cpu1 */
            /*buffer[i + 12u] = EscRtData.ErrorBuff[i];*/
        }                     
        fram_data_write(ESC_BACKUP_ADR + ESC_ERROR_ADR, ESC_ERROR_NUM, buffer);  
        u8DataStore = 0u;
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
            if(!MB_CRC16(Fram_Data, len))
            {
                delay_ms(10u);
                
                if(!eeprom_read(ESC_BACKUP_ADR + Adr, len, Fram_Data_Backup))
                {
                    if(!MB_CRC16(Fram_Data_Backup, len))
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
        fram_data_write(Adr, len, Fram_Data);
        fram_data_write(ESC_BACKUP_ADR + Adr, len, Fram_Data);
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
void fram_data_write(u16 Adr, u16 len, u8 WriteData[])
{
    u16 i;
        
    i = MB_CRC16( WriteData, len - 2u );
    WriteData[len - 2u] = (u8)i;
    WriteData[len - 1u] = (u8)(i >> 8u);
    
    eeprom_write(Adr, len, WriteData);
    
    eeprom_write(ESC_BACKUP_ADR + Adr, len, WriteData); 
}


/*******************************************************************************
* Function Name  : esc_data_check
* Description    : Respectively on two parameters CRC check memory area, such as CRC check is passed,
*                  And then one by one using zone parameters (original code) and backup zone parameters (complement) XOR,
*                  Two storage areas to ensure that the correct parameters, such as parameter error occurs, 
*                  the process proceeds to the error handling routine                   
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void esc_data_check(void)
{
    u16 i,j = 0u;
    u8 result = 0u;
    u8 errorflag = 0u;
    u8 Fram_Data[ESC_RECORD_NUM] = {0};
    u8 Fram_Data_Backup[ESC_RECORD_NUM] = {0};
    
    if(!eeprom_read(ESC_PARA_ADR, ESC_PARA_NUM, Fram_Data))
    {
        j = (u16)Fram_Data[1] << 8;
        j |= (u16)Fram_Data[0];
        
        if(!MB_CRC16(Fram_Data, ESC_PARA_NUM))
        {
            delay_ms(10u);
            
            if(!eeprom_read(ESC_BACKUP_ADR, ESC_PARA_NUM, Fram_Data_Backup))
            {
                if(!MB_CRC16(Fram_Data_Backup, ESC_PARA_NUM))
                {
                    
                    for(i = 0u; i < ESC_PARA_NUM; i++)
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
    
    if(errorflag)
    {
        /* eeprom init first time */
        Fram_Data[0] = 0xf1u;
        Fram_Data[1] = 0xf1u;
        
        for(i = 2u; i < ESC_RECORD_NUM * 2u; i++)
        {
            Fram_Data[i] = 0u;
        }  

        i = MB_CRC16( Fram_Data, ESC_PARA_NUM - 2u );
        Fram_Data[ESC_PARA_NUM - 2u] = (u8)i;
        Fram_Data[ESC_PARA_NUM - 1u] = (u8)(i >> 8u);
        
        eeprom_write(ESC_PARA_ADR, ESC_PARA_NUM, Fram_Data);
        
        eeprom_write(ESC_BACKUP_ADR, ESC_PARA_NUM, Fram_Data); 
        
        if( j == 0xf1f1u )
        {
            ESC_Fram_Error_Process();
            g_u32InitTestError = 1u;
        }

    }
  
}

/******************************  END OF FILE  *********************************/






