/*******************************************************************************
* File Name          : usb_func.c
* Author             : lison
* Version            : V1.0
* Date               : 03/24/2016
* Description        : This file contains usb mass storage funcitons.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "usbh_usr.h"  
#include "delay.h"
#include "ewdt.h"
#include "fatfs.h"
#include "usb_func.h"
#include "led.h"
#include "esc_parameter_process.h"
#include "esc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;


/*******************************************************************************
* Function Name  : USB_LoadParameterProcess
* Description    : Usb main program, load parameter process.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/ 
void USB_LoadParameterProcess(void)
{ 
#if 0    
      /* For test */
    EscParameterInit();
    if(isFileExist("0:S0001.bin"))
    {
        /* File not exists */
        WriteNewFile("0:S0001.bin", (u8*)&SFParameterData,478u);
    }
    else
    {
        /* File exists */
        DeleteFile("0:S0001.bin");
        WriteNewFile("0:S0001.bin", (u8*)&SFParameterData,478u);
        
    }
    if(isFileExist("0:C0001.bin"))
    {
        /* File not exists */
        WriteNewFile("0:C0001.bin", (u8*)&CBParameterData,1434u);
    }
    else
    {
        /* File exists */
        DeleteFile("0:C0001.bin");
        WriteNewFile("0:C0001.bin", (u8*)&CBParameterData,1434u);
        
    }
#endif    
      USB_LoadParameter();
      
      
      /* For test */
      if(isFileExist("0:123.txt"))
      {
          /* File not exists */
      }
      else
      {
          /* File exists */
          
          DeleteFile("0:abc.txt");
          
          CopyFile("0:123.txt", "0:abc.txt");
          
      }
      
      /* wait for device unpluged */
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
      u16 t = 0u;
      u32 timecounter = 0u;
      
      
      /** fatfs apply memory **/ 
      fatfs_init();			
      
      /** USB HOST init **/
      USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb);       
           
      while(1)
      {         
          if(( ParaLoad & USB_DETECTED ) || ( timecounter == 1000u ))
          {
              USBH_DeInit(&USB_OTG_Core, &USB_Host);  
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


