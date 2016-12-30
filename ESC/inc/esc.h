/*******************************************************************************
* File Name          : esc.h
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 11/22/2016
* Description        : This file contains esc parameters.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_H
#define __ESC_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "esc_parameter_process.h"
#include "esc_cfg_io.h"
#include <string.h>

/* Exported types ------------------------------------------------------------*/
/* ESC STATE */
enum EscState
{
    ESC_INIT_STATE = 0u,
    ESC_RUN_STATE = 1u,    
    ESC_STARTING_PROCESS_STATE = 2u,    
    ESC_READY_STATE = 4u,
    ESC_STOPPING_PROCESS_STATE = 5u,
    ESC_INTERM_STATE = 6u,
    ESC_FAULT_STATE = 7u
};

typedef struct dbl1esc
{
    /* input data */
    u8 InputData[4];

    /* Output data */
    u8 OutputData;
      
}DBL1Esc;

/* Enable Diagnostic level 2 board */
/*#define DIAGNOSTIC_LEVEL2*/
#ifdef DIAGNOSTIC_LEVEL2
typedef struct dbl2esc
{
    /* type */
    u8 BoardType;
    
    /* connection */
    u8 Connection; 
      
    /* SEQN */
    u8 SEQN;
    
    /* input data */
    u8 InputData[4];

    /* Output data */
    u8 OutputData;
    
    /* ANALOG/PT100 */
    u16 AnalogData[3];
  
    /* safety comm send data */
    u8 SendData[8];
    
    /* safety comm receive data */
    u8 ReceiveDataA[14];
    u8 ReceiveDataB[14];   
  
}DBL2Esc;
#endif

typedef struct sfescdata 
{
    /* Header code and comm times */
    u8 HeaderCode[6];
    
    /* Input and Feedback */
    u8 SafetyInputData[8];
    u8 ControlInputData[2];
    
    /* Command data*/
    u8 CommandData[10];
    
    /* Warning data */
    u8 Warn[8];
    
    /* Fault data */
    u8 ErrorBuff[64];

    /* Sensor Data */
    u8 SensorData[24];

    /* Error code */
    u16 ErrorCode[5];
       
    /* Configurable input buff */
    u8 Cfg_Input_Mask[50]; 
    u8 Cfg_Input_Level[50]; 

    /* DBL1 */
    DBL1Esc DBL1Upper;
    DBL1Esc DBL1Lower;
    DBL1Esc DBL1Interm1;
    DBL1Esc DBL1Interm2;
    
    /* Parameter */
    u8 ParaStatus;
    u8 ParameterData[10];
    /* Esc parameter crc value */
    u32 ParaCRC[2];    
    
    /* Safety switch state */
    u8 SafetySwitchState;
    u8 SafetySwitchEnter;

#ifdef DIAGNOSTIC_LEVEL2    
    /* DBL2 */
    DBL2Esc DBL2Upper;
    DBL2Esc DBL2Lower;
    DBL2Esc DBL2Interm1;
    DBL2Esc DBL2Interm2;
    
    /* DBL2 comm validate result */
    u8 DBL2ValidateResult;
#endif
    
    u8 DataFromControl[3][8];
    
}SafetyEscData;    


typedef struct motorspeeditem 
{    
    /* Sensor Type */
    u8 SensorType;
    
    /* Sensor NO. */
    u8 SensorX;
    
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
    
    /* motor between pulse last time */
    u32 LastMotorSpeedPulseTime;    
    
    /* motor between pulse counter */
    u8 between_pulse_counter;

    /* motor no pulse timer */
    u16 TimerNoPulse;

    /* timeout stopping process */
    u16 TimeoutTimerMotorSpeed;
    
    /* Sensor use timer init */
    void (*Timer_Init_Sensor)(u16 p1,u16 p2);
    
    /* Sensor use timer number */
    TIM_TypeDef* TimerSensorNum;      
  
    /* Brake distance */
    u16 *const ptBrakeDistanceBuff;
    
    /* shopping process motor speed pulse */
    volatile u16 rt_brake_pulse;     
    
    /* shopping finish flag */
    u16 rt_brake_stop ;    
    
}MotorSpeedItem;



typedef struct handrailspeeditem 
{
    /* Sensor NO. */
    u8 SensorX;
    
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
    u16 *const ptHDLSpeed; 

    /* Handrail speed pulse of motor */
    u16 *const ptHDLMotorOfPulse;
    
    /* handrail counter pulse */
    u16 HDL_Pulse_Value;
   	
}HandrailSpeedItem;

typedef struct stepmissingitem 
{
    /* Sensor NO. */
    u8 SensorX;
    
    /* motor speed real time pulse */
    volatile u16 MtrPulse;	
    
    /* Missing step rising edge detecte */
    u8 rising_edge_detected[2];
    
    /* Measure number of pulses of motor speed between two consecutives pulses of missing step */
    u16 *const ptStepMtrBuff;
    
    /* Missing step in ready state time counter */
    u32 ms_ready_delay;
    
    u16 Motor_speed_pulse_counter;
    
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
#define ESC_NORMAL      0x10u
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

#define ESC_SAFETY_END_CLOSE            0x01u
#define ESC_SAFETY_END_ENABLE           0x02u
#define ESC_FAULT           0x80u


/* esc communicaiton, safety to control interface data */
/* Message ID: 100h */
/* byte 1 */
/* order */
#define ORDER_RUN                       0x01u
#define ORDER_STOP                      0x02u
#define ORDER_FAULT                     0x04u
#define ORDER_RESET                     0x08u
#define ORDER_ACTIVATE_UP_CONTACTOR     0x10u
#define ORDER_SLOW_FAST                 0x20u
#define ORDER_UP_DOWN                   0x40u
#define ORDER_CAPACITOR                 0x80u

/* byte 2 */
/* state */
#define ESC_STATE_MASK                  0x07u
#define ESC_STATE_INIT                  0x00u
#define ESC_STATE_READY                 0x04u
#define ESC_STATE_STARTING_PROCESS      0x02u
#define ESC_STATE_RUN                   0x01u
#define ESC_STATE_INTERM                0x06u
#define ESC_STATE_STOPPING_PROCESS      0x05u
#define ESC_STATE_FAULT                 0x07u

/* mode1 */
#define ESC_MODE1_MASK                  0x18u
#define ESC_MODE1_NORMAL                0x10u
#define ESC_MODE1_INSPECTION            0x08u

/* mode2 */
#define ESC_MODE2_MASK                   0xE0u
#define ESC_MODE2_CONTINOUS              0x00u
#define ESC_MODE2_INTERMITTEND           0x20u
#define ESC_MODE2_STBY                   0x40u
#define ESC_MODE2_INTERMITEND_STBY       0x60u
#define ESC_MODE2_2DIRECTION             0x80u
#define ESC_MODE2_Y_D_SWITCHBACK         0xA0u

/* byte 3 */
#define ESC_INTERM_STOP                 0x01u
#define ESC_INSP_BUTTONS_RELEASED       0x02u
#define ESC_REMOTE_STOP                 0x04u
#define ESC_SAFETY_CURTAIN_STOP         0x08u
#define ESC_STOPPING_PROCESS_FINISH     0x10u
#define ESC_SWITCH_ON_ELEM_UPPER        0x20u
#define ESC_SWITCH_ON_ELEM_LOWER        0x40u
#define ESC_HANDRAIL_ASME               0x80u


/* esc communicaiton, control data to safety */
/* Message ID: 200h */
/* byte 1 */
/* order */
#define ORDER_REMOTE_KEY_SWITCH_MASK              0x03u
#define ORDER_REMOTE_KEY_SWITCH_ON                0x02u
#define ORDER_REMOTE_KEY_SWITCH_OFF               0x01u
#define ORDER_FROM_CB_REMOTE_RUN_MASK                     0x0Cu
#define ORDER_FROM_CB_REMOTE_RUN_ON                       0x08u
#define ORDER_FROM_CB_REMOTE_RUN_OFF                      0x04u
#define ORDER_FROM_CB_STOP                                0x10u
#define ORDER_FROM_CB_FAULT                               0x20u
#define ORDER_FROM_CB_RESET                               0x40u

/* byte 2 */
/* state */
#define ESC_FROM_CB_STATE_MASK                          0x07u
#define ESC_FROM_CB_STATE_INIT                          0x00u
#define ESC_FROM_CB_STATE_READY                         0x04u
#define ESC_FROM_CB_SF_STATE_RUNNING                    0x01u
#define ESC_FROM_CB_SF_STATE_STOPPING_PROCESS           0x05u
#define ESC_FROM_CB_SF_STATE_FAULT                      0x07u

/* mode */
#define ESC_FROM_CB_MODE_LOCAL                  0x10u
#define ESC_FROM_CB_MODE_REMOTE                 0x08u
#define ESC_FROM_CB_MODE_SLOW                   0x20u
#define ESC_FROM_CB_MODE_INTERMITTEND           0x40u
#define ESC_FROM_CB_MODE_2DIRECTION             0x80u


/* byte 3 */
#define ESC_FROM_CB_DIRECTION_MASK               0x03u
#define ESC_FROM_CB_DIRECTION_UP                 0x02u
#define ESC_FROM_CB_DIRECTION_DOWN               0x01u
#define ESC_FROM_CB_MOTOR_MASK                  0x0cu
#define ESC_FROM_CB_MOTOR_DELTA                 0x08u
#define ESC_FROM_CB_MOTOR_STAR                  0x04u
#define ESC_FROM_CB_TEST_AUX_BRAKE              0x10u
#define ESC_FROM_CB_TYPE_OF_CONTROL_FAULT       0x60u
#define ESC_FROM_CB_FAULT_FAILURE_LOCK          0x60u
#define ESC_FROM_CB_FAULT_STANDARD_FAULT        0x20u
#define ESC_FROM_CB_FAULT_RECOVERY_STOP         0x40u


/* byte 4 */
#define ESC_CONTACTORS_OUTPUTS_K1_1             0x01u
#define ESC_CONTACTORS_OUTPUTS_K1_2             0x02u
#define ESC_CONTACTORS_OUTPUTS_K2_1_1           0x04u
#define ESC_CONTACTORS_OUTPUTS_K2_2_1           0x08u
#define ESC_CONTACTORS_OUTPUTS_K10_1            0x10u
#define ESC_CONTACTORS_OUTPUTS_K10_5            0x20u
#define ESC_CONTACTORS_OUTPUTS_K2_1_2           0x40u
#define ESC_CONTACTORS_OUTPUTS_K2_2_2           0x80u

/* led flash freq */
#define FREQ_0_5HZ      200u
#define FREQ_1HZ        100u
#define FREQ_2HZ        50u
#define FREQ_3HZ        33u

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


/*define function input*/
#define ISP_NORMAL_INPUT        (EscRtData. SafetyInputData[3] & 0x10u)



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

#define CMD_FLAG10 	        EscRtData.CommandData[9]


#define OmcSfBase_EscState 	OmcEscRtData.CommandData[0]
#define CMD_OMC_FLAG2    	OmcEscRtData.CommandData[1]
#define CMD_OMC_ESC_KEY 	OmcEscRtData.CommandData[2]
#define CMD_OMC_ESC_RUN 	OmcEscRtData.CommandData[3]
#define CMD_OMC_FLAG5 	        OmcEscRtData.CommandData[4]
#define CMD_OMC_ESC_INSP 	OmcEscRtData.CommandData[5]
#define CMD_OMC_FLAG7 	        OmcEscRtData.CommandData[6]
#define CMD_OMC_FLAG8 	        OmcEscRtData.CommandData[7]

/* Alarm data */
#define         EN_WARN1   	EscRtData.Warn[0]
#define         EN_WARN2   	EscRtData.Warn[1]
#define         EN_WARN3   	EscRtData.Warn[2]
#define         EN_WARN4   	EscRtData.Warn[3]
#define         EN_WARN5   	EscRtData.Warn[4]
#define         EN_WARN6   	EscRtData.Warn[5]
#define         EN_WARN7   	EscRtData.Warn[6]
#define         EN_WARN8   	EscRtData.Warn[7]

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
#define         MOTOR_SPEED1                    *(u16*)&EscRtData.SensorData[0]
#define         MOTOR_SPEED2                    *(u16*)&EscRtData.SensorData[2]

#define         MAIN_SHAFT_SPEED1               *(u16*)&EscRtData.SensorData[4]
#define         MAIN_SHAFT_SPEED2               *(u16*)&EscRtData.SensorData[6]

#define         HANDRAIL_SPEED_LEFT             *(u16*)&EscRtData.SensorData[8]
#define         HANDRAIL_SPEED_RIGHT            *(u16*)&EscRtData.SensorData[10]

#define         HANDRAIL_SPEED_LEFT_MOTOR_PULSE           *(u16*)&EscRtData.SensorData[12]
#define         HANDRAIL_SPEED_RIGHT_MOTOR_PULSE          *(u16*)&EscRtData.SensorData[14]

#define         MISSINGSTEP_MTR_PULSE1          *(u16*)&EscRtData.SensorData[16]
#define         MISSINGSTEP_MTR_PULSE2          *(u16*)&EscRtData.SensorData[18]

#define         STOPPING_DISTANCE1              *(u16*)&EscRtData.SensorData[20]
#define         STOPPING_DISTANCE2              *(u16*)&EscRtData.SensorData[22]

#define         MAIN_SHAFT_SPEED3               *(u16*)&EscRtData.SensorData[2]


#define         OMC_MOTOR_SPEED1                    *(u16*)&OmcEscRtData.SensorData[0]
#define         OMC_MOTOR_SPEED2                    *(u16*)&OmcEscRtData.SensorData[2]

#define         OMC_MAIN_SHAFT_SPEED1               *(u16*)&OmcEscRtData.SensorData[4]
#define         OMC_MAIN_SHAFT_SPEED2               *(u16*)&OmcEscRtData.SensorData[6]

#define         OMC_HANDRAIL_SPEED_LEFT             *(u16*)&OmcEscRtData.SensorData[8]
#define         OMC_HANDRAIL_SPEED_RIGHT            *(u16*)&OmcEscRtData.SensorData[10]

#define         OMC_HANDRAIL_SPEED_LEFT_MOTOR_PULSE       *(u16*)&OmcEscRtData.SensorData[12]
#define         OMC_HANDRAIL_SPEED_RIGHT_MOTOR_PULSE      *(u16*)&OmcEscRtData.SensorData[14]

#define         OMC_MISSINGSTEP_MTR_PULSE1          *(u16*)&OmcEscRtData.SensorData[16]
#define         OMC_MISSINGSTEP_MTR_PULSE2          *(u16*)&OmcEscRtData.SensorData[18]

#define         OMC_STOPPING_DISTANCE1              *(u16*)&OmcEscRtData.SensorData[20]
#define         OMC_STOPPING_DISTANCE2              *(u16*)&OmcEscRtData.SensorData[22]

#define         OMC_MAIN_SHAFT_SPEED3               *(u16*)&OmcEscRtData.SensorData[2]


/*** Timer usage ***
Timer1: motor speed 1 & motor speed 2
Timer2: motor direction / main shaft 3
Timer3: main shaft 1 & main shaft 2
Timer4: main shaft direction
Timer5: sensor shortcircuit(motor1/motor2,handrail1/handrail2,missingstep1/missingstep2,mainshaft1/mainshaft2/mainshaft3)
Timer6: system time delay 
Timer7: self check
*/

#define MOTOR_SPEED1_CHECK_TIMER                TIM1
#define MOTOR_SPEED1_CHECK_TIMER_INIT           TIM1_Int_Init
#define MOTOR_SPEED2_CHECK_TIMER                TIM1
#define MOTOR_SPEED2_CHECK_TIMER_INIT           TIM1_Int_Init

#define MOTOR_DIRECTION_CHECK_TIMER             TIM2
#define MOTOR_DIRECTION_CHECK_TIMER_INIT        TIM2_Int_Init
#define MAIN_SHAFT_SPEED3_CHECK_TIMER           TIM2
#define MAIN_SHAFT_SPEED3_CHECK_TIMER_INIT      TIM2_Int_Init

#define MAIN_SHAFT_SPEED1_CHECK_TIMER           TIM3
#define MAIN_SHAFT_SPEED1_CHECK_TIMER_INIT      TIM3_Int_Init
#define MAIN_SHAFT_SPEED2_CHECK_TIMER           TIM3
#define MAIN_SHAFT_SPEED2_CHECK_TIMER_INIT      TIM3_Int_Init

#define MAIN_SHAFT_DIRECTION_TIMER              TIM5
#define MAIN_SHAFT_DIRECTION_TIMER_INIT         TIM5_Int_Init

#define SYSTEM_DELAY_TIMER                      TIM4

/* Motor/handrail/missing step/main shaft shortcircuit use the same timer, check one by one. */
#define SENSOR_SHORTCIRCUIT_CHECK_TIMER         TIM6

#define CPU_SELF_CHECK_TIMER                    TIM7

/* for debug */
/*#define ESC_TEST*/

/* SF <----> CB */
#define SF_TO_CONTROL_DATA_LEN   21u
#define DATA_FROM_CONTROL_LEN    3u

/* SF <----> DBL1 */
#define SF_TO_DBL1_DATA_LEN   3u
#define DATA_FROM_DBL1_LEN    4u

/* SF <----> DBL2 */
#define SF_TO_DBL2_DATA_LEN   8u
#define DATA_FROM_DBL2_LEN    16u

/* Exported functions ------------------------------------------------------- */
void Esc_Control(void);

extern u8 g_u8FaultCodeStore;
extern u32 g_u32InitTestError;
extern u16 g_u16ParameterLoadingError;
extern u8 g_u8ParameterLoadingFinish;
extern u8 g_u8ParameterLoading;
extern MotorSpeedItem MTRITEM[2];
extern MotorSpeedItem MAINSHAFTITEM[3];
extern HandrailSpeedItem HDL_Right;
extern HandrailSpeedItem HDL_Left;
extern STEPMISSINGITEM STPMS_UPPER;
extern STEPMISSINGITEM STPMS_LOWER;
extern UpDownKeyItem UpKey;
extern UpDownKeyItem DownKey;
extern u8 SAFETY_SWITCH_STATUS[4];
extern u8 g_u8ResetType;
/*extern u8 g_u8SafetyRelayStartCheck; */
extern volatile u16 g_u16DBL1NewData;
extern u16 g_u16ContorlNewData;
#ifdef DIAGNOSTIC_LEVEL2
extern u16 g_u16DBL2NewData;
#endif
extern u8 g_u8CAN2SendFail;
extern u8 DIAGNOSTIC;
extern u8 g_u8LedFreq;

extern u8 g_u8CanCommunicationToCotrolID;
extern u8 g_u8CanCommunicationToCotrolLen;
extern u8 g_u8CanCommunicationToCotrolOk;

extern u8 g_u8SensorShortCircuitCheck;
extern u8 g_u8EscStoppingFinish;

/* ESC */
extern SFPara SFParameterData;
extern CBParaInSF CBParameterInSafety;
extern CBPara CBParameterData;

/* 5 fault code */
extern u16 EscErrorCodeBuff[5];
extern u8 EscErrorBuff[64];

#ifdef GEC_SF_MASTER
/* 4 warn code */
extern u16 EscWarnCodeBuff[5];
extern u16 g_u16EndOfSafetyStringVoltage;
#endif

/* ESC rt data */
extern SafetyEscData EscRtData;
extern SafetyEscData OmcEscRtData;

/* Control board data */
extern u8 EscDataToControl[SF_TO_CONTROL_DATA_LEN][8];
extern u8 EscDataToControlBuffer[SF_TO_CONTROL_DATA_LEN][8];
extern u32 SafetyCommToControlID[SF_TO_CONTROL_DATA_LEN];

/* DBL1 data */
extern u8 EscDataToDBL1[SF_TO_DBL1_DATA_LEN][8];
extern u8 EscDataFromDBL1[DATA_FROM_DBL1_LEN][8];

#ifdef DIAGNOSTIC_LEVEL2
/* DBL2 data */
extern u8 EscDataToDBL2[SF_TO_DBL2_DATA_LEN][8];
extern u8 EscDataFromDBL2[DATA_FROM_DBL2_LEN][8];
#endif

/* Parameter data */
extern u8 ParaDataToControl[2][8];
extern u8 ParaDataFromControl[5][8];

#define ESC_RT_DATA_LEN    sizeof(SafetyEscData)

#endif  /* __ESC_H */


/******************************  END OF FILE  *********************************/








