#include "task.h"
#include "light.h"
//#include "usart.h"
#include "hal_usart1.h"
#include "timer.h"
#include "systick.h"
#include "adc_dma.h"
#include "hal_uart4.h"

void NVIC_Priority_Group_Configuration(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


int task_init(void)
{
    NVIC_Priority_Group_Configuration();
	
	//PA09 PA10 UART1 protocol
	USART1_init();
	
	//PC10 PC11 UART4
	UART4_Init(115200);
	
	//SYSTICK
	SYSTICK_init();

	//CH12864C
	lcd_init();

	//add timer2(10ms)
	TIM2_Init();

	//keyboard init
	//kbd_init();
	
	//ADC1 INIT
	//ADC1_init(adc_update_notify);

	//LF init
	//LF125K_init();

	//i2c eeprom
	//sEE_Init();

	//rtc init
	//rtc_init(rtc_update_notify);
	
	//buzzer
	buzzerInit();
	
	//light
	light_init();
	
	//servo
	TIM3_pwm_init();
	TIM3_CH1_set_servo_degree(90);
	TIM3_CH2_set_servo_degree(90);

    usart1_send_str("hello world!\r\n");

    return 0;
}

int task_driver(void)
{
    usart1_driver();

    return 0;
}
