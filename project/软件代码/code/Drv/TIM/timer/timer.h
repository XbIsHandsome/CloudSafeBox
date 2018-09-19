
#ifndef TIMER_H
#define TIMER_H


/* 设置定时器的频率 */
#define	TIME2_PRESCALER 		72;
/* 设置预先分频系数 */
#define	TIME2_PERIOD 			1000;
/* 定时器触发周期 72000000 / 72 / 1000 = 1000 */
/* 触发频率为1KHz */

void TIM2_Init(void);
#endif

