#include "can.h"
#include "led.h"
#include "delay.h"

#ifdef GEC_SF_MASTER

#include "usbd_cdc_vcp.h"

extern uint32_t APP_Rx_ptr_in;
extern uint8_t APP_Rx_Buffer   [APP_RX_DATA_SIZE];
#endif

/** CAN init
tsjw:CAN_SJW_1tq~ CAN_SJW_4tq
tbs2:CAN_BS2_1tq~CAN_BS2_8tq;
tbs1:CAN_BS1_1tq ~CAN_BS1_16tq
brp:1~1024;  tq=(brp)*tpclk1
baud rate=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
mode:CAN_Mode_Normal;CAN_Mode_LoopBack;
if Fpclk is 36M,CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
baud rate:36M/((8+9+1)*4)=500Kbps **/
u8 CAN_Mode_Init(CAN_TypeDef* CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE || CAN2_RX0_INT_ENABLE
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

        if(CANx == CAN1)
        {
            	                   											 

            RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);	
            
#ifdef GEC_SF_MASTER            
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
            
            GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
            GPIO_Init(GPIOD, &GPIO_InitStructure);			
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
            GPIO_Init(GPIOD, &GPIO_InitStructure);			
            
#else            
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);                   											 
                       
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
            GPIO_Init(GPIOA, &GPIO_InitStructure);			
            
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
            GPIO_Init(GPIOA, &GPIO_InitStructure);			            
#endif            
            
            //CAN 
            CAN_InitStructure.CAN_TTCM=DISABLE;			//非时间触发通信模式  
            CAN_InitStructure.CAN_ABOM=DISABLE;			//软件自动离线管理	 
            CAN_InitStructure.CAN_AWUM=DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
            CAN_InitStructure.CAN_NART=ENABLE;			//禁止报文自动传送 
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	//报文不锁定,新的覆盖旧的  
            CAN_InitStructure.CAN_TXFP=DISABLE;			//优先级由报文标识符决定 
            CAN_InitStructure.CAN_Mode= mode;	                //模式设置： mode:0,普通模式;1,回环模式; 
            //set baud rate
            CAN_InitStructure.CAN_SJW=tsjw;				
            CAN_InitStructure.CAN_BS1=tbs1; 			
            CAN_InitStructure.CAN_BS2=tbs2;				
            CAN_InitStructure.CAN_Prescaler=brp;        
            CAN_Init(CANx, &CAN_InitStructure);        	

            CAN_FilterInitStructure.CAN_FilterNumber=0;	
            CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
            CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	
            
            //any id
//            CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32位ID
//            CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
//            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
            
            //std id
//            CAN_FilterInitStructure.CAN_FilterIdHigh=(0x10) << 5;	//32位ID
//            CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffff;//32位MASK
//            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xfffc;
            
            //ext id
            CAN_FilterInitStructure.CAN_FilterIdHigh=(u16)((0x1234) >> 13);	//32 bit ID
            CAN_FilterInitStructure.CAN_FilterIdLow=(u16)(0x1234 << 3) | CAN_ID_EXT;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffff;//32 bit MASK
            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xfffc;            
            CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
            CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;

            CAN_FilterInit(&CAN_FilterInitStructure);			           
            
            
        }
        else if(CANx == CAN2)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                  											 

            RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);	
            

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
            GPIO_Init(GPIOB, &GPIO_InitStructure);			

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
            GPIO_Init(GPIOB, &GPIO_InitStructure);			
        

            
            CAN_InitStructure.CAN_TTCM=DISABLE;		  
            CAN_InitStructure.CAN_ABOM=DISABLE;				 
            CAN_InitStructure.CAN_AWUM=DISABLE;			
            CAN_InitStructure.CAN_NART=ENABLE;			 
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	  
            CAN_InitStructure.CAN_TXFP=DISABLE;			 
            CAN_InitStructure.CAN_Mode= mode;	         
            
            CAN_InitStructure.CAN_SJW=tsjw;				
            CAN_InitStructure.CAN_BS1=tbs1; 			
            CAN_InitStructure.CAN_BS2=tbs2;				
            CAN_InitStructure.CAN_Prescaler=brp;        	
            CAN_Init(CANx, &CAN_InitStructure);        	 

            CAN_FilterInitStructure.CAN_FilterNumber=14;	
            CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
            CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	 
            CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	
            CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
            CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
            CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;

            CAN_FilterInit(&CAN_FilterInitStructure);			
            
        }
	
#if CAN1_RX0_INT_ENABLE 
	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);				.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
        
#if CAN2_RX0_INT_ENABLE 
	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);						    

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif        
	return 0;
}   
 
#if CAN1_RX0_INT_ENABLE	//enable CAN1 RX0 interrupt
			    
void CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
               
        CAN_Receive(CAN1, 0, &RxMessage);

#ifdef GEC_SF_MASTER
        Usb_Vcp_SendBuf("CAN1RX:", 6);
        Usb_Vcp_SendBuf(RxMessage.Data, RxMessage.DLC);
        Usb_Vcp_SendBuf("\r\n", 2);
#endif
}
#endif

#if CAN2_RX0_INT_ENABLE	//enable CAN2 RX0 interrupt
			    
void CAN2_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
               
        CAN_Receive(CAN2, 0, &RxMessage);
#ifdef GEC_SF_MASTER
        Usb_Vcp_SendBuf("CAN2RX:", 6);
        Usb_Vcp_SendBuf(RxMessage.Data, RxMessage.DLC);
        Usb_Vcp_SendBuf("\r\n", 2);      
#endif
}
#endif

/** CAN send data
len:data len(max len is 8)				     
msg:Data Pointer.
return:0,success;
**/		 
u8 Can_Send_Msg(CAN_TypeDef* CANx,u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;			// STD ID
	TxMessage.ExtId=0x1314;			// EXT ID 
//	TxMessage.IDE=CAN_Id_Standard; 	        
        TxMessage.IDE=CAN_Id_Extended; 	        
	TxMessage.RTR=CAN_RTR_Data;		
	TxMessage.DLC=len;			
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CANx, &TxMessage);   
	i=0; 
        delay_ms(1);
	while((CAN_TransmitStatus(CANx, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))
        {
            i++;	
        }
	if(i>=0XFFF)
        {  
            return 1;
        }
	return 0;	 
}
/** CAN receive data
buf:data cache;	 
return:0,no data receive;
other,Length of the received data;
**/
u8 Can_Receive_Msg(CAN_TypeDef* CANx,u8 *buf)
{		   		   
      u32 i;
      CanRxMsg RxMessage;
      if( CAN_MessagePending(CANx,CAN_FIFO0)==0)return 0;		 
      
      CAN_Receive(CANx, CAN_FIFO0, &RxMessage);	
      for(i=0;i<8;i++)
      {
          buf[i]=RxMessage.Data[i];  
      }
      return RxMessage.DLC;	
}














