#include "sys.h"
#include "delay.h"   
#include "usart.h"
#include "led.h"
#include "malloc.h" 
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 
#include "mb85rcxx.h"
#include "ewdt.h"
#include "spi.h"
#include "can.h"
 

USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况


void usb_device_test(void)
{
  	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;
 	        
 
 	printf("USB Connecting...\n");//提示正在建立连接 	    
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
        

//        delay_ms(200);
// 	GPIO_SetBits(GPIOA,GPIO_Pin_10);  	//USB先断开
//        
//	delay_ms(300);
//   	GPIO_ResetBits(GPIOA,GPIO_Pin_10);	//USB再次连接

        
	delay_ms(1800);	
	while(1)
	{	
		delay_ms(1);	
                
                
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   		  	   
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				LED=0;
				printf("USB Writing...\n");//提示USB正在写入数据	 
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				LED=0;
				printf("USB Reading...\n");//提示USB正在读出数据  		 
			}	 										  
			if(USB_STATUS_REG&0x04)
                          printf("USB Write Err \n");//提示写入错误
	  
			if(USB_STATUS_REG&0x08)
                          printf("USB Read  Err \n");//提示读出错误
  
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)
                          printf("USB Connected    \n");//提示USB连接已经建立
			else 
                          printf("USB DisConnected \n");//提示USB被拔出了
                        
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
//			LED=1;
			LED=!LED;//提示系统在运行
                        
//                        EWDT_TOOGLE();
                        
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=1;
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		}
	};  
  

}

void spi1_test(void)
{
    u8 Master_Temp =0;
    
    SPI1_Init();
    SPI1_SetSpeed(SPI_BaudRatePrescaler_256);

   while(1)
   { 
#if 0
       SPI1_ReadWriteByte(0x55); 
       Master_Temp = SPI1_ReadWriteByte(0x00);
#else
       SPI1_WriteByte(0x55); 
       Master_Temp = SPI1_ReadByte(0x00);
#endif
       printf("Master_Temp =%x\r\n",Master_Temp);
       delay_ms(100); 
   }

}


void can_test(void)
 {	 
	u8 i=0,t=0;
	u8 cnt=0;
	u8 canbuf[8];
	u8 res;
        u8 can_rcv;
	u8 mode=CAN_Mode_LoopBack;//CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式

	 	
   
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);//CAN初始化环回模式,波特率500Kbps    

	
 	while(1)
	{

		if(mode==CAN_Mode_LoopBack)//KEY0按下,发送一次数据
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;//填充发送缓冲区
				if(i<4)printf("%s",canbuf[i]);	//显示数据
				else printf("%s",canbuf[i]);	//显示数据
 			}
			res=Can_Send_Msg(CAN1,canbuf,8);//发送8个字节 
			if(res)printf("Failed");		//提示发送失败
			else printf("OK    ");	 		//提示发送成功								   
		}else if(mode==CAN_Mode_Normal)//WK_UP按下，改变CAN的工作模式
		{	   
//			mode=!mode;
  			CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//CAN普通模式初始化, 波特率500Kbps 

			if(mode==0)//普通模式，需要2个开发板
			{
				printf("Nnormal Mode ");	    
			}else //回环模式,一个开发板就可以测试了.
			{
 				printf("LoopBack Mode");
			}

		}		 
		can_rcv=Can_Receive_Msg(CAN1,canbuf);
		if(can_rcv)//接收到有数据
		{			
			
 			for(i=0;i<can_rcv;i++)
			{									    
				if(i<4)printf("%s",canbuf[i]);	//显示数据
				else printf("%s",canbuf[i]);	//显示数据
 			}
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED=!LED;//提示系统正在运行	
			t=0;
			cnt++;
			printf("%d",cnt);	//显示数据
		}		   
	}
}

int main(void)
{        
        //设置系统中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        
        //初始化延时函数
	delay_init();  
        
        //初始化串口波特率为115200
	uart_init(115200);		
        
        //初始化LED  
	LED_Init();					
        
        //外部看门狗初始化
//        EWDT_Drv_pin_config();
//        power_on_bsp_check();        
  
        //MB85RCXX初始化
//        eep_init();
//        if(MB85RCXX_Check())
//        {
//            printf("MB85RCXX_Check失败\n");
//          
//        }  
        
#if 0
        //SPI双机通信测试
        spi1_test();
#else
  #if 1
        //USB Device测试
        usb_device_test();
  #else
        //cat test
        can_test();
  #endif
#endif
        
}




