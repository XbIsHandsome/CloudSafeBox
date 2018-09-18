#ifndef USART_H
#define USART_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "pwm_output.h"
#include "door.h"
#include "buzzer.h"
#include "LCD12864.h"
#include "light.h"

#define USART1_FIFO_BUF_SIZE			256 //it must be pow(2, x) result 
#define USART1_FIFO_BUF_SIZE_MASK		(USART1_FIFO_BUF_SIZE-1)


void USART1_init(void);
unsigned int usart1_rx_fifo_len(void);
unsigned char usart1_rx_fifo_is_empty(void);
unsigned char usart1_rx_fifo_is_full(void);
void usart1_rx_fifo_clean(void);
int usart1_rx_fifo_put(unsigned char data);
void usart1_send_str(char* data);
int usart1_get_str(unsigned char* data);
char *get_json_value(char *cJson, char *Tag);
void usart_data_analysis_process(char *RxBuf);

#endif


