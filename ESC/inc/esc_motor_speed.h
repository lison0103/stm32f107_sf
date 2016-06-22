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

/* motor speed */
#define MAX_SPEED       ( ( F1 * 115 ) / 100 )
#define MIN_SPEED       ( ( F1 * 10 ) / 100 )

/* stopping distance */
#define CONVERSION      ( ( NOMINAL_SPEED / MOTOR_RPM ) * MOTOR_PLUSE_PER_REV )
#define MAX_DISTANCE    ( ( 12 * 2 * F1 ) / 10 )

/* Exported functions ------------------------------------------------------- */
void Motor_Speed_Ready(MTRFREQITEM* ptMTR);
void Motor_Speed_Run_EN115(MTRFREQITEM* ptMTR);
void Check_Stopping_Distance(MTRFREQITEM* ptMTR);
void Motor_Speed_1_2_Shortcircuit_Run(void);
void ESC_Motor_Check(void);


#endif  /* __ESC_MOTOR_SPEED_H */


/******************************  END OF FILE  *********************************/



