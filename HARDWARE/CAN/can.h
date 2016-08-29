/*******************************************************************************
* File Name          : can.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of can functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H
#define __CAN_H	 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
typedef struct  {
  u8 sending;
  u8 p_CanBuff;
  u8 mlen;
  u8 tx_buff[200];
} CAN_TX_DATA_PROCESS_TypeDef;

typedef struct  {
  u8 recving;
  u8 rxcnt;
  u8 mlen;
  u8 data_packet;
  u8 recv_cnt;
  u8 recv_len;
  u8 rx_buff[200];
} CAN_RX_DATA_PROCESS_TypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define canbuffsize 150u

#define CAN_FRAME_LEN   8u
#define CAN_SEND_LEN    3u*CAN_FRAME_LEN

/* SAFETY TO CONTROL  Speed: 500kbps  Message ID 100: transmitted every 20ms */
/* CONTROL TO SAFETY  Speed: 500kbps  Message ID 200: transmitted every 20ms */
#define CAN1TX_NORMAL_ID  0x0064u
#define CAN1TX_SAFETY_DATA_ID  0x0065u
#define CAN1TX_SAFETY_INPUT_ID  0x0066u
#define CAN1RX_NORMAL_ID  0x00C8u
#define CAN1RX_CONTROL_DATA1_ID  0x00C9u
#define CAN1RX_CONTROL_DATA2_ID  0x00CAu

#define CAN1TX_URGE_ID    0x0062u  

#define CAN2TX_UP_ID    0x1234u
#define CAN2RX_UP_ID    0x3456u
#define CAN2TX_DOWN_ID  0x2345u
#define CAN2RX_DOWN_ID  0x3488u
#define CAN1_TEST_ID  0xfaf1u

/* Exported functions ------------------------------------------------------- */
u8 CAN_Int_Init(CAN_TypeDef* CANx); 
static u8 Can_Send_Msg(CAN_TypeDef* CANx,u32 exid,u8 msg[],u8 len);			
u8 Can_Receive_Msg(CAN_TypeDef* CANx,u8 buf[]);					
void BSP_CAN_Send(CAN_TypeDef* CANx, CAN_TX_DATA_PROCESS_TypeDef* CanTx, uint32_t send_id, uint8_t buff[], uint8_t len);
uint8_t BSP_CAN_Receive(CAN_TypeDef* CANx,CAN_RX_DATA_PROCESS_TypeDef* CanRx, uint8_t buff[],uint8_t mlen);

/* CAN1 */
extern u8 CAN1_TX_Data[canbuffsize];
extern u8 CAN1_RX_Data[canbuffsize];
extern u8 CAN1_TX2_Data[canbuffsize];
extern u8 CAN1_RX2_Data[canbuffsize];
/* CAN2 */
extern u8 CAN2_TX_Data[canbuffsize];
extern u8 CAN2_RX_Data[canbuffsize];
extern u8 CAN2_TX2_Data[canbuffsize];
extern u8 CAN2_RX2_Data[canbuffsize];

extern u8 can1_receive;
extern u8 can2_receive;


extern CAN_TX_DATA_PROCESS_TypeDef  CAN1_TX_Normal;
extern CAN_TX_DATA_PROCESS_TypeDef  CAN1_TX_Urge;
extern CAN_TX_DATA_PROCESS_TypeDef  CAN2_TX_Up;
extern CAN_TX_DATA_PROCESS_TypeDef  CAN2_TX_Down;

extern CAN_RX_DATA_PROCESS_TypeDef  CAN1_RX_Normal;
extern CAN_RX_DATA_PROCESS_TypeDef  CAN2_RX_Up;
extern CAN_RX_DATA_PROCESS_TypeDef  CAN2_RX_Down;
				
#endif  /* __CAN_H */




/******************************  END OF FILE  *********************************/















