/*******************************************************************************
* File Name          : esc_parameter_process.c
* Author             : lison
* Version            : V1.0
* Date               : 06/12/2016
* Last modify date   : 12/17/2016
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
static void esc_para_check(void);

void EscParameterInit_TR1(void);

u8 ParaLoad = 0u;
u8 DIAGNOSTIC = 0u;

SFPara SFParameterData;
CBParaInSF CBParameterInSafety;
CBPara CBParameterData;


#ifdef GEC_SF_MASTER
u8 ParameterFile[2000] = {0u};
u8 ControlNeedInSafetyFile[45][8] = {0u};
u16 ControlFileLen = 0u;
u8 g_u8ParameterLoadingOK = 0u;
u8 g_u8ParameterSendToCPU2 = 0u;
#else
u8 ParameterSFFile[500] = {0u};
u8 ParameterCBNeedInSafetyFile[500] = {0u};
#endif

/*******************************************************************************
* Function Name  : esc_para_init
* Description    : esc parameter initialization, set the default value.                 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void EscParameterInit_TR1(void)
{
    u32 i;
    u8 crc[4];
    /**** Parameter init, for test *****/
    
    /**** Safety parameter ****/
#if 0
    MOTOR_RPM = 1500u;
#else
    MOTOR_RPM = 1000u;
#endif
    NOMINAL_SPEED = 50u;
    MOTOR_PULSE_PER_REV = 10u;
    MAIN_SHAFT_RPM = 1000u;
    MAIN_SHAFT_PULSE_PER_REV = 10u;
    STEP_WIDTH = 405u;
    END_SAFETY_STRING_FAULT_TYPE = 2u;
    KEY_MINIMUM_TIME = 800u;        
    UNDERSPEED_TIME = 5000u;
    TANDEM = 0u;   
    ROLLER_HR_RADIUS = 49u; 
    HR_PULSES_PER_REV = 2u;
    HR_FAULT_TIME = 15u;
    UP_DOWN_ALLOWED = 0u;
    SPEED_SENSOR_INSTALLATION = 0u;
    
    CONTACTORS_TIMEOUT = 2u; /* 2 second */
    DRIVE_CHAIN_DELAY = 500u;
    
    AUX_BRAKE_ENABLE = 1u;
    CAPACITOR_TIME_MEASUREMENT = 0u;    
    AUX_BRAKE_SUPERVISION_TIME = 200u;
    
    PULSE_SIGNALS_MINIMUM_LAG = 300u;
    
    DIAGNOSTIC_BOARD_L2_QUANTITY = 0u;
    
    CONTACTOR_FB1_MASK = 0x03u;
    CONTACTOR_FB2_MASK = 0x0cu;
    
    CONTACTOR_FEEDBACK_FILTER = 1000u;
    
    AUX_BRAKE_SUPERVISION_TIME = 4u;  /* 4s */
    
    RESET_FROM_INSPECTION_CONTROL = 1u;
      
    /**** Control need in safety parameter ****/
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
    UPPER_DIAG_SS_LENGTH = 0u;
    LOWER_DIAG_SS_LENGTH = 0u;
    INTERM1_DIAG_SS_LENGTH = 0u;
    INTERM2_DIAG_SS_LENGTH = 0u;
    
    /**** safety input parameter ****/
    IO_PARAMETER_SAFETY_INPUT[0u] = 1u;        
    IO_PARAMETER_SAFETY_INPUT[1u] = 2u;
    IO_PARAMETER_SAFETY_INPUT[6u] = 4u; 
    IO_PARAMETER_SAFETY_INPUT[7u] = 5u;     

    /* driver chain 1 */
    /*IO_PARAMETER_SAFETY_INPUT[14u] = 36u;*/
    
    /* Inspect un/down input */
    IO_PARAMETER_SAFETY_INPUT[18u] = 6u;    
    IO_PARAMETER_SAFETY_INPUT[19u] = 7u;   
    
    /* Normal un/down key input */
    IO_PARAMETER_SAFETY_INPUT[20u] = 26u;    
    IO_PARAMETER_SAFETY_INPUT[21u] = 27u;   
    
    /* coverplate input */
    IO_PARAMETER_SAFETY_INPUT[22u] = 8u;    
    IO_PARAMETER_SAFETY_INPUT[23u] = 9u;    
    IO_PARAMETER_SAFETY_INPUT[24u] = 10u;    
    IO_PARAMETER_SAFETY_INPUT[25u] = 11u;      
    
    IO_PARAMETER_SAFETY_INPUT[26u] = 34u;      
 
#ifdef GEC_SF_MASTER   
    STAR_DELTA_DELAY = 3u;
    
    /*Static IP ADDRESS*/
    IP_ADDRESS_BYTE1 = 10u;
    IP_ADDRESS_BYTE2 = 129u;
    IP_ADDRESS_BYTE3 = 199u;
    IP_ADDRESS_BYTE4 = 200u;
    
    /*NETMASK*/
    SUBNET_MASK_BYTE1 = 255u;
    SUBNET_MASK_BYTE2 = 255u;
    SUBNET_MASK_BYTE3 = 255u;
    SUBNET_MASK_BYTE4 = 0u;
    
    /*Gateway Address*/
    GATEWAY_BYTE1 = 10u;
    GATEWAY_BYTE2 = 129u;
    GATEWAY_BYTE3 = 199u;
    GATEWAY_BYTE4 = 2u; 
    
    i = MB_CRC32( (u8*)&SFParameterData, (ESC_SF_PARAMETER_DATA_LEN - 4u) , PARAMETER_POLYNOMIALS);
    
    crc[0] = (u8)(i >> 24u);
    crc[1] = (u8)(i >> 16u);     
    crc[2] = (u8)(i >> 8u);
    crc[3] = (u8)i;
    
    SFParameterData.CRC32 = *(u32*)&crc;
    
    i = MB_CRC32( (u8*)&CBParameterData, (ESC_CB_PARAMETER_DATA_LEN - 4u) , PARAMETER_POLYNOMIALS);
    
    crc[0] = (u8)(i >> 24u);
    crc[1] = (u8)(i >> 16u);     
    crc[2] = (u8)(i >> 8u);
    crc[3] = (u8)i;
    
    CBParameterData.CRC32 = *(u32*)&crc;    
    /*
    fram_data_write(ESC_PARA_ADR, ESC_PARA_NUM, (u8*)&SFParameterData);
    fram_data_write(ESC_BACKUP_ADR + ESC_PARA_ADR, ESC_PARA_NUM, (u8*)&SFParameterData);
    */
#endif               
}

void EscParameterInit(void)
{
    u32 i;
    u8 crc[4];
    /**** Parameter init, for test *****/
    
    /**** Safety parameter ****/
#if 0
    MOTOR_RPM = 1500u;
#else
    MOTOR_RPM = 1000u;
#endif
    NOMINAL_SPEED = 50u;
    MOTOR_PULSE_PER_REV = 10u;
    STEP_WIDTH = 405u;
    END_SAFETY_STRING_FAULT_TYPE = 2u;
    KEY_MINIMUM_TIME = 800u;        
    UNDERSPEED_TIME = 5000u;
    TANDEM = 0u;   
    ROLLER_HR_RADIUS = 49u; 
    HR_PULSES_PER_REV = 2u;
    HR_FAULT_TIME = 15u;
    UP_DOWN_ALLOWED = 0u;
    SPEED_SENSOR_INSTALLATION = 0u;
    
    CONTACTORS_TIMEOUT = 2u; /* 2 second */
    DRIVE_CHAIN_DELAY = 500u;
    
    AUX_BRAKE_ENABLE = 0u;
    CAPACITOR_TIME_MEASUREMENT = 0u;    
    
    PULSE_SIGNALS_MINIMUM_LAG = 300u;
    
    DIAGNOSTIC_BOARD_L2_QUANTITY = 0u;
    
    CONTACTOR_FB1_MASK = 0x03u;
    CONTACTOR_FB2_MASK = 0x0cu;
    
    CONTACTOR_FEEDBACK_FILTER = 1000u;
    
    AUX_BRAKE_SUPERVISION_TIME = 4u;  /* 4s */
      
    /**** Control need in safety parameter ****/
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
    UPPER_DIAG_SS_LENGTH = 0u;
    LOWER_DIAG_SS_LENGTH = 0u;
    INTERM1_DIAG_SS_LENGTH = 0u;
    INTERM2_DIAG_SS_LENGTH = 0u;
    
    /**** safety input parameter ****/
    IO_PARAMETER_SAFETY_INPUT[0u] = 1u;    

    /* Inspect un/down input */
    IO_PARAMETER_SAFETY_INPUT[18u] = 6u;    
    IO_PARAMETER_SAFETY_INPUT[19u] = 7u;   
    
    /* Normal un/down key input */
    IO_PARAMETER_SAFETY_INPUT[20u] = 26u;    
    IO_PARAMETER_SAFETY_INPUT[21u] = 27u;   
    
    /* coverplate input */
    IO_PARAMETER_SAFETY_INPUT[22u] = 8u;    
    IO_PARAMETER_SAFETY_INPUT[23u] = 9u;    
    IO_PARAMETER_SAFETY_INPUT[24u] = 10u;    
    IO_PARAMETER_SAFETY_INPUT[25u] = 11u;      
    
    IO_PARAMETER_SAFETY_INPUT[26u] = 34u;      
    
    
#ifdef GEC_SF_MASTER   
    STAR_DELTA_DELAY = 3u;
    
    /*Static IP ADDRESS*/
    IP_ADDRESS_BYTE1 = 10u;
    IP_ADDRESS_BYTE2 = 129u;
    IP_ADDRESS_BYTE3 = 199u;
    IP_ADDRESS_BYTE4 = 200u;
    
    /*NETMASK*/
    SUBNET_MASK_BYTE1 = 255u;
    SUBNET_MASK_BYTE2 = 255u;
    SUBNET_MASK_BYTE3 = 255u;
    SUBNET_MASK_BYTE4 = 0u;
    
    /*Gateway Address*/
    GATEWAY_BYTE1 = 10u;
    GATEWAY_BYTE2 = 129u;
    GATEWAY_BYTE3 = 199u;
    GATEWAY_BYTE4 = 2u; 
    
    i = MB_CRC32( (u8*)&SFParameterData, (ESC_SF_PARAMETER_DATA_LEN - 4u) , PARAMETER_POLYNOMIALS);
    
    crc[0] = (u8)(i >> 24u);
    crc[1] = (u8)(i >> 16u);     
    crc[2] = (u8)(i >> 8u);
    crc[3] = (u8)i;
    
    SFParameterData.CRC32 = *(u32*)&crc;
    
    i = MB_CRC32( (u8*)&CBParameterData, (ESC_CB_PARAMETER_DATA_LEN - 4u) , PARAMETER_POLYNOMIALS);
    
    crc[0] = (u8)(i >> 24u);
    crc[1] = (u8)(i >> 16u);     
    crc[2] = (u8)(i >> 8u);
    crc[3] = (u8)i;
    
    CBParameterData.CRC32 = *(u32*)&crc;    
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
      u8 parabuffer[ESC_PARA_NUM];
      u16 filelen = 0u;
            
      
      LED_ON();
      ParaLoad |= USB_DETECTED;
      g_u8ParameterLoading = 1u;
      
      /* usb load parameter start -------------------------------------------*/
      /* 1. S0001 file present */
      if(!isFileExist("0:S0001.bin"))
      {         
          /* 2. Read parameters from usb stick to buffer */
          filelen = ReadFile( "0:S0001.bin", parabuffer );
          
          /*if( filelen == ESC_PARA_NUM )*/
          
          /* 3. decrypt */
          
          /* 4. Check crc32 is it ok */
          if( MB_CRC32( parabuffer, ESC_PARA_NUM, PARAMETER_POLYNOMIALS ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR49 |= 0x02u;
              
              g_u16ParameterLoadingError = 1u;  
          }
          else
          {
              /* 5. Store the parameters in the fram */
              fram_data_write(ESC_PARA_ADR, ESC_PARA_NUM, parabuffer, PARAMETER_POLYNOMIALS);
              delay_ms(500u);
              fram_data_write(ESC_BACKUP_ADR + ESC_PARA_ADR, ESC_PARA_NUM, parabuffer, PARAMETER_POLYNOMIALS);
              
              ParaLoad |= SAFETY_PARAMETER_LOADED;
          }
      }

      /* 1. C0001 file present */
      if(!isFileExist("0:C0001.bin"))
      {
          /* 2. Read parameters from usb stick to buffer */
          ControlFileLen = ReadFile( "0:C0001.bin", ParameterFile );
          
          /* 3. decrypt */
          
          /* 4. Check crc16 is it ok */
          if( MB_CRC32( ParameterFile, ControlFileLen, PARAMETER_POLYNOMIALS ))
          {
              /* Error message. Abort parameter loading. System remains in Init Fault. */
              EN_ERROR49 |= 0x02u;   
              
              g_u16ParameterLoadingError = 1u;  
          }
          else
          {         
              ParaLoad |= CONTROL_PARAMETER_LOADED;
          }             
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
    u8 i,len;
    u16 *plen = (u16*)&EscRtData.ParameterData[0];
    static u16 stat_u16TimeParameterCheck = 0u;
    
    if( SfBase_EscState == ESC_INIT_STATE )
    {
#ifdef GEC_SF_MASTER     
        if( ParaLoad & USB_DETECTED )
        {    
            /* Power On with USB Stick */    
            if(( ParaLoad & 0x30u ) == 0x30u )
            {
                /* Read S0001.bin and C0001.bin ok */
                /* Message to CPU2 / Control */
                EscRtData.ParaStatus |= ( USB_DETECTED | SAFETY_PARAMETER_LOADED | CONTROL_PARAMETER_LOADED );
                
                /* Send finish */
                /* CPU1 wait for restart */ 
            }
            else
            {
                /* Message to CPU2 / Control */
                EscRtData.ParaStatus |= USB_DETECTED;
                
                /* CPU1 wait for restart */
            }
        }
        else
        {
            /* Power On without USB Stick */
            /* Message to CPU2 / Control */
            EscRtData.ParaStatus |= USB_NOT_DETECTED;
            
            /******* for test ********/
            /*g_u16ParameterLoadingError = 1u;
            g_u8ParameterLoadingFinish = 1u;*/
            
            if(( g_u16ParameterLoadingError == 0u ) && ( g_u8ParameterLoadingOK == 0u ))
            {
                /* cpu1 read parameters and check in power on */
                if(!fram_data_read(ESC_PARA_ADR, ESC_PARA_NUM, ParameterFile))
                {
                    memcpy((u8*)&SFParameterData,&ParameterFile,ESC_PARA_NUM);
                    
                    /* Check crc32 is it ok */
                    if( MB_CRC32( (u8*)&SFParameterData, ESC_PARA_NUM, PARAMETER_POLYNOMIALS ))
                    {
                        /* Error message. Abort parameter loading. System remains in Init Fault. */
                        /* Parameter error F385 */
                        EN_ERROR49 |= 0x02u;
                        g_u16ParameterLoadingError = 1u;
                    }
                    else
                    {
                        EscRtData.ParaCRC[0] = SFParameterData.CRC32;
                        g_u8ParameterLoadingOK = 1u;
                    }
                }
                else
                {
                    /* Parameter error F385 */
                    EN_ERROR49 |= 0x02u;
#if 0                    
                    g_u16ParameterLoadingError = 1u;   
#else
                    /* for test */
                    /* EscParameterInit(); */
                    EscParameterInit_TR1();
                    fram_data_write(ESC_PARA_ADR, ESC_PARA_NUM, (u8*)&SFParameterData, PARAMETER_POLYNOMIALS);
                    delay_ms(500u);
                    fram_data_write(ESC_BACKUP_ADR + ESC_PARA_ADR, ESC_PARA_NUM, (u8*)&SFParameterData, PARAMETER_POLYNOMIALS);
                    g_u8ParameterLoadingOK = 1u;
#endif                    
                }     
            }
            
            /*  Send parameters to Control */
            if( g_u8ParameterLoadingOK == 1u )
            {
                if( ParaDataToControl[0][3] == 1u )
                {
                    g_u8ParameterLoadingOK = 2u;
                }
            }
            /*  receive parameters from Control */
            else if( g_u8ParameterLoadingOK == 2u )
            {
                if(( g_u16ParameterLoadingError == 0u ) && ( ControlNeedInSafetyFile[0][3] == 1u ))
                {
                    memcpy((u8*)&CBParameterInSafety, &ControlNeedInSafetyFile[1][0], ESC_CB_PARAMETER_IN_SF_DATA_LEN);
                    
                    /* Check crc32 is it ok */
                    if( MB_CRC32( (u8*)&CBParameterInSafety, ESC_CB_PARAMETER_IN_SF_DATA_LEN, PARAMETER_POLYNOMIALS ))
                    {
                        /* Error message. Abort parameter loading. System remains in Init Fault. */
                        /* Parameter error F385 */
                        EN_ERROR49 |= 0x02u;
                        g_u16ParameterLoadingError = 1u;
                    }
                    else
                    {
                        EscRtData.ParaCRC[1] = CBParameterInSafety.CRC32;
                        g_u8ParameterLoadingOK = 3u;
                        for( i = 0u; i < 10u; i++ )
                        {
                            EscRtData.ParameterData[i] = 0u;
                        }
                    }
                }
            }            
            /*  Send parameters to CPU2 */
            else if( g_u8ParameterLoadingOK == 3u )
            {
                if( g_u8ParameterSendToCPU2 == 0u )
                {                
                    if( ESC_SF_PARAMETER_DATA_LEN == *plen )
                    {
                        EscRtData.ParaStatus |= SEND_S0001_PARAMETER_FINISH;
                        EscRtData.ParaStatus &= ~SEND_S0001_PARAMETER;
                        if( OmcEscRtData.ParaStatus & SEND_S0001_PARAMETER_FINISH )
                        {
                            *plen = 0u;
                            g_u8ParameterLoadingOK = 4u;
                            for( i = 0u; i < 10u; i++ )
                            {
                                EscRtData.ParameterData[i] = 0u;
                            }                        
                        }
                    }
                    else
                    {
                        EscRtData.ParaStatus |= SEND_S0001_PARAMETER;
                    }
                    
                    
                    /* send S001 */   
                    if( ESC_SF_PARAMETER_DATA_LEN > *plen)
                    {
                        g_u8ParameterSendToCPU2 = 1u;
                        if( ESC_SF_PARAMETER_DATA_LEN - *plen > 8u )
                        {
                            for( i = 0u; i < 8u; i++ )
                            {
                                EscRtData.ParameterData[i+2u] = ParameterFile[*plen + i];
                            }
                            *plen += 8u;
                        }
                        else
                        {
                            for( i = 0u; i < ESC_SF_PARAMETER_DATA_LEN - *plen; i++ )
                            {
                                EscRtData.ParameterData[i+2u] = ParameterFile[*plen + i];
                            }
                            *plen = ESC_SF_PARAMETER_DATA_LEN;
                        }                        
                    }  
                }
            }
            else if( g_u8ParameterLoadingOK == 4u )
            {
                if( g_u8ParameterSendToCPU2 == 0u )
                {                
                    if( ESC_CB_PARAMETER_IN_SF_DATA_LEN == *plen )
                    {
                        EscRtData.ParaStatus |= SEND_CS001_PARAMETER_FINISH;
                        EscRtData.ParaStatus &= ~SEND_CS001_PARAMETER;
                        if( OmcEscRtData.ParaStatus & SEND_CS001_PARAMETER_FINISH )
                        {
                            *plen = 0u;
                            g_u8ParameterLoadingOK = 5u;
                            for( i = 0u; i < 10u; i++ )
                            {
                                EscRtData.ParameterData[i] = 0u;
                            }
                        }
                    }
                    else
                    {
                        EscRtData.ParaStatus |= SEND_CS001_PARAMETER;
                    }
                    
                    
                    /* send CS001 */   
                    if( ESC_CB_PARAMETER_IN_SF_DATA_LEN > *plen)
                    {
                        g_u8ParameterSendToCPU2 = 1u;
                        if( ESC_CB_PARAMETER_IN_SF_DATA_LEN - *plen > 8u )
                        {
                            for( i = 0u; i < 8u; i++ )
                            {
                                EscRtData.ParameterData[i+2u] = ControlNeedInSafetyFile[(*plen/8u) + 1u][i];
                            }
                            *plen += 8u;
                        }
                        else
                        {
                            for( i = 0u; i < ESC_CB_PARAMETER_IN_SF_DATA_LEN - *plen; i++ )
                            {
                                EscRtData.ParameterData[i+2u] = ControlNeedInSafetyFile[(*plen/8u) + 1u][i];
                            }
                            *plen = ESC_CB_PARAMETER_IN_SF_DATA_LEN;
                        }                        
                    }  
                }
            }            
            else if( g_u8ParameterLoadingOK == 5u )
            {
                g_u8ParameterLoadingFinish = 1u;
            }
            else
            {}
            
            /****** for test *********/
            /*g_u16ParameterLoadingError = 0u;
            g_u8ParameterLoadingFinish = 1u;*/
        }
#else
        if( OmcEscRtData.ParaStatus & USB_DETECTED )
        {
            /* Power On with USB Stick */
            /* CPU2 wait for restart */
            g_u8ParameterLoading = 1u;
        }
        else
        {
            /* Power On without USB Stick */
            /* wait for receiving parameter from CPU1 */
            if( OmcEscRtData.ParaStatus & SEND_S0001_PARAMETER )
            {
                if( (*(u16*)&OmcEscRtData.ParameterData[0]) == ESC_SF_PARAMETER_DATA_LEN )
                {
                    len = (u8)((*(u16*)&OmcEscRtData.ParameterData[0])%8u);
                    for( i = 0u; i < len; i++ )
                    {
                        ParameterSFFile[(*(u16*)&OmcEscRtData.ParameterData[0]) - len + i] = OmcEscRtData.ParameterData[i+2u];
                    }
                }
                else
                {
                    for( i = 0u; i < 8u; i++ )
                    {
                        ParameterSFFile[(*(u16*)&OmcEscRtData.ParameterData[0]) - 8u + i] = OmcEscRtData.ParameterData[i+2u];
                    }
                }
            }
            else if( OmcEscRtData.ParaStatus & SEND_CS001_PARAMETER )
            {
                if( (*(u16*)&OmcEscRtData.ParameterData[0]) == ESC_CB_PARAMETER_IN_SF_DATA_LEN )
                {
                    len = (u8)((*(u16*)&OmcEscRtData.ParameterData[0])%8u);
                    for( i = 0u; i < len; i++ )
                    {
                        ParameterCBNeedInSafetyFile[(*(u16*)&OmcEscRtData.ParameterData[0]) - len + i] = OmcEscRtData.ParameterData[i+2u];
                    }
                }
                else
                {                
                    for( i = 0u; i < 8u; i++ )
                    {
                        ParameterCBNeedInSafetyFile[(*(u16*)&OmcEscRtData.ParameterData[0]) - 8u + i] = OmcEscRtData.ParameterData[i+2u];
                    }
                }
            }
            else
            {}
            
            if( OmcEscRtData.ParaStatus & SEND_S0001_PARAMETER_FINISH )
            {
                EscRtData.ParaStatus |= SEND_S0001_PARAMETER_FINISH;
            }
            
            if( OmcEscRtData.ParaStatus & SEND_CS001_PARAMETER_FINISH )
            {
                EscRtData.ParaStatus |= SEND_CS001_PARAMETER_FINISH;
            }
            
            if(( EscRtData.ParaStatus & SEND_S0001_PARAMETER_FINISH ) && ( EscRtData.ParaStatus & SEND_CS001_PARAMETER_FINISH ) && 
               ( g_u16ParameterLoadingError == 0u ))
            {
                memcpy((u8*)&SFParameterData,&ParameterSFFile,ESC_SF_PARAMETER_DATA_LEN);
                
                /* Check crc32 is it ok */
                if( MB_CRC32( (u8*)&SFParameterData, ESC_SF_PARAMETER_DATA_LEN, PARAMETER_POLYNOMIALS ))
                {
                    /* Error message. Abort parameter loading. System remains in Init Fault. */
                    /* Parameter error F385 */
                    EN_ERROR49 |= 0x02u;
                    g_u16ParameterLoadingError = 1u;
                }
                else
                { 
                    memcpy((u8*)&CBParameterInSafety,&ParameterCBNeedInSafetyFile,ESC_CB_PARAMETER_IN_SF_DATA_LEN);
                    
                    /* Check crc32 is it ok */
                    if( MB_CRC32( (u8*)&CBParameterInSafety, ESC_CB_PARAMETER_IN_SF_DATA_LEN, PARAMETER_POLYNOMIALS ))
                    {
                        /* Error message. Abort parameter loading. System remains in Init Fault. */
                        /* Parameter error F385 */
                        EN_ERROR49 |= 0x02u;
                        g_u16ParameterLoadingError = 1u;
                    }
                    else
                    {
                        EscRtData.ParaCRC[0] = SFParameterData.CRC32;
                        EscRtData.ParaCRC[1] = CBParameterInSafety.CRC32;
                        g_u8ParameterLoadingFinish = 1u;
                    }    
                }
            }
            
            if( g_u16ParameterLoadingError == 1u )
            {
                g_u8ParameterLoadingFinish = 1u;
            }
        }
        
#endif
        
        /* Parameter initial, for test */
        /* EscParameterInit(); */
        /*EscParameterInit_TR1();*/
    }
    else if(( SfBase_EscState == ESC_READY_STATE ) || ( SfBase_EscState == ESC_FAULT_STATE ))
    {
        /* Parameter changed from the DDU */       
#ifdef GEC_SF_MASTER 

#else

#endif        
    }
    else
    {}
    
    if( SfBase_EscState != ESC_INIT_STATE )
    {
        if( stat_u16TimeParameterCheck < 0xffffu )
        {
            stat_u16TimeParameterCheck++;
        }
        /* 60 s check */
        if( stat_u16TimeParameterCheck > 12000u )
        {
            stat_u16TimeParameterCheck = 0u;
            /* Check parameter CRC */
            if( MB_CRC32( (u8*)&SFParameterData, ESC_SF_PARAMETER_DATA_LEN, PARAMETER_POLYNOMIALS ))
            {
                /* Parameter error F385 */
                EN_ERROR49 |= 0x02u;
            }
            else if( MB_CRC32( (u8*)&CBParameterInSafety, ESC_CB_PARAMETER_IN_SF_DATA_LEN, PARAMETER_POLYNOMIALS ) )
            {
                /* Parameter error F385 */
                EN_ERROR49 |= 0x02u;            
            }
            else
            {}
            
            if(( EscRtData.ParaCRC[0] != SFParameterData.CRC32 ) || ( EscRtData.ParaCRC[1] != CBParameterInSafety.CRC32 ))
            {
                /* Parameter error F385 */
                EN_ERROR49 |= 0x02u;              
            }
        }
    }
}



/******************************  END OF FILE  *********************************/






