#include "esc_error_process.h"
#include "hw_test.h"
#include "sys.h"

int EscBuff[10] = {0};

/** 在故障处理子程序中死循环，断开安全继电器
扶梯停止运行，进入故障状态，等待故障人工复位。**/
void ESC_Error_Process(void)
{
  
    /** 断开安全继电器 **/
    SF_RL1_CTR = 0;
    SF_RL1_WDT = 0;
    
    while(1)
    {
        /** 在故障处理子程序中死循环 **/
          /**  等待故障人工复位 **/
          if( IN10 )
          {
                __set_FAULTMASK(1);   
                /** 软复位 **/
                NVIC_SystemReset();
          
          }
    }

}