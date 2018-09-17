#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "usart.h"
static void USART1_RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

static void USART1_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void USART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;	 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
}

void USART1_Configuration(void)
{	
	USART_InitTypeDef USART_InitStructure;
	USART1_RCC_Configuration ();
	USART1_GPIO_Configuration ();
#if DEBUG_SHELL
	USART_InitStructure.USART_BaudRate = 460800;
#else
	USART_InitStructure.USART_BaudRate = 115200;
#endif
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
}


void USART1_init(void)
{
	//USART1 NVIC INIT
	USART1_NVIC_Configuration();

	//USART1 INIT
	USART1_Configuration();
}



int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
	return ch;
}
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}
	return (int)USART_ReceiveData(USART1);
}



#define USART1_FIFO_BUF_SIZE		128 //it must be pow(2, x) result 
#define USART1_FIFO_BUF_SIZE_MASK		(USART1_FIFO_BUF_SIZE-1)
unsigned char usart1_rx_fifo_buf[USART1_FIFO_BUF_SIZE];
unsigned int usart1_rx_fifo_buf_in = 0;
unsigned int usart1_rx_fifo_buf_out = 0;
unsigned int usart1_rx_fifo_len(void)
{
	return (usart1_rx_fifo_buf_in - usart1_rx_fifo_buf_out);
}

unsigned char usart1_rx_fifo_is_empty(void)
{
	return (usart1_rx_fifo_buf_in == usart1_rx_fifo_buf_out);
}

unsigned char usart1_rx_fifo_is_full(void)
{
	return (usart1_rx_fifo_len() > USART1_FIFO_BUF_SIZE_MASK);
}

void usart1_rx_fifo_clean(void)
{
	if(!usart1_rx_fifo_is_empty())
		usart1_rx_fifo_buf_out = usart1_rx_fifo_buf_in;
}




/*
* It returns 0 if the rx_fifo was full. Otherwise it returns the number
* processed elements.
*/
int usart1_rx_fifo_put(unsigned char data)
{
	if(usart1_rx_fifo_is_full()){
		return -1;
	}else{
		//printf("index:%d, data:%d\n", usart1_rx_fifo_buf_in & USART1_FIFO_BUF_SIZE_MASK, data);
		usart1_rx_fifo_buf[usart1_rx_fifo_buf_in & USART1_FIFO_BUF_SIZE_MASK] = data;
		usart1_rx_fifo_buf_in++;

		return usart1_rx_fifo_buf_in;
	}
}

/*
* It returns 0 if the rx_fifo was empty. Otherwise it returns the number
* processed elements.
*/
int usart1_rx_fifo_get(unsigned char* data)
{
	if(usart1_rx_fifo_is_empty()){
		return -1;
	}else{
		*data = usart1_rx_fifo_buf[usart1_rx_fifo_buf_out & USART1_FIFO_BUF_SIZE_MASK];
		usart1_rx_fifo_buf_out++;

		return usart1_rx_fifo_buf_out;
	}
}


/*
* 串口1发送字符串
*/
void usart1_send_str(char* data)
{
	if(data == NULL){
		//printf("data为空");
		return;
	}
	
	while(*data){
		/* Wait until end of transmit */
	    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

	    /* Write one byte in the USART1 Transmit Data Register */
	    USART_SendData(USART1, *data);
	    data++;
	}
}











/*
*	串口一从缓存取得字符串
*/
int usart1_get_str(unsigned char* data)
{
	while(usart1_rx_fifo_buf_out != usart1_rx_fifo_buf_in){
		
		*data = usart1_rx_fifo_buf[usart1_rx_fifo_buf_out & USART1_FIFO_BUF_SIZE_MASK];
		usart1_rx_fifo_buf_out++;

		data++;
	}
	return usart1_rx_fifo_buf_out;
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		usart1_rx_fifo_put(USART_ReceiveData(USART1));
	}
}

