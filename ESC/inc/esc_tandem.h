/*******************************************************************************
* File Name          : esc_tandem.h
* Author             : lison
* Version            : V1.0
* Date               : 05/31/2016
* Description        : This file contains prototype of esc tandem.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_TANDEM_H
#define __ESC_TANDEM_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
static void CheckTandemReady(void);
static void CheckTandemRun(void);
static void TandemOutput(void);
void ESC_Tandem_Check(void);


#endif  /* __ESC_TANDEM_H */


/******************************  END OF FILE  *********************************/



