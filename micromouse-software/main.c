#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "adc.h"
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
	MOTORS_Init();
	ENCODERS_Init();
	BUTTON_EXTI_Init();
	UART1_IT_Init();
	TIM7_1KHz_INTERRUPT_Init();
	
	
	MOTOR_PID_INIT(&MOTOR_LEFT,  LEFT_MOTOR, 1.0f, 0.7f, 0.001f);
	MOTOR_PID_INIT(&MOTOR_RIGHT, RIGHT_MOTOR, 1.1f, 0.65f, 0.001f);
	
	MOTOR_SPEED_PROFILER_INIT(&MOTOR_LEFT, 500.0f, 1.0f);
	MOTOR_SPEED_PROFILER_INIT(&MOTOR_RIGHT, 500.0f, 1.0f);
	
	MOTOR_PID_DISABLE(&MOTOR_LEFT);
	MOTOR_PID_DISABLE(&MOTOR_RIGHT);
	MOTOR_SPEED_PROFILER_DISABLE(&MOTOR_LEFT);
	MOTOR_SPEED_PROFILER_DISABLE(&MOTOR_RIGHT);
	MOVE_CONTROLLER_DISABLE(&MOUSE);
	
	MOUSE.state = MOUSE_IDLE;
	
	TEST_PIN2_OFF;
	TEST_PIN_OFF;
		
	while(1)
	{
		TEST_PIN2_ON;
		
		MOUSE.battery_voltage = ADC_GET_BATTERY_VOLTAGE_MEAN();
		MOUSE.left_front_sensor_mm = SENSOR_GET_LEFT_FRONT_DISTANCE(MM);
		MOUSE.right_side_sensor_mm = SENSOR_GET_RIGHT_SIDE_DISTANCE(MM);
		MOUSE.left_side_sensor_mm = SENSOR_GET_LEFT_SIDE_DISTANCE(MM);
		MOUSE.right_front_sensor_mm = SENSOR_GET_RIGHT_FRONT_DISTANCE(MM);
		
		TEST_PIN2_OFF;
		
		
		STATE_Handle(); 
		UART1_COMMAND_PARSERHandler(&UART_Buffer);
		
		if(BUTTON_SEL.wasPressed && LONG_PRESS(BUTTON_SEL.time))
			{
				STATE_Selection();
				
				BUTTON_SEL.wasPressed = false;
			}

	}
}
