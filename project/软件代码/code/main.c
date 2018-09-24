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
/* user files 
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
*/
#include "hal_usart1.h"
#include "timer.h"
#include "task.h"
/*
uint32_t rtc_second = 0, is_rtc_update = 0;
uint32_t ultrasonic_dist = 0, is_ultrasonic_update = 0;
uint8_t adc_nofify_rank = 0;

uint32_t TimeCount=0;//系统时基

//volatile unsigned char FlagDefense = 1;*/
unsigned char isAlert = 0;//报警标志
extern unsigned char FlagDefense;


int main(void)
{
    task_init();

    while(1)
    {
        task_driver();
    }
	return 0;
}

