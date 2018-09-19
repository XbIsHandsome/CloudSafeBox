#include <stdio.h>
#include "my_ultrasonic.h"

ULTRASONIC_CALLBACK_FUNCTION ult = NULL;

/* Private function prototypes -----------------------------------------------*/
void Time4_Init(void);
void TIM4_RCC_Configuration(void);
void TIM4_GPIO_Configuration(void);
void TIM4_NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void ultrasonic_init(ULTRASONIC_CALLBACK_FUNCTION ult_update_notify_cb)
{
	/* System Clocks Configuration */
  TIM4_RCC_Configuration();

  /* NVIC configuration */
  TIM4_NVIC_Configuration();

  /* Configure the GPIO ports */
  TIM4_GPIO_Configuration();
	
	Time4_Init();

  if(ult_update_notify_cb)
  	ult = ult_update_notify_cb;

}

//定时器和超声波数据接收端口使能
void TIM4_RCC_Configuration(void)
{
  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

//超声波出发和数据接收端口分配
void TIM4_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM4 channel 3 pin (PB.08) configuration: ultrasonic input capture */
  GPIO_InitStructure.GPIO_Pin =  UL1ECHO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(UL1ECHO_PORT, &GPIO_InitStructure);

  /*(PB.09) configuration: ultrasonic enable */
  GPIO_InitStructure.GPIO_Pin =  UL1TRIG_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(UL1TRIG_PORT, &GPIO_InitStructure);

}

//中断优先级配置
void TIM4_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM4 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//定时器初始化
void Time4_Init(void)
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 99;//初值
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	TIM_Cmd(TIM4, ENABLE);
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIME4_IRQHandler(void)
{ 
  
	  
}

