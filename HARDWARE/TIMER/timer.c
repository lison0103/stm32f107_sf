/*******************************************************************************
* File Name          : timer.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "bsp_iocfg.h"
#include "sys.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

#ifdef GEC_SF_MASTER
#include "usart.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u8 count = 0;
u32 t_count = 0;



/*******************************************************************************
* Function Name  : TIM4_Int_Init
* Description    : 
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
        
        
        /** TIM2 **/

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM4, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设  
        
}


/*******************************************************************************
* Function Name  : TIM3_Int_Init
* Description    : 
*                  
* Input          : arr: Automatic reload value
*                  psc: Pre clock divisor
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{
#ifdef GEC_SF_MASTER
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


        /** TIM3 **/

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
#endif
        
      
        
							 
}

/*******************************************************************************
* Function Name  : TIM2_Int_Init
* Description    : 
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

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM2, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设  
        
}

void TIM4_IRQHandler(void)   //TIM4中断
{
      if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
      {
        
          TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
          
//          PLUSE_OUT = !PLUSE_OUT;
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
              TickCounter = 0;
              TickCounterInv = 0xFFFFFFFF;

              /* Set Flag read in main loop */
              TimeBaseFlag = 0xAAAAAAAA;
              TimeBaseFlagInv = 0x55555555;

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
              RamTestResult = STL_TranspMarchC();
              ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLER;

        //      ISRCtrlFlowCnt += RAM_MARCHX_ISR_CALLER;
        //      RamTestResult = STL_TranspMarchX();
        //      ISRCtrlFlowCntInv -= RAM_MARCHX_ISR_CALLER;

              switch ( RamTestResult )
              {
                case TEST_RUNNING:
                  break;
                case TEST_OK:
                  #ifdef STL_VERBOSE
//                    printf("\n\r Full RAM verified (Run-time)\n\r");
//                    GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7)));
                  #endif  /* STL_VERBOSE */
                  break;
                case TEST_FAILURE:
                case CLASS_B_DATA_FAIL:
                default:
                  #ifdef STL_VERBOSE
                    printf("\n\r >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check)\n\r");
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
                    printf("\n\r Control Flow error (RAM test) \n\r");
                  #endif  /* STL_VERBOSE */
                  FailSafePOR();
                  }
                  else  /* Full RAM was scanned */
                  {
                    ISRCtrlFlowCnt = 0;
                    ISRCtrlFlowCntInv = 0xFFFFFFFF;
                  }
                } /* End of RAM completed if*/
              } /* End of control flow monitoring */
              else
              {
              #ifdef STL_VERBOSE
                printf("\n\r Control Flow error in ISR \n\r");
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

#ifdef GEC_SF_MASTER
void TIM3_IRQHandler(void)   //TIM3中断
{
      if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
      {
        
          TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 

          count++;
          if(count == 17)
          {
              PLUSE_OUT = 1;
          }
          
          if(count == 25)
          {
              PLUSE_OUT = 0;
              count = 0;
          } 
        
      }
}
#endif

void TIM2_IRQHandler(void)   //TIM2中断
{
      if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
      {
        
          TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 

          t_count++;
          
          #ifdef GEC_SF_MASTER
            #ifdef USING_USART3_OVERTIME
              USART_ReceiveOvertimeProcess();
            #endif
          #endif
      }
}



/******************************  END OF FILE  *********************************/



