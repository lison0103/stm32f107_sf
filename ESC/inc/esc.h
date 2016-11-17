/*******************************************************************************
* File Name          : esc.h
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 10/25/2016
* Description        : This file contains esc parameters.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_H
#define __ESC_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

#include "esc_cfg_io.h"

/* Exported types ------------------------------------------------------------*/
/* ESC STATE */
enum EscState
{
    ESC_INIT_STATE = 0u,
    ESC_FAULT_STATE = 1u,
    ESC_READY_STATE = 2u,
    ESC_STARTING_PROCESS_STATE = 3u,
    ESC_RUN_STATE = 4u,
    ESC_STOPPING_PROCESS_STATE = 5u,
    ESC_INTERM_STATE = 6u
};

typedef struct safetycommdata
{
    /* type */
    u8 BoardType;
    
    /* connection */
    u8 Connection; 
      
    /* SEQN */
    u8 SEQN;
  
    /* safety comm send data */
    u8 SendData[8];
    
    /* safety comm receive data */
    u8 ReceiveDataA[14];
    u8 ReceiveDataB[14];   
  
}SafetyCommData;

typedef struct sfescdata 
{
    /* Header code and comm times */
    u8 HeaderCode[12];
    
    /* Input and Feedback */
    u8 SafetyInputData[8];
    u8 ControlInputData[2];
    
    u8 DBL1UpperInputData[4];
    u8 DBL1LowerInputData[4];
    u8 DBL1Interm1InputData[4];
    u8 DBL1Interm2InputData[4];

    u8 DBL2UpperInputData[4];
    u8 DBL2LowerInputData[4];
    u8 DBL2Interm1InputData[4];
    u8 DBL2Interm2InputData[4];

    /* ANALOG/PT100 */
    u16 DBL2UpperAnalogData[3];
    u16 DBL2LowerAnalogData[3];
    u16 DBL2Interm1AnalogData[3];
    u16 DBL2Interm2AnalogData[3];
    
    /* Output */
    u8 DBL2OutputData;
    
    /* Command data*/
    u8 CommandData[8];
    
    /* Alarm data */
    u8 Alarm[4];
    
    /* Fault data */
    u8 ErrorBuff[64];

    /* Sensor Data */
    u8 SensorData[20];

    /* Error code */
    u16 ErrorCode[5];
    
    /* Esc parameter crc value */
    u8 ParaCRC[4];
    
    /* Configurable input buff */
    u8 Cfg_Input_Mask[20]; 
    u8 Cfg_Input_Level[20]; 
    
    /* Safety comm to DBL2 */
    SafetyCommData DBL2Upper;
    SafetyCommData DBL2Lower;
    SafetyCommData DBL2Interm1;
    SafetyCommData DBL2Interm2;
    
    /* DBL2 comm validate result */
    u8 DBL2ValidateResult;
    
}SafetyEscData;    


typedef struct motorspeeditem 
{    
    /* A timer, Running time */
    u32 TimerRuningTms;
    
    /* Measure 1s pulse time counter */
    u8 Tms_counter;      
    
    /* motor speed real time pulse */
    volatile u16 rt_pulse;  
    
    /* record 100ms motor speed pulse count */
    u16 pulseArray[20]; 					                  
    
    /* Escalator speed */
    u16 *const ptFreqBuff;    
    
    /* motor speed overspeed pulse not ok counter */
    u8 NotOkCounter;
    
    /* motor speed overspeed pulse ok counter */
    u8 OkCounter;
    
    /* motor between pulse time */
    u32 TimerMotorSpeedBetweenPulse[10];
    
    /* motor between pulse counter */
    u8 between_pulse_counter;

    /* motor no pulse timer */
    u16 TimerNoPulse;

    /* timeout stopping process */
    u16 TimeoutTimerMotorSpeed;
    
    /* Sensor use timer init */
    void (*Timer_Init)(u16 p1,u16 p2);
    
    /* Sensor use timer number */
    TIM_TypeDef* TimerNum;
  
    /* Brake distance */
    u16 *const ptBrakeDistanceBuff;
    
    /* shopping process motor speed pulse */
    volatile u16 rt_brake_pulse;     
    
    /* shopping finish flag */
    u16 rt_brake_stop ;    
    
}MotorSpeedItem;



typedef struct handrailspeeditem 
{
    /* handrail speed in ready state time counter */
    u32 hdl_pulse_tms;   
    
    /* motor speed real time pulse */
    volatile u16 handrail_speed_rt_pulse; 					
    
    /* handrail speed continuous fault time counter */
    u16 HR_Fault_timer;
    
    /* Handrail rising edge detecte */
    u8 rising_edge_detected;
    
    /* Number of pulses of motor speed between two consecutives pulses of handrail array */ 
    u16 hdl_mp_array[10];
    
    /* handrail speed pulse counter */
    u8 hdl_pulse_counter;
    
    /* Handrail speed */
    u16 *const ptHDLDataBuff; 
   	
}HandrailSpeedItem;

typedef struct stepmissingitem 
{
    
    /* motor speed real time pulse */
    volatile u16 MtrPulse;	
    
    /* Missing step rising edge detecte */
    u8 rising_edge_detected[2];
    
    /* Measure number of pulses of motor speed between two consecutives pulses of missing step */
    u16 *const ptStepMtrBuff;
    
    /* Missing step in ready state time counter */
    u32 ms_ready_delay;
    
    u32 Motor_speed_pulse_counter_init;
    u32 Motor_speed_pulse_counter;
    u8 First_entry_missing_step_detection;
        
}STEPMISSINGITEM;

typedef struct updownkey 
{
    /* Up down key previous input state */
    u8 InputPreviousState;
    
    /* define key type, up or down*/
    u8 key;
    
    /* define key type, up or down*/
    u8 otherkey;
    
    /* A timer, counting key on times */
    u16 TimerKeyOn;
	
}UpDownKeyItem;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ESC_NORMAL      0x00u
#define ESC_INSPECT     0x10u
#define ESC_UP          0x01u
#define ESC_DOWN        0x02u


#define ESC_UP_KEY                 0x01u
#define ESC_DOWN_KEY               0x02u
#define ESC_INSPECT_UP_BUTTON      0x04u
#define ESC_INSPECT_DOWN_BUTTON    0x08u
#define ESC_INSPECT_NORMAL_KEY     0x10u
#define ESC_RESET_BUTTON           0x20u
#define ESC_RUN_UP                 0x40u
#define ESC_RUN_DOWN               0x80u

#define ESC_STATE_FAST             0x01u
#define ESC_STATE_SLOW             0x02u
#define ESC_STATE_INTERM           0x04u
#define ESC_RUN_STAR               0x08u
#define ESC_RUN_DETLA              0x10u
#define ESC_SAFETY_RELAY_CLOSE     0x20u
#define ESC_AUX_RELAY_OPEN         0x40u
#define ESC_AUX_RELAY_CLOSE        0x80u

#define ESC_SAFETY_END_CLOSE            0x01u
#define ESC_SAFETY_END_ENABLE           0x02u
#define ESC_FAULT           0x80u
#define ESC_STOPPING_PROCESS_FINISH     0x80u

/* Input and Feedback */
#define INPUT_PORT1_8       EscRtData.SafetyInputData[0]
#define INPUT_PORT9_16      EscRtData.SafetyInputData[1]
#define INPUT_PORT17_24     EscRtData.SafetyInputData[2]
#define INPUT_PORT24_29     EscRtData.SafetyInputData[3]
#define INPUT_PORT_EX1_8    EscRtData.SafetyInputData[4]
#define INPUT_PORT_EX9_16   EscRtData.SafetyInputData[5]
#define INPUT_PORT_EX17     EscRtData.SafetyInputData[6]
#define INPUT_FEEDBACK      EscRtData.SafetyInputData[7]

#define INPUT_PORT1_MASK        (u8)( 1 << 0 )
#define INPUT_PORT2_MASK        (u8)( 1 << 1 )
#define INPUT_PORT3_MASK        (u8)( 1 << 2 )
#define INPUT_PORT4_MASK        (u8)( 1 << 3 )
#define INPUT_PORT5_MASK        (u8)( 1 << 4 )
#define INPUT_PORT6_MASK        (u8)( 1 << 5 )
#define INPUT_PORT7_MASK        (u8)( 1 << 6 )
#define INPUT_PORT8_MASK        (u8)( 1 << 7 )

#define INPUT_PORT9_MASK         (u8)( 1 << 0 )
#define INPUT_PORT10_MASK        (u8)( 1 << 1 )
#define INPUT_PORT11_MASK        (u8)( 1 << 2 )
#define INPUT_PORT12_MASK        (u8)( 1 << 3 )
#define INPUT_PORT13_MASK        (u8)( 1 << 4 )
#define INPUT_PORT14_MASK        (u8)( 1 << 5 )
#define INPUT_PORT15_MASK        (u8)( 1 << 6 )
#define INPUT_PORT16_MASK        (u8)( 1 << 7 )

#define INPUT_PORT17_MASK        (u8)( 1 << 0 )
#define INPUT_PORT18_MASK        (u8)( 1 << 1 )
#define INPUT_PORT19_MASK        (u8)( 1 << 2 )
#define INPUT_PORT20_MASK        (u8)( 1 << 3 )
#define INPUT_PORT21_MASK        (u8)( 1 << 4 )
#define INPUT_PORT22_MASK        (u8)( 1 << 5 )
#define INPUT_PORT23_MASK        (u8)( 1 << 6 )
#define INPUT_PORT24_MASK        (u8)( 1 << 7 )

#define INPUT_PORT25_MASK        (u8)( 1 << 0 )
#define INPUT_PORT26_MASK        (u8)( 1 << 1 )
#define INPUT_PORT27_MASK        (u8)( 1 << 2 )
#define INPUT_PORT28_MASK        (u8)( 1 << 3 )
#define INPUT_PORT29_MASK        (u8)( 1 << 4 )

#define INPUT_PORT_EX1_MASK        (u8)( 1 << 0 )
#define INPUT_PORT_EX2_MASK        (u8)( 1 << 1 )
#define INPUT_PORT_EX3_MASK        (u8)( 1 << 2 )
#define INPUT_PORT_EX4_MASK        (u8)( 1 << 3 )
#define INPUT_PORT_EX5_MASK        (u8)( 1 << 4 )
#define INPUT_PORT_EX6_MASK        (u8)( 1 << 5 )
#define INPUT_PORT_EX7_MASK        (u8)( 1 << 6 )
#define INPUT_PORT_EX8_MASK        (u8)( 1 << 7 )

#define INPUT_PORT_EX9_MASK         (u8)( 1 << 0 )
#define INPUT_PORT_EX10_MASK        (u8)( 1 << 1 )
#define INPUT_PORT_EX11_MASK        (u8)( 1 << 2 )
#define INPUT_PORT_EX12_MASK        (u8)( 1 << 3 )
#define INPUT_PORT_EX13_MASK        (u8)( 1 << 4 )
#define INPUT_PORT_EX14_MASK        (u8)( 1 << 5 )
#define INPUT_PORT_EX15_MASK        (u8)( 1 << 6 )
#define INPUT_PORT_EX16_MASK        (u8)( 1 << 7 )

#define INPUT_PORT_EX17_MASK        (u8)( 1 << 0 )

#define INPUT_PORT_SF_RL_DRV_FB_MASK            (u8)( 1 << 0 )
#define INPUT_PORT_SF_PWR_FB_CPU_MASK           (u8)( 1 << 1 )
#define INPUT_PORT_SF_RL_FB_MASK                (u8)( 1 << 2 )
#define INPUT_PORT_SF_RL_FB_CPU_MASK            (u8)( 1 << 3 )
#define INPUT_PORT_AUX_FB_MASK                  (u8)( 1 << 4 )
#define INPUT_PORT_PLUSE_OUTPUT_FB_MASK         (u8)( 1 << 5 )

/* Command data */
#define CMD_ESC_RUN_MODE        EscRtData.CommandData[2]

#define SfBase_EscState         EscRtData.CommandData[0]
#define CMD_FLAG2 	        EscRtData.CommandData[1]
#define CMD_ESC_KEY 	        EscRtData.CommandData[2]
#define CMD_ESC_RUN 	        EscRtData.CommandData[5]
#define CMD_ESC_ORDER 	        EscRtData.CommandData[4]
/* #define CMD_ESC_INSP 	        EscRtData.CommandData[5]*/
#define CMD_FLAG7 	        EscRtData.CommandData[6]
#define CMD_FLAG8 	        EscRtData.CommandData[7]

#define CMD_FLAG3 	        EscRtData.CommandData[2]
#define CMD_FLAG4 	        EscRtData.CommandData[3]
#define CMD_FLAG5 	        EscRtData.CommandData[4]
#define CMD_FLAG6 	        EscRtData.CommandData[5]


#define OmcSfBase_EscState 	OmcEscRtData.CommandData[0]
#define CMD_OMC_FLAG2    	OmcEscRtData.CommandData[1]
#define CMD_OMC_ESC_KEY 	OmcEscRtData.CommandData[2]
#define CMD_OMC_ESC_RUN 	OmcEscRtData.CommandData[3]
#define CMD_OMC_FLAG5 	        OmcEscRtData.CommandData[4]
#define CMD_OMC_ESC_INSP 	OmcEscRtData.CommandData[5]
#define CMD_OMC_FLAG7 	        OmcEscRtData.CommandData[6]
#define CMD_OMC_FLAG8 	        OmcEscRtData.CommandData[7]

/* Alarm data */
#define         ALARM1   	EscRtData.Alarm[0]
#define         ALARM2   	EscRtData.Alarm[1]
#define         ALARM3   	EscRtData.Alarm[2]
#define         ALARM4   	EscRtData.Alarm[3]


/* Fault data, 64*8 = 512 */
#define         EN_ERROR1   	EscRtData.ErrorBuff[0]
#define  	EN_ERROR2   	EscRtData.ErrorBuff[1]
#define 	EN_ERROR3   	EscRtData.ErrorBuff[2]
#define 	EN_ERROR4   	EscRtData.ErrorBuff[3]
#define 	EN_ERROR5   	EscRtData.ErrorBuff[4]
#define 	EN_ERROR6   	EscRtData.ErrorBuff[5]
#define 	EN_ERROR7   	EscRtData.ErrorBuff[6]
#define 	EN_ERROR8   	EscRtData.ErrorBuff[7]
#define 	EN_ERROR9   	EscRtData.ErrorBuff[8]
#define 	EN_ERROR10   	EscRtData.ErrorBuff[9]
#define         EN_ERROR11   	EscRtData.ErrorBuff[10]
#define  	EN_ERROR12   	EscRtData.ErrorBuff[11]
#define 	EN_ERROR13   	EscRtData.ErrorBuff[12]
#define 	EN_ERROR14   	EscRtData.ErrorBuff[13]
#define 	EN_ERROR15   	EscRtData.ErrorBuff[14]
#define 	EN_ERROR16   	EscRtData.ErrorBuff[15]
#define 	EN_ERROR17   	EscRtData.ErrorBuff[16]
#define 	EN_ERROR18   	EscRtData.ErrorBuff[17]
#define 	EN_ERROR19   	EscRtData.ErrorBuff[18]
#define 	EN_ERROR20   	EscRtData.ErrorBuff[19]
#define         EN_ERROR21   	EscRtData.ErrorBuff[20]
#define  	EN_ERROR22   	EscRtData.ErrorBuff[21]
#define 	EN_ERROR23   	EscRtData.ErrorBuff[22]
#define 	EN_ERROR24   	EscRtData.ErrorBuff[23]
#define 	EN_ERROR25   	EscRtData.ErrorBuff[24]
#define 	EN_ERROR26   	EscRtData.ErrorBuff[25]
#define 	EN_ERROR27   	EscRtData.ErrorBuff[26]
#define 	EN_ERROR28   	EscRtData.ErrorBuff[27]
#define 	EN_ERROR29   	EscRtData.ErrorBuff[28]
#define 	EN_ERROR30   	EscRtData.ErrorBuff[29]
#define         EN_ERROR31   	EscRtData.ErrorBuff[30]
#define  	EN_ERROR32   	EscRtData.ErrorBuff[31]
#define         EN_ERROR33   	EscRtData.ErrorBuff[32]
#define  	EN_ERROR34   	EscRtData.ErrorBuff[33]
#define 	EN_ERROR35   	EscRtData.ErrorBuff[34]
#define 	EN_ERROR36   	EscRtData.ErrorBuff[35]
#define 	EN_ERROR37   	EscRtData.ErrorBuff[36]
#define 	EN_ERROR38   	EscRtData.ErrorBuff[37]
#define 	EN_ERROR39   	EscRtData.ErrorBuff[38]
#define 	EN_ERROR40   	EscRtData.ErrorBuff[39]
#define 	EN_ERROR41   	EscRtData.ErrorBuff[40]
#define 	EN_ERROR42   	EscRtData.ErrorBuff[41]
#define         EN_ERROR43   	EscRtData.ErrorBuff[42]
#define  	EN_ERROR44   	EscRtData.ErrorBuff[43]
#define 	EN_ERROR45   	EscRtData.ErrorBuff[44]
#define 	EN_ERROR46   	EscRtData.ErrorBuff[45]
#define 	EN_ERROR47   	EscRtData.ErrorBuff[46]
#define 	EN_ERROR48   	EscRtData.ErrorBuff[47]
#define 	EN_ERROR49   	EscRtData.ErrorBuff[48]
#define 	EN_ERROR50   	EscRtData.ErrorBuff[49]
#define 	EN_ERROR51   	EscRtData.ErrorBuff[50]
#define 	EN_ERROR52   	EscRtData.ErrorBuff[51]
#define         EN_ERROR53   	EscRtData.ErrorBuff[52]
#define  	EN_ERROR54   	EscRtData.ErrorBuff[53]
#define 	EN_ERROR55   	EscRtData.ErrorBuff[54]
#define 	EN_ERROR56   	EscRtData.ErrorBuff[55]
#define 	EN_ERROR57   	EscRtData.ErrorBuff[56]
#define 	EN_ERROR58   	EscRtData.ErrorBuff[57]
#define 	EN_ERROR59   	EscRtData.ErrorBuff[58]
#define 	EN_ERROR60   	EscRtData.ErrorBuff[59]
#define 	EN_ERROR61   	EscRtData.ErrorBuff[60]
#define 	EN_ERROR62   	EscRtData.ErrorBuff[61]
#define         EN_ERROR63   	EscRtData.ErrorBuff[62]
#define  	EN_ERROR64   	EscRtData.ErrorBuff[63]


/* Sensor Data */
#define         MOTOR_SPEED1                    EscRtData.SensorData[0]
#define         MOTOR_SPEED2                    EscRtData.SensorData[2]

#define         MAIN_SHAFT_SPEED1               EscRtData.SensorData[4]
#define         MAIN_SHAFT_SPEED2               EscRtData.SensorData[6]

#define         HANDRAIL_SPEED_LEFT             EscRtData.SensorData[8]
#define         HANDRAIL_SPEED_RIGHT            EscRtData.SensorData[10]

#define         STOPPING_DISTANCE1              EscRtData.SensorData[12]
#define         STOPPING_DISTANCE2              EscRtData.SensorData[14]

#define         MISSINGSTEP_MTR_PULSE1          EscRtData.SensorData[16]
#define         MISSINGSTEP_MTR_PULSE2          EscRtData.SensorData[18]


#define         OMC_MOTOR_SPEED1                    *(u16*)&OmcEscRtData.SensorData[0]
#define         OMC_MOTOR_SPEED2                    *(u16*)&OmcEscRtData.SensorData[2]

#define         OMC_MAIN_SHAFT_SPEED1               *(u16*)&OmcEscRtData.SensorData[4]
#define         OMC_MAIN_SHAFT_SPEED2               *(u16*)&OmcEscRtData.SensorData[6]

#define         OMC_HANDRAIL_SPEED_LEFT             *(u16*)&OmcEscRtData.SensorData[8]
#define         OMC_HANDRAIL_SPEED_RIGHT            *(u16*)&OmcEscRtData.SensorData[10]

#define         OMC_STOPPING_DISTANCE1              *(u16*)&OmcEscRtData.SensorData[12]
#define         OMC_STOPPING_DISTANCE2              *(u16*)&OmcEscRtData.SensorData[14]

#define         OMC_MISSINGSTEP_MTR_PULSE1          *(u16*)&OmcEscRtData.SensorData[16]
#define         OMC_MISSINGSTEP_MTR_PULSE2          *(u16*)&OmcEscRtData.SensorData[18]




#define PARA_INIT                       *(u16*)&ParameterData[0]
/* parameters */
#define MOTOR_RPM                       *(u16*)&ParameterData[2]
#define NOMINAL_SPEED                   *(u16*)&ParameterData[4]
#define MOTOR_PLUSE_PER_REV             *(u16*)&ParameterData[6]
#define STEP_WIDTH                      *(u16*)&ParameterData[8]
#define END_SAFETY_STRING_FAULT_TYPE    *(u16*)&ParameterData[10]
#define KEY_MINIMUM_TIME                *(u16*)&ParameterData[12]
#define UNDERSPEED_TIME                 *(u16*)&ParameterData[14]
#define TANDEM_TYPE                     *(u16*)&ParameterData[16]
#define ROLLER_HR_RADIUS                *(u16*)&ParameterData[18]
#define HR_PULSES_PER_REV               *(u16*)&ParameterData[20]
#define HR_FAULT_TIME                   *(u16*)&ParameterData[22]
#define BRAKE_STATUS_SENSORS            *(u16*)&ParameterData[24]
#define UP_DOWN_ALLOWED                 *(u16*)&ParameterData[26]

#define RESET_MINIMUM_TIME              *(u16*)&ParameterData[28]
#define CONTACTORS_TIMEOUT              *(u16*)&ParameterData[30]
#define DRIVE_CHAIN_DELAY               *(u16*)&ParameterData[32]
#define HANDRAIL_MOTOR_PULSE            *(u16*)&ParameterData[34]

#define DIAGNOSTIC                      *(u16*)&ParameterData[36]
#define DIAGNOSTIC_BOARD_L2_QUANTITY    *(u16*)&ParameterData[38]
#define DIAGNOSTIC_BOARD_L1_QUANTITY    *(u16*)&ParameterData[40]
/*
#define KEY_MINIMUM_TIME                *(u16*)&ParameterData[42]
#define UP_DOWN_ALLOWED                 *(u16*)&ParameterData[44]
#define RESET_MINIMUM_TIME              *(u16*)&ParameterData[46]
#define END_SAFETY_STRING_FAULT_TYPE    *(u16*)&ParameterData[48]
#define CONTACTORS_TIMEOUT              *(u16*)&ParameterData[50]
#define BRAKE_STATUS_SENSORS            *(u16*)&ParameterData[52]
#define DRIVE_CHAIN_DELAY               *(u16*)&ParameterData[54]
*/


/* parameter range */
#define MOTOR_RPM_MIN                   500u
#define MOTOR_RPM_MAX                   2500u

#define NOMINAL_SPEED1                   500u
#define NOMINAL_SPEED2                  650u
#define NOMINAL_SPEED3                   750u
#define NOMINAL_SPEED4                   900u

#define MOTOR_PLUSE_PER_REV_MIN           1u
#define MOTOR_PLUSE_PER_REV_MAX           20u

#define STEP_WIDTH_MIN                  1u
#define STEP_WIDTH_MAX                  1000u

#define END_SAFETY_STRING_FAULT_TYPE_MIN                  0u
#define END_SAFETY_STRING_FAULT_TYPE_MAX                  1u

#define KEY_MINIMUM_TIME_MIN                  100u
#define KEY_MINIMUM_TIME_MAX                  4000u

#define UNDERSPEED_TIME_MIN                  0u
#define UNDERSPEED_TIME_MAX                  10000u

#define TANDEM_TYPE_MIN                  0u
#define TANDEM_TYPE_MAX                  2u

#define ROLLER_HR_RADIUS_MIN                  10u
#define ROLLER_HR_RADIUS_MAX                  120u

#define HR_PULSES_PER_REV_MIN                  1u
#define HR_PULSES_PER_REV_MAX                  8u

#define HR_FAULT_TIME_MIN                  2u
#define HR_FAULT_TIME_MAX                  20u

#define BRAKE_STATUS_SENSORS_MIN                  1u
#define BRAKE_STATUS_SENSORS_MAX                  8u

#define UP_DOWN_ALLOWED_MIN                  0u
#define UP_DOWN_ALLOWED_MAX                  2u

#define NO_DIAGNOSTIC_BOARD     0u
#define DIAGNOSTIC_BOARD_1      1u
#define DIAGNOSTIC_BOARD_2      2u



/* Exported functions ------------------------------------------------------- */
void Esc_Control(void);

extern u8 g_u8FaultCodeStore;
extern u32 g_u32InitTestError;
extern u16 g_u16ParameterLoadingError;
extern MotorSpeedItem MTRITEM[2];
extern HandrailSpeedItem HDL_Right;
extern HandrailSpeedItem HDL_Left;
extern STEPMISSINGITEM STPMS_UPPER;
extern STEPMISSINGITEM STPMS_LOWER;
extern UpDownKeyItem UpKey;
extern UpDownKeyItem DownKey;
extern u8 SAFETY_SWITCH_STATUS[4];
extern u8 g_u8ResetButton;
extern u8 g_u8SafetyRelayStartCheck;
extern u8 TandemRunEnable;
extern u8 TandemMessageRunAllowed;
extern u8 Tandemoutput;
extern volatile u16 g_u16DBL1NewData;
extern volatile u32 g_u16DBL2NewData;
extern u16 g_u16CAN2SendFail;

extern u8 EscRTBuff[200];
extern u8 McRxBuff[200];

extern u8 g_u8CanCommunicationToCotrolID;
extern u8 g_u8CanCommunicationToCotrolLen;
extern u8 g_u8CanCommunicationToCotrolOk;

/* ESC */
extern u8 ParameterData[200];

/* 5 fault code, 1 alarm code */
extern u16 EscErrorCodeBuff[6];
extern u8 EscErrorBuff[64];

#ifdef GEC_SF_MASTER
/* state for display */
extern u8 StateDisplay[8];
#endif

/* ESC rt data */
extern SafetyEscData EscRtData;
extern SafetyEscData OmcEscRtData;

/* Control board data */
extern u8 EscDataToControl[20][8];;
extern u8 EscDataFromControl[3][8];

/* DBL1 data */
extern u8 EscDataToDBL1[3][8];
extern u8 EscDataFromDBL1[4][8];

/* DBL2 data */
extern u8 EscDataToDBL2[8][8];
extern u8 EscDataFromDBL2[16][8];

#define ESC_RT_DATA_LEN    sizeof(SafetyEscData)

#endif  /* __ESC_H */


/******************************  END OF FILE  *********************************/








