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
#define USE_DMA_Transfer 
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define Sample_Num      5

/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADC1ConvertedValue[Sample_Num] = { 0 };

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
        DMA_InitTypeDef  DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	 
 
        /* Set the ADC division factor 6 72M / 6 = 12, ADC can not exceed the maximum time 14M */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   

        /* GPIO configuration ------------------------------------------------*/               
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

#ifdef   USE_DMA_Transfer 
        /* DMA1 channel1 configuration ---------------------------------------*/
        DMA_DeInit(DMA1_Channel1);
        DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC1ConvertedValue;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_BufferSize = Sample_Num;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
        
        /* Enable DMA1 channel1 */
        DMA_Cmd(DMA1_Channel1, ENABLE);
#endif          
        
        /* ADC1 configuration ------------------------------------------------*/
	ADC_DeInit(ADC1);  

        /* ADC operating modes: ADC1 and ADC2 work in standalone mode */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
        /* Analog to digital conversion work in single mode */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
#ifdef   USE_DMA_Transfer
        /* Analog to digital conversion in continuous conversion mode */         
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;;
#else
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
#endif	
        /* Conversion by software rather than external trigger start */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
        /* ADC data is right-aligned */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
        /* The number of order rules convert ADC channels */
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	ADC_Init(ADC1, &ADC_InitStructure);	
               
        
        /* ADC1 regular channels configuration */ 
        ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 ); 

#ifdef   USE_DMA_Transfer
        /* Enable ADC1 DMA */
        ADC_DMACmd(ADC1, ENABLE);
#endif  
  
        /* Enable ADC1 */
        ADC_Cmd(ADC1, ENABLE);
        
        /* Enable ADC1 reset calibaration register */   
        ADC_ResetCalibration(ADC1);
        
        /* Check the end of ADC1 reset calibration register */
        while(ADC_GetResetCalibrationStatus(ADC1));
        
        /* Start ADC1 calibaration */
        ADC_StartCalibration(ADC1);
        
        /* Check the end of ADC1 calibration */
        while(ADC_GetCalibrationStatus(ADC1));

#ifdef   USE_DMA_Transfer
        /* Enable DMA1 channel1 */
        DMA_Cmd(DMA1_Channel1, ENABLE);
        
        /* Start ADC1 Software Conversion */ 
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
#endif
        
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
  	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );	  			    
  
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
#ifdef   USE_DMA_Transfer
  
        uint8_t i;
        uint32_t sum = 0;
        
        for( i = 0; i < Sample_Num; i++ )
        {
          sum += ADC1ConvertedValue[i];
        }
        
        return (sum*60/4096/Sample_Num);  

#else
	static u32 temp_val[5] = { 0 };
	static u8 sf_get_adc_times = 0;
        s32 val = 0;
        
        temp_val[sf_get_adc_times++] = Get_Adc();
        if( sf_get_adc_times == 5 )
        {
            sf_get_adc_times = 0;           
        }
        
        for( u8 i = 0; i < 5; i++ )
        {
          val += temp_val[i];
        }
        
        return (val*60/4096/5);
#endif
} 	 


/******************************  END OF FILE  *********************************/
























