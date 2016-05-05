/*******************************************************************************
* File Name          : led.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : The file contains prototype of led functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LED_H
#define _LED_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** LED Port **/
#ifdef GEC_SF_MASTER
#define LED PAout(2)
#else
#define LED PAout(1)
#endif
/* Exported functions ------------------------------------------------------- */
void LED_Init(void); 
 

#endif  /* _LED_H */


/******************************  END OF FILE  *********************************/


