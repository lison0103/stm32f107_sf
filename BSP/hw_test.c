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

#ifdef GEC_SF_MASTER
#include "usart.h"
#include "crc16.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


u8 sflag,inputnum = 0;
u8 switch_flag = 1;
u8 sfwdt_checkflag = 0;
u8 sf_wdt_check_tms = 0;
//static u8 test_num = 0;
#ifdef GEC_SF_MASTER
u8 R_SF_RL2_FB_CPU1;
#else
u8 R_SF_RL_FB_CPU2;

#endif

extern u8 EscRTBuff[200];

/*******************************************************************************
* Function Name  : SafetyRelayAuxRelayTest
* Description    : Safety Relay and AuxRelay Test
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyRelayAuxRelayTest(void)
{
   
    
    /* Online monitoring safety relay drive failure detection */
    if( SF_RL_DRV_FB || SF_PWR_FB_CPU || SF_RL_FB || !AUX_FB )
    {
        FailSafeTest();
    }   

}

/*******************************************************************************
* Function Name  : SafetyExtWdt_StartUpCheck
* Description    : When powered on,check the safety external watchdog.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyExtWdt_StartUpCheck(void)
{
    
#ifndef GEC_SF_MASTER    
    delay_ms(700);
#endif

    if( SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB && AUX_FB )
    {
        SF_EWDT_TOOGLE();
//        AUX_CTR = 1;
        SF_RL_CTR = 1;   
        SF_EWDT_TOOGLE();
    }   
    
    
    /*  wait 1800ms */
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    
    /** Safety Relay and AuxRelay Test **/
    SafetyRelayAuxRelayTest();
    
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();    
   
    if( !SF_RL_FB )
    {
        FailSafeTest();
    }
    else
    {
//        AUX_CTR = 0;
        SF_RL_CTR = 0; 
        sfwdt_checkflag = 2;
    }   
        


}

/*******************************************************************************
* Function Name  : SafetyExtWdt_RunCheck
* Description    : When the escalator stopped,check the safety external watchdog.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyExtWdt_RunCheck(void)
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
* Function Name  : SafetyCTR_Check
* Description    : Coding protection detection in running process
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void SafetyCTR_Check(void)
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
                printf("SafetyCTR_Check error \n");
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
* Description    : Monitor the input pin status test.
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
                    
//                        AUX_CTR = 1;
                        SF_RL_CTR = 1;                         
//                        delay_ms(1);
                        
//                        SafetyExtWdt_RunCheck();
                }   
                /* Online monitoring safety relay drive failure detection */
                else if( switch_flag == 2 )
                {
                    SafetyRelayAuxRelayTest();
//                    switch_flag = 0;
//                    printf("Input_Check error \n");
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
* Description    : Monitor the input pin status and test.
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
  
    DMA_Check_Flag(100000);
    
    
        
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
                          printf("1");                             
                          break;

                      }
                  
                  }
              } 
              else
              {
                  data_error++;
                  printf("2");                  
  
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
                      printf("3");
                      break;
                  }
              }
              
              SPI1_DMA_ReceiveSendByte(comm_num);

              DMA_Check_Flag(40000);
#endif              
        }      
        

#ifdef GEC_SF_MASTER 
#else
        SPI1_DMA_ReceiveSendByte(comm_num);
#endif
    if( data_error > 2 )
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

 
/*******************************************************************************
* Function Name  : CAN_Comm
* Description    : CAN Communication test.
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_Comm(void)
{
#ifdef GEC_SF_MASTER    
    static u8 can1_comm_timeout,can2_comm_timeout = 0;
    u8 len = 0;
    
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
    
    /** receive a data packet -----------------------------------------------**/ 
    len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
    
    /* just for test */
    if( len > 0 )
    {
        CAN1_TX_Data[4] = CAN1_RX_Data[0];
        CAN1_TX_Data[5] = CAN1_RX_Data[1];
    }
    
    for( u8 i = 6; i < 50; i++ )
    {
        CAN1_TX_Data[i] = EscRTBuff[30+i];
    }
    for( u8 i = 0; i < 10; i++ )
    {
        CAN1_TX_Data[50+i] = EscRTBuff[4+i];
    }    

    CAN2_TX_Data[0] = CMD_FLAG1;
    CAN2_TX_Data[1] = EN_ERROR1;
    CAN2_TX_Data[2] = EN_ERROR2;
    
    len = BSP_CAN_Receive(CAN2, &CAN2_RX_Up, CAN1_RX_Data, 0);
    len = BSP_CAN_Receive(CAN2, &CAN2_RX_Down, CAN1_RX_Data, 0);
    
    /** CAN1 send data ------------------------------------------------------**/
    /** CB normal SEND ID:0x1314, CB URGE SEND ID:0x1234 **/
    BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, CAN1_TX_Data, 100);
    BSP_CAN_Send(CAN1, &CAN1_TX_Urge, CAN1TX_URGE_ID, CAN2_TX_Data, 20);
    
    /** CAN2 send data ------------------------------------------------------**/
    /** DBL1 UP SEND ID:0X1234, DBL1 DOWN SEND ID:0x2345 **/
    BSP_CAN_Send(CAN2, &CAN2_TX_Up, CAN2TX_UP_ID, CAN2_TX_Data, 8);  
    BSP_CAN_Send(CAN2, &CAN2_TX_Down, CAN2TX_DOWN_ID, CAN2_TX_Data, 8);   
    
#else
    static u8 can1_comm_timeout = 0;
    u8 len = 0;
    
    /* if config tandem, use cpu2 CAN1 to communication */
    if( TANDEM_TYPE != 0 )
    {
        if( can1_receive == 1 )
        {
            can1_receive = 0;
            can1_comm_timeout = 0;
        }
        else if( ++can1_comm_timeout >= 3 )
        {
            /*  can communication timeout process */
        }  
        
        /** receive a data packet -----------------------------------------------**/ 
        len = BSP_CAN_Receive(CAN1, &CAN1_RX_Normal, CAN1_RX_Data, 0);
        
        /* for tandem communication */
        if( len > 0 )
        {
            TandemMessageRunAllowed = 0;
            
            if( SfBase_EscState & ESC_STATE_READY )
            {
                TandemMessageRunAllowed = CAN1_RX_Data[1];
            }
            else if( SfBase_EscState & ESC_STATE_RUNNING )
            {
                TandemMessageRunAllowed = CAN1_RX_Data[2];
            }

        }
        CAN1_TX_Data[0] = TANDEM_TYPE;
        CAN1_TX_Data[1] = TandemRunEnable;
        CAN1_TX_Data[2] = Tandemoutput;    
        
        /** CAN1 send data ------------------------------------------------------**/
        /** CB normal SEND ID:0x1314 **/
        BSP_CAN_Send(CAN1, &CAN1_TX_Normal, CAN1TX_NORMAL_ID, CAN1_TX_Data, 20);   
    }
    
#endif
}



/******************************  END OF FILE  *********************************/




