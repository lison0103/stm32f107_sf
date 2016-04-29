/*******************************************************************************
* File Name          : can.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "led.h"
#include "delay.h"
#include "crc16.h"

#ifdef GEC_SF_MASTER
#include "usbd_cdc_vcp.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define CAN_BAUDRATE  1000      /* 1MBps   */
//#define CAN_BAUDRATE  500  /* 500kBps */
#define CAN_BAUDRATE  250  /* 250kBps */
//#define CAN_BAUDRATE  125  /* 125kBps */
//#define CAN_BAUDRATE  100  /* 100kBps */ 
//#define CAN_BAUDRATE  50   /* 50kBps  */ 
//#define CAN_BAUDRATE  20   /* 20kBps  */ 
//#define CAN_BAUDRATE  10   /* 10kBps  */

/* Private macro -------------------------------------------------------------*/
//CAN接收RX0中断使能
#define CAN1_RX0_INT_ENABLE	1		//0,不使能;1,使能.
#ifdef GEC_SF_MASTER
#define CAN2_RX0_INT_ENABLE	1		//0,不使能;1,使能.
#endif

#define CAN_FRAME_LEN   8
#define CAN_SEND_LEN    3*CAN_FRAME_LEN


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* CAN1 */
u8 CAN1_TX_Buff[canbuffsize] = { 0 };
u8 CAN1_RX_Buff[canbuffsize] = { 0 };
u8 CAN1_TX_Data[canbuffsize] = { 0 };
u8 CAN1_RX_Data[canbuffsize] = { 0 };
/* CAN2 */
u8 CAN2_TX_Buff[canbuffsize] = { 0 };
u8 CAN2_RX_Buff[canbuffsize] = { 0 };
u8 CAN2_TX_Data[canbuffsize] = { 0 };
u8 CAN2_RX_Data[canbuffsize] = { 0 };

u8 can1_receive = 0;
u8 can2_receive = 0;

CAN_TX_DATA_PROCESS_TypeDef  CAN1_TX_Normal;
CAN_TX_DATA_PROCESS_TypeDef  CAN1_TX_Urge;
CAN_TX_DATA_PROCESS_TypeDef  CAN2_TX_Up;
CAN_TX_DATA_PROCESS_TypeDef  CAN2_TX_Down;

CAN_RX_DATA_PROCESS_TypeDef  CAN1_RX_Normal;
CAN_RX_DATA_PROCESS_TypeDef  CAN2_RX_Up;
CAN_RX_DATA_PROCESS_TypeDef  CAN2_RX_Down;

/*******************************************************************************
* Function Name  : CAN_Mode_Init
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
/** CAN init
tsjw:CAN_SJW_1tq~ CAN_SJW_4tq
tbs2:CAN_BS2_1tq~CAN_BS2_8tq;
tbs1:CAN_BS1_1tq ~CAN_BS1_16tq
brp:1~1024;  tq=(brp)*tpclk1
baud rate=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
mode:CAN_Mode_Normal;CAN_Mode_LoopBack;
if Fpclk is 36M,CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
baud rate:36M/((8+9+1)*4)=500Kbps **/
u8 CAN_Mode_Init(CAN_TypeDef* CANx,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE || CAN2_RX0_INT_ENABLE
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif       

        if(CANx == CAN1)
        {
            	                   											 	
            
#ifdef GEC_SF_MASTER            
            
            GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
            GPIO_Init(GPIOD, &GPIO_InitStructure);			
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
            GPIO_Init(GPIOD, &GPIO_InitStructure);			
            
#else                              											 

#ifdef GEC_SF_S_NEW
            /* Connect CAN pins to AF9 */
            GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_9);
            GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_9); 
            
            /* Configure CAN RX and TX pins */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
#else
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
            GPIO_Init(GPIOA, &GPIO_InitStructure);			
            
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
            GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
            
#endif  
            
            CAN_DeInit(CANx);
            CAN_StructInit(&CAN_InitStructure);
            
            //CAN 
            CAN_InitStructure.CAN_TTCM=DISABLE;			//非时间触发通信模式  
            CAN_InitStructure.CAN_ABOM=DISABLE;			//软件自动离线管理	 
            CAN_InitStructure.CAN_AWUM=DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
            CAN_InitStructure.CAN_NART=DISABLE;//ENABLE;			//禁止报文自动传送 ,按照CAN标准，CAN硬件在发送报文失败时会一直自动重传直到发送成功
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	//报文不锁定,新的覆盖旧的  
            CAN_InitStructure.CAN_TXFP=DISABLE;			//优先级由报文标识符决定 
            CAN_InitStructure.CAN_Mode= mode;	                //模式设置： mode:0,普通模式;1,回环模式; 
            //set baud rate
            CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;   
            
#if CAN_BAUDRATE == 1000 /* 1MBps */
            CAN_InitStructure.CAN_Prescaler =6;
#elif CAN_BAUDRATE == 500 /* 500KBps */
            CAN_InitStructure.CAN_Prescaler =12;
#elif CAN_BAUDRATE == 250 /* 250KBps */
            CAN_InitStructure.CAN_Prescaler =24;
#elif CAN_BAUDRATE == 125 /* 125KBps */
            CAN_InitStructure.CAN_Prescaler =48;
#elif  CAN_BAUDRATE == 100 /* 100KBps */
            CAN_InitStructure.CAN_Prescaler =60;
#elif  CAN_BAUDRATE == 50 /* 50KBps */
            CAN_InitStructure.CAN_Prescaler =120;
#elif  CAN_BAUDRATE == 20 /* 20KBps */
            CAN_InitStructure.CAN_Prescaler =300;
#elif  CAN_BAUDRATE == 10 /* 10KBps */
            CAN_InitStructure.CAN_Prescaler =600;
#else
#error "Please select first the CAN Baudrate in Private defines "
#endif  /* CAN_BAUDRATE == 1000 */            
            
            /* Initializes the CAN1 */
            CAN_Init(CANx, &CAN_InitStructure);        	

            /* CAN1 filter init */
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
            CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x3234<<3)&0xFFFF0000)>>16;	
            CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x3234<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffff;//32 bit MASK
            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xfffc;            
            CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
            CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;

            CAN_FilterInit(&CAN_FilterInitStructure);			           

#if CAN1_RX0_INT_ENABLE 
            /* IT Configuration for CAN1 */ 
            /* FIFO 0 message pending Interrupt ,full Interrupt , overrun Interrupt */
            CAN_ITConfig(CAN1,CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE); 		    
#ifdef GEC_SF_MASTER
            NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
#else
            NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
#endif
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
#endif
            
            
        }
#ifdef GEC_SF_MASTER        
        else if(CANx == CAN2)
        {                  											 	
            
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
            GPIO_Init(GPIOB, &GPIO_InitStructure);			

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
            GPIO_Init(GPIOB, &GPIO_InitStructure);			
        
            CAN_DeInit(CANx);
            CAN_StructInit(&CAN_InitStructure);
            
            CAN_InitStructure.CAN_TTCM=DISABLE;		  
            CAN_InitStructure.CAN_ABOM=DISABLE;				 
            CAN_InitStructure.CAN_AWUM=DISABLE;			
            CAN_InitStructure.CAN_NART=DISABLE;//ENABLE;			 
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	  
            CAN_InitStructure.CAN_TXFP=DISABLE;			 
            CAN_InitStructure.CAN_Mode= mode;	         
            
            CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;  
            
#if CAN_BAUDRATE == 1000 /* 1MBps */
            CAN_InitStructure.CAN_Prescaler =6;
#elif CAN_BAUDRATE == 500 /* 500KBps */
            CAN_InitStructure.CAN_Prescaler =12;
#elif CAN_BAUDRATE == 250 /* 250KBps */
            CAN_InitStructure.CAN_Prescaler =24;
#elif CAN_BAUDRATE == 125 /* 125KBps */
            CAN_InitStructure.CAN_Prescaler =48;
#elif  CAN_BAUDRATE == 100 /* 100KBps */
            CAN_InitStructure.CAN_Prescaler =60;
#elif  CAN_BAUDRATE == 50 /* 50KBps */
            CAN_InitStructure.CAN_Prescaler =120;
#elif  CAN_BAUDRATE == 20 /* 20KBps */
            CAN_InitStructure.CAN_Prescaler =300;
#elif  CAN_BAUDRATE == 10 /* 10KBps */
            CAN_InitStructure.CAN_Prescaler =600;
#else
#error "Please select first the CAN Baudrate in Private defines  "
#endif  /* CAN_BAUDRATE == 1000 */             
            
            /* Initializes the CAN2 */
            CAN_Init(CANx, &CAN_InitStructure);        	 

            /* CAN2 filter init */
            CAN_FilterInitStructure.CAN_FilterNumber=14;	
            CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
            CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
            
            //ext id
            CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x3456<<3)&0xFFFF0000)>>16;	
            CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x3456<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffff;//32 bit MASK
            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xff00;            
            CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
            CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;

            CAN_FilterInit(&CAN_FilterInitStructure);			
            
#if CAN2_RX0_INT_ENABLE 
            /* IT Configuration for CAN2 */ 
            CAN_ITConfig(CAN1,CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE); 						    

            NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;           
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
#endif             
            
            
        }
#endif           
	return 0;
}   


/*******************************************************************************
* Function Name  : CAN_RX_Process
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/			    
void CAN_RX_Process( CanRxMsg RxMessage, CAN_RX_DATA_PROCESS_TypeDef* CanRx )
{
    
    u8 i;        
    
    if( ( CanRx->recving == 0 ) && ( RxMessage.Data[0] == 0xfa ) )
    {              
        CanRx->recv_len = RxMessage.Data[1] + 4;  
        CanRx->mlen = CanRx->recv_len;
        CanRx->rxcnt = 0;
        
        CanRx->recving = 1;
    }
    
    if( CanRx->recving == 1 )
    {
        
        for( i = 0; i < RxMessage.DLC; i++ )
        {
            /* receive data */
            CanRx->rx_buff[ CanRx->rxcnt++ ] = RxMessage.Data[i];
            
        }   
        CanRx->mlen -= RxMessage.DLC;    
        
        if( CanRx->mlen == 0 )            
        {
            CanRx->recving = 0;
            CanRx->data_packet = 1;  
        }
    
    }
    
}


/*******************************************************************************
* Function Name  : CAN1_RX0_IRQHandler
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
#if CAN1_RX0_INT_ENABLE	
			    
void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    
    if( CAN_GetITStatus(CAN1,CAN_IT_FF0) != RESET)
    {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
    }
    else if( CAN_GetITStatus(CAN1,CAN_IT_FOV0) != RESET)
    {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FOV0);
    }
    else
    {
        
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
        /** CB normal data RECEIVE **/
        if( ( RxMessage.ExtId == CAN1RX_NORMAL_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can1_receive = 1;        

            CAN_RX_Process( RxMessage, &CAN1_RX_Normal );
        }
    }
}
#endif


/*******************************************************************************
* Function Name  : CAN2_RX0_IRQHandler
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
#if CAN2_RX0_INT_ENABLE	
			    
void CAN2_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
        
    if( CAN_GetITStatus(CAN2,CAN_IT_FF0) != RESET)
    {
        CAN_ClearITPendingBit(CAN2,CAN_IT_FF0);
    }
    else if( CAN_GetITStatus(CAN2,CAN_IT_FOV0) != RESET)
    {
        CAN_ClearITPendingBit(CAN2,CAN_IT_FOV0);
    }
    else if( CAN_GetITStatus(CAN2,CAN_FLAG_FMP0) != RESET )
    {
        CAN_ClearITPendingBit(CAN2,CAN_FLAG_FMP0);
        
        CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
        /** SF urge data RECEIVE **/
        if( ( RxMessage.ExtId == CAN2RX_UP_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can2_receive = 1;        
  
            CAN_RX_Process( RxMessage, &CAN2_RX_Up );
            
        }
        /** SF normal data RECEIVE **/
        else if( ( RxMessage.ExtId == CAN2RX_DOWN_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can2_receive = 1;        

            CAN_RX_Process( RxMessage, &CAN2_RX_Down );
        }        
    }        
}
#endif


/*******************************************************************************
* Function Name  : CAN_Send_Process
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/      
void CAN_Send_Process(CAN_TypeDef* CANx,CAN_TX_DATA_PROCESS_TypeDef* CanTx, uint32_t send_id,uint8_t *buff,uint32_t len)
{
	u32 i;

			
				
        /** packet the data pack ------------------------**/
        if( CanTx->sending == 0 && len > 0 )
        {
            CanTx->mlen = len;
            
            CanTx->tx_buff[0] = 0xfa;
            CanTx->tx_buff[1] = CanTx->mlen - 4;
            for( u8 j = 0; j < CanTx->mlen - 4; j++ )
            {
                CanTx->tx_buff[j+2] = buff[j];
            }
            i = MB_CRC16( CanTx->tx_buff, CanTx->mlen - 2 );
            CanTx->tx_buff[CanTx->mlen - 2] = i;
            CanTx->tx_buff[CanTx->mlen - 1] = i>>8;    
            
            CanTx->p_CanBuff = &CanTx->tx_buff[0];
            CanTx->sending = 1;
        } 
        
        
        /** CAN1 send data ---------------------------------**/
        if( CanTx->sending == 1 )
        {
            
            if( CanTx->mlen > CAN_SEND_LEN )
            {
                for( i = 0; i < 3; i++ )
                {
                    Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CAN_FRAME_LEN ); 
                    CanTx->p_CanBuff += CAN_FRAME_LEN;
                }
                CanTx->mlen -= CAN_SEND_LEN;
            }
            else
            {
                if( CanTx->mlen > 2*CAN_FRAME_LEN )
                {
                    for( i = 0; i < 2; i++ )
                    {
                        Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CAN_FRAME_LEN ); 
                        CanTx->p_CanBuff += CAN_FRAME_LEN;
                    }   
                    CanTx->mlen -= 2*CAN_FRAME_LEN;
                }
                else if( CanTx->mlen > CAN_FRAME_LEN )
                {
                    
                    Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CAN_FRAME_LEN ); 
                    CanTx->p_CanBuff += CAN_FRAME_LEN;                               
                    CanTx->mlen -= CAN_FRAME_LEN;                                        
                }
                
                Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CanTx->mlen );
                CanTx->sending = 0;
                
            }
        }



			
}


/*******************************************************************************
* Function Name  : BSP_CAN_Send
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/  
void BSP_CAN_Send(CAN_TypeDef* CANx, CAN_TX_DATA_PROCESS_TypeDef* CanTx, uint32_t send_id, uint8_t *buff, uint32_t len)
{

	
    if( len > canbuffsize ) return;
    
    switch (*(uint32_t*)&CANx)
    {
       case CAN1_BASE:
        
          CAN_Send_Process(CAN1, CanTx, send_id, buff, len);
          
          break;
#ifdef GEC_SF_MASTER
       case CAN2_BASE:
       
          CAN_Send_Process(CAN2, CanTx, send_id, buff, len);
          break;	
#endif         
    }			
}


/*******************************************************************************
* Function Name  : BSP_CAN_Receive
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/   
uint32_t BSP_CAN_Receive(CAN_TypeDef* CANx,CAN_RX_DATA_PROCESS_TypeDef* CanRx, uint8_t *buff,uint32_t mlen)
{
    uint8_t *pstr;
    uint32_t i=0,len=0;
	
    switch (*(uint32_t*)&CANx)
    {
       case CAN1_BASE:
                       
        /** receive a data packet **/
        if( CanRx->data_packet == 1 )
        {
            if(!MB_CRC16(CanRx->rx_buff, CanRx->recv_len))
            {          
                /* ok */
                pstr = CanRx->rx_buff;					
                len = CanRx->recv_len;
                CanRx->recv_len = 0;
            }
            else
            {
                /* fail */
                for( u8 i = 0; i < CanRx->recv_len; i++ )
                {
                    CanRx->rx_buff[i] = 0;
                }
            }
            CanRx->data_packet = 0;
        }                         
        break;	
#ifdef GEC_SF_MASTER
       case CAN2_BASE: 

        break;	
#endif
    }	   
    
    if(mlen && (mlen<len))
    {
        len = mlen;
    }
    
    if(len>canbuffsize) len=0;
    
    for(i=0;i<len;i++)
    {
        buff[i] = pstr[i];
    }		
			
    return(len);
}

/*******************************************************************************
* Function Name  : Can_Send_Msg
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
/** CAN send data
len:data len(max len is 8)				     
msg:Data Pointer.
return:0,success;
**/		 
u8 Can_Send_Msg(CAN_TypeDef* CANx,u32 exid,u8* msg,u8 len)
{	
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;			// STD ID
	TxMessage.ExtId=exid;			// EXT ID 
//	TxMessage.IDE=CAN_Id_Standard; 	        
        TxMessage.IDE=CAN_Id_Extended; 	        
	TxMessage.RTR=CAN_RTR_Data;		
	TxMessage.DLC=len;			
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	if( CAN_TxStatus_NoMailBox == CAN_Transmit(CANx, &TxMessage) )
        {
            return 1;
        }
        
        return 0;
}


/*******************************************************************************
* Function Name  : Can_Receive_Msg
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
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


/******************************  END OF FILE  *********************************/














