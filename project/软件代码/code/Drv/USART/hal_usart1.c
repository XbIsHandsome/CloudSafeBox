#include "hal_usart1.h"
#include "adc_dma.h"
#include "numcal.h"

unsigned char usart1_rx_fifo_buf[USART1_FIFO_BUF_SIZE-1];
unsigned int usart1_rx_fifo_buf_in = 0;
unsigned int usart1_rx_fifo_buf_out = 0;
extern unsigned char isAlert;//������־
//extern uint8_t MEMS_ALERT;

u8 flag_frame = 0;
/*=====================������������ṹ�嶨��======================*/
Usart_Cmd usart_cmd;

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
#     ��    ��: data : �Ӵ��ڽ��յ�������							#
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
#     ��    ��:	data������������      								#
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
#     ��������: ��json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ���ת��Ϊ��ֵ   #
#     ��    ��:	*cJson : json�ַ���									#
#				*Tag   : Ҫ�����Ķ����ǩ							#
#     �� �� ֵ: ������ֵ���ַ�����ʽ����ʼ��ַ						#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
		//printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<20; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
	{
		if(*target == '\"')
		{
			break;
		}
		temp[i]=*target;
		
	}
	temp[i+1] = '\0';
	//printf("��ֵ=%s\r\n",temp);
	return (char *)temp;
}

/* ==================================================================
#     ��������: ��json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ���ת��Ϊ��ֵ   #
#     ��    ��:	*cJson : json�ַ���									#
#				*Tag   : Ҫ�����Ķ����ǩ							#
#     �� �� ֵ: ������ֵ���ַ�����ʽ����ʼ��ַ						#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
#     ��������: ����JSON���ݸ�ʽ���ַ�������ȡ��Ч��Ϣ������ȡ����ֵ#
#				��ֵ�����õ�Usart_Cmd�ṹ��							#
#     ��    ��:	*RxBuf : �������json�ַ���						#
#     �� �� ֵ:    ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
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
#     ��������: ������������������Ӧ�Ĳ���					    #
#     ��    ��:	*RxBuf : �������json�ַ���						#
#     �� �� ֵ:    ��												#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
void usart_data_analysis_process(char *RxBuf)
{
	
	usart1_send_str((char *)"������������\r\n");
	char *servo_degree = NULL;
	uint8_t TxetBuf[128];
	analysis_json_value((char *)RxBuf);
	
	if(strstr(usart_cmd.CMD_TYPE,"5") != NULL)//��������
	{
		if(strstr(usart_cmd.ELEMENT,"defense") != NULL)//����/��������
		{
			memset(TxetBuf,0x00,128);//��ջ���
			if(usart_cmd.DATA == 1)
			{
				FlagDefense=1;
			}
			else if(usart_cmd.DATA == 0)
			{
				usart1_send_str("����\r\n");
				FlagDefense=0;
				buzzerClose();
				lightClose();
				//lcd_clr_row(2);
				//lcd_clr_row(3);
			}
		}
		else if(strstr(usart_cmd.ELEMENT,"ctrl") != NULL)//����/��������
		{
			memset(TxetBuf,0x00,128);//��ջ���
			if(usart_cmd.DATA == 1)//����
			{
				isAlert=0;//�������
				doorOpen();
			}
			else if(usart_cmd.DATA == 0)//����
			{
				isAlert=0;
			}
		}
		else if(strstr(usart_cmd.ELEMENT,"servo_vertical") != NULL)//����/��������
		{
			//TIM3_CH1_set_servo_degree(usart_cmd.DATA);
		}
		else if(strstr(usart_cmd.ELEMENT,"servo_horizontal") != NULL)//����/��������
		{
			//TIM3_CH2_set_servo_degree(usart_cmd.DATA);
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
/*********************** ��ӵĽ��մ����� **************************/


/* ����Ϊ�жϽ���������������ʱ�䣬ʱ��Ҫ���Զ�ʱ�������жϵ�ʱ�� */
void usart1_rx_fifo_monitor(u16 time)
{
    static u16 cnt_bkp = 0;     /* ��Ϊ��¼֮ǰһ�γ��ȵĴ洢�� */
    static u16 idle_tmr = 0;    /* û�н��ܵ����ݵ�ʱ�� */


    if (usart1_rx_fifo_len() > 0) /* ���յ������� */
    {
        /* ���յ������ݳ�����֮ǰ�Ƚϲ���� */
        /* ˵�����ܵ����������� */
        if (cnt_bkp != usart1_rx_fifo_len())
        {
            cnt_bkp = usart1_rx_fifo_len();
            /* ������м��� */
            idle_tmr = 0;
        }
        else
        {
            /* ˵��û�н��ܵ����������� */
            if (idle_tmr < 30)
            {
                /* ���м������� */
                idle_tmr += time;
                /* ������30msû�н��ܵ�����ʱ���ж����������Ѿ����� */
                if (idle_tmr >= 30)
                {
                    /* ��־λ */
                    flag_frame = 1;
                }
            }
        }
    }
    else
    {
        /* ���ܳ������� */
        cnt_bkp = 0;
    }
}

/* buf ��ȡ���ݴ浽�����飬 lenΪ��ȡ�ĳ��� */
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

/*  �������ݵĺ������ɸ��ĵĲ��� */
void usart1_action(char* buf, u8 len)
{
	//char *usart_temp;
	//sprintf(usart_temp, "%s", buf);
	//usart1_send_str(buf);
	usart_data_analysis_process(buf);
}
