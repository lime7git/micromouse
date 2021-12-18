#include "timer.h"
#include "gpio.h"
#include "encoders.h"
#include "profiler.h"
#include "controller.h"
#include "uart.h"

extern 	sProfiler Translation;
extern	sProfiler Rotation;
extern	sPDController Controller;

volatile static uint8_t tick_200ms = 0;

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
		
		if(PROFILER_TRANSLATION_IS_ENABLE(&MOUSE) && PROFILER_ROTATION_IS_ENABLE(&MOUSE))
		{
			PROFILER_TRANSLATION_CONTROLLER(&Translation);
			PROFILER_ROTATION_CONTROLLER(&Rotation);
			
			PROFILER_PD_CONTROLLER(&Controller, &MOUSE);
		}
		
		if(MOTOR_PID_IS_ENABLE(&MOTOR_LEFT)) MOTOR_PID_CONTROLLER(&MOTOR_LEFT);
		if(MOTOR_PID_IS_ENABLE(&MOTOR_RIGHT)) MOTOR_PID_CONTROLLER(&MOTOR_RIGHT);
		
		tick_200ms++;
		if(tick_200ms >= 20)
		{
			tick_200ms = 0;
			if(MOUSE.is_data_logger_enable)
			{
				static char buf[32];
				static int x1, y1, x2, y2, ang;
				
				x1 = x2;
				y1 = y2;
				
				x2 = (int)MOUSE.pos_x;
				y2 = (int)MOUSE.pos_y;
				
				ang = (int)MOUSE.ang;
				
				sprintf(buf, "\nX%iY%iX%iY%iA%i",x1,y1,x2,y2,ang);	
				UART1_Log(buf);
			}
		}
		
	}
}
