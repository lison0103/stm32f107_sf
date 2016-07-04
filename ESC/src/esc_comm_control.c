/*******************************************************************************
* File Name          : esc_comm_control.c
* Author             : lison
* Version            : V1.0
* Date               : 06/20/2016
* Description        : This file contains esc safety board communication  with
*                      control board.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_comm_control.h"
#include "can.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Communication_To_Control
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Communication_To_Control(void)
{
    static u8 can1_comm_timeout = 0;
    u16 *pc_sfescstate = (u16*)&CAN1_TX2_Data[0];
    u8 len = 0;
    
    if( can1_receive == 1 )
    {
        can1_receive = 0;
        can1_comm_timeout = 0;
    }
    else if( ++can1_comm_timeout >= 3 )
    {
        /*  can communication timeout process */
    }  
     
    
    /** receive a data packet -----------------------------------------------**/ 
    len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
    
    /* just for test */
    if( len > 0 )
    {
        CAN1_TX_Data[4] = CAN1_RX_Data[0];
        CAN1_TX_Data[5] = CAN1_RX_Data[1];
    }
    
    for( u8 i = 6; i < 50; i++ )
    {
        CAN1_TX_Data[i] = EscRTBuff[30+i];
    }
    for( u8 i = 0; i < 10; i++ )
    {
        CAN1_TX_Data[50+i] = EscRTBuff[4+i];
        CAN1_TX_Data[60+i] = CAN2_RX_Data[i];//DBL1 UP
        CAN1_TX_Data[70+i] = CAN2_RX2_Data[i];//DBL1 DOWN
    }   
 
    
    *pc_sfescstate = SfBase_EscState;
    CAN1_TX2_Data[2] = EN_ERROR1;
    
    
    /** CAN1 send data ------------------------------------------------------**/
    /** CB normal SEND ID:0x1314, CB URGE SEND ID:0x1234 **/
    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, CAN1_TX_Data, 100);
    BSP_CAN_Send(CAN1, &CAN1_TX_Urge, CAN1TX_URGE_ID, CAN1_TX2_Data, 20);
    
}




/******************************  END OF FILE  *********************************/