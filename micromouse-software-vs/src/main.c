#include "gpio.h"
#include "clock.h"
#include "ui.h"
#include "motors.h"
#include "encoders.h"
#include "pid.h"
#include "controller.h"
#include "timer.h"

volatile float speed_left, speed_right;

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
	
	//MOUSE.state = MOUSE_INIT;
	MOUSE.state = MOUSE_IDLE;	

	speed_left = 0.0f;
	speed_right = 0.0f;

	while(1)
	{
		
		STATE_Handle(); 
		
		MOTOR_LEFT.set_rpm = speed_left;
		MOTOR_RIGHT.set_rpm = speed_right;

		MOTOR_SET_SPEED(&MOTOR_LEFT, MOTOR_LEFT.set_rpm);
		MOTOR_SET_SPEED(&MOTOR_RIGHT, MOTOR_RIGHT.set_rpm);

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
				
				
				BUTTON_OK.wasPressed = false;
			}		
	}
}
