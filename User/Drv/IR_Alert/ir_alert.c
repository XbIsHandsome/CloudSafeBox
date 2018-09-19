#include "..\..\..\..\Libraries\stm32f10x.h"
#include "ir_alert.h"

void irInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}
int irAlert(void)
{
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) || GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9))
		return 1;
	else
		return 0;
}

