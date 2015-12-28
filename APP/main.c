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

 

USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况


void usb_device_test(void)
{
  	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;
  
        //USB Device电源使能
        GPIO_ResetBits(GPIOA,GPIO_Pin_10);

 
 	printf("USB Connecting...\n");//提示正在建立连接 	    
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
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
			LED=1;
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


int main(void)
{        

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  //初始化延时函数
//	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED  
        
        //外部看门狗初始化
//        EWDT_Drv_pin_config();
//        power_on_bsp_check();        
  
        //MB85RCXX初始化
        eep_init();
        if(MB85RCXX_Check())
        {
            printf("MB85RCXX_Check失败\n");
          
        }  
        
        usb_device_test();
}




