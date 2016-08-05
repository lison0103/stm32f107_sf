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
    
    u16 i=0,len=0,tlen=0;
    
    
    len = BSP_USART_Receive(USART2,buff,0);
    
    if(len<5)
    {
        
    }  
    else if( ((buff[0]==1)||(buff[0]==2)) && (!MB_CRC16(buff, len)) )   	
    {
//        DB_Comm_Addr = buff[0];
//        switch(buff[1])                
//        {
//           case 3:                     
//            tlen = modbus_slave_03();                       
//            break; 
//           case 16:	
//            tlen = modbus_slave_16();
//            break;                    
//        }	
        
        if(tlen)	
        {			
            i = MB_CRC16(buff, tlen-2);
            buff[tlen-1] = i>>8;
            buff[tlen-2] = i;
            BSP_USART_Send(USART3,buff,tlen);
        }			
        
    }		
    
    else if( (buff[0]==0x11) && (!MB_CRC16(buff, len)) )
    {
        
    }          
        
 
}


/******************************  END OF FILE  *********************************/