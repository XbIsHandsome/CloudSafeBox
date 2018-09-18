/* ==================================================================
#     FileName: main.c
#      History:
================================================================== */
/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdint.h"
/* Library includes. */
#include "stm32f10x.h"
#include "stm32f10x_it.h"
/* user files */
#include "My_InitTask.h"
#include "systick.h"
#include "LCD12864.h"
#include "keyboard_drv.h"
#include "pwm_output.h"
#include "adc.h"
#include "lf125k.h"
#include "eeprom.h"
#include "rtc.h"
#include "ultrasonic.h"
#include "segled_16bit.h"
#include "buzzer.h"
#include "ir_alert.h"
#include "door.h"
#include "hal_uart4.h"
#include "bsp_flash.h"
#include "app_pwd.h"
#include "WiFiToCloud.h"
#include "numcal.h"
#include "usart.h"
#include "timer.h"

uint32_t rtc_second = 0, is_rtc_update = 0;
uint32_t ultrasonic_dist = 0, is_ultrasonic_update = 0;
uint8_t adc_nofify_rank = 0;

uint32_t TimeCount=0;//系统时基

//volatile unsigned char FlagDefense = 1;
volatile unsigned char isAlert = 0;//报警标志

int main( void )				 
{
	//unsigned char usart_data_buf[128];
	
	USART1_init();
	TIM2_init();
	//printf("串口1初始化...\r\n");
	while(1)
	{
		;
		/*// 接收串口命令
		if(usart1_rx_fifo_len() > 31){
			usart1_get_str(usart_data_buf);
			printf("接收到的数据:%s\r\n",usart_data_buf);
			usart_data_analysis_process((char *)usart_data_buf);
			usart1_rx_fifo_clean();
		}*/
	}
}

