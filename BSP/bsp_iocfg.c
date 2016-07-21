/*******************************************************************************
* File Name          : bsp_iocfg.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains input and output pin Initializes.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "bsp_iocfg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Input_Output_PinInit
* Description    : Initializes the Input and Output GPIO
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Input_Output_PinInit(void)
{
  
      GPIO_InitTypeDef GPIO_InitStruct;
      
#ifdef GEC_SF_MASTER
      
      GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
  
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;      

      /** SAFETY input gpio ------------------------------------------------ **/     
      /* INA1 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INA2 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INA3 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
      GPIO_Init(GPIOE , &GPIO_InitStruct);   

      /* INA4 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
      GPIO_Init(GPIOE , &GPIO_InitStruct);   

      /* INA5 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INA6 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INA7 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INA8 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INA9 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INA10 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INA11 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
      GPIO_Init(GPIOD , &GPIO_InitStruct);         

      /* INA12 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INA13 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* INA14 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
      GPIO_Init(GPIOC , &GPIO_InitStruct);   

      /* INA15 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
      GPIO_Init(GPIOC , &GPIO_InitStruct); 

      /* INA16 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
      GPIO_Init(GPIOC , &GPIO_InitStruct);   

      /* INA17 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOA , &GPIO_InitStruct); 

      /* INA18 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOA , &GPIO_InitStruct);   

      /* INA19 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOA , &GPIO_InitStruct); 

      /* INA20 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOC , &GPIO_InitStruct);   

      /* INA21 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOC , &GPIO_InitStruct); 

      /* INA22 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
      GPIO_Init(GPIOC , &GPIO_InitStruct);   

      /* INA23 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* INA24 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOE , &GPIO_InitStruct);   

      /* INA25 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* INA26 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INA27 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* INA28 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INA29 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
      GPIO_Init(GPIOC , &GPIO_InitStruct);
      
       /** SAFETY EXTEND input gpio ---------------------------------------- **/
      /* AX1 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
      GPIO_Init(GPIOC , &GPIO_InitStruct); 

      /* AX2 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX3 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX4 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX5 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX6 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX7 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX8 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX9 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* AX10 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* AX11 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* AX12 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* AX13 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* AX14 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* AX15 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* AX16 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* AX17 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      
      
      /** feedback gpio -------------------------------------------------- **/ 
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
        
      /* SF_RL_DRV_FB */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;                  
      GPIO_Init(GPIOE , &GPIO_InitStruct);      
      
      /* SF_PWR_FB_CPU */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;                  
      GPIO_Init(GPIOB , &GPIO_InitStruct);        

      /* SF_RL_FB */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;                  
      GPIO_Init(GPIOC , &GPIO_InitStruct);       

      /* SF_RL2_FB_CPU1 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;                  
      GPIO_Init(GPIOE , &GPIO_InitStruct);        

      /* AUX_FB */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;                  
      GPIO_Init(GPIOC , &GPIO_InitStruct); 
 
      
      /** output gpio ----------------------------------------------------- **/
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;

      /* SF_RL_WDT */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; 
      GPIO_Init(GPIOB , &GPIO_InitStruct);       
      
      /* AUX_CTR */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0; 
      GPIO_Init(GPIOC , &GPIO_InitStruct);        
      
      /* SF_RL_CTR */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; 
      GPIO_Init(GPIOE , &GPIO_InitStruct);  
      
      /** pluse out gpio -------------------------------------------------- **/
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5; 
      GPIO_Init(GPIOC , &GPIO_InitStruct);        
      
      
      /** relay output init **/
      AUX_CTR_CLR();
      SF_RL_CTR_CLR();
      SF_RL_WDT_SET(); 
      
      
      
      
#else
      

      
      
#ifdef GEC_SF_S_NEW          
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;   
#else
      GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD; 
#endif

      /** SAFETY input gpio ------------------------------------------------ **/     
      /* INB1 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INB2 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INB3 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INB4 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INB5 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INB6 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INB7 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INB8 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INB9 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOA , &GPIO_InitStruct);   

      /* INB10 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
      GPIO_Init(GPIOA , &GPIO_InitStruct);   

      /* INB11 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOA , &GPIO_InitStruct);         

      /* INB12 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOA , &GPIO_InitStruct);   

      /* INB13 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOC , &GPIO_InitStruct); 

      /* INB14 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOC , &GPIO_InitStruct);   

      /* INB15 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
      GPIO_Init(GPIOC , &GPIO_InitStruct); 

      /* INB16 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
      GPIO_Init(GPIOC , &GPIO_InitStruct);   

      /* INB17 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* INB18 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INB19 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* INB20 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INB21 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* INB22 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INB23 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 

      /* INB24 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOD , &GPIO_InitStruct);   

      /* INB25 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* INB26 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INB27 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* INB28 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
      GPIO_Init(GPIOB , &GPIO_InitStruct);   

      /* INB29 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
      GPIO_Init(GPIOD , &GPIO_InitStruct); 
      
       /** SAFETY EXTEND input gpio ---------------------------------------- **/
      /* BX1 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* BX2 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* BX3 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX4 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX5 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX6 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX7 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX8 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX9 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX10 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX11 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
      GPIO_Init(GPIOE , &GPIO_InitStruct); 

      /* BX12 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* BX13 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* BX14 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
      GPIO_Init(GPIOB , &GPIO_InitStruct); 

      /* BX15 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
      GPIO_Init(GPIOC , &GPIO_InitStruct); 

      /* BX16 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
      GPIO_Init(GPIOC , &GPIO_InitStruct); 

      /* BX17 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
      GPIO_Init(GPIOA , &GPIO_InitStruct); 

      
      
      /** feedback gpio -------------------------------------------------- **/ 
#ifdef GEC_SF_S_NEW 
      GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;          
#else
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif
        
      /* SF_RL_DRV_FB */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;                  
      GPIO_Init(GPIOE , &GPIO_InitStruct);      
      
      /* SF_PWR_FB_CPU */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;                  
      GPIO_Init(GPIOE , &GPIO_InitStruct);        

      /* SF_RL_FB */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;                  
      GPIO_Init(GPIOE , &GPIO_InitStruct);       

      /* SF_RL1_FB_CPU2 */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;                  
      GPIO_Init(GPIOC , &GPIO_InitStruct);        

      /* AUX_FB */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;                  
      GPIO_Init(GPIOE , &GPIO_InitStruct); 
   
      
      /** output gpio ----------------------------------------------------- **/
#ifdef GEC_SF_S_NEW 
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
#else
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
#endif
      /* SF_RL_WDT */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1; 
      GPIO_Init(GPIOE , &GPIO_InitStruct);       
      
      /* AUX_CTR */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5; 
      GPIO_Init(GPIOE , &GPIO_InitStruct);        
      
      /* SF_RL_CTR */
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; 
      GPIO_Init(GPIOE , &GPIO_InitStruct);  

      
      /** relay output init **/
      AUX_CTR_CLR();
      SF_RL_CTR_CLR();
      SF_RL_WDT_SET();   


#endif      

}


/******************************  END OF FILE  *********************************/


