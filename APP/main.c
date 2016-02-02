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
  u8 tt,i = 0;
  u32 aa = 0;
  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  //初始化延时函数
	LED_Init();					//初始化LED  


  
        //MB85RCXX初始化
        eep_init();
        if(MB85RCXX_Check())
        {
              delay_ms(1);
                   
        }  

#if 1        
        HW_TEST_INIT();
//        HW_TEST();

        
        AUX1_CTR = 0;
        SF_RL1_CTR = 0;
        SF_RL1_WDT = 1;
#endif
        

#if 1

	u8 a='a',t=0;
	u8 cnt=0;
	u8 canbuf_recv[8];
	u8 res;
        u8 can_rcv;
	u8 mode=CAN_Mode_Normal;//CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式
          
	 	
   
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);//CAN初始化环回模式,波特率250Kbps    
//        CAN_Mode_Init(CAN2,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);//CAN初始化环回模式,波特率250Kbps            

        
          USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);
          
          
//          spi1_test();
          
          while(1)
          {
                    delay_ms(1);
                      tt++;
                      aa++;
                      
                      //VCP把接收到的内容发回给串口
                      if(tt==200)
                      {
//                          for(i = 0; i < 8; i++)
//                          {
//                              APP_Rx_Buffer[APP_Rx_ptr_in] = i + '0';
//                              APP_Rx_ptr_in++;
//                          }      
                        
                          HW_TEST();
                                                 
                          Usb_Vcp_RecvBufandSend();
                        
                          LED=!LED;
                          SF_RL1_WDT=!SF_RL1_WDT;
                          tt=0;
                      } 
                      
                      // CAN 每秒发送一次，并通过VCP发送给串口
                      if(aa == 1000)
                      {
#if 1                     

//                          for(i=0;i<8;i++)
//                          {
//                            canbuf_send[i]= a;//填充发送缓冲区
//                            
////                            printf("%s",canbuf_send[i]);	//显示数据
//                          }
                          
//                          a++;
//                          if(a > 'z')
//                          {
//                              a = 'a';
//                          }
                          res=Can_Send_Msg(CAN1,canbuf_send,4);//发送8个字节 
                          
                          if(res)
                          {
                             
                              Usb_Vcp_SendBuf("CAN1TX:fail\r\n", 13);
                              
//                            printf("Failed");		//提示发送失败
                          }
                          else 
                          {
//                            printf("OK    ");	 		//提示发送成功	 
                            
                                                        
                              Usb_Vcp_SendBuf("CAN1TX:", 7);
                              
                              Usb_Vcp_SendBuf(canbuf_send, 4); 
                              
                              delay_ms(1);
                              
//                              can_rcv=Can_Receive_Msg(CAN2,canbuf_recv);
                              can_rcv=Can_Receive_Msg(CAN1,canbuf_recv);
                              if(can_rcv)//接收到有数据
                              {			                                 
                                  Usb_Vcp_SendBuf("CAN1RX:", 7);
                                  
                                  Usb_Vcp_SendBuf(canbuf_recv, can_rcv);                                                                                               
                              }                              
                              
                              Usb_Vcp_SendBuf("\r\n\r\n", 4);                             
                              
//                                if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE - 50)
//                                {
//                                  APP_Rx_ptr_in = 0;
//                                }
                          }
#endif                      
                      aa = 0;
                      
                      }
          
          }

          
#endif          
          
}



void can1_can2_test(void)
 {	 
	u8 i=0,t=0;
	u8 cnt=0;
	u8 canbuf_send[8],canbuf_recv[8];
	u8 res;
        u8 can_rcv;
	u8 mode=CAN_Mode_Normal;//CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式

	 	
   
	CAN_Mode_Init(CAN1,CAN_SJW_2tq,CAN_BS2_5tq,CAN_BS1_3tq,20,mode);//CAN初始化环回模式,波特率200Kbps    
        CAN_Mode_Init(CAN2,CAN_SJW_2tq,CAN_BS2_5tq,CAN_BS1_3tq,20,mode);//CAN初始化环回模式,波特率200Kbps    

	
 	while(1)
	{
                //CAN1发送
                
                for(i=0;i<8;i++)
                {
                  canbuf_send[i]=cnt+i;//填充发送缓冲区
                  
                  printf("%s",canbuf_send[i]);	//显示数据
                }
                res=Can_Send_Msg(CAN1,canbuf_send,8);//发送8个字节 
                
                if(res)
                  printf("Failed");		//提示发送失败
                else 
                  printf("OK    ");	 		//提示发送成功								   


                //CAN2接收  
		can_rcv=Can_Receive_Msg(CAN2,canbuf_recv);
		if(can_rcv)//接收到有数据
		{			
			
 			for(i=0;i<can_rcv;i++)
			{									    
                              printf("%s",canbuf_recv[i]);	//显示数据
 			}
		}                               
		   
	}

}