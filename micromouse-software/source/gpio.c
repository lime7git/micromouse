#include "gpio.h"
#include "uart.h"

volatile BUTTONS BUTTON_OK = {false, false, 0};
volatile BUTTONS BUTTON_SEL = {false, false, 0};

void GPIO_Init(void)
{
	// clock enable for A, B and C ports
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	// PB12, PB0, PC4, PC5 as an outputs (LEDs)
	GPIOB->MODER &= ~GPIO_MODER_MODER12;
	GPIOB->MODER |= GPIO_MODER_MODER12_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER0;
	GPIOB->MODER |= GPIO_MODER_MODER0_0;	
	
	GPIOC->MODER &= ~GPIO_MODER_MODER4;
	GPIOC->MODER |= GPIO_MODER_MODER4_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER5;
	GPIOC->MODER |= GPIO_MODER_MODER5_0;
	
	// PA8 (IR_L), PB13 (IR_R), PC8 (IR_FS_L), PB15 (IR_FS_R) as an outputs (IRs)
	GPIOA->MODER &= ~GPIO_MODER_MODER8;
	GPIOA->MODER |= GPIO_MODER_MODER8_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER13;
	GPIOB->MODER |= GPIO_MODER_MODER13_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER8;
	GPIOC->MODER |= GPIO_MODER_MODER8_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER15;
	GPIOB->MODER |= GPIO_MODER_MODER15_0;
	
	// TEST PINS
	GPIOB->MODER &= ~GPIO_MODER_MODER9;
	GPIOB->MODER |= GPIO_MODER_MODER9_0;
  GPIOB->MODER &= ~GPIO_MODER_MODER8;
	GPIOB->MODER |= GPIO_MODER_MODER8_0;
	
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;
	
	
	
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
	
	IR_LEFT_FRONT_OFF;
	IR_RIGHT_FRONT_OFF;
	IR_LEFT_SIDE_OFF;
	IR_RIGHT_SIDE_OFF;
	
}
void BUTTON_EXTI_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	
	GPIOB->MODER &= ~GPIO_MODER_MODER10;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PB;
	EXTI->IMR |= EXTI_IMR_IM10;
	EXTI->FTSR |= EXTI_FTSR_TR10;
	EXTI->RTSR |= EXTI_RTSR_TR10;
	

	GPIOB->MODER &= ~GPIO_MODER_MODER11;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PB;
	EXTI->IMR |= EXTI_IMR_IM11;
	EXTI->FTSR |= EXTI_FTSR_TR11;
	EXTI->RTSR |= EXTI_RTSR_TR11;
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	TIM6->PSC = 56000;
	TIM6->ARR = 65535;
	TIM6->CR1 = TIM_CR1_CEN;
}

void EXTI15_10_IRQHandler(void)
{
	if((EXTI->PR & EXTI_PR_PR10) != RESET)
	{
		if(!BUTTON_SEL.isPressed)
		{
			BUTTON_SEL.time = 0;
			BUTTON_SEL.isPressed = true;
			BUTTON_SEL.wasPressed = false;
			TIM6->CNT = 0;
			
		}
		else
		{
			BUTTON_SEL.time = CNT_2_MILISECONDS((uint16_t)TIM6->CNT);
			BUTTON_SEL.isPressed = false;
			BUTTON_SEL.wasPressed = true;
		}
		
		EXTI->PR |= EXTI_PR_PR10;
	}
	
	if((EXTI->PR & EXTI_PR_PR11) != RESET)
	{
		if(!BUTTON_OK.isPressed)
		{
			BUTTON_OK.time = 0;
			BUTTON_OK.isPressed = true;
			BUTTON_OK.wasPressed = false;
			TIM6->CNT = 0;
		}
		else
		{
			BUTTON_OK.time = CNT_2_MILISECONDS((uint16_t)TIM6->CNT);
			BUTTON_OK.isPressed = false;
			BUTTON_OK.wasPressed = true;
		}
		
		EXTI->PR |= EXTI_PR_PR11;
	}
}

void LED_Switch(eLEDnum num, eLEDstate state)
{
	switch(num)
	{
		case LED1:
			if(state == OFF)				LED1_OFF;
			else if(state == ON)		LED1_ON;
			else if(state == TOG)		LED1_TOG;
		break;
		
		case LED2:
			if(state == OFF)				LED2_OFF;
			else if(state == ON)		LED2_ON;
			else if(state == TOG)		LED2_TOG;
		break;
		
		case LED3:
			if(state == OFF)				LED3_OFF;
			else if(state == ON)		LED3_ON;
			else if(state == TOG)		LED3_TOG;
		break;
		
		case LED4:
			if(state == OFF)				LED4_OFF;
			else if(state == ON)		LED4_ON;
			else if(state == TOG)		LED4_TOG;
		break;
		case LED_ALL:
			if(state == OFF)				
			{
				LED1_OFF;
				LED2_OFF;
				LED3_OFF;
				LED4_OFF;
			}
			else if(state == ON)		
			{
				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;
			}
			else if(state == TOG)	
			{
				LED1_TOG;
				LED2_TOG;
				LED3_TOG;
				LED4_TOG;
			}
		break;
	}
}