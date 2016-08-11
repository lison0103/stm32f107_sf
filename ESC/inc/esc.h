/*******************************************************************************
* File Name          : esc.h
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Description        : This file contains esc parameters.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_H
#define __ESC_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* ESC STATE */
#define ESC_STATE_STOP        ((u16)0x0001)
#define ESC_STATE_INSP        ((u16)0x0002)
#define ESC_STATE_RUNNING     ((u16)0x0004)
#define ESC_STATE_FAST        ((u16)0x0008)
#define ESC_STATE_SLOW        ((u16)0x0010)
#define ESC_STATE_INTT        ((u16)0x0020)
#define ESC_STATE_SPEEDUP     ((u16)0x0040)
#define ESC_STATE_RUN5S       ((u16)0x0080)
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

#define INPUT_PORT1_MASK        ( 1 << 0 )
#define INPUT_PORT2_MASK        ( 1 << 1 )
#define INPUT_PORT3_MASK        ( 1 << 2 )
#define INPUT_PORT4_MASK        ( 1 << 3 )
#define INPUT_PORT5_MASK        ( 1 << 4 )
#define INPUT_PORT6_MASK        ( 1 << 5 )
#define INPUT_PORT7_MASK        ( 1 << 6 )
#define INPUT_PORT8_MASK        ( 1 << 7 )

#define INPUT_PORT9_MASK        ( 1 << 0 )
#define INPUT_PORT10_MASK        ( 1 << 1 )
#define INPUT_PORT11_MASK        ( 1 << 2 )
#define INPUT_PORT12_MASK        ( 1 << 3 )
#define INPUT_PORT13_MASK        ( 1 << 4 )
#define INPUT_PORT14_MASK        ( 1 << 5 )
#define INPUT_PORT15_MASK        ( 1 << 6 )
#define INPUT_PORT16_MASK        ( 1 << 7 )

#define INPUT_PORT17_MASK        ( 1 << 0 )
#define INPUT_PORT18_MASK        ( 1 << 1 )
#define INPUT_PORT19_MASK        ( 1 << 2 )
#define INPUT_PORT20_MASK        ( 1 << 3 )
#define INPUT_PORT21_MASK        ( 1 << 4 )
#define INPUT_PORT22_MASK        ( 1 << 5 )
#define INPUT_PORT23_MASK        ( 1 << 6 )
#define INPUT_PORT24_MASK        ( 1 << 7 )

#define INPUT_PORT25_MASK        ( 1 << 0 )
#define INPUT_PORT26_MASK        ( 1 << 1 )
#define INPUT_PORT27_MASK        ( 1 << 2 )
#define INPUT_PORT28_MASK        ( 1 << 3 )
#define INPUT_PORT29_MASK        ( 1 << 4 )

#define INPUT_PORT_EX1_MASK        ( 1 << 0 )
#define INPUT_PORT_EX2_MASK        ( 1 << 1 )
#define INPUT_PORT_EX3_MASK        ( 1 << 2 )
#define INPUT_PORT_EX4_MASK        ( 1 << 3 )
#define INPUT_PORT_EX5_MASK        ( 1 << 4 )
#define INPUT_PORT_EX6_MASK        ( 1 << 5 )
#define INPUT_PORT_EX7_MASK        ( 1 << 6 )
#define INPUT_PORT_EX8_MASK        ( 1 << 7 )

#define INPUT_PORT_EX9_MASK        ( 1 << 0 )
#define INPUT_PORT_EX10_MASK        ( 1 << 1 )
#define INPUT_PORT_EX11_MASK        ( 1 << 2 )
#define INPUT_PORT_EX12_MASK        ( 1 << 3 )
#define INPUT_PORT_EX13_MASK        ( 1 << 4 )
#define INPUT_PORT_EX14_MASK        ( 1 << 5 )
#define INPUT_PORT_EX15_MASK        ( 1 << 6 )
#define INPUT_PORT_EX16_MASK        ( 1 << 7 )

#define INPUT_PORT_EX17_MASK        ( 1 << 0 )

#define INPUT_PORT_SF_RL_DRV_FB_MASK        ( 1 << 0 )
#define INPUT_PORT_SF_PWR_FB_CPU_MASK        ( 1 << 1 )
#define INPUT_PORT_SF_RL_FB_MASK        ( 1 << 2 )
#define INPUT_PORT_SF_RL_FB_CPU_MASK        ( 1 << 3 )
#define INPUT_PORT_AUX_FB_MASK        ( 1 << 4 )
#define INPUT_PORT_PLUSE_OUTPUT_FB_MASK        ( 1 << 5 )

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

#define CMD_OMC_FLAG1 	pcOMC_EscRTBuff[20]
#define CMD_OMC_FLAG2 	pcOMC_EscRTBuff[21]
#define CMD_OMC_FLAG3 	pcOMC_EscRTBuff[22]
#define CMD_OMC_FLAG4 	pcOMC_EscRTBuff[23]
#define CMD_OMC_FLAG5 	pcOMC_EscRTBuff[24]
#define CMD_OMC_FLAG6 	pcOMC_EscRTBuff[25]
#define CMD_OMC_FLAG7 	pcOMC_EscRTBuff[26]
#define CMD_OMC_FLAG8 	pcOMC_EscRTBuff[27]
#define CMD_OMC_FLAG9 	pcOMC_EscRTBuff[28]
#define CMD_OMC_FLAG10 	pcOMC_EscRTBuff[29]

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

typedef struct mtrfreqitem 
{    
    u8 Tms_counter;      
    u8 MtrSpeedHigh115Cnt;  
    
    volatile u16 rt_pulse;  			
    u16 pulseArray[20]; 					      
    
    u16 BrakeCalTms;
    volatile u16 rt_brake_pulse;             
    
    
    u16 *const ptFreqBuff;  					              
    u16 *const ptBrakeDistanceBuff;	    
    
    u16 PulseCalCounter;
    u16 PulseCalArray[12];
    
    volatile u16 SensorPulse;
    
    u16 delay_no_pulse_tms;
    u16 last_brake_pulse;
    u16 rt_brake_stop ;
    
    u8 *const pcErrorCodeBuff;   
    
}MTRFREQITEM;

typedef struct hdlitem 
{
    u8 Tms_counter;   
    volatile u16 hr_pulse;  			
    u16 pulseArray[20]; 					
    
    u16 HR_Fault_timer;
    u16 delay_no_pulse_tms;
    
    u16 *const ptHDLDataBuff;
    
    u8 *const pcErrorCodeBuff;   
	
}HDLITEM;

typedef struct stepmissingitem 
{
    u8 *const pcErrorCodeBuff;   
    
    volatile u16 MtrPulse;
    
    u16 sensor_error_cnt;
    u16 sensor_error_tms;	
    
    u8 rising_edge_detected[2];
    u16 *const ptStepMtrBuff;
    u16 ms_ready_delay;
    
    u32 Motor_speed_pulse_counter_init;
    u32 Motor_speed_pulse_counter;
    u8 First_entry_missing_step_detection;
        
}STEPMISSINGITEM;


/* Exported functions ------------------------------------------------------- */

extern u16 SfBase_EscState;
extern MTRFREQITEM MTRITEM[2];
extern u8 EscRTBuff[200];
extern u8 McRxBuff[1000];
extern HDLITEM HDL_Right;
extern HDLITEM HDL_Left;
extern STEPMISSINGITEM STPMS_UPPER;
extern STEPMISSINGITEM STPMS_LOWER;
extern u8 TandemRunEnable;
extern u8 TandemMessageRunAllowed;
extern u8 Tandemoutput;
extern u8 Modbuff[3000];
extern u8 *const Sys_Data;
extern u16 *const pt_SysBuff;
extern u8 *const pcOMC_EscRTBuff;
extern u8 sf_wdt_check_en;
extern u16 pcOMC_SfBase_EscState;
extern u8 *const pcErrorBuff;
extern u8 *const pcEscErrorCodeBuff;
extern u8 *const pcEscErrorBuff;
extern u8 *const pcOmcErrorBuff;
extern u8 SAFETY_SWITCH_STATUS[4];
extern u8 *const pcEscSafetySwitchStatus;
extern u8 *const pcSafetyInputToControl;
extern u8 *const pcEscDataToControl;
extern u8 *const pcEscDataFromControl;

#define PARA_INIT                       *(u16*)&Modbuff[1102]
/* parameters */
#define MOTOR_RPM                        *(u16*)&Modbuff[1120]
#define MOTOR_PLUSE_PER_REV              *(u16*)&Modbuff[1122]
#define UNDERSPEED_TIME                 *(u16*)&Modbuff[1124]
#define DELAY_NO_PULSE_CHECKING          *(u16*)&Modbuff[1126]
#define NOMINAL_SPEED                   *(u16*)&Modbuff[1128]
#define SSM_SHORTCIRCUIT_TIME            *(u16*)&Modbuff[1130]
#define HR_FAULT_TIME                   *(u16*)&Modbuff[1132]
#define ROLLER_HR_RADIUS                *(u16*)&Modbuff[1134]
#define HR_PULSES_PER_REV                *(u16*)&Modbuff[1136]
#define STEP_WIDTH                      *(u16*)&Modbuff[1138]
#define TANDEM_TYPE                     *(u16*)&Modbuff[1140]



#endif  /* __ESC_H */


/******************************  END OF FILE  *********************************/








