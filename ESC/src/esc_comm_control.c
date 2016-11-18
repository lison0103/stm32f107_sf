/*******************************************************************************
* File Name          : esc_comm_control.c
* Author             : lison
* Version            : V1.0
* Date               : 06/20/2016
* Last modify date   : 10/25/2016
* Description        : This file contains esc safety board communication  with
*                      control board.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_comm_control.h"
#include "can.h"
#include "hw_test.h"
#include "esc_comm_safety.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CAN_COMM_HAND_TIME      5000u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Receive_Data_From_Control_Process(void);
static void Send_Data_To_Control_Process(void);

u16 *const pEscFault1CHA = (u16*)&EscDataToControl[1][0];
u16 *const pEscFault2CHA = (u16*)&EscDataToControl[1][2];
u16 *const pEscFault1CHB = (u16*)&EscDataToControl[1][4];
u16 *const pEscFault2CHB = (u16*)&EscDataToControl[1][6];

u16 *const pEscFault3CHA = (u16*)&EscDataToControl[2][0];
u16 *const pEscFault4CHA = (u16*)&EscDataToControl[2][2];
u16 *const pEscFault3CHB = (u16*)&EscDataToControl[2][4];
u16 *const pEscFault4CHB = (u16*)&EscDataToControl[2][6];

u16 *const pEscFault5CHA = (u16*)&EscDataToControl[3][0];
u16 *const pEscFault5CHB = (u16*)&EscDataToControl[3][2];

u16 *const pMotorSpeed1CHA = (u16*)&EscDataToControl[6][0];
u16 *const pMotorSpeed2CHA = (u16*)&EscDataToControl[6][2];
u16 *const pMotorSpeed1CHB = (u16*)&EscDataToControl[6][4];
u16 *const pMotorSpeed2CHB = (u16*)&EscDataToControl[6][6];

u16 *const pMainShaftSpeed1CHA = (u16*)&EscDataToControl[7][0];
u16 *const pMainShaftSpeed2CHA = (u16*)&EscDataToControl[7][2];
u16 *const pMainShaftSpeed1CHB = (u16*)&EscDataToControl[7][4];
u16 *const pMainShaftSpeed2CHB = (u16*)&EscDataToControl[7][6];

u16 *const pLeftHandrailSpeedCHA = (u16*)&EscDataToControl[8][0];
u16 *const pRightHandrailSpeedCHA = (u16*)&EscDataToControl[8][2];
u16 *const pLeftHandrailSpeedCHB = (u16*)&EscDataToControl[8][4];
u16 *const pRightHandrailSpeedCHB = (u16*)&EscDataToControl[8][6];

u16 *const pLeftHandrailCHA = (u16*)&EscDataToControl[9][0];
u16 *const pRightHandrailCHA = (u16*)&EscDataToControl[9][2];
u16 *const pLeftHandrailCHB = (u16*)&EscDataToControl[9][4];
u16 *const pRightHandrailCHB = (u16*)&EscDataToControl[9][6];

u16 *const pMissingStepUpperCHA = (u16*)&EscDataToControl[10][0];
u16 *const pMissingStepLowerCHA = (u16*)&EscDataToControl[10][2];
u16 *const pMissingStepUpperCHB = (u16*)&EscDataToControl[10][4];
u16 *const pMissingStepLowerCHB = (u16*)&EscDataToControl[10][6];


/*******************************************************************************
* Function Name  : Receive_Data_From_Control_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Receive_Data_From_Control_Process(void)
{	

}

/*******************************************************************************
* Function Name  : Send_Data_To_Control_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Send_Data_To_Control_Process(void)
{	

    /* FAULT CHA*/
    *pEscFault1CHA = EscErrorCodeBuff[0];
    *pEscFault2CHA = EscErrorCodeBuff[1];
    *pEscFault3CHA = EscErrorCodeBuff[2];
    *pEscFault4CHA = EscErrorCodeBuff[3];
    *pEscFault5CHA = EscErrorCodeBuff[4];

    /* FAULT CHB*/
    *pEscFault1CHB = OmcEscRtData.ErrorCode[0];
    *pEscFault2CHB = OmcEscRtData.ErrorCode[1];
    *pEscFault3CHB = OmcEscRtData.ErrorCode[2];
    *pEscFault4CHB = OmcEscRtData.ErrorCode[3];
    *pEscFault5CHB = OmcEscRtData.ErrorCode[4];
       
#ifdef ESC_TEST   
    /* SAFETY INPUT CHA */
    /* for debug */
    EscDataToControl[4][0] = OmcEscRtData.HeaderCode[0]; /*EscRtData.SafetyInputData[0];*/
    EscDataToControl[4][1] = EscRtData.HeaderCode[0];/*EscRtData.SafetyInputData[1];*/
    EscDataToControl[4][2] = EscRtData.HeaderCode[1];/*EscRtData.SafetyInputData[2];*/
    EscDataToControl[4][3] = EscRtData.HeaderCode[2];/*EscRtData.SafetyInputData[3];*/
    EscDataToControl[4][4] = EscRtData.DBL2Upper.SEQN;/*EscRtData.SafetyInputData[4];*/
    EscDataToControl[4][5] = EscRtData.DBL2Lower.SEQN;/*EscRtData.SafetyInputData[5]; */

    
    /* SAFETY INPUT CHB */
    /* for debug */
    EscDataToControl[5][0] = EscRtData.DBL2Interm1.SEQN;/*OmcEscRtData.SafetyInputData[0];*/
    EscDataToControl[5][1] = EscRtData.DBL2Interm2.SEQN;/*OmcEscRtData.SafetyInputData[1];*/
    EscDataToControl[5][2] = OmcEscRtData.DBL2Upper.SEQN;/*OmcEscRtData.SafetyInputData[2];*/
    EscDataToControl[5][3] = OmcEscRtData.DBL2Lower.SEQN;/*OmcEscRtData.SafetyInputData[3];*/
    EscDataToControl[5][4] = OmcEscRtData.DBL2Interm1.SEQN;/*OmcEscRtData.SafetyInputData[4];*/
    EscDataToControl[5][5] = OmcEscRtData.DBL2Interm2.SEQN;/*OmcEscRtData.SafetyInputData[5];*/  
    
#else
    /* SAFETY INPUT CHA */
    EscDataToControl[4][0] = EscRtData.SafetyInputData[0];
    EscDataToControl[4][1] = EscRtData.SafetyInputData[1];
    EscDataToControl[4][2] = EscRtData.SafetyInputData[2];
    EscDataToControl[4][3] = EscRtData.SafetyInputData[3];
    EscDataToControl[4][4] = EscRtData.SafetyInputData[4];
    EscDataToControl[4][5] = EscRtData.SafetyInputData[5];

    
    /* SAFETY INPUT CHB */
    EscDataToControl[5][0] = OmcEscRtData.SafetyInputData[0];
    EscDataToControl[5][1] = OmcEscRtData.SafetyInputData[1];
    EscDataToControl[5][2] = OmcEscRtData.SafetyInputData[2];
    EscDataToControl[5][3] = OmcEscRtData.SafetyInputData[3];
    EscDataToControl[5][4] = OmcEscRtData.SafetyInputData[4];
    EscDataToControl[5][5] = OmcEscRtData.SafetyInputData[5];  
#endif 
    
    /* MOTOR SPEED */
    *pMotorSpeed1CHA = *MTRITEM[0].ptFreqBuff;
    *pMotorSpeed2CHA = *MTRITEM[1].ptFreqBuff;
    *pMotorSpeed1CHB = OmcEscRtData.SensorData[0];
    *pMotorSpeed2CHB = OmcEscRtData.SensorData[2];
   
    
    /* MAIN SHAFT SPEED */
    *pMainShaftSpeed1CHA = EscRtData.SensorData[4];
    *pMainShaftSpeed2CHA = EscRtData.SensorData[6];
    *pMainShaftSpeed1CHB = OmcEscRtData.SensorData[4];
    *pMainShaftSpeed2CHB = OmcEscRtData.SensorData[6];

    /* HANDRAIL SPEED */
    *pLeftHandrailSpeedCHA = EscRtData.SensorData[8];
    *pRightHandrailSpeedCHA = EscRtData.SensorData[10];
    *pLeftHandrailSpeedCHB = OmcEscRtData.SensorData[8];
    *pRightHandrailSpeedCHB = OmcEscRtData.SensorData[10]; 
    
    /* HANDRAIL (MOTOR PULSE) */
    *pLeftHandrailSpeedCHA = EscRtData.SensorData[12];
    *pRightHandrailSpeedCHA = EscRtData.SensorData[14];
    *pLeftHandrailSpeedCHB = OmcEscRtData.SensorData[12];
    *pRightHandrailSpeedCHB = OmcEscRtData.SensorData[14]; 
    
    /* MISSING STEP */
    *pMissingStepUpperCHA = EscRtData.SensorData[16];
    *pMissingStepLowerCHA = EscRtData.SensorData[18];
    *pMissingStepUpperCHB = OmcEscRtData.SensorData[16];
    *pMissingStepLowerCHB = OmcEscRtData.SensorData[18];     

    /* DBL2 UPPER INPUT CHA */
    EscDataToControl[11][0] = EscRtData.DBL2Upper.InputData[0];
    EscDataToControl[11][1] = EscRtData.DBL2Upper.InputData[1];
    EscDataToControl[11][2] = EscRtData.DBL2Upper.InputData[2];
    EscDataToControl[11][3] = EscRtData.DBL2Upper.InputData[3];
    
    /* DBL2 UPPER INPUT CHB */
    EscDataToControl[11][4] = OmcEscRtData.DBL2Upper.InputData[0];
    EscDataToControl[11][5] = OmcEscRtData.DBL2Upper.InputData[1]; 
    EscDataToControl[11][6] = OmcEscRtData.DBL2Upper.InputData[2];
    EscDataToControl[11][7] = OmcEscRtData.DBL2Upper.InputData[3];     
    
    /* DBL2 LOWER INPUT CHA */
    EscDataToControl[12][0] = EscRtData.DBL2Lower.InputData[0];
    EscDataToControl[12][1] = EscRtData.DBL2Lower.InputData[1];
    EscDataToControl[12][2] = EscRtData.DBL2Lower.InputData[2];
    EscDataToControl[12][3] = EscRtData.DBL2Lower.InputData[3];
    
    /* DBL2 LOWER INPUT CHB */
    EscDataToControl[12][4] = OmcEscRtData.DBL2Lower.InputData[0];
    EscDataToControl[12][5] = OmcEscRtData.DBL2Lower.InputData[1]; 
    EscDataToControl[12][6] = OmcEscRtData.DBL2Lower.InputData[2];
    EscDataToControl[12][7] = OmcEscRtData.DBL2Lower.InputData[3];  
    
    /* DBL2 INTERM 1 INPUT CHA */
    EscDataToControl[13][0] = EscRtData.DBL2Interm1.InputData[0];
    EscDataToControl[13][1] = EscRtData.DBL2Interm1.InputData[1];
    EscDataToControl[13][2] = EscRtData.DBL2Interm1.InputData[2];
    EscDataToControl[13][3] = EscRtData.DBL2Interm1.InputData[3];
    
    /* DBL2 INTERM 1 INPUT CHB */
    EscDataToControl[13][4] = OmcEscRtData.DBL2Interm1.InputData[0];
    EscDataToControl[13][5] = OmcEscRtData.DBL2Interm1.InputData[1]; 
    EscDataToControl[13][6] = OmcEscRtData.DBL2Interm1.InputData[2];
    EscDataToControl[13][7] = OmcEscRtData.DBL2Interm1.InputData[3];     
    
    /* DBL2 INTERM 2 INPUT CHA */
    EscDataToControl[14][0] = EscRtData.DBL2Interm2.InputData[0];
    EscDataToControl[14][1] = EscRtData.DBL2Interm2.InputData[1];
    EscDataToControl[14][2] = EscRtData.DBL2Interm2.InputData[2];
    EscDataToControl[14][3] = EscRtData.DBL2Interm2.InputData[3];
    
    /* DBL2 INTERM 2 INPUT CHB */
    EscDataToControl[14][4] = OmcEscRtData.DBL2Interm2.InputData[0];
    EscDataToControl[14][5] = OmcEscRtData.DBL2Interm2.InputData[1]; 
    EscDataToControl[14][6] = OmcEscRtData.DBL2Interm2.InputData[2];
    EscDataToControl[14][7] = OmcEscRtData.DBL2Interm2.InputData[3];    
    
    /* ANALOG/PT100 UPPER */
    EscDataToControl[15][0] = (u8)EscRtData.DBL2Upper.AnalogData[0];
    EscDataToControl[15][1] |= (u8)((EscRtData.DBL2Upper.AnalogData[0] >> 8u) & 0x0fu);
    EscDataToControl[15][1] |= (u8)((u16)(EscRtData.DBL2Upper.AnalogData[1] << 4u) & 0xf0u);
    EscDataToControl[15][2] = (u8)((EscRtData.DBL2Upper.AnalogData[1] >> 4u) & 0xffu);
    EscDataToControl[15][3] = (u8)EscRtData.DBL2Upper.AnalogData[2];
    
    /* ANALOG/PT100 LOWER */
    EscDataToControl[15][4] = (u8)EscRtData.DBL2Lower.AnalogData[0];
    EscDataToControl[15][5] |= (u8)((EscRtData.DBL2Lower.AnalogData[0] >> 8u) & 0x0fu);
    EscDataToControl[15][5] |= (u8)((u16)(EscRtData.DBL2Lower.AnalogData[1] << 4u) & 0xf0u);
    EscDataToControl[15][6] = (u8)((EscRtData.DBL2Lower.AnalogData[1] >> 4u) & 0xffu);
    EscDataToControl[15][7] = (u8)EscRtData.DBL2Lower.AnalogData[2];     
    
    /* ANALOG/PT100 INTERM 1 */
    EscDataToControl[16][0] = (u8)EscRtData.DBL2Interm1.AnalogData[0];
    EscDataToControl[16][1] |= (u8)((EscRtData.DBL2Interm1.AnalogData[0] >> 8u) & 0x0fu);
    EscDataToControl[16][1] |= (u8)((u16)(EscRtData.DBL2Interm1.AnalogData[1] << 4u) & 0xf0u);
    EscDataToControl[16][2] = (u8)((EscRtData.DBL2Interm1.AnalogData[1] >> 4u) & 0xffu);
    EscDataToControl[16][3] = (u8)EscRtData.DBL2Interm1.AnalogData[2];
    
    /* ANALOG/PT100 INTERM 2 */
    EscDataToControl[16][4] = (u8)EscRtData.DBL2Interm2.AnalogData[0];
    EscDataToControl[16][5] |= (u8)((EscRtData.DBL2Interm2.AnalogData[0] >> 8u) & 0x0fu);
    EscDataToControl[16][5] |= (u8)((u16)(EscRtData.DBL2Interm2.AnalogData[1] << 4u) & 0xf0u);
    EscDataToControl[16][6] = (u8)((EscRtData.DBL2Interm2.AnalogData[1] >> 4u) & 0xffu);
    EscDataToControl[16][7] = (u8)EscRtData.DBL2Interm2.AnalogData[2];    
    
    /* DBL1 UPPER INPUT */
    EscDataToControl[17][0] = EscRtData.DBL1Upper.InputData[0];
    EscDataToControl[17][1] = EscRtData.DBL1Upper.InputData[1];
    EscDataToControl[17][2] = EscRtData.DBL1Upper.InputData[2];
    EscDataToControl[17][3] = EscRtData.DBL1Upper.InputData[3];
    
    /* DBL1 LOWER INPUT */
    EscDataToControl[17][4] = EscRtData.DBL2Lower.InputData[0];
    EscDataToControl[17][5] = EscRtData.DBL2Lower.InputData[1]; 
    EscDataToControl[17][6] = EscRtData.DBL2Lower.InputData[2];
    EscDataToControl[17][7] = EscRtData.DBL2Lower.InputData[3];  

    /* DBL1 INTERM 1 INPUT */
    EscDataToControl[18][0] = EscRtData.DBL2Interm1.InputData[0];
    EscDataToControl[18][1] = EscRtData.DBL2Interm1.InputData[1];
    EscDataToControl[18][2] = EscRtData.DBL2Interm1.InputData[2];
    EscDataToControl[18][3] = EscRtData.DBL2Interm1.InputData[3];
    
    /* DBL1 INTERM 2 INPUT */
    EscDataToControl[18][4] = EscRtData.DBL2Interm2.InputData[0];
    EscDataToControl[18][5] = EscRtData.DBL2Interm2.InputData[1]; 
    EscDataToControl[18][6] = EscRtData.DBL2Interm2.InputData[2];
    EscDataToControl[18][7] = EscRtData.DBL2Interm2.InputData[3];     
}

/*******************************************************************************
* Function Name  : Communication_To_Control
* Description    : None
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Communication_To_Control(void)
{    
    static u8 can1_comm_timeout = 0u;   
    static u16 stat_u16TimerCan1CommWait = 0u, stat_u16Can1HandshakeSuccess = 0u;
    static u8 stat_u8TimerSend = 0u;
    u8 result,i;
    
    if( can1_receive == 1u )
    {
        can1_receive = 0u;
        can1_comm_timeout = 0u;
        stat_u16Can1HandshakeSuccess = 1u;
        /*EN_ERROR7 &= ~0x04u;*/
    }
    else if( stat_u16Can1HandshakeSuccess )
    {
        if( ++can1_comm_timeout >= 10u )
        {
            /*  can communication timeout process */
            EN_ERROR7 |= 0x04u;
        }
    }
    else
    {
        stat_u16TimerCan1CommWait++;
        if( stat_u16TimerCan1CommWait * 10u > CAN_COMM_HAND_TIME )
        {
            /*  can communication handshake timeout when power on */ 
            EN_ERROR7 |= 0x04u;
        }            
    }    
    
    
/*
Message ID: 100h.
Message cycle time: 20ms.
Message ID: 101h.
Message cycle time: 50ms.
Message ID: 102h.
Message cycle time: 50ms.
Message ID: 103h.
Message cycle time: 50ms.
Message ID: 104h.
Message cycle time: 20ms.
Message ID: 105h.
Message cycle time: 20ms.
Message ID: 106h.
Message cycle time: 100ms.
Message ID: 107h.
Message cycle time: 100ms.
Message ID: 108h.
Message cycle time: 100ms.
Message ID: 109h.
Message cycle time: 100ms.
Message ID: 10Ah.
Message cycle time: 100ms
Message ID: 10Bh.
Message cycle time: 20ms.
Message ID: 10Ch.
Message cycle time: 20ms.
Message ID: 10Dh.
Message cycle time: 20ms.
Message ID: 10Eh.
Message cycle time: 20ms.
Message ID: 10Fh.
Message cycle time: 50ms.
Message ID: 110h.
Message cycle time: 50ms.
Message ID: 111h.
Message cycle time: 20ms.
Message ID: 112h.
Message cycle time: 20ms.
*/    

    if( !g_u8CanCommunicationToCotrolOk )
    {
        /* not send finish,54 */
        EN_ERROR7 |= 0x20u;
    }
    else
    {
        EN_ERROR7 &= ~0x20u;
    }

    if(( stat_u8TimerSend % 2u ) == 0u )
    {
        
        /* Recieve data */
        Can_Receive_Data(DATA_FROM_CONTROL);
        Receive_Data_From_Control_Process();
    
        /* Send data, Message cycle time: 20ms*/
        Send_Data_To_Control_Process();
        
        g_u8CanCommunicationToCotrolOk = 0u;
        g_u8CanCommunicationToCotrolID = 0u;
        g_u8CanCommunicationToCotrolLen = 8u;
        for( i = 0u; i < 3u; i++ )
        {
            result = Can_Send_Msg(CAN1, CAN1TX_SAFETY_DATA_ID1 + g_u8CanCommunicationToCotrolID, 
                                  &EscDataToControl[g_u8CanCommunicationToCotrolID][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
            } 
            else
            {
                g_u8CanCommunicationToCotrolID++;
            }
        }
        CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
    }
    else if( (stat_u8TimerSend == 1u) || (stat_u8TimerSend == 7u) )
    {
        /* Send data, Message cycle time: 50ms*/
        g_u8CanCommunicationToCotrolOk = 0u;
        g_u8CanCommunicationToCotrolID = 9u;
        g_u8CanCommunicationToCotrolLen = 13u;
        for( i = 0u; i < 3u; i++ )
        {
            result = Can_Send_Msg(CAN1, CAN1TX_SAFETY_DATA_ID1 + g_u8CanCommunicationToCotrolID, 
                                  &EscDataToControl[g_u8CanCommunicationToCotrolID][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
            } 
            else
            {
                g_u8CanCommunicationToCotrolID++;
            }
        }
        CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
    }
    else if( stat_u8TimerSend == 9u )
    {
        /* Send data, Message cycle time: 100ms*/
        g_u8CanCommunicationToCotrolOk = 0u;
        g_u8CanCommunicationToCotrolID = 14u;
        g_u8CanCommunicationToCotrolLen = 18u;
        for( i = 0u; i < 3u; i++ )
        {
            result = Can_Send_Msg(CAN1, CAN1TX_SAFETY_DATA_ID1 + g_u8CanCommunicationToCotrolID, 
                                  &EscDataToControl[g_u8CanCommunicationToCotrolID][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
            } 
            else
            {
                g_u8CanCommunicationToCotrolID++;
            }
        }
        CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
    }
    else
    {}
    
    stat_u8TimerSend++;
    if( stat_u8TimerSend == 10u )
    {
        stat_u8TimerSend = 0u;
    }      
}


/******************************  END OF FILE  *********************************/