/*******************************************************************************
* File Name          : adc.c
* Author             : lison
* Version            : V1.0
* Date               : 04/12/2016
* Description        : This file contains adc conversion functions.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


	   
/*******************************************************************************
* Function Name  : Adc_Init
* Description    : Initialization ADC.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/		   																   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	 
 
        /* Set the ADC division factor 6 72M / 6 = 12, ADC can not exceed the maximum time 14M */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   

                       
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  

        /* ADC operating modes: ADC1 and ADC2 work in standalone mode */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
        /* Analog to digital conversion work in single mode */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        /* Analog to digital conversion in single conversion mode */
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
        /* Conversion by software rather than external trigger start */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
        /* ADC data is right-aligned */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
        /* The number of order rules convert ADC channels */
	ADC_InitStructure.ADC_NbrOfChannel = 0;	
	ADC_Init(ADC1, &ADC_InitStructure);	
  
	ADC_Cmd(ADC1, ENABLE);	
	
	ADC_ResetCalibration(ADC1);	  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	
	
	ADC_StartCalibration(ADC1);	
 
	while(ADC_GetCalibrationStatus(ADC1));	 


}	

/*******************************************************************************
* Function Name  : Get_Adc
* Description    : Get the adc value.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : Returns the results of a recent conversion ADC1 rule groups.
*******************************************************************************/
u16 Get_Adc(void)   
{
  	
	ADC_RegularChannelConfig(ADC1, 0, 1, ADC_SampleTime_239Cycles5 );	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);			
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

	return ADC_GetConversionValue(ADC1);	
}


/*******************************************************************************
* Function Name  : Get_Adc_Average
* Description    : Get the average adc conversion. 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : -1 : conversion calculation is not complete, other: the average adc conversion. 
*******************************************************************************/
s32 Get_Adc_Average(void)
{
	static u32 temp_val = 0;
	static u8 sf_get_adc_times = 0;
        static s32 val = 0;
        
        temp_val += Get_Adc();
        sf_get_adc_times++;
        if( sf_get_adc_times == 5 )
        {
            val = temp_val*60/4096/5;
            temp_val = 0;
            sf_get_adc_times = 0;           
        }
        return val;
} 	 


/******************************  END OF FILE  *********************************/
























