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

    static u16 PinValue[3][5],ByteAnd[5],ByteOr[5],read_pin_cnt = 0;
    u16 i; 
    
    /* first, clear the data */
    for(i =0; i < 8; i++)
    {
        inBuff[i] = 0;
    }
    
    read_pin_cnt++;  
    if(read_pin_cnt > 2) read_pin_cnt = 0;  
    
    /* Read I/O port */                      
    PinValue[read_pin_cnt][0] = GPIO_ReadInputData(GPIOA);
    PinValue[read_pin_cnt][1] = GPIO_ReadInputData(GPIOB);
    PinValue[read_pin_cnt][2] = GPIO_ReadInputData(GPIOC);
    PinValue[read_pin_cnt][3] = GPIO_ReadInputData(GPIOD);
    PinValue[read_pin_cnt][4] = GPIO_ReadInputData(GPIOE);
    
    for(i=0;i<5;i++)
    {   
        ByteAnd[i] = PinValue[0][i] & PinValue[1][i] & PinValue[2][i];
        ByteOr[i] = PinValue[0][i] | PinValue[1][i] | PinValue[2][i]; 
    }
    
#ifdef GEC_SF_MASTER
    
    /* INA1 ~ INA8 */
    if(ByteAnd[1] & 0x0080) inBuff[0] &= ~0x01; else if(!(ByteOr[1] & 0x0080)) inBuff[0] |= 0x01; 
    if(ByteAnd[1] & 0x0040) inBuff[0] &= ~0x02; else if(!(ByteOr[1] & 0x0040)) inBuff[0] |= 0x02; 
    if(ByteAnd[4] & 0x0001) inBuff[0] &= ~0x04; else if(!(ByteOr[4] & 0x0001)) inBuff[0] |= 0x04; 
    if(ByteAnd[4] & 0x0002) inBuff[0] &= ~0x08; else if(!(ByteOr[4] & 0x0002)) inBuff[0] |= 0x08; 
    if(ByteAnd[1] & 0x0020) inBuff[0] &= ~0x10; else if(!(ByteOr[1] & 0x0020)) inBuff[0] |= 0x10; 
    if(ByteAnd[1] & 0x0010) inBuff[0] &= ~0x20; else if(!(ByteOr[1] & 0x0010)) inBuff[0] |= 0x20; 
    if(ByteAnd[1] & 0x0008) inBuff[0] &= ~0x40; else if(!(ByteOr[1] & 0x0008)) inBuff[0] |= 0x40; 
    if(ByteAnd[3] & 0x0004) inBuff[0] &= ~0x80; else if(!(ByteOr[3] & 0x0004)) inBuff[0] |= 0x80; 
    
    /* INA9 ~ INA16 */
    if(ByteAnd[3] & 0x0040) inBuff[1] |= 0x01; else if(!(ByteOr[3] & 0x0040)) inBuff[1] &= ~0x01; 
    if(ByteAnd[3] & 0x0020) inBuff[1] |= 0x02; else if(!(ByteOr[3] & 0x0020)) inBuff[1] &= ~0x02; 
    if(ByteAnd[3] & 0x0010) inBuff[1] |= 0x04; else if(!(ByteOr[3] & 0x0010)) inBuff[1] &= ~0x04; 
    if(ByteAnd[3] & 0x0008) inBuff[1] |= 0x08; else if(!(ByteOr[3] & 0x0008)) inBuff[1] &= ~0x08; 
    if(ByteAnd[3] & 0x0080) inBuff[1] |= 0x10; else if(!(ByteOr[3] & 0x0080)) inBuff[1] &= ~0x10; 
    if(ByteAnd[2] & 0x1000) inBuff[1] |= 0x20; else if(!(ByteOr[2] & 0x1000)) inBuff[1] &= ~0x20; 
    if(ByteAnd[2] & 0x0800) inBuff[1] |= 0x40; else if(!(ByteOr[2] & 0x0800)) inBuff[1] &= ~0x40; 
    if(ByteAnd[2] & 0x0400) inBuff[1] |= 0x80; else if(!(ByteOr[2] & 0x0400)) inBuff[1] &= ~0x80; 
    
    /* INA17 ~ INA24 */
    if(ByteAnd[0] & 0x8000) inBuff[2] |= 0x01; else if(!(ByteOr[0] & 0x0800)) inBuff[2] &= ~0x01; 
    if(ByteAnd[0] & 0x0200) inBuff[2] |= 0x02; else if(!(ByteOr[0] & 0x0200)) inBuff[2] &= ~0x02; 
    if(ByteAnd[0] & 0x0100) inBuff[2] |= 0x04; else if(!(ByteOr[0] & 0x0100)) inBuff[2] &= ~0x04; 
    if(ByteAnd[2] & 0x0200) inBuff[2] |= 0x08; else if(!(ByteOr[2] & 0x0200)) inBuff[2] &= ~0x08; 
    if(ByteAnd[2] & 0x0100) inBuff[2] |= 0x10; else if(!(ByteOr[2] & 0x0100)) inBuff[2] &= ~0x10; 
    if(ByteAnd[2] & 0x0080) inBuff[2] |= 0x20; else if(!(ByteOr[2] & 0x0080)) inBuff[2] &= ~0x20; 
    if(ByteAnd[4] & 0x0200) inBuff[2] |= 0x40; else if(!(ByteOr[4] & 0x0200)) inBuff[2] &= ~0x40; 
    if(ByteAnd[4] & 0x0100) inBuff[2] |= 0x80; else if(!(ByteOr[4] & 0x0100)) inBuff[2] &= ~0x80; 
    
    /* INA25 ~ INA29 */
    if(ByteAnd[4] & 0x0080) inBuff[3] |= 0x01; else if(!(ByteOr[4] & 0x0080)) inBuff[3] &= ~0x01; 
    if(ByteAnd[1] & 0x0004) inBuff[3] |= 0x02; else if(!(ByteOr[1] & 0x0004)) inBuff[3] &= ~0x02; 
    if(ByteAnd[1] & 0x0002) inBuff[3] |= 0x04; else if(!(ByteOr[1] & 0x0002)) inBuff[3] &= ~0x04; 
    if(ByteAnd[1] & 0x0001) inBuff[3] |= 0x08; else if(!(ByteOr[1] & 0x0001)) inBuff[3] &= ~0x08;
    if(ByteAnd[2] & 0x0020) inBuff[3] |= 0x10; else if(!(ByteOr[2] & 0x0020)) inBuff[3] &= ~0x10;
    
    /* AX1 ~ AX8 */
    if(ByteAnd[2] & 0x0040) inBuff[4] |= 0x01; else if(!(ByteOr[2] & 0x0040)) inBuff[4] &= ~0x01; 
    if(ByteAnd[3] & 0x8000) inBuff[4] |= 0x02; else if(!(ByteOr[3] & 0x8000)) inBuff[4] &= ~0x02; 
    if(ByteAnd[3] & 0x4000) inBuff[4] |= 0x04; else if(!(ByteOr[3] & 0x4000)) inBuff[4] &= ~0x04; 
    if(ByteAnd[3] & 0x2000) inBuff[4] |= 0x08; else if(!(ByteOr[3] & 0x2000)) inBuff[4] &= ~0x08; 
    if(ByteAnd[3] & 0x1000) inBuff[4] |= 0x10; else if(!(ByteOr[3] & 0x1000)) inBuff[4] &= ~0x10; 
    if(ByteAnd[3] & 0x0800) inBuff[4] |= 0x20; else if(!(ByteOr[3] & 0x0800)) inBuff[4] &= ~0x20; 
    if(ByteAnd[3] & 0x0400) inBuff[4] |= 0x40; else if(!(ByteOr[3] & 0x0400)) inBuff[4] &= ~0x40; 
    if(ByteAnd[3] & 0x0200) inBuff[4] |= 0x80; else if(!(ByteOr[3] & 0x0200)) inBuff[4] &= ~0x80;

    /* AX9 ~ AX16 */
    if(ByteAnd[3] & 0x0100) inBuff[5] |= 0x01; else if(!(ByteOr[3] & 0x0100)) inBuff[5] &= ~0x01; 
    if(ByteAnd[1] & 0x8000) inBuff[5] |= 0x02; else if(!(ByteOr[1] & 0x8000)) inBuff[5] &= ~0x02; 
    if(ByteAnd[1] & 0x4000) inBuff[5] |= 0x04; else if(!(ByteOr[1] & 0x4000)) inBuff[5] &= ~0x04; 
    if(ByteAnd[4] & 0x8000) inBuff[5] |= 0x08; else if(!(ByteOr[4] & 0x8000)) inBuff[5] &= ~0x08; 
    if(ByteAnd[4] & 0x4000) inBuff[5] |= 0x10; else if(!(ByteOr[4] & 0x4000)) inBuff[5] &= ~0x10; 
    if(ByteAnd[4] & 0x2000) inBuff[5] |= 0x20; else if(!(ByteOr[4] & 0x2000)) inBuff[5] &= ~0x20; 
    if(ByteAnd[4] & 0x1000) inBuff[5] |= 0x40; else if(!(ByteOr[4] & 0x1000)) inBuff[5] &= ~0x40; 
    if(ByteAnd[4] & 0x0800) inBuff[5] |= 0x80; else if(!(ByteOr[4] & 0x0800)) inBuff[5] &= ~0x80;

    /* AX17 */
    if(ByteAnd[4] & 0x0400) inBuff[6] |= 0x01; else if(!(ByteOr[4] & 0x0400)) inBuff[6] &= ~0x01;
    
//    if(PinValue[read_pin_cnt][1] & 0x0001) inBuff[6] &= ~0x10;  else    inBuff[6] |= 0x10;
//    if(PinValue[read_pin_cnt][1] & 0x0004) inBuff[6] &= ~0x20;  else    inBuff[6] |= 0x20; 
//    if(PinValue[read_pin_cnt][4] & 0x4000) inBuff[6] |= 0x40;   else    inBuff[6] &= ~0x40; 
//    if(PinValue[read_pin_cnt][1] & 0x0400) inBuff[6] |= 0x80;   else    inBuff[6] &= ~0x80; 
    
    /* SF_RL_DRV_FB */
    if(ByteAnd[4] & 0x0008) inBuff[7] |= 0x01; else if(!(ByteOr[4] & 0x0008)) inBuff[7] &= ~0x01; 
    /* SF_PWR_FB_CPU */
    if(ByteAnd[1] & 0x0100) inBuff[7] |= 0x02; else if(!(ByteOr[1] & 0x0100)) inBuff[7] &= ~0x02; 
    /* SF_RL_FB */
    if(ByteAnd[2] & 0x0004) inBuff[7] |= 0x04; else if(!(ByteOr[2] & 0x0004)) inBuff[7] &= ~0x04; 
    /* SF_RL2_FB_CPU1 */
    if(ByteAnd[4] & 0x0040) inBuff[7] |= 0x08; else if(!(ByteOr[4] & 0x0040)) inBuff[7] &= ~0x08; 
    /* AUX_FB */
    if(ByteAnd[2] & 0x0002) inBuff[7] |= 0x10; else if(!(ByteOr[2] & 0x0002)) inBuff[7] &= ~0x10; 

    
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
    
//    if(PinValue[read_pin_cnt][1] & 0x0001) inBuff[6] &= ~0x10;  else    inBuff[6] |= 0x10;
//    if(PinValue[read_pin_cnt][1] & 0x0004) inBuff[6] &= ~0x20;  else    inBuff[6] |= 0x20; 
//    if(PinValue[read_pin_cnt][4] & 0x4000) inBuff[6] |= 0x40;   else    inBuff[6] &= ~0x40; 
//    if(PinValue[read_pin_cnt][1] & 0x0400) inBuff[6] |= 0x80;   else    inBuff[6] &= ~0x80; 
    
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
    
#endif    

}


/******************************  END OF FILE  *********************************/


