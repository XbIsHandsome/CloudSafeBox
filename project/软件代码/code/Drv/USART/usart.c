#include "usart.h"

unsigned char usart1_rx_fifo_buf[USART1_FIFO_BUF_SIZE-1];
unsigned int usart1_rx_fifo_buf_in = 0;
unsigned int usart1_rx_fifo_buf_out = 0;
extern unsigned char isAlert;//报警标志
extern uint8_t MEMS_ALERT;

/* ==================================================================
#     函数介绍: 串口1时钟初始化函数									#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
static void USART1_RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/* ==================================================================
#     函数介绍: 串口1I/O口初始化函数，使用GPIOA的9、10引脚			#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
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

/* ==================================================================
#     函数介绍: 串口1中断优先级配置函数								#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
void USART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;	 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
}

/* ==================================================================
#     函数介绍: 串口1详细参数配置									#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
void USART1_Detail_Configuration(void)
{	
	USART_InitTypeDef USART_InitStructure;
	USART1_RCC_Configuration ();
	USART1_GPIO_Configuration ();
	USART_InitStructure.USART_BaudRate = 115200;
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

/* ==================================================================
#     函数介绍: 串口1初始化函数，外调API以初始化USART1				#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
void USART1_init(void)
{
	USART1_NVIC_Configuration();
	
	USART1_Detail_Configuration();
}

/* ==================================================================
#     函数介绍: 返回当前缓存存储的字符串长度						#
#     参    数:		无												#
#     返 回 值: usart1_rx_fifo_len 缓存中字符串长度					#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
unsigned int usart1_rx_fifo_len(void)
{
	return (usart1_rx_fifo_buf_in - usart1_rx_fifo_buf_out);
}

/* ==================================================================
#     函数介绍: 查询当前缓存是否为空								#
#     参    数:		无												#
#     返 回 值: 是：1	否：0										#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
unsigned char usart1_rx_fifo_is_empty(void)
{
	return (usart1_rx_fifo_buf_in == usart1_rx_fifo_buf_out);
}

/* ==================================================================
#     函数介绍: 查询当前缓存中是否已满								#
#     参    数:		无												#
#     返 回 值: 是：1	否：0										#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
unsigned char usart1_rx_fifo_is_full(void)
{
	return (usart1_rx_fifo_len() > USART1_FIFO_BUF_SIZE_MASK);
}

/* ==================================================================
#     函数介绍: 清除当前串口缓存中的所有数据						#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
void usart1_rx_fifo_clean(void)
{
	if(!usart1_rx_fifo_is_empty())
	{
		usart1_rx_fifo_buf_out = usart1_rx_fifo_buf_in ;
	}
}

/* ==================================================================
#     函数介绍: 配置使用C语言函数printf(); 							#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
	return ch;
}

/* ==================================================================
#     函数介绍: 配置使用C语言函数printf(); 							#
#     参    数:		无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}
	return (int)USART_ReceiveData(USART1);
}

/* ==================================================================
#     函数介绍: 接收串口字符串，并将数据保存到缓存中				#
#     参    数: data : 从串口接收到的数据									#
#     返 回 值:        -1		     ：当前串口缓存已满				#
#				usart1_rx_fifo_buf_in：返回当前缓存数据的长度		#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
int usart1_rx_fifo_put(unsigned char data)
{
	if(usart1_rx_fifo_is_full()){
		return -1;
	}else{
		usart1_rx_fifo_buf[usart1_rx_fifo_buf_in & USART1_FIFO_BUF_SIZE_MASK] = data;
		usart1_rx_fifo_buf_in++;

		return usart1_rx_fifo_buf_in;
	}
}

/* ==================================================================
#     函数介绍: stm32通过串口发送数据								#
#     参    数:	data：待发送数据      									#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
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

/* ==================================================================
#     函数介绍: 获得串口数据										#
#     参    数:	data ：保存数据的指针								#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
int usart1_get_str(unsigned char* data)
{
	while(usart1_rx_fifo_buf_out != usart1_rx_fifo_buf_in){
		
		*data = usart1_rx_fifo_buf[usart1_rx_fifo_buf_out & USART1_FIFO_BUF_SIZE_MASK];
		usart1_rx_fifo_buf_out++;

		data++;
	}
	return usart1_rx_fifo_buf_out;
}

/* ==================================================================
#     函数介绍: 将json格式中的目标对象Tag对应的值字符串转换为数值   #
#     参    数:	*cJson : json字符串									#
#				*Tag   : 要操作的对象标签							#
#     返 回 值: 返回数值的字符串形式的启始地址						#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
char *get_json_value(char *cJson, char *Tag)
{
	char *target = NULL;
	static char temp[10];
	int8_t i=0;
	
	memset(temp, 0x00, 128);
	if(strstr((const char *)Tag, (const char *)"data") != NULL)
	{
		sprintf(temp,"\"%s\":\"",Tag);
	}
	
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
		printf("空字符！\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
	{
		if(*target == '\"')
		{
			break;
		}
		temp[i]=*target;
		
	}
	temp[i+1] = '\0';
	printf("数值=%s\r\n",temp);
	return (char *)temp;
}

/* ==================================================================
#     函数介绍: 解析串口命令并进行相对应的操作					    #
#     参    数:	*RxBuf : 串口命令，json字符串						#
#     返 回 值:    无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
void usart_data_analysis_process(char *RxBuf)
{
	
	usart1_send_str("解析串口命令\r\n");
	char *servo_degree = NULL;
	uint8_t TxetBuf[128];
	
	if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//命令请求？
	{
			if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"defense\"") != NULL)//布防/撤防请求
			{
				memset(TxetBuf,0x00,128);//清空缓存
				if((strstr((const char *)RxBuf, (const char *)"\"data\":\"1\"") != NULL))
				{
					FlagDefense=1;
				}
				else
				{
					usart1_send_str("撤防\r\n");
					FlagDefense=0;
					buzzerClose();
					lightClose();
					lcd_clr_row(2);
					lcd_clr_row(3);
				}
			}
			else if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"ctrl\"") != NULL)//开锁/关锁请求
			{
				memset(TxetBuf,0x00,128);//清空缓存
				if((strstr((const char *)RxBuf, (const char *)"\"data\":\"1\"") != NULL))//开锁
				{
					isAlert=0;//清除警告
					doorOpen();
				}
				else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//关锁
				{
					isAlert=0;
				}
			}
			else if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"servo_vertical\"") != NULL)//开锁/关锁请求
			{
				servo_degree = get_json_value((char *)RxBuf, (char *)"data");
				TIM3_CH1_set_servo_degree(atoi(servo_degree));
			}
			else if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"servo_horizontal\"") != NULL)//开锁/关锁请求
			{
				usart1_send_str("舵机水平调整\r\n");
				servo_degree = get_json_value((char *)RxBuf, (char *)"data");
				TIM3_CH2_set_servo_degree(atoi(servo_degree));
			}
		
	}
}

/* ==================================================================
#     函数介绍: 串口1中断处理函数，接收串口输入中断，并将其保存到数 #
#				据缓存区中											#
#     参    数:	    无												#
#     返 回 值:     无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		usart1_rx_fifo_put(USART_ReceiveData(USART1));
	}
}

