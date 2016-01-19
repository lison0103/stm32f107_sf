#include "sys.h"
#include "delay.h"
#include "hw_test.h"
#include "led.h"


//#define INPUT(x,y) x##y

void HW_TEST_INIT(void)
{
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );
      
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
      GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
  
      GPIO_InitTypeDef GPIO_InitStruct;

      //input
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15;             
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;       
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
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8; 
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
      GPIO_Init(GPIOB , &GPIO_InitStruct);  
          
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12; 
      GPIO_Init(GPIOC , &GPIO_InitStruct);        
      
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6; 
      GPIO_Init(GPIOE , &GPIO_InitStruct);  

}

void HW_TEST(void)
{
    u8 sflag,t,inputnum = 0;
  
    GRL1 = 0;
    GRL2 = 0;
//    GRL3 = 0;
//    GRL4 = 0;
//    GRL5 = 0;
//    GRL6 = 0;
//    GRL7 = 0;
//    GRL8 = 0;
//    GRL9 = 0;
//    
//    GSFR1 = 0;
//    GSFR2 = 0;
//    GSFR3 = 0;
//    GSFR4 = 0;
//    
//    TRANS_CTRL1 = 0;
//    TRANS_CTRL2 = 0;
    

    
    while(1)
    {
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
        if ( !IN14 ) 
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
        if ( !EX_IN1 )
        {
            inputnum = 29;
            sflag++;
        }
        if ( !EX_IN2 ) 
        {       
            inputnum = 30;
            sflag++;
        }
        if ( !EX_IN3 ) 
        {                   
            inputnum = 31;
            sflag++;

        }
        if ( !EX_IN4 ) 
        {
            inputnum = 32;
            sflag++;
        } 
        if ( !EX_IN5 ) 
        {           
            inputnum = 33;
            sflag++;
        }
        if ( !EX_IN6 ) 
        {          
            inputnum = 34;
            sflag++;

        }
        if ( !EX_IN7 ) 
        {         
            inputnum = 35;
            sflag++;

        }        
        if ( !EX_IN8 ) 
        {         
            inputnum = 36;
            sflag++;

        }
        if ( !EX_IN9 ) 
        {     
            inputnum = 37;
            sflag++;

        }
        if ( !EX_IN10 ) 
        {                
            inputnum = 38;
            sflag++;

        } 
        if ( !EX_IN11 ) 
        {          
            inputnum = 39;
            sflag++;

        }
        if ( !EX_IN12 ) 
        {
            inputnum = 40;
            sflag++;

        }
        if ( !EX_IN13 ) 
        {   
            inputnum = 41;
            sflag++;

        }         
        if ( !EX_IN14 ) 
        {    
            inputnum = 42;
            sflag++;

        }
        if ( !EX_IN15 ) 
        { 
            inputnum = 43;
            sflag++;

        }
        if ( !EX_IN16 ) 
        {
            inputnum = 44;
            sflag++;

        }    
        if ( !EX_IN17 ) 
        {          
            inputnum = 45;
            sflag++;

        }       
        

        if ( inputnum == 0 )
        {
            GRL1 = 0; 
            GRL2 = 0;
        }
        else if ( sflag > 1 )
        {
            GRL1 = 0; 
            GRL2 = 0;
            LED= 0;
        }
        else if ( inputnum && ( inputnum % 2 ) )
        {
            GRL1 = 1; 
//            LED= 1;
        }
        else if ( inputnum )
        {
            GRL2 = 1; 
//            LED= 0;
        }
        

        
        
//        delay_ms(1);
//        t++;
//        if(t==200)
//        {
//            LED=!LED;
//            t=0;
//        }        
        
    }
}