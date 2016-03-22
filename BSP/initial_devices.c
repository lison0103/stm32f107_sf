/*******************************************************************************
* File Name          : initial_devices.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "initial_devices.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


#ifdef GEC_SF_MASTER
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
uint8_t Rxbuffer[64]; 
__IO uint32_t receive_count =1;
#endif


/*******************************************************************************
* Function Name  : Bsp_Init
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_Init(void)
{
  
        /** RCC  Configuration **/
        RCC_Configuration();

        /** set system interrupt priority group 2 **/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        
        /** delay init **/
	delay_init();  
        
        /** LED init **/
	LED_Init();
        
        /** input and relay output test init **/
        Input_Output_PinInit();
        
        /** spi communication init **/
        SPI1_Init();
	SPI1_DMA_Configuration();
        
        /** ewdt init **/
        EWDT_Drv_pin_config();
//        power_on_bsp_check();
        
        /** exti init **/
        EXTIX_Init();

#ifdef GEC_SF_MASTER
                
        /** TIM init 1000Khz£¬counting to 10 is 10us **/
        TIM3_Int_Init(9,71);
        
        /** usart3 init **/
        USART3_Init();
        
        /** timer for usart3 **/
        /** TIM init 10khz, counting to 10 is 1ms **/
        TIM2_Int_Init(9,7199);
        
        /** MB85RCXX init **/
        eep_init();
        if(MB85RCXX_Check())
        {
              /** MB85RCXX check fail , do some things **/                   
        }
        else
        {
            esc_data_check();
        }
       	 	
        /** CAN1 init,baud rate 250Kbps **/
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);  
        
        /** CAN2 init,baud rate 250Kbps **/
        /** note : use CAN2 , must CAN1 init **/
//        CAN_Mode_Init(CAN2,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);           

        
        /** USB VCP init **/
          USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);          
          
        /** wait slave spi **/
        delay_ms(200);
        

#else
        /** TIM init 1000Khz£¬counting to 5000 is 5ms **/
        TIM2_Int_Init(4999,71);
      
#endif  /* GEC_SF_MASTER */
        
        
        /* Self test routines initialization ---------------------------------------*/
        STL_InitRunTimeChecks();

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
#ifdef GEC_SF_MASTER
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif
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
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA |
//                         RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC , ENABLE);

}


/******************************  END OF FILE  *********************************/


