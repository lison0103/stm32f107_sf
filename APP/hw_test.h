#ifndef __HW_TEST_H
#define __HW_TEST_H	
#include "sys.h"
#include "bsp_iocfg.h"

void Hw_Test1(void);
void Hw_Test2(void);
void spi1_test(void);
void can1_can2_test(void);
void SF_WDT_Check(void);

extern u8 data_error;
extern u8 canbuf_send[8];
extern u8 onetime;
extern u8 passflag;
extern u8 sfwdt_checkflag;

#ifdef GEC_SF_MASTER

extern u8 R_SF_RL2_FB_CPU1;

#else

extern u8 R_SF_RL1_FB_CPU2;

#endif



#endif  //__HW_TEST_H