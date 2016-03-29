/*******************************************************************************
* File Name          : ewdt.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "delay.h"
#include "ewdt.h"
#include "esc_error_process.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u8 iwdg_check_flag = 0;

/*******************************************************************************
* Function Name  : EWDT_Drv_pin_config
* Description    : min:1.12 typ:1.6 max:2.4
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void EWDT_Drv_pin_config(void)
{
  
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE );
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
#ifdef GEC_SF_MASTER
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
#else
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
#endif
    GPIO_Init(GPIOA , &GPIO_InitStruct);

}

/*******************************************************************************
* Function Name  : write_bkp
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void write_bkp(u16 adr,u16 dat)
{
  PWR_BackupAccessCmd(ENABLE);
  BKP_WriteBackupRegister(adr, dat);   
  PWR_BackupAccessCmd(DISABLE);
}

/*******************************************************************************
* Function Name  : ext_WDT_check
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
u8 ext_WDT_check(void)
{
  u16 bkr_rst_flag=0;

  if( iwdg_check_flag == 0 )
  {
      delay_ms(10); 
      EWDT_TOOGLE();
      delay_ms(10);    
      EWDT_TOOGLE();
      delay_ms(10);    
      EWDT_TOOGLE();
        
      bkr_rst_flag = BKP_ReadBackupRegister(BKP_DR1);
      if(bkr_rst_flag == 0xfa01)
      {
    /** 软件复位 **/
//    if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET)
//    {
//        
//    }

        /** 引脚复位 **/
        if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != SET)
        {
          write_bkp(BKP_DR1, 0);  
          RCC_ClearFlag();   
          EN_ERROR_SYS1 |= 0x01;
        }   
      }  
      else
      {

        write_bkp(BKP_DR1, 0xfa01);   
        RCC_ClearFlag();
        
        delay_ms(2000);    
        
        EN_ERROR_SYS1 |= 0x01;
      }  
      
      write_bkp(BKP_DR1, 0);  
      
      if( EN_ERROR_SYS1&0x01 ) 
      {
          ESC_EWDT_Error_Process();
      }
      
  }
  
  return(0);
}  

/*******************************************************************************
* Function Name  : power_on_bsp_check
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void power_on_bsp_check(void)
{
  //外置看门狗检测，故障F81
  ext_WDT_check();
}


/******************************  END OF FILE  *********************************/


