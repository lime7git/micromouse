#ifndef _CLOCK_H
#define _CLOCK_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"

#include "gpio.h"

void CLOCK_Init(void);
void SysTick_Handler(void);
void delay_ms(uint32_t ms);
uint32_t GET_SYSTEM_TICK(void);

#endif
