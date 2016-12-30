/*******************************************************************************
* File Name          : timer.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Last modify date   : 09/05/2016
* Description        : This file contains stm32 timer funcions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "bsp_iocfg.h"
#include "sys.h"
#include "stm32f10x_STLlib.h"
#include "esc_safety_check.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void TIM7_IRQHandler(void);


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
        NVIC_InitTypeDef NVIC_InitStructure;
                
        /** TIM7 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);  

        TIM_ITConfig(  
                     TIM7, 
                     TIM_IT_Update ,
                     ENABLE  
                         );
	NVIC_InitStructure.NVIC_IRQChannel = (u8)TIM7_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3u; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0u;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
        
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
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
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
        
        /** TIM4 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
   

	TIM_Cmd(TIM4, DISABLE);         
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
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;            
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);      
        
	TIM_Cmd(TIM3, ENABLE);							 
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
                
        /** TIM2 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;            
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);      
        
	TIM_Cmd(TIM2, ENABLE);        
}

/*******************************************************************************
* Function Name  : TIM1_Int_Init
* Description    : Intialization stm32 Timer1.
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_Int_Init(u16 arr,u16 psc)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
                
        /** TIM1 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;            
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);      
        
	TIM_Cmd(TIM1, ENABLE);    
}


/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.                  
* Input          : None  
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)   
{
      if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) 
      {
        
          TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  
          
          Safety_TimingCheck();
        
      }
}





/******************************  END OF FILE  *********************************/



