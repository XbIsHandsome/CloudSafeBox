�����ļ����滻codeĿ¼�µ�USART�ļ���
main����ӣ�
// ���մ�������
		if(usart1_rx_fifo_len() > 3){
			usart1_get_str(usart_data_buf);
			printf("���յ�������:%s\r\n",usart_data_buf);
			usart_data_analysis_process((char *)usart_data_buf);
			usart1_rx_fifo_clean();
		}