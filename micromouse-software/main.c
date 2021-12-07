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
#include "profiler.h"

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
	TIM7_1KHz_INTERRUPT_Init();
	
	MOVE_CONTROLLER_DISABLE(&MOUSE);
	PROFILER_TRANS_DISABLE(&MOUSE);
	
	MOTOR_PID_INIT(&MOTOR_LEFT, LEFT_MOTOR, 1.0f, 0.7f, 0.001f);
	MOTOR_PID_INIT(&MOTOR_RIGHT, RIGHT_MOTOR, 1.1f, 0.65f, 0.001f);
	
	MOTOR_PID_DISABLE(&MOTOR_LEFT);
	MOTOR_PID_DISABLE(&MOTOR_RIGHT);
	
	while(1)
	{
		
		MOUSE.batteryVoltage = ADC_GET_BATTERY_VOLTAGE();
		
		STATE_Handle(); 
		UART1_COMMAND_PARSERHandler(&UART_Buffer);
		
		
		if(BUTTON_SEL.wasPressed && LONG_PRESS(BUTTON_SEL.time))
			{
				STATE_Selection();
				
				BUTTON_SEL.wasPressed = false;
			}
			
		if(BUTTON_OK.wasPressed && LONG_PRESS(BUTTON_OK.time))
			{
				stateT = RUNNING;
				accelerationT = 0.01f;
				max_velocityT = 7.5f;
				target_ST = 1000.0f;
				
				delay_ms(5000);
				
				MOUSE.state = PROFILER_STATE;
				
				BUTTON_OK.wasPressed = false;
			}	
	}
}
