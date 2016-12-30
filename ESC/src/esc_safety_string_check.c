/*******************************************************************************
* File Name          : esc_safety_string_check.c
* Author             : Tu/Lison
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 12/16/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"
#include "initial_devices.h"   
#include "esc_error_process.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Safety_String_End(void);
static void Safety_String_Begin(void);
void Safety_String_Check(void);
void Safety_String_Fault_Check(u16 ID_Number);

u8 stat_u8DBL1SafetyStringSwitchOpen = 0u;
#ifdef GEC_SF_MASTER    
u16 g_u16EndOfSafetyStringVoltage = 0u;
#endif 

/*******************************************************************************
* Function Name  : Safety_String_Begin
* Description    : Go to fault if begin safety string is open.      
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Safety_String_Begin(void)
{    
    /* Input X10 */
    if( !( INPUT_PORT9_16 & INPUT_PORT10_MASK ) ) /* LOW active*/
    {
        /* FALUT: Begin safety string is open */
        EN_ERROR50 |= 0x40u;      
    }  
}

/*******************************************************************************
* Function Name  : Safety_String_End
* Description    : Go to fault if end safety string is open.      
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Safety_String_End(void)
{
    static u8 safety_end_off_tcnt=0u,safety_end_on_flag=0u;
    static u16 stat_u16TimeSafetyString = 0u;
    u32 *pDBL1Input;
    u8 i;
    
    if( safety_end_off_tcnt < 255u ) 
    {
        safety_end_off_tcnt++;
    }
    
    /* End safety string is open */
    if( INPUT_PORT_SF_PWR_FB_CPU_MASK & INPUT_FEEDBACK )
    {        
        CMD_FLAG5 &= ~ESC_SAFETY_END_CLOSE;
        
        if(safety_end_on_flag) 
        {
            safety_end_off_tcnt = 0u;    
        }
        safety_end_on_flag = 0u;  
        
        CMD_FLAG5 &= ~ESC_SAFETY_END_ENABLE;
        
        /* Safety string open F425 */
        if( CMD_FLAG3 & 0x10u) 
        {
          EN_ERROR54 |= 0x02u;
        }
        /* failure lock */
        if( END_SAFETY_STRING_FAULT_TYPE == 0u )
        {
            EscFaultType[425] = EF;
        }
        /* standard fault  */
        else if( END_SAFETY_STRING_FAULT_TYPE == 1u )
        {
            EscFaultType[425] = ES;
        }
        /* recovery fault */
        else if( END_SAFETY_STRING_FAULT_TYPE == 2u )
        {
            EscFaultType[425] = ER;
        }
        else
        {}
    }
    else 
    {
        CMD_FLAG5 |= ESC_SAFETY_END_CLOSE;
        
        safety_end_on_flag = 1u;
        
        if(safety_end_off_tcnt > 100u)
        {  
            CMD_FLAG5 |= ESC_SAFETY_END_ENABLE;
        } 
        
        /* recovery fault */
        if( END_SAFETY_STRING_FAULT_TYPE == 2u )
        {
            EN_ERROR54 &= ~0x02u;
        }
        
        /* emergency stop recovery */
        EN_ERROR4 &= ~0x3Cu;
        EN_ERROR37 &= ~0x60u;
        
    }  
    
    /* DBL1 configured */
    if( DIAGNOSTIC_BOARD_L1_QUANTITY > 0u )
    {
        stat_u8DBL1SafetyStringSwitchOpen = 0u;
        
        /* check DBL1 upper inputs */
        pDBL1Input = (u32*)&EscRtData.DBL1Upper.InputData[0];
        for( i = 0u; i < UPPER_DIAG_SS_LENGTH; i++ )
        {
            if(!( *pDBL1Input & ( 1u << i )))
            {
                stat_u8DBL1SafetyStringSwitchOpen = 1u;
                /* check IO parameter to know the fault */               
                Safety_String_Fault_Check(IO_PARAMETER_DBL1_UPPER_INPUT[i]);                
                break;
            }
        }
        
        if( stat_u8DBL1SafetyStringSwitchOpen == 0u )
        {
            /* check DBL1 lower inputs */
            pDBL1Input = (u32*)&EscRtData.DBL1Lower.InputData[0];
            for( i = 0u; i < LOWER_DIAG_SS_LENGTH; i++ )
            {
                if(!( *pDBL1Input & ( 1u << i )))
                {
                    stat_u8DBL1SafetyStringSwitchOpen = 1u;
                    /* check IO parameter to know the fault */               
                    Safety_String_Fault_Check(IO_PARAMETER_DBL1_LOWER_INPUT[i]);                
                    break;
                }
            }  
            
            if( stat_u8DBL1SafetyStringSwitchOpen == 0u )
            {
                if( DIAGNOSTIC_BOARD_L1_QUANTITY >= 3u )
                {
                    /* check DBL1 interm1 inputs */
                    pDBL1Input = (u32*)&EscRtData.DBL1Interm1.InputData[0];
                    for( i = 0u; i < INTERM1_DIAG_SS_LENGTH; i++ )
                    {
                        if(!( *pDBL1Input & ( 1u << i )))
                        {
                            stat_u8DBL1SafetyStringSwitchOpen = 1u;
                            /* check IO parameter to know the fault */                
                            Safety_String_Fault_Check(IO_PARAMETER_DBL1_INTERM1_INPUT[i]);   
                            break;
                        }
                    }            
                }
                
                if( stat_u8DBL1SafetyStringSwitchOpen == 0u )
                {
                    if( DIAGNOSTIC_BOARD_L1_QUANTITY == 4u )
                    {
                        /* check DBL1 interm2 inputs */
                        pDBL1Input = (u32*)&EscRtData.DBL1Interm2.InputData[0];
                        for( i = 0u; i < INTERM2_DIAG_SS_LENGTH; i++ )
                        {
                            if(!( *pDBL1Input & ( 1u << i )))
                            {
                                stat_u8DBL1SafetyStringSwitchOpen = 1u;
                                /* check IO parameter to know the fault */               
                                Safety_String_Fault_Check(IO_PARAMETER_DBL1_INTERM2_INPUT[i]);   
                                break;                    
                            }
                        }            
                    }   
                }
            }
        }
        
        /* change the fault Safety string open with the correct one */
        if( stat_u8DBL1SafetyStringSwitchOpen == 1u )
        {
            if( EN_ERROR54 & 0x02u )
            {
                EscFaultType[425] = ER;
                EN_ERROR54 &= ~0x02u;
                stat_u16TimeSafetyString = 0u;
            }
            else
            {
                if( stat_u16TimeSafetyString < 500u )
                {
                    stat_u16TimeSafetyString++;
                }
                /* in case of one input in DBL1 be configured as safety string switch input is open 
                and ¡°end of safety string¡± is close, after 2s, safety board goes to FAULT */
                if( stat_u16TimeSafetyString > 400u )
                {
                    /* End safety string incoherence F367 */
                    EN_ERROR46 |= 0x80u;
                }
            }
        }
    }
    
         
  /* The system supervises also the voltage of the end safety string. */
  /* This voltage is sent to Control Board */
  /* Only in High configure safety board CPU1*/
#ifdef GEC_SF_MASTER    
     g_u16EndOfSafetyStringVoltage = Get_Adc_Average();
#endif    
}

/*******************************************************************************
* Function Name  : Safety_String_Fault_Check
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Safety_String_Fault_Check(u16 ID_Number)
{ 
    
    switch(ID_Number)
    {
        /* 150	Top Caterpillar Right (SS) */
       case 150u:
        {
            /* F299	Caterpillar top right */
            EN_ERROR38 |= 0x08u;
            break;
        }
        /* 151	Top Caterpillar Left (SS) */
       case 151u:
        {
            /* F300	Caterpillar top left */
            EN_ERROR38 |= 0x10u;
            break;
        }        
        /* 152	Add Emergency Stop 1 (SS) */
       case 152u:
        {
            /* F27	Add emergency stop top */
            EN_ERROR4 |= 0x08u;
            break;
        }         
        /* 153	Top Emergency Stop Trolley (SS) */
       case 153u:
        {
            /* F293	Emergency stop top (trolley) */
            EN_ERROR37 |= 0x20u;
            break;
        }         
        /* 154	Shutter/Rolling door (SS) */
       case 154u:
        {
            /* F297	Shutters/Rollings doors top error */
            EN_ERROR38 |= 0x02u;
            break;
        }         
        /* 155	Top Buggy Right (SS) */
       case 155u:
        {
            /* F119	Buggy protection top right */
            EN_ERROR15 |= 0x80u;
            break;
        }         
        /* 156	Top Buggy Left (SS) */
       case 156u:
        {
            /* F118	Buggy protection top left */
            EN_ERROR15 |= 0x40u;
            break;
        }         
        /* 157	Top Vertical Combplate Right (SS) */
       case 157u:
        {
            /* F96	Combplate cont.vert. 1 top right */
            EN_ERROR13 |= 0x01u;
            break;
        }         
        /* 158	Top Vertical Combplate Left (SS) */
       case 158u:
        {
            /* F95	Combplate cont.vert. 1 top left */
            EN_ERROR12 |= 0x80u;
            break;
        }        
        /* 159	Top Skirt Right (SS) */
       case 159u:
        {
            /* F126	Skirt limit contacts top right */
            EN_ERROR16 |= 0x40u;
            break;
        }        
        /* 160	Top Skirt Left (SS) */
       case 160u:
        {
            /* F125	Skirt limit contacts top left */
            EN_ERROR16 |= 0x20u;
            break;
        }        
        /* 161	Top Skirt Inclined Right (SS) */
       case 161u:
        {
            /* F302	Skirt inclined contact top right */
            EN_ERROR38 |= 0x40u;
            break;
        }        
        /* 162	Top Skirt Inclined Left (SS) */
       case 162u:
        {
            /* F301	Skirt inclined contact top left */
            EN_ERROR38 |= 0x20u;
            break;
        }        
        /* 163	Intermediate Broken Step (SS) */
       case 163u:
        {
            /* F124	Broken step contact middle 1 */
            EN_ERROR16 |= 0x10u;
            break;
        }        
        /* 164	Addition Emergency Stop 2 (SS) */
       case 164u:
        {
            /* F29	Add emergency stop bottom */
            EN_ERROR4 |= 0x20u;
            break;
        }        
        /* 165	Bottom Emergency Stop Trolley (SS) */
       case 165u:
        {
            /* F294	Emergency stop bottom (trolley) */
            EN_ERROR37 |= 0x40u;
            break;
        }
        
        /* 167	Bottom Buggy Right (SS) */
       case 167u:
        {
            /* F121	Buggy protection bottom right */
            EN_ERROR16 |= 0x02u;
            break;
        }        
        /* 168	Bottom Buggy Left (SS) */
       case 168u:
        {
            /* F120	Buggy protection bottom left */
            EN_ERROR16 |= 0x01u;
            break;
        }        
        /* 169	Bottom Vertical Combplate Right (SS) */
       case 169u:
        {
            /* F98	Combplate cont.vert. 1 bottom right */
            EN_ERROR13 |= 0x04u;
            break;
        }        
        /* 170	Bottom Vertical Combplate Left (SS) */
       case 170u:
        {
            /* F97	Combplate cont.vert. 1 bottom left */
            EN_ERROR14 |= 0x02u;
            break;
        }        
        /* 171	Bottom Glass Outer Cladding Right (SS) */
       case 171u:
        {
            /* F123	Outer cladding right */
            EN_ERROR16 |= 0x08u;
            break;
        }        
        /* 172	Bottom Glass Outer Cladding Left (SS) */
       case 172u:
        {
            /* F122	Outer cladding left */
            EN_ERROR16 |= 0x04u;
            break;
        }        
        /* 173	Bottom Skirt Right (SS) */
       case 173u:
        {
            /* F128	Skirt limit contacts bottom right */
            EN_ERROR17 |= 0x01u;
            break;
        }        
        /* 174	Bottom Skirt Left (SS) */
       case 174u:
        {
            /* F127	Skirt limit contacts bottom left */
            EN_ERROR16 |= 0x80u;
            break;
        }        
        /* 175	Bottom Skirt Inclined Right (SS) */
       case 175u:
        {
            /* F304	Skirt inclined contact bottom right */
            EN_ERROR39 |= 0x01u;
            break;
        }        
        /* 176	Bottom Skirt Inclined Left (SS) */
       case 176u:
        {
            /* F303	Skirt inclined contact bottom left */
            EN_ERROR38 |= 0x80u;
            break;
        }        
        /* 177	Bottom Shutter/Rolling doors (SS) */
       case 177u:
        {
            /* F298	Shutters/Rollings doors bottom error */
            EN_ERROR38 |= 0x04u;
            break;
        }        
        
        /* 310	Top Horizontal Combplate Right */
       case 310u:
        {
            /* F22	Combplate contact horiz. top right */
            EN_ERROR3 |= 0x40u;
            break;
        }          
        /* 311	Top Horizontal Combplate Left */
       case 311u:
        {
            /* F21	Combplate contact horiz. top left */
            EN_ERROR3 |= 0x20u;
            break;
        }          
        /* 312	Top Handrail Inlet Right */
       case 312u:
        {
            /* F24	Handrail inlet top right */
            EN_ERROR4 |= 0x01u;
            break;
        }          
        /* 313	Top Handrail Inlet Left */
       case 313u:
        {
            /* F23	Handrail inlet top left */
            EN_ERROR3 |= 0x80u;
            break;
        }          
        /* 314	Top Pit Emergency Stop */
       case 314u:
        {
            /* F42	Pit emergency stop top */
            EN_ERROR6 |= 0x04u;
            break;
        }          
        /* 315	Top Broken Step */
       case 315u:
        {
            /* F44	Broken step contact top */
            EN_ERROR6 |= 0x10u;
            break;
        }          
        /* 316	Top Emergency Stop */
       case 316u:
        {
            /* F26	Emergency stop top */
            EN_ERROR4 |= 0x04u;
            break;
        }          
        /* 317	Bottom Horizontal Combplate Right */
       case 317u:
        {
            /* F39	Combplate contact horiz. bott. Right */
            EN_ERROR5 |= 0x80u;
            break;
        }          
        /* 318	Bottom Horizontal Combplate Left */
       case 318u:
        {
            /* F38	Combplate contact horiz. bott. Left */
            EN_ERROR5 |= 0x40u;
            break;
        }          
        /* 319	Bottom Handrail Inlet Right */
       case 319u:
        {
            /* F41	Handrail inlet bottom right */
            EN_ERROR6 |= 0x02u;
            break;
        }          
        /* 320	Bottom Handrail Inlet Left */
       case 320u:
        {
            /* F40	Handrail inlet bottom left */
            EN_ERROR6 |= 0x01u;
            break;
        }          
        /* 321	Bottom Pit Emergency Stop */
       case 321u:
        {
            /* F43	Pit emergency stop bottom */
            EN_ERROR6 |= 0x08u;
            break;
        }          
        /* 322	Bottom Broken Step */
       case 322u:
        {
            /* F45	Broken step contact bottom  */
            EN_ERROR6 |= 0x20u;
            break;
        }          
        /* 323	Bottom Emergency Stop */
       case 323u:
        {
            /* F28	Emergency stop bottom */
            EN_ERROR4 |= 0x10u;
            break;
        }          
        /* 324	Bottom Step Chain Right */
       case 324u:
        {
            /* F47	Chain tension cont. bottom right */
            EN_ERROR6 |= 0x80u;
            break;
        }          
        /* 325	Bottom Step Chain Left */
       case 325u:
        {
            /* F46	Chain tension cont. bottom left */
            EN_ERROR6 |= 0x40u;
            break;
        }          
       default:
        {
            /* error, no match ID number?? */
            stat_u8DBL1SafetyStringSwitchOpen = 2u;
            break;
        }
    }
}

/*******************************************************************************
* Function Name  : Safety_String_Check
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void Safety_String_Check(void)
{ 
  Safety_String_End(); 
  
  Safety_String_Begin(); 
  
  /*if */
  
   /* End safety string is close */
  if( !(INPUT_PORT_SF_PWR_FB_CPU_MASK & INPUT_FEEDBACK) )
  {
                   
  }    
}

/******************************  END OF FILE  *********************************/