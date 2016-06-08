/*******************************************************************************
* File Name          : initial_devices.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains the devices initinal.
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
void SysTickTimerInit(void);
void ExtCommDeviceInit(void);
void PluseOutputInit(void);

#ifdef GEC_SF_MASTER
void DataIntegrityInFRAMCheck(void);

#endif


/*******************************************************************************
* Function Name  : Bsp_Init
* Description    : Initialize the CPU1,CPU2 and peripherals
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
        Safety_StartupCheck2();
#endif   
        /** set system interrupt priority group 2 **/
	NVIC_Configuration();
        
        /** RCC  and  PVD Configuration **/
        RCC_Configuration();   
        PVD_Configuration();
        
        /** delay init **/
	Delay_Init();  

        /** LED init **/
	LED_Init();
        
        /** input and relay output init **/
        Input_Output_PinInit();           
        /*----------------------------------------------------------------------*/
        /*------------------------- Safety ExtWdt Test -------------------------*/
        /*---------------------- SafetyRelay AuxRelay Test ---------------------*/
        /*----------------------------------------------------------------------*/   
        
        SafetyExtWdt_StartUpCheck();

        
        /** exti init **/
        EXTIX_Init();

        /** usart3 init **/
        /** just for debug **/
#if DEBUG_PRINTF
        USART3_Init();
#endif        

        /** CAN1 init,baud rate 250Kbps **/
	CAN_Int_Init(CAN1);                               
        
#ifdef GEC_SF_MASTER           
        
        /** CAN2 init,baud rate 250Kbps **/
        /** note : use CAN2 , must CAN1 init **/
        CAN_Int_Init(CAN2);         
        
        /** pluse output init **/
        PluseOutputInit();
        
        /** Extenal communication device init **/
        ExtCommDeviceInit();
        
        /*----------------------------------------------------------------------*/
        /*----------------------------- FRAM Test ------------------------------*/
        /*----------------------------------------------------------------------*/
        
        DataIntegrityInFRAMCheck();      

        
        /** USB init **/
//        usb_init();
          
        /** wait slave spi **/
//        delay_ms(200);       
      
#endif  /* GEC_SF_MASTER */

        /** spi communication init **/
        SPI1_Init();
        /*----------------------------------------------------------------------*/
        /*------------------------- Cross Comm CPU test ------------------------*/
        /*------------------------- Data Integrity Test ------------------------*/
        /*----------------------------------------------------------------------*/   
        
        CrossCommCPUCheck();

        
#if SELF_TEST
        /* Self test routines initialization ---------------------------------------*/
        Safety_InitRunTimeChecks();
#endif
        /* systick timer , 5ms interrupt */
        SysTickTimerInit();
}


/*******************************************************************************
* Function Name  : DataIntegrityInFRAMCheck
* Description    : FRAM initialization and test data integrity.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef GEC_SF_MASTER
void DataIntegrityInFRAMCheck(void)
{
    eep_init();
    if(MB85RCXX_Check())
    {
        /** MB85RCXX check fail , do some things **/                   
    }
    else
    {
        esc_data_check();
    } 
}
#endif

/*******************************************************************************
* Function Name  : ExtCommDeviceInit
* Description    : Initialization the external communication devices interface.
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void ExtCommDeviceInit(void)
{
  
    USART3_Init();   
    
    /** timer for usart3 **/
    /** TIM init 10khz, counting to 10 is 1ms **/
    TIM2_Int_Init(9,7199); 

}

/*******************************************************************************
* Function Name  : PluseOutputInit
* Description    : Initialization the pluse output.
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void PluseOutputInit(void)
{
  
    /** TIM init 1000Khz¡ê?counting to 10 is 10us **/
    TIM3_Int_Init(9,71); 

}

/*******************************************************************************
* Function Name  : SysTickTimerInit
* Description    : Initialization the systick timer.
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void SysTickTimerInit(void)
{
  
    /** interrupt time 5ms **/
    if(SysTick_Config(SystemCoreClock / ( 1000 / SYSTEMTICK )))
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
#ifdef GEC_SF_S_NEW
    FLASH_PrefetchBufferCmd(ENABLE);
#else
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
#endif

    /* PLLCLK = 8MHz * 9 = 72 MHz */
#ifdef GEC_SF_MASTER
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
#ifdef GEC_SF_S_NEW
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif       
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
#ifdef GEC_SF_S_NEW
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1 , ENABLE);    

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2
                           |RCC_APB1Periph_TIM4 ,
                            ENABLE); 
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG | RCC_APB2Periph_SPI1, ENABLE);

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA
                           |RCC_AHBPeriph_GPIOB
                           |RCC_AHBPeriph_GPIOC
                           |RCC_AHBPeriph_GPIOD
                           |RCC_AHBPeriph_GPIOE ,
                           ENABLE);
#else     
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
#endif
    
    #ifdef GEC_SF_MASTER
      RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN2 , ENABLE);    
      RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3 , ENABLE);    
    #endif

#ifdef GEC_SF_S_NEW      
      RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM15 , ENABLE);   
#else
      RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5 , ENABLE);   
#endif
      RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM6 , ENABLE); 
      RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM7 , ENABLE); 
      
    /* Enable CSS */
    RCC_ClockSecuritySystemCmd(ENABLE);
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configuring NVIC interrupt group.
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
* Description    : Configuring system Voltage Detector.
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
    
#ifdef GEC_SF_S_NEW
    PWR_PVDLevelConfig(PWR_PVDLevel_4);  
#else
    PWR_PVDLevelConfig(PWR_PVDLevel_2V5);
#endif
    PWR_PVDCmd(ENABLE);

}


/******************************  END OF FILE  *********************************/


