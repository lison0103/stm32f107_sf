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
//#define EXTERNAL_INPUT_TEST

/* Exported functions ------------------------------------------------------- */
void Input_Output_PinInit(void);


#ifdef GEC_SF_MASTER

//INPUT
#define IN1     PEin(1)
#define IN2     PEin(0)
#define IN3     PBin(7)
#define IN4     PBin(6)
#define IN5     PBin(5)
#define IN6     PBin(4)
#define IN7     PBin(3)
#define IN8     PDin(7)
#define IN9     PDin(6)
#define IN10     PDin(5)
#define IN11     PDin(4)
#define IN12     PDin(3)
#define IN13     PDin(2)
#define IN14     PCin(12)
#define IN15     PCin(11)
#define IN16     PCin(10)
#define IN17     PAin(15)
#define IN18     PAin(9)
#define IN19     PAin(8)
#define IN20     PCin(9)
#define IN21     PCin(8)
#define IN22     PCin(7)
#define IN23     PEin(9)
#define IN24     PEin(8)
#define IN25     PEin(7)
#define IN26     PBin(2)
#define IN27     PBin(1)
#define IN28     PBin(0)

#define EX_IN1     PCin(6)
#define EX_IN2     PDin(15)
#define EX_IN3     PDin(14)
#define EX_IN4     PDin(13)
#define EX_IN5     PDin(12)
#define EX_IN6     PDin(11)
#define EX_IN7     PDin(10)
#define EX_IN8     PDin(9)
#define EX_IN9     PDin(8)
#define EX_IN10     PBin(15)
#define EX_IN11     PBin(14)
#define EX_IN12     PEin(15)
#define EX_IN13     PEin(14)
#define EX_IN14     PEin(13)
#define EX_IN15     PEin(12)
#define EX_IN16     PEin(11)
#define EX_IN17     PEin(10)

//OUTPUT
#define AUX_CTR     PCout(0)
#define SF_RL_CTR     PEout(2)

#define SF_RELAY_ON()    		GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET)
#define SF_RELAY_OFF()    	GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET)
#define AUX_RELAY_ON()   		GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET)
#define AUX_RELAY_OFF()    	GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET)

#define SF_RL_WDT     PBout(9)

//FEEDBACK
#define SF_RL_DRV_FB   PEin(3)
#define SF_PWR_FB_CPU   PBin(8)
#define SF_RL_FB       PCin(2)

#define SF_RL2_FB_CPU1       PEin(6)

#define AUX_FB       PCin(1)

//PLUSE
#define PLUSE_OUT       PCout(5)

//EWDT
#define SF_EWDT_TOOGLE() 	GPIOB->ODR ^= 0X0200


#else

//INPUT
#define IN1     PDin(4)
#define IN2     PDin(3)
#define IN3     PDin(2)
#define IN4     PDin(1)
#define IN5     PDin(0)
#define IN6     PCin(12)
#define IN7     PCin(11)
#define IN8     PCin(10)
#define IN9     PAin(15)
#define IN10     PAin(10)
#define IN11     PAin(9)
#define IN12     PAin(8)
#define IN13     PCin(9)
#define IN14     PCin(8)
#define IN15     PCin(7)
#define IN16     PCin(6)
#define IN17     PDin(15)
#define IN18     PDin(14)
#define IN19     PDin(13)
#define IN20     PDin(12)
#define IN21     PDin(11)
#define IN22     PDin(10)
#define IN23     PDin(9)
#define IN24     PDin(8)
#define IN25     PBin(15)
#define IN26     PBin(14)
#define IN27     PBin(13)
#define IN28     PBin(12)

#define EX_IN1     PBin(11)
#define EX_IN2     PBin(10)
#define EX_IN3     PEin(15)
#define EX_IN4     PEin(14)
#define EX_IN5     PEin(13)
#define EX_IN6     PEin(12)
#define EX_IN7     PEin(11)
#define EX_IN8     PEin(10)
#define EX_IN9     PEin(9)
#define EX_IN10     PEin(8)
#define EX_IN11     PEin(7)
#define EX_IN12     PBin(2)
#define EX_IN13     PBin(1)
#define EX_IN14     PBin(0)
#define EX_IN15     PCin(5)
#define EX_IN16     PCin(4)
#define EX_IN17     PAin(4)

//OUTPUT
#define AUX_CTR     PEout(4)
#define SF_RL_CTR     PEout(2)

#define SF_RELAY_ON()    		GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET)
#define SF_RELAY_OFF()    	GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET)
#define AUX_RELAY_ON()   		GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET)
#define AUX_RELAY_OFF()    	GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET)

#define SF_RL_WDT     PEout(0)

//FEEDBACK
#define SF_RL_DRV_FB   PEin(1)
#define SF_PWR_FB_CPU   PBin(9)
#define SF_RL_FB       PEin(5)

#define SF_RL1_FB_CPU2       PBin(8)

#define AUX_FB       PEin(3)

//EWDT
#define SF_EWDT_TOOGLE() 	GPIOE->ODR ^= 0X0001

#endif  /* GEC_SF_MASTER */


#endif /* __BSP_IOCFG_H */


/******************************  END OF FILE  *********************************/



