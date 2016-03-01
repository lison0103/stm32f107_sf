/**
 * Company	: ThyssenKrupp PDC
 * @file	: config_test.c
 * @purpose     :
 * @brief	: c based Configuration Register test POST configure test 
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 12-May-2014
 * @author	: Paul
*/

#include "stm32f10x.h"
#include "config_test.h"

EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;
ErrorStatus HSEStartUpStatus;

/**
  * @brief  Configure PA.00 in interrupt mode
  * @param  None
  * @retval None
  */
void EXTI0_Config(void)
{
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  /* Configure PA.00 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  /* Connect EXTI0 Line to PA.00 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the different system clocks.PLL (8MHz * 9=72MHz)
  * @param  None
  * @retval None
  */
void RCC_Config(void) 
{ 

  /* 这里是重置了RCC的设置，类似寄存器复位 */ 
  RCC_DeInit(); 

  /* 使能外部高速晶振 */ 
  RCC_HSEConfig(RCC_HSE_ON); 

  /* 等待高速晶振稳定 */ 
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); 

  if (HSEStartUpStatus == SUCCESS) 
  { 
     /* 使能flash预读取缓冲区 */ 
     FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); 

     /* 令Flash处于等待状态，2是针对高频时钟的，这两句跟RCC没直接关系，可以暂且略过 */ 
     FLASH_SetLatency(FLASH_Latency_2); 

     /* HCLK = SYSCLK 设置高速总线时钟=系统时钟*/ 
     RCC_HCLKConfig(RCC_SYSCLK_Div1); 

     /* PCLK2 = HCLK 设置低速总线2时钟=高速总线时钟*/ 
     RCC_PCLK2Config(RCC_HCLK_Div1); 

     /* PCLK1 = HCLK/2 设置低速总线1的时钟=高速时钟的二分频*/ 
     RCC_PCLK1Config(RCC_HCLK_Div2); 

     /* ADCCLK = PCLK2/6 设置ADC外设时钟=低速总线2时钟的六分频*/ 
     RCC_ADCCLKConfig(RCC_PCLK2_Div6); 

     /* Set PLL clock output to 72MHz using HSE (8MHz) as entry clock */ 
     //上面这句例程中缺失了，但却很关键 
     /* 利用锁相环讲外部8Mhz晶振9倍频到72Mhz */ 
//     RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); 

     /* Enable PLL 使能锁相环*/ 
     RCC_PLLCmd(ENABLE); 

     /* Wait till PLL is ready 等待锁相环输出稳定*/ 
     while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) 
     {} 

     /* Select PLL as system clock source 将锁相环输出设置为系统时钟 */ 
     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 

     /* Wait till PLL is used as system clock source 等待校验成功*/ 
     while (RCC_GetSYSCLKSource() != 0x08) 
     {} 
} 

     /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */ 
     //使能外围接口总线时钟，注意各外设的隶属情况，不同芯片的分配不同，到时候查手册就可以 
  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE); 

  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | 
  //                        RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG | 
  //                        RCC_APB2Periph_AFIO, ENABLE); 
  /* Enable GPIOA, GPIOD, RCC_APB2Periph_GPIO_KEY_BUTTON and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_AFIO, ENABLE);
  RCC_ClockSecuritySystemCmd(ENABLE);//使能或者失能时钟安全系统
}

/**
  * @brief  Configures the different system clocks.HSE(8MHz)
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  SystemInit();//频率设定有system_stm32f10x.c文件中的宏定义 开放某个宏 当调用SystemInit()时即可设置好频率
 
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    //FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 0 wait state */
    //FLASH_SetLatency(FLASH_Latency_0);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Select HSE as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

    /* Wait till HSE is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x04)
    {}
  }

  /* Enable GPIOA, GPIOD, RCC_APB2Periph_GPIO_KEY_BUTTON and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_AFIO, ENABLE);
  RCC_ClockSecuritySystemCmd(ENABLE);//使能或者失能时钟安全系统
}

