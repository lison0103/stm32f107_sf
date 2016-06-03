/*******************************************************************************
* File Name          : esc_motor_speed.h
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Description        : This file contains prototype of esc motor speed.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_MOTOR_SPEED_H
#define __ESC_MOTOR_SPEED_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define F1      ( ( MOTOR_RPM * MOTOR_PLUSE_PER_REV ) / 60 )

/* Exported functions ------------------------------------------------------- */
void Motor_Speed_Ready(MTRFREQITEM* ptMTR);
void Motor_Speed_Run_EN115(MTRFREQITEM* ptMTR);
void Check_Stopping_Distance(MTRFREQITEM* ptMTR);
void Motor_Speed_1_2_Shortcircuit_Run(void);
void ESC_Motor_Check(void);
void sfEscStateCheck(void);

#endif  /* __ESC_MOTOR_SPEED_H */


/******************************  END OF FILE  *********************************/


