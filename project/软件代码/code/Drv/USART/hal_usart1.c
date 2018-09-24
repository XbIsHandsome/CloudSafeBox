#include "hal_usart1.h"
#include "adc_dma.h"
#include "numcal.h"

unsigned char usart1_rx_fifo_buf[USART1_FIFO_BUF_SIZE-1];
unsigned int usart1_rx_fifo_buf_in = 0;
unsigned int usart1_rx_fifo_buf_out = 0;
extern unsigned char isAlert;//报警标志
//extern uint8_t MEMS_ALERT;

u8 flag_frame = 0;
/*=====================串口命令解析结构体定义======================*/
Usart_Cmd usart_cmd;

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
#     参    数: data : 从串口接收到的数据							#
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
#     参    数:	data：待发送数据      								#
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
int usart1_get_str(char* data)
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
char *get_single_json_value(char *cJson, char *Tag)
{
	char *target = NULL;
	static char temp[20];
	int8_t i=0;
	
	memset(temp, 0x00, 128);
	sprintf(temp,"\"%s\":\"",Tag);
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
		//printf("空字符！\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<20; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
	{
		if(*target == '\"')
		{
			break;
		}
		temp[i]=*target;
		
	}
	temp[i+1] = '\0';
	//printf("数值=%s\r\n",temp);
	return (char *)temp;
}

/* ==================================================================
#     函数介绍: 将json格式中的目标对象Tag对应的值字符串转换为数值   #
#     参    数:	*cJson : json字符串									#
#				*Tag   : 要操作的对象标签							#
#     返 回 值: 返回数值的字符串形式的启始地址						#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
char *get_multiple_json_value(char *cJson)
{
	static char temp[10];
	int data_temp = 0;
	
	if(strstr((const char *)cJson, (const char *)"SOURCE") != NULL)
	{
		usart_cmd.SOURCE = get_single_json_value((char *)cJson, (char *)"SOURCE");
		printf("usart_cmd.SOURCE=%s\r\n", usart_cmd.SOURCE);
	}
	if(strstr((const char *)cJson, (const char *)"CMD_TYPE") != NULL)
	{
		usart_cmd.CMD_TYPE = get_single_json_value((char *)cJson, (char *)"CMD_TYPE");
		printf("usart_cmd.CMD_TYPE=%s\r\n", usart_cmd.CMD_TYPE);
	}
	if(strstr((const char *)cJson, (const char *)"ELEMENT") != NULL)
	{
		usart_cmd.ELEMENT = get_single_json_value((char *)cJson, (char *)"ELEMENT");
		printf("usart_cmd.ELEMENT=%s\r\n", usart_cmd.ELEMENT);
	}
	if(strstr((const char *)cJson, (const char *)"DATA") != NULL)
	{
		data_temp = atoi(get_single_json_value((char *)cJson, (char *)"DATA"));
		usart_cmd.DATA = data_temp;
		printf("usart_cmd.DATA=%d\r\n", usart_cmd.DATA);
	}

}

/* ==================================================================
#     函数介绍: 解析JSON数据格式的字符串，提取有效信息并将获取到的值#
#				赋值给定好的Usart_Cmd结构体							#
#     参    数:	*RxBuf : 串口命令，json字符串						#
#     返 回 值:    无												#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
================================================================== */
void analysis_json_value(char *RxBuf)
{
	cJSON *json = cJSON_Parse(RxBuf);
	cJSON *node = NULL;
	//cJOSN_GetObjectItem ??key???json?? ???????
	usart_cmd.SOURCE = cJSON_GetObjectItem(json,"SOURCE")->valuestring;
	usart_cmd.CMD_TYPE = cJSON_GetObjectItem(json,"TYPE")->valuestring;
	usart_cmd.ELEMENT = cJSON_GetObjectItem(json,"NAME")->valuestring;
	usart_cmd.DATA = atoi(cJSON_GetObjectItem(json,"DATA")->valuestring);
	
	//printf("cJSON_GetObjectItem(json,\"DATA\")'s type = %d",cJSON_GetObjectItem(json,"DATA")->type);
	usart1_rx_fifo_clean();
	printf("usart_cmd.SOURCE = %s\r\n", usart_cmd.SOURCE);
	printf("usart_cmd.CMD_TYPE = %s\r\n", usart_cmd.CMD_TYPE);
	printf("usart_cmd.ELEMENT = %s\r\n", usart_cmd.ELEMENT);
	printf("usart_cmd.DATA = %d\r\n", usart_cmd.DATA);
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
	
	usart1_send_str((char *)"解析串口命令\r\n");
	char *servo_degree = NULL;
	uint8_t TxetBuf[128];
	analysis_json_value((char *)RxBuf);
	
	if(strstr(usart_cmd.CMD_TYPE,"5") != NULL)//命令请求？
	{
		if(strstr(usart_cmd.ELEMENT,"defense") != NULL)//布防/撤防请求
		{
			memset(TxetBuf,0x00,128);//清空缓存
			if(usart_cmd.DATA == 1)
			{
				FlagDefense=1;
			}
			else if(usart_cmd.DATA == 0)
			{
				usart1_send_str("撤防\r\n");
				FlagDefense=0;
				buzzerClose();
				lightClose();
				//lcd_clr_row(2);
				//lcd_clr_row(3);
			}
		}
		else if(strstr(usart_cmd.ELEMENT,"ctrl") != NULL)//开锁/关锁请求
		{
			memset(TxetBuf,0x00,128);//清空缓存
			if(usart_cmd.DATA == 1)//开锁
			{
				isAlert=0;//清除警告
				doorOpen();
			}
			else if(usart_cmd.DATA == 0)//关锁
			{
				isAlert=0;
			}
		}
		else if(strstr(usart_cmd.ELEMENT,"servo_vertical") != NULL)//开锁/关锁请求
		{
			//TIM3_CH1_set_servo_degree(usart_cmd.DATA);
		}
		else if(strstr(usart_cmd.ELEMENT,"servo_horizontal") != NULL)//开锁/关锁请求
		{
			//TIM3_CH2_set_servo_degree(usart_cmd.DATA);
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
u16 usart1_read_str(char* buf, u16 len)
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
    char buf[64];

    if (1 == flag_frame)
    {
        flag_frame = 0;
        len = usart1_read_str(buf, usart1_rx_fifo_len());
        buf[len] = '\0';
        usart1_action(buf, len);
    }
}

void usart1_send_num(u16 data)
{
    char buf[6];

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
void usart1_action(char* buf, u8 len)
{
	//char *usart_temp;
	//sprintf(usart_temp, "%s", buf);
	//usart1_send_str(buf);
	usart_data_analysis_process(buf);
}
