/*******************************************************************************
* File Name          : esc_comm_safety.c
* Author             : lison
* Version            : V1.0
* Date               : 10/26/2016
* Description        : This file contains esc safety communication protocol process.  
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_comm_safety.h"
#include "can.h"
#include "crc16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CAN_COMM_HAND_TIME      8000u
#define SMVT_TIMEOUT            50u
#define SMCT_TIME               50u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Safety_ReceiveA_Diagnostic(u8 DBL2Type, u8 DBL2InputData[], u8 DBL2ReceiveDataA[] );


#ifdef GEC_SF_MASTER
u8 SEQN_UPPER_A = 1u,SEQN_LOWER_A = 1u,SEQN_INTERM1_A = 1u,SEQN_INTERM2_A = 1u;
#else
static u8 SEQN_UPPER_B = 250u,SEQN_LOWER_B = 250u,SEQN_INTERM1_B = 250u,SEQN_INTERM2_B = 250u;
#endif

/* for test */
u8 g_u8DBL2Respone = 0u;

#ifdef GEC_SF_S_NEW 
/*******************************************************************************
* Function Name  : Safety_Request_Data
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void Safety_Request_Data(void)
{	
    static u16 stat_u16TimerCommWait = 0u, stat_u16HandshakeSuccess = 0u;
    static u16 stat_u16TimerSMCT = 0u;
    static u16 stat_u16TimerSMVT = 0u;
    
    
    /* SMCT cycle send data */
    if( (stat_u16TimerSMCT == 0u) && (!(EN_ERROR7 & 0x40u)) )
    {
        stat_u16TimerSMVT = 0u;
        stat_u16TimerSMCT++;
        stat_u16TimerSMVT++;
        EscRtData.DBL2ValidateResult = 0x00u;
        
        /* for debug */
        EscRtData.HeaderCode[0]++;
        
        if( DIAGNOSTIC_BOARD_L2_QUANTITY == 2u )
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_UPPER, &SEQN_UPPER_B, EscRtData.DBL2SendUpperData, 1u);
            /*Safety_Send_Data_Process(CONNECTION_DBL2_LOWER, &SEQN_LOWER_B, EscRtData.DBL2SendLowerData, 1u);*/
            Safety_Send_Data_Process(CONNECTION_DBL2_INTERM2, &SEQN_INTERM2_B, EscRtData.DBL2SendInterm2Data, 1u);
        }
        else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u )
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_UPPER, &SEQN_UPPER_B, EscRtData.DBL2SendUpperData, 1u);
            Safety_Send_Data_Process(CONNECTION_DBL2_LOWER, &SEQN_LOWER_B, EscRtData.DBL2SendLowerData, 1u);
            Safety_Send_Data_Process(CONNECTION_DBL2_INTERM1, &SEQN_INTERM1_B, EscRtData.DBL2SendInterm1Data, 1u);
        }
        else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_UPPER, &SEQN_UPPER_B, EscRtData.DBL2SendUpperData, 1u);
            Safety_Send_Data_Process(CONNECTION_DBL2_LOWER, &SEQN_LOWER_B, EscRtData.DBL2SendLowerData, 1u);
            Safety_Send_Data_Process(CONNECTION_DBL2_INTERM1, &SEQN_INTERM1_B, EscRtData.DBL2SendInterm1Data, 1u);
            Safety_Send_Data_Process(CONNECTION_DBL2_INTERM2, &SEQN_INTERM2_B, EscRtData.DBL2SendInterm2Data, 1u);
        }
        else
        {
            Safety_Send_Data_Process(CONNECTION_DBL2_LOWER, &SEQN_LOWER_B, EscRtData.DBL2SendLowerData, 1u);
        }
    }
    else
    {     
        stat_u16TimerSMCT++;
        stat_u16TimerSMVT++;
        if( ( stat_u16TimerSMCT * SYSTEMTICK ) > SMCT_TIME )
        {
            stat_u16TimerSMCT = 0u; 
        }
    }
    
    /* SMVT timeout */   
    if( DIAGNOSTIC_BOARD_L2_QUANTITY == 2u )
    {
        if( g_u8DBL2Respone == 0x09u )
        {
            g_u8DBL2Respone = 0u;
            stat_u16HandshakeSuccess = 1u;
            stat_u16TimerSMVT = 0u;
        }
        else
        {
            if( stat_u16HandshakeSuccess == 1u )
            {
                if( ( stat_u16TimerSMVT * SYSTEMTICK ) > SMVT_TIMEOUT )
                {
                    EN_ERROR7 |= 0x40u;
                }
            }
        }
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u )
    {
        if( g_u8DBL2Respone == 0x07u )
        {
            g_u8DBL2Respone = 0u;
            stat_u16HandshakeSuccess = 1u;
            stat_u16TimerSMVT = 0u;
        }
        else
        {
            if( stat_u16HandshakeSuccess == 1u )
            {
                if( ( stat_u16TimerSMVT * SYSTEMTICK ) > SMVT_TIMEOUT )
                {
                    EN_ERROR7 |= 0x40u;
                }
            }
        }
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
    {
        if( g_u8DBL2Respone == 0x0fu )
        {
            g_u8DBL2Respone = 0u;
            stat_u16HandshakeSuccess = 1u;
            stat_u16TimerSMVT = 0u;
        }
        else
        {
            if( stat_u16HandshakeSuccess == 1u )
            {
                if( ( stat_u16TimerSMVT * SYSTEMTICK ) > SMVT_TIMEOUT )
                {
                    EN_ERROR7 |= 0x40u;
                }
            }
        }
    }
    else
    {
        if( g_u8DBL2Respone == 0x02u )
        {
            g_u8DBL2Respone = 0u;
            stat_u16HandshakeSuccess = 1u;
            stat_u16TimerSMVT = 0u;
        }
        else
        {
            if( stat_u16HandshakeSuccess == 1u )
            {
                if( ( stat_u16TimerSMVT * SYSTEMTICK ) > SMVT_TIMEOUT )
                {
                    EN_ERROR7 |= 0x40u;
                }
            }
        }
    }
    
    if( stat_u16HandshakeSuccess == 0u )
    {
        EscRtData.HeaderCode[0] = 0u;
        stat_u16TimerCommWait++;
        if(( stat_u16TimerCommWait * SYSTEMTICK ) > CAN_COMM_HAND_TIME )
        {
            /*  can communication handshake timeout when power on */ 
            EN_ERROR7 |= 0x40u;
        } 
    }
}
#endif

/*******************************************************************************
* Function Name  : Safety_Request_Data
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Safety_ReceiveA_Diagnostic(u8 DBL2Type, u8 DBL2InputData[], u8 DBL2ReceiveDataA[] )
{
    u8 i;
    
    if(( EscRtData.DBL2ValidateResult & DBL2Type ) && ( OmcEscRtData.DBL2ValidateResult & DBL2Type ))
    {
        /* data ok */
        DBL2InputData[0] = DBL2ReceiveDataA[1];
        DBL2InputData[1] = DBL2ReceiveDataA[2];
        
        EscRtData.DBL2ValidateResult &= (u8)(~DBL2Type); 
        
        /* clear receive data */
        for( i = 0u; i < 12u; i++ )
        {
            DBL2ReceiveDataA[i] = 0u;
        }            
    }
    else
    {
        
    }    
}
      
/*******************************************************************************
* Function Name  : Safety_Request_Data
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void Safety_ReceiveB_Diagnostic(u8 Connection, u8 SEQN, u8 DBL2Type, u8 DBL2InputData[], u8 DBL2ReceiveData[] )
{
    u8 i;
    u8 u8_ValidateError = 0u;
    /* DBL2 UPPER */
    /* Validation Response: Check CONNECTION, SEQN, CRC and compare _1 and _2 data */
    /* check CRC */
    if( !MB_CRC16( &DBL2ReceiveData[0], 12u ))
    {  
        /* compare CONNECTION */
        if((DBL2ReceiveData[0] & 0x03u) != (DBL2ReceiveData[4] & 0x03u))
        {
            u8_ValidateError = 1u;
        }
        /* compare FAULT_STATUS */
        else if((DBL2ReceiveData[0] & 0xfcu) != (DBL2ReceiveData[0] & 0xfcu))
        {
            u8_ValidateError = 1u;
        }     
        /* compare INPUT */
        else if( ((DBL2ReceiveData[1]) != (DBL2ReceiveData[5])) || ((DBL2ReceiveData[2]) != (DBL2ReceiveData[6])) )
        {
            u8_ValidateError = 1u;
        }              
        /* compare SEQN */
        else if( DBL2ReceiveData[3] != DBL2ReceiveData[7] )
        {
            u8_ValidateError = 1u;
        }
        else
        {   
            /* check CONNECTION */
            if( (DBL2ReceiveData[0] & 0x03u) != Connection )
            {
                u8_ValidateError = 1u;
            }
            /* check SEQN */
            else if( DBL2ReceiveData[3] != SEQN )
            {
                u8_ValidateError = 1u;
            }
            else
            {
                u8_ValidateError = 0u;
            }
            
            if( u8_ValidateError == 0u )
            {  
                /* validate data */
                EscRtData.DBL2ValidateResult |= DBL2Type;    
#ifdef GEC_SF_S_NEW                 
                if( OmcEscRtData.DBL2ValidateResult & DBL2Type )
                {
                    /* A & B validate data correct */
                    g_u8DBL2Respone |= DBL2Type;
                    DBL2InputData[0] = DBL2ReceiveData[1];
                    DBL2InputData[1] = DBL2ReceiveData[2];
                } 
#endif                
            }
        }
    }
#ifdef GEC_SF_S_NEW   
    /* clear data */
    for( i = 0u; i < 12u; i++ )
    {
        DBL2ReceiveData[i] = 0u;
    }
#endif
}
 

/*******************************************************************************
* Function Name  : Safety_Receive_Data_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void Safety_Receive_Data_Process(void)
{	

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

#ifdef GEC_SF_MASTER
    Safety_ReceiveA_Diagnostic(DBL2_UPPER_VALIDATE, EscRtData.DBL2UpperInputData, EscRtData.DBL2ReceiveUpperDataA );
    Safety_ReceiveA_Diagnostic(DBL2_LOWER_VALIDATE, EscRtData.DBL2LowerInputData, EscRtData.DBL2ReceiveLowerDataA );
    Safety_ReceiveA_Diagnostic(DBL2_INTERM1_VALIDATE, EscRtData.DBL2Interm1InputData, EscRtData.DBL2ReceiveInterm1DataA );
    Safety_ReceiveA_Diagnostic(DBL2_INTERM2_VALIDATE, EscRtData.DBL2Interm2InputData, EscRtData.DBL2ReceiveInterm2DataA );
#else
    Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_UPPER, SEQN_UPPER_B, DBL2_UPPER_VALIDATE, EscRtData.DBL2UpperInputData, OmcEscRtData.DBL2ReceiveUpperDataB );
    Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_LOWER, SEQN_LOWER_B, DBL2_LOWER_VALIDATE, EscRtData.DBL2LowerInputData, OmcEscRtData.DBL2ReceiveLowerDataB );
    Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_INTERM1, SEQN_INTERM1_B, DBL2_INTERM1_VALIDATE, EscRtData.DBL2Interm1InputData, OmcEscRtData.DBL2ReceiveInterm1DataB );
    Safety_ReceiveB_Diagnostic(CONNECTION_DBL2_INTERM2, SEQN_INTERM2_B, DBL2_INTERM2_VALIDATE, EscRtData.DBL2Interm2InputData, OmcEscRtData.DBL2ReceiveInterm2DataB );
    
#endif
}


/*******************************************************************************
* Function Name  : Safety_Send_Data_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void Safety_Send_Data_Process(u8 connection, u8 *SEQN, u8 DBL2SendData[], u8 request)
{	
    u16 crc,len;
    u8 i;
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
    
    if( request )
    {                        
        /* DBL2 UPPER */
#ifdef GEC_SF_MASTER
        
        (*SEQN)++;
        if( *SEQN > 250u )
        {
            *SEQN = 1u;
        }
        
        /* CONNECTION_A_1 */
        DBL2SendData[0] |= connection;
        /* RESET_A_1 */
        DBL2SendData[0] |= 0x00u << 2u;
        /* CONNECTION_A_2 */
        DBL2SendData[0] |= (u8)(connection << 4u);
        /* RESET_A_2 */
        DBL2SendData[0] |= 0x00u << 6u;    
        /* SEQN_A_1 */
        DBL2SendData[1] = *SEQN;
        /* SEQN_A_2 */
        DBL2SendData[2] = *SEQN;
        /* OUTPUTS (No safety relevant) */
        DBL2SendData[3] = 0x01u;    
        /* CRC_A */
        len = 6u;
        crc = MB_CRC16( &DBL2SendData[0], len - 2u );
        DBL2SendData[4] = (u8)crc;
        DBL2SendData[5] = (u8)(crc >> 8u);     
        DBL2SendData[6] = 0x00u;
        DBL2SendData[7] = 0x00u;
#else
        (*SEQN)--;
        if( *SEQN == 1u )
        {
            *SEQN = 250u;
        }
        
        /* DBL2 UPPER */
        /* CONNECTION_B_1 */
        DBL2SendData[0] |= connection;
        /* RESET_B_1 */
        DBL2SendData[0] |= 0x00u << 2u;
        /* CONNECTION_B_2 */
        DBL2SendData[0] |= (u8)(connection << 4u);
        /* RESET_B_2 */
        DBL2SendData[0] |= 0x00u << 6u;    
        /* SEQN_B_1 */
        DBL2SendData[1] = *SEQN;
        /* SEQN_B_2 */
        DBL2SendData[2] = *SEQN;
        
        /* CRC_B */
        len = 5u;
        crc = MB_CRC16( &DBL2SendData[0], len - 2u );
        DBL2SendData[3] = (u8)crc;
        DBL2SendData[4] = (u8)(crc >> 8u);     
        DBL2SendData[5] = 0x00u;
        DBL2SendData[6] = 0x00u;
#endif       
    }
    else
    {
        for( i = 0u; i < 8u; i++ )
        {
            DBL2SendData[i] = 0u;
        }
    }
}

/******************************  END OF FILE  *********************************/