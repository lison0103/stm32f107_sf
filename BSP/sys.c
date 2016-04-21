/*******************************************************************************
* File Name          : sys.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : GetLockCode
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
u32 GetLockCode(void)
{
  
   u32 CpuID[3];
   u32 Lock_Code;  
  
  /** acquire the cpu unique ID **/
  CpuID[0]=*(vu32*)(0x1ffff7e8);
  CpuID[1]=*(vu32*)(0x1ffff7ec);
  CpuID[2]=*(vu32*)(0x1ffff7f0);
  
  printf("\r\nSerial Number:%08X%08X%08X\r\n",CpuID[0],CpuID[1],CpuID[2]);
  
  /** encryption **/
  Lock_Code=(CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
   
  return Lock_Code;
}


/*******************************************************************************
* Function Name  : fputc
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/  
#ifdef GEC_SF_MASTER
#if DEBUG_PRINTF
int fputc(int ch, FILE *f)
{   
  
//        USB_VCP_SendBuf((u8 *)(&ch), 1);
      while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
          USART3->DR = (u8) ch; 

	return ch;
}

#else

int fputc(int ch, FILE *f)
{      
      return ch;
}

#endif

#else

#if DEBUG_PRINTF

#ifdef GEC_SF_S_NEW
int fputc(int ch, FILE *f)
{   
    
	while((USART3->ISR&0X40)==0);//循环发送,直到发送完毕   
            USART3->TDR = (u8) ch;  
            
	return ch;
}
#else
int fputc(int ch, FILE *f)
{   
    
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
            USART3->DR = (u8) ch;  
            
	return ch;
}

#endif

#else

int fputc(int ch, FILE *f)
{      
      return ch;
}

#endif

#endif


//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM("WFI");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{
	__ASM("CPSID I");    		  
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM("CPSIE I");		  
}
//设置栈顶地址
//addr:栈顶地址
void MSR_MSP(u32 addr) 
{
    __ASM("MSR MSP, r0"); 			//set Main Stack value
    __ASM("BX r14");
}

/******************************  END OF FILE  *********************************/



