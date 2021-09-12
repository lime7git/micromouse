#include "clock.h"

volatile static uint32_t tick;

void CLOCK_Init(void)
{
	SysTick_Config(16000000 / 1000);
}

void SysTick_Handler(void)
{
	tick++;
}

void delay_ms(uint32_t ms)
{
	tick = 0;
	while(tick < ms);
}
