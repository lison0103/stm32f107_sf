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
static void Send_Data_To_DBL2_Process(void);
static void Receive_Data_From_DBL2_Process(void);
static void Esc_Comm_Diagnostic_Error_Process(void);
static void Safety_Comm_Timeout_Check(void);

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
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Receive_Data_From_DBL1_Process(void)
{	
    u8 i;

/*
0	0-7	LIFE SIGNAL	
1	0-7	INPUTS 1-8
2	0-7	INPUTS 9-16
3	0-7	INPUTS 17-24
4	0-7	INPUTS 25-32
5-6	0-15	CRC
7	0-7	NOT USED
*/   
    
    /*********************** DBL1 UPPER *********************************/  
    if(( DIAGNOSTIC_BOARD_L1_QUANTITY == 2u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u ))
    {     
        if( !MB_CRC16(&EscDataFromDBL1[0][0], 7u) )
        {
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL1UpperInputData[i] = EscDataFromDBL1[0][i+1u];
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
                EscRtData.DBL1UpperInputData[i] = 0u;
            }        
        }
        
        /*********************** DBL1 LOWER **********************************/
        if( !MB_CRC16(&EscDataFromDBL1[1][0], 7u) )
        {
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL1LowerInputData[i] = EscDataFromDBL1[1][i+1u];
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
                EscRtData.DBL1LowerInputData[i] = 0u;
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
                EscRtData.DBL1Interm1InputData[i] = EscDataFromDBL1[2][i+1u];
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
                EscRtData.DBL1Interm1InputData[i] = 0u;
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
                EscRtData.DBL1Interm2InputData[i] = EscDataFromDBL1[3][i+1u];
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
                EscRtData.DBL1Interm2InputData[i] = 0u;
            }          
        }    
    }
}

/*******************************************************************************
* Function Name  : Send_Data_To_DBL1_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Send_Data_To_DBL1_Process(void)
{	
    u16 crc,len;
    u16 *const pEscErrorCode = (u16*)&EscDataToDBL1[0][0];
    u8 result;
    
/*
0	0-7	LIFE SIGNAL	
1	0-7	OUTPUTS DBL1 UPPER
2	0-7	OUTPUTS DBL1 	LOWER
3	0-7	OUTPUTS DBL1 INTERM. 1
4	0-7	OUTPUTS DBL1 INTERM. 2
5-6	0-15	CRC
7	0-7	NOT USED
*/     

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
    EscDataToDBL1[0][1] = 0x00u;
    /* DBL1 LOWER */
    EscDataToDBL1[0][2] = 0x00u;
    /* DBL1 INTERM1 */
    EscDataToDBL1[0][3] = 0x00u;
    /* DBL1 INTERM2 */
    EscDataToDBL1[0][4] = 0x00u;    

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
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Receive_Data_From_DBL2_Process(void)
{	
    u8 i,j;

/*
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
    
*/    

    /********************************************************************/
    /*********************** DBL2 UPPER *********************************/
    /********************************************************************/
    if( (EscDataFromDBL2[0][3]) && (EscDataFromDBL2[1][3]) && ((EscDataFromDBL2[2][0]) || (EscDataFromDBL2[2][1])))
    {
        Can_Clean_Buffer(CAN2RX_DBL2_UPPER_ID1,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_UPPER_ID2,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_UPPER_ID3,DATA_FROM_DBL2);
        
        /* for debug */
        EscRtData.HeaderCode[1]++;
        
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DBL2ReceiveUpperDataA[i] = EscDataFromDBL2[0][i];
            EscRtData.DBL2ReceiveUpperDataB[i] = EscDataFromDBL2[1][i];
        }
        for( i = 0u; i < 4u; i++ )
        {
            EscRtData.DBL2ReceiveUpperDataA[i+8u] = EscDataFromDBL2[2][i];
            EscRtData.DBL2ReceiveUpperDataB[i+8u] = EscDataFromDBL2[2][i+4u];
        }

        Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_UPPER, SEQN_UPPER_A, DBL2_UPPER_VALIDATE, EscRtData.DBL2UpperInputData, EscRtData.DBL2ReceiveUpperDataA );
        
        /* receive finish, clear the data */
        for( j = 0u; j < 3u; j++ )
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL2[j][i] = 0u;
            }    
        } 
    }

    /********************************************************************/
    /*************************** DBL2 LOWER *****************************/
    /********************************************************************/
    if( (EscDataFromDBL2[3][3]) && (EscDataFromDBL2[4][3]) && ((EscDataFromDBL2[5][0]) || (EscDataFromDBL2[5][1])))
    {
        Can_Clean_Buffer(CAN2RX_DBL2_LOWER_ID1,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_LOWER_ID2,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_LOWER_ID3,DATA_FROM_DBL2);  
        /* for debug */
        EscRtData.HeaderCode[4]++;
        
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DBL2ReceiveLowerDataA[i] = EscDataFromDBL2[3][i];
            EscRtData.DBL2ReceiveLowerDataB[i] = EscDataFromDBL2[4][i];
        }
        for( i = 0u; i < 4u; i++ )
        {
            EscRtData.DBL2ReceiveLowerDataA[i+8u] = EscDataFromDBL2[5][i];
            EscRtData.DBL2ReceiveLowerDataB[i+8u] = EscDataFromDBL2[5][i+4u];
        }
               
        Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_LOWER, SEQN_LOWER_A, DBL2_LOWER_VALIDATE, EscRtData.DBL2LowerInputData, EscRtData.DBL2ReceiveLowerDataA ); 
        
        /* receive finish, clear the data */
        for( j = 3u; j < 6u; j++ )
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL2[j][i] = 0u;
            }    
        }                 
    }        
        

    /********************************************************************/
    /*************************** DBL2 INTERM1 ************************ **/
    /********************************************************************/
    if( (EscDataFromDBL2[6][3]) && (EscDataFromDBL2[7][3]) && ((EscDataFromDBL2[8][0]) || (EscDataFromDBL2[8][1])))
    {
        Can_Clean_Buffer(CAN2RX_DBL2_INTERM1_ID1,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_INTERM1_ID2,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_INTERM1_ID3,DATA_FROM_DBL2);
        
        /* for debug */
        EscRtData.HeaderCode[7]++;
        
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DBL2ReceiveInterm1DataA[i] = EscDataFromDBL2[6][i];
            EscRtData.DBL2ReceiveInterm1DataB[i] = EscDataFromDBL2[7][i];
        }
        for( i = 0u; i < 4u; i++ )
        {
            EscRtData.DBL2ReceiveInterm1DataA[i+8u] = EscDataFromDBL2[8][i];
            EscRtData.DBL2ReceiveInterm1DataB[i+8u] = EscDataFromDBL2[8][i+4u];
        }
        
        Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_INTERM1, SEQN_INTERM1_A, DBL2_INTERM1_VALIDATE, EscRtData.DBL2Interm1InputData, EscRtData.DBL2ReceiveInterm1DataA ); 
        
        /* receive finish, clear the data */
        for( j = 6u; j < 9u; j++ )
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL2[j][i] = 0u;
            }    
        }                 
    }    
    
    /********************************************************************/
    /*************************** DBL2 INTERM2 ***************************/
    /********************************************************************/
    if( (EscDataFromDBL2[9][3]) && (EscDataFromDBL2[10][3]) && ((EscDataFromDBL2[11][0]) || (EscDataFromDBL2[11][1])))
    {
        Can_Clean_Buffer(CAN2RX_DBL2_INTERM2_ID1,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_INTERM2_ID2,DATA_FROM_DBL2);
        Can_Clean_Buffer(CAN2RX_DBL2_INTERM2_ID3,DATA_FROM_DBL2);
        
        /* for debug */
        EscRtData.HeaderCode[10]++;
        
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DBL2ReceiveInterm2DataA[i] = EscDataFromDBL2[9][i];
            EscRtData.DBL2ReceiveInterm2DataB[i] = EscDataFromDBL2[10][i];
        }
        for( i = 0u; i < 4u; i++ )
        {
            EscRtData.DBL2ReceiveInterm2DataA[i+8u] = EscDataFromDBL2[11][i];
            EscRtData.DBL2ReceiveInterm2DataB[i+8u] = EscDataFromDBL2[11][i+4u];
        }
               
        Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_INTERM2, SEQN_INTERM2_A, DBL2_INTERM2_VALIDATE, EscRtData.DBL2Interm2InputData, EscRtData.DBL2ReceiveInterm2DataA );  
        
        /* receive finish, clear the data */
        for( j = 9u; j < 12u; j++ )
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL2[j][i] = 0u;
            }    
        }                 
    }    
    
/*
NON SAFE DATA     
Byte	Bits (7 is MSB)	Data
0-1	0-11	ANALOG INPUT 1
1-2	12-23	ANALOG INPUT 2
3	4-7	PT100
4	0-7	SAFETY SWITCH INPUTS_A (8 inputs)
5	0-7	SAFETY SWITCH INPUTS_A (8 inputs)
6	0-7	SAFETY SWITCH INPUTS_B (8 inputs)
7	0-7	SAFETY SWITCH INPUTS_B (8 inputs)
*/   

    /********************* DBL2 UPPER **********************************/
    for( i = 0u; i < 2u; i++ )
    {
        EscRtData.DBL2ReceiveUpperDataA[i+12u] = EscDataFromDBL2[12][i+4u];
        EscRtData.DBL2ReceiveUpperDataB[i+12u] = EscDataFromDBL2[12][i+6u];
    }
    EscRtData.DBL2UpperAnalogData[0] |= (u8)EscDataFromDBL2[12][0];
    EscRtData.DBL2UpperAnalogData[0] |= (u16)(((u16)EscDataFromDBL2[12][1] << 8u) & 0x0f00u);
    EscRtData.DBL2UpperAnalogData[1] |= (u8)((EscDataFromDBL2[12][1] >> 4u) & 0x0fu);
    EscRtData.DBL2UpperAnalogData[1] |= (u16)(((u16)EscDataFromDBL2[12][2] << 4u) & 0x0ff0u);
    EscRtData.DBL2UpperAnalogData[2] = EscDataFromDBL2[12][3];
    EscRtData.DBL2UpperInputData[2] = EscDataFromDBL2[12][4];
    EscRtData.DBL2UpperInputData[3] = EscDataFromDBL2[12][5];
    
    /*************************** DBL2 LOWER *****************************/
    for( i = 0u; i < 2u; i++ )
    {
        EscRtData.DBL2ReceiveLowerDataA[i+12u] = EscDataFromDBL2[13][i+4u];
        EscRtData.DBL2ReceiveLowerDataB[i+12u] = EscDataFromDBL2[13][i+6u];
    }
    EscRtData.DBL2LowerAnalogData[0] |= (u8)EscDataFromDBL2[13][0];
    EscRtData.DBL2LowerAnalogData[0] |= (u16)(((u16)EscDataFromDBL2[13][1] << 8u) & 0x0f00u);
    EscRtData.DBL2LowerAnalogData[1] |= (u8)((EscDataFromDBL2[13][1] >> 4u) & 0x0fu);
    EscRtData.DBL2LowerAnalogData[1] |= (u16)(((u16)EscDataFromDBL2[13][2] << 4u) & 0x0ff0u);
    EscRtData.DBL2LowerAnalogData[2] = EscDataFromDBL2[13][3];
    EscRtData.DBL2LowerInputData[2] = EscDataFromDBL2[13][4];
    EscRtData.DBL2LowerInputData[3] = EscDataFromDBL2[13][5];    
    
    
    /*************************** DBL2 INTERM1 **************************/
    for( i = 0u; i < 2u; i++ )
    {
        EscRtData.DBL2ReceiveInterm1DataA[i+12u] = EscDataFromDBL2[14][i+4u];
        EscRtData.DBL2ReceiveInterm1DataB[i+12u] = EscDataFromDBL2[14][i+6u];
    }
    EscRtData.DBL2Interm1AnalogData[0] |= (u8)EscDataFromDBL2[14][0];
    EscRtData.DBL2Interm1AnalogData[0] |= (u16)(((u16)EscDataFromDBL2[14][1] << 8u) & 0x0f00u);
    EscRtData.DBL2Interm1AnalogData[1] |= (u8)((EscDataFromDBL2[14][1] >> 4u) & 0x0fu);
    EscRtData.DBL2Interm1AnalogData[1] |= (u16)(((u16)EscDataFromDBL2[14][2] << 4u) & 0x0ff0u);
    EscRtData.DBL2Interm1AnalogData[2] = EscDataFromDBL2[14][3];
    EscRtData.DBL2Interm1InputData[2] = EscDataFromDBL2[14][4];
    EscRtData.DBL2Interm1InputData[3] = EscDataFromDBL2[14][5];    
    
    
    /*************************** DBL2 INTERM2 ***************************/  
    for( i = 0u; i < 2u; i++ )
    {
        EscRtData.DBL2ReceiveInterm2DataA[i+12u] = EscDataFromDBL2[15][i+4u];
        EscRtData.DBL2ReceiveInterm2DataB[i+12u] = EscDataFromDBL2[15][i+6u];
    }
    EscRtData.DBL2Interm2AnalogData[0] |= (u8)EscDataFromDBL2[15][0];
    EscRtData.DBL2Interm2AnalogData[0] |= (u16)(((u16)EscDataFromDBL2[15][1] << 8u) & 0x0f00u);
    EscRtData.DBL2Interm2AnalogData[1] |= (u8)((EscDataFromDBL2[15][1] >> 4u) & 0x0fu);
    EscRtData.DBL2Interm2AnalogData[1] |= (u16)(((u16)EscDataFromDBL2[15][2] << 4u) & 0x0ff0u);
    EscRtData.DBL2Interm2AnalogData[2] = EscDataFromDBL2[15][3];
    EscRtData.DBL2Interm2InputData[2] = EscDataFromDBL2[15][4];
    EscRtData.DBL2Interm2InputData[3] = EscDataFromDBL2[15][5];     
    
}


/*******************************************************************************
* Function Name  : Send_Data_To_DBL2_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Send_Data_To_DBL2_Process(void)
{	
    u8 i,result;
/*
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
    
*/     

    /********************************************************************/
    /*************************** DBL2 UPPER *****************************/
    /********************************************************************/
    /*if( (OmcEscRtData.DBL2SendUpperData[1]) && (OmcEscRtData.DBL2SendUpperData[2]) )*/
    if( !MB_CRC16(&OmcEscRtData.DBL2SendUpperData[0], 7u ) )
    {
        if( (OmcEscRtData.DBL2SendUpperData[0] & 0x03u) == CONNECTION_DBL2_UPPER )
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_UPPER, &SEQN_UPPER_A, EscRtData.DBL2SendUpperData, 1u);
            /*EscRtData.DBL2ValidateResult &= ~DBL2_UPPER_VALIDATE;*/
            
            EscDataToDBL2[0][0] |= (u8)(EscRtData.DBL2SendUpperData[0] & 0x0fu);
            EscDataToDBL2[0][0] |= (u8)((OmcEscRtData.DBL2SendUpperData[0] << 4u) & 0xf0u);
            EscDataToDBL2[0][1] = EscRtData.DBL2SendUpperData[1]; 
            EscDataToDBL2[0][2] = OmcEscRtData.DBL2SendUpperData[1];   
            EscDataToDBL2[0][3] |= (u8)((EscRtData.DBL2SendUpperData[0] >> 4u) & 0x0fu);
            EscDataToDBL2[0][3] |= (u8)(OmcEscRtData.DBL2SendUpperData[0] & 0xf0u);
            EscDataToDBL2[0][4] = EscRtData.DBL2SendUpperData[2]; 
            EscDataToDBL2[0][5] = OmcEscRtData.DBL2SendUpperData[2];      
            EscDataToDBL2[0][6] = EscRtData.DBL2SendUpperData[3];   
            EscDataToDBL2[0][7] = 0x00u; 
            
            EscDataToDBL2[1][0] = EscRtData.DBL2SendUpperData[4];   
            EscDataToDBL2[1][1] = EscRtData.DBL2SendUpperData[5]; 
            EscDataToDBL2[1][2] = EscRtData.DBL2SendUpperData[6];   
            EscDataToDBL2[1][3] = EscRtData.DBL2SendUpperData[7]; 
            EscDataToDBL2[1][4] = OmcEscRtData.DBL2SendUpperData[3];   
            EscDataToDBL2[1][5] = OmcEscRtData.DBL2SendUpperData[4]; 
            EscDataToDBL2[1][6] = OmcEscRtData.DBL2SendUpperData[5];   
            EscDataToDBL2[1][7] = OmcEscRtData.DBL2SendUpperData[6];    
            
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_UPPER_ID1, &EscDataToDBL2[0][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x01u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);               
            }  
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_UPPER_ID2, &EscDataToDBL2[1][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x02u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);        
            }   
            /* for debug */
            EscRtData.HeaderCode[0]++;
        }
    }
    
    /* clear data */
    Safety_Send_Data_Process(CONNECTION_DBL2_UPPER, NULL, EscRtData.DBL2SendUpperData, 0u);
    for( i = 0u; i < 8u; i++ )
    {
        OmcEscRtData.DBL2SendUpperData[i] = 0u;
    }
    
    /********************************************************************/
    /*************************** DBL2 LOWER *****************************/
    /********************************************************************/
    /*if( (OmcEscRtData.DBL2SendLowerData[1]) && (OmcEscRtData.DBL2SendLowerData[2]) )*/
    if( !MB_CRC16(&OmcEscRtData.DBL2SendLowerData[0], 7u ) )
    {
        if( (OmcEscRtData.DBL2SendLowerData[0] & 0x03u) == CONNECTION_DBL2_LOWER )
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_LOWER, &SEQN_LOWER_A, EscRtData.DBL2SendLowerData, 1u);
            /*EscRtData.DBL2ValidateResult &= ~DBL2_LOWER_VALIDATE;*/
            
            EscDataToDBL2[2][0] |= (u8)(EscRtData.DBL2SendLowerData[0] & 0x0fu);
            EscDataToDBL2[2][0] |= (u8)((OmcEscRtData.DBL2SendLowerData[0] << 4u) & 0xf0u);
            EscDataToDBL2[2][1] = EscRtData.DBL2SendLowerData[1]; 
            EscDataToDBL2[2][2] = OmcEscRtData.DBL2SendLowerData[1];   
            EscDataToDBL2[2][3] |= (u8)((EscRtData.DBL2SendLowerData[0] >> 4u) & 0x0fu);
            EscDataToDBL2[2][3] |= (u8)(OmcEscRtData.DBL2SendLowerData[0] & 0xf0u);
            EscDataToDBL2[2][4] = EscRtData.DBL2SendLowerData[2]; 
            EscDataToDBL2[2][5] = OmcEscRtData.DBL2SendLowerData[2];      
            EscDataToDBL2[2][6] = EscRtData.DBL2SendLowerData[3];   
            EscDataToDBL2[2][7] = 0x00u; 
            
            EscDataToDBL2[3][0] = EscRtData.DBL2SendLowerData[4];   
            EscDataToDBL2[3][1] = EscRtData.DBL2SendLowerData[5]; 
            EscDataToDBL2[3][2] = EscRtData.DBL2SendLowerData[6];   
            EscDataToDBL2[3][3] = EscRtData.DBL2SendLowerData[7]; 
            EscDataToDBL2[3][4] = OmcEscRtData.DBL2SendLowerData[3];   
            EscDataToDBL2[3][5] = OmcEscRtData.DBL2SendLowerData[4]; 
            EscDataToDBL2[3][6] = OmcEscRtData.DBL2SendLowerData[5];   
            EscDataToDBL2[3][7] = OmcEscRtData.DBL2SendLowerData[6];    
            
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_LOWER_ID1, &EscDataToDBL2[2][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x04u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);
            }  
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_LOWER_ID2, &EscDataToDBL2[3][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x08u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            /* for debug */
            EscRtData.HeaderCode[3]++;
        }         
    }

    /* clear data */
    Safety_Send_Data_Process(CONNECTION_DBL2_LOWER, &SEQN_LOWER_A, EscRtData.DBL2SendLowerData, 0u);
    for( i = 0u; i < 8u; i++ )
    {
        OmcEscRtData.DBL2SendLowerData[i] = 0u;
    }    
    

    /********************************************************************/
    /*************************** DBL2 INTERM1 ***************************/
    /********************************************************************/ 
    if( !MB_CRC16(&OmcEscRtData.DBL2SendInterm1Data[0], 7u ) )
    {
        if( (OmcEscRtData.DBL2SendInterm1Data[0] & 0x03u) == CONNECTION_DBL2_INTERM1 )
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_INTERM1, &SEQN_INTERM1_A, EscRtData.DBL2SendInterm1Data, 1u);
            /*EscRtData.DBL2ValidateResult &= ~DBL2_LOWER_VALIDATE;*/
            
            EscDataToDBL2[4][0] |= (u8)(EscRtData.DBL2SendInterm1Data[0] & 0x0fu);
            EscDataToDBL2[4][0] |= (u8)((OmcEscRtData.DBL2SendInterm1Data[0] << 4u) & 0xf0u);
            EscDataToDBL2[4][1] = EscRtData.DBL2SendInterm1Data[1]; 
            EscDataToDBL2[4][2] = OmcEscRtData.DBL2SendInterm1Data[1];   
            EscDataToDBL2[4][3] |= (u8)((EscRtData.DBL2SendInterm1Data[0] >> 4u) & 0x0fu);
            EscDataToDBL2[4][3] |= (u8)(OmcEscRtData.DBL2SendInterm1Data[0] & 0xf0u);
            EscDataToDBL2[4][4] = EscRtData.DBL2SendInterm1Data[2]; 
            EscDataToDBL2[4][5] = OmcEscRtData.DBL2SendInterm1Data[2];      
            EscDataToDBL2[4][6] = EscRtData.DBL2SendInterm1Data[3];   
            EscDataToDBL2[4][7] = 0x00u; 
            
            EscDataToDBL2[5][0] = EscRtData.DBL2SendInterm1Data[4];   
            EscDataToDBL2[5][1] = EscRtData.DBL2SendInterm1Data[5]; 
            EscDataToDBL2[5][2] = EscRtData.DBL2SendInterm1Data[6];   
            EscDataToDBL2[5][3] = EscRtData.DBL2SendInterm1Data[7]; 
            EscDataToDBL2[5][4] = OmcEscRtData.DBL2SendInterm1Data[3];   
            EscDataToDBL2[5][5] = OmcEscRtData.DBL2SendInterm1Data[4]; 
            EscDataToDBL2[5][6] = OmcEscRtData.DBL2SendInterm1Data[5];   
            EscDataToDBL2[5][7] = OmcEscRtData.DBL2SendInterm1Data[6];    
            
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM1_ID1, &EscDataToDBL2[4][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x10u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);
            }  
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM1_ID2, &EscDataToDBL2[5][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x20u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            /* for debug */
            EscRtData.HeaderCode[6]++;
        }         
    }

    /* clear data */
    Safety_Send_Data_Process(CONNECTION_DBL2_INTERM1, &SEQN_INTERM1_A, EscRtData.DBL2SendInterm1Data, 0u);
    for( i = 0u; i < 8u; i++ )
    {
        OmcEscRtData.DBL2SendInterm1Data[i] = 0u;
    }      
    
    
    /********************************************************************/
    /*************************** DBL2 INTERM2 ***************************/
    /********************************************************************/ 
    if( !MB_CRC16(&OmcEscRtData.DBL2SendInterm2Data[0], 7u ) )
    {
        if( (OmcEscRtData.DBL2SendInterm2Data[0] & 0x03u) == CONNECTION_DBL2_INTERM2 )
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_INTERM2, &SEQN_INTERM2_A, EscRtData.DBL2SendInterm2Data, 1u);
            /*EscRtData.DBL2ValidateResult &= ~DBL2_LOWER_VALIDATE;*/
            
            EscDataToDBL2[6][0] |= (u8)(EscRtData.DBL2SendInterm2Data[0] & 0x0fu);
            EscDataToDBL2[6][0] |= (u8)((OmcEscRtData.DBL2SendInterm2Data[0] << 4u) & 0xf0u);
            EscDataToDBL2[6][1] = EscRtData.DBL2SendInterm2Data[1]; 
            EscDataToDBL2[6][2] = OmcEscRtData.DBL2SendInterm2Data[1];   
            EscDataToDBL2[6][3] |= (u8)((EscRtData.DBL2SendInterm2Data[0] >> 4u) & 0x0fu);
            EscDataToDBL2[6][3] |= (u8)(OmcEscRtData.DBL2SendInterm2Data[0] & 0xf0u);
            EscDataToDBL2[6][4] = EscRtData.DBL2SendInterm2Data[2]; 
            EscDataToDBL2[6][5] = OmcEscRtData.DBL2SendInterm2Data[2];      
            EscDataToDBL2[6][6] = EscRtData.DBL2SendInterm2Data[3];   
            EscDataToDBL2[6][7] = 0x00u; 
            
            EscDataToDBL2[7][0] = EscRtData.DBL2SendInterm2Data[4];   
            EscDataToDBL2[7][1] = EscRtData.DBL2SendInterm2Data[5]; 
            EscDataToDBL2[7][2] = EscRtData.DBL2SendInterm2Data[6];   
            EscDataToDBL2[7][3] = EscRtData.DBL2SendInterm2Data[7]; 
            EscDataToDBL2[7][4] = OmcEscRtData.DBL2SendInterm2Data[3];   
            EscDataToDBL2[7][5] = OmcEscRtData.DBL2SendInterm2Data[4]; 
            EscDataToDBL2[7][6] = OmcEscRtData.DBL2SendInterm2Data[5];   
            EscDataToDBL2[7][7] = OmcEscRtData.DBL2SendInterm2Data[6];    
            
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM2_ID1, &EscDataToDBL2[6][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x40u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);
            }  
            result = Can_Send_Msg(CAN2, CAN2TX_DBL2_INTERM2_ID2, &EscDataToDBL2[7][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
                g_u16CAN2SendFail |= 0x80u;
                CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
            }  
            /* for debug */
            EscRtData.HeaderCode[9]++;
        }         
    }

    /* clear data */
    Safety_Send_Data_Process(CONNECTION_DBL2_INTERM2, &SEQN_INTERM2_A, EscRtData.DBL2SendInterm2Data, 0u);
    for( i = 0u; i < 8u; i++ )
    {
        OmcEscRtData.DBL2SendInterm2Data[i] = 0u;
    } 
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
                /* for debug */
                /*   EscRtData.HeaderCode[0] = 0u;
                EscRtData.HeaderCode[1] = 0u;
                EscRtData.HeaderCode[2] = 0u;*/
                /*  can communication timeout process */
                EN_ERROR7 |= 0x08u;
                Esc_Comm_Diagnostic_Error_Process();
            }
        }
        else
        {
            /* for debug */
            EscRtData.HeaderCode[0] = 0u;
            EscRtData.HeaderCode[1] = 0u;
            EscRtData.HeaderCode[2] = 0u;
            EscRtData.HeaderCode[3] = 0u;
            EscRtData.HeaderCode[4] = 0u;
            EscRtData.HeaderCode[5] = 0u;
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
            Receive_Data_From_DBL2_Process();
        }

        
        /* Send data to DBL2 */
        Send_Data_To_DBL2_Process();         
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