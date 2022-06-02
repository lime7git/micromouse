#include "clock.h"

volatile static uint32_t tick_delay;
volatile static uint32_t tick_system;

void CLOCK_Init(void)
{
	//Enable HSE and configure clock to run on max 168 MHz
	FLASH->ACR |= (5 << FLASH_ACR_LATENCY_Pos);
	RCC->CR |= RCC_CR_HSEON;
		while(!(RCC->CR & RCC_CR_HSERDY)){} // wait for HSE to be stable
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
	RCC->PLLCFGR |= 4 << RCC_PLLCFGR_PLLQ_Pos;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= 336 << RCC_PLLCFGR_PLLN_Pos;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= 25 << RCC_PLLCFGR_PLLM_Pos;
	RCC->CR |= RCC_CR_PLLON;
		while(!(RCC->CR & RCC_CR_PLLRDY)){} // wait for PLL to lock	
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4 | (RCC_CFGR_PPRE2_DIV2 << 3U);	// idk why it is necessary to shift 3 bit left - but it is working according to HAL
	RCC->CFGR |= RCC_CFGR_SW_PLL;
		while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)){}
		
	SystemCoreClockUpdate();		
	SysTick_Config(SystemCoreClock / 1000);	
}

void SysTick_Handler(void)
{
	tick_delay++;
	tick_system++;
}

void delay_ms(uint32_t ms)
{
	tick_delay = 0;
	while(tick_delay < ms);
}

uint32_t GET_SYSTEM_TICK(void)
{
	return tick_system;
}
