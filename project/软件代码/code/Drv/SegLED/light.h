#ifndef LIGHT_H
#define LIGHT_H

#include "stm32f10x.h"


#define LIGHT_PORT GPIOD
#define LIGHT_PIN GPIO_Pin_12

void lightInit(void);
void lightOpen(void);
void lightClose(void);

#endif
