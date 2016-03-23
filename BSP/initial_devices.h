/*******************************************************************************
* File Name          : initial_devices.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INITIAL_DEVICES_H
#define __INITIAL_DEVICES_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "delay.h"    
#include "led.h"
#include "hw_test.h"
#include "can.h"
#include "spi.h"
#include "ewdt.h"
#include "exti.h"
#include "timer.h"
#include "safety_test.h"

#ifdef GEC_SF_MASTER
#include "esc_comm_db.h"
#include "esc_record_data.h"
#include "usart.h"
#include "mb85rcxx.h"
#include "usbd_cdc_core_loopback.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Bsp_Init(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);


#ifdef GEC_SF_MASTER
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
extern uint8_t Rxbuffer[64]; 
extern __IO uint32_t receive_count;
extern __IO uint32_t  data_sent;
#endif



#endif  /* __INITIAL_DEVICES_H */


/******************************  END OF FILE  *********************************/





