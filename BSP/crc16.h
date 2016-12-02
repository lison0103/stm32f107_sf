/*******************************************************************************
* File Name          : crc16.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains the prototype of 16-bit CRC Calculation and verification.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRC16_H
#define __CRC16_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define PARAMETER_POLYNOMIALS      0xFA567D89u
#define MAIN_POLYNOMIALS           0xFA567D89u
#ifdef GEC_SF_MASTER
#define SAFETY_POLYNOMIALS         0x992C1A4Cu
#define SAFETY_POLYNOMIALSB        0x90022004u
#else
#define SAFETY_POLYNOMIALS         0x90022004u
#endif

/* Exported functions ------------------------------------------------------- */
uint16_t MB_CRC16( uint8_t pucFrame[], uint16_t usLen );
u32 MB_CRC32(u8 pucFrame[], u16 usLen, u32 Polynomials);
u32 STM_CRC32(uint32_t pBuffer[], uint32_t BufferLength);

#endif /* __CRC16_H */


/******************************  END OF FILE  *********************************/
