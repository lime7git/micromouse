#include "ui.h"

#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "adc.h"
#include "buzzer.h"
#include "motors.h"
#include "circular_buffer.h"
#include "pid.h"
#include "controller.h"
#include "encoders.h"
#include "profiler.h"

extern sMOUSE MOUSE;
extern sMOT MOTOR_LEFT;
extern sMOT MOTOR_RIGHT;

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
				MOTOR_PID_DISABLE(&MOTOR_LEFT);
				MOTOR_PID_DISABLE(&MOTOR_RIGHT);
				MOVE_CONTROLLER_DISABLE(&MOUSE);
				
				MOT_STOP
				MOT_STOP
				MOT_STOP
				LED_Switch(LED1, OFF);
				LED_Switch(LED2, OFF);
				LED_Switch(LED3, OFF);
				LED_Switch(LED4, OFF);
				BUZZER_Volume(0);
			
			break;
			}
			case MOUSE_IDLE:
			{
				
			
			break;
			}
			case MOUSE_RUN:
			{
				volatile static int state = 0;
			
				LED_Switch(LED1, TOG);
				delay_ms(250);
			
				
				if(BUTTON_OK.wasPressed && SHORT_PRESS(BUTTON_OK.time))
				{
					static char buf[128];
					sprintf(buf,"\r\nBattery voltage = %.2fV\r\nVREFINT voltage = %.2fV\r\nProcessor temperature = %.1fC\r\n",ADC_GET_BATTERY_VOLTAGE(),ADC_GET_VREF_INTERNAL(),ADC_GET_TEMPERATURE_INTERAL());
					UART1_Log(buf);
					
					BUTTON_OK.wasPressed = false;
				}
					else if(BUTTON_OK.wasPressed && NORMAL_PRESS(BUTTON_OK.time))
					{
						if(state < 4) state++;
							else state = 0;
						
						MOT_STOP
						LED_Switch(LED2, TOG);
						
						BUTTON_OK.wasPressed = false;
					}
						else if(BUTTON_OK.wasPressed && LONG_PRESS(BUTTON_OK.time))
						{
				
						
							BUTTON_OK.wasPressed = false;
						}
				
				if(BUTTON_SEL.wasPressed && NORMAL_PRESS(BUTTON_SEL.time))
				{
					BUZZER_Buzz(32, 3, 100, 250);
					
					BUTTON_SEL.wasPressed = false;
				}
				
				switch(state)
				{
					case 0:
						MOT_STOP
					break;
					case 1:
						MOTOR_SET_SPEED(&MOTOR_LEFT, 25);
					break;
					case 2:
						MOTOR_SET_SPEED(&MOTOR_LEFT, -25);
					break;
					case 3:
						MOTOR_SET_SPEED(&MOTOR_RIGHT, 25);
					break;
					case 4:
						MOTOR_SET_SPEED(&MOTOR_RIGHT, -25);
					break;
				}
			
			break;
			}
			case MOUSE_TEST:
			{
				for(uint8_t i = 0; i < 8; i++)
				{
					if(i < 4) LED_Switch(i, ON);
						else LED_Switch(i - 4, OFF);
					
					delay_ms(250);
				}
				delay_ms(1000);
				
				BUZZER_Buzz(8, 1, 250, 500);
				BUZZER_Buzz(32, 1, 250, 500);
				BUZZER_Buzz(64, 1, 250, 500);
				
				delay_ms(1000);

				
				MOTOR_SET_SPEED(&MOTOR_LEFT, 50);
				delay_ms(1000);
					MOT_STOP
					delay_ms(500);
				MOTOR_SET_SPEED(&MOTOR_LEFT, -50);
				delay_ms(1000);
					MOT_STOP
					delay_ms(500);
				MOTOR_SET_SPEED(&MOTOR_RIGHT, 50);
				delay_ms(1000);
					MOT_STOP
					delay_ms(500);
				MOTOR_SET_SPEED(&MOTOR_RIGHT, -50);
				delay_ms(1000);
					MOT_STOP
					delay_ms(500);
				MOTOR_SET_SPEED(&MOTOR_LEFT, 90);
				MOTOR_SET_SPEED(&MOTOR_RIGHT, 90);
				delay_ms(1000);
					MOT_STOP
					delay_ms(500);
				MOTOR_SET_SPEED(&MOTOR_LEFT, -20);
				MOTOR_SET_SPEED(&MOTOR_RIGHT, -20);
				delay_ms(1000);
					MOT_STOP
					delay_ms(500);
					
				MOT_STOP
				delay_ms(1000);
				for(uint8_t i = 0; i < 100; i++)
				{
					MOTOR_SET_SPEED(&MOTOR_LEFT, i);
					MOTOR_SET_SPEED(&MOTOR_RIGHT, i);
					delay_ms(50);
				}
				
				delay_ms(1000);
				MOT_STOP
				delay_ms(1000);
				for(uint8_t i = 0; i < 100; i++)
				{
					MOTOR_SET_SPEED(&MOTOR_LEFT, -i);
					MOTOR_SET_SPEED(&MOTOR_RIGHT, -i);
					delay_ms(50);
				}
				
				delay_ms(1000);
				MOT_STOP
				delay_ms(1000);
				
				MOUSE.state = MOUSE_IDLE;
			
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
			volatile uint8_t n = number;
			
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
	
	LED_Switch(ALL,OFF);
	
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
	
	LED_Switch(ALL,OFF);
	MOUSE.state = mode;
}
