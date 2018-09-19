#ifndef _ULTRASONTIC_H
#define _ULTRASONTIC_H

#include "stm32f10x.h"

//�ص�����
typedef void (*ULTRASONIC_CALLBACK_FUNCTION)(uint32_t ultrasonic_new_dist);

//�����ź�
#define UL1TRIG_PIN			GPIO_Pin_9
#define UL1TRIG_PORT		GPIOB
//�ز��ź�
#define UL1ECHO_PIN			GPIO_Pin_8
#define UL1ECHO_PORT		GPIOB

//��ȡ����
#define READ_UL1ECHO		GPIO_ReadInputDataBit(UL1ECHO_PORT, UL1ECHO_PIN)
#define UL1TRIG_H				GPIO_SetBits(UL1TRIG_PORT, UL1TRIG_PIN)
#define UL1TRIG_L				GPIO_ResetBits(UL1TRIG_PORT, UL1TRIG_PIN)

//��������
void ultrasonic_init(ULTRASONIC_CALLBACK_FUNCTION ultrasonic_update_flag);
void ULsonicGPIO_Start(void);
void ULsonicGPIO_Measure(void);

#endif