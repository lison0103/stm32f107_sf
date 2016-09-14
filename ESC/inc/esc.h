/*******************************************************************************
* File Name          : esc.h
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 09/06/2016
* Description        : This file contains esc parameters.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_H
#define __ESC_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"


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

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* old, not use */
#define ESC_STATE_STOP        ((u16)0x0001)
#define ESC_STATE_INSP        ((u16)0x0002)
#define ESC_STATE_RUN         ((u16)0x0004)
#define ESC_STATE_FAST        ((u16)0x0008)
#define ESC_STATE_SLOW        ((u16)0x0010)
#define ESC_STATE_INTT        ((u16)0x0020)
#define ESC_STATE_SPEEDUP     ((u16)0x0040)
#define ESC_RUN_STATE5S       ((u16)0x0080)
#define ESC_STATE_UP          ((u16)0x0100)
#define ESC_STATE_DOWN        ((u16)0x0200)
#define ESC_STATE_READY       ((u16)0x0400)
#define ESC_STATE_NORMAL      ((u16)0x0800)
#define ESC_STATE_FAULT       ((u16)0x1000)


#define INPUT_PORT1_8       EscRTBuff[4]
#define INPUT_PORT9_16      EscRTBuff[5]
#define INPUT_PORT17_24     EscRTBuff[6]
#define INPUT_PORT24_29     EscRTBuff[7]
#define INPUT_PORT_EX1_8    EscRTBuff[8]
#define INPUT_PORT_EX9_16   EscRTBuff[9]
#define INPUT_PORT_EX17     EscRTBuff[10]
#define INPUT_FEEDBACK      EscRTBuff[11]

#define INPUT_PORT1_MASK        (u8)( 1 << 0 )
#define INPUT_PORT2_MASK        (u8)( 1 << 1 )
#define INPUT_PORT3_MASK        (u8)( 1 << 2 )
#define INPUT_PORT4_MASK        (u8)( 1 << 3 )
#define INPUT_PORT5_MASK        (u8)( 1 << 4 )
#define INPUT_PORT6_MASK        (u8)( 1 << 5 )
#define INPUT_PORT7_MASK        (u8)( 1 << 6 )
#define INPUT_PORT8_MASK        (u8)( 1 << 7 )

#define INPUT_PORT9_MASK        (u8)( 1 << 0 )
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

#define INPUT_PORT_EX9_MASK        (u8)( 1 << 0 )
#define INPUT_PORT_EX10_MASK        (u8)( 1 << 1 )
#define INPUT_PORT_EX11_MASK        (u8)( 1 << 2 )
#define INPUT_PORT_EX12_MASK        (u8)( 1 << 3 )
#define INPUT_PORT_EX13_MASK        (u8)( 1 << 4 )
#define INPUT_PORT_EX14_MASK        (u8)( 1 << 5 )
#define INPUT_PORT_EX15_MASK        (u8)( 1 << 6 )
#define INPUT_PORT_EX16_MASK        (u8)( 1 << 7 )

#define INPUT_PORT_EX17_MASK        (u8)( 1 << 0 )

#define INPUT_PORT_SF_RL_DRV_FB_MASK        (u8)( 1 << 0 )
#define INPUT_PORT_SF_PWR_FB_CPU_MASK        (u8)( 1 << 1 )
#define INPUT_PORT_SF_RL_FB_MASK        (u8)( 1 << 2 )
#define INPUT_PORT_SF_RL_FB_CPU_MASK        (u8)( 1 << 3 )
#define INPUT_PORT_AUX_FB_MASK        (u8)( 1 << 4 )
#define INPUT_PORT_PLUSE_OUTPUT_FB_MASK        (u8)( 1 << 5 )

#define CMD_FLAG1 	EscRTBuff[20]
#define CMD_FLAG2 	EscRTBuff[21]
#define CMD_FLAG3 	EscRTBuff[22]
#define CMD_FLAG4 	EscRTBuff[23]
#define CMD_FLAG5 	EscRTBuff[24]
#define CMD_FLAG6 	EscRTBuff[25]
#define CMD_FLAG7 	EscRTBuff[26]
#define CMD_FLAG8 	EscRTBuff[27]
#define CMD_FLAG9 	EscRTBuff[28]
#define CMD_FLAG10 	EscRTBuff[29]

#define CMD_OMC_FLAG1 	McRxBuff[20]
#define CMD_OMC_FLAG2 	McRxBuff[21]
#define CMD_OMC_FLAG3 	McRxBuff[22]
#define CMD_OMC_FLAG4 	McRxBuff[23]
#define CMD_OMC_FLAG5 	McRxBuff[24]
#define CMD_OMC_FLAG6 	McRxBuff[25]
#define CMD_OMC_FLAG7 	McRxBuff[26]
#define CMD_OMC_FLAG8 	McRxBuff[27]
#define CMD_OMC_FLAG9 	McRxBuff[28]
#define CMD_OMC_FLAG10 	McRxBuff[29]

#define MTR_MAX_SPEED       *(u16*)&EscRTBuff[35]
#define MTR_MIN_SPEED       *(u16*)&EscRTBuff[37]
#define SHORTCIRCUIT_ERROR  EscRTBuff[39]
#define MS_SYNC_RUN_ERROR   EscRTBuff[66]

#define         EN_ERROR1   	EscRTBuff[70]
#define  	EN_ERROR2   	EscRTBuff[71]
#define 	EN_ERROR3   	EscRTBuff[72]
#define 	EN_ERROR4   	EscRTBuff[73]
#define 	EN_ERROR5   	EscRTBuff[74]
#define 	EN_ERROR6   	EscRTBuff[75]
#define 	EN_ERROR7   	EscRTBuff[76]
#define 	EN_ERROR8   	EscRTBuff[77]
#define 	EN_ERROR9   	EscRTBuff[78]
#define 	EN_ERROR10   	EscRTBuff[79]

#define         TandemRunEnable                 EscRTBuff[90]
#define         TandemMessageRunAllowed         EscRTBuff[91]
#define         Tandemoutput                    EscRTBuff[92]

typedef struct motorspeeditem 
{    
    /* Measure 1s pulse time counter */
    u8 Tms_counter;      
    
    /* motor speed real time pulse */
    volatile u16 rt_pulse;  
    
    /* record 100ms motor speed pulse count */
    u16 pulseArray[20]; 					                  
    
    /* Escalator speed */
    u16 *const ptFreqBuff;   
    
    /* record motor speed error code */
    u8 *const pcErrorCodeBuff;   
    
    /* motor speed overspeed pulse not ok counter */
    u8 NotOkCounter;
    
    /* motor speed overspeed pulse ok counter */
    u8 OkCounter;
    
    /* motor between pulse time */
    u32 TimerMotorSpeedBetweenPulse[10];
    
    /* motor between pulse counter */
    u8 between_pulse_counter;
    
    /* Sensor use timer init */
    void (*Timer_Init)(u16 p1,u16 p2);
    
    /* Sensor use timer number */
    TIM_TypeDef* TimerNum;
  
    /* Brake distance */
    u16 *const ptBrakeDistanceBuff;
    
    u16 BrakeCalTms;
    volatile u16 rt_brake_pulse;     
    
    u16 delay_no_pulse_tms;
    u16 last_brake_pulse;
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
    
    /* record handrail speed error code */
    u8 *const pcErrorCodeBuff;   
	
}HandrailSpeedItem;

typedef struct stepmissingitem 
{
    /* record missing step error code */
    u8 *const pcErrorCodeBuff;   
    
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


/* Exported functions ------------------------------------------------------- */

extern u16 SfBase_EscState;
extern u32 g_u32InitTestError;
extern u16 g_u16ParameterLoadingError;
extern MotorSpeedItem MTRITEM[2];
extern u8 EscRTBuff[200];
extern u8 McRxBuff[1000];
extern HandrailSpeedItem HDL_Right;
extern HandrailSpeedItem HDL_Left;
extern STEPMISSINGITEM STPMS_UPPER;
extern STEPMISSINGITEM STPMS_LOWER;
extern u8 Modbuff[3000];
extern u8 *const Sys_Data;
extern u16 *const pt_SysBuff;
extern u8 *const pcOMC_EscRTBuff;
extern u8 sf_wdt_check_en;
extern u16 pcOMC_SfBase_EscState;
extern u8 *const pcErrorBuff;
extern u8 *const pcEscErrorBuff;
extern u8 *const pcOmcErrorBuff;
extern u8 SAFETY_SWITCH_STATUS[4];
extern u8 *const pcSafetyInputToControl;
extern u8 *const pcEscDataToControl;
extern u8 *const pcEscDataFromControl;
extern u8 g_u8ResetButton;
extern u16 *const g_u16ptCpu2MotorFreqSensor1;
extern u16 *const g_u16ptCpu2MotorFreqSensor2;

#define PARA_INIT                       *(u16*)&Modbuff[1102]
/* parameters */
#define MOTOR_RPM                        *(u16*)&Modbuff[1120]
#define MOTOR_PLUSE_PER_REV              *(u16*)&Modbuff[1122]
#define UNDERSPEED_TIME                 *(u16*)&Modbuff[1124]
#define HANDRAIL_MOTOR_PULSE            *(u16*)&Modbuff[1126]
#define NOMINAL_SPEED                   *(u16*)&Modbuff[1128]
#define SSM_SHORTCIRCUIT_TIME            *(u16*)&Modbuff[1130]
#define HR_FAULT_TIME                   *(u16*)&Modbuff[1132]
#define ROLLER_HR_RADIUS                *(u16*)&Modbuff[1134]
#define HR_PULSES_PER_REV                *(u16*)&Modbuff[1136]
#define STEP_WIDTH                      *(u16*)&Modbuff[1138]
#define TANDEM_TYPE                     *(u16*)&Modbuff[1140]
#define KEY_MINIMUM_TIME                *(u16*)&Modbuff[1142]
#define UP_DOWN_ALLOWED                 *(u16*)&Modbuff[1144]
#define RESET_MINIMUM_TIME                 *(u16*)&Modbuff[1146]

#endif  /* __ESC_H */


/******************************  END OF FILE  *********************************/








