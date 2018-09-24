/******************************************************************************
*
*
*
*
*
*
*
*
*
*
******************************************************************************/
#include "WiFiToCloud.h"

Cloud_Cmd cloud_cmd;
extern unsigned char FlagDefense;
extern unsigned char isAlert;
/*******************************************************************
*������int8_t ESP8266_SetStation(void)
*���ܣ�ESP8266����Ϊstationģʽ
*���룺��
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SetStation(void)
{
	ClrAtRxBuf();//��ջ���
	SendAtCmd((uint8_t *)AT_CWMODE,strlen(AT_CWMODE));
	delay_ms(100);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}

/*******************************************************************
*������int8_t ESP8266_SetAP(void)
*���ܣ�����ESP8266Ҫ���ӵ��ȵ����ƺ�����
*���룺char *wifi-�ȵ����� char *pwd-�ȵ�����
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SetAP(char *wifi, char *pwd)
{
	uint8_t AtCwjap[MAX_AT_TX_LEN];
	memset(AtCwjap, 0x00, MAX_AT_TX_LEN);//��ջ���
	ClrAtRxBuf();//��ջ���
	sprintf((char *)AtCwjap,"AT+CWJAP_CUR=\"%s\",\"%s\"",wifi, pwd);
//	printf("%s\r\n",AtCwjap);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)AtCwjap,strlen((const char *)AtCwjap));
	delay_ms(5500);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}

/*******************************************************************
*������int8_t ESP8266_SetStation(void)
*���ܣ�ESP8266����TCP����
*���룺
	char *IpAddr-IP��ַ�����磺120.77.58.34
	uint16_t port-�˿ںţ�ȡֵ0~65535
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_IpStart(char *IpAddr, uint16_t port)
{
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//��ջ���
	ClrAtRxBuf();//��ջ���
	sprintf((char *)IpStart,"AT+CIPSTART=\"TCP\",\"%s\",%d",IpAddr, port);
	//printf("%s\r\n",IpStart);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpStart,strlen((const char *)IpStart));
	delay_ms(1500);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}

/*******************************************************************
*������int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
*���ܣ�ESP8266��������
*���룺
		char *IpBuf-IP����
		uint8_t len-���ݳ���
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t IpSend[MAX_AT_TX_LEN];
	memset(IpSend, 0x00, MAX_AT_TX_LEN);//��ջ���
	ClrAtRxBuf();//��ջ���
	sprintf((char *)IpSend,"AT+CIPSEND=%d",len);
	//printf("%s\r\n",IpSend);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpSend,strlen((const char *)IpSend));
	delay_ms(3);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	ClrAtRxBuf();//��ջ���
	SendStrLen((uint8_t *)IpBuf, len);
	//printf("%s\r\n",IpBuf);////////////////////////////////////////////////////////////
	for(TryGo = 0; TryGo<60; TryGo++)//���ȴ�ʱ��100*60=6000ms
	{
		if(strstr((const char *)AT_RX_BUF, (const char *)"SEND OK") == NULL)
		{
			error = -2;
		}
		else
		{
			error = 0;
			break;
		}
		delay_ms(100);
	}
	return error;
}

/*******************************************************************
*������int8_t ConnectToServer(void)
*���ܣ����ӵ�������
*���룺��
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ConnectToServer(char *DeviceID, char *SecretKey)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_SetStation() == 0)//����WiFiͨѶģ�鹤��ģʽ
		{
			error = 0;
			break;
		}
		else
		{
			error = -1;
		}
	}
	if(error < 0)
	{
		return error;
	}
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_SetAP((char *)WIFI_AP, (char *)WIFI_PWD) == 0)//�����ȵ����ƺ�����
		{
			error = 0;
			break;
		}
		else
		{
			error = -2;
		}
	}
	if(error < 0)
	{
		return error;
	}
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_IpStart((char *)SERVER_IP,SERVER_PORT) == 0)//���ӷ�����IP��ַ���˿ڣ�120.77.58.34,8600
		{
			error = 0;
			break;
		}
		else
		{
			error = -3;
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	sprintf((char *)TxetBuf,"{\"t\":1,\"device\":\"%s\",\"key\":\"%s\",\"ver\":\"v0.0.0.0\"}",DeviceID,SecretKey);
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//����ʧ��
		error=-4;
	}
	else
	{//���ͳɹ�
		for(TryGo = 0; TryGo<50; TryGo++)//���ȴ�ʱ��50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//�����Ӧ״̬�Ƿ�Ϊ���ֳɹ�
			{
				error = -5;
			}
			else
			{
				error = 0;
				break;
			}
			delay_ms(10);
		}
	}
	
	return error;
}

/*******************************************************************
*������int8_t ESP8266_SendSensor(char *SensorTag, char *TimeStr, uint8_t sensor)
*���ܣ����ʹ������ݵ�������
*���룺
	char *SensorTag, �����ǩ����
	uint8_t sensor, ������ֵ
	char *TimeStr,�ɼ�����ʱ��ʱ�䣬����yyyy-mm-dd hh:mm:ss��ʽ
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SendSensor(char *SensorTag, char *TimeStr, uint8_t sensor)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d}},\"msgid\":001}",SensorTag, TimeStr, sensor);
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//����ʧ��
		error=-1;
	}
	else
	{//���ͳɹ�
		for(TryGo = 0; TryGo<50; TryGo++)//���ȴ�ʱ��50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//�����Ӧ״̬�Ƿ�Ϊ"�ϱ��ɹ�"
			{
				error = -1;
			}
			else
			{
				error = 0;
				break;
			}
			delay_ms(10);
		}
	}
	return error;
}

/*******************************************************************
*������uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
*���ܣ���������������
*���룺
		uint8_t *AtRxBuf ��ԭʼAT���ڻ���
		char *GetIpData ����ȡ������Itcp/ip����
*����������յ���IP���ݳ���
*����˵����
	AT+CIPSEND=76
	{"t":3,"datatype":2,"datas":{"alarm":{"2018-06-19 18:15:02":0}},"msgid":001}
	+IPD,29:{"msgid":1,"status":0,"t":4}
*******************************************************************/
uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
{
	char *Point = NULL;
	uint8_t len = 0;
	
	Point = strstr((const char *)AtRxBuf, (const char *)"+IPD,");
	if(Point != NULL)//���ģ���յ�TCP/IP���ݰ���
	{
		len=(Point[5]-'0')*10+Point[6]-'0';//����IP���ݳ��Ȳ��ᳬ��100���ʿ��԰�����λ���͡�ʮλ�������յ���IP���ݳ���
		Point = strstr((const char *)AtRxBuf, (const char *)":")+1;
		memcpy(GetIpData, Point, len);
		GetIpData[len]='\0';
		//printf("�յ�IP���ݣ�%s\r\n",GetIpData);///////////////////////////////////////////////////////
		//printf("�յ�IP���ݳ���=%d\r\n",len);///////////////////////////////////////////////////
	}
	
	return (len);
}

/* ==================================================================
#     ��������: ��json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ���ת��Ϊ��ֵ   #
#     ��    ��:	*cJson : json�ַ���									#
#				*Tag   : Ҫ�����Ķ����ǩ							#
#     �� �� ֵ: ������ֵ���ַ�����ʽ����ʼ��ַ						#
#     ��    ��: ��     ��											#
#     �޸�ʱ��: 2018-9-17											#
================================================================== */
char *get_single_json_value_for_cloud(char *cJson, char *Tag)
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
char *get_multiple_json_value_for_cloud(char *cJson)
{
	static char temp[10];
	int data_temp = 0;
	
	cloud_cmd.SOURCE = "pc";
	if(strstr((const char *)cJson, (const char *)"t") != NULL)
	{
		cloud_cmd.CMD_TYPE = get_single_json_value_for_cloud((char *)cJson, (char *)"t");
		//printf("cloud_cmd.CMD_TYPE=%s\r\n", cloud_cmd.CMD_TYPE);
	}
	if(strstr((const char *)cJson, (const char *)"apitag") != NULL)
	{
		cloud_cmd.ELEMENT = get_single_json_value_for_cloud((char *)cJson, (char *)"apitag");
		//printf("cloud_cmd.ELEMENT=%s\r\n", cloud_cmd.ELEMENT);
	}
	if(strstr((const char *)cJson, (const char *)"data") != NULL)
	{
		data_temp = atoi(get_single_json_value_for_cloud((char *)cJson, (char *)"data"));
		cloud_cmd.DATA = data_temp;
		//printf("cloud_cmd.DATA=%d\r\n", cloud_cmd.DATA);
	}

}


/*******************************************************************
*������void ESP8266_DataAnalysisProcess(char *RxBuf)
*���ܣ���������������
*���룺char *RxBuf �������·�����
*�����
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
void ESP8266_DataAnalysisProcess(char *RxBuf)
{
	get_multiple_json_value_for_cloud((char *)RxBuf);
	if(strstr(cloud_cmd.CMD_TYPE, (const char *)PING_REQ) != NULL)//��������
	{
		ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//��Ӧ����
	}
	else if(strstr(cloud_cmd.CMD_TYPE, (const char *)"5") != NULL)//��������
	{
		
		if(strstr(cloud_cmd.ELEMENT, (const char *)"ctrl") != NULL)//��������
		{
			if(cloud_cmd.DATA == 1)//����
			{
				isAlert=0;//�������
				doorOpen();
			}
			else if(cloud_cmd.DATA == 0)//����
			{
				isAlert=0;
			}
		}
		else if(strstr(cloud_cmd.ELEMENT, (const char *)"defense") != NULL)//����/��������
		{
			if(cloud_cmd.DATA == 1)
			{
				FlagDefense=1;
			}
			else if(cloud_cmd.DATA == 0)
			{
				//usart1_send_str("����\r\n");
				FlagDefense=0;
				buzzerClose();
				lightClose();
				lcd_clr_row(2);
				lcd_clr_row(3);
			}
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_vertical") != NULL)//����/��������
		{
			TIM3_CH1_set_servo_degree(cloud_cmd.DATA);
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_horizontal") != NULL)//����/��������
		{
			TIM3_CH2_set_servo_degree(cloud_cmd.DATA);
		}
	}
}

void analysis_json_value_for_cloud(char *RxBuf)
{
	cJSON *json = cJSON_Parse(RxBuf);
	cJSON *node = NULL;
	//cJOSN_GetObjectItem ??key???json?? ???????
	cloud_cmd.SOURCE = "SERVER";
	cloud_cmd.CMD_TYPE = cJSON_GetObjectItem(json,"t")->valuestring;
	cloud_cmd.ELEMENT = cJSON_GetObjectItem(json,"apitag")->valuestring;
	cloud_cmd.DATA = cJSON_GetObjectItem(json,"data")->valueint;
}

/*******************************************************************
*������void ESP8266_DataAnalysisProcess(char *RxBuf)
*���ܣ���������������
*���룺char *RxBuf �������·�����
*�����
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
void DataAnalysisProcess(char *RxBuf)
{
	analysis_json_value_for_cloud(RxBuf);
	if(strstr(cloud_cmd.CMD_TYPE, (const char *)PING_REQ) != NULL)//��������
	{
		ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//��Ӧ����
	}
	else if(strstr(cloud_cmd.CMD_TYPE, (const char *)"5") != NULL)//��������
	{
		
		if(strstr(cloud_cmd.ELEMENT, (const char *)"ctrl") != NULL)//��������
		{
			if(cloud_cmd.DATA == 1)//����
			{
				isAlert=0;//�������
				doorOpen();
			}
			else if(cloud_cmd.DATA == 0)//����
			{
				isAlert=0;
			}
		}
		else if(strstr(cloud_cmd.ELEMENT, (const char *)"defense") != NULL)//����/��������
		{
			if(cloud_cmd.DATA == 1)
			{
				FlagDefense=1;
			}
			else if(cloud_cmd.DATA == 0)
			{
				//usart1_send_str("����\r\n");
				FlagDefense=0;
				buzzerClose();
				lightClose();
				lcd_clr_row(2);
				lcd_clr_row(3);
			}
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_vertical") != NULL)//����/��������
		{
			TIM3_CH1_set_servo_degree(cloud_cmd.DATA);
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_horizontal") != NULL)//����/��������
		{
			TIM3_CH2_set_servo_degree(cloud_cmd.DATA);
		}
	}
}
