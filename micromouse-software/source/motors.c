#include "motors.h"

void MOTORS_Init(void)
{
	// clock enable for B and C ports, TIM8
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
	
	// PB7, PB6, PB5, PB4 as an outputs (MOTR_IN1, MOTR_IN2, MOTL_IN1, MOTL_IN2)
	GPIOB->MODER &= ~GPIO_MODER_MODER7;
	GPIOB->MODER |= GPIO_MODER_MODER7_0;
	
	GPIOB->MODER &= ~GPIO_MODER_MODER6;
	GPIOB->MODER |= GPIO_MODER_MODER6_0;
	
	GPIOB->MODER &= ~GPIO_MODER_MODER5;
	GPIOB->MODER |= GPIO_MODER_MODER5_0;
	
	GPIOB->MODER &= ~GPIO_MODER_MODER4;
	GPIOB->MODER |= GPIO_MODER_MODER4_0;	
	
	// PC7, PC6 as an alternative function TIM8 ch2 and ch1, pull-down
	GPIOC->MODER &= ~GPIO_MODER_MODER7;
	GPIOC->MODER |= GPIO_MODER_MODER7_1;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD7_1;
	
	GPIOC->MODER &= ~GPIO_MODER_MODER6;
	GPIOC->MODER |= GPIO_MODER_MODER6_1;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD6_1;
	
	GPIOC->AFR[0] |= 0x33000000;
	
	// ~4khz frequency
	TIM8->PSC = 2-1;
	TIM8->ARR = 1000-1;
	
	//set tim as pwm, enable channel and timer
	TIM8->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM8->CCMR1 |= 	TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;
	TIM8->CCR1 = 0;
	TIM8->CCR2 = 0;
	TIM8->BDTR |= TIM_BDTR_MOE;
	TIM8->CR1 |= TIM_CR1_CEN;
}

void MOTR_SET_SPEED(float speed)
{
	if(speed > 0.0f)
	{
			MOTR_SET_PWM((uint16_t)(fabsf(speed) * 10.0f));
			MOTR_FORWARD
	}
	if(speed < 0.0f)	
	{
			MOTR_SET_PWM((uint16_t)(fabsf(speed) * 10.0f));
			MOTR_BACKWARDS
	}
	if (speed > -0.1f && speed < 0.1f)
	{
		MOTR_SET_PWM(0);
		MOTR_STOP
	}
}
void MOTL_SET_SPEED(float speed)
{
	if(speed > 0.0f)
	{
			MOTL_SET_PWM((uint16_t)(fabsf(speed) * 10.0f));
			MOTL_FORWARD
	}
	if(speed < 0.0f)
	{
			MOTL_SET_PWM((uint16_t)(fabsf(speed) * 10.0f));
			MOTL_BACKWARDS
	}
	if (speed > -0.1f && speed < 0.1f)
	{
		MOTL_SET_PWM(0);
		MOTL_STOP
	}	
}

