#include "initial_devices.h"

#ifdef GEC_SF_MASTER

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
uint8_t Rxbuffer[64]; 
__IO uint32_t receive_count =1;


#else

#endif

void Bsp_Init(void)
{
  
#ifdef GEC_SF_MASTER
        /** stm32 self test **/
        self_test();
#endif
        /** set system interrupt priority group 2 **/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        
        /** delay init **/
	delay_init();  
        
        /** LED init **/
	LED_Init();
        
        /** input and relay output test init **/
        Input_Output_PinInit();
        
        /** spi communication init **/
        SPI1_Init();
	SPI1_DMA_Configuration();
        
        /** ewdt init **/
        EWDT_Drv_pin_config();
//        power_on_bsp_check();
        
        /** exti init **/
        EXTIX_Init();

#ifdef GEC_SF_MASTER
                
        /** TIM init 1000Khz£¬counting to 10 is 10us **/
        TIM3_Int_Init(9,71);
        
        /** usart3 init **/
        USART3_Init();
        
        /** timer for usart3 **/
        /** TIM init 10khz, counting to 10 is 1ms **/
        TIM2_Int_Init(9,7199);
        
        /** MB85RCXX init **/
        eep_init();
        if(MB85RCXX_Check())
        {
              /** MB85RCXX check fail , do some things **/                   
        }
        else
        {
            esc_data_check();
        }
       	 	
        /** CAN1 init,baud rate 250Kbps **/
	CAN_Mode_Init(CAN1,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);  
        
        /** CAN2 init,baud rate 250Kbps **/
        /** note : use CAN2 , must CAN1 init **/
//        CAN_Mode_Init(CAN2,CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,mode);           

        
        /** USB VCP init **/
          USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);          
          
        /** wait slave spi **/
        delay_ms(200);

#else
        /** TIM init 1000Khz£¬counting to 5000 is 5ms **/
        TIM2_Int_Init(4999,71);
      
#endif

}