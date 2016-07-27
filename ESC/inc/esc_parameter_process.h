/*******************************************************************************
* File Name          : esc_parameter_process.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains prototype of esc parameter process.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_PARAMETER_PROCESS_H
#define __ESC_PARAMETER_PROCESS_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define MESSAGE_TO_CPU      0x11
#define MESSAGE_TO_CONTROL      0x22
#define USB_DETECTED        0x01
#define USB_NOT_DETECTED        0x02
#define PARAMETER_ERROR        0x04
#define PARAMETER_CORRECT        0x08

/* Exported functions ------------------------------------------------------- */
void ParametersLoading(void);


#endif  /* __ESC_PARAMETER_PROCESS_H */


/******************************  END OF FILE  *********************************/



