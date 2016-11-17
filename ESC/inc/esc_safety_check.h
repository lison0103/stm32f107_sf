/*******************************************************************************
* File Name          : esc_safety_check.h
* Author             : lison
* Version            : V1.0
* Date               : 06/20/2016
* Description        : This file contains prototype of esc safety module check. 
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_SAFETY_CHECK_H
#define __ESC_SAFETY_CHECK_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SafetyExtWdt_StartUpCheck(void);
void SafetyExtWdt_RunCheck(void);
static void SafetyRelayAuxRelayTest(void);
static void SafetyCTR_Check(void);
void SafetyOutputDisable(void);
void SafetyOutputEnable(void);
void SafetySwitchStatus(void);
void Safety_Relay_Shortcircuit_Check(void);

#endif  /* __ESC_SAFETY_CHECK_H */


/******************************  END OF FILE  *********************************/



