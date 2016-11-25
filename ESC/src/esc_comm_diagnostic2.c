/*******************************************************************************
* File Name          : esc_comm_diagnostic2.c
* Author             : lison
* Version            : V1.0
* Date               : 10/26/2016
* Last modify date   : 11/25/2016
* Description        : This file contains esc safety board communication  with
*                      diagnostic level 2 board. 
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "esc_comm_diagnostic2.h"

#ifdef DIAGNOSTIC_LEVEL2
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CAN_COMM_HAND_TIME      8000u
#define SMVT_TIMEOUT            80u
#define SMCT_TIME               80u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Safety_Comm_Data_Init(DBL2Esc *SFData, u8 connection, u8 boardtype, u8 SEQN);
static void Safety_Receive_Diagnostic_Validate( DBL2Esc *SFData, u8 DBL2ReceiveData[] );
#ifdef GEC_SF_MASTER
static void Safety_ReceiveA_Diagnostic( DBL2Esc *SFData );
static void Send_Data_To_DBL2_Process(DBL2Esc *SFData, u8 SendDataB[], u8 CANSendData[], u8 CANID);
static void Receive_Data_From_DBL2_Process(DBL2Esc *SFData, u8 CANReceiveData[], u8 CANID);
#else
static void Safety_Request_Data(DBL2Comm *DBL2, DBL2Esc *Esc);
#endif


static u8 g_u8DBL2Respone = 0u;

#ifdef GEC_SF_S_NEW 
DBL2Comm DBL2CommUpper = {0,0,0,0};
DBL2Comm DBL2CommLower = {0,0,0,0};
DBL2Comm DBL2CommInterm1 = {0,0,0,0};
DBL2Comm DBL2CommInterm2 = {0,0,0,0};
#endif


/*******************************************************************************
* Function Name  : Safety_Comm_Data_Init
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Safety_Comm_Data_Init(DBL2Esc *SFData, u8 connection, u8 boardtype, u8 SEQN)
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
* Function Name  : Safety_Request_DBL2
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void Safety_Request_DBL2(void)
{	
    static u8 stat_u8FirstEnter = 0u;
    
    if( stat_u8FirstEnter == 0u )
    {
        stat_u8FirstEnter = 1u;
        Safety_Comm_Data_Init(&EscRtData.DBL2Upper, CONNECTION_DBL2_UPPER, DBL2_TYPE_UPPER, 250u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Lower, CONNECTION_DBL2_LOWER, DBL2_TYPE_LOWER, 250u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Interm1, CONNECTION_DBL2_INTERM1, DBL2_TYPE_INTERM1, 250u);
        Safety_Comm_Data_Init(&EscRtData.DBL2Interm2, CONNECTION_DBL2_INTERM2, DBL2_TYPE_INTERM2, 250u);
    }
    
    
    if( DIAGNOSTIC_BOARD_L2_QUANTITY == 2u )
    {
        Safety_Request_Data(&DBL2CommUpper,&EscRtData.DBL2Upper);
        Safety_Request_Data(&DBL2CommLower,&EscRtData.DBL2Lower);
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u )
    {
        Safety_Request_Data(&DBL2CommUpper,&EscRtData.DBL2Upper);
        Safety_Request_Data(&DBL2CommLower,&EscRtData.DBL2Lower);
        Safety_Request_Data(&DBL2CommInterm1,&EscRtData.DBL2Interm1);
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
    {
        Safety_Request_Data(&DBL2CommUpper,&EscRtData.DBL2Upper);
        Safety_Request_Data(&DBL2CommLower,&EscRtData.DBL2Lower);
        Safety_Request_Data(&DBL2CommInterm1,&EscRtData.DBL2Interm1);
        Safety_Request_Data(&DBL2CommInterm2,&EscRtData.DBL2Interm2);
    }
    else
    {
        Safety_Request_Data(&DBL2CommLower,&EscRtData.DBL2Lower);
    }    
    
}


/*******************************************************************************
* Function Name  : Safety_Request_Data
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Safety_Request_Data(DBL2Comm *DBL2, DBL2Esc *Esc)
{	
    u8 u8CommError = 0u;
    
    /* SMCT cycle send data */
    if( (DBL2->TimerSMCT == 0u) && (!(EN_ERROR54 & 0xc0u)) && (!(EN_ERROR55 & 0x03u)))
    {
        DBL2->TimerSMVT = 0u;
        DBL2->TimerSMCT++;
        DBL2->TimerSMVT++;
        EscRtData.DBL2ValidateResult &= (u8)(~Esc->BoardType);

        Safety_Send_Data_Process(Esc, 1u);
    }
    else
    {     
        DBL2->TimerSMCT++;
        DBL2->TimerSMVT++;
        if( ( DBL2->TimerSMCT * SYSTEMTICK ) > SMCT_TIME )
        {
            DBL2->TimerSMCT = 0u; 
        }
    }

    /************************** SMVT timeout **********************************/ 
    if( g_u8DBL2Respone & Esc->BoardType )
    {
        g_u8DBL2Respone &= (u8)(~Esc->BoardType);
        DBL2->HandshakeSuccess = 1u;
        DBL2->TimerSMVT = 0u;
    }
    else
    {
        if( DBL2->HandshakeSuccess == 1u )
        {
            if( ( DBL2->TimerSMVT * SYSTEMTICK ) > SMVT_TIMEOUT )
            {
                /* Communication error diag DBL2 */
                u8CommError = 1u;
            }
        }
    }

    if( DBL2->HandshakeSuccess == 0u )
    {
        DBL2->TimerCommWait++;
        if(( DBL2->TimerCommWait * SYSTEMTICK ) > CAN_COMM_HAND_TIME )
        {
            /*  can communication handshake timeout when power on */ 
            u8CommError = 1u;
        } 
    }    
    
    if( u8CommError == 1u )
    {
        if( Esc->BoardType == DBL2_TYPE_UPPER )
        {
             /* Communication error diag DBL2 upper F430 */
            EN_ERROR54 |= 0x40u;
        }
        else if( Esc->BoardType == DBL2_TYPE_LOWER )
        {
             /* Communication error diag DBL2 lower F431 */
            EN_ERROR54 |= 0x80u;
        }
        else if( Esc->BoardType == DBL2_TYPE_INTERM1 )
        {
             /* Communication error diag DBL2 interm. 1 F432 */
            EN_ERROR55 |= 0x01u;
        }
        else if( Esc->BoardType == DBL2_TYPE_INTERM2 )
        {
             /* Communication error diag DBL2 interm. 2 F433 */
            EN_ERROR55 |= 0x02u;
        }
        else
        {}
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
static void Safety_Receive_Diagnostic_Validate( DBL2Esc *SFData, u8 DBL2ReceiveData[] )
{
    u8 i;
    u8 u8_ValidateError = 0u;

    /* Validation Response: Check CONNECTION, SEQN, CRC and compare _1 and _2 data */
    /* check CRC */
    if( !MB_CRC32( &DBL2ReceiveData[0], 12u, SAFETY_POLYNOMIALS ))
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

    if(( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u ))
    {    
        Safety_ReceiveA_Diagnostic( &EscRtData.DBL2Interm1 );
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
    {    
        Safety_ReceiveA_Diagnostic( &EscRtData.DBL2Interm2 );
    }
    else
    {}
#else
    Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Upper, OmcEscRtData.DBL2Upper.ReceiveDataB );
    Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Lower, OmcEscRtData.DBL2Lower.ReceiveDataB );
    if(( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u ))
    {       
        Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Interm1, OmcEscRtData.DBL2Interm1.ReceiveDataB );
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
    {
        Safety_Receive_Diagnostic_Validate(&EscRtData.DBL2Interm2, OmcEscRtData.DBL2Interm2.ReceiveDataB );
    }
    else
    {}
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
    u32 crc;
    u16 len;
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
        len = 8u;
        crc = MB_CRC32( &SFData->SendData[0], len - 4u, SAFETY_POLYNOMIALS );
        SFData->SendData[len - 4u] = (u8)(crc >> 24u);
        SFData->SendData[len - 3u] = (u8)(crc >> 16u);     
        SFData->SendData[len - 2u] = (u8)(crc >> 8u);
        SFData->SendData[len - 1u] = (u8)crc;
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
        /* NOT USED */
        SFData->SendData[3] = 0u;
        
        /* CRC_B */
        len = 8u;
        crc = MB_CRC32( &SFData->SendData[0], len - 4u, SAFETY_POLYNOMIALS );
        SFData->SendData[len - 4u] = (u8)(crc >> 24u);
        SFData->SendData[len - 3u] = (u8)(crc >> 16u);     
        SFData->SendData[len - 2u] = (u8)(crc >> 8u);
        SFData->SendData[len - 1u] = (u8)crc;
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

#ifdef GEC_SF_MASTER
/*******************************************************************************
* Function Name  : Safety_ReceiveA_Diagnostic
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
void Receive_Data_From_DBL2(void)
{	

    /*********************** DBL2 UPPER *********************************/  
    Receive_Data_From_DBL2_Process( &EscRtData.DBL2Upper, &EscDataFromDBL2[0][0], CAN2RX_DBL2_UPPER_ID1 );
    

    /*************************** DBL2 LOWER *****************************/
    Receive_Data_From_DBL2_Process( &EscRtData.DBL2Lower, &EscDataFromDBL2[4][0], CAN2RX_DBL2_LOWER_ID1 );
        

    if(( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u ))
    {  
        /*************************** DBL2 INTERM1 ************************ **/
        Receive_Data_From_DBL2_Process( &EscRtData.DBL2Interm1, &EscDataFromDBL2[8][0], CAN2RX_DBL2_INTERM1_ID1);  
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
    {
        /*************************** DBL2 INTERM2 ***************************/
        Receive_Data_From_DBL2_Process( &EscRtData.DBL2Interm2, &EscDataFromDBL2[12][0], CAN2RX_DBL2_INTERM2_ID1);
    }
    else
    {}
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

    if( !MB_CRC32(&SendDataB[0] , 8u, SAFETY_POLYNOMIALSB ) )
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
            CANSendData[12] = SendDataB[4];   
            CANSendData[13] = SendDataB[5]; 
            CANSendData[14] = SendDataB[6];   
            CANSendData[15] = SendDataB[7];    
            
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
void Send_Data_To_DBL2(void)
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
    
    /*************************** DBL2 UPPER *****************************/
    Send_Data_To_DBL2_Process(&EscRtData.DBL2Upper , OmcEscRtData.DBL2Upper.SendData, 
                      &EscDataToDBL2[0][0], CAN2TX_DBL2_UPPER_ID1 );

    

    /*************************** DBL2 LOWER *****************************/
    Send_Data_To_DBL2_Process(&EscRtData.DBL2Lower , OmcEscRtData.DBL2Lower.SendData, 
                      &EscDataToDBL2[2][0], CAN2TX_DBL2_LOWER_ID1 );
   
    
    if(( DIAGNOSTIC_BOARD_L2_QUANTITY == 3u ) || ( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u ))
    {  
        
        /*************************** DBL2 INTERM1 ***************************/
        Send_Data_To_DBL2_Process(&EscRtData.DBL2Interm1 , OmcEscRtData.DBL2Interm1.SendData, 
                                  &EscDataToDBL2[4][0], CAN2TX_DBL2_INTERM1_ID1 );   
    }
    else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
    {

        /*************************** DBL2 INTERM2 ***************************/
        Send_Data_To_DBL2_Process(&EscRtData.DBL2Interm2 , OmcEscRtData.DBL2Interm2.SendData, 
                                  &EscDataToDBL2[6][0], CAN2TX_DBL2_INTERM2_ID1 );
    }
    else
    {}
}
#endif
#endif

/******************************  END OF FILE  *********************************/