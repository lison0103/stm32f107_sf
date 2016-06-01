/*******************************************************************************
* File Name          : esc_missing_step.h
* Author             : lison
* Version            : V1.0
* Date               : 05/10/2016
* Description        : This file contains prototype of esc missing step check.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_MISSING_STEP_H
#define __ESC_MISSING_STEP_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Missing_Step_UpperLower_Shortcircuit_Run(void);
u16 Pulse_counter_sensor_speed(STEPMISSINGITEM* psSTPMS);
void Missing_StepRun(STEPMISSINGITEM* psSTPMS);
void Missing_Step_UpperLower_SyncRun(void);
void ESC_Missingstep_Check(void);

#endif  /* __ESC_MISSING_STEP_H */


/******************************  END OF FILE  *********************************/



