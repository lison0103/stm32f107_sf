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
void USB_Send_Data(uint8_t *ptrBuffer, uint8_t Send_length)
{
    if (bDeviceState == CONFIGURED)
    {
          if(Send_length > 63)
          {      
              s8 SendCount = Send_length / 63;
              Send_length = Send_length%63;
//              Delay_us(10000);
              while(SendCount--)
              {
                  while( 0 == packet_sent );
                      
                      CDC_Send_DATA (ptrBuffer,63);
                      ptrBuffer += 63;
                  
//                  Delay_us(1);
              }
              while( 0 == packet_sent );
              CDC_Send_DATA (ptrBuffer,Send_length);
              delay_us(1);
//            CDC_Send_DATA (ptrBuffer,63);
          }
          else
          {
              while( 0 == packet_sent );
              CDC_Send_DATA (ptrBuffer,Send_length);
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
uint32_t USB_Receive_Data(uint8_t *ptrBuffer)
{
    uint32_t  receive_length = 0;
    uint32_t i;
    
    if (bDeviceState == CONFIGURED)
    {
//      CDC_Receive_DATA();

//      while (Receive_length  != 0)
      while ( 1 == packet_receive )
      {
          for( i = 0; i < Receive_length; i++ )
          {
              ptrBuffer[i] = Receive_Buffer[i];

          }
          receive_length += Receive_length;
          
          ptrBuffer += Receive_length;          
//          Receive_length = 0;
                       
          CDC_Receive_DATA();         
          
          delay_us(100);
      }
      return receive_length;
    }
    
    return 0;

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
      CDC_Receive_DATA();//usb_endp.c   EP3_OUT_Callback
      /*Check to see if we have data yet */
//      while(!packet_receive);
      while(Receive_length  != 0)
      {
          CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
          Receive_length = 0;
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


