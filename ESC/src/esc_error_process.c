/*******************************************************************************
* File Name          : esc_error_process.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_error_process.h"
#include "hw_test.h"
#include "sys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


int EscBuff[10] = {0};



/*******************************************************************************
* Function Name  : ESC_EWDT_Error_Process
* Description    : 
*                  
* Input          : 
*                 
* Output         : None
* Return         : None
*******************************************************************************/
/** 在故障处理子程序中死循环，断开安全继电器
扶梯停止运行，进入故障状态，等待故障人工复位。**/
void ESC_EWDT_Error_Process(void)
{
 
    /** 断开安全继电器 **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;
   
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
 
    /** 断开安全继电器 **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;  

}


/** 故障处理子程序，断开安全继电器，
扶梯停止运行，进入故障状态, 等待人工修正参数 **/
void ESC_Fram_Error_Process(void)
{
#ifdef GEC_SF_MASTER  
    /** 断开安全继电器 **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;    
#endif    

}

/** 进入故障处理子程序，断开安全继电器，
扶梯停止运行，进入故障状态 **/
void ESC_SPI_Error_Process(void)
{
  
    /** 断开安全继电器 **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;
    printf("ESC_SPI_Error_Process \r\n");
}


/** 进入故障处理子程序，断开安全继电器，
扶梯停止运行，进入故障状态 **/
void ESC_SafeRelay_Error_Process(void)
{
 
    /** 断开安全继电器 **/
    SF_RL_CTR = 0;
    SF_RL_WDT = 0;
    printf("ESC_SafeRelay_Error_Process \r\n");
}