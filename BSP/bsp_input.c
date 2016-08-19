/*******************************************************************************
* File Name          : bsp_input.c
* Author             : lison
* Version            : V1.0
* Date               : 06/16/2016
* Description        : Contains get the input pin value function.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "bsp_input.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Get_GpioInput
* Description    : Get GPIO input pin value.
*                  
* Input          : inBuff: read the gpio pin value of data
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Get_GpioInput(u8 inBuff[])
{     

    static u16 PinValue[3u][5u],ByteAnd[5u],ByteOr[5u],read_pin_cnt = 0u;
    u16 i; 
    
    /* first, clear the data */
    for(i = 0u; i < 8u; i++)
    {
        inBuff[i] = 0u;
    }

#if 1
    /* read the pin value three times */
    
    read_pin_cnt++;  
    if( read_pin_cnt > 2u ) 
    {
        read_pin_cnt = 0u;  
    }
    
    /* Read I/O port */                      
    PinValue[read_pin_cnt][0u] = GPIO_ReadInputData(GPIOA);
    PinValue[read_pin_cnt][1u] = GPIO_ReadInputData(GPIOB);
    PinValue[read_pin_cnt][2u] = GPIO_ReadInputData(GPIOC);
    PinValue[read_pin_cnt][3u] = GPIO_ReadInputData(GPIOD);
    PinValue[read_pin_cnt][4u] = GPIO_ReadInputData(GPIOE);
    
    for(i = 0u;i < 5u; i++)
    {   
        ByteAnd[i] = PinValue[0u][i] & PinValue[1u][i] & PinValue[2u][i];
        ByteOr[i] = PinValue[0u][i] | PinValue[1u][i] | PinValue[2u][i]; 
    }
#else
    /* read the pin value only one times */
    
    read_pin_cnt = 0u;
    
    /* Read I/O port */                      
    PinValue[read_pin_cnt][0u] = GPIO_ReadInputData(GPIOA);
    PinValue[read_pin_cnt][1u] = GPIO_ReadInputData(GPIOB);
    PinValue[read_pin_cnt][2u] = GPIO_ReadInputData(GPIOC);
    PinValue[read_pin_cnt][3u] = GPIO_ReadInputData(GPIOD);
    PinValue[read_pin_cnt][4u] = GPIO_ReadInputData(GPIOE);
    
    for( i = 0u; i < 5u; i++ )
    {   
        ByteAnd[i] = PinValue[0u][i]; 
        ByteOr[i] = PinValue[0u][i];
    }    
    
#endif
    
    
#ifdef GEC_SF_MASTER
    
    /* INA1 ~ INA8 */
    if(ByteAnd[1u] & 0x0080u){ inBuff[0u] &= ~0x01u;} else if(!(ByteOr[1u] & 0x0080u)){ inBuff[0u] |= 0x01u;}else{} 
    if(ByteAnd[1u] & 0x0040u){ inBuff[0u] &= ~0x02u;} else if(!(ByteOr[1u] & 0x0040u)){ inBuff[0u] |= 0x02u;}else{}  
    if(ByteAnd[4u] & 0x0001u){ inBuff[0u] &= ~0x04u;} else if(!(ByteOr[4u] & 0x0001u)){ inBuff[0u] |= 0x04u;}else{}  
    if(ByteAnd[4u] & 0x0002u){ inBuff[0u] &= ~0x08u;} else if(!(ByteOr[4u] & 0x0002u)){ inBuff[0u] |= 0x08u;}else{}  
    if(ByteAnd[1u] & 0x0020u){ inBuff[0u] &= ~0x10u;} else if(!(ByteOr[1u] & 0x0020u)){ inBuff[0u] |= 0x10u;}else{}  
    if(ByteAnd[1u] & 0x0010u){ inBuff[0u] &= ~0x20u;} else if(!(ByteOr[1u] & 0x0010u)){ inBuff[0u] |= 0x20u;}else{}  
    if(ByteAnd[1u] & 0x0008u){ inBuff[0u] &= ~0x40u;} else if(!(ByteOr[1u] & 0x0008u)){ inBuff[0u] |= 0x40u;}else{}  
    if(ByteAnd[3u] & 0x0004u){ inBuff[0u] &= ~0x80u;} else if(!(ByteOr[3u] & 0x0004u)){ inBuff[0u] |= 0x80u;}else{}  
    
    /* INA9 ~ INA16 */
    if(ByteAnd[3u] & 0x0040u){ inBuff[1u] |= 0x01u;} else if(!(ByteOr[3u] & 0x0040u)){ inBuff[1u] &= ~0x01u;}else{}  
    if(ByteAnd[3u] & 0x0020u){ inBuff[1u] |= 0x02u;} else if(!(ByteOr[3u] & 0x0020u)){ inBuff[1u] &= ~0x02u;}else{}  
    if(ByteAnd[3u] & 0x0010u){ inBuff[1u] |= 0x04u;} else if(!(ByteOr[3u] & 0x0010u)){ inBuff[1u] &= ~0x04u;}else{}  
    if(ByteAnd[3u] & 0x0008u){ inBuff[1u] |= 0x08u;} else if(!(ByteOr[3u] & 0x0008u)){ inBuff[1u] &= ~0x08u;}else{}  
    if(ByteAnd[3u] & 0x0080u){ inBuff[1u] |= 0x10u;} else if(!(ByteOr[3u] & 0x0080u)){ inBuff[1u] &= ~0x10u;}else{}  
    if(ByteAnd[2u] & 0x1000u){ inBuff[1u] |= 0x20u;} else if(!(ByteOr[2u] & 0x1000u)){ inBuff[1u] &= ~0x20u;}else{}  
    if(ByteAnd[2u] & 0x0800u){ inBuff[1u] |= 0x40u;} else if(!(ByteOr[2u] & 0x0800u)){ inBuff[1u] &= ~0x40u;}else{}  
    if(ByteAnd[2u] & 0x0400u){ inBuff[1u] |= 0x80u;} else if(!(ByteOr[2u] & 0x0400u)){ inBuff[1u] &= ~0x80u;}else{}  
    
    /* INA17 ~ INA24 */
    if(ByteAnd[0u] & 0x8000u){ inBuff[2u] |= 0x01u;} else if(!(ByteOr[0u] & 0x0800u)){ inBuff[2u] &= ~0x01u;}else{} 
    if(ByteAnd[0u] & 0x0200u){ inBuff[2u] |= 0x02u;} else if(!(ByteOr[0u] & 0x0200u)){ inBuff[2u] &= ~0x02u;}else{} 
    if(ByteAnd[0u] & 0x0100u) {inBuff[2u] |= 0x04u;} else if(!(ByteOr[0u] & 0x0100u)){ inBuff[2u] &= ~0x04u;}else{} 
    if(ByteAnd[2u] & 0x0200u){ inBuff[2u] |= 0x08u;} else if(!(ByteOr[2u] & 0x0200u)){ inBuff[2u] &= ~0x08u;}else{} 
    if(ByteAnd[2u] & 0x0100u){ inBuff[2u] |= 0x10u;} else if(!(ByteOr[2u] & 0x0100u)){ inBuff[2u] &= ~0x10u;}else{} 
    if(ByteAnd[2u] & 0x0080u){ inBuff[2u] |= 0x20u;} else if(!(ByteOr[2u] & 0x0080u)){ inBuff[2u] &= ~0x20u; }else{}
    if(ByteAnd[4u] & 0x0200u) {inBuff[2u] |= 0x40u;} else if(!(ByteOr[4u] & 0x0200u)){ inBuff[2u] &= ~0x40u; }else{}
    if(ByteAnd[4u] & 0x0100u) {inBuff[2u] |= 0x80u;} else if(!(ByteOr[4u] & 0x0100u)){ inBuff[2u] &= ~0x80u; }else{}
    
    /* INA25 ~ INA29 */
    if(ByteAnd[4u] & 0x0080u) {inBuff[3u] |= 0x01u;} else if(!(ByteOr[4u] & 0x0080u)){ inBuff[3u] &= ~0x01u; }else{}
    if(ByteAnd[1u] & 0x0004u) {inBuff[3u] |= 0x02u;} else if(!(ByteOr[1u] & 0x0004u)){ inBuff[3u] &= ~0x02u; }else{}
    if(ByteAnd[1u] & 0x0002u) {inBuff[3u] |= 0x04u;} else if(!(ByteOr[1u] & 0x0002u)){ inBuff[3u] &= ~0x04u; }else{}
    if(ByteAnd[1u] & 0x0001u) {inBuff[3u] |= 0x08u;} else if(!(ByteOr[1u] & 0x0001u)) {inBuff[3u] &= ~0x08u;}else{}
    if(ByteAnd[2u] & 0x0020u) {inBuff[3u] |= 0x10u;} else if(!(ByteOr[2u] & 0x0020u)){ inBuff[3u] &= ~0x10u;}else{}
    
    /* AX1 ~ AX8 */
    if(ByteAnd[2u] & 0x0040u) {inBuff[4u] |= 0x01u;} else if(!(ByteOr[2u] & 0x0040u)){ inBuff[4u] &= ~0x01u; }else{}
    if(ByteAnd[3u] & 0x8000u) {inBuff[4u] |= 0x02u;} else if(!(ByteOr[3u] & 0x8000u)){ inBuff[4u] &= ~0x02u; }else{}
    if(ByteAnd[3u] & 0x4000u) {inBuff[4u] |= 0x04u;} else if(!(ByteOr[3u] & 0x4000u)){ inBuff[4u] &= ~0x04u; }else{}
    if(ByteAnd[3u] & 0x2000u) {inBuff[4u] |= 0x08u;} else if(!(ByteOr[3u] & 0x2000u)){ inBuff[4u] &= ~0x08u; }else{}
    if(ByteAnd[3u] & 0x1000u) {inBuff[4u] |= 0x10u;} else if(!(ByteOr[3u] & 0x1000u)){ inBuff[4u] &= ~0x10u; }else{}
    if(ByteAnd[3u] & 0x0800u) {inBuff[4u] |= 0x20u;} else if(!(ByteOr[3u] & 0x0800u)){ inBuff[4u] &= ~0x20u; }else{}
    if(ByteAnd[3u] & 0x0400u) {inBuff[4u] |= 0x40u;} else if(!(ByteOr[3u] & 0x0400u)){ inBuff[4u] &= ~0x40u; }else{}
    if(ByteAnd[3u] & 0x0200u) {inBuff[4u] |= 0x80u;} else if(!(ByteOr[3u] & 0x0200u)){ inBuff[4u] &= ~0x80u;}else{}

    /* AX9 ~ AX16 */
    if(ByteAnd[3u] & 0x0100u) {inBuff[5u] |= 0x01u;} else if(!(ByteOr[3u] & 0x0100u)) {inBuff[5u] &= ~0x01u; }else{}
    if(ByteAnd[1u] & 0x8000u) {inBuff[5u] |= 0x02u;} else if(!(ByteOr[1u] & 0x8000u)){ inBuff[5u] &= ~0x02u; }else{}
    if(ByteAnd[1u] & 0x4000u) {inBuff[5u] |= 0x04u;} else if(!(ByteOr[1u] & 0x4000u)) {inBuff[5u] &= ~0x04u; }else{}
    if(ByteAnd[4u] & 0x8000u) {inBuff[5u] |= 0x08u;} else if(!(ByteOr[4u] & 0x8000u)){ inBuff[5u] &= ~0x08u; }else{}
    if(ByteAnd[4u] & 0x4000u) {inBuff[5u] |= 0x10u;} else if(!(ByteOr[4u] & 0x4000u)){ inBuff[5u] &= ~0x10u; }else{}
    if(ByteAnd[4u] & 0x2000u) {inBuff[5u] |= 0x20u;} else if(!(ByteOr[4u] & 0x2000u)){ inBuff[5u] &= ~0x20u; }else{}
    if(ByteAnd[4u] & 0x1000u) {inBuff[5u] |= 0x40u;} else if(!(ByteOr[4u] & 0x1000u)){ inBuff[5u] &= ~0x40u; }else{}
    if(ByteAnd[4u] & 0x0800u) {inBuff[5u] |= 0x80u;} else if(!(ByteOr[4u] & 0x0800u)){ inBuff[5u] &= ~0x80u;}else{}

    /* AX17 */
    if(ByteAnd[4u] & 0x0400u) {inBuff[6u] |= 0x01u;} else if(!(ByteOr[4u] & 0x0400u)) {inBuff[6u] &= ~0x01u;}else{}
    
    
    /* SF_RL_DRV_FB */
    if(ByteAnd[4u] & 0x0008u){ inBuff[7u] |= 0x01u;} else if(!(ByteOr[4u] & 0x0008u)){ inBuff[7u] &= ~0x01u; }else{}
    /* SF_PWR_FB_CPU */
    if(ByteAnd[1u] & 0x0100u) {inBuff[7u] |= 0x02u;} else if(!(ByteOr[1u] & 0x0100u)){ inBuff[7u] &= ~0x02u; }else{}
    /* SF_RL_FB */
    if(ByteAnd[2u] & 0x0004u) {inBuff[7u] |= 0x04u;} else if(!(ByteOr[2u] & 0x0004u)){ inBuff[7u] &= ~0x04u; }else{}
    /* SF_RL2_FB_CPU1 */
    if(ByteAnd[4u] & 0x0040u) {inBuff[7u] |= 0x08u;} else if(!(ByteOr[4u] & 0x0040u)){ inBuff[7u] &= ~0x08u; }else{}
    /* AUX_FB */
    if(ByteAnd[2u] & 0x0002u){ inBuff[7u] |= 0x10u;} else if(!(ByteOr[2u] & 0x0002u)) {inBuff[7u] &= ~0x10u; }else{}
    /* PLUSE_OUTPUT_FB */
    if(ByteAnd[2u] & 0x0010u) {inBuff[7u] |= 0x20u;} else if(!(ByteOr[2u] & 0x0010u)){ inBuff[7u] &= ~0x20u; }else{}
    
#else  
    
    /* INB1 ~ INB8 */
    if(ByteAnd[1] & 0x0080) inBuff[0] &= ~0x01; else if(!(ByteOr[1] & 0x0080)) inBuff[0] |= 0x01; 
    if(ByteAnd[1] & 0x0040) inBuff[0] &= ~0x02; else if(!(ByteOr[1] & 0x0040)) inBuff[0] |= 0x02; 
    if(ByteAnd[1] & 0x0020) inBuff[0] &= ~0x04; else if(!(ByteOr[1] & 0x0020)) inBuff[0] |= 0x04; 
    if(ByteAnd[1] & 0x0010) inBuff[0] &= ~0x08; else if(!(ByteOr[1] & 0x0010)) inBuff[0] |= 0x08; 
    if(ByteAnd[1] & 0x0008) inBuff[0] &= ~0x10; else if(!(ByteOr[1] & 0x0008)) inBuff[0] |= 0x10; 
    if(ByteAnd[3] & 0x0004) inBuff[0] &= ~0x20; else if(!(ByteOr[3] & 0x0004)) inBuff[0] |= 0x20; 
    if(ByteAnd[3] & 0x0002) inBuff[0] &= ~0x40; else if(!(ByteOr[3] & 0x0002)) inBuff[0] |= 0x40; 
    if(ByteAnd[3] & 0x0001) inBuff[0] &= ~0x80; else if(!(ByteOr[3] & 0x0001)) inBuff[0] |= 0x80; 
    
    /* INB9 ~ INB16 */
    if(ByteAnd[0] & 0x8000) inBuff[1] |= 0x01; else if(!(ByteOr[0] & 0x8000)) inBuff[1] &= ~0x01; 
    if(ByteAnd[0] & 0x0400) inBuff[1] |= 0x02; else if(!(ByteOr[0] & 0x0400)) inBuff[1] &= ~0x02; 
    if(ByteAnd[0] & 0x0200) inBuff[1] |= 0x04; else if(!(ByteOr[0] & 0x0200)) inBuff[1] &= ~0x04; 
    if(ByteAnd[0] & 0x0100) inBuff[1] |= 0x08; else if(!(ByteOr[0] & 0x0100)) inBuff[1] &= ~0x08; 
    if(ByteAnd[2] & 0x0200) inBuff[1] |= 0x10; else if(!(ByteOr[2] & 0x0200)) inBuff[1] &= ~0x10; 
    if(ByteAnd[2] & 0x0100) inBuff[1] |= 0x20; else if(!(ByteOr[2] & 0x0100)) inBuff[1] &= ~0x20; 
    if(ByteAnd[2] & 0x0080) inBuff[1] |= 0x40; else if(!(ByteOr[2] & 0x0080)) inBuff[1] &= ~0x40; 
    if(ByteAnd[2] & 0x0040) inBuff[1] |= 0x80; else if(!(ByteOr[2] & 0x0040)) inBuff[1] &= ~0x80; 
    
    /* INB17 ~ INB24 */
    if(ByteAnd[3] & 0x8000) inBuff[2] |= 0x01; else if(!(ByteOr[3] & 0x8000)) inBuff[2] &= ~0x01; 
    if(ByteAnd[3] & 0x4000) inBuff[2] |= 0x02; else if(!(ByteOr[3] & 0x4000)) inBuff[2] &= ~0x02; 
    if(ByteAnd[3] & 0x2000) inBuff[2] |= 0x04; else if(!(ByteOr[3] & 0x2000)) inBuff[2] &= ~0x04; 
    if(ByteAnd[3] & 0x1000) inBuff[2] |= 0x08; else if(!(ByteOr[3] & 0x1000)) inBuff[2] &= ~0x08; 
    if(ByteAnd[3] & 0x0800) inBuff[2] |= 0x10; else if(!(ByteOr[3] & 0x0800)) inBuff[2] &= ~0x10; 
    if(ByteAnd[3] & 0x0400) inBuff[2] |= 0x20; else if(!(ByteOr[3] & 0x0400)) inBuff[2] &= ~0x20; 
    if(ByteAnd[3] & 0x0200) inBuff[2] |= 0x40; else if(!(ByteOr[3] & 0x0200)) inBuff[2] &= ~0x40; 
    if(ByteAnd[3] & 0x0100) inBuff[2] |= 0x80; else if(!(ByteOr[3] & 0x0100)) inBuff[2] &= ~0x80; 
    
    /* INB25 ~ INB29 */
    if(ByteAnd[1] & 0x8000) inBuff[3] |= 0x01; else if(!(ByteOr[1] & 0x8000)) inBuff[3] &= ~0x01; 
    if(ByteAnd[1] & 0x4000) inBuff[3] |= 0x02; else if(!(ByteOr[1] & 0x4000)) inBuff[3] &= ~0x02; 
    if(ByteAnd[1] & 0x2000) inBuff[3] |= 0x04; else if(!(ByteOr[1] & 0x2000)) inBuff[3] &= ~0x04; 
    if(ByteAnd[1] & 0x1000) inBuff[3] |= 0x08; else if(!(ByteOr[1] & 0x1000)) inBuff[3] &= ~0x08;
    if(ByteAnd[3] & 0x0008) inBuff[3] |= 0x10; else if(!(ByteOr[3] & 0x0008)) inBuff[3] &= ~0x10;
    
    /* BX1 ~ BX8 */
    if(ByteAnd[1] & 0x0800) inBuff[4] |= 0x01; else if(!(ByteOr[1] & 0x0800)) inBuff[4] &= ~0x01; 
    if(ByteAnd[1] & 0x0400) inBuff[4] |= 0x02; else if(!(ByteOr[1] & 0x0400)) inBuff[4] &= ~0x02; 
    if(ByteAnd[4] & 0x8000) inBuff[4] |= 0x04; else if(!(ByteOr[4] & 0x8000)) inBuff[4] &= ~0x04; 
    if(ByteAnd[4] & 0x4000) inBuff[4] |= 0x08; else if(!(ByteOr[4] & 0x4000)) inBuff[4] &= ~0x08; 
    if(ByteAnd[4] & 0x2000) inBuff[4] |= 0x10; else if(!(ByteOr[4] & 0x2000)) inBuff[4] &= ~0x10; 
    if(ByteAnd[4] & 0x1000) inBuff[4] |= 0x20; else if(!(ByteOr[4] & 0x1000)) inBuff[4] &= ~0x20; 
    if(ByteAnd[4] & 0x0800) inBuff[4] |= 0x40; else if(!(ByteOr[4] & 0x0800)) inBuff[4] &= ~0x40; 
    if(ByteAnd[4] & 0x0400) inBuff[4] |= 0x80; else if(!(ByteOr[4] & 0x0400)) inBuff[4] &= ~0x80;

    /* BX9 ~ BX16 */
    if(ByteAnd[4] & 0x0200) inBuff[5] |= 0x01; else if(!(ByteOr[4] & 0x0200)) inBuff[5] &= ~0x01; 
    if(ByteAnd[4] & 0x0100) inBuff[5] |= 0x02; else if(!(ByteOr[4] & 0x0100)) inBuff[5] &= ~0x02; 
    if(ByteAnd[4] & 0x0080) inBuff[5] |= 0x04; else if(!(ByteOr[4] & 0x0080)) inBuff[5] &= ~0x04; 
    if(ByteAnd[1] & 0x0004) inBuff[5] |= 0x08; else if(!(ByteOr[1] & 0x0004)) inBuff[5] &= ~0x08; 
    if(ByteAnd[1] & 0x0002) inBuff[5] |= 0x10; else if(!(ByteOr[1] & 0x0002)) inBuff[5] &= ~0x10; 
    if(ByteAnd[1] & 0x0001) inBuff[5] |= 0x20; else if(!(ByteOr[1] & 0x0001)) inBuff[5] &= ~0x20; 
    if(ByteAnd[2] & 0x0020) inBuff[5] |= 0x40; else if(!(ByteOr[2] & 0x0020)) inBuff[5] &= ~0x40; 
    if(ByteAnd[2] & 0x0010) inBuff[5] |= 0x80; else if(!(ByteOr[2] & 0x0010)) inBuff[5] &= ~0x80;

    /* BX17 */
    if(ByteAnd[0] & 0x0010) inBuff[6] |= 0x01; else if(!(ByteOr[0] & 0x0010)) inBuff[6] &= ~0x01;
     
    
    /* SF_RL_DRV_FB */
    if(ByteAnd[4] & 0x0004) inBuff[7] |= 0x01; else if(!(ByteOr[4] & 0x0004)) inBuff[7] &= ~0x01; 
    /* SF_PWR_FB_CPU */
    if(ByteAnd[4] & 0x0001) inBuff[7] |= 0x02; else if(!(ByteOr[4] & 0x0001)) inBuff[7] &= ~0x02; 
    /* SF_RL_FB */
    if(ByteAnd[4] & 0x0040) inBuff[7] |= 0x04; else if(!(ByteOr[4] & 0x0040)) inBuff[7] &= ~0x04; 
    /* SF_RL1_FB_CPU2 */
    if(ByteAnd[2] & 0x0004) inBuff[7] |= 0x08; else if(!(ByteOr[2] & 0x0004)) inBuff[7] &= ~0x08; 
    /* AUX_FB */
    if(ByteAnd[4] & 0x0010) inBuff[7] |= 0x10; else if(!(ByteOr[4] & 0x0010)) inBuff[7] &= ~0x10; 
    /* PLUSE_OUTPUT_FB */
    if(ByteAnd[2] & 0x0002) inBuff[7] |= 0x20; else if(!(ByteOr[2] & 0x0002)) inBuff[7] &= ~0x20; 
    
#endif    

}


/******************************  END OF FILE  *********************************/


