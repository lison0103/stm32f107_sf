/*******************************************************************************
* File Name          : usb_virtual_com_port.c
* Author             : lison
* Version            : V1.0
* Date               : 06/07/2016
* Description        : This file contains usb virtual com port funcitons.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "usb_virtual_com_port.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint8_t Send_Buffer[64];
extern __IO  uint32_t packet_sent;
extern __IO  uint32_t packet_receive;


/*******************************************************************************
* Function Name   : Connect_To_COM.
* Descriptioan    : usb init
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
void Connect_To_COM(void)
{
   
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
  
} 

/*******************************************************************************
* Function Name   : USB_Send_Data
* Descriptioan    : usb send data
* Input           : ptrBuffer: data buffer  Send_length:the length of data
*                   (A data transmission can not be greater than 63)
* Output          : None.
* Return          : None.
*******************************************************************************/
void USB_Send_Data(uint8_t ptrBuffer[], uint8_t Send_length)
{
    uint16_t p = 0u;
        
    if (bDeviceState == CONFIGURED)
    {
          if(Send_length > 63u)
          {      
              u8 SendCount = Send_length / 63u;
              Send_length = Send_length%63u;
              
              while(SendCount--)
              {
                  while( 0u == packet_sent )
                  {}
                      
                      CDC_Send_DATA (&ptrBuffer[p],63u);
                      p += 63u;
                  
              }
              while( 0u == packet_sent )
              {}
              CDC_Send_DATA (&ptrBuffer[p],Send_length);
              delay_us(1u);
          }
          else
          {
              while( 0u == packet_sent )
              {}
              CDC_Send_DATA (&ptrBuffer[p],Send_length);
          }
    }
}

/*******************************************************************************
* Function Name   : USB_Receive_Data
* Descriptioan    : usb receive data
* Input           : ptrBuffer: data buffer 
* Output          : None.
* Return          : the length of receive data.
*******************************************************************************/
uint32_t USB_Receive_Data(uint8_t ptrBuffer[])
{
    uint32_t  receive_length = 0u;
    uint32_t i;
    uint32_t res = 0u;
    uint32_t p = 0u;
    
    if (bDeviceState == CONFIGURED)
    {

      while ( 1u == packet_receive )
      {
          for( i = 0u; i < Receive_length; i++ )
          {
              ptrBuffer[p + i] = Receive_Buffer[i];

          }
          receive_length += Receive_length;
          
          p += Receive_length;          
                       
          CDC_Receive_DATA();         
          
          delay_us(100u);
      }
      res = receive_length;
    }
    
    return res;

}


/*******************************************************************************
* Function Name   : USB_Receive_Data_Send
* Descriptioan    : Returns received data sent back
* Input           : None. 
* Output          : None.
* Return          : None.
*******************************************************************************/
void USB_Receive_Data_Send(void)
{
  
  
    if (bDeviceState == CONFIGURED)
    {
      CDC_Receive_DATA();
      /*Check to see if we have data yet */
      while(Receive_length  != 0u)
      {
          CDC_Send_DATA ((unsigned char*)Receive_Buffer,(u8)Receive_length);
          Receive_length = 0u;
          CDC_Receive_DATA();
      }
      
    }

}
/*******************************************************************************
* Function Name   : USB_Disconnect
* Descriptioan    : usb disconnect
* Input           : None. 
* Output          : None.
* Return          : None.
*******************************************************************************/
void USB_Disconnect(void)
{

  Disconnect_USB_Virtual_COM_Port();
  
}

/******************************  END OF FILE  *********************************/


