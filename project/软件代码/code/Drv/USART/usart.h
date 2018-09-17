#ifndef USART_H
#define USART_H

void USART1_init(void);
unsigned int usart1_rx_fifo_len(void);
unsigned char usart1_rx_fifo_is_empty(void);
unsigned char usart1_rx_fifo_is_full(void);
void usart1_rx_fifo_clean(void);
void usart1_send_str(char* data);
int usart1_get_str(unsigned char* data);
int usart1_rx_fifo_put(unsigned char data);
int usart1_rx_fifo_get(unsigned char* data);
#endif


