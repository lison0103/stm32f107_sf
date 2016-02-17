#include "exti.h"
#include "timer.h"	  

u32 in2_counter = 0;

void EXTIX_Init(void)
{
 
        EXTI_InitTypeDef EXTI_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

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
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	
        
 
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
#else
        
        /** IN1 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource4);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		

        /** IN2 **/
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource3);

   	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
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
        
#endif
}

/** MASTER IN2 **/ 
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearFlag(EXTI_Line0);
        EXTI_ClearITPendingBit(EXTI_Line0);
        
        in2_counter = TIM_GetCounter(TIM2); 
        TIM_SetCounter(TIM2, 0);
      
    }
}

/** MASTER IN1 **/
void EXTI1_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line1);
      EXTI_ClearITPendingBit(EXTI_Line1);
    
  }
}

/** SLAVE IN2 **/ 
void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearFlag(EXTI_Line3);
        EXTI_ClearITPendingBit(EXTI_Line3);
        
        in2_counter = TIM_GetCounter(TIM2); 
        TIM_SetCounter(TIM2, 0);        
      
    }
}

/** SLAVE IN1 **/
void EXTI4_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
      EXTI_ClearFlag(EXTI_Line4);
      EXTI_ClearITPendingBit(EXTI_Line4);
    
  }
}