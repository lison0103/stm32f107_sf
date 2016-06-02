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
        
}STEPMISSINGITEM;


/* Exported functions ------------------------------------------------------- */

extern u16 SfBase_EscState;
extern MTRFREQITEM MTRITEM[2];
extern u8 EscRTBuff[200];
extern u16 DELAY_NO_PULSE_CHECKING;
extern HDLITEM HDL_Right;
extern HDLITEM HDL_Left;
extern u16 NOMINAL_SPEED;
extern u16 MOTOR_RPM;
extern u16 MOTOR_PLUSE_PER_REV;
extern u16 SSM_SHORTCIRCUIT_TIME;
extern STEPMISSINGITEM STPMS_UPPER;
extern STEPMISSINGITEM STPMS_LOWER;

#endif  /* __ESC_H */


/******************************  END OF FILE  *********************************/








