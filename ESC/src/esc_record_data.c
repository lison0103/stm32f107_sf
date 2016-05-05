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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define ESC_RECORD_ADR 0
#define ESC_BACKUP_ADR 1024

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


u8 Sys_Data[100] = {0};
u8 Sys_Data_Backup[100] = {0};



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

  
  i = MB_CRC16( Sys_Data, 98 );
  Sys_Data[98] = i;
  Sys_Data[99] = i>>8;
      
  eeprom_write(ESC_RECORD_ADR,100,Sys_Data);
  
  eeprom_write(ESC_BACKUP_ADR,100,Sys_Data); 
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
  u16 i;
  u8 result = 0;
  u8 errorflag = 0;
  
  eeprom_read(ESC_RECORD_ADR,100,Sys_Data);
  if(!MB_CRC16(Sys_Data, 100))
  {
    delay_ms(10);
    eeprom_read(ESC_BACKUP_ADR,100,Sys_Data_Backup);  
    if(!MB_CRC16(Sys_Data_Backup, 100))
    {

        for(i=0;i<100;i++)
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
      for(i=0;i<100;i++)
      {
        Sys_Data[i] = 0;
      }  
      sys_data_write();

      ESC_Fram_Error_Process();
  }
  
}

/******************************  END OF FILE  *********************************/






