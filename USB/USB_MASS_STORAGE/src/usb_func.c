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
#include "exfuns.h" 
#include "delay.h"
#include "ewdt.h"
#include "fatfs.h"
#include "usb_func.h"
#include "led.h"
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
        
      u32 total,free;
      u8 res = 0;
      printf("Device connect success!.\n");	 
      
      
      
      
      res=exf_getfree("0:",&total,&free);
      if(res==0)
      {	   
            total = total>>10;
            free  = free>>10;
            printf("FATFS OK!\n");	
            printf("U Disk Total Size:  %d   MB\n",total);
            printf("U Disk  Free Size:  %d   MB\n",free); 	    	
      } 
      
      /* usb load parameter start ---------------------------*/
      if(isFileExist("0:123.txt"))
      {
            printf("File not exists\n");
      }
      else
      {
            printf("File exists\n");
            
            DeleteFile("0:abc.txt");
            
            
            
            CopyFile("0:123.txt", "0:abc.txt");
       
      }
      
      /* usb load parameter finish ---------------------------*/ 
      
      
      while(HCD_IsDeviceConnected(&USB_OTG_Core))
      {	
            
            delay_ms(200);
            
            EWDT_TOOGLE();
            IWDG_ReloadCounter();
             
      }
      
      printf("Device is connecting...\n");
      
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
      if(exfuns_init())			
      {
            printf("fatfs memory apply fail \n");           
      }
      
      /** USB HOST init **/
      USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb);    
      printf("wait usb mass storage connected...\n");
      
           
      while(1)
      {
            
            USBH_Process(&USB_OTG_Core, &USB_Host);
            delay_ms(1);
            t++;
            timecounter++;
            
            /* wait for the usb connect 3s */
            if( timecounter == 3000 )
            {                             
                  while(1)
                  {
                        NVIC_SystemReset();
                  }
            }
            
            if( t == 200 )
            {      
                  LED =! LED;
                  t = 0;
                  
                  EWDT_TOOGLE();
                  IWDG_ReloadCounter();  
            }
      } 
         
}







/******************************  END OF FILE  *********************************/


