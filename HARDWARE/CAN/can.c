/*******************************************************************************
* File Name          : can.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains can functions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "led.h"
#include "delay.h"
#include "crc16.h"
#include "esc.h"

#ifdef GEC_SF_S_NEW
#include "usb_istr.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* #define CAN_BAUDRATE  1000 */     /* 1MBps   */
#define CAN_BAUDRATE  500  /* 500kBps */
/* #define CAN_BAUDRATE  250 */ /* 250kBps */
/* #define CAN_BAUDRATE  125 */ /* 125kBps */
/* #define CAN_BAUDRATE  100 */ /* 100kBps */ 
/* #define CAN_BAUDRATE  50 */  /* 50kBps  */ 
/* #define CAN_BAUDRATE  20 */ /* 20kBps  */ 
/* #define CAN_BAUDRATE  10 */  /* 10kBps  */

/* Private macro -------------------------------------------------------------*/
#define CAN1_RX0_INT_ENABLE	1		
#ifdef GEC_SF_MASTER
#define CAN2_RX0_INT_ENABLE	1
#else
#define CAN2_RX0_INT_ENABLE	0
#endif


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void CAN_RX_Process( CanRxMsg RxMessage, CAN_RX_DATA_PROCESS_TypeDef* CanRx );
void CAN1_RX0_IRQHandler(void);
void CAN2_RX0_IRQHandler(void);
void CAN1_TX_IRQHandler(void);
void CAN2_TX_IRQHandler(void);


/* CAN1 */
u8 CAN1_TX_Data[canbuffsize] = { 0 };
u8 CAN1_RX_Data[canbuffsize] = { 0 };

/* CAN2 */
#ifdef GEC_SF_MASTER
u8 CAN1_TX2_Data[canbuffsize] = { 0 };
u8 CAN1_RX2_Data[canbuffsize] = { 0 };
u8 CAN2_TX_Data[canbuffsize] = { 0 };
u8 CAN2_RX_Data[canbuffsize] = { 0 };
u8 CAN2_TX2_Data[canbuffsize] = { 0 };
u8 CAN2_RX2_Data[canbuffsize] = { 0 };
#endif

u8 can1_receive = 0u;
#ifdef GEC_SF_MASTER
u8 can2_receive = 0u;
#endif

#ifdef GEC_SF_MASTER
CAN_TX_DATA_PROCESS_TypeDef  CAN1_TX_Normal;
CAN_TX_DATA_PROCESS_TypeDef  CAN1_TX_Urge;
CAN_TX_DATA_PROCESS_TypeDef  CAN2_TX_Up;
CAN_TX_DATA_PROCESS_TypeDef  CAN2_TX_Down;

CAN_RX_DATA_PROCESS_TypeDef  CAN1_RX_Normal;
CAN_RX_DATA_PROCESS_TypeDef  CAN2_RX_Up;
CAN_RX_DATA_PROCESS_TypeDef  CAN2_RX_Down;
#else
CAN_TX_DATA_PROCESS_TypeDef  CAN1_TX_Normal;
CAN_RX_DATA_PROCESS_TypeDef  CAN1_RX_Normal;
#endif

/*******************************************************************************
* Function Name  : CAN_Int_Init
* Description    : Initialization can.
* CAN_SJW: CAN_SJW_1tq~ CAN_SJW_4tq
* CAN_BS2: CAN_BS2_1tq~CAN_BS2_8tq;
* CAN_BS1: CAN_BS1_1tq ~CAN_BS1_16tq
* CAN_Prescaler: 1~1024;  tq=(brp)*tpclk1
* baud rate = Fpclk1/((tbs1+1+tbs2+1+1)*brp)
* if Fpclk is 36M, baud rate:36M/((1+3+2)*24)=250Kbps               
* Input          : CANx: CAN1 or CAN2
*                  None
* Output         : None
* Return         : None
*******************************************************************************/

u8 CAN_Int_Init(CAN_TypeDef* CANx)
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
            GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_9);
            GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_9); 
            
            /* Configure CAN RX and TX pins */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
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
                        
            /*  non-time-triggered communication mode */
            CAN_InitStructure.CAN_TTCM=DISABLE;			
            /* automatic offline management software */
            CAN_InitStructure.CAN_ABOM=DISABLE;				 
            /* wake-sleep mode via software (Clear CAN-> MCR's SLEEP bit) */
            CAN_InitStructure.CAN_AWUM=DISABLE;			
            /* message is automatically transferred, in accordance with the CAN standard, */
            /* CAN hardware failure when sending packets would have been automatic retransmission until sent successfully */
            CAN_InitStructure.CAN_NART=DISABLE;	
            /* message is not locked, the new over the old one */
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	
            /* priority is determined by the packet identifier */
            CAN_InitStructure.CAN_TXFP=DISABLE;			
            CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 
            
            /* set baud rate */
            CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;   
            
#if CAN_BAUDRATE == 1000 /* 1MBps */
            CAN_InitStructure.CAN_Prescaler = 6u;
#elif CAN_BAUDRATE == 500 /* 500KBps */
            CAN_InitStructure.CAN_Prescaler = 12u;
#elif CAN_BAUDRATE == 250 /* 250KBps */
            CAN_InitStructure.CAN_Prescaler = 24u;
#elif CAN_BAUDRATE == 125 /* 125KBps */
            CAN_InitStructure.CAN_Prescaler = 48u;
#elif  CAN_BAUDRATE == 100 /* 100KBps */
            CAN_InitStructure.CAN_Prescaler = 60u;
#elif  CAN_BAUDRATE == 50 /* 50KBps */
            CAN_InitStructure.CAN_Prescaler = 120u;
#elif  CAN_BAUDRATE == 20 /* 20KBps */
            CAN_InitStructure.CAN_Prescaler = 300u;
#elif  CAN_BAUDRATE == 10 /* 10KBps */
            CAN_InitStructure.CAN_Prescaler = 600u;
#else
#error "Please select first the CAN Baudrate in Private defines "
#endif  /* CAN_BAUDRATE == 1000 */            
            
            /* Initializes the CAN1 */
            CAN_Init(CANx, &CAN_InitStructure);        	

            /* CAN1 filter init */
            /* 16 bit mask: STDID[10:0], IDE, RTR, EXTDID[17:15] */
            /* 32 bit mask: STDID[10:0], EXTDID[17:0], IDE, RTR,0 */
            CAN_FilterInitStructure.CAN_FilterNumber = 0u;	
            CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 	
            CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	

/* #ifdef GEC_SF_S_NEW  */          
            /* any id */
            CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000u;	
            CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000u;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000u;
            CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000u;
            
/*
#else            
            //ext id
            CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x00C8<<3)&0xFFFF0000)>>16;	
            CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x00C8<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffff;//32 bit MASK
            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xffff;         
#endif            
*/            
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
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1u;     
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0u;            
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
#endif
            
#ifdef GEC_SF_MASTER
            CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);                
            /* Enable CAN1 TX0 interrupt IRQ channel */
            NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1u;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2u;
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
            CAN_InitStructure.CAN_NART=DISABLE;			 
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	  
            CAN_InitStructure.CAN_TXFP=DISABLE;			 
            CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	         
            
            CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  
            CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
            CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;  
            
#if CAN_BAUDRATE == 1000 /* 1MBps */
            CAN_InitStructure.CAN_Prescaler = 6u;
#elif CAN_BAUDRATE == 500 /* 500KBps */
            CAN_InitStructure.CAN_Prescaler = 12u;
#elif CAN_BAUDRATE == 250 /* 250KBps */
            CAN_InitStructure.CAN_Prescaler = 24u;
#elif CAN_BAUDRATE == 125 /* 125KBps */
            CAN_InitStructure.CAN_Prescaler = 48u;
#elif  CAN_BAUDRATE == 100 /* 100KBps */
            CAN_InitStructure.CAN_Prescaler = 60u;
#elif  CAN_BAUDRATE == 50 /* 50KBps */
            CAN_InitStructure.CAN_Prescaler = 120u;
#elif  CAN_BAUDRATE == 20 /* 20KBps */
            CAN_InitStructure.CAN_Prescaler = 300u;
#elif  CAN_BAUDRATE == 10 /* 10KBps */
            CAN_InitStructure.CAN_Prescaler = 600u;
#else
#error "Please select first the CAN Baudrate in Private defines "
#endif  /* CAN_BAUDRATE == 1000 */           
            
            /* Initializes the CAN2 */
            CAN_Init(CANx, &CAN_InitStructure);        	 

            /* CAN2 filter init */
            CAN_FilterInitStructure.CAN_FilterNumber = 14u;	
            CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
            CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 

            /* any id */
            CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000u;	
            CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000u;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000u;
            CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000u;   
            
            /* ext id */
/*            
            CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x3456<<3)&0xFFFF0000)>>16;	
            CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x3456<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffff;//32 bit MASK
            CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xf807;  
            CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)CAN2RX_UP_ID << 3) & 0xFFFF0000 ) >> 16;	
            CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)CAN2RX_UP_ID << 3) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFF;
            CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((((u32)(~( CAN2RX_UP_ID ^ CAN2RX_DOWN_ID ))) << 3) & 0xFFFF0000) >> 16;
            CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((((u32)(~( CAN2RX_UP_ID ^ CAN2RX_DOWN_ID ))) << 3) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFF;             
*/            
            CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
            CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
            CAN_FilterInit(&CAN_FilterInitStructure);			
            
#if CAN2_RX0_INT_ENABLE 
            /* IT Configuration for CAN2 */ 
            CAN_ITConfig(CAN2,CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE); 						    

            NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1u;     
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1u;           
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
#endif             
            /* Enable CAN2 TX0 interrupt IRQ channel */
/*            
            CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE);                            
            NVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);             
*/            
        }
        else
        {}
#endif           
	return 0u;
}   


/*******************************************************************************
* Function Name  : CAN_RX_Process
* Description    : process the receive data.
*                  
* Input          : RxMessage: receive a CanRxMsg
*                  CanRx: define a CAN_RX_DATA_PROCESS_TypeDef struct to receive a frame data
* Output         : None
* Return         : None
*******************************************************************************/			    
void CAN_RX_Process( CanRxMsg RxMessage, CAN_RX_DATA_PROCESS_TypeDef* CanRx )
{
    
    u8 i;        
    
    if( ( CanRx->recving == 0u ) && ( RxMessage.Data[0] == 0xfau ) )
    {              
        CanRx->recv_len = RxMessage.Data[1] + 4u;  
        CanRx->mlen = CanRx->recv_len;
        CanRx->rxcnt = 0u;
        
        CanRx->recving = 1u;
    }
    
    if( CanRx->recving == 1u )
    {
        
        for( i = 0u; i < RxMessage.DLC; i++ )
        {
            /* receive data */
            CanRx->rx_buff[ CanRx->rxcnt++ ] = RxMessage.Data[i];
            
        }   
        
        if( CanRx->mlen > RxMessage.DLC )
        {
            CanRx->mlen -= RxMessage.DLC;    
        }
        else if( CanRx->mlen == RxMessage.DLC )
        {
            CanRx->recving = 0u;
            CanRx->data_packet = 1u;
        }
        else
        {
            CanRx->recving = 0u;
            CanRx->data_packet = 0u; 
        }
            
    }
    
}


/*******************************************************************************
* Function Name  : CAN1_RX0_IRQHandler
* Description    : This function handles CAN1 RX0 interrupt request.                
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if CAN1_RX0_INT_ENABLE	
			    
void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    u8 num;
    
    if( CAN_GetITStatus(CAN1,CAN_IT_FF0) != RESET)
    {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
    }
    else if( CAN_GetITStatus(CAN1,CAN_IT_FOV0) != RESET)
    {
        CAN_ClearITPendingBit(CAN1,CAN_IT_FOV0);
    }
    else if( CAN_GetITStatus(CAN1,CAN_IT_FMP0) != RESET)
    {
        
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
        
        /** CB control data RECEIVE **/
        if( ( RxMessage.ExtId == CAN1RX_CONTROL_DATA1_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can1_receive = 1u;        

            for( num = 0u; num < RxMessage.DLC; num++ )
            {
                pcEscDataFromControl[num] = RxMessage.Data[num];
            }
        }  
        /** CB normal data RECEIVE **/        
        else if( ( RxMessage.ExtId == CAN1RX_NORMAL_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can1_receive = 1u;        

            CAN_RX_Process( RxMessage, &CAN1_RX_Normal );
        }        
        /* Test Mode */        
        else if( ( RxMessage.ExtId == CAN1_TEST_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can1_receive = 1u;        
            
            CAN_RX_Process( RxMessage, &CAN1_RX_Normal );
        }  
        else
        {}
    }

    else
    {
#ifdef GEC_SF_S_NEW         
        USB_Istr();
#endif
    }
}
#endif


/*******************************************************************************
* Function Name  : CAN2_RX0_IRQHandler
* Description    : This function handles CAN2 RX0 interrupt request.                 
* Input          : None             
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
    else
    {
        
        CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
        /** DBL1 UP data RECEIVE **/
        if( ( RxMessage.ExtId == CAN2RX_UP_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can2_receive = 1u;        
  
            CAN_RX_Process( RxMessage, &CAN2_RX_Up );
            
        }
        /** DBL1 DOWN data RECEIVE **/
        else if( ( RxMessage.ExtId == CAN2RX_DOWN_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can2_receive = 1u;        

            CAN_RX_Process( RxMessage, &CAN2_RX_Down );
        }
        /* Test Mode */        
        else if( ( RxMessage.ExtId == CAN1_TEST_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can2_receive = 1u;        
            
            CAN_RX_Process( RxMessage, &CAN2_RX_Up );
        }  
        else
        {
        }
    }        
}
#endif


/*******************************************************************************
* Function Name  : BSP_CAN_Send
* Description    : CAN send a frame data.
*                  
* Input          : CANx: CAN1 or CAN2
*                  CanRx: define a CAN_TX_DATA_PROCESS_TypeDef struct to send a frame data
*                  send_id: Extended identifier ID
*                  buff: send data address
*                  len: want to send data len
* Output         : None
* Return         : None
*******************************************************************************/  
void BSP_CAN_Send(CAN_TypeDef* CANx, CAN_TX_DATA_PROCESS_TypeDef* CanTx, uint32_t send_id, uint8_t buff[], uint8_t len)
{

        u16 i;
        u8  result = 0u;	
        u8 j;
        
        if( len > canbuffsize ) 
        {
            /* error */		
        }
        else
        {
            /** packet the data pack ------------------------**/
            if( ( CanTx->sending == 0u ) && ( len > 0u ) )
            {               
                CanTx->mlen = len + 4u;
                
                CanTx->tx_buff[0] = 0xfau;
                CanTx->tx_buff[1] = CanTx->mlen - 4u;
                for( j = 0u; j < CanTx->mlen - 4u; j++ )
                {
                    CanTx->tx_buff[j+2u] = buff[j];
                }
                i = MB_CRC16( CanTx->tx_buff, (u16)CanTx->mlen - 2u );
                CanTx->tx_buff[CanTx->mlen - 2u] = (u8)i;
                CanTx->tx_buff[CanTx->mlen - 1u] = (u8)(i>>8u);    
                
                CanTx->p_CanBuff = &CanTx->tx_buff[0];
                CanTx->sending = 1u;
                
            } 
        }
        
        
        /** CAN send data ---------------------------------**/
        if( CanTx->sending == 1u )
        {
            
            if( CanTx->mlen > CAN_SEND_LEN )
            {
                for( i = 0u; i < 3u; i++ )
                {
                    result = Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CAN_FRAME_LEN ); 
                    if( result != 1u )
                    {
                        CanTx->p_CanBuff += CAN_FRAME_LEN;
                        CanTx->mlen -= CAN_FRAME_LEN;
                    }
                    
                }
            }
            else
            {
                if( CanTx->mlen > 2u*CAN_FRAME_LEN )
                {
                    for( i = 0u; i < 2u; i++ )
                    {
                        result = Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CAN_FRAME_LEN ); 
                        if( result != 1u )
                        {
                            CanTx->p_CanBuff += CAN_FRAME_LEN;
                            CanTx->mlen -= CAN_FRAME_LEN;
                        }
                    }   
                }
                else if( CanTx->mlen > CAN_FRAME_LEN )
                {
                    
                    result = Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CAN_FRAME_LEN ); 
                    if( result != 1u )
                    {
                        CanTx->p_CanBuff += CAN_FRAME_LEN;
                        CanTx->mlen -= CAN_FRAME_LEN;
                    }                                      
                }
                else
                {}
                
                if( CanTx->mlen <= CAN_FRAME_LEN )
                {
                    result = Can_Send_Msg(CANx, send_id, CanTx->p_CanBuff, CanTx->mlen );
                    if( result != 1u )
                    {
                        CanTx->mlen = 0u;
                        CanTx->sending = 0u;
                    }
                }
                
            }
#ifdef GEC_SF_MASTER
            CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
#endif
        } 
    
}

/*******************************************************************************
* Function Name  : CAN1_TX_IRQHandler
* Description    : This function handles CAN1 TX interrupt request.               
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
void CAN1_TX_IRQHandler(void)
{

    CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);
#ifdef GEC_SF_MASTER    
/*    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, CAN1_TX_Data, 0u);*/
    BSP_CAN_Send(CAN1, &CAN1_TX_Urge, CAN1TX_URGE_ID, CAN1_TX2_Data, 0u);
#endif   
    
}

/*******************************************************************************
* Function Name  : CAN2_TX_IRQHandler
* Description    : This function handles CAN2 TX interrupt request.                
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
#ifdef GEC_SF_MASTER
void CAN2_TX_IRQHandler(void)
{

    CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP0);

   
    
}
#endif

/*******************************************************************************
* Function Name  : BSP_CAN_Receive
* Description    : CAN reveive a frame data.
*                  
* Input          : CANx: CAN1 or CAN2
*                  CanRx: define a CAN_RX_DATA_PROCESS_TypeDef struct to receive a frame data
*                  buff: receive data address
*                  mlen: want to receive data len
* Output         : None
* Return         : Length of the received data
*******************************************************************************/   
uint8_t BSP_CAN_Receive(CAN_TypeDef* CANx,CAN_RX_DATA_PROCESS_TypeDef* CanRx, uint8_t buff[], uint8_t mlen)
{
    uint8_t *pstr;
    uint32_t i = 0u,len = 0u;
	
    switch (*(uint32_t*)&CANx)
    {
       case CAN1_BASE:
                       
        /** receive a data packet **/
        if( CanRx->data_packet == 1u )
        {
            if(!MB_CRC16(CanRx->rx_buff, (u16)CanRx->recv_len))
            {          
                /* ok */
                pstr = &CanRx->rx_buff[2];					
                len = CanRx->recv_len - 4u;
                CanRx->recv_len = 0u;
            }
            else
            {
                /* fail */
                for( i = 0u; i < CanRx->recv_len; i++ )
                {
                    CanRx->rx_buff[i] = 0u;
                }
            }
            CanRx->data_packet = 0u;
        }                         
        break;	
#ifdef GEC_SF_MASTER
       case CAN2_BASE: 
        
        /** receive a data packet **/
        if( CanRx->data_packet == 1u )
        {
            if(!MB_CRC16(CanRx->rx_buff, (u16)CanRx->recv_len))
            {          
                /* ok */
                pstr = &CanRx->rx_buff[2];					
                len = CanRx->recv_len - 4u;
                CanRx->recv_len = 0u;
            }
            else
            {
                /* fail */
                for( i = 0u; i < CanRx->recv_len; i++ )
                {
                    CanRx->rx_buff[i] = 0u;
                }
            }
            CanRx->data_packet = 0u;
        } 
        break;	
#endif
       default:
        break;
    }	

    
    if(mlen && (mlen<len))
    {
        len = mlen;
    }
    
    if(len > canbuffsize) 
    {
        len = 0u;
    }
    
    for(i = 0u; i < len; i++)
    {
        buff[i] = pstr[i];
    }		
			
    return(len);
}

/*******************************************************************************
* Function Name  : Can_Send_Msg
* Description    : CAN send data
*                  
* Input          : len: data len(max len is 8)
*                  msg: Data Pointer.
*                  exid: Extended identifier ID.
*                  CANx: CAN1 or CAN2
* Output         : None
* Return         : 0: success
*                  1: fail, no send mailbox 
*******************************************************************************/		 
u8 Can_Send_Msg(CAN_TypeDef* CANx,u32 exid,u8 msg[],u8 len)
{	
	u16 i = 0u;
        u8 result = 0u;        
	CanTxMsg TxMessage;
        
	TxMessage.StdId = 0x12u;			
	TxMessage.ExtId = exid;			
/*	TxMessage.IDE = CAN_Id_Standard; */
        TxMessage.IDE = CAN_Id_Extended; 	        
	TxMessage.RTR = CAN_RTR_Data;		
	TxMessage.DLC = len;			
	for(i = 0u; i < len; i++)
        {
          TxMessage.Data[i] = msg[i];
        }
	if( CAN_TxStatus_NoMailBox == CAN_Transmit(CANx, &TxMessage) )
        {
            result =  1u;
        }
        else
        {
            result =  0u;
        }
        
        return result;
}


/*******************************************************************************
* Function Name  : Can_Receive_Msg
* Description    : CAN receive data                 
* Input          : buf:data cache
* Output         : None
* Return         : 0: no data receive; 
*                  other: Length of the received data;
*******************************************************************************/
u8 Can_Receive_Msg(CAN_TypeDef* CANx,u8 buf[])
{		   		   
      u32 i;
      u8 result = 0u; 
      CanRxMsg RxMessage;
      
      if( CAN_MessagePending(CANx,CAN_FIFO0)== 0u )
      {
          result =  0u;		 
      }  
      else
      {          
          CAN_Receive(CANx, CAN_FIFO0, &RxMessage);
          
          for(i = 0u; i < RxMessage.DLC; i++)
          {
              buf[i] = RxMessage.Data[i];  
          }
          
          result = RxMessage.DLC;
      }
      return result;	
}


/******************************  END OF FILE  *********************************/














