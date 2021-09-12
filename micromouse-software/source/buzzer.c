#include "buzzer.h"
#include "clock.h"

void BUZZER_Init(void)
{
	// enable clock for port B and TIM3
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// set PB1 as alternative function TIM3_CH4, pull-down
	GPIOB->MODER &= ~GPIO_MODER_MODER1;
	GPIOB->MODER |= GPIO_MODER_MODER1_1;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD1_1;
	GPIOB->AFR[0] |= 0x00000020;
	
	// ~4khz frequency
	TIM3->PSC = 16-1;
	TIM3->ARR = 256-1;
	
	//set tim as pwm, enable channel and timer
	TIM3->CCMR2 |= 	TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
	TIM3->CCR4 = 0;
	TIM3->CCER |= TIM_CCER_CC4E;
	TIM3->CR1 |= TIM_CR1_CEN;
}

void BUZZER_Volume(uint8_t volume)
{
	TIM3->CCR4 = volume / 2;
}

void BUZZER_Buzz(uint8_t buzz_volume, uint8_t num_of_buzz, uint16_t buzz_time, uint16_t buzz_delay)
{
	for(uint8_t i = 0; i < num_of_buzz; i++)
	{
		BUZZER_Volume(buzz_volume);
			delay_ms(buzz_time);
		BUZZER_Volume(0);
			delay_ms(buzz_delay);
	}
}
