#include "ui.h"

#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "adc.h"
#include "motors.h"
#include "circular_buffer.h"
#include "pid.h"
#include "controller.h"
#include "encoders.h"
#include "map.h"

extern sMOUSE MOUSE;
extern sMOT MOTOR_LEFT;
extern sMOT MOTOR_RIGHT;

volatile static uint32_t previous_led_system_tick = 0;

void STATE_Handle(void)
{	
		switch (MOUSE.state)
		{
			case MOUSE_INIT:
			{
				LED_Switch(LED2, TOG);
				LED_Switch(LED3, TOG);
				delay_ms(500);
				LED_Switch(LED1, TOG);
				LED_Switch(LED4, TOG);
			
			break;
			}
			case MOUSE_CRITICAL:
			{
				BATTERY_CRITICAL_PROCEDURE();
			break;
			}
			case MOUSE_STOP:
			{
				MOTL_SET_PWM(0);
				MOTR_SET_PWM(0);
				MOT_STOP
				
				MOTOR_PID_DISABLE(&MOTOR_LEFT);
				MOTOR_PID_DISABLE(&MOTOR_RIGHT);
				MOVE_CONTROLLER_DISABLE(&MOUSE);
				
				LED_Switch(LED1, OFF);
				LED_Switch(LED2, OFF);
				LED_Switch(LED3, OFF);
				
				if(MOUSE.current_map_index != 0 && ((MOUSE.map[MOUSE.current_map_index] & VISITED) == 0)) // if mouse isnt in initial cell and current cell is not visited yet
				{
					MAP_UPDATE(&MOUSE);
					
					char buf[64];
					sprintf(buf, "$MAP=%d,%d,%d#", MOUSE.current_map_index, MOUSE.map[MOUSE.current_map_index], MOUSE.face_direction);
					UART1_Log(buf);
				}
				
				IR_LEFT_FRONT_OFF;
				IR_RIGHT_FRONT_OFF;
				IR_LEFT_SIDE_OFF;
				IR_RIGHT_SIDE_OFF;
			break;
			}
			case MOUSE_IDLE:
			{
				
			
			break;
			}
			case MOUSE_RUN:
			{
			
			
			break;
			}
			case MOUSE_TEST:
			{
				
			
			break;
			}
			case MOUSE_PID:
			{
				MOVE_CONTROLLER_DISABLE(&MOUSE);
				MOTOR_PID_ENABLE(&MOTOR_LEFT);
				MOTOR_PID_ENABLE(&MOTOR_RIGHT);
			break;
			}
			case MOUSE_MANUAL:
			{
				MOTOR_PID_DISABLE(&MOTOR_LEFT);
				MOTOR_PID_DISABLE(&MOTOR_RIGHT);
				MOVE_CONTROLLER_DISABLE(&MOUSE);
				MOTOR_SET_SPEED(&MOTOR_LEFT, MOTOR_LEFT.set_rpm);
				MOTOR_SET_SPEED(&MOTOR_RIGHT, MOTOR_RIGHT.set_rpm);
				
			break;
			}
			case MOUSE_MOVE_CONTROLLER:
			{
				MOTOR_PID_ENABLE(&MOTOR_LEFT);
				MOTOR_PID_ENABLE(&MOTOR_RIGHT);
				MOVE_CONTROLLER_ENABLE(&MOUSE);
				
			break;
			}
		}
}

void LED_DISPLAY_DIGIT_IN_BINARY(uint8_t number)
{
		if(number <= 15)
		{
			uint8_t n = number;
			
			for(uint8_t i = 0; i < 4; i++)
			{
				if(n % 2 == 1) LED_Switch(i, ON);
				else LED_Switch(i, OFF);
				
				n = n/2;
			}
		}
}

void STATE_Selection(void)
{
	volatile static uint8_t mode = 1;
	
	mode = MOUSE.state;
	
	LED_Switch(LED_ALL,OFF);
	
	for(uint8_t i = 0; i < 4; i++) 
	{
		LED_Switch(i, ON); 
		delay_ms(100);
	}
	for(uint8_t i = 0; i < 4; i++) 
	{
		LED_Switch(i, OFF); 
		delay_ms(100);
	}
	
	while(1)
	{
		if(BUTTON_OK.wasPressed && LONG_PRESS(BUTTON_OK.time))
		{
			BUTTON_OK.wasPressed = false;
			
			break;
		}
		
		if(BUTTON_SEL.wasPressed && SHORT_PRESS(BUTTON_SEL.time))
		{
			if(mode < 15) mode++;
			else mode = 0;
			
			BUTTON_SEL.wasPressed = false;
		}
		
		LED_DISPLAY_DIGIT_IN_BINARY(mode);
	}
	
	LED_Switch(LED_ALL,OFF);
	MOUSE.state = mode;
}
void LED_SYSTEM_UPDATE(void)
{
		if(GET_SYSTEM_TICK() - previous_led_system_tick >= LED_SYSTEM_DELAY)
		{
			LED_Switch(LED4, TOG);
			previous_led_system_tick = GET_SYSTEM_TICK();
		}
}