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
#include "crc16.h"
#include "delay.h"
#include "esc_error_process.h"
#include "esc.h"
#include "esc_comm_safety_dualcpu.h"
#include "can.h"
#include "ewdt.h"
#include "led.h"

#ifdef GEC_SF_MASTER
#include "mb85rcxx.h"
#include "usb_func.h"
#include "esc_record_data.h"
#endif

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

#ifdef GEC_SF_MASTER
/*******************************************************************************
* Function Name  : esc_para_init
* Description    : esc parameter initialization, set the default value.                 
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
   
        sys_data_write();
        
    }
}
#endif

/*******************************************************************************
* Function Name  : get_para_from_usb
* Description    : get esc parameter from usb.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void get_para_from_usb(void)
{
    u8 senddata[50],recvdata[50];
    u8 len = 0;
    
#ifdef GEC_SF_MASTER 
    USBH_Mass_Storage_Init();
    
    /* USB-stick undetected */
    /* 1. Message to CPU2 */
    senddata[0] = MESSAGE_TO_CPU;
    senddata[1] = USB_NOT_DETECTED;
    CPU_Exchange_Data(senddata, 2);//send
    CPU_Data_Check(recvdata, &len);
    
    /* 2. Message to Control */    
    senddata[0] = MESSAGE_TO_CONTROL;
    senddata[1] = USB_NOT_DETECTED;
    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, senddata, 2);
    
    /* cpu1 read parameters and check in power on */
    /* 2. Send parameters to CPU2 */
    for( u8 i = 0; i < 50; i++)
    {
        senddata[i] = Sys_Data[i];
    }    
    CPU_Exchange_Data(senddata, 50);//send
    CPU_Data_Check(recvdata, &len);
    
    /* 3. Message received from CPU2 */
    delay_ms(50);
    CPU_Exchange_Data(senddata, 2);
    CPU_Data_Check(recvdata, &len);//recv
    
    
    if( len == 0x02 && recvdata[0] == MESSAGE_TO_CPU )
    {
        if( recvdata[1] == PARAMETER_CORRECT )
        {
            /* 4. Save parameters into variables */
            esc_para_init();
        }
        else if( recvdata[1] == PARAMETER_ERROR )
        {        
            /* Error message. Abort parameter loading due to CRC fault in CPU2. 
            System remains in Init Fault. */
            EN_ERROR9 |= 0x01;
        }
    }
    
#else
    
    /* 1. Waiting for message from CPU1 to start parameter loading process */
    CPU_Exchange_Data(senddata, 2);
    CPU_Data_Check(recvdata, &len);//recv
    if( len == 0x02 && recvdata[0] == MESSAGE_TO_CPU )
    {
        if( recvdata[1] == USB_DETECTED )
        {
            /* CPU2 wait until unit restarts. */
            while(1)
            {
                LED_FLASH();                                                
                delay_ms(200);
                EWDT_TOOGLE();
                IWDG_ReloadCounter();
            }
        }
        else if( recvdata[1] == USB_NOT_DETECTED )
        {
            /* 2. Message received with parameters from CPU1 */
            CPU_Exchange_Data(senddata, 2);
            CPU_Data_Check(recvdata, &len);//recv
            
            /* 3. Check paremeters received, CRC16 is ok */
//            if( MB_CRC16( recvdata, len ))
            if( len != 50 )
            {
                /* Send error to CPU1. ¡°CPU2 parameters error¡± System remains in Init Fault. */
                senddata[0] = MESSAGE_TO_CPU;
                senddata[1] = PARAMETER_ERROR;
            }
            else
            {
                /* 4. Save parameters into variables */
                for( u8 i = 0; i < len - 2; i++)
                {
                    Sys_Data[i] = recvdata[i];
                }  
                
                senddata[0] = MESSAGE_TO_CPU;
                senddata[1] = PARAMETER_CORRECT; 
            }
        }
    }
    else
    {
        /* para init error */
    }

    
    /* 5. Send confirmation to CPU1 or Send error to CPU1 */  
    CPU_Exchange_Data(senddata, 2);//send
    CPU_Data_Check(recvdata, &len);    
    
    
    CPU_Exchange_Data(senddata, 2);
    
#endif
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
      
}



/******************************  END OF FILE  *********************************/






