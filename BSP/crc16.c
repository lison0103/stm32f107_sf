/*******************************************************************************
* File Name          : crc16.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains the 16-bit CRC Calculation and verification.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "crc16.h"
#include "stm32f10x_STLcrc32.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void STM_CRC_Init(void);
static u32 revbit(u32 data);
static u32 CRC32_ForBytes(u8 pData[],u32 uLen);

static const unsigned char aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const unsigned char aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};


/*******************************************************************************
* Function Name  : MB_CRC16
* Description    : Computes the 16-bit CRC
*                  
* Input          : pucFrame: The first address data to be checked
*                  usLen:    The length of the data to be checked
* Output         : None
* Return         : Check result
*******************************************************************************/
uint16_t MB_CRC16( uint8_t pucFrame[], uint16_t usLen )
{
    uint8_t ucCRCHi = 0xFFu;
    uint8_t ucCRCLo = 0xFFu;
    uint8_t iIndex;
    uint16_t returnCRC;
    uint16_t p = 0u;

    while( usLen-- )
    {
        iIndex = (ucCRCLo ^ pucFrame[p] );
        p++;
        ucCRCLo = ( uint8_t )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    
    returnCRC = (u16)ucCRCHi << 8u;
    returnCRC |= (u16)ucCRCLo;
    return returnCRC;
}


/*******************************************************************************
* Function Name  : MB_CRC32
* Description    : Computes the 32-bit CRC
*                  
* Input          : pucFrame: The first address data to be checked
*                  usLen:    The length of the data to be checked
                   Polynomials: Polynomials
* Output         : None
* Return         : Check result
*******************************************************************************/
u32 MB_CRC32(u8 pucFrame[], u16 usLen, u32 Polynomials)           
{
    u16 i,j;
    u32 crc = 0xffffffffu,flag;
    
    for( i = 0u; i < usLen; i++ )
    {
        crc ^= ((u32)pucFrame[i])<<24u;
        
        for(j = 0u;j < 8u; j++)  
        {
            flag = crc & 0x80000000u;
            crc <<= 1u;
            
            if(flag)
            {        
                crc  ^=  Polynomials;
                /*crc  ^=  0xfa567d89u;*/
            }
        }
    }
    
    /* Note: CRC32 return value after the call need to be reversed high and low, high in the former, low after the */
    return (crc);
}


static void STM_CRC_Init(void)
{
    /* Enable CRC module clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC ,ENABLE);
    
    /* Reset CRC generator */
    CRC->CR = CRC_RESET;
}

/*******************************************************************************
* Function Name  : MB_CRC32
* Description    : Computes the 32-bit CRC
*                  
* Input          : pBuffer: The first address data to be checked
*                  BufferLength:    The length of the data to be checked
* Output         : None
* Return         : Check result
*******************************************************************************/
u32 STM_CRC32(uint32_t pBuffer[], uint32_t BufferLength)
{
    uint32_t index = 0u;
    
    STM_CRC_Init();
    
    for(index = 0u; index < BufferLength; index++)
    {
        CRC->DR = pBuffer[index];
    }
  
    return (CRC->DR);  
}


static u32 revbit(u32 data)
{
    asm("rbit r0,r0");
    return data;
};

#define CRC32_POLYNOMIAL                        (0xEDB88320u)
/*******************************************************************************
* Function Name  : MB_CRC32
* Description    : Computes the 32-bit CRC
*                  
* Input          : pucFrame: The first address data to be checked
*                  usLen:    The length of the data to be checked
* Output         : None
* Return         : Check result
*******************************************************************************/
static u32 CRC32_ForBytes(u8 pData[],u32 uLen)
{
    u32 uIndex = 0u,*uData = 0u,i;
    uIndex = uLen >> 2;
    
    
    STM_CRC_Init();
    
    while(uIndex--)
    {
        uData = (u32*)pData[uIndex*4u];
        *uData = revbit(*uData);
        CRC->DR = *uData;
    }
    *uData = revbit(CRC->DR);
    uIndex = uLen & 0x03u;
    while(uIndex--)
    {
        *uData ^= (u32)*pData;
        for(i = 0u;i < 8u;i++)
        {
            if (*uData & 0x1u)
            {
                *uData = (*uData >> 1u) ^ CRC32_POLYNOMIAL;
            }
            else
            {
                *uData >>= 1u;
            }
        }
    }
    return *uData^0xFFFFFFFFu;
}









/******************************  END OF FILE  *********************************/