/*******************************************************************************
* File Name          : esc_tandem.c
* Author             : lison
* Version            : V1.0
* Date               : 05/31/2016
* Description        : This file contains esc tandem.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "esc_tandem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/




/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* parameters */ 
u8 TANDEM_TYPE = 0;

/* variable */
u8 TandemRunEnable = 0;
u8 TandemMessageRunAllowed = 0;
u8 Tandemoutput = 0;

/*******************************************************************************
* Function Name  : CheckTandemReady
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void CheckTandemReady(void)
{
    if( TANDEM_TYPE == 1 )
    {
        if( SfBase_EscState & ESC_STATE_UP )
        {
            if( TandemMessageRunAllowed == 1 )
            {
                TandemRunEnable = 1;
            }
        }
        else if( SfBase_EscState & ESC_STATE_DOWN )
        {
            TandemRunEnable = 1;
        }
            
    }
    else if( TANDEM_TYPE == 2 )
    {
        if( SfBase_EscState & ESC_STATE_DOWN )
        {
            if( TandemMessageRunAllowed == 1 )
            {
                TandemRunEnable = 1;
            }
        }
        else if( SfBase_EscState & ESC_STATE_UP )
        {
            TandemRunEnable = 1;
        }        
    }
    

}

/*******************************************************************************
* Function Name  : CheckTandemReady
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void CheckTandemRun(void)
{
    if( TANDEM_TYPE == 1 )
    {
        if( SfBase_EscState & ESC_STATE_UP )
        {
            if( TandemMessageRunAllowed == 0 )
            {
                /* Tandem fault */
                /* stop escalator */
                
            }
        }           
    }
    else if( TANDEM_TYPE == 2 )
    {
        if( SfBase_EscState & ESC_STATE_DOWN )
        {
            if( TandemMessageRunAllowed == 0 )
            {
                /* Tandem fault */
                /* stop escalator */
                
            }
        }      
    }
}

/*******************************************************************************
* Function Name  : CheckTandemReady
* Description    : 
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void TandemOutput(void)
{
    if( SfBase_EscState & ESC_STATE_INSP )
    {
        Tandemoutput = 0;
    }
    if( SfBase_EscState & ESC_STATE_NORMAL )
    {
        if( TANDEM_TYPE == 1 )
        {
            if( SfBase_EscState & ESC_STATE_DOWN )
            {
                Tandemoutput = 1;
            }
            else
            {
                Tandemoutput = 0;
            }            
        }
        else if( TANDEM_TYPE == 2 )
        {
            if( SfBase_EscState & ESC_STATE_UP )
            {
                Tandemoutput = 1;
            }
            else
            {
                Tandemoutput = 0;
            }            
        }
    }
    else
    {
        Tandemoutput = 0;
    }
    

}



/******************************  END OF FILE  *********************************/