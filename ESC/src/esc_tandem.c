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

/* variable */
u8 TandemRunEnable = 0;
u8 TandemMessageRunAllowed = 0;
u8 Tandemoutput = 0;

/*******************************************************************************
* Function Name  : CheckTandemReady
* Description    : Check Tandem in ready state.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void CheckTandemReady(void)
{
       
    if( TANDEM_TYPE == 1 )
    {
        /* Need CheckUpDown_Key() */
        if( CMD_FLAG1 & 0x04 )
        {
            /* need allow by other escalator */
            if( TandemMessageRunAllowed == 1 )
            {
                TandemRunEnable = 1;
            }
        }
        else if( CMD_FLAG1 & 0x08 )
        {
            TandemRunEnable = 1;
            /* send TandemMessageRunAllowed = 1 */
            /* communication by cpu2 CAN1 */
        }
            
    }
    else if( TANDEM_TYPE == 2 )
    {
        if( CMD_FLAG1 & 0x08 )
        {
            if( TandemMessageRunAllowed == 1 )
            {
                TandemRunEnable = 1;
            }
        }
        else if( CMD_FLAG1 & 0x04 )
        {
            TandemRunEnable = 1;
            /* send TandemMessageRunAllowed = 1 */
            /* communication by cpu2 CAN1 */
        }        
    }
    else
    {
        TandemRunEnable = 0;
    }
    

}

/*******************************************************************************
* Function Name  : CheckTandemRun
* Description    : Check Tandem in run state.
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
                EN_ERROR7 |= 0x01;
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
                EN_ERROR7 |= 0x01;
            }
        }      
    }
}

/*******************************************************************************
* Function Name  : TandemOutput
* Description    : Tandem output.
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
    else if( SfBase_EscState & ESC_STATE_NORMAL )
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
    
    /* send Tandemoutput to other escalator */
    /* communication by cpu2 CAN1 */

}


/*******************************************************************************
* Function Name  : ESC_Tandem_Check
* Description    : esc check tandem.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Tandem_Check(void)
{

    if( SfBase_EscState & ESC_STATE_READY )
    {
        CheckTandemReady();
    }
    else if(SfBase_EscState & ESC_STATE_RUNNING)
    {
        CheckTandemRun();
        TandemOutput();
    }
}


/******************************  END OF FILE  *********************************/