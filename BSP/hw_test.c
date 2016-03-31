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
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u8 sflag,inputnum = 0;
u8 switch_flag = 1;
u8 sfwdt_checkflag = 0;
u8 canbuf_send[8] = {0};
u16 comm_num = 0;
#ifdef GEC_SF_MASTER
u8 R_SF_RL2_FB_CPU1;
#else
u8 R_SF_RL_FB_CPU2;
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
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    delay_ms(600);
    EWDT_TOOGLE();
    IWDG_ReloadCounter();
    delay_ms(600);
    EWDT_TOOGLE(); 
    IWDG_ReloadCounter();
    delay_ms(700);//>1.8s
    EWDT_TOOGLE(); 
    IWDG_ReloadCounter();
   
    if( !SF_RL_FB )
    {
          switch_flag = 0;
          sfwdt_checkflag = 0;
    }
    else
    {
        sfwdt_checkflag = 1;
        SF_EWDT_TOOGLE();
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
void CPU_Exchange_Data_Check(void)
{
          
          u16 i;
          
          /* communication buffer */
          comm_num = buffersize;  
          for(i = 0; i < comm_num - 2; i++)
          {
              SPI1_TX_Buff[i] = 0;
          }
    
          SPI_DMA_RECEIVE_FLAG = 0;
 
          if(SF_RL_DRV_FB)
            SPI1_TX_Buff[0] = 0x01;
         
          if(SF_PWR_FB_CPU)
            SPI1_TX_Buff[1] = 0x01;
          
          if(SF_RL_FB)
            SPI1_TX_Buff[2] = 0x01;
            
          if(AUX_FB)
            SPI1_TX_Buff[3] = 0x01;
               
          SPI1_TX_Buff[4] = sfwdt_checkflag;
          
          SPI1_TX_Buff[5] = canbuf_send[0];
          SPI1_TX_Buff[6] = canbuf_send[1];
          SPI1_TX_Buff[7] = canbuf_send[2];
          SPI1_TX_Buff[8] = canbuf_send[3];
          
          i = MB_CRC16( SPI1_TX_Buff, comm_num - 2 );
          SPI1_TX_Buff[comm_num - 2] = i;
          SPI1_TX_Buff[comm_num - 1] = i>>8;          
          
          SPI1_DMA_ReceiveSendByte(comm_num); 

#ifdef GEC_SF_MASTER          
          while( SPI_DMA_RECEIVE_FLAG != 1 );
#endif          
          
          if(!MB_CRC16(SPI1_RX_Buff, comm_num))
          {
            
#if DEBUG_PRINTF            
              printf("SPI1_RX_Buff :  \n");
              for( u8 cnt = 0; cnt < 5; cnt++ )
              {
                printf(" %01d \n",SPI1_RX_Buff[cnt]);
              }
              printf("    SPI1_TX_Buff :  \n");
              for( u8 cnt = 0; cnt < 5; cnt++ )
              {
                printf(" %01d \n",SPI1_TX_Buff[cnt]);
              }
#endif                  
              if(SPI1_TX_Buff[4] == 1 && SPI1_RX_Buff[4] == 1)
              {
                  switch_flag = 2;
                  sfwdt_checkflag = 0;
              }
              else if( sfwdt_checkflag == 1)
              {
                  EN_ERROR_SYS3++;
              }
              
              if( switch_flag == 2 )
              {
                  if( (SPI1_TX_Buff[0] != SPI1_RX_Buff[0]) || (SPI1_TX_Buff[1] != SPI1_RX_Buff[1]) 
                     || (SPI1_TX_Buff[2] != SPI1_RX_Buff[2]) || (SPI1_TX_Buff[3] != SPI1_RX_Buff[3]) )
                  {
                      EN_ERROR_SYS3++;
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
          }
          
          if(EN_ERROR_SYS3 > 2)
          {
              EN_ERROR_SYS3 = 0;
              ESC_SafeRelay_Error_Process();
              printf("CPU_Exchange_Data_Check error \n");
          }
             
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
          canbuf_send[i]= 0x0;                 
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
                if(SF_RL_DRV_FB && !SF_PWR_FB_CPU && SF_RL_FB && AUX_FB)
                {
                    
                        AUX_CTR = 1;
                        SF_RL_CTR = 1;  
                        
                        delay_ms(1);
                        
                        SF_WDT_Check();
                }   
                /* Online monitoring safety relay drive failure detection */
                else if(( switch_flag == 2 ) && ( SF_RL_DRV_FB || SF_PWR_FB_CPU || SF_RL_FB || AUX_FB ))
                {
                    switch_flag = 0; 
                    printf("Input_Check error \n");
                }                

                
                for(u8 i=0;i<3;i++)
                {
                    canbuf_send[i]= 0xff;                 
                }
                canbuf_send[3] = 0x0f;
          
        }
        else
        {
                switch_flag = 0;
                AUX_CTR = 0; 
                SF_RL_CTR = 0;            

                /****test input,The actual test should be commented****/
//                canbuf_send[0] = 0xff;
//                canbuf_send[1] = 0xff;
//                canbuf_send[2] = 0xfc;
//                canbuf_send[3] = 0x0f;
                /*****************************************************/
        
                  if ( !IN1 ) 
                  { 
                      canbuf_send[0] |= 1 << 0;                 
                  }
                  if ( !IN2 ) 
                  {       
                      canbuf_send[0] |= 1 << 1;
                  }
                  if ( !IN3 ) 
                  {                   
                      canbuf_send[0] |= 1 << 2; 

                  }
                  if ( !IN4 ) 
                  {
                      canbuf_send[0] |= 1 << 3;
                  } 
                  if ( !IN5 ) 
                  {           
                      canbuf_send[0] |= 1 << 4;
                  }
                  if ( !IN6 ) 
                  {          
                      canbuf_send[0] |= 1 << 5;

                  }
                  if ( !IN7 ) 
                  {         
                      canbuf_send[0] |= 1 << 6;

                  }        
                  if ( !IN8 ) 
                  {         
                      canbuf_send[0] |= 1 << 7;

                  }
                  if ( IN9 ) 
                  {     
                      canbuf_send[1] |= 1 << 0;

                  }
                  if ( IN10 ) 
                  {                
                      canbuf_send[1] |= 1 << 1;

                  } 
                  if ( IN11 ) 
                  {          
                      canbuf_send[1] |= 1 << 2;

                  }
                  if ( IN12 ) 
                  {
                      canbuf_send[1] |= 1 << 3;

                  }
                  if ( IN13 ) 
                  {   
                      canbuf_send[1] |= 1 << 4; 

                  }         
                  if ( IN14 ) 
                  {    
                      canbuf_send[1] |= 1 << 5; 

                  }
                  if ( IN15 ) 
                  { 
                      canbuf_send[1] |= 1 << 6;

                  }
                  if ( IN16 ) 
                  {
                      canbuf_send[1] |= 1 << 7; 

                  }    
                  if ( IN17 ) 
                  {          
                      canbuf_send[2] |= 1 << 0;

                  }
                  if ( IN18 ) 
                  {
                      canbuf_send[2] |= 1 << 1;

                  }
                  if ( IN19 ) 
                  {   
                      canbuf_send[2] |= 1 << 2;

                  }         
                  if ( IN20 ) 
                  {    
                      canbuf_send[2] |= 1 << 3; 

                  }
                  if ( IN21 ) 
                  { 
                      canbuf_send[2] |= 1 << 4;

                  }
                  if ( IN22 ) 
                  {
                      canbuf_send[2] |= 1 << 5;

                  } 
                  if ( IN23 ) 
                  {
                      canbuf_send[2] |= 1 << 6;

                  }
                  if ( IN24 ) 
                  {   
                      canbuf_send[2] |= 1 << 7;

                  }         
                  if ( IN25 ) 
                  {    
                      canbuf_send[3] |= 1 << 0;

                  }
                  if ( IN26 ) 
                  { 
                      canbuf_send[3] |= 1 << 1;

                  }
                  if ( IN27 ) 
                  {
                      canbuf_send[3] |= 1 << 2; 

                  }  
                  if ( IN28 ) 
                  {
                      canbuf_send[3] |= 1 << 3; 

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
u32 ms_count = 10;

void SPI1_DMA_Check(void)
{    

  u16 i = 0;
  u8 result = 0;
  comm_num = buffersize;
  
#ifdef GEC_SF_MASTER      
    

        
    number++;

    for( i = 0; i < comm_num; i++)
    {
        SPI1_TX_Buff[i] = number;
    }
    
    i = MB_CRC16( SPI1_TX_Buff, comm_num - 2 );
    SPI1_TX_Buff[comm_num - 2] = i;
    SPI1_TX_Buff[comm_num - 1] = i>>8;
#endif    
    
    SPI1_DMA_ReceiveSendByte(comm_num);
  
    while( SPI_DMA_RECEIVE_FLAG != 1 );

    if ( SPI_DMA_RECEIVE_FLAG == 1 )
    {
        
        while(ms_count--)
        {
            
              SPI_DMA_RECEIVE_FLAG = 0;

#ifdef GEC_SF_MASTER              
              
              delay_ms(10);
              
              number++;

              for( i = 0; i < comm_num - 2; i++ )
              {
                    SPI1_TX_Buff[i] = number;
              }
              
              i = MB_CRC16( SPI1_TX_Buff, comm_num - 2 );
              SPI1_TX_Buff[comm_num - 2] = i;
              SPI1_TX_Buff[comm_num - 1] = i>>8;
              
              SPI1_DMA_ReceiveSendByte(comm_num);
                            
              while( SPI_DMA_RECEIVE_FLAG != 1 );

              
              if(!MB_CRC16(SPI1_RX_Buff, comm_num))
              {

                  for( i=0; i < comm_num - 2; i++ )
                  {
                      result = SPI1_RX_Buff[i]^(SPI1_TX_Buff[i] - 1);
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
              }
              
              
              
              
#else
              if(!MB_CRC16(SPI1_RX_Buff, comm_num))
              {                 
                  for( i=0; i < comm_num - 2; i++ )
                  {
                      SPI1_TX_Buff[i] = SPI1_RX_Buff[i];
                  }   
                  
                  i = MB_CRC16( SPI1_TX_Buff, comm_num - 2 );
                  SPI1_TX_Buff[comm_num - 2] = i;
                  SPI1_TX_Buff[comm_num - 1] = i>>8;
              } 
              else
              {
                  data_error = 1;                 
              }
              
              SPI1_DMA_ReceiveSendByte(comm_num);
              
              while( SPI_DMA_RECEIVE_FLAG != 1 );
#endif              
        }
        
        if( data_error == 1 )
        {
            LED = 0;
            while(1);            
        }
        
    }


}

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
  
    u8 canbuf_recv[8];
    u8 res;
    u8 can_rcv;
       
    /** CAN1 send data **/
    res=Can_Send_Msg(CAN1,canbuf_send,4);                          
    if(res)
    {        
#if DEBUG_PRINTF 
//        printf("CAN1TX:fail\r\n");
#endif
    }
    else 
    {	 
#if DEBUG_PRINTF                         
        USB_VCP_SendBuf(canbuf_send, 4); 
#endif
        delay_ms(1);
        
        /** CAN1 receive data **/
        can_rcv=Can_Receive_Msg(CAN1,canbuf_recv);
        if(can_rcv)
        {		
#if DEBUG_PRINTF 
            USB_VCP_SendBuf(canbuf_recv, can_rcv);
#endif
        }                                                                       
        
    } 
    
}


/*******************************************************************************
* Function Name  : can_test
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
void can_test(void)
 {	 
	u8 i=0,t=0;
	u8 canbuf[8];
	u8 res;
        u8 can_rcv;
	u8 mode=CAN_Mode_Normal;

	 	
   
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);  

	
 	while(1)
	{
         
                for(i=0;i<8;i++)
                {
                  canbuf[i]=i;
                }
                
                res=Can_Send_Msg(CAN1,canbuf,8);
                if(res)
                {                             
                   /** send fail **/                            
                }							   
                               
                can_rcv=Can_Receive_Msg(CAN1,canbuf);
		if(can_rcv)
		{			
			
 			for(i=0;i<can_rcv;i++)
			{									    
				
 			}
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED=!LED;	
			t=0;
		}		   
	}
}


/*******************************************************************************
* Function Name  : can1_can2_test
* Description    : 
*                  
* Input          : None
*                  None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef GEC_SF_MASTER
void can1_can2_test(void)
{	
  
	u8 i=0;
	u8 canbuf_send[8],canbuf_recv[8];
	u8 res;
        u8 can_rcv;
	 	
   
        /** CAN1 init,baud rate 250Kbps **/
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);  
        
        /** CAN2 init,baud rate 250Kbps **/
        /** note : use CAN2 , must CAN1 init **/
        CAN_Mode_Init(CAN2,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);    

	
 	while(1)
	{
          
                /** CAN1 send **/
                
                for(i=0;i<8;i++)
                {
                  canbuf_send[i]=i;                
                }
                res=Can_Send_Msg(CAN1,canbuf_send,8);
                
                #if DEBUG_PRINTF
                if(res)                   
                    printf("Failed\r\n");		
                else 
                    printf("OK\r\n");	 										   
                #endif

                /** CAN2 receive **/
		can_rcv=Can_Receive_Msg(CAN2,canbuf_recv);
		if(can_rcv)
		{			
			
 			for(i=0;i<can_rcv;i++)
			{	
                            #if DEBUG_PRINTF 
                              USB_VCP_SendBuf(canbuf_recv, can_rcv);	
                            #endif
 			}
		}                               
		   
	}

}
#endif



/******************************  END OF FILE  *********************************/




