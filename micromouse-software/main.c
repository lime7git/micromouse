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
	ADC2_DMA_init();
	//BUZZER_Init();
	//MOTORS_Init();
	//ENCODERS_Init();
	BUTTON_EXTI_Init();
	UART1_IT_Init();
	TIM7_1KHz_INTERRUPT_Init();
	
	/*
	MOTOR_PID_INIT(&MOTOR_LEFT, LEFT_MOTOR, 1.0f, 0.7f, 0.001f);
	MOTOR_PID_INIT(&MOTOR_RIGHT, RIGHT_MOTOR, 1.1f, 0.65f, 0.001f);
	
	MOTOR_SPEED_PROFILER_INIT(&MOTOR_LEFT, 500.0f, 1.0f);
	MOTOR_SPEED_PROFILER_INIT(&MOTOR_RIGHT, 500.0f, 1.0f);
	
	MOTOR_PID_DISABLE(&MOTOR_LEFT);
	MOTOR_PID_DISABLE(&MOTOR_RIGHT);
	MOTOR_SPEED_PROFILER_DISABLE(&MOTOR_LEFT);
	MOTOR_SPEED_PROFILER_DISABLE(&MOTOR_RIGHT);
	MOVE_CONTROLLER_DISABLE(&MOUSE);
	*/
	
	MOUSE.state = MOUSE_IDLE;
	TEST_PIN_OFF;
		
	while(1)
	{
		
		//MOUSE.battery_voltage = ADC_GET_BATTERY_VOLTAGE();
		
		STATE_Handle(); 
		UART1_COMMAND_PARSERHandler(&UART_Buffer);
		
		if(BUTTON_SEL.wasPressed && LONG_PRESS(BUTTON_SEL.time))
			{
				STATE_Selection();
				
				BUTTON_SEL.wasPressed = false;
			}
			
		if(BUTTON_OK.wasPressed && LONG_PRESS(BUTTON_OK.time))
			{
				

				BUTTON_OK.wasPressed = false;
			}	
			
		if(BUTTON_OK.wasPressed && NORMAL_PRESS(BUTTON_OK.time))
			{
				
				char buf[32];
				sprintf(buf,"\r\n%.2f", SENSOR_GET_LEFT_FRONT_DISTANCE_MM());
				UART1_Log(buf);
				
				BUTTON_OK.wasPressed = false;
			}		
			
			
	}
}
