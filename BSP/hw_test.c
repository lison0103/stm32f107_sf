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
u8 sflag1 = 0,inputnum1 = 0;
u8 sflag2 = 0,inputnum2 = 0;

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
    
    for( i = 0; i < 4; i++ )
    {
        CAN1_TX_Data[i]= 0x0;                 
    }
    
    for( i = 0; i < 28; i++ )
    {
        if( ulPt_Input[0] & ( 1 << i ))
        {
            *pc_can_tx |= 1 << i;
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
        u8 i;


        if( testmode == 1 )
        {
            ulPt_Input1 = (u32*)&EscRTBuff[4];       
            ulPt_Input2 = (u32*)&EscRTBuff[8];
            ulPt_Input3 = (u32*)&McRxBuff[4];       
            ulPt_Input4 = (u32*)&McRxBuff[8];      
            
            sflag1 = 0;
            inputnum1 = 0;      
            sflag2 = 0;
            inputnum2 = 0;       
            
            
            for( i = 0; i < 29; i++ )
            {
                if( *ulPt_Input1 & ((u32)( 1 << i )))
                {
                    sflag1++;
                    inputnum1 = i + 1;
                }
            }
            
            for( i = 0; i < 17; i++ )
            {
                if( *ulPt_Input2 & ((u32)( 1 << i )))
                {
                    sflag1++;
                    inputnum1 = i + 30;
                }
            }  
            
            for( i = 0; i < 29; i++ )
            {
                if( *ulPt_Input3 & ((u32)( 1 << i )))
                {
                    sflag2++;
                    inputnum2 = i + 1;
                }
            }
            
            for( i = 0; i < 17; i++ )
            {
                if( *ulPt_Input4 & ((u32)( 1 << i )))
                {
                    sflag2++;
                    inputnum2 = i + 30;
                }
            }             

            if( sflag1 != sflag2 || inputnum1 != inputnum2 )
            {
                sflag1 = 2;
            }
            
#ifdef GEC_SF_MASTER             
            CAN2_TX_Data[0] = inputnum1;
            CAN2_TX_Data[1] = sflag1;            
#endif         
            
            if (( inputnum1== 0 ) || ( sflag1 > 1 ))
            {
                SF_RELAY_OFF(); 
                AUX_RELAY_OFF();
            }
#ifdef GEC_SF_MASTER   
            else if( inputnum1 & 0x0A )
            {
                if ( inputnum1 & 0x08 )
                {
                    SF_RELAY_ON(); 
                }
                
                if ( inputnum1 & 0x02 )
                {
                    AUX_RELAY_ON(); 
                } 
            }
#else
            else if( inputnum1 & 0x05 )
            {
                if ( inputnum1 & 0x04 )
                {
                    SF_RELAY_ON(); 
                }
                
                if ( inputnum1 & 0x01 )
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

    u16 i = 0;  
    u8 number = 0;
    u8 data_error = 0;
    u32 test_cnt = 100;  
    u16 comm_num = buffersize;
  
#ifdef GEC_SF_MASTER      
    u8 result = 0;
#endif
        
    number++;

    for( i = 0; i < comm_num; i++)
    {
        SPI1_TX_Data[i] = number;
    }
    
    i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
    SPI1_TX_Data[comm_num - 2] = i;
    SPI1_TX_Data[comm_num - 1] = i>>8;
    
    
    SPI1_DMA_ReceiveSendByte(comm_num);

#ifdef GEC_SF_MASTER
    DMA_Check_Flag(100000);
#else
    DMA_Check_Flag(100000000);
#endif
    
        
        while(test_cnt--)
        {
            
            EWDT_TOOGLE(); 
            IWDG_ReloadCounter();             
        
#ifdef GEC_SF_MASTER              
              
              delay_ms(2);
              
              number++;
              if(number >= 255)number = 0;

              for( i = 0; i < comm_num - 2; i++ )
              {
                    SPI1_TX_Data[i] = number;
              }
              
              i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
              SPI1_TX_Data[comm_num - 2] = i;
              SPI1_TX_Data[comm_num - 1] = i>>8;
              
              SPI1_DMA_ReceiveSendByte(comm_num);
                        
              DMA_Check_Flag(40000);
              
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {

                  for( i=0; i < comm_num - 2; i++ )
                  {
                      result = SPI1_RX_Data[i]^(SPI1_TX_Data[i] - 1);
                      if( result )
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
              
              if( data_error > 2 )
              {
                  break;
              }               
              
              
#else
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {                 
                  for( i=0; i < comm_num - 2; i++ )
                  {
                      SPI1_TX_Data[i] = SPI1_RX_Data[i];
                  }   
                  
                  i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
                  SPI1_TX_Data[comm_num - 2] = i;
                  SPI1_TX_Data[comm_num - 1] = i>>8;
              } 
              else
              {
                  data_error++;
                  if( data_error > 2 )
                  {
                      break;
                  }
              }
              
              SPI1_DMA_ReceiveSendByte(comm_num);

              DMA_Check_Flag(40000);
#endif              
        }      
        

#ifdef GEC_SF_MASTER 
#else
//        SPI1_DMA_ReceiveSendByte(comm_num);
#endif
    if( data_error > 2 )
    {
        /* SPI1_DMA_Check error */
        EN_ERROR7 |= 0x01;
//        FailSafeTest();
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
    u8 testerror = 0;
    u8 len = 0, len1 = 0, len2 = 0;
    u16 waittms = 0;
    u8 senddata[50],recvdata[50];
    u8 i;
    
    CAN1_TX_Data[0] = 0xf1;
    CAN2_TX_Data[0] = 0xf1;
    testdata1[0] = 0xf1;
    testdata2[0] = 0xf1;
    for( i = 1; i < 10 ; i++ )
    {
        CAN1_TX_Data[i] = i + 0xd0;
        testdata1[i] = i + 0xd0;
        CAN2_TX_Data[i] = i + 0xe0;
        testdata2[i] = i + 0xe0;        
    }
    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10);
    BSP_CAN_Send(CAN2, &CAN2_TX_Up, CAN1_TEST_ID, CAN2_TX_Data, 10);
    
    do
    {
        len1 = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
        delay_ms(1);
        EWDT_TOOGLE();
        IWDG_ReloadCounter(); 
        waittms++;
        if( waittms > 5000 )
        {
            waittms = 0;
            break;
        }
    }
    while( len1 != 10 || CAN1_RX_Data[0] != 0xf1 );     

    waittms = 0;
    do
    {
        len2 = BSP_CAN_Receive(CAN2, &CAN2_RX_Up, CAN2_RX_Data, 0);
        delay_ms(1);
        EWDT_TOOGLE();
        IWDG_ReloadCounter(); 
        waittms++;
        if( waittms > 100 )
        {
            waittms = 0;
            break;
        }
    }
    while( len2 != 10 || CAN2_RX_Data[0] != 0xf1 );   
    
    if( len1 == 10 && CAN1_RX_Data[0] == 0xf1 )
    {
        waittms = 0;
        for( i = 2; i < 10 ; i++ )
        {
            CAN1_TX_Data[i] = CAN1_RX_Data[i];
        }
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10);
        
        if( len2 == 10 && CAN2_RX_Data[0] == 0xf1 )
        {
            waittms = 0;
            for( i = 2; i < 10 ; i++ )
            {
                CAN2_TX_Data[i] = CAN2_RX_Data[i];
            }
            BSP_CAN_Send(CAN2, &CAN2_TX_Up, CAN1_TEST_ID, CAN2_TX_Data, 10);
        }        
        
        waittms = 0;
        do
        {
            len1 = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
            delay_ms(1);
            EWDT_TOOGLE();
            IWDG_ReloadCounter(); 
            waittms++;
            if( waittms > 2000 )
            {
                waittms = 0;
                break;
            }
        }
        while( len1 != 10 || CAN1_RX_Data[0] != 0xf1 ); 
        
        waittms = 0;
        do
        {
            len2 = BSP_CAN_Receive(CAN2, &CAN2_RX_Up, CAN2_RX_Data, 0);
            delay_ms(1);
            EWDT_TOOGLE();
            IWDG_ReloadCounter(); 
            waittms++;
            if( waittms > 2000 )
            {
                waittms = 0;
                break;
            }
        }
        while( len2 != 10 || CAN2_RX_Data[0] != 0xf1 );         
        
        
        if( len1 == 10 && CAN1_RX_Data[0] == 0xf1 && len2 == 10 && CAN2_RX_Data[0] == 0xf1 )
        {
            for( i = 2; i < 10 ; i++ )
            {
                if( CAN1_RX_Data[i] != testdata1[i] )
                {
                    testerror++;
                    break;
                }
            }

//            if( len2 == 10 && CAN2_RX_Data[0] == 0xf1 )
//            {
                for( i = 2; i < 10 ; i++ )
                {
                    if( CAN2_RX_Data[i] != testdata2[i] )
                    {
                        testerror++;
                        break;
                    }
                }
//            }  
            
            if( testerror == 0 )
            {
                if( ( CAN1_RX_Data[1] == 0xc1 && CAN2_RX_Data[1] == 0xb1 ) 
                   || ( CAN1_RX_Data[1] == 0xf1 && CAN2_RX_Data[1] == 0xb1 ) )
                {
                    testmode = 1;
                }
            }
        } 
        
        
    }   
    else
    {
        CAN_FilterInitTypeDef  	        CAN_FilterInitStructure;
        
        /* CAN1 filter init */
        CAN_FilterInitStructure.CAN_FilterNumber=0;	
        CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	
        
        CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x00C8<<3)&0xFFFF0000)>>16;	
        CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x00C8<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffff;//32 bit MASK
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xffff; 
        
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);    

        /* CAN2 filter init */
        CAN_FilterInitStructure.CAN_FilterNumber=14;	
        CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
        
        CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)CAN2RX_UP_ID << 3) & 0xFFFF0000 ) >> 16;	
        CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)CAN2RX_UP_ID << 3) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFF;
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((((u32)(~( CAN2RX_UP_ID ^ CAN2RX_DOWN_ID ))) << 3) & 0xFFFF0000) >> 16;
        CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((((u32)(~( CAN2RX_UP_ID ^ CAN2RX_DOWN_ID ))) << 3) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFF;   
        
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);	        
    }
    

    senddata[0] = 0xbc;
    if( testmode == 1 )
    {
        senddata[1] = 0x01;
    }
    else
    {
        senddata[1] = 0x02;
    }
    CPU_Exchange_Data(senddata, 2);//send
    CPU_Data_Check(recvdata, &len);
    
//    delay_ms(10);
//    CPU_Exchange_Data(senddata, 2);
//    CPU_Data_Check(recvdata, &len); //recv  
//    
//    if( len == 0x02 && recvdata[0] == 0xbc )
//    {
//        delay_ms(10);
//        CPU_Exchange_Data(senddata, 2);
//        CPU_Data_Check(recvdata, &len); //recv      
//        if( len == 0x02 && recvdata[0] == 0xbc )
//        {
//            if( recvdata[1] == 0xf1 )
//            {
//                testmode = 1;
//            }
//            else
//            {
//                testmode = 0;
//            }            
//        }
//    }
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
    u8 testerror = 0;
    u8 len = 0;
    u16 waittms = 0;
    u8 senddata[10],recvdata[10];

    CAN1_TX_Data[0] = 0xf1;
    testdata[0] = 0xf1;
    for( u8 i = 1; i < 10 ; i++ )
    {
        CAN1_TX_Data[i] = i + 0xf0;
        testdata[i] = i + 0xf0;
    }
    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10);
    
    do
    {
        len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
        delay_ms(1);
        EWDT_TOOGLE();
        IWDG_ReloadCounter(); 
        waittms++;
        if( waittms > 2000 )
        {
            waittms = 0;
            break;
        }
    }
    while( len != 10 || CAN1_RX_Data[0] != 0xf1 );     
    
    if( len == 10 && CAN1_RX_Data[0] == 0xf1 )
    {
        waittms = 0;
        for( u8 i = 2; i < 10 ; i++ )
        {
            CAN1_TX_Data[i] = CAN1_RX_Data[i];
        }
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1_TEST_ID, CAN1_TX_Data, 10);
        
        do
        {
            len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
            delay_ms(1);
            EWDT_TOOGLE();
            IWDG_ReloadCounter(); 
            waittms++;
            if( waittms > 8000 )
            {
                waittms = 0;
                break;
            }
        }
        while( len != 10 || CAN1_RX_Data[0] != 0xf1 ); 
        
        if( len == 10 && CAN1_RX_Data[0] == 0xf1 )
        {
            for( u8 i = 2; i < 10 ; i++ )
            {
                if( CAN1_RX_Data[i] != testdata[i] )
                {
                    testerror = 1;
                    break;
                }
            }
            
            if( testerror == 0 )
            {
                if( CAN1_RX_Data[1] == 0xd1 )
                {
                    testmode = 1;
                }   
            }
        } 
        
        
    }
    else
    {
        
    }
    
    
    senddata[0] = 0xbc;
    if( testmode == 1 )
    {
        senddata[1] = 0x01;
    }
    else
    {
        senddata[1] = 0x02;
    }
    CPU_Exchange_Data(senddata, 2);
    CPU_Data_Check(recvdata, &len);//recv  
    
    if( len == 0x02 && recvdata[0] == 0xbc )
    {
        if( recvdata[1] == 1 )
        {
            testmode = 1;
        }
        else if( recvdata[1] == 2 )
        {
            testmode = 0;
        }
        
//        CPU_Exchange_Data(senddata, 2);
//        CPU_Data_Check(recvdata, &len);//recv 
//        
//        senddata[1] = 0xf1;
//        CPU_Exchange_Data(senddata, 2);//send
//        CPU_Data_Check(recvdata, &len);  
    } 
    
//    CPU_Exchange_Data(senddata, 2);
}


#endif


/******************************  END OF FILE  *********************************/




