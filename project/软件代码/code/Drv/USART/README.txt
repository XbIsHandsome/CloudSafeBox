将此文件夹替换code目录下的USART文件夹
main（添加）
// 接收串口命令
		if(usart1_rx_fifo_len() > 3){
			usart1_get_str(usart_data_buf);
			printf("接收到的数据:%s\r\n",usart_data_buf);
			usart_data_analysis_process((char *)usart_data_buf);
			usart1_rx_fifo_clean();
		}