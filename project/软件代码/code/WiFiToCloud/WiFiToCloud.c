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
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266设置为station模式
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetStation(void)
{
	ClrAtRxBuf();//清空缓存
	SendAtCmd((uint8_t *)AT_CWMODE,strlen(AT_CWMODE));
	delay_ms(100);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}

/*******************************************************************
*函数：int8_t ESP8266_SetAP(void)
*功能：设置ESP8266要连接的热点名称和密码
*输入：char *wifi-热点名称 char *pwd-热点密码
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetAP(char *wifi, char *pwd)
{
	uint8_t AtCwjap[MAX_AT_TX_LEN];
	memset(AtCwjap, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
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
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266建立TCP连接
*输入：
	char *IpAddr-IP地址，例如：120.77.58.34
	uint16_t port-端口号，取值0~65535
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpStart(char *IpAddr, uint16_t port)
{
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
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
*函数：int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
*功能：ESP8266发送数据
*输入：
		char *IpBuf-IP数据
		uint8_t len-数据长度
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t IpSend[MAX_AT_TX_LEN];
	memset(IpSend, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
	sprintf((char *)IpSend,"AT+CIPSEND=%d",len);
	//printf("%s\r\n",IpSend);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpSend,strlen((const char *)IpSend));
	delay_ms(3);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	ClrAtRxBuf();//清空缓存
	SendStrLen((uint8_t *)IpBuf, len);
	//printf("%s\r\n",IpBuf);////////////////////////////////////////////////////////////
	for(TryGo = 0; TryGo<60; TryGo++)//最多等待时间100*60=6000ms
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
*函数：int8_t ConnectToServer(void)
*功能：连接到服务器
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ConnectToServer(char *DeviceID, char *SecretKey)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	for(TryGo = 0; TryGo<3; TryGo++)
	{
		if(ESP8266_SetStation() == 0)//设置WiFi通讯模块工作模式
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
		if(ESP8266_SetAP((char *)WIFI_AP, (char *)WIFI_PWD) == 0)//设置热点名称和密码
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
		if(ESP8266_IpStart((char *)SERVER_IP,SERVER_PORT) == 0)//连接服务器IP地址，端口：120.77.58.34,8600
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
	{//发送失败
		error=-4;
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//检查响应状态是否为握手成功
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
*函数：int8_t ESP8266_SendSensor(char *SensorTag, char *TimeStr, uint8_t sensor)
*功能：发送传感数据到服务器
*输入：
	char *SensorTag, 对象标签名称
	uint8_t sensor, 传感数值
	char *TimeStr,采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SendSensor(char *SensorTag, char *TimeStr, uint8_t sensor)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d}},\"msgid\":001}",SensorTag, TimeStr, sensor);
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-1;
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//检查响应状态是否为"上报成功"
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
*函数：uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
*功能：解析服务器数据
*输入：
		uint8_t *AtRxBuf ，原始AT串口缓存
		char *GetIpData ，截取出来的Itcp/ip数据
*输出：返回收到的IP数据长度
*特殊说明：
	AT+CIPSEND=76
	{"t":3,"datatype":2,"datas":{"alarm":{"2018-06-19 18:15:02":0}},"msgid":001}
	+IPD,29:{"msgid":1,"status":0,"t":4}
*******************************************************************/
uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
{
	char *Point = NULL;
	uint8_t len = 0;
	
	Point = strstr((const char *)AtRxBuf, (const char *)"+IPD,");
	if(Point != NULL)//检查模块收到TCP/IP数据包？
	{
		len=(Point[5]-'0')*10+Point[6]-'0';//这里IP数据长度不会超过100，故可以按“个位”和“十位”计算收到的IP数据长度
		Point = strstr((const char *)AtRxBuf, (const char *)":")+1;
		memcpy(GetIpData, Point, len);
		GetIpData[len]='\0';
		//printf("收到IP数据：%s\r\n",GetIpData);///////////////////////////////////////////////////////
		//printf("收到IP数据长度=%d\r\n",len);///////////////////////////////////////////////////
	}
	
	return (len);
}

/* ==================================================================
#     函数介绍: 将json格式中的目标对象Tag对应的值字符串转换为数值   #
#     参    数:	*cJson : json字符串									#
#				*Tag   : 要操作的对象标签							#
#     返 回 值: 返回数值的字符串形式的启始地址						#
#     作    者: 许     兵											#
#     修改时间: 2018-9-17											#
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
*函数：void ESP8266_DataAnalysisProcess(char *RxBuf)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void ESP8266_DataAnalysisProcess(char *RxBuf)
{
	get_multiple_json_value_for_cloud((char *)RxBuf);
	if(strstr(cloud_cmd.CMD_TYPE, (const char *)PING_REQ) != NULL)//心跳请求？
	{
		ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//响应心跳
	}
	else if(strstr(cloud_cmd.CMD_TYPE, (const char *)"5") != NULL)//命令请求？
	{
		
		if(strstr(cloud_cmd.ELEMENT, (const char *)"ctrl") != NULL)//开锁请求
		{
			if(cloud_cmd.DATA == 1)//开锁
			{
				isAlert=0;//清除警告
				doorOpen();
			}
			else if(cloud_cmd.DATA == 0)//关锁
			{
				isAlert=0;
			}
		}
		else if(strstr(cloud_cmd.ELEMENT, (const char *)"defense") != NULL)//布防/撤防请求
		{
			if(cloud_cmd.DATA == 1)
			{
				FlagDefense=1;
			}
			else if(cloud_cmd.DATA == 0)
			{
				//usart1_send_str("撤防\r\n");
				FlagDefense=0;
				buzzerClose();
				lightClose();
				lcd_clr_row(2);
				lcd_clr_row(3);
			}
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_vertical") != NULL)//开锁/关锁请求
		{
			TIM3_CH1_set_servo_degree(cloud_cmd.DATA);
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_horizontal") != NULL)//开锁/关锁请求
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
*函数：void ESP8266_DataAnalysisProcess(char *RxBuf)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void DataAnalysisProcess(char *RxBuf)
{
	analysis_json_value_for_cloud(RxBuf);
	if(strstr(cloud_cmd.CMD_TYPE, (const char *)PING_REQ) != NULL)//心跳请求？
	{
		ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//响应心跳
	}
	else if(strstr(cloud_cmd.CMD_TYPE, (const char *)"5") != NULL)//命令请求？
	{
		
		if(strstr(cloud_cmd.ELEMENT, (const char *)"ctrl") != NULL)//开锁请求
		{
			if(cloud_cmd.DATA == 1)//开锁
			{
				isAlert=0;//清除警告
				doorOpen();
			}
			else if(cloud_cmd.DATA == 0)//关锁
			{
				isAlert=0;
			}
		}
		else if(strstr(cloud_cmd.ELEMENT, (const char *)"defense") != NULL)//布防/撤防请求
		{
			if(cloud_cmd.DATA == 1)
			{
				FlagDefense=1;
			}
			else if(cloud_cmd.DATA == 0)
			{
				//usart1_send_str("撤防\r\n");
				FlagDefense=0;
				buzzerClose();
				lightClose();
				lcd_clr_row(2);
				lcd_clr_row(3);
			}
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_vertical") != NULL)//开锁/关锁请求
		{
			TIM3_CH1_set_servo_degree(cloud_cmd.DATA);
		}
		else if(strstr(cloud_cmd.ELEMENT,"servo_horizontal") != NULL)//开锁/关锁请求
		{
			TIM3_CH2_set_servo_degree(cloud_cmd.DATA);
		}
	}
}
