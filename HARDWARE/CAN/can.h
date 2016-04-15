/*******************************************************************************
* File Name          : can.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H
#define __CAN_H	 

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u8 CAN_Mode_Init(CAN_TypeDef* CANx,u8 mode);//CAN初始化 
u8 Can_Send_Msg(CAN_TypeDef* CANx,u32 exid,u8* msg,u8 len);			//发送数据
u8 Can_Receive_Msg(CAN_TypeDef* CANx,u8 *buf);					//接收数据

extern u8 can1_receive;
extern u8 can2_receive;
	 										 							 				    
				
#endif  /* __CAN_H */




/******************************  END OF FILE  *********************************/















