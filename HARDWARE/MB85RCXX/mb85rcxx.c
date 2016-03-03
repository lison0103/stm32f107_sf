 
#include "mb85rcxx.h"
#include "stm32f10x_gpio.h"
#include "delay.h" 

/******************************************************************************* 
*******************************************************************************/
#define	EEP_SDA_PORT	        GPIOE
#define	EEP_SDA_PIN			GPIO_Pin_5

#define	EEP_SCL_PORT		GPIOE
#define	EEP_SCL_PIN			GPIO_Pin_4

#define EEP_SDA_SET()  						GPIO_WriteBit(EEP_SDA_PORT, EEP_SDA_PIN,Bit_SET)
#define EEP_SDA_CLR()  						GPIO_WriteBit(EEP_SDA_PORT, EEP_SDA_PIN,Bit_RESET)
#define EEP_SDA_READ() 						GPIO_ReadInputDataBit(EEP_SDA_PORT, EEP_SDA_PIN)

#define EEP_SCL_SET()  						GPIO_WriteBit(EEP_SCL_PORT, EEP_SCL_PIN, Bit_SET)
#define EEP_SCL_CLR()  						GPIO_WriteBit(EEP_SCL_PORT, EEP_SCL_PIN, Bit_RESET)

#define EEP_ACK       0
#define EEP_NACK      1
#define EEP_ERROR     1

u16 EEPROM_WR_TIME=0;

/******************************************************************************* 
*******************************************************************************/
void EEP_SDA_OUT(void)  
{
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE );
      
      GPIO_InitTypeDef GPIO_InitStruct;
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;             
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; 
      
      GPIO_Init(GPIOE , &GPIO_InitStruct);	
}

void EEP_SCL_OUT(void) 
{
      GPIO_InitTypeDef GPIO_InitStruct;
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;             
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;   
      
      GPIO_Init(GPIOE , &GPIO_InitStruct);
}

/******************************************************************************* 
*******************************************************************************/
void eep_start(void)	 
{ 
  /** scl and sda high level at the same **/
  delay_us(6);
  EEP_SDA_SET();
  EEP_SCL_SET();
  
  /** delay > 5us **/
  delay_us(6); 
  /** SDA set 0 **/
  EEP_SDA_CLR(); 

  /** delay > 5us **/
  delay_us(6); 
  /** SCL set 0 **/
  EEP_SCL_CLR(); 
}

void eep_stop(void)	
{
  /** scl and sda low level at the same **/
  EEP_SCL_CLR();  
  EEP_SDA_CLR(); 

  delay_us(5); 

  /** SCL set 1 **/
  EEP_SCL_SET(); 
  
  /** delay > 5us **/
  delay_us(10); 

  /** SDA set 1 **/
  EEP_SDA_SET(); 
}

u8 eep_write(u8 d)		
{
  u8 i;
                   
  EEP_SCL_CLR();       
  delay_us(2); 

  for (i = 0; i < 8; i++)
  {
    if(d&0x80)    
    {
      EEP_SDA_SET();  
    }
    else
    {
      EEP_SDA_CLR();
    } 
    
    delay_us(2); 
    
    EEP_SCL_SET();
    d = d << 1;	                 
    delay_us(5); 
    EEP_SCL_CLR(); 
    
    delay_us(1); 
  }   
  
  /** turn input **/
  EEP_SDA_SET();

  /** ACK **/
  delay_us(3); 
  EEP_SCL_SET(); 
  delay_us(5); 
  if(EEP_SDA_READ()) 
  {
    i = EEP_NACK;
  }
  else
  {
    i = EEP_ACK;
  }
  EEP_SCL_CLR(); 
  
  return(i);
}
      
u8 eep_read(u8 ack)	 
{
  u8 d=0, i;
  
  EEP_SCL_CLR();
  EEP_SDA_SET();                
         
  for (i = 0; i < 8; i++)	
  {
    delay_us(5);
    EEP_SCL_SET();    

    delay_us(5);
    d = d << 1;
    if(EEP_SDA_READ()) d++;		                  
 
    EEP_SCL_CLR();   
  }
  
  if(ack==EEP_ACK)  
  {
    EEP_SDA_CLR();
  } 
  else
  {
    EEP_SDA_SET();     
  } 
      
  delay_us(5);   
  EEP_SCL_SET();
  delay_us(5);    
  EEP_SCL_CLR();   
  
  return d;
} 

/******************************************************************************* 
*******************************************************************************/
void eep_init(void)
{
  EEP_SDA_OUT();  
  EEP_SDA_SET();  

  EEP_SCL_OUT();
  EEP_SCL_SET();  
}

/******************************************************************************* 
*******************************************************************************/
uint8_t eeprom_data_write1(u16 addr,u16 len,u8 *dat)
{
  u16 uint1; 
  u8 err=0;
		
  eep_start();  
  if(eep_write(0xA0)) err=1;
  if(MB85RCXX_TYPE > MB85RC16)
  {
      if(eep_write(addr>>8)) err=1;
      if(eep_write(addr&0x00ff)) err=1;
  }
  else 
  {
      if(eep_write(addr)) err=1;
  }
             
  for(uint1=0;uint1<len;uint1++)  
  {
    if(eep_write(*dat)) 
    {
      err=1;
      break;
    }
    dat++;
  }  
  
  eep_stop();	
  
  EEPROM_WR_TIME = 1;
  
  return(err);
}

u8 eeprom_write(u16 addr,u16 len,u8 *dat)
{
	u8 ucCounter=0,err=0;
    
	while(ucCounter<3)
	{
		err = eeprom_data_write1(addr,len,dat);
		if(!err) break;
		
		ucCounter++;
		delay_ms( 50 );
	}
   	
	return(err); 
}

/******************************************************************************* 
*******************************************************************************/
uint8_t eeprom_data_read1(u16 addr, u16 len, u8 *dat)
{
	u16 uint1;
	u8 err=0;
	
	eep_start();     
	if(eep_write(0xA0))  err=1;
        if(MB85RCXX_TYPE > MB85RC16)
        {
            if(eep_write(addr>>8)) err=1;
            if(eep_write(addr&0x00ff)) err=1;
        }
        else 
        {
            if(eep_write(addr)) err=1;
        }
	eep_start();
	if(eep_write(0xA1))  err=1;
  
	for(uint1=0;uint1<(len-1);uint1++)
	{
		*dat = eep_read(EEP_ACK);        
		dat++;   	
	}  
  
	*dat = eep_read(EEP_NACK);         	
  
	eep_stop();     	

	EEPROM_WR_TIME = 1;		 
  
	return(err);                                                                                              
}

u8 eeprom_read(u16 addr,u16 len,u8 *dat)
{
	u8 ucCounter=0,err=0;

	while(ucCounter<3)
	{
		err =  eeprom_data_read1(addr,len,dat);
		if(!err) break;
		
		ucCounter++;
		delay_ms( 50 );
	}
	    
	return(err);   
}


/******************************************************************************* 
*******************************************************************************/
u8 MB85RCXX_Check(void)
{
	u8 temp;
	eeprom_read(2047,1,&temp);	//2048		   
	if(temp==0X55)return 0;		   
        /** Rule out the first initialization **/
	else
	{       
                temp = 0x55;
		eeprom_write(2047,1,&temp);
                eeprom_read(2047,1,&temp);	  
		if(temp==0X55)return 0;
	}
	return 1;

}

/******************************************************************************* 
*******************************************************************************/
