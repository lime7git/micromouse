#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"

void TIM7_1KHz_INTERRUPT_Init(void);
void TIM7_IRQHandler(void);

void TIM9_1MHz_INTERRUPT_INIT(void);
void TIM1_BRK_TIM9_IRQHandler(void);
void delay_us(uint8_t us);

#endif
