
#include "..\..\..\..\Libraries\stm32f10x.h"
#include "door.h"

#define DOOR_OUT_PORT GPIOE
#define DOOR_OUT_PIN GPIO_Pin_3

/*#define DOOR_IN_PORT GPIOB
#define DOOR_IN_PIN GPIO_Pin_1*/

void doorInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Pin = DOOR_OUT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DOOR_OUT_PORT, &GPIO_InitStructure);

	/*GPIO_InitStructure.GPIO_Pin = DOOR_IN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(DOOR_IN_PORT, &GPIO_InitStructure);*/

}

unsigned char FlagDefense=1;//������־��1-������0-����
unsigned char FlagLockSta=0;//����״̬��1-������0-����
unsigned long int LockTimeCount=0;//�����ʱ���ʱ

unsigned char isDoorOpen(void)
{
	return FlagLockSta;/*GPIO_ReadInputDataBit(DOOR_IN_PORT, DOOR_IN_PIN)*/;
}

void doorOpen(void)
{
	/*if(isDoorOpen())
		return;*/
	FlagLockSta=1;
	GPIO_WriteBit(DOOR_OUT_PORT, DOOR_OUT_PIN, 1);
	delay_ms(50);
	GPIO_WriteBit(DOOR_OUT_PORT, DOOR_OUT_PIN, 0);
}
