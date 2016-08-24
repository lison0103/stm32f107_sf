/*******************************************************************************
* File Name          : mb85rcxx.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : This file contains mb85rcxx driver functions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "mb85rcxx.h"
#include "stm32f10x_gpio.h"
#include "delay.h" 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define	EEP_SDA_PORT	        GPIOE
#define	EEP_SDA_PIN			GPIO_Pin_5

#define	EEP_SCL_PORT		GPIOE
#define	EEP_SCL_PIN			GPIO_Pin_4

#define EEP_SDA_SET()  		GPIO_WriteBit(EEP_SDA_PORT, EEP_SDA_PIN,Bit_SET)
#define EEP_SDA_CLR()  		GPIO_WriteBit(EEP_SDA_PORT, EEP_SDA_PIN,Bit_RESET)
#define EEP_SDA_READ() 		GPIO_ReadInputDataBit(EEP_SDA_PORT, EEP_SDA_PIN)

#define EEP_SCL_SET()  		GPIO_WriteBit(EEP_SCL_PORT, EEP_SCL_PIN, Bit_SET)
#define EEP_SCL_CLR()  		GPIO_WriteBit(EEP_SCL_PORT, EEP_SCL_PIN, Bit_RESET)

#define EEP_ACK       0u
#define EEP_NACK      1u
#define EEP_ERROR     1u

#define MB85RC16        2047
#define MB85RC64        8191

#define MB85RCXX_TYPE     MB85RC16
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void EEP_SDA_OUT(void);
void EEP_SCL_OUT(void);
void eep_start(void);
void eep_stop(void);
u8 eep_write(u8 d);
u8 eep_read(u8 ack);
uint8_t eeprom_data_write1(u16 addr,u16 len,u8 *dat);
uint8_t eeprom_data_read1(u16 addr, u16 len, u8 *dat);


u16 EEPROM_WR_TIME = 0u;



/*******************************************************************************
* Function Name  : EEP_SDA_OUT
* Description    : Initialization the SDA pin.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/ 
void EEP_SDA_OUT(void)  
{
    
      GPIO_InitTypeDef GPIO_InitStruct;
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;             
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; 
      
      GPIO_Init(GPIOE , &GPIO_InitStruct);	
}

/*******************************************************************************
* Function Name  : EEP_SCL_OUT
* Description    : Initialization the SCL pin.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EEP_SCL_OUT(void) 
{
      GPIO_InitTypeDef GPIO_InitStruct;
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;             
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;   
      
      GPIO_Init(GPIOE , &GPIO_InitStruct);
}

/*******************************************************************************
* Function Name  : eep_start
* Description    : Simulate IIC start
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void eep_start(void)	 
{ 
  /** scl and sda high level at the same **/
  delay_us(6u);
  EEP_SDA_SET();
  EEP_SCL_SET();
  
  /** delay > 5us **/
  delay_us(6u); 
  /** SDA set 0 **/
  EEP_SDA_CLR(); 

  /** delay > 5us **/
  delay_us(6u); 
  /** SCL set 0 **/
  EEP_SCL_CLR(); 
}

/*******************************************************************************
* Function Name  : eep_stop
* Description    : Simulate IIC stop
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void eep_stop(void)	
{
  /** scl and sda low level at the same **/
  EEP_SCL_CLR();  
  EEP_SDA_CLR(); 

  delay_us(5u); 

  /** SCL set 1 **/
  EEP_SCL_SET(); 
  
  /** delay > 5us **/
  delay_us(10u); 

  /** SDA set 1 **/
  EEP_SDA_SET(); 
}

/*******************************************************************************
* Function Name  : eep_write
* Description    : write data to mb85rcxx
*                  
* Input          : d: data want to be written
*                  None
* Output         : None
* Return         : EEP_ACK: success EEP_NACK: fail
*******************************************************************************/
u8 eep_write(u8 d)		
{
  u8 i;
                   
  EEP_SCL_CLR();       
  delay_us(2u); 

  for (i = 0u; i < 8u; i++)
  {
    if(d&0x80u)    
    {
      EEP_SDA_SET();  
    }
    else
    {
      EEP_SDA_CLR();
    } 
    
    delay_us(2u); 
    
    EEP_SCL_SET();
    d = d << 1u;	                 
    delay_us(5u); 
    EEP_SCL_CLR(); 
    
    delay_us(1u); 
  }   
  
  /** turn input **/
  EEP_SDA_SET();

  /** ACK **/
  delay_us(3u); 
  EEP_SCL_SET(); 
  delay_us(5u); 
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

/*******************************************************************************
* Function Name  : eep_read
* Description    : read a data from mb85rcxx
*                  
* Input          : None
*                  None
* Output         : None
* Return         : return read the data
*******************************************************************************/
u8 eep_read(u8 ack)	 
{
  u8 d=0u, i;
  
  EEP_SCL_CLR();
  EEP_SDA_SET();                
         
  for (i = 0u; i < 8u; i++)	
  {
    delay_us(5u);
    EEP_SCL_SET();    

    delay_us(5u);
    d = d << 1u;
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
      
  delay_us(5u);   
  EEP_SCL_SET();
  delay_us(5u);    
  EEP_SCL_CLR();   
  
  return d;
} 

/*******************************************************************************
* Function Name  : eep_init
* Description    : Initialization simulation IIC.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void eep_init(void)
{
  EEP_SDA_OUT();  
  EEP_SDA_SET();  

  EEP_SCL_OUT();
  EEP_SCL_SET();  
}

/*******************************************************************************
* Function Name  : eeprom_data_write1
* Description    : From address to start writing a length data
*                  
* Input          : addr: mb85rcxx address want to be written
*                  len: the lenth of data
*                  dat: want to write the address of data  
* Output         : None
* Return         : 0: success 1: fail
*******************************************************************************/
uint8_t eeprom_data_write1(u16 addr,u16 len,u8 *dat)
{
  u16 uint1; 
  u8 err=0u;
		
  eep_start();  
  if(eep_write(0xA0u)) 
  {
      err=1u;
  }
  if(MB85RCXX_TYPE > MB85RC16)
  {
      if(eep_write(addr>>8u))
      {
          err = 1u;
      }
      if(eep_write(addr&0x00ffu))
      {
          err = 1u;
      }
  }
  else 
  {
      if(eep_write(addr))
      {
          err=1u;
      }
  }
             
  for(uint1 = 0u; uint1 < len; uint1++)  
  {
    if(eep_write(*dat)) 
    {
      err=1u;
      break;
    }
    dat++;
  }  
  
  eep_stop();	
  
  EEPROM_WR_TIME = 1u;
  
  return(err);
}

/*******************************************************************************
* Function Name  : eeprom_write
* Description    : From address to start writing a length data
*                  
* Input          : addr: mb85rcxx address want to be written
*                  len: the lenth of data
*                  dat: want to write the address of data  
* Output         : None
* Return         : 0: success 1: fail
*******************************************************************************/
u8 eeprom_write(u16 addr,u16 len,u8 *dat)
{
	u8 ucCounter=0u,err=0u;
    
	while(ucCounter < 3u)
	{
		err = eeprom_data_write1(addr,len,dat);
		if(!err) 
                {
                    break;
		}
		ucCounter++;
		delay_ms( 50u );
	}
   	
	return(err); 
}

/*******************************************************************************
* Function Name  : eeprom_data_read1
* Description    : From address to start reading a length data
*                  
* Input          : addr: mb85rcxx address want to be read
*                  len: the lenth of data
*                  dat: want to read the address of data  
* Output         : None
* Return         : 0: success 1: fail
*******************************************************************************/
uint8_t eeprom_data_read1(u16 addr, u16 len, u8 *dat)
{
	u16 uint1;
	u8 err=0u;
	
	eep_start();     
	if(eep_write(0xA0u)) 
        {
            err=1u;
        }
        if(MB85RCXX_TYPE > MB85RC16)
        {
            if(eep_write(addr>>8u))
            {
                err=1u;
            }
            if(eep_write(addr&0x00ffu))
            {
                err=1u;
            }
        }
        else 
        {
            if(eep_write(addr))
            {
                err=1u;
            }
        }
	eep_start();
	if(eep_write(0xA1u)) 
        {
            err = 1u;
        }
        
	for(uint1=0u;uint1<(len-1u);uint1++)
	{
		*dat = eep_read(EEP_ACK);        
		dat++;   	
	}  
  
	*dat = eep_read(EEP_NACK);         	
  
	eep_stop();     	

	EEPROM_WR_TIME = 1u;		 
  
	return(err);                                                                                              
}

/*******************************************************************************
* Function Name  : eeprom_read
* Description    : From address to start reading a length data
*                  
* Input          : addr: mb85rcxx address want to be read
*                  len: the lenth of data
*                  dat: want to read the address of data  
* Output         : None
* Return         : 0: success 1: fail
*******************************************************************************/
u8 eeprom_read(u16 addr,u16 len,u8 *dat)
{
	u8 ucCounter=0u,err=0u;

	while(ucCounter < 3u)
	{
		err =  eeprom_data_read1(addr,len,dat);
		if(!err) 
                {
                    break;
		}
		ucCounter++;
		delay_ms( 50u );
	}
	    
	return(err);   
}


/*******************************************************************************
* Function Name  : MB85RCXX_Check
* Description    : Check the mb85rcxx.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : 0: success 1: fail
*******************************************************************************/
u8 MB85RCXX_Check(void)
{
	u8 temp;
        u8 result = 1u;
        
	eeprom_read(2047u,1u,&temp);			   
	if(temp == 0X55u)
        {		   
            result = 0u;
        }
        /** Rule out the first initialization **/
	else
	{       
                temp = 0x55u;
		eeprom_write(2047u,1u,&temp);
                eeprom_read(2047u,1u,&temp);	  
		if(temp == 0X55u)
                {
                    result = 0u;
                }
	}
	return result;

}


/******************************  END OF FILE  *********************************/


