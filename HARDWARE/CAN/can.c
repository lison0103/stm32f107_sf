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
/* #define CAN_BAUDRATE  500 */ /* 500kBps */
#define CAN_BAUDRATE  250  /* 250kBps */
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
void CAN1_RX0_IRQHandler(void);
void CAN2_RX0_IRQHandler(void);
void CAN1_TX_IRQHandler(void);
void CAN2_TX_IRQHandler(void);
#ifdef GEC_SF_MASTER
static void Can_Receive_Buffer(u8 rxmsg[], u16 canid, u8 datatype);
#endif

u8 can1_receive = 0u;
#ifdef GEC_SF_MASTER
u8 can2_receive = 0u;
#endif

#ifdef GEC_SF_MASTER
volatile u16 g_u16DBL1NewData = 0u;
u16 g_u16ContorlNewData = 0u;
#ifdef DIAGNOSTIC_LEVEL2
u16 g_u16DBL2NewData = 0u;
#endif
u8 g_u8CAN2SendFail = 0u;


/* Esc receive data buffer */
static u8 EscDataFromControlBuffer[DATA_FROM_CONTROL_LEN][8];
static u8 EscDataFromDBL1Buffer[DATA_FROM_DBL1_LEN][8];
#ifdef DIAGNOSTIC_LEVEL2
static u8 EscDataFromDBL2Buffer[DATA_FROM_DBL2_LEN][8];
#endif
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
            CAN_InitStructure.CAN_ABOM=ENABLE;
            /* wake-sleep mode via software (Clear CAN-> MCR's SLEEP bit) */
            CAN_InitStructure.CAN_AWUM=ENABLE;			
            /* message is automatically transferred, in accordance with the CAN standard, */
            /* CAN hardware failure when sending packets would have been automatic retransmission until sent successfully */
            CAN_InitStructure.CAN_NART=DISABLE;	
            /* message is not locked, the new over the old one */
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	
            /* priority is determined by the packet identifier */
            CAN_InitStructure.CAN_TXFP=ENABLE;			
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
            CAN_ITConfig(CAN1,CAN_IT_FMP0 , ENABLE); 		    
            CAN_ITConfig(CAN1,CAN_IT_FF0, ENABLE); 		    
            CAN_ITConfig(CAN1,CAN_IT_FOV0, ENABLE); 		    
#ifdef GEC_SF_MASTER
            NVIC_InitStructure.NVIC_IRQChannel = (u8)CAN1_RX0_IRQn;
#else
            NVIC_InitStructure.NVIC_IRQChannel = (u8)USB_LP_CAN1_RX0_IRQn;
#endif
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1u;     
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0u;            
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
#endif
            
#ifdef GEC_SF_MASTER
            CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);                
            /* Enable CAN1 TX0 interrupt IRQ channel */
            NVIC_InitStructure.NVIC_IRQChannel = (u8)CAN1_TX_IRQn;
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
            CAN_InitStructure.CAN_ABOM=ENABLE;				 
            CAN_InitStructure.CAN_AWUM=ENABLE;			
            CAN_InitStructure.CAN_NART=DISABLE;			 
            CAN_InitStructure.CAN_RFLM=DISABLE;		 	  
            CAN_InitStructure.CAN_TXFP=ENABLE;			 
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
            CAN_ITConfig(CAN2,CAN_IT_FMP0, ENABLE); 
            CAN_ITConfig(CAN2,CAN_IT_FF0, ENABLE); 						    
            CAN_ITConfig(CAN2,CAN_IT_FOV0, ENABLE); 						    

            NVIC_InitStructure.NVIC_IRQChannel = (u8)CAN2_RX0_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1u;     
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1u;           
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
#endif             
            /* Enable CAN2 TX0 interrupt IRQ channel */            
            CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE);                            
            NVIC_InitStructure.NVIC_IRQChannel = (u8)CAN2_TX_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1u;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2u;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);                         
        }
        else
        {}
#endif           
	return 0u;
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
    u8 i;
    
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
#ifdef GEC_SF_MASTER        
        /** CB control data RECEIVE **/        
        if(( RxMessage.ExtId >= CAN1RX_CONTROL_DATA1_ID ) && ( RxMessage.ExtId <= CAN1RX_CONTROL_DATA2_ID ))
        {
            if( ( RxMessage.DLC == CAN_FRAME_LEN ) && ( RxMessage.IDE == CAN_ID_EXT ))
            {
                can1_receive = 1u;            
                Can_Receive_Buffer(RxMessage.Data, (u16)RxMessage.ExtId, DATA_FROM_CONTROL);
            }
        }   
        if(( RxMessage.ExtId >= CAN1RX_PARA_STATUS_ID ) && ( RxMessage.ExtId <= CAN1RX_PARA_CB_IN_SAFETY_ID2 ))
        {
            if( ( RxMessage.DLC == CAN_FRAME_LEN ) && ( RxMessage.IDE == CAN_ID_EXT ))
            {
                can1_receive = 1u;            
                for( i = 0u; i < 8u; i++ )
                {
                    ControlNeedInSafetyFile[RxMessage.ExtId - CAN1RX_PARA_STATUS_ID][i] = RxMessage.Data[i];
                } 
            }
        }          
        /* Test Mode */        
        else if( ( RxMessage.ExtId == CAN1_TEST_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can1_receive = 1u;        
            
        }  
        else
        {}
#endif        
        CAN_FIFORelease(CAN1,CAN_FIFO0);
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
        
        /** DBL1 data RECEIVE **/ 
        if( DIAGNOSTIC == DIAGNOSTIC_BOARD_1 )
        {
            if(( RxMessage.ExtId >= CAN2RX_DBL1_UPPER_ID ) && ( RxMessage.ExtId <= CAN2RX_DBL1_INTERM2_ID ))
            {
                if( ( RxMessage.DLC == CAN_FRAME_LEN ) && ( RxMessage.IDE == CAN_ID_EXT ))
                {
                    can2_receive = 1u;            
                    Can_Receive_Buffer(RxMessage.Data, (u16)RxMessage.ExtId, DATA_FROM_DBL1);
                }
            } 
        }
#ifdef DIAGNOSTIC_LEVEL2        
        /** DBL2 data RECEIVE **/    
        else if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
        {
            if((( RxMessage.ExtId >= CAN2RX_DBL2_UPPER_ID1 ) && ( RxMessage.ExtId <= CAN2RX_DBL2_UPPER_ID3 )) 
               || (( RxMessage.ExtId >= CAN2RX_DBL2_LOWER_ID1 ) && ( RxMessage.ExtId <= CAN2RX_DBL2_LOWER_ID3 )) 
                   || (( RxMessage.ExtId >= CAN2RX_DBL2_INTERM1_ID1 ) && ( RxMessage.ExtId <= CAN2RX_DBL2_INTERM1_ID3 ))
                       || (( RxMessage.ExtId >= CAN2RX_DBL2_INTERM2_ID1 ) && ( RxMessage.ExtId <= CAN2RX_DBL2_INTERM2_ID3 ))
                           || (( RxMessage.ExtId >= CAN2RX_DBL2_UPPER_NONSAFETY_ID ) && ( RxMessage.ExtId <= CAN2RX_DBL2_INTERM2_NONSAFETY_ID )))               
            {
                if( ( RxMessage.DLC == CAN_FRAME_LEN ) && ( RxMessage.IDE == CAN_ID_EXT ))
                {
                    can2_receive = 1u;            
                    Can_Receive_Buffer(RxMessage.Data, (u16)RxMessage.ExtId, DATA_FROM_DBL2);
                }
            }         
        }
#endif        
        /* Test Mode */        
        else if( ( RxMessage.ExtId == CAN1_TEST_ID ) && ( RxMessage.IDE == CAN_ID_EXT ) )
        {
            can2_receive = 1u;                    
        }  
        else
        {
        }
        
        CAN_FIFORelease(CAN2,CAN_FIFO0);
    }
}
#endif



/*******************************************************************************
* Function Name  : CAN1_TX_IRQHandler
* Description    : This function handles CAN1 TX interrupt request.               
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
void CAN1_TX_IRQHandler(void)
{
    u8 result;
    
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP0);
    CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);
    
#ifdef GEC_SF_MASTER    
    
    result = Can_Send_Msg(CAN1, SafetyCommToControlID[g_u8CanCommunicationToCotrolID], 
                          &EscDataToControlBuffer[SafetyCommToControlID[g_u8CanCommunicationToCotrolID] - CAN1TX_SAFETY_DATA_ID1][0], CAN_FRAME_LEN ); 
    if( result )
    {
        /* No mail box, send fail */
        CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
    }  
    else
    {
        g_u8CanCommunicationToCotrolID++;
        if( g_u8CanCommunicationToCotrolID <= g_u8CanCommunicationToCotrolLen )
        {
            CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
        }
        else
        {
            g_u8CanCommunicationToCotrolOk = 1u;
        }
    }
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
    u8 result;
    
    CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP0);
    CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE);
   
    if( DIAGNOSTIC == DIAGNOSTIC_BOARD_1 )
    {
        if( g_u8CAN2SendFail & 0x01u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL1_ID, &EscDataToDBL1[0][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x01u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }
            else
            {
                g_u8CAN2SendFail &= ~0x01u;
            }
        }        
    }
#ifdef DIAGNOSTIC_LEVEL2    
    else if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
    {
        if( g_u8CAN2SendFail & 0x01u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_UPPER_ID1, &EscDataToDBL2[0][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x01u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);               
            }
            else
            {
                g_u8CAN2SendFail &= ~0x01u;
            }
        }
        
        if( g_u8CAN2SendFail & 0x02u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_UPPER_ID2, &EscDataToDBL2[1][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x02u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);        
            } 
            else
            {
                g_u8CAN2SendFail &= ~0x02u;
            }
        }
        
        if( g_u8CAN2SendFail & 0x04u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_LOWER_ID1, &EscDataToDBL2[2][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x04u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);
            } 
            else
            {
                g_u8CAN2SendFail &= ~0x04u;
            }
        }
        
        if( g_u8CAN2SendFail & 0x08u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_LOWER_ID2, &EscDataToDBL2[3][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x08u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            else
            {
                g_u8CAN2SendFail &= ~0x08u;
            }
        } 
        
        if( g_u8CAN2SendFail & 0x10u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM1_ID1, &EscDataToDBL2[4][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x10u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            else
            {
                g_u8CAN2SendFail &= ~0x10u;
            }
        }    
        
        if( g_u8CAN2SendFail & 0x20u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM1_ID2, &EscDataToDBL2[5][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x20u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            else
            {
                g_u8CAN2SendFail &= ~0x20u;
            }
        } 

        if( g_u8CAN2SendFail & 0x40u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM2_ID1, &EscDataToDBL2[6][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x40u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            else
            {
                g_u8CAN2SendFail &= ~0x40u;
            }
        } 

        if( g_u8CAN2SendFail & 0x80u )
        {
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM2_ID2, &EscDataToDBL2[7][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u8CAN2SendFail |= 0x80u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            else
            {
                g_u8CAN2SendFail &= ~0x80u;
            }
        }         
    }
    else
    {}
#endif    
}
#endif


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

#ifdef GEC_SF_MASTER
/*******************************************************************************
* Function Name  : Can_Clean_Buffer
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void Can_Clean_Buffer(u16 canid, u8 datatype)
{	  
    u8 i;

    switch( datatype )
    {
       case DATA_FROM_CONTROL:
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromControlBuffer[canid - CAN1RX_CONTROL_DATA1_ID][i] = 0u;
            }
            break;
        }
       case DATA_FROM_DBL1:
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1Buffer[canid - CAN2RX_DBL1_UPPER_ID][i] = 0u;
            }
            break;
        }
#ifdef DIAGNOSTIC_LEVEL2        
       case DATA_FROM_DBL2:
        {   
            if(( canid >= CAN2RX_DBL2_UPPER_ID1 ) && ( canid <= CAN2RX_DBL2_UPPER_ID3 )) 
            {
                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL2Buffer[canid - CAN2RX_DBL2_UPPER_ID1][i] = 0u;
                }
            }
            else if(( canid >= CAN2RX_DBL2_LOWER_ID1 ) && ( canid <= CAN2RX_DBL2_LOWER_ID3 )) 
            {
                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL2Buffer[canid - CAN2RX_DBL2_LOWER_ID1 + 4u][i] = 0u;
                }
            }
            else if(( canid >= CAN2RX_DBL2_INTERM1_ID1 ) && ( canid <= CAN2RX_DBL2_INTERM1_ID3 )) 
            {
                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL2Buffer[canid - CAN2RX_DBL2_INTERM1_ID1 + 8u][i] = 0u;
                }
            }
            else if(( canid >= CAN2RX_DBL2_INTERM2_ID1 ) && ( canid <= CAN2RX_DBL2_INTERM2_ID3 )) 
            {
                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL2Buffer[canid - CAN2RX_DBL2_INTERM2_ID1 + 12u][i] = 0u;
                }
            }
            else if(( canid >= CAN2RX_DBL2_UPPER_NONSAFETY_ID ) && ( canid <= CAN2RX_DBL2_INTERM2_NONSAFETY_ID )) 
            {
                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL2Buffer[(canid - CAN2RX_DBL2_UPPER_NONSAFETY_ID) * 4u + 3u][i] = 0u;
                }
            }  
            else
            {}    
            break;
        }  
#endif        
       default:
        break;
    }
}

/*******************************************************************************
* Function Name  : Can_Receive_Buffer
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Can_Receive_Buffer(u8 rxmsg[], u16 canid, u8 datatype)
{	  
    u8 i;

    switch( datatype )
    {
       case DATA_FROM_CONTROL:
        {
            if( canid == CAN1RX_CONTROL_DATA1_ID )
            {
                g_u16ContorlNewData |= 0x0001u;
            }
            else if( canid == CAN1RX_CONTROL_DATA2_ID )
            {
                g_u16ContorlNewData |= 0x0002u;
            }
            else
            {}
            
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromControlBuffer[canid - CAN1RX_CONTROL_DATA1_ID][i] = rxmsg[i];
            }
            break;
        }
       case DATA_FROM_CONTROL_PARA:
        {

            break;        
        }
       case DATA_FROM_DBL1:
        {
            if( canid == CAN2RX_DBL1_UPPER_ID )
            {
                g_u16DBL1NewData |= 0x0001u;
            }
            else if( canid == CAN2RX_DBL1_LOWER_ID )
            {
                g_u16DBL1NewData |= 0x0002u;
            }
            else if( canid == CAN2RX_DBL1_INTERM1_ID )
            {
                g_u16DBL1NewData |= 0x0004u;
            }
            else if( canid == CAN2RX_DBL1_INTERM2_ID )
            {
                g_u16DBL1NewData |= 0x0008u;
            }            
            else
            {
            }            
            
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1Buffer[canid - CAN2RX_DBL1_UPPER_ID][i] = rxmsg[i];
            }
            break;
        }
#ifdef DIAGNOSTIC_LEVEL2        
       case DATA_FROM_DBL2:
        {   
            if(( canid == CAN2RX_DBL2_UPPER_ID1 ) || ( canid == CAN2RX_DBL2_UPPER_ID2 ) 
               || ( canid == CAN2RX_DBL2_UPPER_ID3 ) || ( canid == CAN2RX_DBL2_UPPER_NONSAFETY_ID )) 
            {
                if( canid == CAN2RX_DBL2_UPPER_ID1 )
                {
                    g_u16DBL2NewData |= 0x0001u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[0][i] = rxmsg[i];
                    } 
                }
                else if( canid == CAN2RX_DBL2_UPPER_ID2 )
                {
                    g_u16DBL2NewData |= 0x0002u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[1][i] = rxmsg[i];
                    }                   
                }
                else if( canid == CAN2RX_DBL2_UPPER_ID3 )
                {
                    g_u16DBL2NewData |= 0x0004u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[2][i] = rxmsg[i];
                    }                   
                }
                else if( canid == CAN2RX_DBL2_UPPER_NONSAFETY_ID )
                {
                    g_u16DBL2NewData |= 0x0008u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[3][i] = rxmsg[i];
                    }                    
                }                
                else
                {
                }
            }
            else if(( canid == CAN2RX_DBL2_LOWER_ID1 ) || ( canid == CAN2RX_DBL2_LOWER_ID2 ) 
                    || ( canid == CAN2RX_DBL2_LOWER_ID3 ) || ( canid == CAN2RX_DBL2_LOWER_NONSAFETY_ID )) 
            {                
                if( canid == CAN2RX_DBL2_LOWER_ID1 )
                {
                    g_u16DBL2NewData |= 0x0010u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[4][i] = rxmsg[i];
                    }                     
                }
                else if( canid == CAN2RX_DBL2_LOWER_ID2 )
                {
                    g_u16DBL2NewData |= 0x0020u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[5][i] = rxmsg[i];
                    }                       
                }
                else if( canid == CAN2RX_DBL2_LOWER_ID3 )
                {
                    g_u16DBL2NewData |= 0x0040u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[6][i] = rxmsg[i];
                    }                    
                }
                else if( canid == CAN2RX_DBL2_LOWER_NONSAFETY_ID )
                {
                    g_u16DBL2NewData |= 0x0080u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[7][i] = rxmsg[i];
                    }                   
                }                
                else
                {
                }                
            }
            else if(( canid == CAN2RX_DBL2_INTERM1_ID1 ) || ( canid == CAN2RX_DBL2_INTERM1_ID2 )
                    || ( canid == CAN2RX_DBL2_INTERM1_ID3 ) || ( canid == CAN2RX_DBL2_INTERM1_NONSAFETY_ID )) 
            {
                if( canid == CAN2RX_DBL2_INTERM1_ID1 )
                {
                    g_u16DBL2NewData |= 0x0100u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[8][i] = rxmsg[i];
                    }                    
                }
                else if( canid == CAN2RX_DBL2_INTERM1_ID2 )
                {
                    g_u16DBL2NewData |= 0x0200u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[9][i] = rxmsg[i];
                    }                    
                }
                else if( canid == CAN2RX_DBL2_INTERM1_ID3 )
                {
                    g_u16DBL2NewData |= 0x0400u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[10][i] = rxmsg[i];
                    }                    
                }
                else if( canid == CAN2RX_DBL2_INTERM1_NONSAFETY_ID )
                {
                    g_u16DBL2NewData |= 0x0800u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[11][i] = rxmsg[i];
                    }                    
                }                
                else
                {}                  
            }
            else if(( canid == CAN2RX_DBL2_INTERM2_ID1 ) || ( canid == CAN2RX_DBL2_INTERM2_ID2 )
                    || ( canid == CAN2RX_DBL2_INTERM2_ID3 ) || ( canid == CAN2RX_DBL2_INTERM2_NONSAFETY_ID )) 
            {
                if( canid == CAN2RX_DBL2_INTERM2_ID1 )
                {
                    g_u16DBL2NewData |= 0x1000u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[12][i] = rxmsg[i];
                    }                     
                }
                else if( canid == CAN2RX_DBL2_INTERM2_ID2 )
                {
                    g_u16DBL2NewData |= 0x2000u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[13][i] = rxmsg[i];
                    }                     
                }
                else if( canid == CAN2RX_DBL2_INTERM2_ID3 )
                {
                    g_u16DBL2NewData |= 0x4000u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[14][i] = rxmsg[i];
                    }                     
                }
                else if( canid == CAN2RX_DBL2_INTERM2_NONSAFETY_ID )
                {
                    g_u16DBL2NewData |= 0x8000u;
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2Buffer[15][i] = rxmsg[i];
                    }                     
                }                
                else
                {}                                 
            }
            else
            {}    
            break;
        }  
#endif        
       default:
        break;
    }
}

/*******************************************************************************
* Function Name  : Can_Receive_Data
* Description    :                 
* Input          : datatype: receive data type. 
* Output         : None
* Return         : None 
*******************************************************************************/
void Can_Receive_Data(u8 datatype)
{	  
    u8 i,j;
    
    switch( datatype )
    {
       case DATA_FROM_CONTROL:
        {
            if( g_u16ContorlNewData & 0x0001u )
            {
                g_u16ContorlNewData &= ~0x0001u;

                for( i = 0u; i < 8u; i++ )
                {
                    EscRtData.DataFromControl[0][i] = EscDataFromControlBuffer[0][i];
                    EscDataFromControlBuffer[0][i] = 0u;
                }  
            }

            if( g_u16ContorlNewData & 0x0002u )
            {
                g_u16ContorlNewData &= ~0x0002u;

                for( i = 0u; i < 8u; i++ )
                {
                    EscRtData.DataFromControl[1][i] = EscDataFromControlBuffer[1][i];
                    EscDataFromControlBuffer[1][i] = 0u;
                }  
            }
            
            break;
        }
       case DATA_FROM_DBL1:
        {
            /* receive data */
            if( g_u16DBL1NewData & 0x0001u )
            {
                g_u16DBL1NewData &= ~0x0001u;

                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL1[0][i] = EscDataFromDBL1Buffer[0][i];
                    EscDataFromDBL1Buffer[0][i] = 0u;
                }  
            }
            
            if( g_u16DBL1NewData & 0x0002u )
            {
                g_u16DBL1NewData &= ~0x0002u;

                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL1[1][i] = EscDataFromDBL1Buffer[1][i];
                    EscDataFromDBL1Buffer[1][i] = 0u;
                }  
            }
            
            if( g_u16DBL1NewData & 0x0004u )
            {
                g_u16DBL1NewData &= ~0x0004u;

                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL1[2][i] = EscDataFromDBL1Buffer[2][i];
                    EscDataFromDBL1Buffer[2][i] = 0u;
                }  
            } 
            
            if( g_u16DBL1NewData & 0x0008u )
            {
                g_u16DBL1NewData &= ~0x0008u;

                for( i = 0u; i < 8u; i++ )
                {
                    EscDataFromDBL1[3][i] = EscDataFromDBL1Buffer[3][i];
                    EscDataFromDBL1Buffer[3][i] = 0u;
                }  
            }          
            break;
        }
#ifdef DIAGNOSTIC_LEVEL2        
       case DATA_FROM_DBL2:
        {  
            if( ( g_u16DBL2NewData & 0x0007u ) == 0x0007u )
            {
                /*g_u16DBL2NewData &= (u16)(~0x0007u);*/
                for( j = 0u; j < 3u; j++ )
                {
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[j][i] = EscDataFromDBL2Buffer[j][i];
                        /*EscDataFromDBL2Buffer[j][i] = 0u;*/
                    }  
                } 
            }
            if( ( g_u16DBL2NewData & 0x0070u ) == 0x0070u )
            {
                /*g_u16DBL2NewData &= (u16)(~0x0070u);*/
                for( j = 4u; j < 7u; j++ )
                {
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[j][i] = EscDataFromDBL2Buffer[j][i];
                        /*EscDataFromDBL2Buffer[j][i] = 0u;*/
                    }  
                } 
            }
            if( ( g_u16DBL2NewData & 0x0700u ) == 0x0700u )
            {
                /*g_u16DBL2NewData &= (u16)(~0x0700u);*/
                for( j = 8u; j < 11u; j++ )
                {
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[j][i] = EscDataFromDBL2Buffer[j][i];
                        /*EscDataFromDBL2Buffer[j][i] = 0u;*/
                    }  
                } 
            }
            if( ( g_u16DBL2NewData & 0x7000u ) == 0x7000u )
            {
                /*g_u16DBL2NewData &= (u16)(~0x7000u);*/
                for( j = 12u; j < 15u; j++ )
                {
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[j][i] = EscDataFromDBL2Buffer[j][i];
                        /*EscDataFromDBL2Buffer[j][i] = 0u;*/
                    }  
                } 
            }            
            if( g_u16DBL2NewData & 0x8888u )
            {
                if( g_u16DBL2NewData & 0x0008u )
                {
                    /*g_u16DBL2NewData &= (u16)(~0x0008u);*/
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[3][i] = EscDataFromDBL2Buffer[3][i];
                    }  
                }
                if( g_u16DBL2NewData & 0x0080u )
                {
                    /*g_u16DBL2NewData &= (u16)(~0x0080u);*/
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[7][i] = EscDataFromDBL2Buffer[7][i];
                    }  
                }
                if( g_u16DBL2NewData & 0x0800u )
                {
                    /*g_u16DBL2NewData &= (u16)(~0x0800u);*/
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[11][i] = EscDataFromDBL2Buffer[11][i];
                    }  
                }
                if( g_u16DBL2NewData & 0x8000u )
                {
                    /*g_u16DBL2NewData &= (u16)(~0x8000u);*/
                    for( i = 0u; i < 8u; i++ )
                    {
                        EscDataFromDBL2[15][i] = EscDataFromDBL2Buffer[15][i];
                    }  
                }
            }            
            break;
        } 
#endif        
       default:
        break;
    }
}
#endif
/******************************  END OF FILE  *********************************/














