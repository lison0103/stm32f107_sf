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
#define INPUT_PORT24_28     EscRTBuff[7]
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

#define CF8_ESC_Driver_On           ((u8)0x01)
#define CF8_ESC_Driver_Feedback	    ((u8)0x02)
#define CF8_3	                      ((u8)0x04)
#define CF8_4	                      ((u8)0x08)
#define CF8_DY_State_Star	          ((u8)0x10)
#define CF8_DY_State_Delta	        ((u8)0x20)
#define CF8_Auto_Fast_Mode1	        ((u8)0x40)
#define CF8_Auto_Fast_Mode2	        ((u8)0x80)

#define MTR_MAX_SPEED       EscRTBuff[37]
#define MTR_MIN_SPEED       EscRTBuff[38]
#define SHORTCIRCUIT_ERROR  EscRTBuff[39]
#define MS_SYNC_RUN_ERROR   EscRTBuff[66]

#define         EN_ERROR1   	EscRTBuff[70]
#define  	EN_ERROR2   	EscRTBuff[71]
#define 	EN_ERROR3   	EscRTBuff[72]
#define 	EN_ERROR4   	EscRTBuff[73]
#define 	EN_ERROR5   	EscRTBuff[74]
#define 	EN_ERROR6   	EscRTBuff[75]
#define 	EN_ERROR7   	EscRTBuff[76]

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

/* parameters */
#define MOTOR_RPM                        pt_SysBuff[10]
#define MOTOR_PLUSE_PER_REV              pt_SysBuff[11]
#define UNDERSPEED_TIME                 pt_SysBuff[12]
#define DELAY_NO_PULSE_CHECKING          pt_SysBuff[13]
#define NOMINAL_SPEED                   pt_SysBuff[14]
#define SSM_SHORTCIRCUIT_TIME            pt_SysBuff[15]
#define HR_FAULT_TIME                   pt_SysBuff[16]
#define ROLLER_HR_RADIUS                pt_SysBuff[17]
#define HR_PULSES_PER_REV                pt_SysBuff[18]
#define STEP_WIDTH                      pt_SysBuff[19]
#define TANDEM_TYPE                     pt_SysBuff[20]



#endif  /* __ESC_H */


/******************************  END OF FILE  *********************************/








