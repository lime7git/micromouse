#include "timer.h"
#include "gpio.h"
#include "encoders.h"
#include "profiler.h"
#include "controller.h"

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
		
		if(PROFILER_TRANS_IS_ENABLE(&MOUSE))
		{
			float speed = PROFILER_TRANS();
			
			MOTOR_LEFT.set_rpm 	= speed;
			MOTOR_RIGHT.set_rpm = speed;
			
			PROFILER_REGULATOR();
		}
		
		if(MOTOR_PID_IS_ENABLE(&MOTOR_LEFT)) MOTOR_PID_CONTROLLER(&MOTOR_LEFT);
		if(MOTOR_PID_IS_ENABLE(&MOTOR_RIGHT)) MOTOR_PID_CONTROLLER(&MOTOR_RIGHT);
		
	}
}
