/*******************************************************************************
* File Name          : esc_comm_diagnostic.c
* Author             : lison
* Version            : V1.0
* Date               : 06/23/2016
* Last modify date   : 11/25/2016
* Description        : This file contains esc safety board communication  with
*                      diagnostic level 1 board.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_comm_diagnostic.h"
#include "can.h"
#include "hw_test.h"
#include "crc16.h"
#include "esc.h"
#ifdef DIAGNOSTIC_LEVEL2
#include "esc_comm_diagnostic2.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CAN_COMM_HAND_TIME      8000u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Send_Data_To_DBL1_Process(void);
static void Receive_Data_From_DBL1_Process(void);
static void Esc_Comm_Diagnostic_Error_Process(void);
static void Safety_Comm_Timeout_Check(DBL1Comm *DBL1);


DBL1Comm DBL1CommUpper = {DBL1_TYPE_UPPER,0,0,0};
DBL1Comm DBL1CommLower = {DBL1_TYPE_LOWER,0,0,0};
DBL1Comm DBL1CommInterm1 = {DBL1_TYPE_INTERM1,0,0,0};
DBL1Comm DBL1CommInterm2 = {DBL1_TYPE_INTERM2,0,0,0};

static u8 g_u8DBL1CommDataValidate = 0u;

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
#ifdef DIAGNOSTIC_LEVEL2    
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
#endif    
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
            g_u8DBL1CommDataValidate |= DBL1_TYPE_UPPER;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_TYPE_UPPER;
            
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
                EscRtData.DBL1Lower.InputData[i] = EscDataFromDBL1[1][i+1u];
            }
            g_u8DBL1CommDataValidate |= DBL1_TYPE_LOWER;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_TYPE_LOWER;
            
            /* data error */
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[1][i] = 0u;
            }
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL1Lower.InputData[i] = 0u;
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
                EscRtData.DBL1Interm1.InputData[i] = EscDataFromDBL1[2][i+1u];
            }
            g_u8DBL1CommDataValidate |= DBL1_TYPE_INTERM1;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_TYPE_INTERM1;
        
            /* data error */
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[2][i] = 0u;
            }
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL1Interm1.InputData[i] = 0u;
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
                EscRtData.DBL1Interm2.InputData[i] = EscDataFromDBL1[3][i+1u];
            }
            g_u8DBL1CommDataValidate |= DBL1_TYPE_INTERM2;
        }
        else
        {
            g_u8DBL1CommDataValidate &= ~DBL1_TYPE_INTERM2;
            
            /* data error */
            for( i = 0u; i < 8u; i++ )
            {
                EscDataFromDBL1[3][i] = 0u;
            }
            for( i = 0u; i < 4u; i++ )
            {
                EscRtData.DBL1Interm2.InputData[i] = 0u;
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
        g_u8CAN2SendFail |= 0x01u;
        CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); 
    } 
}




/*******************************************************************************
* Function Name  : Safety_Comm_Timeout_Check
* Description    : Communicate to Diagnostic level 1 or diagnostic level 2.              
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Safety_Comm_Timeout_Check(DBL1Comm *DBL1)
{
    static u8 stat_u8DBLCommTimeout = 0u;
    static u16 stat_u16DBLHandshakeSuccess = 0u,stat_u16TimerDBLCommWait = 0u;
    u8 u8CommError = 0u;

    if( DIAGNOSTIC == DIAGNOSTIC_BOARD_1 )
    {        
        if( g_u8DBL1CommDataValidate & DBL1->Type )
        {
            DBL1->CommTimeout = 0u;
            DBL1->HandshakeSuccess = 1u;
            g_u8DBL1CommDataValidate &= (u8)(~DBL1->Type);
        }
        else
        {  
            DBL1->CommTimeout++;
        }
     
        if( DBL1->HandshakeSuccess == 1u )
        {
            if( DBL1->CommTimeout > 10u )
            {
                DBL1->CommTimeout = 11u;
                /*  Communication error diag DBL1 */
                u8CommError = 1u;
                Esc_Comm_Diagnostic_Error_Process();
            }          
        }
        else
        {
            DBL1->TimerCommWait++;
            if( ( DBL1->TimerCommWait * SYSTEMTICK ) > CAN_COMM_HAND_TIME )
            {
                /*  can communication handshake timeout when power on */ 
                u8CommError = 1u;
            }            
        }    
        
        if( u8CommError == 1u )
        {
            if( DBL1->Type == DBL1_TYPE_UPPER )
            {
                /*  Communication error diag DBL1 upper F426 */
                EN_ERROR54 |= 0x04u;
            }
            else if( DBL1->Type == DBL1_TYPE_LOWER )
            {
                /*  Communication error diag DBL1 lower F427 */
                EN_ERROR54 |= 0x08u;
            }
            else if( DBL1->Type == DBL1_TYPE_INTERM1 )
            {
                /*  Communication error diag DBL1 interm. 1 F428 */
                EN_ERROR54 |= 0x10u;
            }
            else if( DBL1->Type == DBL1_TYPE_INTERM2 )
            {
                /*  Communication error diag DBL1 interm. 2 F429 */
                EN_ERROR54 |= 0x20u;
            }
            else
            {}        
        }
    }  
#ifdef DIAGNOSTIC_LEVEL2    
    else if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
    {
        if( can2_receive == 1u )
        {
            can2_receive = 0u;
            stat_u8DBLCommTimeout = 0u;
            stat_u16DBLHandshakeSuccess = 1u;
            EN_ERROR48 &= ~0x02u;
        }
        else if( stat_u16DBLHandshakeSuccess )
        {
            if( ++stat_u8DBLCommTimeout >= 20u )
            {
                /* Communication error diag DBL2 F377 */
                EN_ERROR48 |= 0x02u;
                Esc_Comm_Diagnostic_Error_Process();
            }
        }
        else
        {
            stat_u16TimerDBLCommWait++;
            if( ( stat_u16TimerDBLCommWait * SYSTEMTICK ) > CAN_COMM_HAND_TIME )
            {
                /*  can communication handshake timeout when power on */ 
                /* Communication error diag DBL2 F377 */
                EN_ERROR48 |= 0x02u;
            }            
        }    
    }
    else
    {}    
#endif    
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
            
            if( DIAGNOSTIC_BOARD_L1_QUANTITY == 2u )
            {
                Safety_Comm_Timeout_Check(&DBL1CommUpper);
                Safety_Comm_Timeout_Check(&DBL1CommLower);
            }
            else if( DIAGNOSTIC_BOARD_L1_QUANTITY == 3u )
            {
                Safety_Comm_Timeout_Check(&DBL1CommUpper);
                Safety_Comm_Timeout_Check(&DBL1CommLower);
                Safety_Comm_Timeout_Check(&DBL1CommInterm1);
            }
            else if( DIAGNOSTIC_BOARD_L2_QUANTITY == 4u )
            {
                Safety_Comm_Timeout_Check(&DBL1CommUpper);
                Safety_Comm_Timeout_Check(&DBL1CommLower);
                Safety_Comm_Timeout_Check(&DBL1CommInterm1);
                Safety_Comm_Timeout_Check(&DBL1CommInterm2);
            }
            else
            {}           
            
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
#ifdef DIAGNOSTIC_LEVEL2    
    else if( DIAGNOSTIC == DIAGNOSTIC_BOARD_2 )
    {
        /*Safety_Comm_Timeout_Check(NULL);*/
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
#endif    

#endif
}



/******************************  END OF FILE  *********************************/