/*******************************************************************************
* File Name          : timer.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains stm32 timer funcions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "bsp_iocfg.h"
#include "sys.h"
#include "stm32f10x_STLlib.h"
#include "esc_safety_check.h"

#ifdef GEC_SF_MASTER
#include "usart.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);

u8 count = 0u;
static u32 t_count = 0u;

/*******************************************************************************
* Function Name  : TIM7_Int_Init
* Description    : Intialization stm32 Timer7.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_Int_Init(u16 arr,u16 psc)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
                
        /** TIM7 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);  

	TIM_Cmd(TIM7, ENABLE);  
        
}


/*******************************************************************************
* Function Name  : TIM6_Int_Init
* Description    : Intialization stm32 Timer6.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_Int_Init(u16 arr,u16 psc)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
                
        /** TIM6 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);  

	TIM_Cmd(TIM6, ENABLE);  
        
}


/*******************************************************************************
* Function Name  : TIM5_Int_Init
* Description    : Intialization stm32 Timer5.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_Int_Init(u16 arr,u16 psc)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
                
        /** TIM5 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;            
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);      
        
	TIM_Cmd(TIM5, ENABLE);

        
}

/*******************************************************************************
* Function Name  : TIM4_Int_Init
* Description    : Intialization stm32 Timer4.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_Int_Init(u16 arr,u16 psc)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
        
        
        /** TIM4 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(  
		TIM4, 
		TIM_IT_Update ,
		ENABLE  
		);
	NVIC_InitStructure.NVIC_IRQChannel = (u8)TIM4_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3u; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0u;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM4, ENABLE);  
        
}


/*******************************************************************************
* Function Name  : TIM3_Int_Init
* Description    : Intialization stm32 Timer3.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{

        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
                
        /** TIM3 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;            
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);      
        
	TIM_Cmd(TIM3, DISABLE);							 
}

/*******************************************************************************
* Function Name  : TIM2_Int_Init
* Description    : Intialization stm32 Timer2.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_Int_Init(u16 arr,u16 psc)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
        
        
        /** TIM2 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc;   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(  
		TIM2, 
		TIM_IT_Update ,
		ENABLE  
		);
	NVIC_InitStructure.NVIC_IRQChannel = (u8)TIM2_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3u;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2u;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM2, ENABLE);  
        
}

/*******************************************************************************
* Function Name  : TIM1_PWM_Init
* Description    : Intialization stm32 Timer1.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef GEC_SF_MASTER
void TIM1_PWM_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  
    
        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
    TIM_TimeBaseStructure.TIM_Period = arr; 
    TIM_TimeBaseStructure.TIM_Prescaler = psc; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0u;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
    
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    TIM_OCInitStructure.TIM_Pulse = 0u; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE);		
    
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);   
    
    TIM_ARRPreloadConfig(TIM1, ENABLE); 
    
    TIM_Cmd(TIM1, ENABLE); 
    

}
#endif


/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.                  
* Input          : None  
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)   
{
      if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) 
      {
        
          TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  
          
          Safety_TimingCheck();
        
      }
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.                  
* Input          : None  
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)   
{
      if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
      {
        
          TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  

      }
}



/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.                  
* Input          : None  
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)   
{
      if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
      {
        
          TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  

          t_count++;
          
          #ifdef GEC_SF_MASTER
            #ifdef USING_USART3_OVERTIME
              USART_ReceiveOvertimeProcess();
            #endif
          #endif
      }
}



/******************************  END OF FILE  *********************************/



