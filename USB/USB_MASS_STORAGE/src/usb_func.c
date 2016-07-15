/*******************************************************************************
* File Name          : usb_func.c
* Author             : lison
* Version            : V1.0
* Date               : 03/24/2016
* Description        : This file contains usb mass storage funcitons.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "usbh_usr.h" 
#include "malloc.h" 
#include "ff.h"  
#include "delay.h"
#include "ewdt.h"
#include "fatfs.h"
#include "usb_func.h"
#include "led.h"
#include "esc.h"
#include "crc16.h"
#include "can.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

u8 ParaLoadFinsh = 0;

/*******************************************************************************
* Function Name  : USB_LoadParameter
* Description    : Usb main program, load parameter.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : 0:ok  1:error
*******************************************************************************/ 
u8 USB_LoadParameter(void)
{ 

      u8 res = 0;

      LED = 1;
      ParaLoadFinsh |= 0x01;
      
      /* USB-stick detected */
      /* Message to Control */
      /* Message to CPU2 */
      
      
      /* usb load parameter start ---------------------------*/

      /* 1. S0001 file present */
      if(!isFileExist("0:123.txt"))
      {
          
          /* 2. Read parameters from usb stick to buffer */
//          ReadFile( "0:S0001.bin", parabuffer );
          
          /* 3. decrypt */
          
          /* 4. Check crc16 is it ok */
//          if( !MB_CRC16( parabuffer, ESC_PARA_NUMBER ))
//          {
//              
//          }
          
          /* 5. Store the parameters in the fram */
//          eeprom_write(ESC_RECORD_ADR, ESC_PARA_NUMBER, parabuffer);
          
          /* just for test */
          DeleteFile("0:abc.txt");       
          CopyFile("0:123.txt", "0:abc.txt");
          
          
          ParaLoadFinsh |= 0x02;
          
      }

      /* 1. C0001 file present */
      if(!isFileExist("0:123.txt"))
      {
          
          /* 2. Read parameters from usb stick to buffer */
//          ReadFile( "0:C0001.bin", parabuffer );
          
          /* 3. decrypt */
          
          /* 4. Check crc16 is it ok */
//          if( !MB_CRC16( parabuffer, ESC_PARA_NUMBER ))
//          {
//              
//          }
          
          /* 5. Send the parameters to the cb board */
//          BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, parabuffer, 100);
                    
          ParaLoadFinsh |= 0x04;
          
      }      
      /* usb load parameter finish ---------------------------*/ 
      
      
      while( HCD_IsDeviceConnected( &USB_OTG_Core ))
      {	
            if( ParaLoadFinsh & 0x06 ) 
            {
                LED=!LED;                
            }
            else
            {
                LED = 0;
            }
            
            delay_ms(200);
            EWDT_TOOGLE();
            IWDG_ReloadCounter();
             
      }
      
      
      return res;
}





/*******************************************************************************
* Function Name  : USBH_Mass_Storage_Init
* Description    : Usb host mass storage initialzation.
*                  
* Input          : None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void USBH_Mass_Storage_Init(void)
{
        
      u16 t = 0;
      u32 timecounter = 0;
      
      /** mem init **/	
      mmem_init(); 
      
      /** fatfs apply memory **/ 
      if(fatfs_init())			
      {
            printf("fatfs memory apply fail \n");           
      }
      
      /** USB HOST init **/
      USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb);       
           
      while(1)
      {
            
            USBH_Process(&USB_OTG_Core, &USB_Host);
            delay_ms(1);
            t++;
            timecounter++;
            
            if( ParaLoadFinsh & 0x01 )
            {
                while(1)
                {
                    NVIC_SystemReset();
                }                
            }
            else if( timecounter == 1000 )
            {                             
                USBH_DeInit(&USB_OTG_Core, &USB_Host);
                myfree(fs[0]);
                break;
            }
            
            if( t == 200 )
            {      
                  LED=!LED;
                  t = 0;
                  
                  EWDT_TOOGLE();
                  IWDG_ReloadCounter();  
            }
      } 
         
}







/******************************  END OF FILE  *********************************/


