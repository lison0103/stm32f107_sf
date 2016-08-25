/*******************************************************************************
* File Name          : malloc.h
* Author             : lison
* Version            : V1.0
* Date               : 03/24/2016
* Description        : This file contains prototype of Memory management functions.
*			          
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MALLOC_H
#define __MALLOC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
struct _m_mallco_dev
{
	void (*init)(void);				/* Memory Initialization*/
	u8 (*perused)(void);		  	        /*Memory Usage*/
	u8  *membase;				        /*Memory pool */
	u16 *memmap; 					/*Memory management, state table*/
	u8  memrdy; 					/*Memory management is ready*/
};
extern struct _m_mallco_dev mallco_dev;	

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/*
#ifndef NULL
#define NULL 0
#endif
*/

/* Exported functions ------------------------------------------------------- */
void mymemset(void *s,u8 c,u32 count);	
void mymemcpy(void *des,void *src,u32 n);     					
u32 mmem_malloc(u32 size);		 		
u8 mmem_free(u32 offset);		 		
u8 mem_perused(void);					
/* Users call the function */
void mmem_init(void);
void myfree(void *ptr);  				
void *mymalloc(u32 size);				
void *myrealloc(void *ptr,u32 size);	



#endif /* __IAP_H */


/******************************  END OF FILE  *********************************/

















