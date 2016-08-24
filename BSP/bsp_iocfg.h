/*******************************************************************************
* File Name          : bsp_iocfg.h
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains prototype of input and output pin Initializes.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_IOCFG_H
#define __BSP_IOCFG_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Input_Output_PinInit(void);


#ifdef GEC_SF_MASTER

/** OUTPUT **/
#define AUX_CTR_SET()    		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET)
#define AUX_CTR_CLR()    	GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET)
#define SF_RL_CTR_SET()    		GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET)
#define SF_RL_CTR_CLR()    	GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET)

#define SF_RELAY_ON()    		GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET)
#define SF_RELAY_OFF()    	GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET)
#define AUX_RELAY_ON()   		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET)
#define AUX_RELAY_OFF()    	GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET)

#define SF_RL_WDT_SET()   		GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET)
#define SF_RL_WDT_CLR()    	GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET)

#define PULSE_OUTPUT_SET()                 GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_SET)
#define PULSE_OUTPUT_CLR()       GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_RESET)

#define SINA_BUSY_OUT_SET()       GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET)
#define SINA_BUSY_OUT_CLR()       GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET)
#define SYNC_SYS_OUT_SET()       GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET)
#define SYNC_SYS_OUT_CLR()       GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET)

/** FEEDBACK **/
#define SF_RL_DRV_FB   GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)
#define SF_PWR_FB_CPU   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) 
#define SF_RL_FB       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)

#define SF_RL2_FB_CPU1       GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6)

#define AUX_FB       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)

#define PULSE_OUTPUT_FB       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)

#define SINA_BUSY_IN       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14)
#define SYNC_SYS_IN       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15)

/** SAFETY SWITCH INPUT**/
#define SAFETY_SWITCH_INPUT_1       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)
#define SAFETY_SWITCH_INPUT_2       GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)
#define SAFETY_SWITCH_INPUT_3       GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)
#define SAFETY_SWITCH_INPUT_4       GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10)


/* EWDT */
#define SF_EWDT_TOOGLE() 	GPIOB->ODR ^= 0X0200u


#define SW_SPDT_KEY       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)

#else


/* OUTPUT */
#define AUX_CTR_SET()    		GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_SET)
#define AUX_CTR_CLR()    	GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_RESET)
#define SF_RL_CTR_SET()    		GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_SET)
#define SF_RL_CTR_CLR()    	GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_RESET)

#define SF_RELAY_ON()    		GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_SET)
#define SF_RELAY_OFF()    	GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_RESET)
#define AUX_RELAY_ON()   		GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_SET)
#define AUX_RELAY_OFF()    	GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_RESET)

#define SF_RL_WDT_SET()   		GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_SET)
#define SF_RL_WDT_CLR()    	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_RESET)

#define PULSE_OUTPUT_SET()                 GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET)
#define PULSE_OUTPUT_CLR()       GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET)

#define SINA_BUSY_OUT_SET()       GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_SET)
#define SINA_BUSY_OUT_CLR()       GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET)
#define SYNC_SYS_OUT_SET()       GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_SET)
#define SYNC_SYS_OUT_CLR()       GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_RESET)

/* FEEDBACK */
#define SF_RL_DRV_FB 		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)
#define SF_PWR_FB_CPU   GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)
#define SF_RL_FB       GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6)

#define SF_RL1_FB_CPU2       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)

#define AUX_FB       GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)

#define PULSE_OUTPUT_FB       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)

#define SINA_BUSY_IN       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)
#define SYNC_SYS_IN       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)

/** SAFETY SWITCH INPUT**/
#define SAFETY_SWITCH_INPUT_1       GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3)
#define SAFETY_SWITCH_INPUT_2       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)
#define SAFETY_SWITCH_INPUT_3       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)
#define SAFETY_SWITCH_INPUT_4       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)

/* EWDT */
#define SF_EWDT_TOOGLE() 	GPIOE->ODR ^= 0X0002u

#endif  /* GEC_SF_MASTER */


#endif /* __BSP_IOCFG_H */


/******************************  END OF FILE  *********************************/



