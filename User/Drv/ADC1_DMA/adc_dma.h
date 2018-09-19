#ifndef _ADC_DMA_H
#define _ADC_DMA_H

#include "stm32f10x.h"

/* 固定一个通道的版本 */

void ADC1_Init(void);
void update_adc1(void);
u16 get_adc_result(void);

#endif
