#include "sys.h"
#include "delay.h"
#include "hw_test.h"
#include "led.h"
#include "can.h"
#include "spi.h"
#include "usbd_cdc_vcp.h"


u8 sflag,inputnum = 0;
u8 passflag = 1;

extern u8 canbuf_send[8];


/******************************************************************************* 
*******************************************************************************/
u8 Master_Temp =0;

void spi1_test(void)
{  
  
    u8 t;
    
    SPI1_Init();
//    SPI1_NVIC();
//    SPI1_SetSpeed(SPI_BaudRatePrescaler_256);

   while(1)
   { 
     
#if 0
       SPI1_ReadWriteByte(0x55); 
       Master_Temp = SPI1_ReadWriteByte(0x00);
#else
       SPI1_WriteByte(0x66); 
//       delay_ms(1);
//       Master_Temp = SPI1_ReadByte(0x00);
#endif
       
       delay_ms(10); 
       
       t++;
       if(t == 50)
       {
             t = 0;
             LED =! LED;
             
             Usb_Vcp_SendBuf(&Master_Temp, 1);                  
         
       }
   }

}

/******************************************************************************* 
*******************************************************************************/
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
                  Usb_Vcp_SendBuf("Failed", 6);		
                else 
                  Usb_Vcp_SendBuf("OK", 2);	 										   


                /** CAN2 receive **/
		can_rcv=Can_Receive_Msg(CAN2,canbuf_recv);
		if(can_rcv)
		{			
			
 			for(i=0;i<can_rcv;i++)
			{									    
                              Usb_Vcp_SendBuf(canbuf_recv, can_rcv);	
 			}
		}                               
		   
	}

}

/******************************************************************************* 
*******************************************************************************/
void Hw_Test_Init(void)
{
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );
      
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
      GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
  
      GPIO_InitTypeDef GPIO_InitStruct;

      /** input gpio **/
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15;             
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;       
      GPIO_Init(GPIOA , &GPIO_InitStruct);    
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_15;                  
      GPIO_Init(GPIOB , &GPIO_InitStruct);        

      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;                  
      GPIO_Init(GPIOC , &GPIO_InitStruct);   

      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;                  
      GPIO_Init(GPIOD , &GPIO_InitStruct);       
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;                  
      GPIO_Init(GPIOE , &GPIO_InitStruct);   
      
      
      /** output gpio **/
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;

      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; 
      GPIO_Init(GPIOB , &GPIO_InitStruct);       
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0; 
      GPIO_Init(GPIOC , &GPIO_InitStruct);        
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; 
      GPIO_Init(GPIOE , &GPIO_InitStruct);  
      
      
      /** relay output init **/
      AUX1_CTR = 0;
      SF_RL1_CTR = 0;
      SF_RL1_WDT = 1;

}


void Hw_Test1(void)
{  
        
        for(u8 i=0;i<4;i++)
        {
          canbuf_send[i]= 0x0;                 
        }
        
        /**************************** test input *************
        IN1 = IN2 = IN3 = IN4 = IN5 = IN6 =  IN7 = IN8 = IN9 = IN10 = IN11 = IN12 = IN13 = IN14 = IN15 = IN16 = IN17 = IN18 = 1;  
        IN19 = IN20 = IN21 = IN22 = IN23 = IN24 =  IN25 = IN26 = IN27 = IN28 = 1;
        *****************************************************/
        
        /****test input,The actual test should be uncommented****/
        if(passflag && ( /*IN1 && IN2 && IN3 && IN4 && IN5 && IN6 && IN7 && IN8 && 
                        IN9 && IN10 && IN11 && IN12 && IN13 && IN14 && IN15 && IN16 && */
                          IN17 && IN18 /*&& IN19 && IN20 && IN21 && IN22 && IN23 && IN24 && IN25 && IN26 && IN27 && IN28 */))
        {
          
                AUX1_CTR = 1;
                SF_RL1_CTR = 1;
                
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
                canbuf_send[0] = 0xff;
                canbuf_send[1] = 0xff;
                canbuf_send[2] = 0xfc;
                canbuf_send[3] = 0x0f;
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
        


void Hw_Test2(void)
{
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
        
}