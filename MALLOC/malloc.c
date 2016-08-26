/*******************************************************************************
* File Name          : malloc.c
* Author             : lison
* Version            : V1.0
* Date               : 03/24/2016
* Description        : This file contains Memory management functions.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "malloc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/** Memory parameter settings **/
#define MEM_BLOCK_SIZE			( 32u ) 	  						/* Memory block size */
#define MEM_MAX_SIZE			( 13u * 1024u )          				        /* Maximum Memory Management */
#define MEM_ALLOC_TABLE_SIZE	( MEM_MAX_SIZE / MEM_BLOCK_SIZE )	                        /* Memory size table */

/* Private variables ---------------------------------------------------------*/
#pragma pack(push) /* memory size table */
#pragma pack(4)/* Is set to 4-byte alignment */ 
__no_init u8 membase[MEM_MAX_SIZE]; /* Internal RAM memory pool  */
#pragma pack(pop)/* Restore the alignment */
/* Memory management table */
u16 memmapbase[MEM_ALLOC_TABLE_SIZE];			
/* Memory management parameters */	   
const u32 memtblsize=MEM_ALLOC_TABLE_SIZE;		
const u32 memblksize=MEM_BLOCK_SIZE;			
const u32 memsize=MEM_MAX_SIZE;					

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* Memory Management Controller */
struct _m_mallco_dev mallco_dev=
{
	mmem_init,			
	mem_perused,		
	membase,			
	memmapbase,			
	0,  				
}; 



/*******************************************************************************
* Function Name  : mymemcpy
* Description    : Copy memory
*                  
* Input          : des: destination address.
*                  src: source address.
*                  N: need to copy the memory length (in bytes)
* Output         : None
* Return         : None
*******************************************************************************/ 	   
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
    u8 *xsrc=src; 
    
    while(n--)
    {
        *xdes++=*xsrc++;  
    }
}  


/*******************************************************************************
* Function Name  : mymemset
* Description    : Set memory
*                  
* Input          : S: first memory address
*                  C: value to set
*                  Count: need to set the memory size (in bytes)
* Output         : None
* Return         : None
*******************************************************************************/ 
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;
    
    while(count--)
    {
        *xs++=c;  
    }
}	


/*******************************************************************************
* Function Name  : mmem_init
* Description    : Memory Management Initialization
*                  
* Input          : None
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void mmem_init(void)  
{  
    mymemset(mallco_dev.memmap, 0u,memtblsize*2u); 
    mymemset(mallco_dev.membase, 0u,memsize);	 
    mallco_dev.memrdy = 1u;						 
}  

/*******************************************************************************
* Function Name  : mem_perused
* Description    : Get Memory Usage
*                  
* Input          : None
*                  
* Output         : None
* Return         : Return Value: usage rate (0 to 100)
*******************************************************************************/ 

u8 mem_perused(void)  
{  
    u32 used=0u;  
    u32 i;  
    
    for(i = 0u; i < memtblsize; i++)  
    {  
        if(mallco_dev.memmap[i])
        {
            used++; 
        }
    }
    
    return (u8)((used*100u)/(memtblsize));  
}  


/*******************************************************************************
* Function Name  : mmem_malloc
* Description    : Memory allocation (internal call)
*                  
* Input          : Size: to allocate memory size (bytes)
*                  
* Output         : None
* Return         : Return Value: 0XFFFFFFFF, on behalf of the error; the other, the memory offset address
*******************************************************************************/ 
u32 mmem_malloc(u32 size)  
{  
    int offset = 0;  
    u32 nmemb;	
    u16 cmemb=0u;
    u32 i;
    u32 result = 0XFFFFFFFFu;
    
    if(!mallco_dev.memrdy)
    {
        mallco_dev.init();	
    }
    if(size == 0u)
    {	
        result = 0XFFFFFFFFu;
    }
    else
    {
        nmemb=size/memblksize;  					
        if(size%memblksize)
        {
            nmemb++;  
        }
        for(offset = (int)memtblsize - 1; offset >= 0; offset--)	  
        {     
            if(!mallco_dev.memmap[offset])
            {
                cmemb++;	
            }
            else 
            {
                cmemb = 0u;							
            }
            if(cmemb == nmemb)						
            {
                for(i=0u;i<nmemb;i++)  				
                {  
                    mallco_dev.memmap[(u32)offset + i] = (u16)nmemb;  
                }  
                result = ((u32)offset*memblksize);
            }
        } 
    }
    return result; 
}  


/*******************************************************************************
* Function Name  : mmem_free
* Description    : Release memory (internal call)
*                  
* Input          : Offset: memory address offset
*                  
* Output         : None
* Return         : 0, release success; 1, release failure;
*******************************************************************************/  
u8 mmem_free(u32 offset)  
{  
    u32 i;  
    u8 result = 0u;
    
    if(!mallco_dev.memrdy)
    {
        mallco_dev.init();    
        result = 1u;
    }  
    else
    {
        if(offset<memsize) 
        {  
            u32 index=offset/memblksize;		
            u32 nmemb=mallco_dev.memmap[index];	
            for( i = 0u; i < nmemb; i++ )  				
            {  
                mallco_dev.memmap[index+i] = 0u;  
            }
            result = 0u;
        }
        else 
        {
            result = 2u;
        }
    }
    
    return result;
}  


/*******************************************************************************
* Function Name  : myfree
* Description    : Release memory (external calls)
*                  
* Input          : ptr: first memory address
*                  
* Output         : None
* Return         : None
*******************************************************************************/ 
void myfree(void *ptr)  
{  
    u32 offset;  
    
    if( ptr != 0 )
    {       
        offset=(u32)ptr-(u32)mallco_dev.membase;  
        mmem_free(offset);	  
    }
}  


/*******************************************************************************
* Function Name  : mymalloc
* Description    : Allocate memory (external calls)
*                  
* Input          : Size: memory size (bytes)
*                  
* Output         : None
* Return         : memory allocated to the first address
*******************************************************************************/ 
void *mymalloc(u32 size)  
{  
    u32 offset;  	
    
    offset=mmem_malloc(size);  	   				   
    if( offset == 0XFFFFFFFFu )
    {
        return 0;  
    }
    else 
    {
        return (void*)((u32)mallco_dev.membase+offset);  
    }
}  


/*******************************************************************************
* Function Name  : myrealloc
* Description    : Reallocate memory (external calls)
*                  
* Input          : ptr: the old memory first address
*                  size: to allocate memory size (bytes)
* Output         : None
* Return         : the newly allocated memory to the first address.
*******************************************************************************/ 
void *myrealloc(void *ptr,u32 size)  
{  
    u32 offset;  
    
    offset=mmem_malloc(size);  
    if( offset == 0XFFFFFFFFu )
    {
        return 0;     
    }
    else  
    {  									   
        mymemcpy((void*)((u32)mallco_dev.membase+offset),ptr,size);	
        myfree(ptr);  											  	
        return (void*)((u32)mallco_dev.membase+offset);  			
    }  
}


/******************************  END OF FILE  *********************************/














