/*******************************************************************************
* File Name          : esc_pga_input.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 11/07/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"
#include "bsp_iocfg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void pga_input_decode_item(u8 ucPgaNum, u8 ucLevel);  /* pga iput decode sub */


static u8 Cfg_ParaBuff[200];


/*******************************************************************************
* Function Name  :  
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void pga_input_decode_item(u8 ucPgaNum, u8 ucLevel)
{
  u8 temp1,ucByte,ucBit;
  
  temp1 = ucPgaNum-1u;
  
  ucByte = temp1>>3u;    /* byte address */
  ucBit = temp1%8u;      /* bit */
  
  EscRtData. Cfg_Input_Mask[ucByte] |= (1u<<ucBit); /* Function enable  */
            
  if(ucLevel) 
  {
    EscRtData. Cfg_Input_Level[ucByte] |= (1u<<ucBit); /* input 1 */
  }
}

/*******************************************************************************
*******************************************************************************/
void pga_input_decode(void)
{
  u8 i;

  for(i=0u;i<25u;i++)
  {
    EscRtData. Cfg_Input_Mask[i] = 0u; 
    EscRtData. Cfg_Input_Level[i] = 0u;
  }  
  
  Cfg_ParaBuff[0u] = 1u;    
  /* SF key input */
  Cfg_ParaBuff[10u] = 17u;    
  Cfg_ParaBuff[11u] = 18u;    
  
  
  if(Cfg_ParaBuff[0u]) { pga_input_decode_item(Cfg_ParaBuff[0u], EscRtData. SafetyInputData[3u]&0x04u);} else{} 
  
  /*SF X21~X22*/
  if(Cfg_ParaBuff[10u]) { pga_input_decode_item(Cfg_ParaBuff[10u], EscRtData. SafetyInputData[2u]&0x10u);} else{} 
  if(Cfg_ParaBuff[11u]) { pga_input_decode_item(Cfg_ParaBuff[11u], EscRtData. SafetyInputData[2u]&0x20u);} else{} 
  
}

/******************************  END OF FILE  *********************************/