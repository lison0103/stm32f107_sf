#include "sys.h"
#include "delay.h"
#include "hw_test.h"
#include "led.h"


//#define INPUT(x,y) x##y

#if 0

void HW_TEST_INIT(void)
{
//      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );
      
      RCC->APB2ENR|=0X7D;

      
//      RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
      GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
  
//      GPIO_InitTypeDef GPIO_InitStruct;

      //input
//      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15;             
//      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
//      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;       
//      GPIO_Init(GPIOA , &GPIO_InitStruct);    
      
      GPIOA->CRH&=0X0FFFFF00; 
      GPIOA->CRH|=0X80000088; 	 
//      GPIOA->ODR|=0X8003;     

      
//      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_15;                  
//      GPIO_Init(GPIOB , &GPIO_InitStruct);  

      GPIOB->CRL&=0X00000000; 
      GPIOB->CRL|=0X88888888;
      GPIOB->CRH&=0X00FFFFFF; 
      GPIOB->CRH|=0X88000000; 	 
//      GPIOB->ODR|=0XC0FF;       

//      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;                  
//      GPIO_Init(GPIOC , &GPIO_InitStruct);   
      
      GPIOC->CRL&=0X00FFFFFF; 
      GPIOC->CRL|=0X88000000;
      GPIOC->CRH&=0XFFF00000; 
      GPIOC->CRH|=0X00088888; 	 
//      GPIOC->ODR|=0X1FC0;

//      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;                  
//      GPIO_Init(GPIOD , &GPIO_InitStruct);       

      GPIOD->CRL&=0X000000FF; 
      GPIOD->CRL|=0X88888800;
      GPIOD->CRH&=0X00000000; 
      GPIOD->CRH|=0X88888888; 	 
//      GPIOD->ODR|=0XFFFC;
      
//      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;                  
//      GPIO_Init(GPIOE , &GPIO_InitStruct);   

      GPIOE->CRL&=0X0FFFFF00; 
      GPIOE->CRL|=0X80000088;
      GPIOE->CRH&=0X00000000; 
      GPIOE->CRH|=0X88888888; 	 
//      GPIOE->ODR|=0XFF03;      
      
      //output
//      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
      
//      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0; 
//      GPIO_Init(GPIOC , &GPIO_InitStruct);  

      GPIOC->CRL&=0XFFFFFFF0; 
      GPIOC->CRL|=0X00000003;
      GPIOC->ODR|=1<<0;      
      
//      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; 
//      GPIO_Init(GPIOE , &GPIO_InitStruct);  
      
      GPIOE->CRL&=0XFFFFF0FF; 
      GPIOE->CRL|=0X00000300;	 
      GPIOE->ODR|=1<<2;      
      

}

#else

void HW_TEST_INIT(void)
{
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );
      
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
      GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
  
      GPIO_InitTypeDef GPIO_InitStruct;

      //input
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
      
      
      //output
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;

      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; 
      GPIO_Init(GPIOB , &GPIO_InitStruct);       
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0; 
      GPIO_Init(GPIOC , &GPIO_InitStruct);        
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; 
      GPIO_Init(GPIOE , &GPIO_InitStruct);  

}
#endif

u8 sflag,inputnum = 0;
u8 passflag = 1;

extern u8 canbuf_send[8];

#if 1
void HW_TEST(void)
{

        sflag = 0;
        inputnum = 0;    
        
        for(u8 i=0;i<4;i++)
        {
          canbuf_send[i]= 0x0;                 
        }
        
//        /****test input,The actual test should be commented****/
//        IN1 = IN2 = IN3 = IN4 = IN5 = IN6 =  IN7 = IN8 = IN9 = IN10 = IN11 = IN12 = IN13 = IN14 = IN15 = IN16 = IN17 /*= IN18*/ = 1;  
//        IN9 = 1;
//        IN19 = IN20 = IN21 = IN22 = IN23 = IN24 =  IN25 = IN26 = IN27 = IN28 = 1;
//        /*****************************************************/
        
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

                canbuf_send[0] = 0xff;
                canbuf_send[1] = 0xff;
                canbuf_send[2] = 0xfc;
                canbuf_send[3] = 0x0f;
        
        
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
                  
#if 0
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
        

//        if ( inputnum == 0 )
//        {
//            AUX1_CTR = 0; 
//            SF_RL1_CTR = 0;
//        }
//        else if ( sflag > 1 )
//        {
//            AUX1_CTR = 0; 
//            SF_RL1_CTR = 0;
//        }
//        else if ( inputnum && ( inputnum % 2 ) )
//        {
//            AUX1_CTR = 1; 
//        }
//        else if ( inputnum )
//        {
//            SF_RL1_CTR = 1; 
//        }
        

}        
        


#else
void HW_TEST(void)
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
//        for(int i = 0;i < 28;i++)
//        {
//            if( INPUT( IN, 1 ) )
//            {
//                inputnum = i;
//            }
//        }
        
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
#endif