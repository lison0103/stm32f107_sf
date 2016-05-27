/*******************************************************************************
* File Name          : exti.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains the external interrupt functions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "exti.h"
#include "timer.h"
#include "safety_test.h"
#include "esc_interrupt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 in2_counter = 0;



/*******************************************************************************
* Function Name  : EXTIX_Init
* Description    : Initialization EXTI.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/	
void EXTIX_Init(void)
{
 
        EXTI_InitTypeDef EXTI_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

#ifdef GEC_SF_MASTER
        
        /** IN1 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource1);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		

        /** IN2 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource0);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_Init(&EXTI_InitStructure);	

        /** IN3 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
  	EXTI_Init(&EXTI_InitStructure);	
        
        /** IN4 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line6;
  	EXTI_Init(&EXTI_InitStructure);		

        /** IN5 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_Init(&EXTI_InitStructure);	      

        /** IN6 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);	   
        
        /** IN7 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource3);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_Init(&EXTI_InitStructure);		

        /** IN8 **/
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource7);
//
//   	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
//  	EXTI_Init(&EXTI_InitStructure);	        
        
 
        /** IN1 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure);  	  
	
        /** IN2 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure);  

        /** IN3 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure);  	  
	
        /** IN4 **/
//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
//  	NVIC_Init(&NVIC_InitStructure); 

        /** IN5 **/
//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
//  	NVIC_Init(&NVIC_InitStructure);  	  
	
        /** IN6 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure); 
        
        /** IN7 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure);          
        
        /** IN8 **/
//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
//  	NVIC_Init(&NVIC_InitStructure); 
        
#else
        
        /** IN1 **/
#ifdef GEC_SF_S_NEW
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource4);
#else        
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource4);
#endif   

   	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		

        /** IN2 **/
#ifdef GEC_SF_S_NEW
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource3);
#else        
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource3);
#endif

   	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_Init(&EXTI_InitStructure);	

        /** IN7 **/
#ifdef GEC_SF_S_NEW
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);
#else        
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
#endif   

   	EXTI_InitStructure.EXTI_Line=EXTI_Line11;
  	EXTI_Init(&EXTI_InitStructure);		

        /** IN8 **/
#ifdef GEC_SF_S_NEW
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource10);
#else        
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource10);
#endif

   	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
  	EXTI_Init(&EXTI_InitStructure);	        
 
        /** IN1 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure);  	  
	
        /** IN2 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure);          
 
        /** IN7 IN8 **/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure); 
        
#endif
}


/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
*                  MASTER IN2
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearFlag(EXTI_Line0);
        EXTI_ClearITPendingBit(EXTI_Line0);
        	
        mtr_X2_int();     
    }
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
*                  MASTER IN1
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line1);
      EXTI_ClearITPendingBit(EXTI_Line1);
	
      mtr_X1_int();     
  }
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
*                  SLAVE IN2
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearFlag(EXTI_Line3);
        EXTI_ClearITPendingBit(EXTI_Line3);
        
//        in2_counter = TIM_GetCounter(TIM2); 
//        TIM_SetCounter(TIM2, 0);  
        
#ifndef GEC_SF_MASTER      
        mtr_X2_int();
#endif
    }
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
*                  SLAVE IN1
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line4);
      EXTI_ClearITPendingBit(EXTI_Line4);
    	
      mtr_X1_int();
  }
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External interrupt Line 5~9 request.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line6);
      EXTI_ClearITPendingBit(EXTI_Line6);
    
      handrail_X2_int();
  }
    
  if(EXTI_GetITStatus(EXTI_Line7) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line7);
      EXTI_ClearITPendingBit(EXTI_Line7);
    
      handrail_X1_int();
  }

  
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External interrupt Line 10~15 request.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
    
  if(EXTI_GetITStatus(EXTI_Line10) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line10);
      EXTI_ClearITPendingBit(EXTI_Line10);
    
  }


  if(EXTI_GetITStatus(EXTI_Line11) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line11);
      EXTI_ClearITPendingBit(EXTI_Line11);
    
  }
  
}


/******************************  END OF FILE  *********************************/


