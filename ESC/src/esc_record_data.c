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
/* Private macro -------------------------------------------------------------*/
#define ESC_RECORD_ADR 0u
#define ESC_BACKUP_ADR 1024u
#define ESC_RECORD_NUM 1020u

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


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
    u8 Fram_Data[ESC_RECORD_NUM] = {0};
    u8 Fram_Data_Backup[ESC_RECORD_NUM] = {0};
    u8 errorflag = 0u,result = 0u;
        
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
    
    if( errorflag == 0u )
    {
        for( i = 0u; i < len; i++ )
        {
            ReadData[i] = Fram_Data[i];
        }
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
    
    if(!eeprom_read(ESC_RECORD_ADR, ESC_RECORD_NUM, Fram_Data))
    {
        j = Fram_Data[1] << 8 | Fram_Data[0];
        
        if(!MB_CRC16(Fram_Data, ESC_RECORD_NUM))
        {
            delay_ms(10u);
            
            if(!eeprom_read(ESC_BACKUP_ADR, ESC_RECORD_NUM, Fram_Data_Backup))
            {
                if(!MB_CRC16(Fram_Data_Backup, ESC_RECORD_NUM))
                {
                    
                    for(i = 0u; i < ESC_RECORD_NUM; i++)
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
        
        for(i = 2u; i < ESC_RECORD_NUM; i++)
        {
            Fram_Data[i] = 0u;
        }  

        i = MB_CRC16( Fram_Data, ESC_RECORD_NUM - 2u );
        Fram_Data[ESC_RECORD_NUM - 2u] = (u8)i;
        Fram_Data[ESC_RECORD_NUM - 1u] = (u8)(i >> 8u);
        
        eeprom_write(ESC_RECORD_ADR, ESC_RECORD_NUM, Fram_Data);
        
        eeprom_write(ESC_BACKUP_ADR, ESC_RECORD_NUM, Fram_Data); 
        
        if( j == 0xf1f1u )
        {
            ESC_Fram_Error_Process();
        }

    }
  
}

/******************************  END OF FILE  *********************************/






