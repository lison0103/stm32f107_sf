
#ifndef __EWDT_H
#define __EWDT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#ifdef GEC_SF_MASTER
#define EWDT_TOOGLE() 	GPIOA->ODR ^= 0X0002
#define EWDT_ON     PAout(1)
#else
#define EWDT_TOOGLE() 	GPIOA->ODR ^= 0X0001
#endif

void power_on_bsp_check(void);
void EWDT_Drv_pin_config(void);


#endif //__EWDT_H