#include "led.h"

/* u32 LED_Cnt = 0; */

void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    LED0 = 0;
}

void LED_Change_Station(void)
{
    LED0 = ~LED0;
}
/* void LED_Driver(void) */
/* { */
/*     if (LED_Cnt >= 1000) */
/*     { */
/*         LED_Cnt = 0; */
/*         led0 = ~led0; */
/*     } */
/* } */


