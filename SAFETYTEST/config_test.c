/*******************************************************************************
* File Name          : config_test.c
* Author             : lison
* Version            : V1.0
* Date               : 03/30/2016
* Description        : This file contains Configuration register check.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
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
static type_testResult_t PWRReg_Check(void);
static type_testResult_t RCCReg_Check(void);
static type_testResult_t EXTIReg_Check(void);
static type_testResult_t DMAReg_Check(void);
static type_testResult_t TIMReg_Check(void);
static type_testResult_t CANReg_Check(void);
static type_testResult_t SPIReg_Check(void);
#ifdef GEC_SF_MASTER
static type_testResult_t USARTReg_Check(void);
#endif

GPIO_InitTypeDef   GPIO_InitStructure;
static ErrorStatus HSEStartUpStatus;
static IEC61508_ConfCheck_struct  ConfigReg_Check;


/*******************************************************************************
* Function Name  : PWRReg_Check
* Description    : Check Power Control register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
static type_testResult_t PWRReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;
      
      ReadRegVal |= ( PWR_CR_DBP | PWR_CR_PVDE );
      if ( ( ReadRegVal & PWR->CR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }    

      return testResult;
}


/*******************************************************************************
* Function Name  : RCCReg_Check
* Description    : Check Reset and Clock Control register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
static type_testResult_t RCCReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;
      uint32_t SetRegVal = 0x00000000u; 
      
      /* RCC_CR register */
      ReadRegVal |= ( RCC_CR_PLLON | RCC_CR_CSSON | RCC_CR_HSEON );
      if ( ( ReadRegVal & (RCC->CR) ) != ReadRegVal )  
      {
          testResult = IEC61508_testFailed;
      }    

      /* RCC_CFGR register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2 
#ifdef GEC_SF_MASTER
                  | RCC_CFGR_PLLSRC_PREDIV1 
#else
#ifdef GEC_SF_S_NEW 
                  | RCC_CFGR_PLLSRC_PREDIV1     
#else
                  | RCC_PLLSource_HSE_Div1
#endif
#endif
                  | RCC_CFGR_PLLMULL );      
      SetRegVal |= ( RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1 
#ifdef GEC_SF_MASTER
                  | RCC_CFGR_PLLSRC_PREDIV1 
#else
#ifdef GEC_SF_S_NEW 
                  | RCC_CFGR_PLLSRC_PREDIV1     
#else
                  | RCC_PLLSource_HSE_Div1
#endif
#endif
                  | RCC_CFGR_PLLMULL9 );
      if ( ( ReadRegVal &= RCC->CFGR ) != SetRegVal ) 
      {
          testResult = IEC61508_testFailed;
      }

#ifdef GEC_SF_S_NEW

      /* RCC_AHBENR register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= ( RCC_AHBENR_DMA1EN | RCC_AHBENR_CRCEN | RCC_AHBENR_GPIOAEN
                   | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN
                     | RCC_AHBENR_GPIOEEN );
      if( ( ReadRegVal & RCC->AHBENR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }
     
      /* RCC_APB2ENR register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= ( RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_SPI1EN ); 
      if( ( ReadRegVal & RCC->APB2ENR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }

      /* RCC_APB1ENR register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= ( RCC_APB1ENR_PWREN | RCC_APB1ENR_CAN1EN
                     | RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM4EN );                      
      if( ( ReadRegVal & RCC->APB1ENR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }       

#else      
      /* RCC_AHBENR register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= ( RCC_AHBENR_DMA1EN | RCC_AHBENR_CRCEN );
      if( ( ReadRegVal & RCC->AHBENR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }
     
      /* RCC_APB2ENR register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= ( RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN
                     | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN
                     | RCC_APB2ENR_SPI1EN );
      if( ( ReadRegVal & RCC->APB2ENR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }

      /* RCC_APB1ENR register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= ( RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN | RCC_APB1ENR_CAN1EN
#ifdef GEC_SF_MASTER                      
                     | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_CAN2EN
#endif
                     | RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM4EN );                      
      if( ( ReadRegVal & RCC->APB1ENR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }      
#endif
      
      return testResult;
}

/*******************************************************************************
* Function Name  : EXTIReg_Check
* Description    : Check External Interrupt register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
static type_testResult_t EXTIReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;      
      
      /* EXTI_IMR register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= 
#ifdef GEC_SF_MASTER
                  ( EXTI_IMR_MR0 | EXTI_IMR_MR1 );
#else      
                  ( EXTI_IMR_MR3 | EXTI_IMR_MR4 );
#endif
      if( ( ReadRegVal & EXTI->IMR ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }
          
      
      return testResult;
}

/*******************************************************************************
* Function Name  : DMAReg_Check
* Description    : Check DMA Controller register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
static type_testResult_t DMAReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;
      uint32_t SetRegVal = 0x00000000u;      

#ifdef GEC_SF_S_NEW      
      /* DMA_CCR2 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( /*DMA_CCR2_TCIE |*/ DMA_CCR_DIR | DMA_CCR_CIRC | DMA_CCR_PINC
                     | DMA_CCR_MINC | DMA_CCR_PSIZE | DMA_CCR_MSIZE 
                     | DMA_CCR_PL | DMA_CCR_MEM2MEM );
      SetRegVal |= ( /*DMA_CCR2_TCIE |*/ DMA_CCR_MINC | DMA_CCR_PL_1 );
      if( ( ReadRegVal &= DMA1_Channel2->CCR ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }
          
      /* DMA_CCR3 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( /*DMA_CCR3_TCIE |*/ DMA_CCR_DIR | DMA_CCR_CIRC | DMA_CCR_PINC
                     | DMA_CCR_MINC | DMA_CCR_PSIZE | DMA_CCR_MSIZE 
                     | DMA_CCR_PL | DMA_CCR_MEM2MEM );
      SetRegVal |= ( /*DMA_CCR2_TCIE |*/ DMA_CCR_DIR | DMA_CCR_MINC );
      if( ( ReadRegVal &= DMA1_Channel3->CCR ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }
#else
      /* DMA_CCR2 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( /*DMA_CCR2_TCIE |*/ DMA_CCR2_DIR | DMA_CCR2_CIRC | DMA_CCR2_PINC
                     | DMA_CCR2_MINC | DMA_CCR2_PSIZE | DMA_CCR2_MSIZE 
                     | DMA_CCR2_PL | DMA_CCR2_MEM2MEM );
      SetRegVal |= ( /*DMA_CCR2_TCIE |*/ DMA_CCR2_MINC | DMA_CCR2_PL_1 );
      if( ( ReadRegVal &= DMA1_Channel2->CCR ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }
          
      /* DMA_CCR3 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( /*DMA_CCR3_TCIE |*/ DMA_CCR3_DIR | DMA_CCR3_CIRC | DMA_CCR3_PINC
                     | DMA_CCR3_MINC | DMA_CCR3_PSIZE | DMA_CCR3_MSIZE 
                     | DMA_CCR3_PL | DMA_CCR3_MEM2MEM );
      SetRegVal |= ( /*DMA_CCR2_TCIE |*/ DMA_CCR3_DIR | DMA_CCR3_MINC );
      if( ( ReadRegVal &= DMA1_Channel3->CCR ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }      
      
#endif
      
      return testResult;
}


/*******************************************************************************
* Function Name  : TIMReg_Check
* Description    : Check TIM register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
static type_testResult_t TIMReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;
      uint32_t SetRegVal = 0x00000000u;      
      
      /* TIM_CR1 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( TIM_CR1_CEN | TIM_CR1_DIR | TIM_CR1_CKD );
      SetRegVal |= ( TIM_CR1_CEN );
      if( ( ReadRegVal &= TIM2->CR1 ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }
#ifdef GEC_SF_MASTER      
      if( ( ReadRegVal &= TIM3->CR1 ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }  
#endif
      if( ( ReadRegVal &= TIM4->CR1 ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }
          
      /* TIM_DIER register */
      ReadRegVal = 0x00000000u;
      ReadRegVal |= ( TIM_DIER_UIE );
      if( ( ReadRegVal & TIM2->DIER ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }
#ifdef GEC_SF_MASTER      
      if( ( ReadRegVal & TIM3->DIER ) != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }
#endif
      if( ( ReadRegVal & TIM4->DIER )  != ReadRegVal )
      {
          testResult = IEC61508_testFailed;
      }
      
      return testResult;
}

/*******************************************************************************
* Function Name  : CANReg_Check
* Description    : Check Controller Area Network register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
static type_testResult_t CANReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;
      uint32_t SetRegVal = 0x00000000u;      
      
      /* CAN_MCR register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( CAN_MCR_TXFP | CAN_MCR_RFLM | CAN_MCR_NART | CAN_MCR_AWUM
                    | CAN_MCR_ABOM | CAN_MCR_TTCM );
/*      SetRegVal |= ( CAN_MCR_NART );*/
      if( ( ReadRegVal &= CAN1->MCR ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }
#ifdef GEC_SF_MASTER      
      if( ( ReadRegVal &= CAN2->MCR ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }  
#endif
      
      return testResult;
}

/*******************************************************************************
* Function Name  : SPIReg_Check
* Description    : Check Serial Peripheral Interface register
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
static type_testResult_t SPIReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;
      uint32_t SetRegVal = 0x00000000u;      

#ifdef GEC_SF_S_NEW       
      /* SPI_CR1 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_MSTR | SPI_CR1_SPE
                    | SPI_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_RXONLY 
                    | SPI_CR1_CRCEN | SPI_CR1_BIDIMODE);
      SetRegVal |= ( SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_SPE
                    | SPI_CR1_SSM | SPI_CR1_CRCEN );
      if( ( ReadRegVal &= SPI1->CR1 ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }

      /* SPI_CR2 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( SPI_CR2_DS );
      SetRegVal |= ( SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 );

      if( ( ReadRegVal &= SPI1->CR2 ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }      
#else
      /* SPI_CR1 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_MSTR | SPI_CR1_SPE
                    | SPI_CR1_LSBFIRST | SPI_CR1_SSM | SPI_CR1_RXONLY 
                    | SPI_CR1_DFF | SPI_CR1_CRCEN | SPI_CR1_BIDIMODE);
      SetRegVal |= ( SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_SPE
#ifdef GEC_SF_MASTER                    
                    | SPI_CR1_MSTR 
#endif
                    | SPI_CR1_SSM | SPI_CR1_CRCEN );
      if( ( ReadRegVal &= SPI1->CR1 ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }      
      
#endif
      
      return testResult;
}

/*******************************************************************************
* Function Name  : USARTReg_Check
* Description    : Check Universal Synchronous Asynchronous Receiver Transmitter register.
* Input          : None
* Output         : None
* Return         : fail: IEC61508_testFailed   pass: IEC61508_testPassed
*******************************************************************************/
#ifdef GEC_SF_MASTER
static type_testResult_t USARTReg_Check(void)
{
      type_testResult_t testResult = IEC61508_testPassed;
      uint32_t ReadRegVal = 0x00000000u;
      uint32_t SetRegVal = 0x00000000u;      
      
      /* USART_CR1 register */
      ReadRegVal = 0x00000000u;
      SetRegVal = 0x00000000u;
      ReadRegVal |= ( USART_CR1_RE | USART_CR1_TE | USART_CR1_PCE
                    | USART_CR1_M | USART_CR1_UE );
      SetRegVal |= ( USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );
      if( ( ReadRegVal &= USART3->CR1 ) != SetRegVal )
      {
          testResult = IEC61508_testFailed;
      }

      
      return testResult;
}
#endif

/*******************************************************************************
* Function Name  : ConfigurationRegister_Check
* Description    : Check the configuration register.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
type_testResult_t ConfigurationRegister_Check(void)
{       
       type_testResult_t testResult = IEC61508_testPassed;
       
       ConfigReg_Check.Conf_Pass_Cnt = 0u;
       ConfigReg_Check.Conf_Err_Cnt = 0u;
        
      
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
      if( EXTIReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }      
      
      /*  DMA  Register Check */
      if( DMAReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }       
      
      /*  ADC  Register Check */
      /*  DAC  Register Check */
      /*  TIM  Register Check */
      if( TIMReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }      
      
      /*  RTC  Register Check */
      /*  IWDG  Register Check */           
      /*  WWDG  Register Check */
      /*  FSMC  Register Check */
      /*  SDIO  Register Check */
      /*  USB  Register Check */            
      /*  CAN  Register Check */
      if( CANReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }       
      
      /*  SPI  Register Check */
      if( SPIReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }      
      
      /*  I2C  Register Check */
      /*  USART  Register Check */ 
#ifdef GEC_SF_MASTER
      if( USARTReg_Check() == IEC61508_testPassed )
      {
          ConfigReg_Check.Conf_Pass_Cnt++;
      }
      else
      {
          ConfigReg_Check.Conf_Err_Cnt++;
      }      
#endif      

      /*  OTG  Register Check */           
      /*  ETH  Register Check */     
      
      
      
      if( ConfigReg_Check.Conf_Err_Cnt >= 1u ) 
      {
          testResult = IEC61508_testFailed;
      }
      
      return testResult;
}


/*******************************************************************************
* Function Name  : RCC_Configuration_72M
* Description    : Configures the different system clocks.PLL (8MHz * 9=72MHz)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration_72M(void) 
{ 


  RCC_DeInit(); 

  RCC_HSEConfig(RCC_HSE_ON); 

  HSEStartUpStatus = RCC_WaitForHSEStartUp(); 

  if (HSEStartUpStatus == SUCCESS) 
  { 
#ifdef GEC_SF_S_NEW
    FLASH_PrefetchBufferCmd(ENABLE);
#else
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
#endif

     FLASH_SetLatency(FLASH_Latency_2); 

     RCC_HCLKConfig(RCC_SYSCLK_Div1); 

     RCC_PCLK2Config(RCC_HCLK_Div1); 

     RCC_PCLK1Config(RCC_HCLK_Div2); 

#ifdef GEC_SF_S_NEW
     RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div6);
#else
     RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
#endif
     /* Set PLL clock output to 72MHz using HSE (8MHz) as entry clock */ 
#ifdef GEC_SF_MASTER
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
#ifdef GEC_SF_S_NEW
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif
#endif 

     RCC_PLLCmd(ENABLE); 

     while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) 
     {} 

     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 

     while (RCC_GetSYSCLKSource() != 0x08u) 
     {} 
  } 
  /* Enable or disable clock security system */
  RCC_ClockSecuritySystemCmd(ENABLE);
}


/*******************************************************************************
* Function Name  : RCC_Configuration_8M
* Description    : Configures the different system clocks.HSE(8MHz)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration_8M(void)
{
 
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Select HSE as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

    /* Wait till HSE is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x04u)
    {}
  }
  /* Enable or disable clock security system */
  RCC_ClockSecuritySystemCmd(ENABLE);
}


/******************************  END OF FILE  *********************************/

