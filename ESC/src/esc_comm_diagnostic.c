/*******************************************************************************
* File Name          : esc_comm_diagnostic.c
* Author             : lison
* Version            : V1.0
* Date               : 06/23/2016
* Last modify date   : 10/27/2016
* Description        : This file contains esc safety board communication  with
*                      diagnostic board.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_comm_diagnostic.h"
#include "esc_comm_safety.h"
#include "can.h"
#include "hw_test.h"
#include "crc16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CAN_COMM_HAND_TIME      8000u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Send_Data_To_DBL1_Process(void);
static void Receive_Data_From_DBL1_Process(void);
static void Send_Data_To_DBL2(void);
static void Receive_Data_From_DBL2(void);
static void Esc_Comm_Diagnostic_Error_Process(void);
static void Safety_Comm_Timeout_Check(void);
static void Send_Data_To_DBL2_Process(DBL2Esc *SFData, u8 SendDataB[], u8 CANSendData[], u8 CANID);
static void Receive_Data_From_DBL2_Process(DBL2Esc *SFData, u8 CANReceiveData[], u8 CANID);

u8 g_u8DBL1CommDataValidate = 0u;

/*******************************************************************************
* Function Name  : Esc_Comm_Diagnostic_Error_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Esc_Comm_Diagnostic_Error_Process(void)
{	
    u8 i,j;
    
    if( DIAGNOSTIC == DIAGNOSTIC_BOARD_1 )
    {
        /* Clear the data when communicaiton timeout or error */
        for( j = 0u; j < 4u; j++ )
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[j][i] = 0u;
            }
        }
    }
    else if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
    {
        /* Clear the data when communicaiton timeout or error */
        for( j = 0u; j < 16u; j++ )
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL2[j][i] = 0u;
            }
        }
    }
    else
    {}
}

/*******************************************************************************
* Function Name  : Receive_Data_From_DBL1_Process
* Description    :     
*
    0	0-7	LIFE SIGNAL	
    1	0-7	INPUTS 1-8
    2	0-7	INPUTS 9-16
    3	0-7	INPUTS 17-24
    4	0-7	INPUTS 25-32
    5-6	0-15	CRC
    7	0-7	NOT USED
*
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Receive_Data_From_DBL1_Process(void)
{	
    u8 i;
  
    
    /*********************** DBL1 UPPER *********************************/  
    if(( DIAGNOSTIC_BOARD_L1_QUANTITY == 2u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u ))
    {     
        if( !MB_CRC16(&EscDataFromDBL1[0][0], 7u) )
        {
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL1Upper.InputData[i] = EscDataFromDBL1[0][i+1u];
            }
            g_u8DBL1CommDataValidate |= DBL1_UPPER_VALIDATE;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_UPPER_VALIDATE;
            
            /* data error */
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[0][i] = 0u;
            }
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL1Upper.InputData[i] = 0u;
            }        
        }
        
        /*********************** DBL1 LOWER **********************************/
        if( !MB_CRC16(&EscDataFromDBL1[1][0], 7u) )
        {
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL2Lower.InputData[i] = EscDataFromDBL1[1][i+1u];
            }
            g_u8DBL1CommDataValidate |= DBL1_LOWER_VALIDATE;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_LOWER_VALIDATE;
            
            /* data error */
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[1][i] = 0u;
            }
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL2Lower.InputData[i] = 0u;
            }          
        }
    }
    
    /********************** DBL1 INTERM1 *********************************/
    if(( DIAGNOSTIC_BOARD_L1_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u ))
    {
        if( !MB_CRC16(&EscDataFromDBL1[2][0], 7u) )
        {
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL2Interm1.InputData[i] = EscDataFromDBL1[2][i+1u];
            }
            g_u8DBL1CommDataValidate |= DBL1_INTERM1_VALIDATE;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_INTERM1_VALIDATE;
        
            /* data error */
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[2][i] = 0u;
            }
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL2Interm1.InputData[i] = 0u;
            }          
        } 
    }
    
    /********************** DBL1 INTERM2 *********************************/
    if( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u )
    {
        if( !MB_CRC16(&EscDataFromDBL1[3][0], 7u) )
        {
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL2Interm2.InputData[i] = EscDataFromDBL1[3][i+1u];
            }
            g_u8DBL1CommDataValidate |= DBL1_INTERM2_VALIDATE;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_INTERM2_VALIDATE;
            
            /* data error */
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[3][i] = 0u;
            }
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL2Interm2.InputData[i] = 0u;
            }          
        }    
    }
}

/*******************************************************************************
* Function Name  : Send_Data_To_DBL1_Process
* Description    :      
*
    0	0-7	LIFE SIGNAL	
    1	0-7	OUTPUTS DBL1 UPPER
    2	0-7	OUTPUTS DBL1 	LOWER
    3	0-7	OUTPUTS DBL1 INTERM. 1
    4	0-7	OUTPUTS DBL1 INTERM. 2
    5-6	0-15	CRC
    7	0-7	NOT USED
*
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Send_Data_To_DBL1_Process(void)
{	
    u16 crc,len;
    u16 *const pEscErrorCode = (u16*)&EscDataToDBL1[0][0];
    u8 result;
    
    
    /* for test */
    if( EscErrorCodeBuff[0] )
    {
        *pEscErrorCode = EscRtData.ErrorCode[0];
    }
    else
    {
        *pEscErrorCode = OmcEscRtData.ErrorCode[0];
    }
    
    /* DBL1 UPPER */
    EscDataToDBL1[0][1] = EscRtData.DBL1Upper.OutputData;
    /* DBL1 LOWER */
    EscDataToDBL1[0][2] = EscRtData.DBL1Lower.OutputData;
    /* DBL1 INTERM1 */
    EscDataToDBL1[0][3] = EscRtData.DBL1Interm1.OutputData;
    /* DBL1 INTERM2 */
    EscDataToDBL1[0][4] = EscRtData.DBL1Interm2.OutputData;    

    /* CRC */
    len = 7u;
    crc = MB_CRC16( &EscDataToDBL1[0][0], len - 2u );
    EscDataToDBL1[0][len - 2u] = (u8)crc;
    EscDataToDBL1[0][len - 1u] = (u8)(crc >> 8u);   
    
    /* Send data */
    result = Can_Send_Msg(CAN2, CAN2TX_DBL1_ID, &EscDataToDBL1[0][0], CAN_FRAME_LEN ); 
    if( result )
    {
        /* No mail box, send fail */
        g_u16CAN2SendFail |= 0x01u;
        CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
    } 
}

/*******************************************************************************
* Function Name  : Receive_Data_From_DBL2_Process
* Description    :      
*
    MESSAGE 1
    Byte	Bits (7 is MSB)	Data
    0	0-1	CONNECTION_A_1
    0	2-8	FAULT_STATUS_A_1
    1	0-7	SAFETY_SENSOR_INPUTS_A_1 (8inputs =8x1 bit )
    2	0-5	SAFETY_SENSOR_INPUTS_A_1 (6inputs =6x1 bit )
    2	6-7	SAFETY_SWITCH_INPUTS_A_1 (2inputs =2x1 bit )
    3	0-7	SEQN_A_1
    4	0-1	CONNECTION_A_2
    4	2-8	FAULT_STATUS_A_2
    5	0-7	SAFETY_SENSOR_INPUTS_A_2 (8inputs =8x1 bit )
    6	0-5	SAFETY_SENSOR_INPUTS_A_2 (6inputs =6x1 bit )
    6	6-7	SAFETY_SWITCH_INPUTS_A _2 (2inputs =2x1 bit )
    7	0-7	SEQN_A_2

    MESSAGE 2
    Byte	Bits (7 is MSB)	Data
    0	0-1	CONNECTION_B_1
    0	2-8	FAULT_STATUS_B_1
    1	0-7	SAFETY_SENSOR_INPUTS_B_1 (8inputs =8x1 bit )
    2	0-5	SAFETY_SENSOR_INPUTS_B_1 (6inputs =6x1 bit )
    2	6-7	SAFETY_SWITCH_INPUTS_B_1 (2inputs =2x1 bit )
    3	0-7	SEQN_B_1
    4	0-1	CONNECTION_B_2
    4	2-8	FAULT_STATUS_B_2
    5	0-7	SAFETY_SENSOR_INPUTS_B_2 (8inputs =8x1 bit )
    6	0-5	SAFETY_SENSOR_INPUTS_B_2 (6inputs =6x1 bit )
    6	6-7	SAFETY_SWITCH_INPUTS_B_2 (2inputs =2x1 bit )
    7	0-7	SEQN_B_1
        
    MESSAGE 3
    Byte	Bits (7 is MSB)	Data
    0,1,2,3	0-31	CRC_A
    4,5,6,7	0-31	CRC_B

    CRC_A: CONNECTION_A , FAULT_STATUS_A, SAFETY_SENSOR_INPUTS_A, 
    SAFETY_SWITCH_INPUTS_A, SEQN_A

    CRC_B: CONNECTION_B , FAULT_STATUS_B, SAFETY_SENSOR_INPUTS_B, 
    SAFETY_SWITCH_INPUTS_B, SEQN_B


    NON SAFE DATA     
    Byte	Bits (7 is MSB)	Data
    0-1	0-11	ANALOG INPUT 1
    1-2	12-23	ANALOG INPUT 2
    3	4-7	PT100
    4	0-7	SAFETY SWITCH INPUTS_A (8 inputs)
    5	0-7	SAFETY SWITCH INPUTS_A (8 inputs)
    6	0-7	SAFETY SWITCH INPUTS_B (8 inputs)
    7	0-7	SAFETY SWITCH INPUTS_B (8 inputs)

*
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Receive_Data_From_DBL2_Process(DBL2Esc *SFData, u8 CANReceiveData[], u8 CANID)
{	
    u8 i;

    if( (CANReceiveData[3]) && (CANReceiveData[11]) && ((CANReceiveData[16]) || (CANReceiveData[17])))
    {
        /*
        Can_Clean_Buffer((u16)CANID,DATA_FROM_DBL2);
        Can_Clean_Buffer((u16)CANID+1u,DATA_FROM_DBL2);
        Can_Clean_Buffer((u16)CANID+2u,DATA_FROM_DBL2);
        */
        
        for( i = 0u; i < 8u; i++ )
        {
            SFData->ReceiveDataA[i] = CANReceiveData[i];
            SFData->ReceiveDataB[i] = CANReceiveData[i + 8u];
        }
        for( i = 0u; i < 4u; i++ )
        {
            SFData->ReceiveDataA[i+8u] = CANReceiveData[i + 16u];
            SFData->ReceiveDataB[i+8u] = CANReceiveData[i + 20u];
        }

        Safety_Receive_Diagnostic_Validate(SFData, SFData->ReceiveDataA );
        
        /* receive finish, clear the data */
        for( i = 0u; i < 24u; i++ )
        {
          CANReceiveData[i] = 0u;
        }   
    }
    
    /********************* NON Safety Data **********************************/
    
    for( i = 0u; i < 2u; i++ )
    {
        SFData->ReceiveDataA[i+12u] = CANReceiveData[i + 28u];
        SFData->ReceiveDataB[i+12u] = CANReceiveData[i + 30u];
    }
    SFData->AnalogData[0] |= (u8)CANReceiveData[24];
    SFData->AnalogData[0] |= (u16)(((u16)CANReceiveData[25] << 8u) & 0x0f00u);
    SFData->AnalogData[1] |= (u8)((CANReceiveData[25] >> 4u) & 0x0fu);
    SFData->AnalogData[1] |= (u16)(((u16)CANReceiveData[26] << 4u) & 0x0ff0u);
    SFData->AnalogData[2] = CANReceiveData[27];
    SFData->InputData[2] = CANReceiveData[28];
    SFData->InputData[3] = CANReceiveData[29];    
    
    
}



/*******************************************************************************
* Function Name  : Receive_Data_From_DBL2
* Description    :      
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Receive_Data_From_DBL2(void)
{	

    /*********************** DBL2 UPPER *********************************/  
    Receive_Data_From_DBL2_Process( &EscRtData.DBL2Upper, &EscDataFromDBL2[0][0], CAN2RX_DBL2_UPPER_ID1 );
    

    /*************************** DBL2 LOWER *****************************/
    Receive_Data_From_DBL2_Process( &EscRtData.DBL2Lower, &EscDataFromDBL2[4][0], CAN2RX_DBL2_LOWER_ID1 );
        
        
    /*************************** DBL2 INTERM1 ************************ **/
    Receive_Data_From_DBL2_Process( &EscRtData.DBL2Interm1, &EscDataFromDBL2[8][0], CAN2RX_DBL2_INTERM1_ID1);  
  
    
    /*************************** DBL2 INTERM2 ***************************/
    Receive_Data_From_DBL2_Process( &EscRtData.DBL2Interm2, &EscDataFromDBL2[12][0], CAN2RX_DBL2_INTERM2_ID1);
    
}

/*******************************************************************************
* Function Name  : Send_Data_To_DBL2_Process
* Description    :      
*
    MESSAGE 1
    Byte	Bits (7 is MSB)	Data
    0	0-1	CONNECTION_A_1
    0	2-3	RESET_A_1
    0	4-5	CONNECTION_B_1
    0	6-7	RESET_B_1
    1	0-7	SEQN_A_1
    2	0-7	SEQN_B_1
    3	0-1	CONNECTION_A_2
    3	2-3	RESET_A_2
    3	4-5	CONNECTION_B_2
    3	6-7	RESET_B_2
    4	0-7	SEQN_A_2
    5	0-7	SEQN_B_2
    6	0-7	OUTPUTS (No safety relevant)
    7	0-7	NOT USED

    MESSAGE 2
    Byte	Bits (7 is MSB)	Data
    0,1,2,3	0-31	CRC_A
    4,5,6,7	0-31	CRC_B

    CRC_A: RESET_A, SEQN_A, CONNECTION_A, OUTPUTS
    CRC_B: RESET_B, SEQN_B, CONNECTION_B
*
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Send_Data_To_DBL2_Process(DBL2Esc *SFData, u8 SendDataB[], u8 CANSendData[], u8 CANID)
{	
    u8 i,result;

    if( !MB_CRC16(&SendDataB[0] , 7u ) )
    {
        if(( SendDataB[0] & 0x03u) == SFData->Connection )
        {
            Safety_Send_Data_Process(SFData, 1u);
            
            CANSendData[0] |= (u8)(SFData->SendData[0] & 0x0fu);
            CANSendData[0] |= (u8)((SendDataB[0] << 4u) & 0xf0u);
            CANSendData[1] = SFData->SendData[1]; 
            CANSendData[2] = SendDataB[1];   
            CANSendData[3] |= (u8)((SFData->SendData[0] >> 4u) & 0x0fu);
            CANSendData[3] |= (u8)(SendDataB[0] & 0xf0u);
            CANSendData[4] = SFData->SendData[2]; 
            CANSendData[5] = SendDataB[2];      
            CANSendData[6] = SFData->SendData[3];   
            CANSendData[7] = 0x00u; 
            
            CANSendData[8] = SFData->SendData[4];   
            CANSendData[9] = SFData->SendData[5]; 
            CANSendData[10] = SFData->SendData[6];   
            CANSendData[11] = SFData->SendData[7]; 
            CANSendData[12] = SendDataB[3];   
            CANSendData[13] = SendDataB[4]; 
            CANSendData[14] = SendDataB[5];   
            CANSendData[15] = SendDataB[6];    
            
            result = Can_Send_Msg(CAN2, (u32)CANID, &CANSendData[0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= SFData->BoardType;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);               
            }  
            result = Can_Send_Msg(CAN2, (u32)CANID + 1u, &CANSendData[8], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= (u16)((u16)SFData->BoardType << 1u);
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);        
            }       
        }
    }
    
    /* clear data */
    for( i = 0u; i < 8u; i++ )
    {
        SFData->SendData[i] = 0u;
        SendDataB[i] = 0u;
    }
}


/*******************************************************************************
* Function Name  : Send_Data_To_DBL2
* Description    :      
*
    MESSAGE 1
    Byte	Bits (7 is MSB)	Data
    0	0-1	CONNECTION_A_1
    0	2-3	RESET_A_1
    0	4-5	CONNECTION_B_1
    0	6-7	RESET_B_1
    1	0-7	SEQN_A_1
    2	0-7	SEQN_B_1
    3	0-1	CONNECTION_A_2
    3	2-3	RESET_A_2
    3	4-5	CONNECTION_B_2
    3	6-7	RESET_B_2
    4	0-7	SEQN_A_2
    5	0-7	SEQN_B_2
    6	0-7	OUTPUTS (No safety relevant)
    7	0-7	NOT USED

    MESSAGE 2
    Byte	Bits (7 is MSB)	Data
    0,1,2,3	0-31	CRC_A
    4,5,6,7	0-31	CRC_B

    CRC_A: RESET_A, SEQN_A, CONNECTION_A, OUTPUTS
    CRC_B: RESET_B, SEQN_B, CONNECTION_B
*
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Send_Data_To_DBL2(void)
{	
    static u8 stat_u8FirstEnter = 0u;
    
    if( stat_u8FirstEnter == 0u )
    {
        stat_u8FirstEnter = 1u;
        Safety_Comm_Data_Init(&EscRtData.DBL2Upper, CONNECTION_DBL2_UPPER, DBL2_TYPE_UPPER, 1u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Lower, CONNECTION_DBL2_LOWER, DBL2_TYPE_LOWER, 1u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Interm1, CONNECTION_DBL2_INTERM1, DBL2_TYPE_INTERM1, 1u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Interm2, CONNECTION_DBL2_INTERM2, DBL2_TYPE_INTERM2, 1u);
    }
    
    /********************************************************************/
    /*************************** DBL2 UPPER *****************************/
    /********************************************************************/
    Send_Data_To_DBL2_Process(&EscRtData.DBL2Upper , OmcEscRtData.DBL2Upper.SendData, 
                      &EscDataToDBL2[0][0], CAN2TX_DBL2_UPPER_ID1 );

    
    /********************************************************************/
    /*************************** DBL2 LOWER *****************************/
    /********************************************************************/
    Send_Data_To_DBL2_Process(&EscRtData.DBL2Lower , OmcEscRtData.DBL2Lower.SendData, 
                      &EscDataToDBL2[2][0], CAN2TX_DBL2_LOWER_ID1 );
   
    

    /********************************************************************/
    /*************************** DBL2 INTERM1 ***************************/
    /********************************************************************/ 
    Send_Data_To_DBL2_Process(&EscRtData.DBL2Interm1 , OmcEscRtData.DBL2Interm1.SendData, 
                      &EscDataToDBL2[4][0], CAN2TX_DBL2_INTERM1_ID1 );
       
    
    
    /********************************************************************/
    /*************************** DBL2 INTERM2 ***************************/
    /********************************************************************/ 
    Send_Data_To_DBL2_Process(&EscRtData.DBL2Interm2 , OmcEscRtData.DBL2Interm2.SendData, 
                      &EscDataToDBL2[6][0], CAN2TX_DBL2_INTERM2_ID1 );
    
}



/*******************************************************************************
* Function Name  : Safety_Comm_Timeout_Check
* Description    : Communicate to Diagnostic level 1 or diagnostic level 2.              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Safety_Comm_Timeout_Check(void)
{
    static u8 stat_u8DBLCommUpperTimeout = 0u,stat_u8DBLCommLowerTimeout = 0u,stat_u8DBLCommInterm1Timeout = 0u,stat_u8DBLCommInterm2Timeout = 0u;    
    static u16 stat_u16TimerDBLCommWait = 0u, stat_u16DBLHandshakeSuccess = 0u;
    static u8 stat_u8DBLCommTimeout = 0u;
    
    if( DIAGNOSTIC == DIAGNOSTIC_BOARD_1 )
    { 
        if(( DIAGNOSTIC_BOARD_L1_QUANTITY == 2u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u ))
        {          
            /* upper */
            if( g_u8DBL1CommDataValidate & DBL1_UPPER_VALIDATE )
            {
                stat_u8DBLCommUpperTimeout = 0u;
                stat_u16DBLHandshakeSuccess = 1u;
                g_u8DBL1CommDataValidate &= ~DBL1_UPPER_VALIDATE;
            }
            else
            {  
                stat_u8DBLCommUpperTimeout++;
            }
            
            /* lower */
            if( g_u8DBL1CommDataValidate & DBL1_LOWER_VALIDATE )
            {
                stat_u8DBLCommLowerTimeout = 0u;
                stat_u16DBLHandshakeSuccess = 1u;
                g_u8DBL1CommDataValidate &= ~DBL1_LOWER_VALIDATE;            
            }
            else
            {
                stat_u8DBLCommLowerTimeout++;
            }            
        }
        
        if(( DIAGNOSTIC_BOARD_L1_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u ))
        {        
            /* Interm1 */
            if( g_u8DBL1CommDataValidate & DBL1_INTERM1_VALIDATE )
            {
                stat_u8DBLCommInterm1Timeout = 0u;
                stat_u16DBLHandshakeSuccess = 1u;
                g_u8DBL1CommDataValidate &= ~DBL1_INTERM1_VALIDATE;            
            }
            else
            {
                stat_u8DBLCommInterm1Timeout++;
            }  
        }
        
        if( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u )
        {
            /* Interm2 */
            if( g_u8DBL1CommDataValidate & DBL1_INTERM2_VALIDATE )
            {
                stat_u8DBLCommInterm2Timeout = 0u;
                stat_u16DBLHandshakeSuccess = 1u;
                g_u8DBL1CommDataValidate &= ~DBL1_INTERM2_VALIDATE;            
            }
            else
            {
                stat_u8DBLCommInterm2Timeout++;
            }    
        }
                       
        if( stat_u16DBLHandshakeSuccess )
        {
            if( stat_u8DBLCommUpperTimeout > 10u )
            {
                stat_u8DBLCommUpperTimeout = 11u;
                /*  can communication timeout process */
                EN_ERROR9 |= 0x04u;
                Esc_Comm_Diagnostic_Error_Process();
            }
            if( stat_u8DBLCommLowerTimeout > 10u )
            {
                stat_u8DBLCommLowerTimeout = 11u;
                /*  can communication timeout process */
                EN_ERROR9 |= 0x08u;
                Esc_Comm_Diagnostic_Error_Process();
            }
            if( stat_u8DBLCommInterm1Timeout > 10u )
            {
                stat_u8DBLCommInterm1Timeout = 11u;
                /*  can communication timeout process */
                EN_ERROR9 |= 0x10u;
                Esc_Comm_Diagnostic_Error_Process();
            }
            if( stat_u8DBLCommInterm2Timeout > 10u )
            {
                stat_u8DBLCommInterm2Timeout = 11u;
                /*  can communication timeout process */
                EN_ERROR9 |= 0x20u;
                Esc_Comm_Diagnostic_Error_Process();
            }            
        }
        else
        {
            stat_u16TimerDBLCommWait++;
            if( ( stat_u16TimerDBLCommWait * SYSTEMTICK ) > CAN_COMM_HAND_TIME )
            {
                /*  can communication handshake timeout when power on */ 
                EN_ERROR9 |= 0x08u;
            }            
        }    
    }
    else if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
    {
        if( can2_receive == 1u )
        {
            can2_receive = 0u;
            stat_u8DBLCommTimeout = 0u;
            stat_u16DBLHandshakeSuccess = 1u;
            EN_ERROR7 &= ~0x08u;
        }
        else if( stat_u16DBLHandshakeSuccess )
        {
            if( ++stat_u8DBLCommTimeout >= 20u )
            {
                /*  can communication timeout process */
                EN_ERROR7 |= 0x08u;
                Esc_Comm_Diagnostic_Error_Process();
            }
        }
        else
        {
            stat_u16TimerDBLCommWait++;
            if( ( stat_u16TimerDBLCommWait * SYSTEMTICK ) > CAN_COMM_HAND_TIME )
            {
                /*  can communication handshake timeout when power on */ 
                EN_ERROR7 |= 0x08u;
            }            
        }    
    }
    else
    {}    
}

/*******************************************************************************
* Function Name  : Safety_Comm_Diag
* Description    : Communicate to Diagnostic level 1 or diagnostic level 2.              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Safety_Comm_Diag(void)
{
#ifdef GEC_SF_MASTER      
    static u8 stat_u8DBL1Comm = 0u;


    if( DIAGNOSTIC == DIAGNOSTIC_BOARD_1 )
    {
        /* Message cycle time: 10ms */
        stat_u8DBL1Comm++;        
        if( stat_u8DBL1Comm == 2u )
        {
            stat_u8DBL1Comm = 0u;
            
            Safety_Comm_Timeout_Check();            
            if( g_u16DBL1NewData != 0x0000u )
            {
                /* Recieve data from DBL1 */
                Can_Receive_Data(DATA_FROM_DBL1);
                Receive_Data_From_DBL1_Process();
            }
            /* Send data to DBL1 */
            Send_Data_To_DBL1_Process();  
        }
    }
    else if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
    {
        Safety_Comm_Timeout_Check();
        /* Recieve data from DBL2 */
        if( g_u16DBL2NewData != 0x0000u )
        {
            /* Receive data from SF */
            Can_Receive_Data(DATA_FROM_DBL2);
            Receive_Data_From_DBL2();
        }

        
        /* Send data to DBL2 */
        Send_Data_To_DBL2();         
    } 
    else
    {
        /* no configure diagnostic board */
    }
    
#else
#if 0    
    static u8 can1_comm_timeout = 0u;
    u8 len = 0u;
    
    /* if config tandem, use cpu2 CAN1 to communication */
    if( TANDEM_TYPE != 0u )
    {
        if( can1_receive == 1u )
        {
            can1_receive = 0u;
            can1_comm_timeout = 0u;
            EN_ERROR7 &= ~0x10u;
        }
        else if( ++can1_comm_timeout >= 3u )
        {
            /*  can communication timeout process */
            // EN_ERROR7 |= 0x10u;
        }  
        else
        {}
        
        /** receive a data packet -----------------------------------------------**/ 
        len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0u);
        
        /* for tandem communication */
        if( len > 0u )
        {
            TandemMessageRunAllowed = 0u;
            
            if( SfBase_EscState == ESC_READY_STATE )
            {
                TandemMessageRunAllowed = CAN1_RX_Data[1];
            }
            else if( SfBase_EscState == ESC_RUN_STATE )
            {
                TandemMessageRunAllowed = CAN1_RX_Data[2];
            }
            else
            {}

        }
        CAN1_TX_Data[0] = (u8)TANDEM_TYPE;
        CAN1_TX_Data[1] = TandemRunEnable;
        CAN1_TX_Data[2] = Tandemoutput;    
        
        /** CAN1 send data ------------------------------------------------------**/
        /** tandem normal SEND ID:0x1314 **/ 
        Can_Send_Msg(CAN1, CAN1TX_NORMAL_ID, CAN1_TX_Data, CAN_FRAME_LEN ); 
    }
    
    if( testmode == 1u )
    {
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10u);
    }
#endif    
#endif
}



/******************************  END OF FILE  *********************************/