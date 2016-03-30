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
#include "check_instruction.h"
#include "check_instruction_2.h"
#include "pc_test.h"
#include "flag_test.h"
#include "config_test.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void PVD_Configuration(void);

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

        /* self test ---------------------------------------------------------*/
#if SELF_TEST
        Safety_StartupCheck();
#endif  
        /** set system interrupt priority group 2 **/
	NVIC_Configuration();
        
        /** RCC  and  PVD Configuration **/
        RCC_Configuration();   
        PVD_Configuration();
        
        /** delay init **/
	delay_init();  

        /** LED init **/
	LED_Init();
        
        /** input and relay output test init **/
        Input_Output_PinInit();        
        
        /** ewdt init **/
        /* move to STLstartup.c */
//        EWDT_Drv_pin_config();
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
//        delay_ms(200);
        

#else
        /** TIM init 1000Khz£¬counting to 5000 is 5ms **/
        TIM2_Int_Init(4999,71);
      
#endif  /* GEC_SF_MASTER */

        /** spi communication init **/
        SPI1_Init();
	SPI1_DMA_Configuration();
        SPI1_DMA_Check();
        
#if SELF_TEST
        /* Self test routines initialization ---------------------------------------*/
        STL_InitRunTimeChecks();
#endif
        /* systick timer , 5ms interrupt */
 	if(SysTick_Config(SystemCoreClock / 200))
        {
          /* Capture error */
          while (1);
        }
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
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2); 

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

    
    /* Enable GPIO and Peripherals clocks */   
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE );
    
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1 , ENABLE);    
    
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2
                           |RCC_APB1Periph_TIM4 ,
                            ENABLE);  
                                        
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1, ENABLE);
  
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA
                           |RCC_APB2Periph_GPIOB
                           |RCC_APB2Periph_GPIOC
                           |RCC_APB2Periph_GPIOD
                           |RCC_APB2Periph_GPIOE ,
                           ENABLE); 
  

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_CRC, ENABLE); 
  
    #ifdef GEC_SF_MASTER
      RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN2 , ENABLE);    
      RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3 , ENABLE);   
    #endif
      
    /* Enable CSS */
    RCC_ClockSecuritySystemCmd(ENABLE);
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
    
//    INTX_ENABLE();

}

/*******************************************************************************
* Function Name  : PVD_Configuration
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_Configuration(void)
{
  
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//EXTI_Trigger_Falling;//EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the PVD Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    PWR_PVDLevelConfig(PWR_PVDLevel_2V5);
    PWR_PVDCmd(ENABLE);

}


/******************************  END OF FILE  *********************************/


