#ifndef _LED_H
#define _LED_H
#include "sys.h"


/** LED Port **/
#ifdef GEC_SF_MASTER
#define LED PAout(2)
#else
#define LED PAout(1)
#endif

void LED_Init(void); 
 
#endif
