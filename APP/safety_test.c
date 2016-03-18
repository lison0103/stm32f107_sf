
/* Includes ------------------------------------------------------------------*/
#include "safety_test.h"
#include "sys.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 RamPntr;

/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
int Safety_test(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int Safety_test(void)
{
#ifdef DEBUG
  debug();
#endif

  /* System Clocks Configuration ---------------------------------------------*/
  RCC_Configuration();
  /* GPIO configuration ------------------------------------------------------*/
//  GPIO_Configuration();
  /* Configure the USART1 */
//  USART_Configuration();

  /* Configure the DMA for debug purposes */
//  DMA_Configuration(0);

  /* -------------------------------------------------------------------------*/
//  printf("... main routine starts ...\r\n");
  /* -------------------------------------------------------------------------*/

  /* NVIC configuration ------------------------------------------------------*/
//  NVIC_Configuration();

  /* Self test routines initialization ---------------------------------------*/
  STL_InitRunTimeChecks();

  while(1)
  {
    /* This is where the main self-test routines are executed ----------------*/
    STL_DoRunTimeChecks();

    /* This is to monitor transparent RAM test activity */
    RamPntr = (u32)(p_RunTimeRamChk) - 0x20000000;
//    printf("RamPntr= %d\r", RamPntr);

    /* Test if PB.09 level is low (Key push-button on Eval Board pressed) */
//    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0x00)
//    {
//      /* WWDG configuration --------------------------------------------------------*/
//      /* Enable WWDG clock */
//      RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
//      /* WWDG clock counter = (PCLK1/4096)/8 = 244 Hz (~4 ms)  */
//      WWDG_SetPrescaler(WWDG_Prescaler_8);
//      /* Set Window value to 0x7F */
//      WWDG_SetWindowValue(0x7F);
//      /* Enable WWDG and set counter value to 0x7F, WWDG timeout = ~4 ms * 64 = 262 ms */
//      WWDG_Enable(0x7F);
//    }

  } /* while(1) */

}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  // Done already?

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);// Done already?

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2); // Done already?

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }

  /* Enable USART1, GPIOA and GPIOC clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA |
                         RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC , ENABLE);

}





