#ifndef __HAL_USART_H
#define __HAL_USART_H

#include "stm32f10x.h"

void USART1_init(void);
unsigned int usart1_rx_fifo_len(void);
unsigned char usart1_rx_fifo_is_empty(void);
unsigned char usart1_rx_fifo_is_full(void);
void usart1_rx_fifo_clean(void);
void usart1_send_str(unsigned char* data);
void usart1_send_num(u16 data);
int usart1_get_str(unsigned char* data);
int usart1_rx_fifo_put(unsigned char data);
int usart1_rx_fifo_get(unsigned char* data);
void usart1_action(u8 *buf, u8 len);

#endif
