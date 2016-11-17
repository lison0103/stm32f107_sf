/*******************************************************************************
* File Name          : esc_cmd_state.h
* Author             : lison
* Version            : V1.0
* Date               : 05/31/2016
* Description        : This file contains prototype of esc command and state.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_CMD_STATE_H
#define __ESC_CMD_STATE_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Esc_Safety_Input_Check(void);
void CheckUpDown_Key(UpDownKeyItem *ptKEY);
void Inspection_UpDown_Button(void);
void CheckReset(void);


#endif  /* __ESC_CMD_STATE_H */


/******************************  END OF FILE  *********************************/



