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
#include "hw_test.h"

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
#if 0
void Communication_To_Control(void)
{
    static u8 can1_comm_timeout = 0u;
    u8 result;
    
    if( can1_receive == 1u )
    {
        can1_receive = 0u;
        can1_comm_timeout = 0u;
        EN_ERROR7 &= ~0x04u;
    }
    else if( ++can1_comm_timeout >= 3u )
    {
        /*  can communication timeout process */
        EN_ERROR7 |= 0x04u;
    }  
    else
    {}
    
    result = Can_Send_Msg(CAN1, CAN1TX_SAFETY_DATA_ID, pcEscDataToControl, CAN_FRAME_LEN ); 
    if( result )
    {
        /* No mail box, send fail */
    }    
    result = Can_Send_Msg(CAN1, CAN1TX_SAFETY_INPUT_ID, pcSafetyInputToControl, CAN_FRAME_LEN ); 
    if( result )
    {
        /* No mail box, send fail */
    }    
    
    
}

#else
void Communication_To_Control(void)
{
    static u8 can1_comm_timeout = 0u;
    u16 *pc_sfescstate = (u16*)&CAN1_TX2_Data[0];
    u8 len = 0u;
    u8 i;
    
    if( can1_receive == 1u )
    {
        can1_receive = 0u;
        can1_comm_timeout = 0u;
        EN_ERROR7 &= ~0x04u;
    }
    else if( ++can1_comm_timeout >= 3u )
    {
        /*  can communication timeout process */
        EN_ERROR7 |= 0x04u;
    }  
    else
    {}
     
    
    /** receive a data packet -----------------------------------------------**/ 
    len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0u);
    
    /* just for test */
    if( len > 0u )
    {
        CAN1_TX_Data[0] = SAFETY_SWITCH_STATUS[0];
        CAN1_TX_Data[1] = McRxBuff[80];
/*        CAN1_TX_Data[2] = 0;*/
/*        CAN1_TX_Data[3] = 0;  */      
        CAN1_TX_Data[4] = CAN1_RX_Data[0];
        CAN1_TX_Data[5] = CAN1_RX_Data[1];
    }
    
    for(  i = 6u; i < 50u; i++ )
    {
        CAN1_TX_Data[i] = EscRTBuff[30u+i] ;/* | McRxBuff[30+i];*/
    }
    for( i = 0u; i < 10u; i++ )
    {
        CAN1_TX_Data[50u+i] = EscRTBuff[4u+i] | McRxBuff[4u+i];
        CAN1_TX_Data[60u+i] = CAN2_RX_Data[i];/* DBL1 UP */
        CAN1_TX_Data[70u+i] = CAN2_RX2_Data[i];/* DBL1 DOWN */
    }   
    for( i = 80u; i < 124u; i++ )
    {
        CAN1_TX_Data[i] =  McRxBuff[ i - 44u ];
    } 
    
    *pc_sfescstate = SfBase_EscState;
    CAN1_TX2_Data[2] = pcEscErrorCodeBuff[0];
    
    
    if( testmode == 0u )
    {
      /** CAN1 send data ------------------------------------------------------**/
      /** CB normal SEND ID:0x1314, CB URGE SEND ID:0x1234 **/
      BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, CAN1_TX_Data, 125u);
      BSP_CAN_Send(CAN1, &CAN1_TX_Urge, CAN1TX_URGE_ID, CAN1_TX2_Data, 20u);
    }
    else if( testmode == 1u )
    {
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10u);
    }
    else
    {}
}
#endif



/******************************  END OF FILE  *********************************/