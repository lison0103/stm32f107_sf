#include "delay.h"
#include "hw_test.h"
#include "led.h"
#include "can.h"
#include "spi.h"
#include "ewdt.h"

#ifdef GEC_SF_MASTER
#include "usbd_cdc_vcp.h"
#include "usart.h"
#include "crc16.h"

u8 R_SF_RL2_FB_CPU1;

#else

u8 R_SF_RL1_FB_CPU2;

#endif

u8 sflag,inputnum = 0;
u8 passflag = 1;
u8 sfwdt_checkflag = 0;

u8 canbuf_send[8];


void SF_WDT_Check(void)
{
    EWDT_TOOGLE();
    delay_ms(1000);
    EWDT_TOOGLE();
    delay_ms(800);
    EWDT_TOOGLE(); 
    
#ifdef GEC_SF_MASTER    
    if( !SF_RL1_FB )
    {
          passflag = 0;
          sfwdt_checkflag = 0;
    }
    else
    {
        sfwdt_checkflag = 1;
        SF_RL1_WDT=!SF_RL1_WDT;
    }
#else
    if( !SF_RL2_FB )
    {
          passflag = 0;
          sfwdt_checkflag = 0;
    }       
    else
    {
        sfwdt_checkflag = 1;
        SF_RL2_WDT=!SF_RL2_WDT;
    }
#endif
    
    

}

void Hw_Test1(void)
{  

#ifdef GEC_SF_MASTER
  

  
        for(u8 i=0;i<4;i++)
        {
          canbuf_send[i]= 0x0;                 
        }
        
        /**************************** test input *************
        IN1 = IN2 = IN3 = IN4 = IN5 = IN6 =  IN7 = IN8 =  0;  
        IN9 = IN10 = IN11 = IN12 = IN13 = IN14 = IN15 = IN16 = IN17 = IN18 = IN19 = IN20 = IN21 = IN22 = IN23 = IN24 =  IN25 = IN26 = IN27 = IN28 = 1;
        *****************************************************/
        
        /****test input,The actual test should be uncommented****/
        if(passflag  /*&& ( !IN1 && !IN2 && !IN3 && !IN4 && !IN5 && !IN6 && !IN7 && !IN8 && 
                        IN9 && IN10 && IN11 && IN12 && IN13 && IN14 && IN15 && IN16 && 
                          IN17 && IN18 && IN19 && IN20 && IN21 && IN22 && IN23 && IN24 && IN25 && IN26 && IN27 && IN28 )*/)
        {
                if(SF_RL1_DRV_FB && !SF_PWR_FB_CPU1 && SF_RL1_FB && AUX1_FB)
                {
                    
//                    if(SF_RL2_FB_CPU1 == R_SF_RL2_FB_CPU1)
//                    {
                        AUX1_CTR = 1;
                        SF_RL1_CTR = 1;  
                        
                        delay_ms(10);
                        
//                        passflag = 2;
//                    }
                        SF_WDT_Check();
                }                
                else if(( passflag == 2 ) && ( SF_RL1_DRV_FB || SF_PWR_FB_CPU1 || SF_RL1_FB || AUX1_FB ))
                {
                    passflag = 0;                  
                }                

                
                for(u8 i=0;i<3;i++)
                {
                  canbuf_send[i]= 0xff;                 
                }
                canbuf_send[3] = 0x0f;
          
        }
        else
        {
                passflag = 0;
                AUX1_CTR = 0; 
                SF_RL1_CTR = 0;            

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
        
#else
        
        if(passflag /*&& ( !IN1 && !IN2 && !IN3 && !IN4 && !IN5 && !IN6 && !IN7 && !IN8 && 
                        IN9 && IN10 && IN11 && IN12 && IN13 && IN14 && IN15 && IN16 && 
                          IN17 && IN18 && IN19 && IN20 && IN21 && IN22 && IN23 && IN24 && IN25 && IN26 && IN27 && IN28 )*/)
        {
          
            if(SF_RL2_DRV_FB && !SF_PWR_FB_CPU2 && SF_RL2_FB && AUX2_FB)
            {
                
//                if(SF_RL1_FB_CPU2 == R_SF_RL1_FB_CPU2)
//                {
                    AUX2_CTR = 1;
                    SF_RL2_CTR = 1;  
                    
                    delay_ms(10);
                    
//                    passflag = 2;
//                }
                    
                    SF_WDT_Check();
            }            
            else if(( passflag == 2 ) && (SF_RL2_DRV_FB || SF_PWR_FB_CPU2 || SF_RL2_FB || AUX2_FB))
            {
                passflag = 0;                  
            }   
            
        }
        else
        {

            passflag = 0;
            AUX2_CTR = 0; 
            SF_RL2_CTR = 0;  
                       
        }        
#endif
        
}        
        


void Hw_Test2(void)
{
  
#ifdef GEC_SF_MASTER
  
//    u8 sflag,t,inputnum = 0;
//  
//    AUX1_CTR = 0;
//    SF_RL1_CTR = 0;
//    SF_RL1_WDT = 1;
    
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
            AUX1_CTR = 0; 
            SF_RL1_CTR = 0;
        }
        else if ( sflag > 1 )
        {
            AUX1_CTR = 0; 
            SF_RL1_CTR = 0;
        }
        else if ( inputnum && ( inputnum % 2 ) )
        {
            AUX1_CTR = 1; 
        }
        else if ( inputnum )
        {
            SF_RL1_CTR = 1; 
        }
               
        
//        delay_ms(1);
//        t++;
//        if(t==200)
//        {
//            LED=!LED;
//            SF_RL1_WDT=!SF_RL1_WDT;
//            t=0;
//        }        
//        
//    }
        
#else

  
    AUX2_CTR = 0;
    SF_RL2_CTR = 0;
    SF_RL2_WDT = 1;
    
    while(1)
    {
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
        

        if ( inputnum == 0 )
        {
            AUX2_CTR = 0; 
            SF_RL2_CTR = 0;
        }
        else if ( sflag > 1 )
        {
            AUX2_CTR = 0; 
            SF_RL2_CTR = 0;
        }
        else if ( inputnum && ( inputnum % 2 ) )
        {
            AUX2_CTR = 1; 
        }
        else if ( inputnum )
        {
            SF_RL2_CTR = 1; 
        }
        

        
        
//        delay_ms(1);
//        t++;
//        if(t==200)
//        {
//            LED=!LED;
//            SF_RL2_WDT=!SF_RL2_WDT;
//            t=0;
//        }        
        
    }


#endif        
        
}
/******************************************************************************* 
*******************************************************************************/

/******************************************************************************/
u8 number = 0;
u8 onetime = 0;

u8 data_error = 0;
u32 ms_count = 0;

void spi1_test(void)
{    

  u16 i = 0;
  u16 num = 512;
  
#ifdef GEC_SF_MASTER      
    
    ms_count++;
    delay_ms(1);
    
    if( ms_count == 2000 && onetime == 0)
    {
        ms_count = 0;
        onetime++;
        number++;
        if(number > 10)
          number = 0;
        for( i = 0;i < 512;i++)
        {
              SPI1_TX_Buff[i] = number;
        }
        
        num = i;

        SPI1_DMA_ReceiveSendByte(num);
    }

    if ( SPI_DMA_RECEIVE_FLAG == 1 )
    {
        SPI_DMA_RECEIVE_FLAG = 2;
//        INTX_DISABLE();
        for(i = 0; i < 512; i++)
        {
            if(SPI1_TX_Buff[i] != SPI1_RX_Buff[i])
            {
                  data_error++;
                  USB_VCP_SendBuf(SPI1_RX_Buff, 512);  
                  break;
            }
        }  
//        INTX_ENABLE();
        if( data_error <= 0)
        {
            if(i == num) 
              AUX1_CTR = 1;
            else        
              AUX1_CTR = 0;    
        }
        else
        {
            AUX1_CTR = 0;  
        }                 
        
    }
    
    if (ms_count >= 2000 && SPI_DMA_RECEIVE_FLAG == 2 )
    {
        ms_count = 0;
        number++;
        if(number > 10)
          number = 0;
        for( i = 0;i < 512;i++)
        {
              SPI1_TX_Buff[i] = number;
        }
        
        num = i;

        SPI1_DMA_ReceiveSendByte(num);     
    
    }
    EWDT_TOOGLE();

#else
    
    delay_ms(1);
    if( onetime == 0)
    {
        onetime++;
        number++;
        if(number > 10)
          number = 0;
        for( i = 0;i < 512;i++)
        {
              SPI1_TX_Buff[i] = number;
        }

        SPI1_DMA_ReceiveSendByte(512);

    }
  
    num = i;

    if( SPI_DMA_RECEIVE_FLAG == 1 )
    {
        SPI_DMA_RECEIVE_FLAG = 0;
        
        INTX_DISABLE();
        for(i = 0; i < 512; i++)
        {
            if(SPI1_TX_Buff[i] != SPI1_RX_Buff[i])
            {
                data_error++;
                break;
            }
        }
        INTX_ENABLE();
        if( data_error <= 0)
        {
            if(i == num) 
              AUX2_CTR = 1;
            else        
              AUX2_CTR = 0;    
        }
        else
        {
            AUX2_CTR = 0;  
        }   
          number++;
          if(number > 10)
              number = 0;          
          for(int i = 0;i < 512;i++)
          {
              SPI1_TX_Buff[i] = number;
          }         

          SPI1_DMA_ReceiveSendByte(num);
        
    }
    EWDT_TOOGLE();
    

    
    
#endif

}

/******************************************************************************* 
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
                
                if(res)
                  USB_VCP_SendBuf("Failed", 6);		
                else 
                  USB_VCP_SendBuf("OK", 2);	 										   


                /** CAN2 receive **/
		can_rcv=Can_Receive_Msg(CAN2,canbuf_recv);
		if(can_rcv)
		{			
			
 			for(i=0;i<can_rcv;i++)
			{									    
                              USB_VCP_SendBuf(canbuf_recv, can_rcv);	
 			}
		}                               
		   
	}

}
#endif
/******************************************************************************* 
*******************************************************************************/
