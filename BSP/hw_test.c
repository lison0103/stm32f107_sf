/*******************************************************************************
* File Name          : hw_test.c
* Author             : lison
* Version            : V1.0
* Date               : 03/22/2016
* Description        : 
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

#ifdef GEC_SF_MASTER
#include "usbd_cdc_vcp.h"
#include "usart.h"
#include "crc16.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define CPU_COMM_TIMEOUT  5

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void CPU_Data_Check(void);
void CPU_Exchange_Data(void);

u8 sflag,inputnum = 0;
u8 switch_flag = 1;
u8 sfwdt_checkflag = 0;
u16 comm_num = 0;
u8 sf_wdt_check_tms = 0;
u8 onetime = 0;
//static u8 test_num = 0;
#ifdef GEC_SF_MASTER
u8 R_SF_RL2_FB_CPU1;
#else
u8 R_SF_RL_FB_CPU2;
static u16 comm_timeout = 100;
static u16 cntt = 0;
#endif

/*******************************************************************************
* Function Name  : SF_WDT_Check
* Description    : Safety relay output circuit
*                  安全继电器输出电路
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SF_WDT_Check(void)
{
    
    sfwdt_checkflag = 1;
    
    sf_wdt_check_tms++;
    /*  (41 - 1)*45ms = 1800ms */
    if( sf_wdt_check_tms >= 41)
    {
        
        sf_wdt_check_tms = 0;
   
        if( !SF_RL_FB )
        {
              switch_flag = 0;
              sfwdt_checkflag = 0;
        }
        else
        {
            sfwdt_checkflag = 2;
            SF_EWDT_TOOGLE();
            printf("SF_WDT_Check---ok! \r\n");
        }   
        
    }

}


/*******************************************************************************
* Function Name  : CPU_Exchange_Data_Check
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CPU_Comm(void)
{

#ifdef GEC_SF_MASTER
          if( onetime == 0 )
          {
              onetime++;
              CPU_Exchange_Data();
          }
          else
          {
              CPU_Data_Check();
              CPU_Exchange_Data();
          }
#else  
          comm_timeout--;
//          printf("CPU_Comm---comm_timeout:%d\r\n",comm_timeout);
          if( comm_timeout == 0 )
          {
              printf("CPU_Comm---comm_timeout \r\n");
              
              ESC_SPI_Error_Process();
              comm_timeout = CPU_COMM_TIMEOUT;
          }
          if ( ( DMA_GetFlagStatus(DMA1_IT_TC2) ) != RESET )
          {
                
                if(cntt != 0)
                {
                    cntt--;
                    comm_timeout = 100;
                }
                else
                {
                  comm_timeout = CPU_COMM_TIMEOUT;
                }
                CPU_Data_Check();
                CPU_Exchange_Data();
          }   
#endif
}

/*******************************************************************************
* Function Name  : CPU_Data_Check
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CPU_Data_Check(void)
{ 
    
    
#if 1
    
          /* communication buffer */
          comm_num = buffersize;  

    
         DMA_Check_Flag(2000);    
             
          
          if(!MB_CRC16(SPI1_RX_Data, comm_num))
          {
                            
              if(SPI1_TX_Data[4] == 2 && SPI1_RX_Data[4] == 2)
              {
                  switch_flag = 2;
                  sfwdt_checkflag = 0;
              }
              else if( sfwdt_checkflag == 2)
              {
                  EN_ERROR_SYS3++;
                  printf("sfwdt_checkflag error \r\n");
              }
              
              if( switch_flag == 2 )
              {
                  if( (SPI1_TX_Data[0] != SPI1_RX_Data[0]) || (SPI1_TX_Data[1] != SPI1_RX_Data[1]) 
                     || (SPI1_TX_Data[2] != SPI1_RX_Data[2]) || (SPI1_TX_Data[3] != SPI1_RX_Data[3]) )
                  {
                      EN_ERROR_SYS3++;
                      printf("data error \r\n");
                  }
                  else
                  {
                      EN_ERROR_SYS3 = 0;
                  }
              }
          
          }
          else
          {
              EN_ERROR_SYS3++;     
              printf("MB_CRC16 error \r\n");
              
#if DEBUG_PRINTF            

              printf("SPI1_RX_Data : %01d, %01d, %01d, %01d ,%01d, %01d, %01d \r\n",
                     SPI1_RX_Data[0],SPI1_RX_Data[1],SPI1_RX_Data[2],SPI1_RX_Data[3]
                     ,SPI1_RX_Data[4],SPI1_RX_Data[comm_num - 2],SPI1_RX_Data[comm_num - 1]);

              printf("SPI1_TX_Data : %01d, %01d, %01d, %01d ,%01d, %01d, %01d \r\n",
                     SPI1_TX_Data[0],SPI1_TX_Data[1],SPI1_TX_Data[2],SPI1_TX_Data[3]
                     ,SPI1_TX_Data[4],SPI1_TX_Data[comm_num - 2],SPI1_TX_Data[comm_num - 1]);
#endif                
          }
          
          if(EN_ERROR_SYS3 > 2)
          {
              EN_ERROR_SYS3 = 0;
              ESC_SafeRelay_Error_Process();
              printf("CPU_Exchange_Data_Check error \r\n");
          }
          
          
#else
         u8 i;
          /* communication buffer */
          comm_num = buffersize;  

    
         DMA_Check_Flag(2000);          
         
        
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {
//#ifdef GEC_SF_MASTER 
//                  for( i=0; i < comm_num - 2; i++ )
//                  {
//
//                      if( (SPI1_RX_Data[i]^(SPI1_TX_Data[i] - 1)) )
//                      {
//                          EN_ERROR_SYS3++;
//                          break;
//                      }
//                  
//                  }
//#endif
              } 
              else
              {
                          EN_ERROR_SYS3++;    
              }          

                        

          
          if(EN_ERROR_SYS3 > 3)
          {
#if DEBUG_PRINTF            

              printf("RX: %01d, %01d, %01d, %01d ,%01d, %01d, %01d \r\n",
                     SPI1_RX_Data[0],SPI1_RX_Data[1],SPI1_RX_Data[2],SPI1_RX_Data[3]
                     ,SPI1_RX_Data[4],SPI1_RX_Data[comm_num - 2],SPI1_RX_Data[comm_num - 1]);

              printf("TX : %01d, %01d, %01d, %01d ,%01d, %01d, %01d \r\n",
                     SPI1_TX_Data[0],SPI1_TX_Data[1],SPI1_TX_Data[2],SPI1_TX_Data[3]
                     ,SPI1_TX_Data[4],SPI1_TX_Data[comm_num - 2],SPI1_TX_Data[comm_num - 1]);
#endif 
                LED = 0;
                while(1)
                {
                    EWDT_TOOGLE(); 
                    IWDG_ReloadCounter();        
                }
          }
          
#endif
             
}

/*******************************************************************************
* Function Name  : CPU_Exchange_Data
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CPU_Exchange_Data(void)
{
#if 1
    
          u16 i;
         
          /* communication buffer */
          comm_num = buffersize;  
          for(i = 0; i < comm_num - 2; i++)
          {
              SPI1_TX_Data[i] = 0;
          }
    
          SPI_DMA_RECEIVE_FLAG = 0;
 
          if(SF_RL_DRV_FB)
            SPI1_TX_Data[0] = 0x01;
         
          if(SF_PWR_FB_CPU)
            SPI1_TX_Data[1] = 0x01;
          
          if(SF_RL_FB)
            SPI1_TX_Data[2] = 0x01;
            
          if(AUX_FB)
            SPI1_TX_Data[3] = 0x01;
               
          SPI1_TX_Data[4] = sfwdt_checkflag;
          
          SPI1_TX_Data[5] = CAN1_TX_Data[0];
          SPI1_TX_Data[6] = CAN1_TX_Data[1];
          SPI1_TX_Data[7] = CAN1_TX_Data[2];
          SPI1_TX_Data[8] = CAN1_TX_Data[3];
          
          i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
          SPI1_TX_Data[comm_num - 2] = i;
          SPI1_TX_Data[comm_num - 1] = i>>8; 
          
          SPI1_DMA_ReceiveSendByte(comm_num);
#else
          u16 i;
         
          /* communication buffer */
          comm_num = buffersize;  
#ifdef GEC_SF_MASTER          
          test_num++;
          if(test_num > 255)test_num = 0;
          
          for( i = 0; i < comm_num - 2; i++ )
          {
              SPI1_TX_Data[i] = test_num;
          }
#else
          for( i=0; i < comm_num - 2; i++ )
          {
              SPI1_TX_Data[i] = SPI1_RX_Data[i];
          }   
#endif          
          i = MB_CRC16( SPI1_TX_Data, comm_num - 2 );
          SPI1_TX_Data[comm_num - 2] = i;
          SPI1_TX_Data[comm_num - 1] = i>>8;
          
          SPI1_DMA_ReceiveSendByte(comm_num);
#endif            
}

/*******************************************************************************
* Function Name  : SF_CTR_Check
* Description    : Coding protection detection in running process
*                  运行过程中进行代码保护检查 
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SF_CTR_Check(void)
{
 
      if(switch_flag == 2)
      {
        SF_RL_CTR = 0;
        delay_us(150);
        if(!SF_RL_DRV_FB)
        {
            EN_ERROR_SYS2++;
            if(EN_ERROR_SYS2 > 2)
            {
                printf("SF_CTR_Check error \n");
                ESC_SafeRelay_Error_Process();
            }
        }
        else
        {
            EN_ERROR_SYS2 = 0;
        }
        SF_RL_CTR = 1;
      }

}

/*******************************************************************************
* Function Name  : Input_Check
* Description    :  
*                   
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Input_Check(void)
{  

  
        for(u8 i=0;i<4;i++)
        {
          CAN1_TX_Data[i]= 0x0;                 
        }
        
        /**************************** test input *************
        IN1 = IN2 = IN3 = IN4 = IN5 = IN6 =  IN7 = IN8 =  0;  
        IN9 = IN10 = IN11 = IN12 = IN13 = IN14 = IN15 = IN16 = IN17 = IN18 = IN19 = IN20 = IN21 = IN22 = IN23 = IN24 =  IN25 = IN26 = IN27 = IN28 = 1;
        *****************************************************/
        
        /****test input,The actual test should be uncommented****/
        if(switch_flag  /*&& ( !IN1 && !IN2 && !IN3 && !IN4 && !IN5 && !IN6 && !IN7 && !IN8 && 
                        IN9 && IN10 && IN11 && IN12 && IN13 && IN14 && IN15 && IN16 && 
                          IN17 && IN18 && IN19 && IN20 && IN21 && IN22 && IN23 && IN24 && IN25 && IN26 && IN27 && IN28 )*/)
        {
          
                /* System does the self-test for safety relay, running relay and auxiliary brake relay. */
                if( ( sfwdt_checkflag == 1 ) || ( SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB && AUX_FB ) )
                {
                    
                        AUX_CTR = 1;
                        SF_RL_CTR = 1;                         
//                        delay_ms(1);
                        
                        SF_WDT_Check();
                }   
                /* Online monitoring safety relay drive failure detection */
                else if(( switch_flag == 2 ) && ( SF_RL_DRV_FB || SF_PWR_FB_CPU || SF_RL_FB || AUX_FB ))
                {
                    switch_flag = 0; 
                    printf("Input_Check error \n");
                }                

                
//                for(u8 i=0;i<3;i++)
//                {
//                    CAN1_TX_Data[i]= 0xff;                 
//                }
//                CAN1_TX_Data[3] = 0x0f;
          
//        }
//        else
//        {
//                switch_flag = 0;
//                AUX_CTR = 0; 
//                SF_RL_CTR = 0;            

                /****test input,The actual test should be commented****/
//                CAN1_TX_Data[0] = 0xff;
//                CAN1_TX_Data[1] = 0xff;
//                CAN1_TX_Data[2] = 0xfc;
//                CAN1_TX_Data[3] = 0x0f;
                /*****************************************************/
        
                  if ( !IN1 ) 
                  { 
                      CAN1_TX_Data[0] |= 1 << 0;                 
                  }
                  if ( !IN2 ) 
                  {       
                      CAN1_TX_Data[0] |= 1 << 1;
                  }
                  if ( !IN3 ) 
                  {                   
                      CAN1_TX_Data[0] |= 1 << 2; 

                  }
                  if ( !IN4 ) 
                  {
                      CAN1_TX_Data[0] |= 1 << 3;
                  } 
                  if ( !IN5 ) 
                  {           
                      CAN1_TX_Data[0] |= 1 << 4;
                  }
                  if ( !IN6 ) 
                  {          
                      CAN1_TX_Data[0] |= 1 << 5;

                  }
                  if ( !IN7 ) 
                  {         
                      CAN1_TX_Data[0] |= 1 << 6;

                  }        
                  if ( !IN8 ) 
                  {         
                      CAN1_TX_Data[0] |= 1 << 7;

                  }
                  if ( IN9 ) 
                  {     
                      CAN1_TX_Data[1] |= 1 << 0;

                  }
                  if ( IN10 ) 
                  {                
                      CAN1_TX_Data[1] |= 1 << 1;

                  } 
                  if ( IN11 ) 
                  {          
                      CAN1_TX_Data[1] |= 1 << 2;

                  }
                  if ( IN12 ) 
                  {
                      CAN1_TX_Data[1] |= 1 << 3;

                  }
                  if ( IN13 ) 
                  {   
                      CAN1_TX_Data[1] |= 1 << 4; 

                  }         
                  if ( IN14 ) 
                  {    
                      CAN1_TX_Data[1] |= 1 << 5; 

                  }
                  if ( IN15 ) 
                  { 
                      CAN1_TX_Data[1] |= 1 << 6;

                  }
                  if ( IN16 ) 
                  {
                      CAN1_TX_Data[1] |= 1 << 7; 

                  }    
                  if ( IN17 ) 
                  {          
                      CAN1_TX_Data[2] |= 1 << 0;

                  }
                  if ( IN18 ) 
                  {
                      CAN1_TX_Data[2] |= 1 << 1;

                  }
                  if ( IN19 ) 
                  {   
                      CAN1_TX_Data[2] |= 1 << 2;

                  }         
                  if ( IN20 ) 
                  {    
                      CAN1_TX_Data[2] |= 1 << 3; 

                  }
                  if ( IN21 ) 
                  { 
                      CAN1_TX_Data[2] |= 1 << 4;

                  }
                  if ( IN22 ) 
                  {
                      CAN1_TX_Data[2] |= 1 << 5;

                  } 
                  if ( IN23 ) 
                  {
                      CAN1_TX_Data[2] |= 1 << 6;

                  }
                  if ( IN24 ) 
                  {   
                      CAN1_TX_Data[2] |= 1 << 7;

                  }         
                  if ( IN25 ) 
                  {    
                      CAN1_TX_Data[3] |= 1 << 0;

                  }
                  if ( IN26 ) 
                  { 
                      CAN1_TX_Data[3] |= 1 << 1;

                  }
                  if ( IN27 ) 
                  {
                      CAN1_TX_Data[3] |= 1 << 2; 

                  }  
                  if ( IN28 ) 
                  {
                      CAN1_TX_Data[3] |= 1 << 3; 

                  } 
                  
#if EXTERNAL_INPUT_TEST
                  
                  if ( EX_IN1 )
                  {
                      inputnum = 29;
                      sflag++;
                  }
                  if ( EX_IN2 ) 
                  {       
                      inputnum = 30;
                      sflag++;
                  }
                  if ( EX_IN3 ) 
                  {                   
                      inputnum = 31;
                      sflag++;

                  }
                  if ( EX_IN4 ) 
                  {
                      inputnum = 32;
                      sflag++;
                  } 
                  if ( EX_IN5 ) 
                  {           
                      inputnum = 33;
                      sflag++;
                  }
                  if ( EX_IN6 ) 
                  {          
                      inputnum = 34;
                      sflag++;

                  }
                  if ( EX_IN7 ) 
                  {         
                      inputnum = 35;
                      sflag++;

                  }        
                  if ( EX_IN8 ) 
                  {         
                      inputnum = 36;
                      sflag++;

                  }
                  if ( EX_IN9 ) 
                  {     
                      inputnum = 37;
                      sflag++;

                  }
                  if ( EX_IN10 ) 
                  {                
                      inputnum = 38;
                      sflag++;

                  } 
                  if ( EX_IN11 ) 
                  {          
                      inputnum = 39;
                      sflag++;

                  }
                  if ( EX_IN12 ) 
                  {
                      inputnum = 40;
                      sflag++;

                  }
                  if ( EX_IN13 ) 
                  {   
                      inputnum = 41;
                      sflag++;

                  }         
                  if ( EX_IN14 ) 
                  {    
                      inputnum = 42;
                      sflag++;

                  }
                  if ( EX_IN15 ) 
                  { 
                      inputnum = 43;
                      sflag++;

                  }
                  if ( EX_IN16 ) 
                  {
                      inputnum = 44;
                      sflag++;

                  }    
                  if ( EX_IN17 ) 
                  {          
                      inputnum = 45;
                      sflag++;

                  }  
                  
#endif
                  
        }
        else
        {
                switch_flag = 0;
                AUX_CTR = 0; 
                SF_RL_CTR = 0;          
        }
        
        
}        
        

/*******************************************************************************
* Function Name  : Input_Check2
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void Input_Check2(void)
{
  
//    u8 sflag,t,inputnum = 0;
//  
//    AUX_CTR = 0;
//    SF_RL_CTR = 0;
//    SF_RL_WDT = 1;
    
//    while(1)
//    {
        sflag = 0;
        inputnum = 0;        
        
        if ( !IN1 )
        {
            inputnum = 1;
            sflag++;
        }
        if ( !IN2 ) 
        {       
            inputnum = 2;
            sflag++;
        }
        if ( !IN3 ) 
        {                   
            inputnum = 3;
            sflag++;

        }
        if ( !IN4 ) 
        {
            inputnum = 4;
            sflag++;
        } 
        if ( !IN5 ) 
        {           
            inputnum = 5;
            sflag++;
        }
        if ( !IN6 ) 
        {          
            inputnum = 6;
            sflag++;

        }
        if ( !IN7 ) 
        {         
            inputnum = 7;
            sflag++;

        }        
        if ( !IN8 ) 
        {         
            inputnum = 8;
            sflag++;

        }
        if ( IN9 ) 
        {     
            inputnum = 9;
            sflag++;

        }
        if ( IN10 ) 
        {                
            inputnum = 10;
            sflag++;

        } 
        if ( IN11 ) 
        {          
            inputnum = 11;
            sflag++;

        }
        if ( IN12 ) 
        {
            inputnum = 12;
            sflag++;

        }
        if ( IN13 ) 
        {   
            inputnum = 13;
            sflag++;

        }         
        if ( IN14 ) 
        {    
            inputnum = 14;
            sflag++;

        }
        if ( IN15 ) 
        { 
            inputnum = 15;
            sflag++;

        }
        if ( IN16 ) 
        {
            inputnum = 16;
            sflag++;

        }    
        if ( IN17 ) 
        {          
            inputnum = 17;
            sflag++;

        }
        if ( IN18 ) 
        {
            inputnum = 18;
            sflag++;

        }
        if ( IN19 ) 
        {   
            inputnum = 19;
            sflag++;

        }         
        if ( IN20 ) 
        {    
            inputnum = 20;
            sflag++;

        }
        if ( IN21 ) 
        { 
            inputnum = 21;
            sflag++;

        }
        if ( IN22 ) 
        {
            inputnum = 22;
            sflag++;

        } 
        if ( IN23 ) 
        {
            inputnum = 23;
            sflag++;

        }
        if ( IN24 ) 
        {   
            inputnum = 24;
            sflag++;

        }         
        if ( IN25 ) 
        {    
            inputnum = 25;
            sflag++;

        }
        if ( IN26 ) 
        { 
            inputnum = 26;
            sflag++;

        }
        if ( IN27 ) 
        {
            inputnum = 27;
            sflag++;

        }  
        if ( IN28 ) 
        {
            inputnum = 28;
            sflag++;

        } 

#if EXTERNAL_INPUT_TEST        
        
        if ( EX_IN1 )
        {
            inputnum = 29;
            sflag++;
        }
        if ( EX_IN2 ) 
        {       
            inputnum = 30;
            sflag++;
        }
        if ( EX_IN3 ) 
        {                   
            inputnum = 31;
            sflag++;

        }
        if ( EX_IN4 ) 
        {
            inputnum = 32;
            sflag++;
        } 
        if ( EX_IN5 ) 
        {           
            inputnum = 33;
            sflag++;
        }
        if ( EX_IN6 ) 
        {          
            inputnum = 34;
            sflag++;

        }
        if ( EX_IN7 ) 
        {         
            inputnum = 35;
            sflag++;

        }        
        if ( EX_IN8 ) 
        {         
            inputnum = 36;
            sflag++;

        }
        if ( EX_IN9 ) 
        {     
            inputnum = 37;
            sflag++;

        }
        if ( EX_IN10 ) 
        {                
            inputnum = 38;
            sflag++;

        } 
        if ( EX_IN11 ) 
        {          
            inputnum = 39;
            sflag++;

        }
        if ( EX_IN12 ) 
        {
            inputnum = 40;
            sflag++;

        }
        if ( EX_IN13 ) 
        {   
            inputnum = 41;
            sflag++;

        }         
        if ( EX_IN14 ) 
        {    
            inputnum = 42;
            sflag++;

        }
        if ( EX_IN15 ) 
        { 
            inputnum = 43;
            sflag++;

        }
        if ( EX_IN16 ) 
        {
            inputnum = 44;
            sflag++;

        }    
        if ( EX_IN17 ) 
        {          
            inputnum = 45;
            sflag++;

        }       
#endif        

        if ( inputnum == 0 )
        {
            AUX_CTR = 0; 
            SF_RL_CTR = 0;
        }
        else if ( sflag > 1 )
        {
            AUX_CTR = 0; 
            SF_RL_CTR = 0;
        }
        else if ( inputnum && ( inputnum % 2 ) )
        {
            AUX_CTR = 1; 
        }
        else if ( inputnum )
        {
            SF_RL_CTR = 1; 
        }
               
        
//        delay_ms(1);
//        t++;
//        if(t==200)
//        {
//            LED=!LED;
//            SF_RL_WDT=!SF_RL_WDT;
//            t=0;
//        }        
//        
//    }
              
        
}


/*******************************************************************************
* Function Name  : SPI1_DMA_Check
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
u8 number = 0;
u8 data_error = 0;
u32 test_cnt = 100;

void SPI1_DMA_Check(void)
{    

  u16 i = 0;  
  comm_num = buffersize;
  
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
  
    DMA_Check_Flag(400000);
    
    
        
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
                        
              DMA_Check_Flag(400000);
              
              if(!MB_CRC16(SPI1_RX_Data, comm_num))
              {

                  for( i=0; i < comm_num - 2; i++ )
                  {
                      result = SPI1_RX_Data[i]^(SPI1_TX_Data[i] - 1);
                      if( result )
                      {
                          data_error = 1;
                          break;
                      }
                  
                  }
              } 
              else
              {
                          data_error = 1;    
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
                  data_error = 1;    
                  break;
              }
              
              SPI1_DMA_ReceiveSendByte(comm_num);

              DMA_Check_Flag(400000);
#endif              
        }      
        

#ifdef GEC_SF_MASTER 
#else
        SPI1_DMA_ReceiveSendByte(comm_num);
#endif
    if( data_error == 1 )
    {
        LED = 0;
        printf("SPI1_DMA_Check error \r\n");
        while(1)
        {
            EWDT_TOOGLE(); 
            IWDG_ReloadCounter();        
        }
    }


}

#ifdef GEC_SF_MASTER 
/*******************************************************************************
* Function Name  : can_comm
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_Comm(void)
{
    static u8 can1_comm_timeout,can2_comm_timeout = 0;
    static u8 can1_send_cnt = 0;
    u8 * p_CanBuff;
    u16 i = 0;
    
    if( can1_receive == 1 )
    {
        can1_receive = 0;
        can1_comm_timeout = 0;
    }
    else if( ++can1_comm_timeout >= 3 )
    {
        /*  can communication timeout process */
    }  
    
    if( can2_receive == 1 )
    {
        can2_receive = 0;
        can2_comm_timeout = 0;
    }
    else if( ++can2_comm_timeout >= 3 )
    {
        /*  can communication timeout process */
    }    

    /** packet the data pack **/
    if( can1_send_cnt == 0)
    {
        CAN1_TX_Buff[0] = 0xfa;
        CAN1_TX_Buff[1] = canbuffsize - 4;
        for( u8 j = 0; j < canbuffsize - 4; j++ )
        {
            CAN1_TX_Buff[j+2] = CAN1_TX_Data[j];
        }
        i = MB_CRC16( CAN1_TX_Buff, canbuffsize - 2 );
        CAN1_TX_Buff[canbuffsize - 2] = i;
        CAN1_TX_Buff[canbuffsize - 1] = i>>8;    
    }    
    /** CAN1 send data **/
    /** CB normal SEND ID:0x1314, CB URGE SEND ID:0x1234 **/
    if( can1_send_cnt >= canbuffsize/24 )
    {
        p_CanBuff = &CAN1_TX_Buff[ 24*can1_send_cnt ];
        Can_Send_Msg(CAN1,0x1314,p_CanBuff,canbuffsize%24); 
        can1_send_cnt = 0;
    }    
    else
    {
        for( i = 0; i < 3; i++ )
        {
            p_CanBuff = &CAN1_TX_Buff[ (8*i) + (24*can1_send_cnt) ];
            Can_Send_Msg(CAN1,0x1314,p_CanBuff,8); 
        }
        can1_send_cnt++;
    }
//    Can_Send_Msg(CAN1,0x1234,CAN1_TX_Buff,4);

    /** CAN2 send data **/
    /** DBL1 UP SEND ID:0X1234, DBL1 DOWN SEND ID:0x2345 **/
    Can_Send_Msg(CAN2,0x1234,CAN1_TX_Buff,4);   
    Can_Send_Msg(CAN2,0x2345,CAN1_TX_Buff,4);   
}
#endif


/******************************  END OF FILE  *********************************/




