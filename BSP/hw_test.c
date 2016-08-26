/*******************************************************************************
* File Name          : hw_test.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : Contains some test function.
*                      
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "hw_test.h"
#include "led.h"
#include "can.h"
#include "spi.h"
#include "ewdt.h"
#include "esc_error_process.h"
#include "crc16.h"
#include "safety_test.h"
#include "esc.h"
#include "esc_comm_safety_dualcpu.h"

#ifdef GEC_SF_MASTER
#include "usart.h"
#include "crc16.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 sflag1 = 0u,inputnum1 = 0u;
u32 sflag2 = 0u,inputnum2 = 0u;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : Input_Check
* Description    : Monitor the input pin status test.
*                   
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Input_Check(void)
{  
    
    u32 *ulPt_Input,*pc_can_tx;
    u8 i;
    
    ulPt_Input = (u32*)&EscRTBuff[4];
    pc_can_tx = (u32*)&CAN1_TX_Data[0];
    
    for( i = 0u; i < 4u; i++ )
    {
        CAN1_TX_Data[i]= 0x0u;                 
    }
    
    for( i = 0u; i < 28u; i++ )
    {
        if( *ulPt_Input & ( 1u << i ))
        {
            *pc_can_tx |= 1u << i;
        }
    } 

}        
        

/*******************************************************************************
* Function Name  : Input_Check2
* Description    : Monitor the input pin status and test.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Input_Check2(void)
{
  
        u32 *ulPt_Input1,*ulPt_Input2;
        u32 *ulPt_Input3,*ulPt_Input4;
        u32 i;


        if( testmode == 1u )
        {
            ulPt_Input1 = (u32*)&EscRTBuff[4];       
            ulPt_Input2 = (u32*)&EscRTBuff[8];
            ulPt_Input3 = (u32*)&McRxBuff[4];       
            ulPt_Input4 = (u32*)&McRxBuff[8];      
            
            sflag1 = 0u;
            inputnum1 = 0u;      
            sflag2 = 0u;
            inputnum2 = 0u;       
            
            
            for( i = 0u; i < 29u; i++ )
            {
                if( *ulPt_Input1 & (( 1u << i )))
                {
                    sflag1++;
                    inputnum1 = i + 1u;
                }
            }
            
            for( i = 0u; i < 17u; i++ )
            {
                if( *ulPt_Input2 & (( 1u << i )))
                {
                    sflag1++;
                    inputnum1 = i + 30u;
                }
            }  
            
            for( i = 0u; i < 29u; i++ )
            {
                if( *ulPt_Input3 & (( 1u << i )))
                {
                    sflag2++;
                    inputnum2 = i + 1u;
                }
            }
            
            for( i = 0u; i < 17u; i++ )
            {
                if( *ulPt_Input4 & (( 1u << i )))
                {
                    sflag2++;
                    inputnum2 = i + 30u;
                }
            }             

            if( (sflag1 != sflag2) || (inputnum1 != inputnum2) )
            {
                sflag1 = 2u;
            }
            
#ifdef GEC_SF_MASTER             
            CAN2_TX_Data[0] = (u8)inputnum1;
            CAN2_TX_Data[1] = (u8)sflag1;            
#endif         
            
            if (( inputnum1== 0u ) || ( sflag1 > 1u ))
            {
                SF_RELAY_OFF(); 
                AUX_RELAY_OFF();
            }
#ifdef GEC_SF_MASTER   
            else if( inputnum1 & 0x0Au )
            {
                if ( inputnum1 & 0x08u )
                {
                    SF_RELAY_ON(); 
                }
                
                if ( inputnum1 & 0x02u )
                {
                    AUX_RELAY_ON(); 
                } 
            }
#else
            else if( inputnum1 & 0x05u )
            {
                if ( inputnum1 & 0x04u )
                {
                    SF_RELAY_ON(); 
                }
                
                if ( inputnum1 & 0x01u )
                {
                    AUX_RELAY_ON(); 
                }   
            }
#endif
            else
            {
                SF_RELAY_OFF(); 
                AUX_RELAY_OFF();                
            }
            
        }
}


/*******************************************************************************
* Function Name  : CrossCommCPUCheck
* Description    : CrossCommCPU and Data Integrity test
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CrossCommCPUCheck(void)
{    

    u16 i = 0u;  
    u8 number = 0u;
    u8 data_error = 0u;
    u32 test_cnt = 100u;  
    u16 comm_num = buffersize;
  
#ifdef GEC_SF_MASTER      
    u8 testresult = 0u;
#endif
        
    number++;

    for( i = 0u; i < comm_num; i++)
    {
        SPI1_TX_Data[i] = number;
    }
    
    i = MB_CRC16( SPI1_TX_Data, comm_num - 2u );
    SPI1_TX_Data[comm_num - 2u] = (u8)i;
    SPI1_TX_Data[comm_num - 1u] = (u8)(i>>8u);
    
    
    SPI1_DMA_ReceiveSendByte(comm_num);

#ifdef GEC_SF_MASTER
    DMA_Check_Flag(100000u);
#else
    DMA_Check_Flag(100000000u);
#endif
    
        
        while(test_cnt--)
        {
            
            EWDT_TOOGLE(); 
            IWDG_ReloadCounter();             
        
#ifdef GEC_SF_MASTER              
              
              delay_ms(2u);
              
              number++;
              if(number >= 255u)
              {
                  number = 0u;
              }
              for( i = 0u; i < comm_num - 2u; i++ )
              {
                    SPI1_TX_Data[i] = number;
              }
              
              i = MB_CRC16( SPI1_TX_Data, comm_num - 2u );
              SPI1_TX_Data[comm_num - 2u] = (u8)i;
              SPI1_TX_Data[comm_num - 1u] = (u8)(i>>8u);
              
              SPI1_DMA_ReceiveSendByte(comm_num);
                        
              DMA_Check_Flag(40000u);
              
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {

                  for( i = 0u; i < comm_num - 2u; i++ )
                  {
                      testresult = SPI1_RX_Data[i]^(SPI1_TX_Data[i] - 1u);
                      if( testresult )
                      {
                          data_error++;                            
                          break;

                      }
                  
                  }
              } 
              else
              {
                  data_error++;                 
  
              }
              
              if( data_error > 2u )
              {
                  break;
              }               
              
              
#else
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {                 
                  for( i = 0u; i < comm_num - 2u; i++ )
                  {
                      SPI1_TX_Data[i] = SPI1_RX_Data[i];
                  }   
                  
                  i = MB_CRC16( SPI1_TX_Data, comm_num - 2u );
                  SPI1_TX_Data[comm_num - 2u] = (u8)i;
                  SPI1_TX_Data[comm_num - 1u] = (u8)(i>>8u);
              } 
              else
              {
                  data_error++;
                  if( data_error > 2u )
                  {
                      break;
                  }
              }
              
              SPI1_DMA_ReceiveSendByte(comm_num);

              DMA_Check_Flag(40000u);
#endif              
        }      
        

#ifdef GEC_SF_MASTER 
#else
/*        SPI1_DMA_ReceiveSendByte(comm_num);*/
#endif
    if( data_error > 2u )
    {
        /* SPI1_DMA_Check error */
        EN_ERROR7 |= 0x01u;
/*        FailSafeTest();*/
    }


}


#ifdef GEC_SF_MASTER 
/*******************************************************************************
* Function Name  : HardwareTEST
* Description    : Test the board.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void HardwareTEST(void)
{
    u8 testdata1[10],testdata2[10];
    u8 testerror = 0u;
    u8 len = 0u, len1 = 0u, len2 = 0u;
    u16 waittms = 0u;
    u8 senddata[50],recvdata[50];
    u8 i;
    
    CAN1_TX_Data[0] = 0xf1u;
    CAN2_TX_Data[0] = 0xf1u;
    testdata1[0] = 0xf1u;
    testdata2[0] = 0xf1u;
    for( i = 1u; i < 10u ; i++ )
    {
        CAN1_TX_Data[i] = i + 0xd0u;
        testdata1[i] = i + 0xd0u;
        CAN2_TX_Data[i] = i + 0xe0u;
        testdata2[i] = i + 0xe0u;        
    }
    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10u);
    BSP_CAN_Send(CAN2, &CAN2_TX_Up, CAN1_TEST_ID, CAN2_TX_Data, 10u);
    
    do
    {
        len1 = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0u);
        delay_ms(1u);
        EWDT_TOOGLE();
        IWDG_ReloadCounter(); 
        waittms++;
        if( waittms > 5000u )
        {
            waittms = 0u;
            break;
        }
    }
    while( (len1 != 10u) || (CAN1_RX_Data[0] != 0xf1u) );     

    waittms = 0u;
    do
    {
        len2 = BSP_CAN_Receive(CAN2, &CAN2_RX_Up, CAN2_RX_Data, 0u);
        delay_ms(1u);
        EWDT_TOOGLE();
        IWDG_ReloadCounter(); 
        waittms++;
        if( waittms > 100u )
        {
            waittms = 0u;
            break;
        }
    }
    while( (len2 != 10u) || (CAN2_RX_Data[0] != 0xf1u) );   
    
    if( (len1 == 10u) && (CAN1_RX_Data[0] == 0xf1u) )
    {
        waittms = 0u;
        for( i = 2u; i < 10u ; i++ )
        {
            CAN1_TX_Data[i] = CAN1_RX_Data[i];
        }
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10u);
        
        if( (len2 == 10u) && (CAN2_RX_Data[0] == 0xf1u) )
        {
            waittms = 0u;
            for( i = 2u; i < 10u ; i++ )
            {
                CAN2_TX_Data[i] = CAN2_RX_Data[i];
            }
            BSP_CAN_Send(CAN2, &CAN2_TX_Up, CAN1_TEST_ID, CAN2_TX_Data, 10u);
        }        
        
        waittms = 0u;
        do
        {
            len1 = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0u);
            delay_ms(1u);
            EWDT_TOOGLE();
            IWDG_ReloadCounter(); 
            waittms++;
            if( waittms > 2000u )
            {
                waittms = 0u;
                break;
            }
        }
        while( (len1 != 10u) || (CAN1_RX_Data[0] != 0xf1u) ); 
        
        waittms = 0u;
        do
        {
            len2 = BSP_CAN_Receive(CAN2, &CAN2_RX_Up, CAN2_RX_Data, 0u);
            delay_ms(1u);
            EWDT_TOOGLE();
            IWDG_ReloadCounter(); 
            waittms++;
            if( waittms > 2000u )
            {
                waittms = 0u;
                break;
            }
        }
        while( (len2 != 10u) || (CAN2_RX_Data[0] != 0xf1u) );         
        
        
        if( (len1 == 10u) && (CAN1_RX_Data[0] == 0xf1u) && (len2 == 10u) && (CAN2_RX_Data[0] == 0xf1u) )
        {
            for( i = 2u; i < 10u ; i++ )
            {
                if( CAN1_RX_Data[i] != testdata1[i] )
                {
                    testerror++;
                    break;
                }
            }


            for( i = 2u; i < 10u ; i++ )
            {
                if( CAN2_RX_Data[i] != testdata2[i] )
                {
                    testerror++;
                    break;
                }
            }
 
            
            if( testerror == 0u )
            {
                if( ( (CAN1_RX_Data[1] == 0xc1u) && (CAN2_RX_Data[1] == 0xb1u) ) 
                   || ( (CAN1_RX_Data[1] == 0xf1u) && (CAN2_RX_Data[1] == 0xb1u) ) )
                {
                    testmode = 1u;
                }
            }
        } 
        
        
    }   
    else
    {
        CAN_FilterInitTypeDef  	        CAN_FilterInitStructure;
        
        /* CAN1 filter init */
        CAN_FilterInitStructure.CAN_FilterNumber=0u;	
        CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	
        
        CAN_FilterInitStructure.CAN_FilterIdHigh=(u16)((((u32)0x00C8u<<3u)&0xFFFF0000u)>>16u);	
        CAN_FilterInitStructure.CAN_FilterIdLow=(u16)((((u32)0x00C8<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFFu);
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffffu;
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xffffu; 
        
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);    

        /* CAN2 filter init */
        CAN_FilterInitStructure.CAN_FilterNumber=14u;	
        CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
        
        CAN_FilterInitStructure.CAN_FilterIdHigh = (u16)((((u32)CAN2RX_UP_ID << 3u) & 0xFFFF0000u ) >> 16u);	
        CAN_FilterInitStructure.CAN_FilterIdLow = (u16)((((u32)CAN2RX_UP_ID << 3u) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFFu);
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (u16)(((((u32)(~( CAN2RX_UP_ID ^ CAN2RX_DOWN_ID ))) << 3u) & 0xFFFF0000u) >> 16u);
        CAN_FilterInitStructure.CAN_FilterMaskIdLow = (u16)(((((u32)(~( CAN2RX_UP_ID ^ CAN2RX_DOWN_ID ))) << 3u) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFFu);   
        
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);	        
    }
    

    senddata[0] = 0xbcu;
    if( testmode == 1u )
    {
        senddata[1] = 0x01u;
    }
    else
    {
        senddata[1] = 0x02u;
    }
    CPU_Exchange_Data(senddata, 2u);
    CPU_Data_Check(recvdata, &len);
/*    
    delay_ms(10);
    CPU_Exchange_Data(senddata, 2);
    CPU_Data_Check(recvdata, &len); //recv  
    
    if( len == 0x02 && recvdata[0] == 0xbc )
    {
        delay_ms(10);
        CPU_Exchange_Data(senddata, 2);
        CPU_Data_Check(recvdata, &len); //recv      
        if( len == 0x02 && recvdata[0] == 0xbc )
        {
            if( recvdata[1] == 0xf1 )
            {
                testmode = 1;
            }
            else
            {
                testmode = 0;
            }            
        }
    }
*/    
}
#else
/*******************************************************************************
* Function Name  : HardwareTEST
* Description    : Test the board.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void HardwareTEST(void)
{
    u8 testdata[10];
    u8 testerror = 0u;
    u8 len = 0u;
    u16 waittms = 0u;
    u8 senddata[10],recvdata[10];
    u8 i;

    CAN1_TX_Data[0] = 0xf1u;
    testdata[0] = 0xf1u;
    for( i = 1u; i < 10u ; i++ )
    {
        CAN1_TX_Data[i] = i + 0xf0u;
        testdata[i] = i + 0xf0u;
    }
    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10u);
    
    do
    {
        len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0u);
        delay_ms(1u);
        EWDT_TOOGLE();
        IWDG_ReloadCounter(); 
        waittms++;
        if( waittms > 2000u )
        {
            waittms = 0u;
            break;
        }
    }
    while( (len != 10u) || (CAN1_RX_Data[0] != 0xf1u) );     
    
    if( (len == 10u) && (CAN1_RX_Data[0] == 0xf1u) )
    {
        waittms = 0u;
        for( i = 2u; i < 10u; i++ )
        {
            CAN1_TX_Data[i] = CAN1_RX_Data[i];
        }
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10u);
        
        do
        {
            len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0u);
            delay_ms(1u);
            EWDT_TOOGLE();
            IWDG_ReloadCounter(); 
            waittms++;
            if( waittms > 8000u )
            {
                waittms = 0u;
                break;
            }
        }
        while( (len != 10u) || (CAN1_RX_Data[0] != 0xf1u) ); 
        
        if( (len == 10u) && (CAN1_RX_Data[0] == 0xf1u) )
        {
            for( i = 2u; i < 10u ; i++ )
            {
                if( CAN1_RX_Data[i] != testdata[i] )
                {
                    testerror = 1u;
                    break;
                }
            }
            
            if( testerror == 0u )
            {
                if( CAN1_RX_Data[1] == 0xd1u )
                {
                    testmode = 1u;
                }   
            }
        } 
        
        
    }
    else
    {
        
    }
    
    
    senddata[0] = 0xbcu;
    if( testmode == 1u )
    {
        senddata[1] = 0x01u;
    }
    else
    {
        senddata[1] = 0x02u;
    }
    CPU_Exchange_Data(senddata, 2u);
    CPU_Data_Check(recvdata, &len);
    
    if( len == 0x02u && recvdata[0] == 0xbcu )
    {
        if( recvdata[1] == 1u )
        {
            testmode = 1u;
        }
        else if( recvdata[1] == 2u )
        {
            testmode = 0u;
        }
/*        
        CPU_Exchange_Data(senddata, 2);
        CPU_Data_Check(recvdata, &len);//recv 
        
        senddata[1] = 0xf1;
        CPU_Exchange_Data(senddata, 2);//send
        CPU_Data_Check(recvdata, &len);  
*/        
    } 
    
/*    CPU_Exchange_Data(senddata, 2);*/
}


#endif


/******************************  END OF FILE  *********************************/




