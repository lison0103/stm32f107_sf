/*******************************************************************************
* File Name          : esc_tandem.c
* Author             : lison
* Version            : V1.0
* Date               : 05/31/2016
* Last modify date   : 09/23/2016
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

static u8 TandemRunEnable = 0u;
static u8 TandemMessageRunAllowed = 0u;
static u8 Tandemoutput = 0u;

/*******************************************************************************
* Function Name  : CheckTandemReady
* Description    : Check Tandem in ready state.
* Input          : None          
* Output         : None
* Return         : None
*******************************************************************************/
static void CheckTandemReady(void)
{
       
    if( TANDEM == 1u )
    {
        /* Need CheckUpDown_Key() */
        if( CMD_ESC_RUN & 0x04u )
        {
            /* need allow by other escalator */
            if( TandemMessageRunAllowed == 1u )
            {
                TandemRunEnable = 1u;
            }
        }
        else if( CMD_ESC_RUN & 0x08u )
        {
            TandemRunEnable = 1u;
            /* send TandemMessageRunAllowed = 1 */
            /* communication by cpu2 CAN1 */
        }
        else
        {
          
        }
            
    }
    else if( TANDEM == 2u )
    {
        if( CMD_ESC_RUN & 0x08u )
        {
            if( TandemMessageRunAllowed == 1u )
            {
                TandemRunEnable = 1u;
            }
        }
        else if( CMD_ESC_RUN & 0x04u )
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
    if( TANDEM == 1u )
    {
        if( CMD_ESC_RUN & ESC_UP )
        {
            if( TandemMessageRunAllowed == 0u )
            {
                /* Stop escalator Tandem Fault (F343) */
                EN_ERROR43 |= 0x80u;
            }
        }           
    }
    else if( TANDEM == 2u )
    {
        if( CMD_ESC_RUN & ESC_DOWN )
        {
            if( TandemMessageRunAllowed == 0u )
            {
                /* Stop escalator Tandem Fault (F344) */
                EN_ERROR44 |= 0x01u;
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
    if( CMD_ESC_RUN_MODE & ESC_INSPECT )
    {
        Tandemoutput = 0u;
    }
    else if( ( CMD_ESC_RUN_MODE & ESC_INSPECT ) == ESC_NORMAL )
    {
        if( TANDEM == 1u )
        {
            if( CMD_ESC_RUN & ESC_DOWN )
            {
                Tandemoutput = 1u;
            }
            else
            {
                Tandemoutput = 0u;
            }            
        }
        else if( TANDEM == 2u )
        {
            if( CMD_ESC_RUN & ESC_UP )
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

    if( SfBase_EscState == ESC_READY_STATE )
    {
        CheckTandemReady();
    }
    else if(SfBase_EscState == ESC_RUN_STATE)
    {
        CheckTandemRun();
        TandemOutput();
    }    
    else if(( SfBase_EscState == ESC_STOPPING_PROCESS_STATE ) || ( SfBase_EscState == ESC_FAULT_STATE ))
    {
        TandemOutput();
    }
    else
    {
      
    }    
}


/******************************  END OF FILE  *********************************/