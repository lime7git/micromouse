#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "adc.h"
#include "buzzer.h"
#include "ui.h"
#include "motors.h"
#include "circular_buffer.h"
#include "encoders.h"
#include "pid.h"
#include "controller.h"
#include "timer.h"

int main(void)
{
	tCircular_buffer_init(&UART_Buffer, 128);
	
	CLOCK_Init();
	GPIO_Init();
	ADC1_DMA_Init();
	BUZZER_Init();
	MOTORS_Init();
	ENCODERS_Init();
	BUTTON_EXTI_Init();
	UART1_IT_Init();
	TIM7_10MS_INTERRUPT_Init();
	
	MOTOR_PID_INIT(&MOTOR_LEFT, LEFT_MOTOR, 1.0f, 1.0f, 0.01f);
	MOTOR_PID_INIT(&MOTOR_RIGHT, RIGHT_MOTOR, 1.0f, 1.0f, 0.01f);
	
	//STATE_Selection();
	mouse_state = TEST3;
	
	while(1)
	{
		
		STATE_Handle(); 
		UART1_COMMAND_PARSERHandler(&UART_Buffer);
		
		if(BUTTON_SEL.wasPressed && LONG_PRESS(BUTTON_SEL.time))
			{
				STATE_Selection();
				
				BUTTON_SEL.wasPressed = false;
			}
	}
}
