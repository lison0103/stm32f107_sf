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
#define SMVT_TIMEOUT            80u
#define SMCT_TIME               80u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#ifdef GEC_SF_MASTER
static void Safety_ReceiveA_Diagnostic( DBL2Esc *SFData );
#else
static void Safety_Comm_Data_Init(DBL2Esc *SFData, u8 connection, u8 boardtype, u8 SEQN);
static void Safety_Receive_Diagnostic_Validate( DBL2Esc *SFData, u8 DBL2ReceiveData[] );
#endif

static u8 g_u8DBL2Respone = 0u;


/*******************************************************************************
* Function Name  : Safety_Comm_Data_Init
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
#ifdef GEC_SF_MASTER
void Safety_Comm_Data_Init(DBL2Esc *SFData, u8 connection, u8 boardtype, u8 SEQN)
#else
static void Safety_Comm_Data_Init(DBL2Esc *SFData, u8 connection, u8 boardtype, u8 SEQN)
#endif
{	
    u8 i;
    
    SFData->BoardType = boardtype;
    SFData->Connection = connection;
    SFData->SEQN = SEQN;
    
    for( i = 0u; i < 8u; i++ )
    {
        SFData->SendData[i] = 0u;
    }
    
    for( i = 0u; i < 14u; i++ )
    {
        SFData->ReceiveDataA[i] = 0u;
        SFData->ReceiveDataB[i] = 0u;
    }    
}

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
    static u8 stat_u8FirstEnter = 0u;
    
    if( stat_u8FirstEnter == 0u )
    {
        stat_u8FirstEnter = 1u;
        Safety_Comm_Data_Init(&EscRtData.DBL2Upper, CONNECTION_DBL2_UPPER, DBL2_TYPE_UPPER, 250u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Lower, CONNECTION_DBL2_LOWER, DBL2_TYPE_LOWER, 250u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Interm1, CONNECTION_DBL2_INTERM1, DBL2_TYPE_INTERM1, 250u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Interm2, CONNECTION_DBL2_INTERM2, DBL2_TYPE_INTERM2, 250u);
    }
    
    
    /* SMCT cycle send data */
    if( (stat_u16TimerSMCT == 0u) && (!(EN_ERROR48 & 0x02u)) )
    {
        stat_u16TimerSMVT = 0u;
        stat_u16TimerSMCT++;
        stat_u16TimerSMVT++;
        EscRtData.DBL2ValidateResult = 0x00u;
        
        /* for debug */
        EscRtData.HeaderCode[0]++;
        
        if( DIAGNOSTIC_BOARD_L2_QUANTITY == 2u )
        {
            Safety_Send_Data_Process(&EscRtData.DBL2Upper, 1u);
            Safety_Send_Data_Process(&EscRtData.DBL2Interm2, 1u);
        }
        else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u )
        {
            Safety_Send_Data_Process(&EscRtData.DBL2Upper, 1u);
            Safety_Send_Data_Process(&EscRtData.DBL2Lower, 1u);
            Safety_Send_Data_Process(&EscRtData.DBL2Interm1, 1u);
        }
        else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
        {
            Safety_Send_Data_Process(&EscRtData.DBL2Upper, 1u);
            Safety_Send_Data_Process(&EscRtData.DBL2Lower, 1u);
            Safety_Send_Data_Process(&EscRtData.DBL2Interm1, 1u);
            Safety_Send_Data_Process(&EscRtData.DBL2Interm2, 1u);
        }
        else
        {
            Safety_Send_Data_Process(&EscRtData.DBL2Lower, 1u);
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
        if( g_u8DBL2Respone == ( DBL2_TYPE_UPPER | DBL2_TYPE_INTERM2 ) )
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
                    /* Communication error diag DBL2 F377 */
                    EN_ERROR48 |= 0x02u;
                }
            }
        }
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u )
    {
        if( g_u8DBL2Respone == ( DBL2_TYPE_UPPER | DBL2_TYPE_LOWER | DBL2_TYPE_INTERM1 ) )
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
                    /* Communication error diag DBL2 F377 */
                    EN_ERROR48 |= 0x02u;
                }
            }
        }
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
    {
        if( g_u8DBL2Respone == ( DBL2_TYPE_UPPER | DBL2_TYPE_LOWER | DBL2_TYPE_INTERM1 | DBL2_TYPE_INTERM2 ) )
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
                    /* Communication error diag DBL2 F377 */
                    EN_ERROR48 |= 0x02u;
                }
            }
        }
    }
    else
    {
        if( g_u8DBL2Respone == DBL2_TYPE_LOWER )
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
                    /* Communication error diag DBL2 F377 */
                    EN_ERROR48 |= 0x02u;
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
            /* Communication error diag DBL2 F377 */
            EN_ERROR48 |= 0x02u;
        } 
    }
}
#endif

#ifdef GEC_SF_MASTER
/*******************************************************************************
* Function Name  : Safety_Request_Data
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Safety_ReceiveA_Diagnostic( DBL2Esc *SFData )
{
    u8 i;
    
    if(( EscRtData.DBL2ValidateResult & SFData->BoardType ) && ( OmcEscRtData.DBL2ValidateResult & SFData->BoardType ))
    {
        /* data ok */
        SFData->InputData[0] = SFData->ReceiveDataA[1];
        SFData->InputData[1] = SFData->ReceiveDataA[2];
        
        EscRtData.DBL2ValidateResult &= (u8)(~SFData->BoardType); 
        
        /* clear receive data */
        for( i = 0u; i < 12u; i++ )
        {
            SFData->ReceiveDataA[i] = 0u;
        }            
    }   
}
#endif
      
/*******************************************************************************
* Function Name  : Safety_Receive_Diagnostic_Validate
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
#ifdef GEC_SF_MASTER
void Safety_Receive_Diagnostic_Validate( DBL2Esc *SFData, u8 DBL2ReceiveData[] )
#else
static void Safety_Receive_Diagnostic_Validate( DBL2Esc *SFData, u8 DBL2ReceiveData[] )
#endif
{
    u8 i;
    u8 u8_ValidateError = 0u;

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
            if( (DBL2ReceiveData[0] & 0x03u) != SFData->Connection )
            {
                u8_ValidateError = 1u;
            }
            /* check SEQN */
            else if( DBL2ReceiveData[3] != SFData->SEQN )
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
                EscRtData.DBL2ValidateResult |= SFData->BoardType;    
#ifdef GEC_SF_S_NEW                 
                if( OmcEscRtData.DBL2ValidateResult & SFData->BoardType )
                {
                    /* A & B validate data correct */
                    g_u8DBL2Respone |= SFData->BoardType;
                    SFData->InputData[0] = DBL2ReceiveData[1];
                    SFData->InputData[1] = DBL2ReceiveData[2];
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

*
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void Safety_Receive_Data_Process(void)
{	 

#ifdef GEC_SF_MASTER
    Safety_ReceiveA_Diagnostic( &EscRtData.DBL2Upper );
    Safety_ReceiveA_Diagnostic( &EscRtData.DBL2Lower );
    Safety_ReceiveA_Diagnostic( &EscRtData.DBL2Interm1 );
    Safety_ReceiveA_Diagnostic( &EscRtData.DBL2Interm2 );
#else
    Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Upper, OmcEscRtData.DBL2Upper.ReceiveDataB );
    Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Lower, OmcEscRtData.DBL2Lower.ReceiveDataB );
    Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Interm1, OmcEscRtData.DBL2Interm1.ReceiveDataB );
    Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Interm2, OmcEscRtData.DBL2Interm2.ReceiveDataB );
    
#endif
}


/*******************************************************************************
* Function Name  : Safety_Send_Data_Process
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
void Safety_Send_Data_Process(DBL2Esc *SFData, u8 request)
{	
    u16 crc,len;
    u8 i;    
    
    if( request )
    {                        
#ifdef GEC_SF_MASTER
        
        SFData->SEQN++;
        if( SFData->SEQN > 250u )
        {
            SFData->SEQN = 1u;
        }
        
        /* CONNECTION_A_1 */
        SFData->SendData[0] |= SFData->Connection;
        /* RESET_A_1 */
        SFData->SendData[0] |= 0x00u << 2u;
        /* CONNECTION_A_2 */
        SFData->SendData[0] |= (u8)(SFData->Connection << 4u);
        /* RESET_A_2 */
        SFData->SendData[0] |= 0x00u << 6u;    
        /* SEQN_A_1 */
        SFData->SendData[1] = SFData->SEQN;
        /* SEQN_A_2 */
        SFData->SendData[2] = SFData->SEQN;
        /* OUTPUTS (No safety relevant) */
#ifdef ESC_TEST        
        SFData->SendData[3] = 0x01u;
#else
        SFData->SendData[3] = SFData->OutputData ;    
#endif
        /* CRC_A */
        len = 6u;
        crc = MB_CRC16( &SFData->SendData[0], len - 2u );
        SFData->SendData[4] = (u8)crc;
        SFData->SendData[5] = (u8)(crc >> 8u);     
        SFData->SendData[6] = 0x00u;
        SFData->SendData[7] = 0x00u;
#else
        (SFData->SEQN)--;
        if( SFData->SEQN == 1u )
        {
            SFData->SEQN = 250u;
        }
        
        /* DBL2 UPPER */
        /* CONNECTION_B_1 */
        SFData->SendData[0] |= SFData->Connection;
        /* RESET_B_1 */
        SFData->SendData[0] |= 0x00u << 2u;
        /* CONNECTION_B_2 */
        SFData->SendData[0] |= (u8)(SFData->Connection << 4u);
        /* RESET_B_2 */
        SFData->SendData[0] |= 0x00u << 6u;    
        /* SEQN_B_1 */
        SFData->SendData[1] = SFData->SEQN;
        /* SEQN_B_2 */
        SFData->SendData[2] = SFData->SEQN;
        
        /* CRC_B */
        len = 5u;
        crc = MB_CRC16( &SFData->SendData[0], len - 2u );
        SFData->SendData[3] = (u8)crc;
        SFData->SendData[4] = (u8)(crc >> 8u);     
        SFData->SendData[5] = 0x00u;
        SFData->SendData[6] = 0x00u;
#endif       
    }
    else
    {
        for( i = 0u; i < 8u; i++ )
        {
            SFData->SendData[i] = 0u;
        }
    }
}

/******************************  END OF FILE  *********************************/