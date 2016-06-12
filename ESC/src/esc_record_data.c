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
#define ESC_RECORD_ADR 0
#define ESC_BACKUP_ADR 1024
#define ESC_RECORD_NUM 500

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : sys_data_write
* Description    : write the data to the Fram.                  
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void sys_data_write(void)
{
    u16 i;
        
    i = MB_CRC16( Sys_Data, ESC_RECORD_NUM - 2 );
    Sys_Data[ESC_RECORD_NUM - 2] = i;
    Sys_Data[ESC_RECORD_NUM - 1] = i>>8;
    
    eeprom_write(ESC_RECORD_ADR, ESC_RECORD_NUM, Sys_Data);
    
    eeprom_write(ESC_BACKUP_ADR, ESC_RECORD_NUM, Sys_Data); 
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
    u16 i,j = 0;
    u8 result = 0;
    u8 errorflag = 0;
    u8 Sys_Data_Backup[ESC_RECORD_NUM] = {0};
    
    eeprom_read(ESC_RECORD_ADR, ESC_RECORD_NUM, Sys_Data);
    j = Sys_Data[0]<<8 | Sys_Data[0];
    
    if(!MB_CRC16(Sys_Data, ESC_RECORD_NUM))
    {
        delay_ms(10);
        eeprom_read(ESC_BACKUP_ADR, ESC_RECORD_NUM, Sys_Data_Backup);  
        if(!MB_CRC16(Sys_Data_Backup, ESC_RECORD_NUM))
        {
            
            for(i=0; i < ESC_RECORD_NUM; i++)
            {
                result = Sys_Data[i]^Sys_Data_Backup[i];
                if( result )
                {
                    errorflag = 1;
                    break;
                }
                
            }
        } 
        else
        {
            errorflag = 1;
        }
    }  
    else
    {
        errorflag = 1;
    }  
    
    if(errorflag)
    {
        Sys_Data[0] = 0xf1;
        Sys_Data[1] = 0xf1;
        
        for(i=2; i < ESC_RECORD_NUM; i++)
        {
            Sys_Data[i] = 0;
        }  
        sys_data_write();
        
        if( j == 0xf1f1)
        {
            ESC_Fram_Error_Process();
        }

    }
  
}

/******************************  END OF FILE  *********************************/






