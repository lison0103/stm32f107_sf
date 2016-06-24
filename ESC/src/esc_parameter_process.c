/*******************************************************************************
* File Name          : esc_parameter_process.c
* Author             : lison
* Version            : V1.0
* Date               : 06/12/2016
* Description        : This file contains esc parameter process.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_parameter_process.h"
#include "esc_record_data.h"
#include "mb85rcxx.h"
#include "crc16.h"
#include "delay.h"
#include "esc_error_process.h"
#include "esc.h"
#include "usb_func.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Check_Error_Present_Memory
* Description    : esc check error in memory.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Check_Error_Present_Memory(void)
{

}

/*******************************************************************************
* Function Name  : esc_para_init
* Description    : esc parameter initialization.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void esc_para_init(void)
{

    if(PARA_INIT != 0xff01)
    {
        
        MOTOR_RPM = 1000;
        MOTOR_PLUSE_PER_REV = 10;
        UNDERSPEED_TIME = 5000;
        DELAY_NO_PULSE_CHECKING = 1500;
        NOMINAL_SPEED = 500;
        SSM_SHORTCIRCUIT_TIME = 750;
        HR_FAULT_TIME = 10000;
        ROLLER_HR_RADIUS = ( 0.050 * 1000 ); //mm
        HR_PULSES_PER_REV = 2;
        STEP_WIDTH = 400;
        TANDEM_TYPE = 0;
        
        PARA_INIT = 0xff01;
        USB_LOOD_PARA = 0;
   
        sys_data_write();
        
    }
}

/*******************************************************************************
* Function Name  : get_para_from_usb
* Description    : get esc parameter from usb.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void get_para_from_usb(void)
{
    if( USB_LOOD_PARA == 0 )
    {
          USB_LOOD_PARA = 1;
          sys_data_write();
          USBH_Mass_Storage_Init();
    }
    else
    {
          USB_LOOD_PARA = 0;
          sys_data_write();
    }
}

/*******************************************************************************
* Function Name  : ParametersLoading
* Description    : esc get parameter.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void ParametersLoading(void)
{
      get_para_from_usb();
      esc_para_init();
}



/******************************  END OF FILE  *********************************/






