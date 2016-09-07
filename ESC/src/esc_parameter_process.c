/*******************************************************************************
* File Name          : esc_parameter_process.c
* Author             : lison
* Version            : V1.0
* Date               : 06/12/2016
* Last modify date   : 09/06/2016
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
#include "fatfs.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Check_Error_Present_Memory(void);
static void get_para_from_usb(void);
#ifdef GEC_SF_MASTER
static void esc_para_init(void);
#endif


u8 ParaLoad = 0u;


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
static void esc_para_init(void)
{

    if(PARA_INIT != 0xff01u)
    {
        
        MOTOR_RPM = 1000u;
        MOTOR_PLUSE_PER_REV = 10u;
        UNDERSPEED_TIME = 5000u;
        HANDRAIL_MOTOR_PULSE = 230u;
        NOMINAL_SPEED = 500u;
        SSM_SHORTCIRCUIT_TIME = 750u;
        HR_FAULT_TIME = 10000u;
        ROLLER_HR_RADIUS = 50u; /*( 0.050 * 1000 );  mm */
        HR_PULSES_PER_REV = 2u;
        STEP_WIDTH = 400u;
        TANDEM_TYPE = 0u;       
        
        PARA_INIT = 0xff01u;
   
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
static u8 Send_State_Message(u8 board, u8 state, u8 buff[], u8 len)
{
    u8 senddata[120],recvdata[120];
    u8 i;
    u8 res = 0u;
    
    senddata[0] = board;
    senddata[1] = state;    
    
    /* Send the state to the CPU */
    if( board == MESSAGE_TO_CPU )
    {
        if( state == SEND_PARAMETER )
        {
            for( i = 0u; i < len; i++)
            {
                senddata[i + 2u] = buff[i];
            } 
            CPU_Exchange_Data(senddata, len + 2u);
            CPU_Data_Check(recvdata, &len);            
        }
        else if( state == RECEIVE_PARAMETER )
        {
            CPU_Exchange_Data(senddata, 2u);
            CPU_Data_Check(recvdata, &len); 
            
            for( i = 0u; i < len; i++)
            {
                buff[i] = recvdata[i];
            }    
            
            res = len;
        }
        else
        {
            CPU_Exchange_Data(senddata, 2u);
            CPU_Data_Check(recvdata, &len);                         
        }
    }
    /* Send the state to the cb board */
    else if( board == MESSAGE_TO_CONTROL )
    {
        if( state == SEND_PARAMETER )
        {
            for( i = 0u; i < len; i++)
            {
                senddata[i + 2u] = buff[i];
            } 
            BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, senddata, len + 2u);            
        }
        else
        {        
            BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, senddata, 2u);
        }
    }
    else
    {}
    
    return res;
}

/*******************************************************************************
* Function Name  : get_para_from_usb
* Description    : get esc parameter from usb.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void get_para_from_usb(void)
{
    
    u8 len = 0u;   
    
#ifdef GEC_SF_MASTER 
    
    u8 recvdata[10];
    u8 paradata[120];
    u8 i;
    
    USBH_Mass_Storage_Init();
    
    /* USB-stick undetected */
    /* 1. Message to CPU2 */
    Send_State_Message( MESSAGE_TO_CPU, USB_NOT_DETECTED, NULL, 0u );
    
    /* 2. Message to Control */    
    Send_State_Message( MESSAGE_TO_CONTROL, USB_NOT_DETECTED, NULL, 0u );
    
    /* 3. cpu1 read parameters and check in power on */
    if(!fram_data_read(ESC_PARA_ADR, ESC_PARA_NUM, paradata))
    {
        /* 4. Check crc16 is it ok */
        if( MB_CRC16( paradata, ESC_PARA_NUM ))
        {
            /* Error message. Abort parameter loading. System remains in Init Fault. */
            EN_ERROR9 |= 0x01u;
            
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
        EN_ERROR9 |= 0x01u;
        
        ESC_Init_Fault();    
    }
          
    
    /* 6. Message received from CPU2 */
    delay_ms(50u);
    len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0u );   
    
    if( (len == 0x02u) && (recvdata[0] == MESSAGE_TO_CPU) )
    {
        if( recvdata[1] == PARAMETER_CORRECT )
        {
            /* 7. Save parameters into variables */
            for( i = 0u; i < len - 2u; i++)
            {
                Modbuff[1100u + i] = paradata[i];
            } 
            
            delay_ms(5u);
            /* 8. Parametrization Loading Finished. Send Finish message to CPU2 */
            Send_State_Message( MESSAGE_TO_CPU, PARAMETER_LOADED_FINSH, NULL, 0u );
        }
        else if( recvdata[1] == PARAMETER_ERROR )
        {        
            /* Error message. Abort parameter loading due to CRC fault in CPU2. 
            System remains in Init Fault. */
            EN_ERROR9 |= 0x01u;
            
            ESC_Init_Fault();
        }
        else
        {}
    }
    else
    {
        /* Message received timeout. Send error to CPU2. Restart required. 
        System remains in Init Fault. */
        EN_ERROR9 |= 0x01u;
        
        ESC_Init_Fault();
    }
    
#else
    
    u8 senddata[5];
    u8 recvdata[120];
    u8 i;
    
    /* 1. Waiting for message from CPU1 to start parameter loading process */
    len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0u ); 

    if( (len == 0x02u) && (recvdata[0] == MESSAGE_TO_CPU) )
    {
        if( recvdata[1] == USB_DETECTED )
        {
            /* CPU2 wait until unit restarts. */
            while(1)
            {
                LED_FLASH();                                                
                delay_ms(200u);
                EWDT_TOOGLE();
                IWDG_ReloadCounter();
            }
        }
        else if( recvdata[1] == USB_NOT_DETECTED )
        {
            /* 2. Message received with parameters from CPU1 */
            len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0u ); 
            
            /* 3. Check paremeters received, CRC16 is ok */
            if( (recvdata[0] == MESSAGE_TO_CPU) && (recvdata[1] == SEND_PARAMETER) )
            {
                if( MB_CRC16( &recvdata[2], ((u16)len - 2u) ))
                {
                    /* Send error to CPU1. ¡°CPU2 parameters error¡± System remains in Init Fault. */
                    Send_State_Message( MESSAGE_TO_CPU, PARAMETER_ERROR, NULL, 0u );
                    
                    ESC_Init_Fault();
                }
                else
                {
                    /* 4. Save parameters into variables */
                    for( i = 0u; i < len - 4u; i++)
                    {
                        Modbuff[1100u + i] = recvdata[i + 2u];
                    }  
                    
                    /* 5. Send confirmation to CPU1 or Send error to CPU1 */  
                    Send_State_Message( MESSAGE_TO_CPU, PARAMETER_CORRECT, NULL, 0u );
                    
                    /* Received Finish message from CPU1 */  
                    len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0u ); 
                    if( (recvdata[0] == MESSAGE_TO_CPU) && (recvdata[1] == PARAMETER_LOADED_FINSH) )
                    {
                        /* SPI Slave Send */
                        CPU_Exchange_Data(senddata, 2u);
                    }
                    else
                    {
                        EN_ERROR9 |= 0x01u;
                        
                        ESC_Init_Fault(); 
                    }
                }
            }
            else
            {
                /* Message received timeout. Send error to CPU1. ¡°CPU2 parameters 
                communication error. Timeout message from CPU1¡± */
                EN_ERROR9 |= 0x01u;
                
                ESC_Init_Fault();                
            }
        }
        else
        {}
    }
    else
    {
        /* para init error */
        EN_ERROR9 |= 0x01u;
        
        ESC_Init_Fault();
    }


    
#endif
}

/*******************************************************************************
* Function Name  : USB_LoadParameter
* Description    : Usb main program, load parameter.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : 0:ok  1:error
*******************************************************************************/ 
#ifdef GEC_SF_MASTER
int USB_LoadParameter(void)
{ 

      int res = 0;
      u8 parabuffer[100];
      u16 filelen = 0u;
            
      
      LED_ON();
      ParaLoad |= USB_DETECTED;
      
      /* USB-stick detected */     
      /* 1. Message to CPU2 */
      Send_State_Message( MESSAGE_TO_CPU, USB_DETECTED, NULL, 0u );
      
      /* 2. Message to Control */
      Send_State_Message( MESSAGE_TO_CONTROL, USB_DETECTED, NULL, 0u );
      
      /* usb load parameter start -------------------------------------------*/
      /* 1. S0001 file present */
      if(!isFileExist("0:S0001.bin"))
      {
          
          Send_State_Message( MESSAGE_TO_CONTROL, SAFETY_PARAMETER_EXIST, NULL, 0u );
          
          /* 2. Read parameters from usb stick to buffer */
          filelen = ReadFile( "0:S0001.bin", parabuffer );
          
          /* 3. decrypt */
          
          /* 4. Check crc16 is it ok */
          if( MB_CRC16( parabuffer, filelen ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR9 |= 0x01u;
              
              Send_State_Message( MESSAGE_TO_CONTROL, PARAMETER_ERROR, NULL, 0u );
              
              ESC_Init_Fault();
          }
          else
          {
              /* 5. Store the parameters in the fram */
              fram_data_write(ESC_PARA_ADR, ESC_PARA_NUM, parabuffer);
              
              ParaLoad |= SAFETY_PARAMETER_LOADED;
          }
          
      }
      else
      {
          Send_State_Message( MESSAGE_TO_CONTROL, SAFETY_PARAMETER_NOT_EXIST, NULL, 0u );
      }

      /* 1. C0001 file present */
      if(!isFileExist("0:C0001.bin"))
      {
          
          Send_State_Message( MESSAGE_TO_CONTROL, CONTROL_PARAMETER_EXIST, NULL, 0u );
          
          /* 2. Read parameters from usb stick to buffer */
          filelen = ReadFile( "0:C0001.bin", parabuffer );
          
          /* 3. decrypt */
          
          /* 4. Check crc16 is it ok */
          if( MB_CRC16( parabuffer, filelen ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR9 |= 0x01u;   
              
              Send_State_Message( MESSAGE_TO_CONTROL, PARAMETER_ERROR, NULL, 0u );
              
              ESC_Init_Fault();
          }
          else
          {         
              /* 5. Send the parameters to the cb board */
              Send_State_Message( MESSAGE_TO_CONTROL, SEND_PARAMETER, parabuffer, (u8)filelen );
              
              ParaLoad |= CONTROL_PARAMETER_LOADED;
          }
                      
      }   
      else
      {
          Send_State_Message( MESSAGE_TO_CONTROL, CONTROL_PARAMETER_NOT_EXIST, NULL, 0u );
      }
      /* usb load parameter finish -------------------------------------------*/ 
      

      return res;
}
#endif

/*******************************************************************************
* Function Name  : ParametersLoading
* Description    : esc get parameter.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void ParametersLoading(void)
{
    if( testmode == 0u )
    {       
        /* for test */
#ifdef GEC_SF_MASTER 
        esc_para_init();
#else

#endif
        get_para_from_usb();
    }
}



/******************************  END OF FILE  *********************************/






