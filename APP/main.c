#include "sys.h"
#include "delay.h"    
#include "led.h"
#include "mb85rcxx.h"
#include "hw_test.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "usbd_cdc_core.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
 
int main(void)
{        
  u8 tt;
  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  //初始化延时函数
	LED_Init();					//初始化LED  


  
        //MB85RCXX初始化
        eep_init();
        if(MB85RCXX_Check())
        {
              ;
                   
        }  

#if 0        
        HW_TEST_INIT();
        HW_TEST();
#endif


#if 1
        

        
          USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);
          
          
          while(1)
          {
                    delay_ms(1);
                      tt++;
                      if(tt==200)
                      {
                          LED=!LED;
                          tt=0;
                      } 
          
          }

          
#endif          
          
}




