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
#include "esc_comm_diagnostic2.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CAN_COMM_HAND_TIME      5000u

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Receive_Data_From_Control_Process(void);
static void Send_Data_To_Control_Process(void);
static void Esc_Comm_Error_Process(void);

u8 EscDataToControlBuffer[SF_TO_CONTROL_DATA_LEN][8];

u32 SafetyCommToControlID[SF_TO_CONTROL_DATA_LEN] = 
{ 
    /* cycle 20ms */
    CAN1TX_SAFETY_DATA_ID1, CAN1TX_SAFETY_DATA_ID5, CAN1TX_SAFETY_DATA_ID6, CAN1TX_SAFETY_DATA_ID12, 
    CAN1TX_SAFETY_DATA_ID13, CAN1TX_SAFETY_DATA_ID14, CAN1TX_SAFETY_DATA_ID15, CAN1TX_SAFETY_DATA_ID18,
    CAN1TX_SAFETY_DATA_ID19,
    
    /* cycle 50ms */
    CAN1TX_SAFETY_DATA_ID2, CAN1TX_SAFETY_DATA_ID3, CAN1TX_SAFETY_DATA_ID4, CAN1TX_SAFETY_DATA_ID16, 
    CAN1TX_SAFETY_DATA_ID17, 
    
    /* cycle 100ms */
    CAN1TX_SAFETY_DATA_ID7, CAN1TX_SAFETY_DATA_ID8, CAN1TX_SAFETY_DATA_ID9, CAN1TX_SAFETY_DATA_ID10, 
    CAN1TX_SAFETY_DATA_ID11, CAN1TX_SAFETY_DATA_ID20, CAN1TX_SAFETY_DATA_ID21  
};

u8 g_u8ControlCommDataValidate = 0u;


static u16 *const pEscFault1CHA = (u16*)&EscDataToControl[1][0];
static u16 *const pEscFault2CHA = (u16*)&EscDataToControl[1][2];
static u16 *const pEscFault1CHB = (u16*)&EscDataToControl[1][4];
static u16 *const pEscFault2CHB = (u16*)&EscDataToControl[1][6];

static u16 *const pEscFault3CHA = (u16*)&EscDataToControl[2][0];
static u16 *const pEscFault4CHA = (u16*)&EscDataToControl[2][2];
static u16 *const pEscFault3CHB = (u16*)&EscDataToControl[2][4];
static u16 *const pEscFault4CHB = (u16*)&EscDataToControl[2][6];

static u16 *const pEscFault5CHA = (u16*)&EscDataToControl[3][0];
static u16 *const pEscFault5CHB = (u16*)&EscDataToControl[3][2];

static u16 *const pMotorSpeed1CHA = (u16*)&EscDataToControl[6][0];
static u16 *const pMotorSpeed2CHA = (u16*)&EscDataToControl[6][2];
static u16 *const pMotorSpeed1CHB = (u16*)&EscDataToControl[6][4];
static u16 *const pMotorSpeed2CHB = (u16*)&EscDataToControl[6][6];

static u16 *const pMainShaftSpeed1CHA = (u16*)&EscDataToControl[7][0];
static u16 *const pMainShaftSpeed2CHA = (u16*)&EscDataToControl[7][2];
static u16 *const pMainShaftSpeed1CHB = (u16*)&EscDataToControl[7][4];
static u16 *const pMainShaftSpeed2CHB = (u16*)&EscDataToControl[7][6];

static u16 *const pLeftHandrailSpeedCHA = (u16*)&EscDataToControl[8][0];
static u16 *const pRightHandrailSpeedCHA = (u16*)&EscDataToControl[8][2];
static u16 *const pLeftHandrailSpeedCHB = (u16*)&EscDataToControl[8][4];
static u16 *const pRightHandrailSpeedCHB = (u16*)&EscDataToControl[8][6];

static u16 *const pLeftHandrailCHA = (u16*)&EscDataToControl[9][0];
static u16 *const pRightHandrailCHA = (u16*)&EscDataToControl[9][2];
static u16 *const pLeftHandrailCHB = (u16*)&EscDataToControl[9][4];
static u16 *const pRightHandrailCHB = (u16*)&EscDataToControl[9][6];

static u16 *const pMissingStepUpperCHA = (u16*)&EscDataToControl[10][0];
static u16 *const pMissingStepLowerCHA = (u16*)&EscDataToControl[10][2];
static u16 *const pMissingStepUpperCHB = (u16*)&EscDataToControl[10][4];
static u16 *const pMissingStepLowerCHB = (u16*)&EscDataToControl[10][6];

static u16 *const pDistanceCHA = (u16*)&EscDataToControl[19][0];
static u16 *const pDistanceCHB = (u16*)&EscDataToControl[19][2];
static u16 *const pVoltage = (u16*)&EscDataToControl[19][4];

/*******************************************************************************
* Function Name  : Esc_Comm_Error_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Esc_Comm_Error_Process(void)
{	
    u8 i,j;
    
    /* Clear the data when communicaiton timeout or error */
    for( j = 0u; j < 2u; j++ )
    {
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DataFromControl[j][i] = 0u;
        }
    }
}

/*******************************************************************************
* Function Name  : Receive_Data_From_Control_Process
* Description    :                  
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Receive_Data_From_Control_Process(void)
{	
    static u8 stat_u8SEQN = 0u;
    u8 i;
    
/*
Message ID: 200h
Message cycle time: 20ms.
BYTE	BIT	DESCRIPTION		VALUE	
0	0-7	LIFE SIGNAL	
1	0-1	ORDER_REMOTE_KEY_SWITCH	1 0 : ON / 0 1: OFF  / 0 0: NO VALID / 1 1: NO VALID	(driver test is performed) (timing to generate a valid run order to the safety must be performed)
	2-3	ORDER_REMOTE_RUN 		1 0: ON / 0 1: OFF / 0 0: NO VALID / 1 1: NO VALID  	(driver test is not performed) (timing to generate a valid run order to the safety must be performed)
	4	ORDER_STOP			1: STOP (this order is to indicate to the Safety that the system is going to stop due to a commanded stop not due to a fault, the order remains actived during the STOPPING PROCESS state)	
	5	ORDER_FAULT			1: FAULT  (Control activates the order (=1) when a fault is detected and removes the order (=0) when checks that the safety board is in FAULT state)
        6	ORDER_RESET			1: RESET  (timing to generate a valid reset signal must be performed)	
        7	
2 	0-2	STATE				0 0 0 : INIT / 1 0 0 : READY / 0 0 1: RUN  / 1 0 1:  STOPPING PROCESS / 1 1 1: FAULT
	3-4	MODE				1 0: LOCAL / 0 1: REMOTE / 0 0: NO VALID / 1 1: NO VALID
	5	MODE				1: SLOW (TEC)
	6	MODE				1: STOP (Intermittend)
	7	MODE				1: 2 DIRECTION	
3	0-1	DIRECTION			1 0: UP / 0 1: DOWN / 0 0: NO VALID / 1 1: NO VALID   (Remote run direction order)			
	2-3	SPEED				1 0: NOMINAL SPEED / 0 1: LOW SPEED / 0 0: NO VALID / 1 1: NO VALID	(just feedback to the safety)		
	4	TEST AUX BRAKE		        1 : SWITCH AUX BRAKE TEST ON / 0 : SWITCH AUX BRAKE TEST OFF 		
	5-6	TYPE OF CONTROL FAULT	        11: FAILURE LOCK / 01: STANDARD FAULT / 10: RECOVERY STOP
	7			
4	0-7	CONTACTORS OUTPUTS		bit7	bit6	bit5	bit4	        bit3	bit2	bit1	bit0        
						K2.2.2	K2.1.2	K10.5	K10.1/K10.2     K2.2.1	   K2.1.1	  K1.2	K1.1
5	0-7	

6-7	0-15	CRC
*/     
    if(( !(MB_CRC16(&EscRtData.DataFromControl[0][0], 8u ))) )
    {
        g_u8ControlCommDataValidate |= 0x01u;
        
        /* LIFE SIGNAL check */
        if( stat_u8SEQN != EscRtData.DataFromControl[0][0] )
        {
            stat_u8SEQN = EscRtData.DataFromControl[0][0];
        }
        else
        {
            /* error */
        }

    }
    else
    {
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DataFromControl[0][i] = 0u;
        }
    }
    
    if(( !(MB_CRC16(&EscRtData.DataFromControl[1][0], 8u ))) )
    {
        g_u8ControlCommDataValidate |= 0x02u;
        
        /* OUTPUTS (DBL UPPER) */
        EscRtData.DBL1Upper.OutputData = EscRtData.DataFromControl[1][0];
        /* OUTPUTS (DBL LOWER) */
        EscRtData.DBL1Lower.OutputData = EscRtData.DataFromControl[1][1];
        /* OUTPUTS (DBL INTERM. 1) */
        EscRtData.DBL1Interm1.OutputData = EscRtData.DataFromControl[1][2];
        /* OUTPUTS (DBL INTERM. 2) */
        EscRtData.DBL1Interm2.OutputData = EscRtData.DataFromControl[1][3];
        
        /* CONTROL STANDARD INPUT (1-8) */
        EscRtData.ControlInputData[0] = EscRtData.DataFromControl[1][4];        
        /* CONTROL STANDARD INPUT (9-16) */ 
        EscRtData.ControlInputData[1] = EscRtData.DataFromControl[1][5];
    }
    else
    {
        for( i = 0u; i < 8u; i++ )
        {
            EscRtData.DataFromControl[1][i] = 0u;
        }
    }    
}

/*******************************************************************************
* Function Name  : Send_Data_To_Control_Process
* Description    :   
*
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
    Message ID: 113h.
    Message cycle time: 100ms
*   
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
static void Send_Data_To_Control_Process(void)
{	
    static u8 stat_u8SEQN = 1u;
    u16 *plen = (u16*)&ParaDataToControl[0][4];
    static u8 ParaSendFail = 0u;
    u8 i,j,result = 0u;
/*
Message ID: 100h.
Message cycle time: 20ms.
BYTE	BIT	DESCRIPTION		VALUE	
0	0-7	LIFE SIGNAL	
1	0	ORDER_RUN		1: Signal activated / 0: Signal no activated (Safety activates the order (=1) when changes to RUN state and removes the order (=0) when checks that the control board is in RUN state)
	1	ORDER_STOP		1: Signal activated / 0: Signal no activated (This order is to indicate to the Control that the system is going to stop due to a commanded stop not due to a fault, the order remains actived during the STOPPING PROCESS state)
	2	ORDER_FAULT		1: Signal activated / 0: Signal no activated  (Safety activates the order (=1) when a fault is detected and removes the order (=0) when checks that the control board is in FAULT state) 
	3	ORDER_RESET		1: Signal activated / 0: Signal no activated (Safety board activates reset order when the last active fault is reset, removes the order (=0) when checks that the control board is in READY state, maximum 200ms)
	4	ORDER_CONTACTOR		1: ACIVATE UP CONTACTOR / 0: DEACTIVATE UP CONT. (Control board only checks this order in READY state)
	5 	ORDER_SLOW_FAST		0: NOMINAL SPEED or DELTA / 1: LOW SPEED or STAR  (Control board checks this value when goes to RUN)
	6	ORDER_UP_DOWN		1: UP / 0: DOWN (Control board checks this value when goes to RUN)
	7	ORDER_CAPACITOR		1: Activation of main shaft brake capacitator Contactor (K4.2) / 0: Deactivation

2	0-2	STATE			0 0 0 : INIT / 1 0 0 : READY / 0 1 0 : STARTING PROCESS 
					0 0 1: RUN / 1 1 0 : INTERM / 1 0 1:  STOPPING PROCESS / 1 1 1: FAULT
        3-4	MODE 1			1 0: NORMAL / 0 1: INSPECTION / 0 0: NO VALID / 1 1: NO VALID
        5-7	MODE 2			0 0 0: CONTINOUS / 0 0 1: INTERMITTEND / 0 1 0: STBY / 1 0 0: INTERMITEND +STBY
					0 1 1: 2 DIRECTION / 1 0 1: Y/D SWITCHBACK		

3	0	TYPE OF STOP		1: INTERM STOP
	1	TYPE OF STOP		1: INSP BUTTONS RELEASED
	2	TYPE OF STOP		1: REMOTE
	3	TYPE OF STOP		1: SAFETY CURTAIN 
	4	STOPPING PROCESS	1: FINISH	0: STOPPING      (=0 during the STOPPING PROCESS state)			
	5	SWITCH ON ELEM.	        1: PEOPLE DETECTED UPPER
	6	SWITCH ON ELEM.	        1: PEOPLE DETECTED LOWER 	
	7	HANDRAIL ASME		1: HANDRAIL NOT SYNCRO (FOR ASME CODE)

4	0-7	WARNING		WARNING 1 = WARNING 1 CHA OR WARNING 1 CHB
5	0-7	WARNING		WARNING 2 = WARNING 2 CHA OR WARNING 2 CHB
6	0-7	WARNING		WARNING 3 = WARNING 3 CHA OR WARNING 3 CHB
7	0-7	WARNING		WARNING 4 = WARNING 4 CHA OR WARNING 4 CHB
*/ 
    if( SfBase_EscState != ESC_INIT_STATE )
    {
        
        stat_u8SEQN++;
        if( stat_u8SEQN > 250u )
        {
            stat_u8SEQN = 1u;
        }
        
        /* LIFE SIGNAL */
        EscDataToControl[0][0] = stat_u8SEQN;
        
        /* order / state & mode / type of stop, combine CPU1 && CPU2 ? */
        
        /* Warning */    
        EscDataToControl[0][4] = (u8)EscWarnCodeBuff[0];
        EscDataToControl[0][5] = (u8)EscWarnCodeBuff[1];
        EscDataToControl[0][6] = (u8)EscWarnCodeBuff[2];
        EscDataToControl[0][7] = (u8)EscWarnCodeBuff[3];
        
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
#ifdef DIAGNOSTIC_LEVEL2    
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
#endif    
#else
        /* SAFETY INPUT CHA */
        EscDataToControl[4][0] = EscRtData.SafetyInputData[0];
        EscDataToControl[4][1] = EscRtData.SafetyInputData[1];
        EscDataToControl[4][2] = EscRtData.SafetyInputData[2];    
        EscDataToControl[4][3] = ((EscRtData.SafetyInputData[3] & 0x0fu) | ((u8)((u8)EscRtData.SafetyInputData[4] << 4u ) & 0xf0u));
        EscDataToControl[4][4] = ((u8)(((u8)EscRtData.SafetyInputData[4] >> 4u ) & 0x0fu) | (u8)(((u8)EscRtData.SafetyInputData[5] << 4u ) & 0x0fu));
        EscDataToControl[4][5] = ((((u8)EscRtData.SafetyInputData[5] >> 4u ) & 0x03u ) | (((u8)EscRtData.SafetyInputData[3] >> 2u ) & 0x04u )
                                  | ((u8)((u8)EscRtData.SafetyInputData[5] >> 3u ) & 0x18u ) | ((u8)((u8)EscRtData.SafetyInputData[6] << 5u ) & 0x20u ));
        
        
        /* SAFETY INPUT CHB */
        EscDataToControl[5][0] = OmcEscRtData.SafetyInputData[0];
        EscDataToControl[5][1] = OmcEscRtData.SafetyInputData[1];
        EscDataToControl[5][2] = OmcEscRtData.SafetyInputData[2];
        EscDataToControl[5][3] = ((OmcEscRtData.SafetyInputData[3] & 0x0fu) | ((u8)((u8)OmcEscRtData.SafetyInputData[4] << 4u ) & 0xf0u));
        EscDataToControl[5][4] = (((u8)((u8)OmcEscRtData.SafetyInputData[4] >> 4u ) & 0x0fu) | ((u8)((u8)OmcEscRtData.SafetyInputData[5] << 4u ) & 0x0fu));
        EscDataToControl[5][5] = ((((u8)OmcEscRtData.SafetyInputData[5] >> 4u ) & 0x03u ) | (((u8)OmcEscRtData.SafetyInputData[3] >> 2u ) & 0x04u )
                                  | ((u8)((u8)OmcEscRtData.SafetyInputData[5] >> 3u ) & 0x18u ) | ((u8)((u8)OmcEscRtData.SafetyInputData[6] << 5u ) & 0x20u ));  
#endif 
        
        /* MOTOR SPEED */
        *pMotorSpeed1CHA = MOTOR_SPEED1;
        *pMotorSpeed2CHA = MOTOR_SPEED2;
        *pMotorSpeed1CHB = OMC_MOTOR_SPEED1;
        *pMotorSpeed2CHB = OMC_MOTOR_SPEED2;
        
        /* MAIN SHAFT SPEED */
        *pMainShaftSpeed1CHA = MAIN_SHAFT_SPEED1;
        *pMainShaftSpeed2CHA = MAIN_SHAFT_SPEED2;
        *pMainShaftSpeed1CHB = OMC_MAIN_SHAFT_SPEED1;
        *pMainShaftSpeed2CHB = OMC_MAIN_SHAFT_SPEED2;
        
        /* HANDRAIL SPEED */
        *pLeftHandrailSpeedCHA = HANDRAIL_SPEED_LEFT;
        *pRightHandrailSpeedCHA = HANDRAIL_SPEED_RIGHT;
        *pLeftHandrailSpeedCHB = OMC_HANDRAIL_SPEED_LEFT;
        *pRightHandrailSpeedCHB = OMC_HANDRAIL_SPEED_RIGHT; 
        
        /* HANDRAIL (PULSE OF MOTOR) */
        *pLeftHandrailCHA = HANDRAIL_SPEED_LEFT_MOTOR_PULSE;
        *pRightHandrailCHA = HANDRAIL_SPEED_RIGHT_MOTOR_PULSE;
        *pLeftHandrailCHB = OMC_HANDRAIL_SPEED_LEFT_MOTOR_PULSE;
        *pRightHandrailCHB = OMC_HANDRAIL_SPEED_RIGHT_MOTOR_PULSE; 
        
        /* MISSING STEP (PULSE OF MOTOR) */
        *pMissingStepUpperCHA = MISSINGSTEP_MTR_PULSE1;
        *pMissingStepLowerCHA = MISSINGSTEP_MTR_PULSE2;
        *pMissingStepUpperCHB = OMC_MISSINGSTEP_MTR_PULSE1;
        *pMissingStepLowerCHB = OMC_MISSINGSTEP_MTR_PULSE2;     
        
        
#ifdef DIAGNOSTIC_LEVEL2
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
#endif
        
        /* DBL1 UPPER INPUT */
        EscDataToControl[17][0] = EscRtData.DBL1Upper.InputData[0];
        EscDataToControl[17][1] = EscRtData.DBL1Upper.InputData[1];
        EscDataToControl[17][2] = EscRtData.DBL1Upper.InputData[2];
        EscDataToControl[17][3] = EscRtData.DBL1Upper.InputData[3];
        
        /* DBL1 LOWER INPUT */
        EscDataToControl[17][4] = EscRtData.DBL1Lower.InputData[0];
        EscDataToControl[17][5] = EscRtData.DBL1Lower.InputData[1]; 
        EscDataToControl[17][6] = EscRtData.DBL1Lower.InputData[2];
        EscDataToControl[17][7] = EscRtData.DBL1Lower.InputData[3];  
        
        /* DBL1 INTERM 1 INPUT */
        EscDataToControl[18][0] = EscRtData.DBL1Interm1.InputData[0];
        EscDataToControl[18][1] = EscRtData.DBL1Interm1.InputData[1];
        EscDataToControl[18][2] = EscRtData.DBL1Interm1.InputData[2];
        EscDataToControl[18][3] = EscRtData.DBL1Interm1.InputData[3];
        
        /* DBL1 INTERM 2 INPUT */
        EscDataToControl[18][4] = EscRtData.DBL1Interm2.InputData[0];
        EscDataToControl[18][5] = EscRtData.DBL1Interm2.InputData[1]; 
        EscDataToControl[18][6] = EscRtData.DBL1Interm2.InputData[2];
        EscDataToControl[18][7] = EscRtData.DBL1Interm2.InputData[3]; 
        
        /* Stopping Distace(mm) and end of safety string voltage */
        *pDistanceCHA = STOPPING_DISTANCE1;
        *pDistanceCHB = OMC_STOPPING_DISTANCE2;
        *pVoltage = g_u16EndOfSafetyStringVoltage;
        
        for( j = 0u; j < SF_TO_CONTROL_DATA_LEN; j++ )
        {
            for( i = 0u; i < 8u; i++ )
            {
                EscDataToControlBuffer[j][i] = EscDataToControl[j][i];
            }  
        }
    }
    else
    {
        /* Init state */
       
        
        
        if(( EscRtData.ParaStatus & 0x31u ) == 0x31u )
        {       
            /* Send C0001.bin file to Control board */                
            ParaDataToControl[0][0] = EscRtData.ParaStatus;               
            ParaDataToControl[0][1] = (u8)ControlFileLen & 0xffu;
            ParaDataToControl[0][2] = (u8)( ControlFileLen >> 8u ) & 0xffu;
            if(( ControlFileLen > 0u ) && ( ControlFileLen == *plen ))
            {
                /* send finish */
                ParaDataToControl[0][3] = 1u;
            }
            else
            {
                ParaDataToControl[0][3] = 0u;
            }
            ParaDataToControl[0][6] = 0u;
            ParaDataToControl[0][7] = 0u;
            result = Can_Send_Msg(CAN1, CAN1TX_PARA_STATUS_ID, &ParaDataToControl[0][0], CAN_FRAME_LEN ); 
            if( result )
            {
                /* No mail box, send fail */
            }   
            
            for( j = 0u; j < 2u; j++ )
            {
                if(( ControlFileLen > *plen ) || ( ParaSendFail == 1u ))
                {    
                    if( ParaSendFail == 0u )
                    {
                        for( i = 0u; i < 8u; i++ )
                        {
                            ParaDataToControl[1][i] = 0u;
                        }  
                        
                        if( ControlFileLen - *plen > 8u )
                        {
                            for( i = 0u; i < 8u; i++ )
                            {
                                ParaDataToControl[1][i] = ParameterFile[*plen + i];
                            }
                            *plen += 8u;
                        }
                        else
                        {
                            for( i = 0u; i < ControlFileLen - *plen; i++ )
                            {
                                ParaDataToControl[2][i] = ParameterFile[*plen + i];
                            }
                            *plen = ControlFileLen;
                        }
                    }
                    result = Can_Send_Msg(CAN1, (CAN1TX_PARA_CONTROL_ID1 + ((*plen - 1u)/ 8u)), &ParaDataToControl[1][0], CAN_FRAME_LEN ); 
                    if( result )
                    {
                        /* No mail box, send fail */
                        ParaSendFail = 1u;
                    }
                    else
                    {
                        ParaSendFail = 0u;
                    }
                }
            }
        }
        else if( EscRtData.ParaStatus & USB_NOT_DETECTED )
        {
            ParaDataToControl[0][0] = EscRtData.ParaStatus; 
            ParaDataToControl[0][1] = (u8)ESC_SF_PARAMETER_DATA_LEN & 0xffu;
            ParaDataToControl[0][2] = (u8)( ESC_SF_PARAMETER_DATA_LEN >> 8u ) & 0xffu;
            if( ESC_SF_PARAMETER_DATA_LEN == *plen )
            {
                /* send finish */
                ParaDataToControl[0][3] = 1u;
            }
            else
            {
                ParaDataToControl[0][3] = 0u;
            }
            ParaDataToControl[0][6] = 0u;
            ParaDataToControl[0][7] = 0u;
            Can_Send_Msg(CAN1, CAN1TX_PARA_STATUS_ID, &ParaDataToControl[0][0], CAN_FRAME_LEN ); 
      
            if( g_u8ParameterLoadingOK == 1u )
            {
                for( j = 0u; j < 2u; j++ )
                {
                    if(( ESC_SF_PARAMETER_DATA_LEN > *plen ) || ( ParaSendFail == 1u ))
                    {    
                        if( ParaSendFail == 0u )
                        {
                            for( i = 0u; i < 8u; i++ )
                            {
                                ParaDataToControl[1][i] = 0u;
                            }  
                            
                            if( ESC_SF_PARAMETER_DATA_LEN - *plen > 8u )
                            {
                                for( i = 0u; i < 8u; i++ )
                                {
                                    ParaDataToControl[1][i] = ParameterFile[*plen + i];
                                }
                                *plen += 8u;
                            }
                            else
                            {
                                for( i = 0u; i < ESC_SF_PARAMETER_DATA_LEN - *plen; i++ )
                                {
                                    ParaDataToControl[1][i] = ParameterFile[*plen + i];
                                }
                                *plen = ESC_SF_PARAMETER_DATA_LEN;
                            }
                        }
                        result = Can_Send_Msg(CAN1, (CAN1TX_PARA_SAFETY_ID1 + ((*plen - 1u)/ 8u)), &ParaDataToControl[1][0], CAN_FRAME_LEN ); 
                        if( result )
                        {
                            /* No mail box, send fail */
                            ParaSendFail = 1u;
                        }
                        else
                        {
                            ParaSendFail = 0u;
                        }
                    }
                }   
            }
        }
        else
        {}
        
    }
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
    static u16 stat_u16TimeCan1Error = 0u;
    u8 result,i;
    
    if( SfBase_EscState == ESC_INIT_STATE )
    {
        Send_Data_To_Control_Process();
    }
    else
    {    
        /* CAN error check */            
        if( can1_receive == 0u )
        {
            if( stat_u16TimeCan1Error < 0xffffu )
            {
                stat_u16TimeCan1Error++;
            }
            
            /* 1000ms */
            if( stat_u16TimeCan1Error > 100u )
            {
                stat_u16TimeCan1Error = 0u;
                if( CAN1->ESR & CAN_ESR_BOFF )
                {
                    CAN_Int_Init(CAN1);
                }                          
            }
        }
        else
        {
            stat_u16TimeCan1Error = 0u;
            can1_receive = 0u;
        }
            
        /* comm timeout check */
        if( g_u8ControlCommDataValidate == 0x03u )
        {        
            can1_comm_timeout = 0u;
            stat_u16Can1HandshakeSuccess = 1u;
        }
        else if( stat_u16Can1HandshakeSuccess )
        {
            if( can1_comm_timeout < 0xffu )
            {
                can1_comm_timeout++;
            }            
            /* 200ms */
            if( can1_comm_timeout >= 10u )
            {
                /*  Communication error Control F378 */
                EN_ERROR48 |= 0x04u;
                Esc_Comm_Error_Process();
            }
        }
        else
        {
            if( stat_u16TimerCan1CommWait < 0xffffu )
            {
                stat_u16TimerCan1CommWait++;
            }
            if( stat_u16TimerCan1CommWait * 10u > CAN_COMM_HAND_TIME )
            {
                /*  can communication handshake timeout when power on */ 
                /*  Communication error Control F378 */
                EN_ERROR48 |= 0x04u;
                Esc_Comm_Error_Process();
            }            
        }    
        
        if( stat_u16Can1HandshakeSuccess && (!g_u8CanCommunicationToCotrolOk ))
        {
            /* not send finish */
            /*  Communication error Control F378 */
            EN_ERROR48 |= 0x04u;
        }
        
        
        if(( stat_u8TimerSend % 2u ) == 0u )
        {
            g_u8ControlCommDataValidate = 0u;
            if( g_u16ContorlNewData != 0x0000u )
            {
                /* Recieve data */
                Can_Receive_Data(DATA_FROM_CONTROL);
                Receive_Data_From_Control_Process();
            }
            
            /* Send data, Message cycle time: 20ms*/
            Send_Data_To_Control_Process();
            
            g_u8CanCommunicationToCotrolOk = 0u;
            g_u8CanCommunicationToCotrolID = 0u;
            g_u8CanCommunicationToCotrolLen = 8u;
            for( i = 0u; i < 3u; i++ )
            {
                result = Can_Send_Msg(CAN1, SafetyCommToControlID[g_u8CanCommunicationToCotrolID], 
                                      &EscDataToControlBuffer[SafetyCommToControlID[g_u8CanCommunicationToCotrolID] - CAN1TX_SAFETY_DATA_ID1][0], CAN_FRAME_LEN ); 
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
                result = Can_Send_Msg(CAN1, SafetyCommToControlID[g_u8CanCommunicationToCotrolID], 
                                      &EscDataToControlBuffer[SafetyCommToControlID[g_u8CanCommunicationToCotrolID] - CAN1TX_SAFETY_DATA_ID1][0], CAN_FRAME_LEN ); 
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
            g_u8CanCommunicationToCotrolLen = 20u;
            for( i = 0u; i < 3u; i++ )
            {
                result = Can_Send_Msg(CAN1, SafetyCommToControlID[g_u8CanCommunicationToCotrolID], 
                                      &EscDataToControlBuffer[SafetyCommToControlID[g_u8CanCommunicationToCotrolID] - CAN1TX_SAFETY_DATA_ID1][0], CAN_FRAME_LEN ); 
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
}


/******************************  END OF FILE  *********************************/