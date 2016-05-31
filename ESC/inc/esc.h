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

typedef struct mtrfreqitem 
{    
    u8 Tms_counter;   
    u8 MtrSpeedHighCnt;   
    u8 MtrSpeedHigh115Cnt;  
    
    u8 MtrSpeedLow50Cnt;
    
    volatile u16 rt_pulse;  			//实时飞轮频率计数   
    u16 pulseArray[20]; 					//脉冲周期数组       
    
    u16 BrakeCalTms;
    volatile u16 rt_brake_pulse;  //制动距离计数             
    
    
    u16 *const ptFreqBuff;  					//频率值               
    u16 *const ptBrakeDistanceBuff;	  //频率值   
    
    u16 PulseCalCounter;
    u16 PulseCalArray[12];
    
    volatile u16 SensorPulse;
    
    u16 delay_no_pulse_tms;
    u16 last_brake_pulse;
    u16 rt_brake_stop ;
    
}MTRFREQITEM;

typedef struct hdlitem 
{
    u8 Tms_counter;   
    volatile u16 hr_pulse;  			
    u16 pulseArray[20]; 					
    
    u16 HR_Fault_timer;
    u16 delay_no_pulse_tms;
    
    u16 *const ptHDLDataBuff;
	
}HDLITEM;

typedef struct stepmissingitem 
{
  u8 *const pcErrorCodeBuff;  
  u8 ErrorCodeMask;  
  
  u8 *const pcInputPort;
  u8 InputPortMask;
  
  
  u8 pin_level_high_cnt;
  u8 pin_level_low_cnt;
  u8 pin_state;
  
  u16 high_pulse_buff;
  u16 low_pulse_buff;
  
  volatile u16 MtrPulse;
  
	u16 sensor_error_cnt;
	u16 sensor_error_tms;	
        
        u8 rising_edge_detected[2];
        u16 *const ptStepMtrBuff;
        
}STEPMISSINGITEM;

/* Exported functions ------------------------------------------------------- */

extern u16 SfBase_EscState;
extern u16 escState_old;
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








