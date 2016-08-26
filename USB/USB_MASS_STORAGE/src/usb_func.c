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
#include "esc_comm_safety_dualcpu.h"
#include "mb85rcxx.h"
#include "esc_parameter_process.h"
#include "esc_error_process.h"
#include "esc_record_data.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int USB_LoadParameter(void);

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

u8 ParaLoad = 0u;


/*******************************************************************************
* Function Name  : USB_LoadParameter
* Description    : Usb main program, load parameter.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : 0:ok  1:error
*******************************************************************************/ 
int USB_LoadParameter(void)
{ 

      int res = 0;
      u8 parabuffer[100];
      u16 filelen = 0u;

      LED_ON();
      ParaLoad |= USB_DETECTED;
      
      /* USB-stick detected */     
      /* 1. Message to CPU2 */
      Send_State_Message( MESSAGE_TO_CPU, USB_DETECTED, NULL, 0u );
      
      /* 2. Message to Control */
      Send_State_Message( MESSAGE_TO_CONTROL, USB_DETECTED, NULL, 0u );
      
      /* usb load parameter start -------------------------------------------*/
      /* 1. S0001 file present */
      if(!isFileExist("0:S0001.bin"))
      {
          
          Send_State_Message( MESSAGE_TO_CONTROL, SAFETY_PARAMETER_EXIST, NULL, 0u );
          
          /* 2. Read parameters from usb stick to buffer */
          filelen = ReadFile( "0:S0001.bin", parabuffer );
          
          /* 3. decrypt */
          
          /* 4. Check crc16 is it ok */
          if( MB_CRC16( parabuffer, filelen ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR9 |= 0x01u;
              
              Send_State_Message( MESSAGE_TO_CONTROL, PARAMETER_ERROR, NULL, 0u );
              
              ESC_Init_Fault();
          }
          else
          {
              /* 5. Store the parameters in the fram */
              fram_data_write(ESC_PARA_ADR, ESC_PARA_NUM, parabuffer);
              
              ParaLoad |= SAFETY_PARAMETER_LOADED;
          }
          
      }
      else
      {
          Send_State_Message( MESSAGE_TO_CONTROL, SAFETY_PARAMETER_NOT_EXIST, NULL, 0u );
      }

      /* 1. C0001 file present */
      if(!isFileExist("0:C0001.bin"))
      {
          
          Send_State_Message( MESSAGE_TO_CONTROL, CONTROL_PARAMETER_EXIST, NULL, 0u );
          
          /* 2. Read parameters from usb stick to buffer */
          filelen = ReadFile( "0:C0001.bin", parabuffer );
          
          /* 3. decrypt */
          
          /* 4. Check crc16 is it ok */
          if( MB_CRC16( parabuffer, filelen ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR9 |= 0x01u;   
              
              Send_State_Message( MESSAGE_TO_CONTROL, PARAMETER_ERROR, NULL, 0u );
              
              ESC_Init_Fault();
          }
          else
          {         
              /* 5. Send the parameters to the cb board */
              Send_State_Message( MESSAGE_TO_CONTROL, SEND_PARAMETER, parabuffer, (u8)filelen );
              
              ParaLoad |= CONTROL_PARAMETER_LOADED;
          }
                      
      }   
      else
      {
          Send_State_Message( MESSAGE_TO_CONTROL, CONTROL_PARAMETER_NOT_EXIST, NULL, 0u );
      }
      /* usb load parameter finish -------------------------------------------*/ 
      
      
      while( HCD_IsDeviceConnected( &USB_OTG_Core ))
      {	
          
            if( ParaLoad & ( SAFETY_PARAMETER_LOADED | CONTROL_PARAMETER_LOADED ) ) 
            {
                LED_FLASH();                
            }
            else
            {
                LED_OFF();
            }
            
            delay_ms(200u);
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
      u8 wait_for_restart = 0u;  
      u16 t = 0u;
      u32 timecounter = 0u;
      
      /** mem init **/	
      mmem_init(); 
      
      /** fatfs apply memory **/ 
      if(fatfs_init())			
      {
            /* fatfs memory apply fail */
      }
      
      /** USB HOST init **/
      USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb);       
           
      while(1)
      {
          
          if( ParaLoad & USB_DETECTED )
          {
              if( wait_for_restart == 0u )
              {
                  USBH_DeInit(&USB_OTG_Core, &USB_Host);
                  myfree(fs[0]);   
                  wait_for_restart = 1u;
              }
          }
          else if( timecounter == 1000u )
          {                             
              USBH_DeInit(&USB_OTG_Core, &USB_Host);
              myfree(fs[0]);
              break;
          }
          else
          {
              USBH_Process(&USB_OTG_Core, &USB_Host);
              timecounter++;
          }
          
          delay_ms(1u);
          t++;
          
          if( t == 200u )
          {                       
              t = 0u;
              LED_FLASH();
              
              EWDT_TOOGLE();
              IWDG_ReloadCounter();                  
          }
      } 
         
}







/******************************  END OF FILE  *********************************/


