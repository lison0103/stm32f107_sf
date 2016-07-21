/*******************************************************************************
* File Name          : initial_devices.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains prototype of the devices initinal.
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
#include "usart.h"


#ifdef GEC_SF_MASTER
//#include "esc_comm_db.h"
#include "esc_record_data.h"
#include "mb85rcxx.h"
#include "usb_func.h"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define SELF_TEST       ( 0 )
#define SYSTEMTICK      ( 5 )

/* Exported functions ------------------------------------------------------- */
void Initial_Device(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);






#endif  /* __INITIAL_DEVICES_H */


/******************************  END OF FILE  *********************************/





