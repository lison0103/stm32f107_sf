/*******************************************************************************
* File Name          : esc_parameter_process.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Last modify date   : 11/22/2016
* Description        : This file contains prototype of esc parameter process.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_PARAMETER_PROCESS_H
#define __ESC_PARAMETER_PROCESS_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/

#pragma pack(1)
typedef struct sfparameter
{
    u8 p_MANUFACTURING_ORDER[20];
    u8 p_CODE;
    u8 p_NOMINAL_SPEED;
    u16 p_MOTOR_RPM;
    u8 p_MOTOR_PULSE_PER_REV;
    u16 p_MAIN_SHAFT_RPM;
    u16 p_MAIN_SHAFT_PULSE_PER_REV;
    u8 p_ROLLER_HR_RADIUS;
    u8 p_HR_PULSES_PER_REV;
    u8 p_HR_FAULT_TIME;
    u16 p_STEP_WIDTH; 
    u16 p_DELAY_NO_PULSE_CHECKING;
    u8 p_SPEED_SENSOR_INSTALLATION;
    u16 p_UNDERSPEED_TIME;
    u8 p_END_SAFETY_STRING_FAULT_TYPE;
    u16 p_CONTACTOR_FEEDBACK_FILTER;
    u8 p_CONTACTORS_TIMEOUT;
    u8 p_CONTACTOR_FB1_MASK;
    u8 p_CONTACTOR_FB2_MASK;
    u8 p_CONTACTOR_FB3_MASK;
    u8 p_CONTACTOR_FB4_MASK;
    u8 p_CONTACTOR_FB5_MASK;
    u8 p_CONTACTOR_FB6_MASK;    
    u8 p_CONTACTOR_FB7_MASK;
    u8 p_CONTACTOR_FB8_MASK;    
    u16 p_KEY_MINIMUM_TIME;
    u8 p_UP_DOWN_ALLOWED;
    u8 p_AUTCONT_OPTIONS;
    u8 p_DIAGNOSTIC_BOARD_L2_QUANTITY;
    u8 p_TANDEM;
    u8 p_INSPECTION_CATCH_THE_MOTOR;
    u8 p_RESET_FROM_INSPECTION_CONTROL;
    u8 p_AUX_BRAKE_SUPERVISION_TIME;
    u8 p_AUX_BRAKE_ENABLE;
    u8 p_CAPACITOR_TIME_MEASUREMENT;
    u8 p_RADAR_TYPE;
    u8 p_LIGHT_BARRIER_COMBS_AREA_TYPE;
    u8 p_LIGHT_BARRIER_ENTRY_TYPE;
    u16 p_TIME_TRANSPORTATION;
    u16 p_TIME_DIRECTION_INDICATION;
    u16 p_TIME_REVERSING;
    u8 p_SAFETY_CURTAIN_LONG_TIME;
    u16 p_PULSE_SIGNALS_MINIMUM_LAG;
    u16 p_DRIVE_CHAIN_DELAY;    
    u8 p_SPARE_PARAMETER_1;
    u8 p_SPARE_PARAMETER_2;
    u8 p_SPARE_PARAMETER_3;
    u8 p_SPARE_PARAMETER_4;
    u16 p_SPARE_PARAMETER_5;
    u16 p_SPARE_PARAMETER_6;
    u16 p_SPARE_PARAMETER_7;
    u16 p_SPARE_PARAMETER_8;
    u16 p_SPARE_PARAMETER_9;
    u16 p_SPARE_PARAMETER_10;
    u8 p_SAFETY_INPUT_CONFIGURABLE_1_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_2_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_3_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_4_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_5_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_6_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_7_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_8_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_9_TYPE;
    u8 p_SAFETY_INPUT_CONFIGURABLE_10_TYPE;
    
    u16 IO_PARAMETER[200];
    
    
    u16 CRC32;
    
}SFPara;
#pragma pack() 


#pragma pack(1)
typedef struct cb_para_in_sf
{  
    u16 p_TIME_LOW_SPEED;
    u16 p_ADDITIONAL_DIRECTION_INDICATION_TIME;
    u16 p_ADDITIONAL_TRANSPORTATION_TIME;
    u16 p_ADDITIONAL_REVERSING_TIME;        
    u8 p_DIAGNOSTIC_BOARD_L1_QUANTITY;
    u8 p_UPPER_DIAG_SS_LENGTH;
    u8 p_LOWER_DIAG_SS_LENGTH;
    u8 p_INTERM1_DIAG_SS_LENGTH;
    u8 p_INTERM2_DIAG_SS_LENGTH;
    u8 p_DIAG3_ENABLE;   
    u8 p_SPARE_PARAMETER_11;
    u8 p_SPARE_PARAMETER_12;
    u16 p_SPARE_PARAMETER_16;
    u16 p_SPARE_PARAMETER_17;
    
}CBParaInSF;
#pragma pack() 

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define MESSAGE_TO_CPU      0x11u
#define MESSAGE_TO_CONTROL      0x22u
#define USB_DETECTED        0x01u
#define USB_NOT_DETECTED        0x02u
#define SAFETY_PARAMETER_LOADED        0x10u
#define CONTROL_PARAMETER_LOADED        0x20u
#define PARAMETER_ERROR        0x03u
#define PARAMETER_CORRECT        0x04u
#define SAFETY_PARAMETER_NOT_EXIST        0x05u
#define CONTROL_PARAMETER_NOT_EXIST        0x06u
#define SAFETY_PARAMETER_EXIST        0x07u
#define CONTROL_PARAMETER_EXIST        0x08u
#define SEND_PARAMETER        0x09u
#define RECEIVE_PARAMETER        0x0Au
#define PARAMETER_LOADED_FINSH        0x0Bu



/* parameters safety*/
#define MANUFACTURING_ORDER             SFParameterData.p_MANUFACTURING_ORDER
#define CODE                            SFParameterData.p_MOTOR_RPM
#define NOMINAL_SPEED                   SFParameterData.p_NOMINAL_SPEED
#define MOTOR_RPM                       SFParameterData.p_MOTOR_RPM
#define MOTOR_PULSE_PER_REV             SFParameterData.p_MOTOR_PULSE_PER_REV
#define MAIN_SHAFT_RPM                  SFParameterData.p_MAIN_SHAFT_RPM
#define MAIN_SHAFT_PULSE_PER_REV        SFParameterData.p_MAIN_SHAFT_PULSE_PER_REV
#define ROLLER_HR_RADIUS                SFParameterData.p_ROLLER_HR_RADIUS
#define HR_PULSES_PER_REV               SFParameterData.p_HR_PULSES_PER_REV
#define HR_FAULT_TIME                   SFParameterData.p_HR_FAULT_TIME
#define STEP_WIDTH                      SFParameterData.p_STEP_WIDTH
#define DELAY_NO_PULSE_CHECKING         SFParameterData.p_DELAY_NO_PULSE_CHECKING
#define SPEED_SENSOR_INSTALLATION       SFParameterData.p_SPEED_SENSOR_INSTALLATION
#define UNDERSPEED_TIME                 SFParameterData.p_UNDERSPEED_TIME
#define END_SAFETY_STRING_FAULT_TYPE    SFParameterData.p_END_SAFETY_STRING_FAULT_TYPE
#define CONTACTOR_FEEDBACK_FILTER       SFParameterData.p_CONTACTOR_FEEDBACK_FILTER
#define CONTACTORS_TIMEOUT              SFParameterData.p_CONTACTORS_TIMEOUT
#define CONTACTOR_FB1_MASK              SFParameterData.p_CONTACTOR_FB1_MASK
#define CONTACTOR_FB2_MASK              SFParameterData.p_CONTACTOR_FB2_MASK
#define CONTACTOR_FB3_MASK              SFParameterData.p_CONTACTOR_FB3_MASK
#define CONTACTOR_FB4_MASK              SFParameterData.p_CONTACTOR_FB4_MASK
#define CONTACTOR_FB5_MASK              SFParameterData.p_CONTACTOR_FB5_MASK
#define CONTACTOR_FB6_MASK              SFParameterData.p_CONTACTOR_FB6_MASK
#define CONTACTOR_FB7_MASK              SFParameterData.p_CONTACTOR_FB7_MASK
#define CONTACTOR_FB8_MASK              SFParameterData.p_CONTACTOR_FB8_MASK
#define KEY_MINIMUM_TIME                SFParameterData.p_KEY_MINIMUM_TIME
#define UP_DOWN_ALLOWED                 SFParameterData.p_UP_DOWN_ALLOWED
#define AUTCONT_OPTIONS                 SFParameterData.p_AUTCONT_OPTIONS
#define DIAGNOSTIC_BOARD_L2_QUANTITY    SFParameterData.p_DIAGNOSTIC_BOARD_L2_QUANTITY
#define TANDEM                          SFParameterData.p_TANDEM
#define INSPECTION_CATCH_THE_MOTOR      SFParameterData.p_INSPECTION_CATCH_THE_MOTOR
#define RESET_FROM_INSPECTION_CONTROL   SFParameterData.p_RESET_FROM_INSPECTION_CONTROL
#define AUX_BRAKE_SUPERVISION_TIME      SFParameterData.p_AUX_BRAKE_SUPERVISION_TIME
#define AUX_BRAKE_ENABLE                SFParameterData.p_AUX_BRAKE_ENABLE
#define CAPACITOR_TIME_MEASUREMENT      SFParameterData.p_CAPACITOR_TIME_MEASUREMENT
#define RADAR_TYPE                      SFParameterData.p_RADAR_TYPE
#define LIGHT_BARRIER_COMBS_AREA_TYPE   SFParameterData.p_LIGHT_BARRIER_COMBS_AREA_TYPE
#define LIGHT_BARRIER_ENTRY_TYPE        SFParameterData.p_LIGHT_BARRIER_ENTRY_TYPE
#define TIME_TRANSPORTATION             SFParameterData.p_TIME_TRANSPORTATION
#define TIME_DIRECTION_INDICATION       SFParameterData.p_TIME_DIRECTION_INDICATION
#define TIME_REVERSING                  SFParameterData.p_TIME_REVERSING
#define SAFETY_CURTAIN_LONG_TIME        SFParameterData.p_SAFETY_CURTAIN_LONG_TIME
#define PULSE_SIGNALS_MINIMUM_LAG       SFParameterData.p_PULSE_SIGNALS_MINIMUM_LAG
#define DRIVE_CHAIN_DELAY               SFParameterData.p_DRIVE_CHAIN_DELAY
#define SPARE_PARAMETER_1               SFParameterData.p_SPARE_PARAMETER_1
#define SPARE_PARAMETER_2               SFParameterData.p_SPARE_PARAMETER_2
#define SPARE_PARAMETER_3               SFParameterData.p_SPARE_PARAMETER_3
#define SPARE_PARAMETER_4               SFParameterData.p_SPARE_PARAMETER_4
#define SPARE_PARAMETER_5               SFParameterData.p_SPARE_PARAMETER_5
#define SPARE_PARAMETER_6               SFParameterData.p_SPARE_PARAMETER_6
#define SPARE_PARAMETER_7               SFParameterData.p_SPARE_PARAMETER_7
#define SPARE_PARAMETER_8               SFParameterData.p_SPARE_PARAMETER_8
#define SPARE_PARAMETER_9               SFParameterData.p_SPARE_PARAMETER_9
#define SPARE_PARAMETER_10              SFParameterData.p_SPARE_PARAMETER_10
#define SAFETY_INPUT_CONFIGURABLE_1_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_1_TYPE
#define SAFETY_INPUT_CONFIGURABLE_2_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_2_TYPE
#define SAFETY_INPUT_CONFIGURABLE_3_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_3_TYPE
#define SAFETY_INPUT_CONFIGURABLE_4_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_4_TYPE
#define SAFETY_INPUT_CONFIGURABLE_5_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_5_TYPE
#define SAFETY_INPUT_CONFIGURABLE_6_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_6_TYPE
#define SAFETY_INPUT_CONFIGURABLE_7_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_7_TYPE
#define SAFETY_INPUT_CONFIGURABLE_8_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_8_TYPE
#define SAFETY_INPUT_CONFIGURABLE_9_TYPE       SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_9_TYPE
#define SAFETY_INPUT_CONFIGURABLE_10_TYPE      SFParameterData.p_SAFETY_INPUT_CONFIGURABLE_10_TYPE



/* control parameters in safety*/
#define TIME_LOW_SPEED                           CBParameterInSafety.p_TIME_LOW_SPEED
#define ADDITIONAL_DIRECTION_INDICATION_TIME     CBParameterInSafety.p_ADDITIONAL_DIRECTION_INDICATION_TIME
#define ADDITIONAL_TRANSPORTATION_TIME           CBParameterInSafety.p_ADDITIONAL_TRANSPORTATION_TIME
#define ADDITIONAL_REVERSING_TIME                CBParameterInSafety.p_ADDITIONAL_REVERSING_TIME
#define DIAGNOSTIC_BOARD_L1_QUANTITY             CBParameterInSafety.p_DIAGNOSTIC_BOARD_L1_QUANTITY
#define UPPER_DIAG_SS_LENGTH                     CBParameterInSafety.p_UPPER_DIAG_SS_LENGTH
#define LOWER_DIAG_SS_LENGTH                     CBParameterInSafety.p_LOWER_DIAG_SS_LENGTH
#define INTERM1_DIAG_SS_LENGTH                   CBParameterInSafety.p_INTERM1_DIAG_SS_LENGTH
#define INTERM2_DIAG_SS_LENGTH                   CBParameterInSafety.p_INTERM2_DIAG_SS_LENGTH
#define DIAG3_ENABLE                             CBParameterInSafety.p_DIAG3_ENABLE
#define SPARE_PARAMETER_11                       CBParameterInSafety.p_SPARE_PARAMETER_11
#define SPARE_PARAMETER_12                       CBParameterInSafety.p_SPARE_PARAMETER_12
#define SPARE_PARAMETER_16                       CBParameterInSafety.p_SPARE_PARAMETER_16
#define SPARE_PARAMETER_17                       CBParameterInSafety.p_SPARE_PARAMETER_17



/* parameter range */
#define MOTOR_RPM_MIN                   500u
#define MOTOR_RPM_MAX                   2500u

#define NOMINAL_SPEED_MIN                20u
#define NOMINAL_SPEED_MAX                90u


#define MOTOR_PLUSE_PER_REV_MIN           1u
#define MOTOR_PLUSE_PER_REV_MAX           20u

#define STEP_WIDTH_MIN                  127u
#define STEP_WIDTH_MAX                  405u

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

#define ESC_SF_PARAMETER_DATA_LEN               sizeof(SFPara)
#define ESC_CB_PARAMETER_IN_SF_DATA_LEN         sizeof(CBParaInSF)

/* Exported functions ------------------------------------------------------- */
#ifdef GEC_SF_MASTER
int USB_LoadParameter(void);
#endif
void ParametersLoading(void);
static u16 Send_State_Message(u8 board, u8 state, u8 buff[], u16 len);

extern u8 ParaLoad;

#endif  /* __ESC_PARAMETER_PROCESS_H */


/******************************  END OF FILE  *********************************/



