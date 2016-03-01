/**
 * Company	: ThyssenKrupp PDC
 * @file	: main.h
 * @purpose     :
 * @brief	: header for c based Main function 
 *                  POST and BIST WDT test for IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 26-March-2014
 * @author	: Paul
*/
#ifndef _SELF_TEST_H__
#define _SELF_TEST_H__

//typedef enum 
//{
//  LED1 = 0,
//  LED2 = 1,
//} Led_TypeDef;
//
//#define LEDn                     2
//
//#define LED1_PIN                 GPIO_Pin_8
//#define LED1_GPIO_PORT           GPIOA
//#define LED1_GPIO_CLK            RCC_APB2Periph_GPIOA  
//  
//#define LED2_PIN                 GPIO_Pin_2
//#define LED2_GPIO_PORT           GPIOD
//#define LED2_GPIO_CLK            RCC_APB2Periph_GPIOD  

//GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT};
//const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN};
//const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK};
//extern IEC61508_InstCheck_struct InstCheckPOST_struct;
//extern IEC61508_InstCheck_struct InstCheckBIST_struct;

//extern CPUreg_struct CPUregTestPOST_struct;
//ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
//static void RCC_Config(void) ;

//void RCC_Configuration(void);
void Delay(__IO uint32_t nCount);

//void STM_EVAL_LEDInit(Led_TypeDef Led);
//void STM_EVAL_LEDOn(Led_TypeDef Led);
//void STM_EVAL_LEDOff(Led_TypeDef Led);
//void STM_EVAL_LEDToggle(Led_TypeDef Led);

int self_test(void);

#endif  /* _MAIN_H__ */

/************************************** EOF *********************************/