/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    09-November-2015
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_usr.h" 
#include "led.h"
#include "ff.h" 
#include "usart.h" 
#include "fatfs.h" 	   

static u8 AppState;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;

/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
  	USBH_OTG_ISR_Handler(&USB_OTG_Core);
} 


/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb=
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};



/**
* @brief  USBH_USR_Init 
*         Host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
	printf("USB OTG HS MSC Host\r\n");
	printf(" USB Host library started.\r\n");
	printf("  USB Host Library v2.2.0\r\n\r\n");
	
}


/**
* @brief  USBH_USR_DeviceAttached 
*         Device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
//	LED1=0;
	printf("USB Device attached!\r\n");
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Status
*/
void USBH_USR_DeviceDisconnected (void)
{
//	LED1=1;
	printf("USB Device disconnect!\r\n");
}  


/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
	printf("Reset device...\r\n");
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("High speed device detected!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("Full speed device detected!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("Low speed device detected!\r\n");  
	}
	else
	{
		printf("Device fault!\r\n");  
	}
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs=DeviceDesc;   
	printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
	printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
	printf("USB device is successfully assigned the Address!\r\n");   
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		printf("Mass storage device connected!\r\n"); 
	}else if((*id).bInterfaceClass==0x03)
	{
		printf("HID device connected!\r\n"); 
	}    
}


/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}


/**
* @brief  USBH_USR_Product_String 
*         Displays the message for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
	printf("Product: %s\r\n",(char *)ProductString);  
}


/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	printf("Serial Number: %s\r\n",(char *)SerialNumString);    
} 


/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{ 
	printf("Enumeration completed!\r\n\r\n");    
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
	printf("No registered class for this device!\r\n\r\n");    
}  

/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{ 

	return USBH_USR_RESP_OK;
} 


/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Status
*/
void USBH_USR_OverCurrentDetected (void)
{
	printf("Overcurrent detected!!!\r\n");
} 

extern u8 USB_LoadParameter(void);	


/**
* @brief  USBH_USR_MSC_Application 
*         Application for mass storage
* @param  None
* @retval Status
*/
int USBH_USR_MSC_Application(void)
{
	u8 res=0;
  	switch(AppState)
  	{
    	case USH_USR_FS_INIT:
			printf("Start user program execution!!!\r\n");
			AppState=USH_USR_FS_TEST;
                        
                        /* mount usb stick */ 
                        f_mount(fs[0],"0:",1); 
      		break;
    	case USH_USR_FS_TEST:	
			res = USB_LoadParameter(); 
     		res=0;
			if(res)AppState=USH_USR_FS_INIT;
      		break;
    	default:break;
  	} 
	return res;
}


/**
* @brief  USBH_USR_DeInit
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  	AppState=USH_USR_FS_INIT;
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
	printf("UNRECOVERED ERROR STATE!!!\r\n\r\n");	
}



/* User-defined functions, interface functions to achieve fatfs diskio */
extern USBH_HOST              USB_Host;


/**
* @brief  USBH_UDISK_Status
*         Get the U-disk state
* @param  None
* @retval Return value: 0, U disk not ready  1, ready
*/
u8 USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core);
}

	
/**
* @brief  USBH_UDISK_Read
*         Read U disk
* @param  Buf: read data buffer
*         Sector: sector address
*         Cnt: Number of sectors
* @retval Return Value: Error status; 0, normal; the other, an error code;
*/
u8 USBH_UDISK_Read(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)
	{  		    
		do
		{
			res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
                                /* read write error */
				res=1;
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}

	
/**
* @brief  USBH_UDISK_Write
*         Write U disk
* @param  Buf: write data buffer
*         Sector: sector address
*         Cnt: Number of sectors
* @retval Return Value: Error status; 0, normal; the other, an error code;
*/
u8 USBH_UDISK_Write(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)
	{  		    
		do
		{
			res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt); 
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
                                /* read write error */
				res=1;
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}



/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/








































