#include "timer.h"
#include "gpio.h"
#include "encoders.h"
#include "profiler.h"

void TIM7_1KHz_INTERRUPT_Init(void)
{
		RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
		TIM7->PSC = 168 - 1;
		TIM7->ARR = 500 - 1;
		TIM7->DIER |= TIM_DIER_UIE;
		TIM7->CR1 |= TIM_CR1_CEN;
	
		NVIC_EnableIRQ(TIM7_IRQn);
}
void TIM7_IRQHandler(void)
{
	if( (TIM7->SR & TIM_SR_UIF) != RESET)
	{
		TIM7->SR &= ~TIM_SR_UIF;
		
		CALCULATE_ACTUAL_POSITION(&MOUSE, &MOTOR_LEFT, &MOTOR_RIGHT);
		MOTOR_CALCULATE_SPEED(&MOTOR_LEFT);
		MOTOR_CALCULATE_SPEED(&MOTOR_RIGHT);
		
		if(MOVE_CONTROLLER_IS_ENABLE(&MOUSE)) 
		{
			MOVE_CONTROLLER(&MOUSE);
			
			MOTOR_LEFT.set_rpm 	= MOUSE.Front + MOUSE.Dir;
			MOTOR_RIGHT.set_rpm = MOUSE.Front - MOUSE.Dir;
		}
		
		if(PROFILER_IS_ENABLE(&MOUSE))
		{
			int speed = PROFILER();
			
			MOTOR_LEFT.set_rpm 	= (float)speed;
			MOTOR_RIGHT.set_rpm = (float)speed;
			
		}
		
		if(MOTOR_PID_IS_ENABLE(&MOTOR_LEFT)) MOTOR_PID_CONTROLLER(&MOTOR_LEFT);
		if(MOTOR_PID_IS_ENABLE(&MOTOR_RIGHT)) MOTOR_PID_CONTROLLER(&MOTOR_RIGHT);
		
	}
}
