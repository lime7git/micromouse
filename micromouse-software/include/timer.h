#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"

void TIM7_10MS_INTERRUPT_Init(void);
void TIM7_IRQHandler(void);

#endif
