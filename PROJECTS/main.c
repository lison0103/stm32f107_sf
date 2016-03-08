#include "sys.h"
#include "delay.h"    
#include "led.h"
#include "hw_test.h"
#include "can.h"
#include "spi.h"
#include "ewdt.h"
#include "exti.h"
#include "timer.h"
#include "self_test.h"

extern u8 SPI1_TX_Buff[buffersize] ;
extern u8 SPI1_RX_Buff[buffersize] ;

#ifdef GEC_SF_MASTER

#include "esc_record_data.h"
#include "usart.h"
#include "mb85rcxx.h"
#include "usbd_cdc_core_loopback.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
uint8_t Rxbuffer[64]; 
__IO uint32_t receive_count =1;
extern __IO uint32_t  data_sent;

u8 R_SF_RL2_FB_CPU1 = 0;
u8 Master_Temp[10] = {0};
u32 TimingDelay = 0;

#else

u8 Slave_Temp[10] = {0};
u8 R_SF_RL1_FB_CPU2 = 0;

#endif

u8 canbuf_send[8]; 


void Bsp_Init(void)
{
   #ifdef GEC_SF_MASTER
        /** stm32 self test **/
        self_test();
#endif
        /** set system interrupt priority group 2 **/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        
        /** delay init **/
	delay_init();  
        
        /** LED init **/
	LED_Init();
        
        /** input and relay output test init **/
        Hw_Test_Init();
        
        /** spi communication init **/
        SPI1_Init();
        SPI1_NVIC();
//	SPI1_Configuration();
	SPI1_DMA_Configuration();
        
        /** ewdt init **/
        EWDT_Drv_pin_config();
//        power_on_bsp_check();
        
        /** exti init **/
        EXTIX_Init();

#ifdef GEC_SF_MASTER
                
        /** TIM init 1000Khz，计数到10为10us **/
        TIM3_Int_Init(9,71);
        
        /** usart3 init **/
//        USART3_Init();
        
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
        delay_ms(1000);

#else
          TIM2_Int_Init(4999,71);
      
//            for(int i = 0;i < 512;i++)
//    {
//          SPI1_TX_Buff[i] = i%10;
//    }
#endif

}
u8 test = 0;
void LED_indicator(void)
{
	static u32 led_idr_cnt=0;	 
	
	led_idr_cnt++;
	
	if(led_idr_cnt >= 300 && test <= 0)
	{
                led_idr_cnt = 0;
		LED=!LED;                
	}   
}

extern u8 flag,number;
u8 onetime = 0;
#if 1
u32 ms_count = 0;
u16 i = 0;
u16 num = 0;


void Task_Loop(void)
{
    
    
    

#ifdef GEC_SF_MASTER      
    
    ms_count++;
    delay_ms(1);
    
    if( ms_count == 2000 && onetime == 0)
    {
        ms_count = 0;
        onetime++;
        number++;
        if(number > 10)
          number = 0;
        for( i = 0;i < 512;i++)
        {
              SPI1_TX_Buff[i] = number;
        }
        
        num = i;
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA_Cmd(DMA1_Channel3, DISABLE);
        SPI1_ReceiveSendByte(num);
    }

    if ( flag == 1 )
    {
        flag = 2;
        INTX_DISABLE();
        for(i = 0; i < 512; i++)
        {
            if(SPI1_TX_Buff[i] != SPI1_RX_Buff[i])
            {
                  test++;
                  USB_VCP_SendBuf(SPI1_RX_Buff, 512);  
                  break;
            }
        }  
        INTX_ENABLE();
        if( test <= 0)
        {
            if(i == num) 
              AUX1_CTR = 1;
            else        
              AUX1_CTR = 0;    
        }
        else
        {
            AUX1_CTR = 0;  
        } 
        
       
        
        
    }
    
    if (ms_count >= 2000 && flag == 2 )
    {
        ms_count = 0;
        number++;
        if(number > 10)
          number = 0;
        for( i = 0;i < 512;i++)
        {
              SPI1_TX_Buff[i] = number;
        }
        
        num = i;
//        SPI1_ReadWriteByte(0xff);
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA_Cmd(DMA1_Channel3, DISABLE);
        SPI1_ReceiveSendByte(num);     
    
    }
    EWDT_TOOGLE();

#else
    
    delay_ms(1);
    if( onetime == 0)
    {
        onetime++;
        number++;
        if(number > 10)
          number = 0;
        for( i = 0;i < 512;i++)
        {
              SPI1_TX_Buff[i] = number;
        }
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA_Cmd(DMA1_Channel3, DISABLE);
        SPI1_ReceiveSendByte(512);

    }
  
    num = 512;
//    SPI1_ReceiveSendByte(num);
    if( flag == 1 )
    {
        flag = 0;
        INTX_DISABLE();
//        delay_ms(3000);
        for(i = 0; i < 512; i++)
        {
            if(SPI1_TX_Buff[i] != SPI1_RX_Buff[i])
            {
                test++;
                break;
            }
        }
        INTX_ENABLE();
        if( test <= 0)
        {
            if(i == num) 
              AUX2_CTR = 1;
            else        
              AUX2_CTR = 0;    
        }
        else
        {
            AUX2_CTR = 0;  
        }   
          number++;
          if(number > 10)
              number = 0;          
          for(int i = 0;i < 512;i++)
          {
              SPI1_TX_Buff[i] = number;
          }         
          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
          DMA_Cmd(DMA1_Channel2, DISABLE);
          DMA_Cmd(DMA1_Channel3, DISABLE);
          SPI1_ReceiveSendByte(512);
        
    }
    EWDT_TOOGLE();
    

    
    
#endif

}
#else
void Task_Loop(void)
{
u16 num;
#ifdef GEC_SF_MASTER  
  


      static u32 count1 = 0,count2 = 0,count3 = 0;
      u8 canbuf_recv[8];
      u8 res;
      u8 can_rcv;
  
      delay_ms(1);
//      delay_us(1);
      count1++;
      count2++;
      count3++;
      
//      if(count3 == 25)
//      {
//          PLUSE_OUT = 1;
//      }
//      
//      if(count3 == 44)
//      {
//          PLUSE_OUT = 0;
//          count3 = 0;
//      }      
      
      if(count1 == 150)
      {
          if(SF_RL1_FB)
            SPI1_TX_Buff[0] = 0x01;
          else
            SPI1_TX_Buff[0] = 0x00;
          
          num = 512;
          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
          DMA_Cmd(DMA1_Channel2, DISABLE);
          DMA_Cmd(DMA1_Channel3, DISABLE);                   
          SPI1_ReceiveSendByte(num);           
          R_SF_RL2_FB_CPU1 = SPI1_RX_Buff[0];
      }
      
      if(count1==200)
      {                       
          Hw_Test1();
          
          USB_VCP_RecvBufandSend();
          
          SF_RL1_WDT=!SF_RL1_WDT;
          EWDT_TOOGLE();
          
          count1=0;
      } 
      
      
      if(count2 == 1000)
      {         
        
          count2 = 0;
          
          /** CAN1 send data **/
          res=Can_Send_Msg(CAN1,canbuf_send,4);                          
          if(res)
          {                             
              USB_VCP_SendBuf("CAN1TX:fail\r\n", 13);                             
          }
          else 
          {	 
            
//              Usb_Vcp_SendBuf("CAN1TX:", 7);                             
              USB_VCP_SendBuf(canbuf_send, 4); 
              
              delay_ms(1);
              
              /** CAN1 receive data **/
              can_rcv=Can_Receive_Msg(CAN1,canbuf_recv);
              if(can_rcv)
              {			                                 
//                  Usb_Vcp_SendBuf("CAN1RX:", 7);                                 
                  USB_VCP_SendBuf(canbuf_recv, can_rcv);                                                                                               
              }                              
              
//              Usb_Vcp_SendBuf("\r\n\r\n", 4);                             
            
          }                      
        
      }
      
//      if(count3 == 500000)
//      {
//          count3 = 0;
//          Comm_DisplayBoard();
//      
//      }
#else

      extern u8 R_SF_RL1_FB_CPU2;
      
      static u32 count1 = 0;
      
//      delay_us(1);
      delay_ms(1);
      count1++;
      
      if( onetime == 0)
      {
          onetime++;
        
          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
          DMA_Cmd(DMA1_Channel2, DISABLE);
          DMA_Cmd(DMA1_Channel3, DISABLE);                   
          SPI1_ReceiveSendByte(512);
      }
      if(count1 >= 150 && flag == 1)
      {
          flag = 0;
          
          if(SF_RL2_FB)
          {
//            Slave_Temp[1] = 0x01;
              SPI1_TX_Buff[0] = 0x01;
          }
          else
          {
//            Slave_Temp[1] = 0x00;
              SPI1_TX_Buff[0] = 0x00;
          }
        num = 512;  

          while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
          DMA_Cmd(DMA1_Channel2, DISABLE);
          DMA_Cmd(DMA1_Channel3, DISABLE);                   
          SPI1_ReceiveSendByte(num);                             
          
          R_SF_RL1_FB_CPU2 = SPI1_RX_Buff[0];
//          R_SF_RL1_FB_CPU2 = Slave_Temp[0];
      }
      
      if(count1==200)
      {                       
          Hw_Test1();     
          
          SF_RL2_WDT=!SF_RL2_WDT;
          EWDT_TOOGLE();
          
          count1=0;
      }      
      
#endif
}
#endif
int main(void)
{        
    
    /** hardware init **/
    Bsp_Init();

    while(1)
    {
      
        Task_Loop();
        LED_indicator();
           
    }          
          
}
