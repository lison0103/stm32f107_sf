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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;



/*******************************************************************************
* Function Name  : USH_User_App
* Description    : User test main program
*                  
* Input          : None
*                  None
* Output         : None
* Return         : 0:ok  1:error
*******************************************************************************/ 
u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	printf("Device connect success!.\n");	 
        
//        LED0 = 1;

        
	res=exf_getfree("0:",&total,&free);
	if(res==0)
	{	   
                total = total>>10;
                free  = free>>10;
		printf("FATFS OK!\n");	
		printf("U Disk Total Size:  %d   MB\n",total);
		printf("U Disk  Free Size:  %d   MB\n",free); 	    	
	} 

       
        if(isFileExist("0:123.txt"))
        {
            printf("File not exists\n");
        }
        else
        {
              printf("File exists\n");
              
              DeleteFile("0:abc.txt");
              
//              if(isFileExist("0:abc.txt"))
//              {
              
                  CopyFile("0:123.txt", "0:abc.txt");
//              }
              
//              DeleteFile("0:123.txt");
              

        }
       
        
	while(HCD_IsDeviceConnected(&USB_OTG_Core))
	{	
//		LED1=!LED1;
		delay_ms(200);
                
                EWDT_TOOGLE();
                res=exf_getfree("0:",&total,&free);
                if(res==0)
                {	   
                  printf("FATFS OK!\n");	
                  printf("U Disk Total Size:  %d   MB\n",total>>10); 
                  printf("U Disk  Free Size:  %d   MB\n",free>>10); 	    	
                }
	}
   
	printf("Device is connecting...\n");

	return res;
}





/*******************************************************************************
* Function Name  : usb_init
* Description    : None
*                  
* Input          : None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void usb_init(void)
{
  
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
        
        
//	for( ; ; )
//	{
//                USBH_Process(&USB_OTG_Core, &USB_Host);
//	} 
         
}







/******************************  END OF FILE  *********************************/


