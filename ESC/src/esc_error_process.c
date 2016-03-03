#include "esc_error_process.h"
#include "hw_test.h"
#include "sys.h"

int EscBuff[10] = {0};

/** 在故障处理子程序中死循环，断开安全继电器
扶梯停止运行，进入故障状态，等待故障人工复位。**/
void ESC_EWDT_Error_Process(void)
{
#ifdef GEC_SF_MASTER  
    /** 断开安全继电器 **/
    SF_RL1_CTR = 0;
    SF_RL1_WDT = 0;
#endif    
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

/** 在故障处理子程序中死循环，等待看门狗复位
断开安全继电器，扶梯停止运行，进入故障状态，等待故障复位 **/
void ESC_Flash_Error_Process(void)
{
#ifdef GEC_SF_MASTER  
    /** 断开安全继电器 **/
    SF_RL1_CTR = 0;
    SF_RL1_WDT = 0;    
#endif
}


/** 故障处理子程序，断开安全继电器，
扶梯停止运行，进入故障状态, 等待人工修正参数 **/
void ESC_Fram_Error_Process(void)
{
#ifdef GEC_SF_MASTER  
    /** 断开安全继电器 **/
    SF_RL1_CTR = 0;
    SF_RL1_WDT = 0;
#endif    

}