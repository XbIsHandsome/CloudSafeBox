#include "hal_usart1.h"
#include "adc_dma.h"
//#include "numcal.c"

#define USART1_FIFO_BUF_SIZE   128 /* 队列最多存储128个数据缓存 */
#define USART1_FIFO_BUF_SIZE_MASK (USART1_FIFO_BUF_SIZE-1)

/* 接收完成标志位 */
u8 flag_frame = 0;
/* 缓存区的数组 */
unsigned char usart1_rx_fifo_buf[USART1_FIFO_BUF_SIZE];
/* 缓冲写入缓存的位置 */
unsigned int usart1_rx_fifo_buf_in = 0;
/* 缓冲读取到的位置 */
unsigned int usart1_rx_fifo_buf_out = 0;

void USART1_RCC_Configuration(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void USART1_GPIO_Configuration(void)
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART1_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

	USART1_RCC_Configuration ();
	USART1_GPIO_Configuration ();

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
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

/***************************** 处理数据部分 *****************************/

/* 返回剩余的未读取的长度 */
unsigned int usart1_rx_fifo_len(void)
{
    return (usart1_rx_fifo_buf_in - usart1_rx_fifo_buf_out);
}

/* 判断是否数组已经读取完成了，读完了返回1， 未读完返回0 */
unsigned char usart1_rx_fifo_is_empty(void)
{
    return (usart1_rx_fifo_buf_in == usart1_rx_fifo_buf_out);
}

/* 判断数组写入的数据是否超出数组，如果超出返回0， 如果没有超出返回1 */
unsigned char usart1_rx_fifo_is_full(void)
{
    if (usart1_rx_fifo_len() > USART1_FIFO_BUF_SIZE_MASK)
        return 1;
    else
        return 0;
}

void usart1_rx_fifo_clean(void)
{
    if (0 == usart1_rx_fifo_is_empty())
        usart1_rx_fifo_buf_out = usart1_rx_fifo_buf_in;
}

/* 这个函数是单个写入函数，每运行一次将一个字节的数据存入队列 */
int usart1_rx_fifo_put(unsigned char data)
{
	if(usart1_rx_fifo_is_full())
    {
		return -1;
	}
    else
    {
		usart1_rx_fifo_buf[usart1_rx_fifo_buf_in & USART1_FIFO_BUF_SIZE_MASK] = data;
		usart1_rx_fifo_buf_in++;

		return usart1_rx_fifo_buf_in;
	}
}

/* 这个函数每次都将数据通过给的地址发送出去 */
int usart1_rx_fifo_get(unsigned char* data)
{
	if(usart1_rx_fifo_is_empty())
    {
		return -1;
	}
    else
    {
		*data = usart1_rx_fifo_buf[usart1_rx_fifo_buf_out & USART1_FIFO_BUF_SIZE_MASK];
		usart1_rx_fifo_buf_out++;

		return usart1_rx_fifo_buf_out;
	}
}


/*
 * 串口1发送字符串
 */
void usart1_send_str(unsigned char* data)
{
	while(*data)
    {
		/* Wait until end of transmit */
	    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

	    /* Write one byte in the USART1 Transmit Data Register */
	    USART_SendData(USART1, *data);
	    data++;
	}
}

/*
 *	串口一从缓存取得字符串,这个函数一次性将数组缓存全部取完
 */
int usart1_get_str(unsigned char* data)
{
    while (usart1_rx_fifo_buf_out != usart1_rx_fifo_buf_in)
    {
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
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
		usart1_rx_fifo_put(USART_ReceiveData(USART1));
	}
}

/*********************** 添加的接收处理函数 **************************/


/* 输入为判断接受任务结束的最短时间，时间要乘以定时器进入中断的时间 */
void usart1_rx_fifo_monitor(u16 time)
{
    static u16 cnt_bkp = 0;     /* 作为记录之前一次长度的存储区 */
    static u16 idle_tmr = 0;    /* 没有接受到数据的时间 */


    if (usart1_rx_fifo_len() > 0) /* 接收到了数据 */
    {
        /* 接收到的数据长度于之前比较不相等 */
        /* 说明接受到串口数据了 */
        if (cnt_bkp != usart1_rx_fifo_len())
        {
            cnt_bkp = usart1_rx_fifo_len();
            /* 清零空闲计数 */
            idle_tmr = 0;
        }
        else
        {
            /* 说明没有接受到串口数据了 */
            if (idle_tmr < 30)
            {
                /* 空闲计数增加 */
                idle_tmr += time;
                /* 当连续30ms没有接受到数据时候，判定接收数据已经结束 */
                if (idle_tmr >= 30)
                {
                    /* 标志位 */
                    flag_frame = 1;
                }
            }
        }
    }
    else
    {
        /* 接受长度清零 */
        cnt_bkp = 0;
    }
}

/* buf 读取数据存到的数组， len为读取的长度 */
u16 usart1_read_str(u8 *buf, u16 len)
{
    if (len > usart1_rx_fifo_len())
    {
        len = usart1_rx_fifo_len();
    }

    usart1_get_str(buf);

    return len;
}

void usart1_driver(void)
{
    u16 len = 0;
    u8 buf[40];

    if (1 == flag_frame)
    {
        flag_frame = 0;
        len = usart1_read_str(buf, sizeof(buf));
        buf[len] = '\0';
        usart1_action(buf, len);
    }
}

void usart1_send_num(u16 data)
{
    u8 buf[6];

    buf[0] = data / 10000 + '0';
    buf[1] = data / 1000 % 10 + '0';
    buf[2] = data / 100 % 10 + '0';
    buf[3] = data / 10 % 10 + '0';
    buf[4] = data % 10 + '0';
    buf[5] = '\0';

    usart1_send_str(buf);
}

u8 Cmp_Memory( u8 *ptr1, u8 *ptr2, u16 len )
{
    while ( len-- )
    {
        if ( *ptr1++ != *ptr2++ )
        {
            return 0;
        }
    }

    return 1;
}

/*  处理数据的函数，可更改的部分 */
void usart1_action(u8 *buf, u8 len)
{
    int temp = 0;
    u16 adc_value = 0;

    usart1_send_str(buf);
    adc_value = get_adc_result();
    temp = conv_res_to_temp((unsigned short)adc_value);
    usart1_send_num(temp);
}
