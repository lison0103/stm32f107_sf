/*******************************************************************************
* File Name          : main.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "initial_devices.h"
#include "esc_error_process.h"
#include "safety_test.h"
#include "stm32f10x_STLlib.h"
#include "stm32f10x_STLclassBvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef GEC_SF_MASTER
u32 TimingDelay = 0;
u8 canbuf_recv[8];
u8 res;
u8 can_rcv;
#endif
 

/*******************************************************************************
* Function Name  : LED_indicator
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void LED_indicator(void)
{
	static u32 led_idr_cnt=0;	 
	
	led_idr_cnt++;
	
	if(led_idr_cnt >= 300 && data_error <= 0)
	{
                led_idr_cnt = 0;
		LED=!LED;                
	}   
}


/*******************************************************************************
* Function Name  : Task_Loop
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
void Task_Loop(void)
{ 
  
      static u16 Tms10Counter=0,Tms25Counter=0,Tms50Counter=0,Tms100Counter=0,Tms500Counter=0,Tms1000Counter=0;
      static u32 comm_timeout = 0;
      
      delay_ms(1);
      Tms10Counter++;
      Tms25Counter++;
      Tms50Counter++;
      Tms100Counter++;
      Tms500Counter++; 
      Tms1000Counter++;   

      if(Tms10Counter>=10) Tms10Counter=0;
      if(Tms25Counter>=25) Tms25Counter=0;
      if(Tms50Counter>=50) Tms50Counter=0;
      if(Tms100Counter>=100) Tms100Counter=0;
      if(Tms500Counter>=500) Tms500Counter=0;
      if(Tms1000Counter>=1000) Tms1000Counter=0;      
  
#ifdef GEC_SF_MASTER  
          

//      if( onetime == 0)
//      {
//          onetime++;
//        
//          if(SF_RL1_FB)
//            SPI1_TX_Buff[0] = 0x01;
//          else
//            SPI1_TX_Buff[0] = 0x00;
//                           
//          SPI1_DMA_ReceiveSendByte(num);
//      }
      
     
      STL_DoRunTimeChecks();

      
      if(Tms25Counter == 0)      
      {
          CPU_Exchange_Data_Check();
      }
      
      if(Tms50Counter == 0)
      {                       
          Input_Check();                   
          
          USB_VCP_RecvBufandSend();
          
          SF_EWDT_TOOGLE();
          EWDT_TOOGLE();
          
      } 
      
      if(Tms100Counter == 0)
      {         
          SF_CTR_Check();
      }
      
      
      if(Tms1000Counter == 0)
      {                  
          /** CAN1 send data **/
          res=Can_Send_Msg(CAN1,canbuf_send,4);                          
          if(res)
          {                               
              printf("CAN1TX:fail\r\n");
          }
          else 
          {	 
                                       
              USB_VCP_SendBuf(canbuf_send, 4); 
              
              delay_ms(1);
              
              /** CAN1 receive data **/
              can_rcv=Can_Receive_Msg(CAN1,canbuf_recv);
              if(can_rcv)
              {			                                                                 
                  USB_VCP_SendBuf(canbuf_recv, can_rcv);                                                                                               
              }                                                                       
            
          }                      
        
      }
      
      if(Tms500Counter == 0)
      {
          Comm_DisplayBoard();      
      }
      
#else

      STL_DoRunTimeChecks();

      
      if( onetime == 0)
      {
          onetime++;
                          
          SPI1_DMA_ReceiveSendByte(512);
      }
      if( Tms25Counter == 0 )
      {
          comm_timeout++;
          if(comm_timeout > 100)
          {
              ESC_SPI_Error_Process();
          }
          if( SPI_DMA_RECEIVE_FLAG == 1 )
          {
                comm_timeout = 0;
                
                CPU_Exchange_Data_Check();              
          }          
      }
      
      if( Tms50Counter == 0 )
      {                       
          Input_Check();   
                              
          SF_EWDT_TOOGLE();
          EWDT_TOOGLE();
      } 
      
      if( Tms100Counter == 0 )
      {         
          SF_CTR_Check();
      }      
      
#endif
}


/*******************************************************************************
* Function Name  : main
* Description    : 
*                  
* Input          : None
*                 
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{        
//    Safety_test();
    /** hardware init **/
    Bsp_Init();    
    
    while(1)
    {
//      spi1_test();
        Task_Loop();
        LED_indicator();
           
    }          
          
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/******************************  END OF FILE  *********************************/



