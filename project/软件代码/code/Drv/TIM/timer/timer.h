
#ifndef TIMER_H
#define TIMER_H


/* ���ö�ʱ����Ƶ�� */
#define	TIME2_PRESCALER 		72;
/* ����Ԥ�ȷ�Ƶϵ�� */
#define	TIME2_PERIOD 			1000;
/* ��ʱ���������� 72000000 / 72 / 1000 = 1000 */
/* ����Ƶ��Ϊ1KHz */

void TIM2_Init(void);
#endif

