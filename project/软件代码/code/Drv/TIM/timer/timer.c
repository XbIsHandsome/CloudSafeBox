#include <stdio.h>
#include "stm32f10x.h"
#include "systick.h"
#include "keyboard_drv.h"
#include "adc.h"
#include "ultrasonic.h"
#include "segled_16bit.h"
#include "timer.h"
#include "usart.h"
//#include "buzzer.h"

extern const int32_t mems_alert_timeout;  //300ms
extern const int32_t adc_sample_rate;  //50ms
extern uint8_t MEMS_ALERT;
extern const int32_t ledseg_refresh_tm; //2.5ms , 50Hz
extern const int32_t kbd_scan_tm;  //10ms


void TIM2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void TIM2_cfg(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//����TIM2��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	//���½�Timer����Ϊȱʡֵ
	TIM_DeInit(TIM2);

	//�����ڲ�ʱ�Ӹ�TIM2�ṩʱ��Դ
	TIM_InternalClockConfig(TIM2);

	//Ԥ��Ƶϵ��Ϊ36000-1������������ʱ��Ϊ72MHz/3600 = 20kHz
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / TIME2_INPUT_FREQ) -1;

	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	//���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	//���ü��������С��ÿ��1000�����Ͳ���һ�������¼�
	TIM_TimeBaseStructure.TIM_Period = (TIME2_PERIOD_US / TIME2_TICK_US);
	//������Ӧ�õ�TIM2��
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	//�������жϱ�־
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	//��ֹARRԤװ�ػ�����
	TIM_ARRPreloadConfig(TIM2, DISABLE);

	//����TIM2���ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

}

void TIM2_test_GPIO_cfg(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                 //ѡ������5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //���Ƶ�����50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void TIM2_init(void)
{
	#if 0
	TIM2_test_GPIO_cfg();
	#endif

	//interrupt init
	TIM2_NVIC_Configuration();

	//tim2 timer init
	TIM2_cfg();
	
	//������ʱ��2
	TIM_Cmd(TIM2,ENABLE);
}




//enter ISR per TIME2_PERIOD_US
void TIM2_IRQHandler(void)
{
	static int32_t count = 0;
	unsigned char usart_data_buf[128];
//	static uint8_t data = 0;
//	static uint16_t buzzerPrescaler = 0;
	
   //����Ƿ�����������¼�
   if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
   {
	   /*
      count++;
	  if(count >= adc_sample_rate * mems_alert_timeout * kbd_scan_tm * ledseg_refresh_tm)
	  	count = 0;
	  
      //���TIM2���жϴ�����λ
      TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);

	  //do keboard scaning
	  if(count % kbd_scan_tm == 0)
	  	kbd_scan();

	  if(count % adc_sample_rate == 0){
		//do ADC per 30ms 
		start_ADC1();
	  }
	  if(count % mems_alert_timeout == 0){
		if(MEMS_ALERT < MEMS_ALERT_TRIGER)
			MEMS_ALERT = 0;
	  }*/
	  
	  if(usart1_rx_fifo_len() > 43){
			usart1_get_str(usart_data_buf);
			printf("���յ�������:%s\r\n",usart_data_buf);
			usart_data_analysis_process((char *)usart_data_buf);
			usart1_rx_fifo_clean();
		}

	  //if(count % ledseg_refresh_tm == 0){
	  //	refresh_segleds();
	  // }
   }
}

