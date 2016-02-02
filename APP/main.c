#include "sys.h"
#include "delay.h"    
#include "led.h"
#include "mb85rcxx.h"
#include "hw_test.h"
#include "can.h"
#include "spi.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"


void can1_can2_test(void);


__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

extern u8 sflag,inputnum;

u8 Master_Temp =0;

void spi1_test(void)
{
    
    u8 t;
    
    SPI1_Init();
//    SPI1_NVIC();
//    SPI1_SetSpeed(SPI_BaudRatePrescaler_256);

   while(1)
   { 
     
//     HW_TEST();
#if 0
       SPI1_ReadWriteByte(0x55); 
       Master_Temp = SPI1_ReadWriteByte(0x00);
#else
       SPI1_WriteByte(0x66); 
//       delay_ms(1);
//       Master_Temp = SPI1_ReadByte(0x00);
#endif
       //       printf("Master_Temp =%x\r\n",Master_Temp);
       

       delay_ms(10); 
       
       t++;
       if(t == 50)
       {
             t = 0;
             LED =! LED;
             
            Usb_Vcp_SendBuf(&Master_Temp, 1);                  
         
       }
   }

}





u8 canbuf_send[8]; 

int main(void)
{        
    u8 tt = 0;
    u32 aa = 0;
       
    u8 canbuf_recv[8];
    u8 res;
    u8 can_rcv;
    /** CAN mode : CAN_Mode_Normal£¬CAN_Mode_LoopBack **/
    u8 mode=CAN_Mode_Normal;


        /** set system interrupt priority group 2 **/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        
        /** delay init **/
	delay_init();  
        
        /** LED init **/
	LED_Init();				 
  
        /** MB85RCXX init **/
        eep_init();
        if(MB85RCXX_Check())
        {
              /** MB85RCXX check fail , do some things **/                   
        }  
       
        /** input and relay output test init **/
        Hw_Test_Init();

#if 1       	 	
        /** CAN1 init,baud rate 250Kbps **/
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);  
        
        /** CAN2 init,baud rate 250Kbps **/
        /** note : use CAN2 , must CAN1 init **/
//        CAN_Mode_Init(CAN2,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);           

        
        /** USB VCP init **/
          USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);
          
          /** SPI1 init **/
//          spi1_test();

          
          while(1)
          {
            
                      delay_ms(1);
                      tt++;
                      aa++;
                      
                      if(tt==200)
                      {                       
                          Hw_Test1();
                                                 
                          Usb_Vcp_RecvBufandSend();
                        
                          LED=!LED;
                          SF_RL1_WDT=!SF_RL1_WDT;
                          tt=0;
                      } 
                      
                      
                      if(aa == 1000)
                      {         
                        
                          aa = 0;

                          /** CAN1 send data **/
                          res=Can_Send_Msg(CAN1,canbuf_send,4);                          
                          if(res)
                          {
                             
                              Usb_Vcp_SendBuf("CAN1TX:fail\r\n", 13);
                              
                          }
                          else 
                          {	 
                                                                                   
                              Usb_Vcp_SendBuf("CAN1TX:", 7);                             
                              Usb_Vcp_SendBuf(canbuf_send, 4); 
                              
                              delay_ms(1);
                              
                              /** CAN1 receive data **/
                              can_rcv=Can_Receive_Msg(CAN1,canbuf_recv);
                              if(can_rcv)
                              {			                                 
                                  Usb_Vcp_SendBuf("CAN1RX:", 7);                                 
                                  Usb_Vcp_SendBuf(canbuf_recv, can_rcv);                                                                                               
                              }                              
                              
                              Usb_Vcp_SendBuf("\r\n\r\n", 4);                             
                              
                          }                      
                                            
                      }
          
          }

          
#endif          
          
}



void can1_can2_test(void)
{	
  
	u8 i=0;
	u8 canbuf_send[8],canbuf_recv[8];
	u8 res;
        u8 can_rcv;
	 	
   
        /** CAN1 init,baud rate 250Kbps **/
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);  
        
        /** CAN2 init,baud rate 250Kbps **/
        /** note : use CAN2 , must CAN1 init **/
        CAN_Mode_Init(CAN2,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);    

	
 	while(1)
	{
          
                /** CAN1 send **/
                
                for(i=0;i<8;i++)
                {
                  canbuf_send[i]=i;                
                }
                res=Can_Send_Msg(CAN1,canbuf_send,8);
                
                if(res)
                  Usb_Vcp_SendBuf("Failed", 6);		
                else 
                  Usb_Vcp_SendBuf("OK", 2);	 										   


                //CAN2 receive  
		can_rcv=Can_Receive_Msg(CAN2,canbuf_recv);
		if(can_rcv)
		{			
			
 			for(i=0;i<can_rcv;i++)
			{									    
                              Usb_Vcp_SendBuf(canbuf_recv, can_rcv);	
 			}
		}                               
		   
	}

}