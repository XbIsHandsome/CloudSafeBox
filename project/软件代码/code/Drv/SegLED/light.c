#include "light.h"

void light_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

	GPIO_InitStructure.GPIO_Pin = LIGHT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LIGHT_PORT, &GPIO_InitStructure);

}

void lightOpen(void)
{
	GPIO_WriteBit(LIGHT_PORT, LIGHT_PIN, 1);
}
void lightClose(void)
{
	GPIO_WriteBit(LIGHT_PORT, LIGHT_PIN, 0);
}
