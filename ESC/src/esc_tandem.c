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


/*******************************************************************************
* Function Name  : CheckTandemReady
* Description    : Check Tandem in ready state.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void CheckTandemReady(void)
{
       
    if( TANDEM_TYPE == 1u )
    {
        /* Need CheckUpDown_Key() */
        if( CMD_FLAG1 & 0x04u )
        {
            /* need allow by other escalator */
            if( TandemMessageRunAllowed == 1u )
            {
                TandemRunEnable = 1u;
            }
        }
        else if( CMD_FLAG1 & 0x08u )
        {
            TandemRunEnable = 1u;
            /* send TandemMessageRunAllowed = 1 */
            /* communication by cpu2 CAN1 */
        }
        else
        {
          
        }
            
    }
    else if( TANDEM_TYPE == 2u )
    {
        if( CMD_FLAG1 & 0x08u )
        {
            if( TandemMessageRunAllowed == 1u )
            {
                TandemRunEnable = 1u;
            }
        }
        else if( CMD_FLAG1 & 0x04u )
        {
            TandemRunEnable = 1u;
            /* send TandemMessageRunAllowed = 1 */
            /* communication by cpu2 CAN1 */
        } 
        else
        {
          
        }        
    }
    else
    {
        TandemRunEnable = 0u;
    }
    

}

/*******************************************************************************
* Function Name  : CheckTandemRun
* Description    : Check Tandem in run state.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void CheckTandemRun(void)
{
    if( TANDEM_TYPE == 1u )
    {
        if( SfBase_EscState & ESC_STATE_UP )
        {
            if( TandemMessageRunAllowed == 0u )
            {
                /* Tandem fault */
                /* stop escalator */
                EN_ERROR6 |= 0x01u;
            }
        }           
    }
    else if( TANDEM_TYPE == 2u )
    {
        if( SfBase_EscState & ESC_STATE_DOWN )
        {
            if( TandemMessageRunAllowed == 0u )
            {
                /* Tandem fault */
                /* stop escalator */
                EN_ERROR6 |= 0x01u;
            }
        }      
    }
    else
    {
      
    }    
}

/*******************************************************************************
* Function Name  : TandemOutput
* Description    : Tandem output.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void TandemOutput(void)
{
    if( SfBase_EscState & ESC_STATE_INSP )
    {
        Tandemoutput = 0u;
    }
    else if( SfBase_EscState & ESC_STATE_NORMAL )
    {
        if( TANDEM_TYPE == 1u )
        {
            if( SfBase_EscState & ESC_STATE_DOWN )
            {
                Tandemoutput = 1u;
            }
            else
            {
                Tandemoutput = 0u;
            }            
        }
        else if( TANDEM_TYPE == 2u )
        {
            if( SfBase_EscState & ESC_STATE_UP )
            {
                Tandemoutput = 1u;
            }
            else
            {
                Tandemoutput = 0u;
            }            
        }
        else
        {
          
        }        
    }
    else
    {
        Tandemoutput = 0u;
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
    else if(SfBase_EscState & ESC_STATE_RUN)
    {
        CheckTandemRun();
        TandemOutput();
    }
    else
    {
      
    }    
}


/******************************  END OF FILE  *********************************/