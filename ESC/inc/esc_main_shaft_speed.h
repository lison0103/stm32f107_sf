/*******************************************************************************
* File Name          : esc_main_shaft_speed.h
* Author             : lison
* Version            : V1.0
* Date               : 12/20/2016
* Description        : This file contains prototype of esc main shaft speed.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_MAIN_SHAFT_SPEED_H
#define __ESC_MAIN_SHAFT_SPEED_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define F_Main_Shaft   ( ( MAIN_SHAFT_RPM * MAIN_SHAFT_PULSE_PER_REV ) / 60u )

/* main shaft speed */
#define MAX_SPEED_MAINSHAFT   ( ( F_Main_Shaft * 115u ) / 100u )
#define MIN_SPEED_MAINSHAFT   ( ( F_Main_Shaft * 10u ) / 100u )

/* stopping distance */
#define CONVERSION_MAINSHAFTSPEED       ( ( (u16)NOMINAL_SPEED * 10u * 60u ) / MAIN_SHAFT_RPM / MAIN_SHAFT_PULSE_PER_REV )  /* [mm/pulse] */


/* Exported functions ------------------------------------------------------- */
void ESC_Mainshaft_Check(void);
void MainShaftWrongDirection(MotorSpeedItem* ptMTR);
void MainShaft_Speed_Shortcircuit_Run(void);

extern u8 g_u8FirstMainSaftSpeedEdgeDetected;

#endif  /* __ESC_MAIN_SHAFT_SPEED_H */


/******************************  END OF FILE  *********************************/



