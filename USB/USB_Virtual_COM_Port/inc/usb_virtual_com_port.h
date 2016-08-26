/*******************************************************************************
* File Name          : usb_virtual_com_port.h
* Author             : lison
* Version            : V1.0
* Date               : 06/07/2016
* Description        : Contains the prototype of usb virtual com port funcitons.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_VIRTUAL_COM_PORT_H
#define __USB_VIRTUAL_COM_PORT_H 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Connect_To_COM(void);
void USB_Send_Data(uint8_t *ptrBuffer, uint8_t Send_length);
uint32_t USB_Receive_Data(uint8_t ptrBuffer[]);
void USB_Receive_Data_Send(void);
void USB_Disconnect(void);


#endif  /* __USB_VIRTUAL_COM_PORT_H */


/******************************  END OF FILE  *********************************/




