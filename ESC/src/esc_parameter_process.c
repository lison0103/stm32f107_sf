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
#include "hw_test.h"

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
   
        fram_data_write(ESC_PARA_ADR, ESC_PARA_NUM, &Modbuff[1100]);
        
    }
}
#endif


/*******************************************************************************
* Function Name  : Send_State_Message
* Description    : Send state to control or CPU2.
*                  
* Input          : board: CPU or control board.
*                  state: load the parameter state.
* Output         : None
* Return         : None
*******************************************************************************/ 
u8 Send_State_Message(u8 board, u8 state, u8 *buff, u8 len)
{
    u8 senddata[120],recvdata[120];
    u8 i;
    
    senddata[0] = board;
    senddata[1] = state;    
    
    /* Send the state to the CPU */
    if( board == MESSAGE_TO_CPU )
    {
        if( state == SEND_PARAMETER )
        {
            for( i = 0; i < len; i++)
            {
                senddata[i + 2] = buff[i];
            } 
            CPU_Exchange_Data(senddata, len + 2);
            CPU_Data_Check(recvdata, &len);            
        }
        else if( state == RECEIVE_PARAMETER )
        {
            CPU_Exchange_Data(senddata, 2);
            CPU_Data_Check(recvdata, &len); 
            
            for( i = 0; i < len; i++)
            {
                buff[i] = recvdata[i];
            }    
            
            return len;
        }
        else
        {
            CPU_Exchange_Data(senddata, 2);
            CPU_Data_Check(recvdata, &len);                         
        }
    }
    /* Send the state to the cb board */
    else if( board == MESSAGE_TO_CONTROL )
    {
        if( state == SEND_PARAMETER )
        {
            for( i = 0; i < len; i++)
            {
                senddata[i + 2] = buff[i];
            } 
            BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, senddata, len + 2);            
        }
        else
        {        
            BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, senddata, 2);
        }
    }
    
    return 0;
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
    
    u8 len = 0;   
    
#ifdef GEC_SF_MASTER 
    
    u8 recvdata[10];
    u8 paradata[120];
    u8 i;
    
    USBH_Mass_Storage_Init();
    
    /* USB-stick undetected */
    /* 1. Message to CPU2 */
    Send_State_Message( MESSAGE_TO_CPU, USB_NOT_DETECTED, NULL, 0 );
    
    /* 2. Message to Control */    
    Send_State_Message( MESSAGE_TO_CONTROL, USB_NOT_DETECTED, NULL, 0 );
    
    /* 3. cpu1 read parameters and check in power on */
    if(!fram_data_read(ESC_PARA_ADR, ESC_PARA_NUM, paradata))
    {
        /* 4. Check crc16 is it ok */
        if( MB_CRC16( paradata, ESC_PARA_NUM ))
        {
            /* Error message. Abort parameter loading. System remains in Init Fault. */
            EN_ERROR9 |= 0x01;
            
            ESC_Init_Fault();
        }
        else
        {
            /* 5. Send parameters to CPU2 */
            Send_State_Message( MESSAGE_TO_CPU, SEND_PARAMETER, paradata, ESC_PARA_NUM );
        }
    }
    else
    {
        EN_ERROR9 |= 0x01;
        
        ESC_Init_Fault();    
    }
          
    
    /* 6. Message received from CPU2 */
    delay_ms(50);
    len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0 );   
    
    if( len == 0x02 && recvdata[0] == MESSAGE_TO_CPU )
    {
        if( recvdata[1] == PARAMETER_CORRECT )
        {
            /* 7. Save parameters into variables */
            for( i = 0; i < len - 2; i++)
            {
                Sys_Data[i] = paradata[i];
            } 
            
            /* 8. Parametrization Loading Finished. Send Finish message to CPU2 */
            Send_State_Message( MESSAGE_TO_CPU, PARAMETER_LOADED_FINSH, NULL, 0 );
        }
        else if( recvdata[1] == PARAMETER_ERROR )
        {        
            /* Error message. Abort parameter loading due to CRC fault in CPU2. 
            System remains in Init Fault. */
            EN_ERROR9 |= 0x01;
            
            ESC_Init_Fault();
        }
    }
    else
    {
        /* Message received timeout. Send error to CPU2. Restart required. 
        System remains in Init Fault. */
        EN_ERROR9 |= 0x01;
        
        ESC_Init_Fault();
    }
    
#else
    
    u8 senddata[5];
    u8 recvdata[120];
    u8 i;
    
    /* 1. Waiting for message from CPU1 to start parameter loading process */
    len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0 ); 

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
            len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0 ); 
            
            /* 3. Check paremeters received, CRC16 is ok */
            if( recvdata[0] == MESSAGE_TO_CPU && recvdata[1] == SEND_PARAMETER )
            {
                if( MB_CRC16( &recvdata[2], (len - 2) ))
                {
                    /* Send error to CPU1. ¡°CPU2 parameters error¡± System remains in Init Fault. */
                    Send_State_Message( MESSAGE_TO_CPU, PARAMETER_ERROR, NULL, 0 );
                    
                    ESC_Init_Fault();
                }
                else
                {
                    /* 4. Save parameters into variables */
                    for( i = 0; i < len - 4; i++)
                    {
                        Sys_Data[i] = recvdata[i + 2];
                    }  
                    
                    /* 5. Send confirmation to CPU1 or Send error to CPU1 */  
                    Send_State_Message( MESSAGE_TO_CPU, PARAMETER_CORRECT, NULL, 0 );
                    
                    /* Received Finish message from CPU1 */  
                    len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0 ); 
                    if( recvdata[0] == MESSAGE_TO_CPU && recvdata[1] == PARAMETER_LOADED_FINSH )
                    {
                        /* SPI Slave Send */
                        CPU_Exchange_Data(senddata, 2);
                    }
                    else
                    {
                        EN_ERROR9 |= 0x01;
                        
                        ESC_Init_Fault(); 
                    }
                }
            }
            else
            {
                /* Message received timeout. Send error to CPU1. ¡°CPU2 parameters 
                communication error. Timeout message from CPU1¡± */
                EN_ERROR9 |= 0x01;
                
                ESC_Init_Fault();                
            }
        }
    }
    else
    {
        /* para init error */
        EN_ERROR9 |= 0x01;
        
        ESC_Init_Fault();
    }


    
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
    if( testmode == 0 )
    {       
        /* for test */
#ifdef GEC_SF_MASTER 
        esc_para_init();
#else

#endif
//        get_para_from_usb();
    }
}



/******************************  END OF FILE  *********************************/






