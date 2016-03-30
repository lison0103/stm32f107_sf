/*******************************************************************************
* File Name          : config_test.c
* Author             : lison
* Version            : V1.0
* Date               : 03/30/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "config_test.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  /* base 0 */
  unsigned int Conf_Pass_Cnt;     /*!< tests passed */
  /* base 4 */
  unsigned int Conf_Err_Cnt;      /*!< tests failed */
}IEC61508_ConfCheck_struct;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

GPIO_InitTypeDef   GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;
IEC61508_ConfCheck_struct  ConfigReg_Check;


/*******************************************************************************
* Function Name  : PWRReg_Check
* Description    : 
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
int PWRReg_Check(void)
{
  
      if ( PWR->CR & PWR_CR_DBP  != (uint32_t)RESET )
      {
          return IEC61508_testFailed;
      }    

      return IEC61508_testPassed;
}


/*******************************************************************************
* Function Name  : PwrReg_Check
* Description    : 
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
int RCCReg_Check(void)
{
//      uint32_t ReadRegVal = 0x00000000;
      
      /* RCC_CR register */
//      ReadRegVal |= ( RCC_CR_PLLON | RCC_CR_CSSON | RCC_CR_HSEON );
      if ( ( RCC->CR & RCC_CR_PLLON  != RCC_CR_PLLON ) || 
           ( RCC->CR & RCC_CR_CSSON  != RCC_CR_CSSON ) ||
           ( RCC->CR & RCC_CR_HSEON  != RCC_CR_HSEON ) )
      {
          return IEC61508_testFailed;
      }    

      /* RCC_CFGR register */
//      ReadRegVal = 0x00000000;
//      ReadRegVal |= ( RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1 
//                     | RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLMULL9 | RCC_CFGR_OTGFSPRE);
      if ( ( RCC->CFGR & RCC_CFGR_HPRE_DIV1  != RCC_CFGR_HPRE_DIV1 ) || 
           ( RCC->CFGR & RCC_CFGR_PPRE1_DIV2  != RCC_CFGR_PPRE1_DIV2 ) ||
           ( RCC->CFGR & RCC_CFGR_PPRE2_DIV1  != RCC_CFGR_PPRE2_DIV1 ) ||
#ifdef GEC_SF_MASTER             
           ( RCC->CFGR & RCC_CFGR_PLLSRC_PREDIV1  != RCC_CFGR_PLLSRC_PREDIV1 ) ||
           ( RCC->CFGR & RCC_CFGR_OTGFSPRE  != RCC_CFGR_OTGFSPRE ) ||  
#else
           ( RCC->CFGR & RCC_PLLSource_HSE_Div1  != RCC_PLLSource_HSE_Div1 ) || 
#endif
           ( RCC->CFGR & RCC_CFGR_PLLMULL9  != RCC_CFGR_PLLMULL9 ) )
      {
          return IEC61508_testFailed;
      }
      
      return IEC61508_testPassed;
}

/*******************************************************************************
* Function Name  : ConfigurationRegister_Check
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
type_testResult_t ConfigurationRegister_Check(void)
{       
  
       ConfigReg_Check.Conf_Pass_Cnt = 0;
       ConfigReg_Check.Conf_Err_Cnt = 0;
       
      
      /*  CRC  Register Check */
  
      /*  PWR  Register Check */
      if( PWRReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }
      
      /*  BKP  Register Check */
      /*  RCC  Register Check */
      if( RCCReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }
     
      
      /*  GPIO  Register Check */
      
      
      /*  EXTI  Register Check */
      
      
      /*  DMA  Register Check */
      
      
      /*  ADC  Register Check */
      /*  DAC  Register Check */
      /*  TIM  Register Check */
      
      
      /*  RTC  Register Check */
      /*  IWDG  Register Check */
      
      
      /*  WWDG  Register Check */
      /*  FSMC  Register Check */
      /*  SDIO  Register Check */
      /*  USB  Register Check */  
      
      
      /*  CAN  Register Check */
      
      
      /*  SPI  Register Check */
      
      
      /*  I2C  Register Check */
      /*  USART  Register Check */ 
      
      

      /*  OTG  Register Check */
      
      
      /*  ETH  Register Check */       
      
      
      return IEC61508_testPassed;
}



/**
  * @brief  Configures the different system clocks.PLL (8MHz * 9=72MHz)
  * @param  None
  * @retval None
  */
void RCC_Configuration_72M(void) 
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
#ifdef GEC_SF_MASTER
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif 

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

  RCC_ClockSecuritySystemCmd(ENABLE);//使能或者失能时钟安全系统
}

/**
  * @brief  Configures the different system clocks.HSE(8MHz)
  * @param  None
  * @retval None
  */
void RCC_Configuration_8M(void)
{
//  SystemInit();//频率设定有system_stm32f10x.c文件中的宏定义 开放某个宏 当调用SystemInit()时即可设置好频率
 
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

  RCC_ClockSecuritySystemCmd(ENABLE);//使能或者失能时钟安全系统
}


/******************************  END OF FILE  *********************************/

