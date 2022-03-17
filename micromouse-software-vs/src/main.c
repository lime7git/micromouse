#include "gpio.h"
#include "clock.h"
#include "ui.h"
#include "motors.h"
#include "encoders.h"
#include "pid.h"
#include "controller.h"
#include "timer.h"

int main(void)
{
	
	CLOCK_Init();
	GPIO_Init();
	MOTORS_Init();
	ENCODERS_Init();
	BUTTON_EXTI_Init();
	TIM7_1KHz_INTERRUPT_Init();
	
	
	MOTOR_PID_INIT(&MOTOR_LEFT, LEFT_MOTOR, 1.0f, 0.7f, 0.001f);
	MOTOR_PID_INIT(&MOTOR_RIGHT, RIGHT_MOTOR, 1.1f, 0.65f, 0.001f);
	
	MOTOR_SPEED_PROFILER_INIT(&MOTOR_LEFT, 500.0f, 1.0f);
	MOTOR_SPEED_PROFILER_INIT(&MOTOR_RIGHT, 500.0f, 1.0f);
	
	MOTOR_PID_DISABLE(&MOTOR_LEFT);
	MOTOR_PID_DISABLE(&MOTOR_RIGHT);
	MOTOR_SPEED_PROFILER_DISABLE(&MOTOR_LEFT);
	MOTOR_SPEED_PROFILER_DISABLE(&MOTOR_RIGHT);
	MOVE_CONTROLLER_DISABLE(&MOUSE);
	
	MOUSE.state = MOUSE_INIT;
		
	while(1)
	{
		
		STATE_Handle(); 
		
		
		if(BUTTON_SEL.wasPressed && LONG_PRESS(BUTTON_SEL.time))
			{
				STATE_Selection();
				
				BUTTON_SEL.wasPressed = false;
			}
			
		if(BUTTON_OK.wasPressed && LONG_PRESS(BUTTON_OK.time))
			{
				LED_Switch(LED_ALL, OFF);
					for(uint8_t i = 0; i < 4; i++)
					{
						LED_Switch(i, ON);
						delay_ms(400);
					}
				LED_Switch(LED_ALL, OFF);
				
				MOUSE.actual_position_x = 0.0;
				MOUSE.actual_position_y = 0.0;
				MOUSE.actual_angle = 0.0;
				
				MOUSE.new_position_x = 0.0;
				MOUSE.new_position_y = 1000.0;
				
				MOUSE.state = MOUSE_MOVE_CONTROLLER;

				BUTTON_OK.wasPressed = false;
			}	
			
		if(BUTTON_OK.wasPressed && NORMAL_PRESS(BUTTON_OK.time))
			{
				IR_R_ON;
				delay_ms(10000);
				IR_R_OFF;
				
				BUTTON_OK.wasPressed = false;
			}		
	}
}
