#include "sys.h"
#include "delay.h"    
#include "led.h"
#include "mb85rcxx.h"
#include "hw_test.h"
 
 
int main(void)
{        
  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  //初始化延时函数
	LED_Init();					//初始化LED  


  
        //MB85RCXX初始化
        eep_init();
        if(MB85RCXX_Check())
        {
              ;
                   
        }  
        
        HW_TEST_INIT();
        HW_TEST();

}




