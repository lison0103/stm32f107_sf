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
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define CAN_FRAME_LEN   8u
#define CAN_SEND_LEN    3u*CAN_FRAME_LEN

/* SAFETY TO CONTROL  Speed: 250kbps  : transmitted every 20ms/50ms/100ms */
/* CONTROL TO SAFETY  Speed: 250kbps  : transmitted every 20ms */

#define CAN1_TEST_ID  0xfaf1u

/* SF <---> CB */
#define CAN1TX_SAFETY_DATA_ID1  0x0100u
#define CAN1TX_SAFETY_DATA_ID2  0x0101u
#define CAN1TX_SAFETY_DATA_ID3  0x0102u
#define CAN1TX_SAFETY_DATA_ID4  0x0103u
#define CAN1TX_SAFETY_DATA_ID5  0x0104u
#define CAN1TX_SAFETY_DATA_ID6  0x0105u
#define CAN1TX_SAFETY_DATA_ID7  0x0106u
#define CAN1TX_SAFETY_DATA_ID8  0x0107u
#define CAN1TX_SAFETY_DATA_ID9  0x0108u
#define CAN1TX_SAFETY_DATA_ID10  0x0109u
#define CAN1TX_SAFETY_DATA_ID11  0x010Au
#define CAN1TX_SAFETY_DATA_ID12  0x010Bu
#define CAN1TX_SAFETY_DATA_ID13  0x010Cu
#define CAN1TX_SAFETY_DATA_ID14  0x010Du
#define CAN1TX_SAFETY_DATA_ID15  0x010Eu
#define CAN1TX_SAFETY_DATA_ID16  0x010Fu
#define CAN1TX_SAFETY_DATA_ID17  0x0110u
#define CAN1TX_SAFETY_DATA_ID18  0x0111u
#define CAN1TX_SAFETY_DATA_ID19  0x0112u
#define CAN1RX_CONTROL_DATA1_ID  0x0200u
#define CAN1RX_CONTROL_DATA2_ID  0x0202u

/* SF <---> DBL1 */
#define CAN2TX_DBL1_ID  0xD0u
#define CAN2RX_DBL1_UPPER_ID  0xD1u
#define CAN2RX_DBL1_LOWER_ID  0xD2u
#define CAN2RX_DBL1_INTERM1_ID  0xD3u
#define CAN2RX_DBL1_INTERM2_ID  0xD4u

#ifdef DIAGNOSTIC_LEVEL2
/* SF <---> DBL2 */
#define CAN2TX_DBL2_UPPER_ID1  0x20u
#define CAN2TX_DBL2_UPPER_ID2  0x21u
#define CAN2TX_DBL2_LOWER_ID1  0x24u
#define CAN2TX_DBL2_LOWER_ID2  0x25u
#define CAN2TX_DBL2_INTERM1_ID1  0x28u
#define CAN2TX_DBL2_INTERM1_ID2  0x29u
#define CAN2TX_DBL2_INTERM2_ID1  0x2Cu
#define CAN2TX_DBL2_INTERM2_ID2  0x2Du

#define CAN2RX_DBL2_UPPER_ID1  0x40u
#define CAN2RX_DBL2_UPPER_ID2  0x41u
#define CAN2RX_DBL2_UPPER_ID3  0x42u
#define CAN2RX_DBL2_LOWER_ID1  0x64u
#define CAN2RX_DBL2_LOWER_ID2  0x65u
#define CAN2RX_DBL2_LOWER_ID3  0x66u
#define CAN2RX_DBL2_INTERM1_ID1  0x88u
#define CAN2RX_DBL2_INTERM1_ID2  0x89u
#define CAN2RX_DBL2_INTERM1_ID3  0x8Au
#define CAN2RX_DBL2_INTERM2_ID1  0xACu
#define CAN2RX_DBL2_INTERM2_ID2  0xADu
#define CAN2RX_DBL2_INTERM2_ID3  0xAEu

#define CAN2RX_DBL2_UPPER_NONSAFETY_ID  0xC1u
#define CAN2RX_DBL2_LOWER_NONSAFETY_ID  0xC2u
#define CAN2RX_DBL2_INTERM1_NONSAFETY_ID  0xC3u
#define CAN2RX_DBL2_INTERM2_NONSAFETY_ID  0xC4u
#endif

#define DATA_FROM_CONTROL       1u
#define DATA_FROM_DBL1          2u
#ifdef DIAGNOSTIC_LEVEL2
#define DATA_FROM_DBL2          3u
#endif

/* Exported functions ------------------------------------------------------- */
u8 CAN_Int_Init(CAN_TypeDef* CANx); 
u8 Can_Send_Msg(CAN_TypeDef* CANx,u32 exid,u8 msg[],u8 len);			
u8 Can_Receive_Msg(CAN_TypeDef* CANx,u8 buf[]);					
void Can_Receive_Data(u8 datatype);
void Can_Clean_Buffer(u16 canid, u8 datatype);

extern u8 can1_receive;
extern u8 can2_receive;
				
#endif  /* __CAN_H */




/******************************  END OF FILE  *********************************/















