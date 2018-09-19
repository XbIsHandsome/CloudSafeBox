#include "timer.h"
#include "light.h"
#include "hal_usart1.h"

void TIM2_NVIC_Configuration(void);
void TIM2_Configuration(void);

void TIM2_Init(void)
{
    // interrupt init
    TIM2_NVIC_Configuration();

    // tim2 timer init
    TIM2_Configuration();

    // ������ʱ��2
    TIM_Cmd(TIM2, ENABLE);

}

void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//����TIM2��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	//���½�Timer����Ϊȱʡֵ
	TIM_DeInit(TIM2);

	//�����ڲ�ʱ�Ӹ�TIM2�ṩʱ��Դ
	TIM_InternalClockConfig(TIM2);

	//Ԥ��Ƶϵ��Ϊ36000-1������������ʱ��Ϊ72MHz/3600 = 20kHz
	TIM_TimeBaseStructure.TIM_Prescaler = TIME2_PRESCALER - 1;

	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	//���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	//���ü��������С��ÿ��1000�����Ͳ���һ�������¼�
	TIM_TimeBaseStructure.TIM_Period = TIME2_PERIOD;
	//������Ӧ�õ�TIM2��
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	//�������жϱ�־
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	//��ֹARRԤװ�ػ�����
	TIM_ARRPreloadConfig(TIM2, DISABLE);

	//����TIM2���ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}

// enter ISR per TIME2_PERIOD_US
void TIM2_IRQHandler(void)
{
    static u16 cnt = 0;

    // ����Ƿ�����������¼�
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        usart1_rx_fifo_monitor(1);
		

		/*
        if (cnt++ >= 1000)
        {
            
            // ��תLED�Ƶ�״̬
            lightOpen();
			if (cnt++ >= 2000)
			{
				// ��תLED�Ƶ�״̬
				lightClose();
				cnt = 0;
			}
        }
		*/

        //���TIM2���жϴ�����λ
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
    }
}
