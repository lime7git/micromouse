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
	
	MOTOR_PID_INIT(&MOTOR_LEFT,  LEFT_MOTOR, 0.01f, 0.7f, 0.0001f);
	MOTOR_PID_INIT(&MOTOR_RIGHT, RIGHT_MOTOR, 0.01f, 0.65f, 0.0001f);
	
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
			
		if(BUTTON_OK.wasPressed && SHORT_PRESS(BUTTON_OK.time))
		{
				delay_ms(500);
				MOVE_SET_POSITION(&MOUSE, MOUSE.actual_position_x, MOUSE.actual_position_y + 504.0f);
				
				BUTTON_OK.wasPressed = false;
			}	
			
		if(BUTTON_OK.wasPressed && LONG_PRESS(BUTTON_OK.time))
			{
				delay_ms(500);
				MOTOR_PID_ENABLE(&MOTOR_LEFT);
				MOTOR_PID_ENABLE(&MOTOR_RIGHT);
				
				for(int i = 0; i < 150; i+= 20)
				{
					MOUSE.forward = i;
					delay_ms(2500);
				}
				
				
				for(int i = 150; i > 0; i-= 20)
				{
					MOUSE.forward = i;
					delay_ms(2500);
				}
				BUTTON_OK.wasPressed = false;
			}		
			
		if(BUTTON_OK.wasPressed && NORMAL_PRESS(BUTTON_OK.time))
		{
			delay_ms(500);
			MOUSE.state = MOUSE_MANUAL;
			TIM5->CNT = 0;
			TIM2->CNT = 0;
			MOUSE.actual_angle = 0.0f;
			MOUSE.actual_position_x = 0.0f;
			MOUSE.actual_position_y = 0.0f;
			MOTOR_SET_SPEED(&MOTOR_LEFT, 14.5f);
			MOTOR_SET_SPEED(&MOTOR_RIGHT, -14.5f);
			
			while(MOUSE.actual_angle <= 72.5f)
			{
				CALCULATE_ACTUAL_POSITION(&MOUSE, &MOTOR_LEFT, &MOTOR_RIGHT);
			}
			MOT_STOP;
			MOUSE.state = MOUSE_STOP;
			
			
				
			BUTTON_OK.wasPressed = false;
		}	

	}
}
