/*******************************************************************************
* File Name          : esc_handrail_speed.c
* Author             : lison
* Version            : V1.0
* Date               : 05/12/2016
* Description        : This file contains esc handrail speed.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_handrail_speed.h"
#include "esc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : HR_Speed_Ready
* Description    : Check the escalator handrail speed ready.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void HR_Speed_Ready(void)
{
    if( SfBase_EscState & ESC_STATE_READY ) 
    {
        Delay(DELAY_NO_PULSE_CHECKING);
        
        if( handrail_speed_pluse_freq > 1 )
        {
            /* fault */
        }
    } 
}


/*******************************************************************************
* Function Name  : Motor_Speed_Ready
* Description    : Check the escalator handrail speed run.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void HR_Speed_Run_EN115(void)
{

}

/*******************************************************************************
* Function Name  : Measure_handrail_speed
* Description    : Measure the escalator hanrail speed.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
u32 Measure_handrail_speed(Sensor)
{


    return current_hanrail_speed_sensor
}



/******************************  END OF FILE  *********************************/






