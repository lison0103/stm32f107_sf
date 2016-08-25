/*******************************************************************************
* File Name          : comm_display_board.c
* Author             : lison
* Version            : V1.0
* Date               : 08/05/2016
* Description        : This file contains communication with external display board.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "comm_display_board.h"
#include "usart.h" 
#include "crc16.h" 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static u8 buff[300];

/*******************************************************************************
* Function Name  : Comm_DisplayBoard
* Description    : communication with external display board thread.               
* Input          : None         
* Output         : None
* Return         : None
*******************************************************************************/ 		
void Comm_DisplayBoard(void)
{
    
    u32 i = 0u,len = 0u,tlen = 0u;
    
    
    len = BSP_USART_Receive(USART2,buff,0u);
    
    if(len < 5u)
    {
        
    }  
    else if( ((buff[0]==1u)||(buff[0]==2u)) && (!MB_CRC16(buff, len)) )   	
    {
/*        
        DB_Comm_Addr = buff[0];
        switch(buff[1])                
        {
           case 3:                     
            tlen = modbus_slave_03();                       
            break; 
           case 16:	
            tlen = modbus_slave_16();
            break;                    
        }	
*/        
        if(tlen)	
        {			
            i = MB_CRC16(buff, (u16)(tlen-2u));
            buff[tlen-1u] = (u8)(i>>8u);
            buff[tlen-2u] = (u8)i;
            BSP_USART_Send(USART3,buff,tlen);
        }			
        
    }		   
    else if( (buff[0]==0x11u) && (!MB_CRC16(buff, len)) )
    {
        
    }  
    else
    {}
        
 
}


/******************************  END OF FILE  *********************************/