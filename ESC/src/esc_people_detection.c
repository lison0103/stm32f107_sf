
/*******************************************************************************
* File Name          : esc_isp_normal_mode.c
* Author             : Tu
* Version            : V1.0
* Date               : 11/04/2016
* Last modify date   : 11/07/2016
* Description        : 
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_control.h"

static void CheckSafetyCurtain(void);
static u8 PeopleDetectionCalc(u8 mask,u8 input);
static void CheckPeopleDetection(void);

/* Private variables ---------------------------------------------------------*/
u8 Top_PD_Sensor_Mask,Top_PD_Sensor_Input,Bottom_PD_Sensor_Mask,Bottom_PD_Sensor_Input;

/*******************************************************************************
* Function Name  : CheckSafetyCurtain
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
static void CheckSafetyCurtain(void)
{
    
}

/*******************************************************************************
* Function Name  : CheckPeopleDetection
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
static u8 PeopleDetectionCalc(u8 mask,u8 input)
{
  u8 res=0u;
  
  /* Radar */  
  
  if( RADAR_TYPE == 0u ) /* Safety radar */
  {
    if(  (mask  & 0x03u) &&  ((input & 0x03u)!= 0x01u)) /* Sensor fault or detected */ 
    {
      
      res = 0x01u;      
    }                    
    
    if(  (mask  & 0x0Cu) &&  ((input & 0x0cu)!= 0x04u)) /* Sensor fault or detected */ 
    {
      
      res = 0x01u;
      
    }                     
  } 
  else /* Standard radar */
  {
    if( input & 0x0fu ) /* any radar detected, input will be 1 */
    {
       res |= 0x01u;          
    }                        
  } 
   
  /* Entry Light Barrier */  
   
  if( (mask & 0x30u) ) /* Entry Light Barrier be configured  */
  {
    if( LIGHT_BARRIER_COMBS_AREA_TYPE == 0u ) /* 2 input Barrier */
    {
      if( (input & 0xC0u) != 0x40u )
      {
        res |= 0x02u;     /* Entry light barrier detected */
      }
    }
    else if( (mask & 0xc0u) != (input & 0xc0u) ) 
    {
      res |= 0x02u;     /* Entry light barrier detected */
    }  
    else
    {
    }  
  }
  
  /* Combplate Light Barrier */  

  if( (mask & 0xc0u) ) /* Combplate Light Barrier be configured  */
  {
    if( LIGHT_BARRIER_COMBS_AREA_TYPE==0u ) /* 2 input Barrier */
    {
      if( (input & 0xC0u) != 0x40u )
      {
        res |= 0x04u;     /* Combs light barrier detected */
      }
    }
    else if( (mask & 0xc0u) != (input & 0xc0u) ) 
    {
      res |= 0x04u;     /* Combs light barrier detected */
    }  
    else
    {
    }  
  }
  
  return(res);
}


/*******************************************************************************
* Function Name  : CheckPeopleDetection
* Description    :            
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
static void CheckPeopleDetection(void)
{
  u8 res;
  
  /* input decode */
  Top_PD_Sensor_Mask = (((EscRtData. Cfg_Input_Mask[11])>>3u)&0x1fu) | ((u8)(EscRtData. Cfg_Input_Mask[12]<<5u)&0xe0u);           
  Top_PD_Sensor_Input = ((EscRtData. Cfg_Input_Level[11]>>3u)&0x1fu) | ((u8)(EscRtData. Cfg_Input_Level[12]<<5u)&0xe0u);      
                                                                                                                       
  Bottom_PD_Sensor_Mask = ((EscRtData. Cfg_Input_Mask[12]>>3u)&0x1fu) | ((u8)(EscRtData. Cfg_Input_Mask[13]<<5u)&0xe0u);         
  Bottom_PD_Sensor_Input = ((EscRtData. Cfg_Input_Level[12]>>3u)&0x1fu) | ((u8)(EscRtData. Cfg_Input_Level[13]<<5u)&0xe0u);       
  
  if(Top_PD_Sensor_Mask)
  {
    res = PeopleDetectionCalc( Top_PD_Sensor_Mask , Top_PD_Sensor_Input );
    
  }  

  if(Bottom_PD_Sensor_Mask)
  {
    res = PeopleDetectionCalc( Bottom_PD_Sensor_Mask , Bottom_PD_Sensor_Input );
    
  }  
}

/******************************  END OF FILE  *********************************/