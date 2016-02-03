#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 

	 
static u8  fac_us=0;
static u16 fac_ms=0;



void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	
	fac_us=SystemCoreClock/8000000;	 
	 
	fac_ms=(u16)fac_us*1000;   
}								    

/** delay nus **/		    								   
void delay_us(u32 nus)
{		
	u32 temp;
        
        if(nus != 0)
        {
            SysTick->LOAD=nus*fac_us; 	  		 
            SysTick->VAL=0x00;        
            SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          	 
            do
            {
                    temp=SysTick->CTRL;
            }
            while(temp&0x01&&!(temp&(1<<16)));   
            SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       
            SysTick->VAL =0X00;       
        }
}

/** delay nms **/
void delay_ms(u16 nms)
{	 		  	  
        u32 temp;
        u32 Count_H = 0,Count_L = 0;
        
        if(nms != 0)
        {                
            Count_H = nms/1864;
            Count_L = nms%1864;
                   
            Count_H ++;
            while(Count_H--)
            {
                if(Count_H > 0)nms = 1864;
                else nms = Count_L;
                
                SysTick->LOAD=(u32)nms*fac_ms;
                SysTick->VAL =0x00;           
                SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          
                do
                {
                  temp=SysTick->CTRL;
                }
                while(temp&0x01&&!(temp&(1<<16)));   
                SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       
                SysTick->VAL =0X00;      	  	    
            }
        }
} 
































