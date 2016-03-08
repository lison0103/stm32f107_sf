#include "initial_devices.h"

#ifdef GEC_SF_MASTER

u32 TimingDelay = 0;

#else

#endif
 


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


void Task_Loop(void)
{
  
  u16 num = 512;
  
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
      if(count1 == 150)      
      {
          SPI_DMA_RECEIVE_FLAG = 0;

          if(SF_RL1_FB)
            SPI1_TX_Buff[0] = 0x01;
          else
            SPI1_TX_Buff[0] = 0x00;
                            
          SPI1_DMA_ReceiveSendByte(num);     
          
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
      
      static u32 count1 = 0;
      
//      delay_us(1);
      delay_ms(1);
      count1++;
      
      if( onetime == 0)
      {
          onetime++;
                          
          SPI1_DMA_ReceiveSendByte(num);
      }
      if(count1 >= 150 && SPI_DMA_RECEIVE_FLAG == 1)
      {
          SPI_DMA_RECEIVE_FLAG = 0;
          
          if(SF_RL2_FB)
          {
              SPI1_TX_Buff[0] = 0x01;
          }
          else
          {
              SPI1_TX_Buff[0] = 0x00;
          }  

                  
          SPI1_DMA_ReceiveSendByte(num);                             
          
          R_SF_RL1_FB_CPU2 = SPI1_RX_Buff[0];
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


int main(void)
{        
    
    /** hardware init **/
    Bsp_Init();

    while(1)
    {
//      spi1_test();
        Task_Loop();
        LED_indicator();
           
    }          
          
}
