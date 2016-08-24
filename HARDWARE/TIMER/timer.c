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
#include "stm32f10x_STLclassBvar.h"
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
u32 t_count = 0u;

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
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; 
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
	NVIC_InitTypeDef NVIC_InitStructure;


        /** TIM3 **/

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(  
		TIM3, 
		TIM_IT_Update ,
		ENABLE  
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3u;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1u;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

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
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3u;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2u;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

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
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;            
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);      
        
	TIM_Cmd(TIM1, DISABLE);
}



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
          
#if 1         
          /* Verify TickCounter integrity */
          if ((TickCounter ^ TickCounterInv) == 0xFFFFFFFFuL)
          {
            TickCounter++;
            TickCounterInv = ~TickCounter;

            if (TickCounter >= SYSTICK_20ms_TB)
            {
                u32 RamTestResult;

              /* Reset timebase counter */
              TickCounter = 0u;
              TickCounterInv = 0xFFFFFFFFu;

              /* Set Flag read in main loop */
              TimeBaseFlag = 0xAAAAAAAAu;
              TimeBaseFlagInv = 0x55555555u;

              if ((CurrentHSEPeriod ^ CurrentHSEPeriodInv) == 0xFFFFFFFFuL)
              {
                ISRCtrlFlowCnt += MEASPERIOD_ISR_CALLER;
                CurrentHSEPeriod = STL_MeasurePeriod();
                CurrentHSEPeriodInv = ~CurrentHSEPeriod;
                ISRCtrlFlowCntInv -= MEASPERIOD_ISR_CALLER;
              }
              else  /* Class B Error on CurrentHSEPeriod */
              {
              #ifdef STL_VERBOSE
                printf("\n\r Class B Error on CurrentHSEPeriod \n\r");
              #endif  /* STL_VERBOSE */
              }

              ISRCtrlFlowCnt += RAM_MARCHC_ISR_CALLER;
              RamTestResult = RAM_RunCheck();
              ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLER;
/*
              ISRCtrlFlowCnt += RAM_MARCHX_ISR_CALLER;
              RamTestResult = STL_TranspMarchX();
              ISRCtrlFlowCntInv -= RAM_MARCHX_ISR_CALLER;
*/
              switch ( RamTestResult )
              {
                case TEST_RUNNING:
                  break;
                case TEST_OK:
                  #ifdef STL_VERBOSE
/*                 
                    printf("\n\r Full RAM verified (Run-time)\n\r");
                    GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7)));
*/                 
                  #endif  /* STL_VERBOSE */
                  break;
                case TEST_FAILURE:
                case CLASS_B_DATA_FAIL:
                default:
                  #ifdef STL_VERBOSE
                    /* >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check) */
                  #endif  /* STL_VERBOSE */
                  FailSafePOR();
                  break;
              } /* End of the switch */

              /* Do we reached the end of RAM test? */
              /* Verify 1st ISRCtrlFlowCnt integrity */
              if ((ISRCtrlFlowCnt ^ ISRCtrlFlowCntInv) == 0xFFFFFFFFuL)
              {
                if (RamTestResult == TEST_OK)
                {
                  if (ISRCtrlFlowCnt != RAM_TEST_COMPLETED)
                  {
                  #ifdef STL_VERBOSE
                    /* Control Flow error (RAM test) */
                  #endif  /* STL_VERBOSE */
                  FailSafePOR();
                  }
                  else  /* Full RAM was scanned */
                  {
                    ISRCtrlFlowCnt = 0u;
                    ISRCtrlFlowCntInv = 0xFFFFFFFFu;
                  }
                } /* End of RAM completed if*/
              } /* End of control flow monitoring */
              else
              {
              #ifdef STL_VERBOSE
                /* Control Flow error in ISR */
              #endif  /* STL_VERBOSE */
              FailSafePOR();
              }
            } /* End of the 20 ms timebase interrupt */
          }
          else  /* Class error on TickCounter */
          {
          #ifdef STL_VERBOSE
            printf("\n\r Class B Error on TickCounter\n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
          }          
          
          
          

#endif
        
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

          SafetySwitchStatus();
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



