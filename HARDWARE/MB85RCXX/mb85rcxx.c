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
#define EEP_FAST_MODE

/* Private macro -------------------------------------------------------------*/
#define	EEP_SDA_PORT	        GPIOE
#define	EEP_SDA_PIN			GPIO_Pin_5

#define	EEP_SCL_PORT		GPIOE
#define	EEP_SCL_PIN			GPIO_Pin_4

#ifdef EEP_FAST_MODE

#define EEP_SDA_SET()  		EEP_SDA_PORT->BSRR = EEP_SDA_PIN
#define EEP_SDA_CLR()  		EEP_SDA_PORT->BRR  = EEP_SDA_PIN 
#define EEP_SDA_READ() 		EEP_SDA_PORT->IDR  & EEP_SDA_PIN

#define EEP_SCL_SET()  		EEP_SCL_PORT->BSRR = EEP_SCL_PIN
#define EEP_SCL_CLR()  		EEP_SCL_PORT->BRR  = EEP_SCL_PIN

#else

#define EEP_SDA_SET()  		GPIO_WriteBit(EEP_SDA_PORT, EEP_SDA_PIN,Bit_SET)
#define EEP_SDA_CLR()  		GPIO_WriteBit(EEP_SDA_PORT, EEP_SDA_PIN,Bit_RESET)
#define EEP_SDA_READ() 		GPIO_ReadInputDataBit(EEP_SDA_PORT, EEP_SDA_PIN)

#define EEP_SCL_SET()  		GPIO_WriteBit(EEP_SCL_PORT, EEP_SCL_PIN, Bit_SET)
#define EEP_SCL_CLR()  		GPIO_WriteBit(EEP_SCL_PORT, EEP_SCL_PIN, Bit_RESET)

#endif

#define EEP_ACK       0u
#define EEP_NACK      1u
#define EEP_ERROR     1u

#define MB85RC16        2047
#define MB85RC64        8191

#define MB85RCXX_TYPE     MB85RC16
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void EEP_SDA_OUT(void);
static void EEP_SCL_OUT(void);
static void eep_start(void);
static void eep_stop(void);
static u8 eep_write(u8 d);
static u8 eep_read(u8 ack);
static uint8_t eeprom_data_write1(u16 addr,u16 len,u8 dat[]);
static uint8_t eeprom_data_read1(u16 addr, u16 len, u8 dat[]);
#ifdef EEP_FAST_MODE
static void I2C_delay(void);
static void I2C_NoAck(void);
static void I2C_Ack(void);
static u8 I2C_WaitAck(void);
#endif

/* static u16 EEPROM_WR_TIME = 0u; */

#ifdef EEP_FAST_MODE
static void I2C_delay(void)
{        
    delay_us(1u); 
}
#endif

/*******************************************************************************
* Function Name  : EEP_SDA_OUT
* Description    : Initialization the SDA pin.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/ 
static void EEP_SDA_OUT(void)  
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
static void EEP_SCL_OUT(void) 
{
      GPIO_InitTypeDef GPIO_InitStruct;
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;             
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;   
      
      GPIO_Init(GPIOE , &GPIO_InitStruct);
}

#ifdef EEP_FAST_MODE

static void eep_start(void)
{
    EEP_SDA_SET();
    EEP_SCL_SET();
    I2C_delay();
    
    EEP_SDA_CLR();
    I2C_delay();
    
    EEP_SCL_CLR();
    I2C_delay();
}

static void eep_stop(void)
{
    EEP_SCL_CLR();
    I2C_delay();
    EEP_SDA_CLR();
    I2C_delay();
    
    EEP_SCL_SET();
    I2C_delay();
    EEP_SDA_SET();
    I2C_delay();
}

static void I2C_Ack(void)
{        
    EEP_SCL_CLR();
    I2C_delay();
    EEP_SDA_CLR();
    I2C_delay();
    EEP_SCL_SET();
    I2C_delay();
    EEP_SCL_CLR();
    I2C_delay();
}

static void I2C_NoAck(void)
{        
    EEP_SCL_CLR();
    I2C_delay();
    EEP_SDA_SET();
    I2C_delay();
    EEP_SCL_SET();
    I2C_delay();
    EEP_SCL_CLR();
    I2C_delay();
}

static u8 I2C_WaitAck(void)          
{
    u8 i;
    
    EEP_SCL_CLR();
    I2C_delay();
    EEP_SDA_SET();                        
    I2C_delay();
    EEP_SCL_SET();
    I2C_delay();
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

static u8 eep_write(u8 d) 
{
    u8 i = 8u;
    
    while(i--)
    {
        EEP_SCL_CLR();
        I2C_delay();
        
        if(d&0x80u)
        {
            EEP_SDA_SET();  
        }
        else 
        {
            EEP_SDA_CLR();   
        }
        
        d <<= 1u;
        I2C_delay();
        EEP_SCL_SET();
        I2C_delay();
    }
    
    EEP_SCL_CLR();
    
    return I2C_WaitAck();
}

#else

/*******************************************************************************
* Function Name  : eep_start
* Description    : Simulate IIC start
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
static void eep_start(void)	 
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
static void eep_stop(void)	
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
static u8 eep_write(u8 d)		
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

#endif

/*******************************************************************************
* Function Name  : eep_read
* Description    : read a data from mb85rcxx
*                  
* Input          : None
*                  None
* Output         : None
* Return         : return read the data
*******************************************************************************/
static u8 eep_read(u8 ack)	 
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
    if(EEP_SDA_READ()) 
    {
        d++;		                  
    }
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
static uint8_t eeprom_data_write1(u16 addr,u16 len,u8 dat[])
{
  u16 uint1; 
  u8 err=0u;
		
  eep_start();  

  if(MB85RCXX_TYPE > MB85RC16)
  {
      if(eep_write(0xA0u)) 
      {
          err = 1u;
      }      
      if(eep_write((u8)(addr>>8u)))
      {
          err = 1u;
      }
      if(eep_write((u8)(addr&0x00ffu)))
      {
          err = 1u;
      }
  }
  else 
  {
      if(eep_write( 0xA0u | (u8)(addr>>7)&0x0eu ))
      {
          err = 1u;      
      }
      if(eep_write((u8)addr))
      {
          err = 1u;
      }
  }
             
  for(uint1 = 0u; uint1 < len; uint1++)  
  {
    if(eep_write(dat[uint1])) 
    {
      err=1u;
      break;
    }
  }  
  
  eep_stop();	
  
/*  EEPROM_WR_TIME = 1u; */
  
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
static uint8_t eeprom_data_read1(u16 addr, u16 len, u8 dat[])
{
	u16 uint1;
	u8 err=0u;
	
	eep_start();     
        if(MB85RCXX_TYPE > MB85RC16)
        {
            if(eep_write(0xA0u)) 
            {
                err = 1u;
            }      
            if(eep_write((u8)(addr>>8u)))
            {
                err = 1u;
            }
            if(eep_write((u8)(addr&0x00ffu)))
            {
                err = 1u;
            }
        }
        else 
        {
            if(eep_write( 0xA0u | (u8)(addr>>7)&0x0eu ))
            {
                err = 1u;      
            }
            if(eep_write((u8)addr))
            {
                err = 1u;
            }
        }
        
	eep_start();
	if(eep_write(0xA1u)) 
        {
            err = 1u;
        }
        
	for(uint1=0u;uint1<(len-1u);uint1++)
	{
		dat[uint1] = eep_read(EEP_ACK);          	
	}  
  
	dat[uint1] = eep_read(EEP_NACK);         	
  
	eep_stop();     	

/*	EEPROM_WR_TIME = 1u;		 */
  
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


