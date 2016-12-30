/*******************************************************************************
* File Name          : esc_pga_input.c
* Author             : Tu/Lison
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 12/17/2016
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
static void pga_input_decode_item(u16 ucPgaNum, u8 ucLevel);  /* pga iput decode sub */



/*******************************************************************************
* Function Name  :  
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void pga_input_decode_item(u16 ucPgaNum, u8 ucLevel)
{
  u16 temp1,ucByte,ucBit;
  
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
    u32 *pConfigInput;
    
    /* The maximal ID <160, so 20 byte is enough */
    for(i=0u;i<50u;i++)
    {
        EscRtData. Cfg_Input_Mask[i] = 0u; 
        EscRtData. Cfg_Input_Level[i] = 0u;
    }  
    
    /* Safety board input */
    pConfigInput = (u32*)&EscRtData.SafetyInputData[0];
    for( i = 0u; i < 29u; i++ )
    {
        if( IO_PARAMETER_SAFETY_INPUT[i] )
        {
            pga_input_decode_item(IO_PARAMETER_SAFETY_INPUT[i], ((u8)(*pConfigInput >> i) & 0x01u));
        }
    }
    pConfigInput = (u32*)&EscRtData.SafetyInputData[4];
    for( i = 29u; i < 46u; i++ )
    {
        if( IO_PARAMETER_SAFETY_INPUT[i] )
        {
            pga_input_decode_item(IO_PARAMETER_SAFETY_INPUT[i], ((u8)(*pConfigInput >> (i-29u) ) & 0x01u));
        }
    }  
}

/******************************  END OF FILE  *********************************/