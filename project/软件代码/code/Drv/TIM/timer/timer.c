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

    // 开启定时器2
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

	//允许TIM2的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	//重新将Timer设置为缺省值
	TIM_DeInit(TIM2);

	//采用内部时钟给TIM2提供时钟源
	TIM_InternalClockConfig(TIM2);

	//预分频系数为36000-1，这样计数器时钟为72MHz/3600 = 20kHz
	TIM_TimeBaseStructure.TIM_Prescaler = TIME2_PRESCALER - 1;

	//设置时钟分割
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	//设置计数器模式为向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	//设置计数溢出大小，每计1000个数就产生一个更新事件
	TIM_TimeBaseStructure.TIM_Period = TIME2_PERIOD;
	//将配置应用到TIM2中
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	//清除溢出中断标志
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	//禁止ARR预装载缓冲器
	TIM_ARRPreloadConfig(TIM2, DISABLE);

	//开启TIM2的中断
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}

// enter ISR per TIME2_PERIOD_US
void TIM2_IRQHandler(void)
{
    static u16 cnt = 0;

    // 检测是否发生溢出更新事件
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        usart1_rx_fifo_monitor(1);
		

		/*
        if (cnt++ >= 1000)
        {
            
            // 翻转LED灯的状态
            lightOpen();
			if (cnt++ >= 2000)
			{
				// 翻转LED灯的状态
				lightClose();
				cnt = 0;
			}
        }
		*/

        //清除TIM2的中断待处理位
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
    }
}
