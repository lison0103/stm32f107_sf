/*******************************************************************************
* File Name          : esc_motor_speed.h
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Last modify date   : 11/22/2016
* Description        : This file contains prototype of esc motor speed and brake distance.
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
#define F1      ( ( MOTOR_RPM * MOTOR_PULSE_PER_REV ) / 60u )

/* motor speed */
#define MAX_SPEED       ( ( F1 * 118u ) / 100u )
#define MIN_SPEED       ( ( F1 * 10u ) / 100u )

/* stopping distance */
#define CONVERSION_MOTORSPEED           ( ( (u16)NOMINAL_SPEED * 10u * 60u ) / MOTOR_RPM / MOTOR_PULSE_PER_REV ) /* [mm/pulse] */
#define CONVERSION_MAINSHAFTSPEED       ( ( (u16)NOMINAL_SPEED * 10u * 60u ) / MAIN_SHAFT_RPM / MAIN_SHAFT_PULSE_PER_REV )  /* [mm/pulse] */

/*
If (NOMINAL_SPEED <= 75[cm/s]) then
	MAX_DISTANCE = 2[s]*NOMINAL_SPEED*10 [mm/s]*1,2;	//[mm]
*/
#define MAX_DISTANCE    ( 2u * (u16)NOMINAL_SPEED * 12u )  /* [mm] */
/*
Else_if (NOMINAL_SPEED <= 90[cm/s]) then
	MAX_DISTANCE = (4/3[s]*NOMINAL_SPEED*10 [mm/s] + 500[mm] )*1,2;
*/
#define MAX_DISTANCE2    (((( 4u / 3u ) * (u16)NOMINAL_SPEED * 10u + 500u ) * 12u ) / 10u )  /* [mm] */

/* Exported functions ------------------------------------------------------- */
void Motor_Speed_1_2_Shortcircuit_Run(void);
void ESC_Motor_Check(void);
void Measure_motor_between_pulse(MotorSpeedItem* ptMTR);

#endif  /* __ESC_MOTOR_SPEED_H */


/******************************  END OF FILE  *********************************/



