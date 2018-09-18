#include "usart.h"

unsigned char usart1_rx_fifo_buf[USART1_FIFO_BUF_SIZE-1];
unsigned int usart1_rx_fifo_buf_in = 0;
unsigned int usart1_rx_fifo_buf_out = 0;
extern unsigned char isAlert;//������־
extern uint8_t MEMS_ALERT;

/* ==================================================================
#     ��������: ����1ʱ�ӳ�ʼ������									#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
static void USART1_RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/* ==================================================================
#     ��������: ����1I/O�ڳ�ʼ��������ʹ��GPIOA��9��10����			#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
#     ��������: ����1�ж����ȼ����ú���								#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
#     ��������: ����1��ϸ��������									#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
#     ��������: ����1��ʼ�����������API�Գ�ʼ��USART1				#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
void USART1_init(void)
{
	USART1_NVIC_Configuration();
	
	USART1_Detail_Configuration();
}

/* ==================================================================
#     ��������: ���ص�ǰ����洢���ַ�������						#
#     ��    ��:		��												#
#     �� �� ֵ: usart1_rx_fifo_len �������ַ�������					#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
unsigned int usart1_rx_fifo_len(void)
{
	return (usart1_rx_fifo_buf_in - usart1_rx_fifo_buf_out);
}

/* ==================================================================
#     ��������: ��ѯ��ǰ�����Ƿ�Ϊ��								#
#     ��    ��:		��												#
#     �� �� ֵ: �ǣ�1	��0										#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
unsigned char usart1_rx_fifo_is_empty(void)
{
	return (usart1_rx_fifo_buf_in == usart1_rx_fifo_buf_out);
}

/* ==================================================================
#     ��������: ��ѯ��ǰ�������Ƿ�����								#
#     ��    ��:		��												#
#     �� �� ֵ: �ǣ�1	��0										#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
unsigned char usart1_rx_fifo_is_full(void)
{
	return (usart1_rx_fifo_len() > USART1_FIFO_BUF_SIZE_MASK);
}

/* ==================================================================
#     ��������: �����ǰ���ڻ����е���������						#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
void usart1_rx_fifo_clean(void)
{
	if(!usart1_rx_fifo_is_empty())
	{
		usart1_rx_fifo_buf_out = usart1_rx_fifo_buf_in ;
	}
}

/* ==================================================================
#     ��������: ����ʹ��C���Ժ���printf(); 							#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
	return ch;
}

/* ==================================================================
#     ��������: ����ʹ��C���Ժ���printf(); 							#
#     ��    ��:		��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}
	return (int)USART_ReceiveData(USART1);
}

/* ==================================================================
#     ��������: ���մ����ַ������������ݱ��浽������				#
#     ��    ��: data : �Ӵ��ڽ��յ�������									#
#     �� �� ֵ:        -1		     ����ǰ���ڻ�������				#
#				usart1_rx_fifo_buf_in�����ص�ǰ�������ݵĳ���		#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
#     ��������: stm32ͨ�����ڷ�������								#
#     ��    ��:	data������������      									#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
void usart1_send_str(char* data)
{
	if(data == NULL){
		//printf("dataΪ��");
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
#     ��������: ��ô�������										#
#     ��    ��:	data ���������ݵ�ָ��								#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
#     ��������: ��json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ���ת��Ϊ��ֵ   #
#     ��    ��:	*cJson : json�ַ���									#
#				*Tag   : Ҫ�����Ķ����ǩ							#
#     �� �� ֵ: ������ֵ���ַ�����ʽ����ʼ��ַ						#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
		printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
	{
		if(*target == '\"')
		{
			break;
		}
		temp[i]=*target;
		
	}
	temp[i+1] = '\0';
	printf("��ֵ=%s\r\n",temp);
	return (char *)temp;
}

/* ==================================================================
#     ��������: ������������������Ӧ�Ĳ���					    #
#     ��    ��:	*RxBuf : �������json�ַ���						#
#     �� �� ֵ:    ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
void usart_data_analysis_process(char *RxBuf)
{
	
	usart1_send_str("������������\r\n");
	char *servo_degree = NULL;
	uint8_t TxetBuf[128];
	
	if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//��������
	{
			if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"defense\"") != NULL)//����/��������
			{
				memset(TxetBuf,0x00,128);//��ջ���
				if((strstr((const char *)RxBuf, (const char *)"\"data\":\"1\"") != NULL))
				{
					FlagDefense=1;
				}
				else
				{
					usart1_send_str("����\r\n");
					FlagDefense=0;
					buzzerClose();
					lightClose();
					lcd_clr_row(2);
					lcd_clr_row(3);
				}
			}
			else if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"ctrl\"") != NULL)//����/��������
			{
				memset(TxetBuf,0x00,128);//��ջ���
				if((strstr((const char *)RxBuf, (const char *)"\"data\":\"1\"") != NULL))//����
				{
					isAlert=0;//�������
					doorOpen();
				}
				else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//����
				{
					isAlert=0;
				}
			}
			else if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"servo_vertical\"") != NULL)//����/��������
			{
				servo_degree = get_json_value((char *)RxBuf, (char *)"data");
				TIM3_CH1_set_servo_degree(atoi(servo_degree));
			}
			else if(strstr((const char *)RxBuf, (const char *)"\"cmdtag\":\"servo_horizontal\"") != NULL)//����/��������
			{
				usart1_send_str("���ˮƽ����\r\n");
				servo_degree = get_json_value((char *)RxBuf, (char *)"data");
				TIM3_CH2_set_servo_degree(atoi(servo_degree));
			}
		
	}
}

/* ==================================================================
#     ��������: ����1�жϴ����������մ��������жϣ������䱣�浽�� #
#				�ݻ�������											#
#     ��    ��:	    ��												#
#     �� �� ֵ:     ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		usart1_rx_fifo_put(USART_ReceiveData(USART1));
	}
}

