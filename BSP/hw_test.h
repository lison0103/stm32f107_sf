/*******************************************************************************
* File Name          : hw_test.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains prototype of some test function.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_TEST_H
#define __HW_TEST_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "bsp_iocfg.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Input_Check(void);
void Input_Check2(void);
void CrossCommCPUCheck(void);
void SafetyExtWdt_StartUpCheck(void);
void SafetyExtWdt_RunCheck(void);
void SafetyRelayAuxRelayTest(void);
void SafetyCTR_Check(void);
#ifdef GEC_SF_MASTER 
void CAN_Comm(void);
#endif
void CPU_Comm(void);
void CPU_Data_Check(void);
void CPU_Exchange_Data(void);

	
extern u8 data_error;
extern u8 onetime;
extern u8 switch_flag;
extern u8 sfwdt_checkflag;
#ifdef GEC_SF_MASTER
extern u8 R_SF_RL2_FB_CPU1;
#else
extern u8 R_SF_RL1_FB_CPU2;
#endif



#endif  /* __HW_TEST_H */


/******************************  END OF FILE  *********************************/



