/*******************************************************************************
* File Name          : esc_parameter_process.c
* Author             : lison
* Version            : V1.0
* Date               : 06/12/2016
* Last modify date   : 11/22/2016
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
static void get_para_from_usb(void);
static void esc_para_init(void);
static void esc_para_check(void);

u8 ParaLoad = 0u;
u8 DIAGNOSTIC = 0u;

SFPara SFParameterData;
CBParaInSF CBParameterInSafety;

/*******************************************************************************
* Function Name  : esc_para_init
* Description    : esc parameter initialization, set the default value.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void esc_para_init(void)
{
    MOTOR_RPM = 1000u;
    NOMINAL_SPEED = 50u;
    MOTOR_PULSE_PER_REV = 10u;
    STEP_WIDTH = 405u;
    END_SAFETY_STRING_FAULT_TYPE = 0u;
    KEY_MINIMUM_TIME = 800u;        
    UNDERSPEED_TIME = 5000u;
    TANDEM = 0u;   
    ROLLER_HR_RADIUS = 49u; 
    HR_PULSES_PER_REV = 2u;
    HR_FAULT_TIME = 20u;
    UP_DOWN_ALLOWED = 0u;
    SPEED_SENSOR_INSTALLATION = 0u;
    
    CONTACTORS_TIMEOUT = 100u;
    DRIVE_CHAIN_DELAY = 500u;
    
    AUX_BRAKE_ENABLE = 1u;
    CAPACITOR_TIME_MEASUREMENT = 1u;    
    
    DIAGNOSTIC_BOARD_L2_QUANTITY = 2u;
    DIAGNOSTIC_BOARD_L1_QUANTITY = 0u;
    
    if(( DIAGNOSTIC_BOARD_L1_QUANTITY != 0u ) && ( DIAGNOSTIC_BOARD_L2_QUANTITY == 0u ))
    {
        DIAGNOSTIC = DIAGNOSTIC_BOARD_1;
    }
    else if(( DIAGNOSTIC_BOARD_L1_QUANTITY == 0u ) && ( DIAGNOSTIC_BOARD_L2_QUANTITY != 0u ))
    {
        DIAGNOSTIC = DIAGNOSTIC_BOARD_2;
    }    
    else
    {
        DIAGNOSTIC = NO_DIAGNOSTIC_BOARD;
    }        
    
    
#ifdef GEC_SF_MASTER   
    /*
    fram_data_write(ESC_PARA_ADR, ESC_PARA_NUM, (u8*)&SFParameterData);
    fram_data_write(ESC_BACKUP_ADR + ESC_PARA_ADR, ESC_PARA_NUM, (u8*)&SFParameterData);
    */
#endif        
        
}

/*******************************************************************************
* Function Name  : esc_para_check
* Description    :                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void esc_para_check(void)
{
    if(!(( MOTOR_RPM <= MOTOR_RPM_MIN ) && ( MOTOR_RPM >= MOTOR_RPM_MAX )))
    {
        /* Parameter error F385 */
        EN_ERROR49 |= 0x02u;
    }
    
    if(!(( NOMINAL_SPEED <= NOMINAL_SPEED_MIN ) && ( NOMINAL_SPEED >= NOMINAL_SPEED_MAX ))) 
    {
        /* Parameter error F385 */
        EN_ERROR49 |= 0x02u;
    }    
    
    if(!((MOTOR_PULSE_PER_REV <= MOTOR_PLUSE_PER_REV_MIN ) && ( MOTOR_PULSE_PER_REV >= MOTOR_PLUSE_PER_REV_MAX )))
    {
        /* Parameter error F385 */
        EN_ERROR49 |= 0x02u;
    }    
}



/*******************************************************************************
* Function Name  : Send_State_Message
* Description    : Send state to control or CPU2.
*                  
* Input          : board: CPU or control board.
*                  state: load the parameter state.
* Output         : None
* Return         : None
*******************************************************************************/ 
static u16 Send_State_Message(u8 board, u8 state, u8 buff[], u16 len)
{
    u8 senddata[500],recvdata[500];
    u16 i;
    u16 res = 0u;
    
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
            CPU_Data_Check(recvdata, &len, 2000000u );            
        }
        else if( state == RECEIVE_PARAMETER )
        {
            CPU_Exchange_Data(senddata, 2u);
            CPU_Data_Check(recvdata, &len, 2000000u ); 
            
            for( i = 0u; i < len; i++)
            {
                buff[i] = recvdata[i];
            }    
            
            res = len;
        }
        else
        {
            CPU_Exchange_Data(senddata, 2u);
            CPU_Data_Check(recvdata, &len, 2000000u);                         
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
            /*BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, senddata, len + 2u);*/
        }
        else
        {        
            /*BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, senddata, 2u);*/
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
    u16 len = 0u;   
    u8 *sfpara = (u8*)&SFParameterData;
    
#ifdef GEC_SF_MASTER 
    
    u8 recvdata[10];
    u8 paradata[500];   
    u16 i;
    
    USBH_Mass_Storage_Init();
    
    /* USB-stick undetected */
    /* 1. Message to CPU2 */
    Send_State_Message( MESSAGE_TO_CPU, USB_NOT_DETECTED, NULL, 0u );
    
    /* 2. Message to Control */    
    Send_State_Message( MESSAGE_TO_CONTROL, USB_NOT_DETECTED, NULL, 0u );
    
    /* 3. cpu1 read parameters and check in power on */
    if(!fram_data_read(ESC_PARA_ADR, ESC_PARA_NUM, paradata))
    {
        /* 4. Check crc32 is it ok */
        if( MB_CRC32( paradata, ESC_PARA_NUM, PARAMETER_POLYNOMIALS ))
        {
            /* Error message. Abort parameter loading. System remains in Init Fault. */
            /* Parameter error F385 */
            EN_ERROR49 |= 0x02u;
            g_u16ParameterLoadingError = 1u;
        }
        else
        {
            /* 5. Send parameters to CPU2 */
            Send_State_Message( MESSAGE_TO_CPU, SEND_PARAMETER, paradata, ESC_PARA_NUM );
        }
    }
    else
    {
        /* Parameter error F385 */
        EN_ERROR49 |= 0x02u;
        g_u16ParameterLoadingError = 1u;   
    }
          
    /* if no fault, continue */
    if( g_u16ParameterLoadingError == 0u )
    {
        
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
                    /*sfpara[i] = paradata[i];*/
                } 
                memcpy(sfpara,&paradata,ESC_PARA_NUM);
                
                esc_para_check();
                
                delay_ms(5u);
                /* 8. Parametrization Loading Finished. Send Finish message to CPU2 */
                Send_State_Message( MESSAGE_TO_CPU, PARAMETER_LOADED_FINSH, NULL, 0u );
            }
            else if( recvdata[1] == PARAMETER_ERROR )
            {        
                /* Error message. Abort parameter loading due to CRC fault in CPU2. 
                System remains in Init Fault. */
                EN_ERROR49 |= 0x02u;
                
                g_u16ParameterLoadingError = 1u;   
            }
            else
            {}
        }
        else
        {
            /* Message received timeout. Send error to CPU2. Restart required. 
            System remains in Init Fault. */
            EN_ERROR49 |= 0x02u;
            
            g_u16ParameterLoadingError = 1u;   
        }
    }
    
#else
    
    u8 senddata[5];
    u8 recvdata[500];
    u16 i;
    
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
        else
        {  
            if( recvdata[1] == USB_NOT_DETECTED )
            {
                /* 2. Message received with parameters from CPU1 */
                len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0u ); 
                
                /* 3. Check paremeters received, CRC32 is ok */
                if( (recvdata[0] == MESSAGE_TO_CPU) && (recvdata[1] == SEND_PARAMETER) )
                {
                    if( MB_CRC32( &recvdata[2], ((u16)len - 2u), PARAMETER_POLYNOMIALS ))
                    {
                        /* Send error to CPU1. ¡°CPU2 parameters error¡± System remains in Init Fault. */
                        Send_State_Message( MESSAGE_TO_CPU, PARAMETER_ERROR, NULL, 0u );
                        
                        g_u16ParameterLoadingError = 1u;   
                    }
                    else
                    {
                        /* 4. Save parameters into variables */
                        for( i = 0u; i < len - 4u; i++)
                        {
                            /*ParameterData[i] = recvdata[i + 2u];*/
                        }
                        memcpy(sfpara,&recvdata,ESC_SF_PARAMETER_DATA_LEN);
                        
                        /* 5. Send confirmation to CPU1 or Send error to CPU1 */  
                        Send_State_Message( MESSAGE_TO_CPU, PARAMETER_CORRECT, NULL, 0u );
                        
                        /* Received Finish message from CPU1 */  
                        len = Send_State_Message( MESSAGE_TO_CPU, RECEIVE_PARAMETER, recvdata, 0u ); 
                        if( (recvdata[0] == MESSAGE_TO_CPU) && (recvdata[1] == PARAMETER_LOADED_FINSH) )
                        {
                            /* SPI Slave Send */
                            /*CPU_Exchange_Data(senddata, 2u);*/
                        }
                        else
                        {
                            EN_ERROR49 |= 0x02u;
                            
                            g_u16ParameterLoadingError = 1u;    
                        }
                    }
                }
                else
                {
                    /* Message received timeout. Send error to CPU1. ¡°CPU2 parameters 
                    communication error. Timeout message from CPU1¡± */
                    EN_ERROR49 |= 0x02u;
                    
                    g_u16ParameterLoadingError = 1u;                
                }
            }
        }
    }
    else
    {
        /* para init error */
        EN_ERROR49 |= 0x02u;
        
        g_u16ParameterLoadingError = 1u;  
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
      u8 parabuffer[500];
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
          
          /* 4. Check crc32 is it ok */
          if( MB_CRC32( parabuffer, filelen, PARAMETER_POLYNOMIALS ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR49 |= 0x02u;
              
              Send_State_Message( MESSAGE_TO_CONTROL, PARAMETER_ERROR, NULL, 0u );
              
              g_u16ParameterLoadingError = 1u;  
          }
          else
          {
              /*if(( filelen - 4u ) == ESC_PARA_NUM )*/
              
              /* 5. Store the parameters in the fram */
              fram_data_write(ESC_PARA_ADR, filelen, parabuffer);
              fram_data_write(ESC_BACKUP_ADR + ESC_PARA_ADR, filelen, parabuffer);
              
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
          if( MB_CRC32( parabuffer, filelen, PARAMETER_POLYNOMIALS ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR49 |= 0x02u;   
              
              Send_State_Message( MESSAGE_TO_CONTROL, PARAMETER_ERROR, NULL, 0u );
              
              g_u16ParameterLoadingError = 1u;  
          }
          else
          {         
              /* 5. Send the parameters to the cb board */
              Send_State_Message( MESSAGE_TO_CONTROL, SEND_PARAMETER, parabuffer, filelen );
              
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
        esc_para_init();

        /*get_para_from_usb();*/
        
    }
    else
    {
        get_para_from_usb();
    }
}



/******************************  END OF FILE  *********************************/






