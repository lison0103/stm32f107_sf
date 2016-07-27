/*******************************************************************************
* File Name          : esc_comm_diagnostic.c
* Author             : lison
* Version            : V1.0
* Date               : 06/23/2016
* Description        : This file contains esc safety board communication  with
*                      diagnostic board.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_comm_diagnostic.h"
#include "can.h"
#include "hw_test.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Safety_Comm_Diag
* Description    : CAN Communication.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Safety_Comm_Diag(void)
{
#ifdef GEC_SF_MASTER    
    static u8 can2_comm_timeout = 0;
    u8 len = 0;
    
    if( can2_receive == 1 )
    {
        can2_receive = 0;
        can2_comm_timeout = 0;
        EN_ERROR7 &= ~0x08;
    }
    else if( ++can2_comm_timeout >= 3 )
    {
        /*  can communication timeout process */
        EN_ERROR7 |= 0x08;
    }  

    len = BSP_CAN_Receive(CAN2, &CAN2_RX_Up, CAN2_RX_Data, 0);
    
    /* DBL1 UP, just for test */
    if( len > 0 )
    {
        CAN2_TX_Data[0] = 0x11;
        CAN2_TX_Data[1] = 0x33;
    }
    else
    {
        CAN2_RX_Data[0] = 0x00;
        CAN2_RX_Data[1] = 0x00;      
    }
  
    len = BSP_CAN_Receive(CAN2, &CAN2_RX_Down, CAN2_RX2_Data, 0);
    
    /* DBL1 DOWN, just for test */
    if( len > 0 )
    {
        CAN2_TX2_Data[0] = 0x22;
        CAN2_TX2_Data[1] = 0x55;
    }
    else
    {
        CAN2_RX2_Data[0] = 0x00;
        CAN2_RX2_Data[1] = 0x00;      
    }    
    
    if( testmode == 0 )
    {
        /** CAN2 send data ------------------------------------------------------**/
        /** DBL1 UP SEND ID:0x1234, DBL1 DOWN SEND ID:0x2345 **/
        BSP_CAN_Send(CAN2, &CAN2_TX_Up, CAN2TX_UP_ID, CAN2_TX_Data, 8);   
        BSP_CAN_Send(CAN2, &CAN2_TX_Down, CAN2TX_DOWN_ID, CAN2_TX2_Data, 8);  
    }
    else if( testmode == 1 )
    {
        BSP_CAN_Send(CAN2, &CAN2_TX_Up, CAN1_TEST_ID, CAN2_TX_Data, 8);   
    }
    
#else
    static u8 can1_comm_timeout = 0;
    u8 len = 0;
    
    /* if config tandem, use cpu2 CAN1 to communication */
    if( TANDEM_TYPE != 0 )
    {
        if( can1_receive == 1 )
        {
            can1_receive = 0;
            can1_comm_timeout = 0;
            EN_ERROR7 &= ~0x10;
        }
        else if( ++can1_comm_timeout >= 3 )
        {
            /*  can communication timeout process */
            EN_ERROR7 |= 0x10;
        }  
        
        /** receive a data packet -----------------------------------------------**/ 
        len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
        
        /* for tandem communication */
        if( len > 0 )
        {
            TandemMessageRunAllowed = 0;
            
            if( SfBase_EscState & ESC_STATE_READY )
            {
                TandemMessageRunAllowed = CAN1_RX_Data[1];
            }
            else if( SfBase_EscState & ESC_STATE_RUNNING )
            {
                TandemMessageRunAllowed = CAN1_RX_Data[2];
            }

        }
        CAN1_TX_Data[0] = TANDEM_TYPE;
        CAN1_TX_Data[1] = TandemRunEnable;
        CAN1_TX_Data[2] = Tandemoutput;    
        
        /** CAN1 send data ------------------------------------------------------**/
        /** tandem normal SEND ID:0x1314 **/
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, CAN1_TX_Data, 20);   
    }
    
    if( testmode == 1 )
    {
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10);
    }
    
#endif
}



/******************************  END OF FILE  *********************************/