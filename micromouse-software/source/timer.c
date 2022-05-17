#include "timer.h"
#include "gpio.h"
#include "encoders.h"
#include "controller.h"
#include "uart.h"
#include "adc.h"

void TIM7_1KHz_INTERRUPT_Init(void)
{
	/*
		TIM7 - APB1 RUNNING ON 84 MHz
	
		tim uptade frequency = TIM_CLK / (TIM_PSC + 1) / (TIM_ARR + 1)
		
		100Hz	= 84 000 000 / 20 / 42 000
	
		100Hz = 10 ms
	
	*/
		RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
		TIM7->PSC = 20 - 1;
		TIM7->ARR = 42000 - 1;
		TIM7->DIER |= TIM_DIER_UIE;
		TIM7->CR1 |= TIM_CR1_CEN;
	
		NVIC_EnableIRQ(TIM7_IRQn);
}
void TIM7_IRQHandler(void)
{
	if( (TIM7->SR & TIM_SR_UIF) != RESET)
	{
		TIM7->SR &= ~TIM_SR_UIF;	// clear interrupt flag
		
		TEST_PIN_ON;
		
		CALCULATE_ACTUAL_POSITION(&MOUSE, &MOTOR_LEFT, &MOTOR_RIGHT);
		
		if(MOVE_CONTROLLER_IS_ENABLE(&MOUSE)) 
		{
			if(MOUSE.forward_control) 	MOVE_CONTROLLER_FORWARD(&MOUSE);
			if(MOUSE.rotation_control) 	MOVE_CONTROLLER_DIRECTION(&MOUSE);
		}
		
		if(MOTOR_SPEED_PROFILER_IS_ENABLE(&MOTOR_LEFT) && MOTOR_SPEED_PROFILER_IS_ENABLE(&MOTOR_RIGHT))
		{
			MOTOR_SPEED_PROFILER(&MOUSE, &MOTOR_LEFT);
			MOTOR_SPEED_PROFILER(&MOUSE, &MOTOR_RIGHT);
		}
		else if(MOUSE.state != MOUSE_MANUAL)
		{
			MOTOR_LEFT.set_rpm 	=	MOUSE.forward + MOUSE.direction;
			MOTOR_RIGHT.set_rpm = MOUSE.forward - MOUSE.direction;
		}
		
		if(MOTOR_PID_IS_ENABLE(&MOTOR_LEFT)) MOTOR_PID_CONTROLLER(&MOTOR_LEFT);
		if(MOTOR_PID_IS_ENABLE(&MOTOR_RIGHT)) MOTOR_PID_CONTROLLER(&MOTOR_RIGHT);
		
		TEST_PIN_OFF;
	}
}
