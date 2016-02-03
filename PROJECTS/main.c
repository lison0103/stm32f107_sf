#include "sys.h"
#include "delay.h"    
#include "led.h"
#include "hw_test.h"
#include "can.h"
#include "spi.h"

#ifdef GEC_SF_MASTER

#include "mb85rcxx.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

#endif

u8 canbuf_send[8]; 


void Bsp_Init(void)
{

        /** set system interrupt priority group 2 **/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        
        /** delay init **/
	delay_init();  
        
        /** LED init **/
	LED_Init();
        
        /** input and relay output test init **/
        Hw_Test_Init();

#ifdef GEC_SF_MASTER
        
        /** MB85RCXX init **/
        eep_init();
        if(MB85RCXX_Check())
        {
              /** MB85RCXX check fail , do some things **/                   
        }  
       	 	
        /** CAN1 init,baud rate 250Kbps **/
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);  
        
        /** CAN2 init,baud rate 250Kbps **/
        /** note : use CAN2 , must CAN1 init **/
//        CAN_Mode_Init(CAN2,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);           

        
        /** USB VCP init **/
//          USBD_Init(&USB_OTG_dev,
//#ifdef USE_USB_OTG_HS 
//            USB_OTG_HS_CORE_ID,
//#else            
//            USB_OTG_FS_CORE_ID,
//#endif  
//            &USR_desc, 
//            &USBD_CDC_cb, 
//            &USR_cb);
          


#else

      
        
#endif

}

void LED_indicator(void)
{
	static u16 led_idr_cnt=0;	 
	
	led_idr_cnt++;
	
	if(led_idr_cnt >= 300)
	{
                led_idr_cnt = 0;
		LED=!LED;                
	}   
}


void Task_Loop(void)
{

#ifdef GEC_SF_MASTER  
      static u32 count1 = 0,count2 = 0;
      u8 canbuf_recv[8];
      u8 res;
      u8 can_rcv;
  
      delay_ms(1);
      count1++;
      count2++;
      
      if(count1==200)
      {                       
          Hw_Test1();
          
          Usb_Vcp_RecvBufandSend();
          
          SF_RL1_WDT=!SF_RL1_WDT;
          count1=0;
      } 
      
      
      if(count2 == 1000)
      {         
        
          count2 = 0;
          
          /** CAN1 send data **/
          res=Can_Send_Msg(CAN1,canbuf_send,4);                          
          if(res)
          {                             
              Usb_Vcp_SendBuf("CAN1TX:fail\r\n", 13);                             
          }
          else 
          {	 
            
//              Usb_Vcp_SendBuf("CAN1TX:", 7);                             
              Usb_Vcp_SendBuf(canbuf_send, 4); 
              
              delay_ms(1);
              
              /** CAN1 receive data **/
              can_rcv=Can_Receive_Msg(CAN1,canbuf_recv);
              if(can_rcv)
              {			                                 
//                  Usb_Vcp_SendBuf("CAN1RX:", 7);                                 
                  Usb_Vcp_SendBuf(canbuf_recv, can_rcv);                                                                                               
              }                              
              
//              Usb_Vcp_SendBuf("\r\n\r\n", 4);                             
            
          }                      
        
      }
#else

      static u32 count1 = 0;
      
      delay_ms(1);
      count1++;
      
      if(count1==200)
      {                       
          Hw_Test1();     
          
          SF_RL2_WDT=!SF_RL2_WDT;
          count1=0;
      }      
      
#endif
}

int main(void)
{        
    
    /** hardware init **/
    Bsp_Init();
    
    while(1)
    {
      
        Task_Loop();
        LED_indicator();
           
    }          
          
}
