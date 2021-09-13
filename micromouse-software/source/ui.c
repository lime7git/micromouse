#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "adc.h"
#include "buzzer.h"
#include "ui.h"
#include "motors.h"
#include "circular_buffer.h"

volatile eMouseState mouse_state;

void STATE_Selection(void)
{
	volatile static uint8_t mode = 1;
	
	mode = mouse_state;
	
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
		
		LED_Write(mode);
	}
	
	LED_Switch(ALL,OFF);
	mouse_state = mode;
}

void STATE_Handle(void)
{
	if(mouse_state == RUN)
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
					MOTR_SET_SPEED(25);
				break;
				case 2:
					MOTR_SET_SPEED(-25);
				break;
				case 3:
					MOTL_SET_SPEED(25);
				break;
				case 4:
					MOTL_SET_SPEED(-25);
				break;
				
			}
			
		}
		
		else if(mouse_state == STOP)
		{
			MOT_STOP
			MOT_STOP
			MOT_STOP
			LED_Switch(LED1, OFF);
			LED_Switch(LED2, OFF);
			LED_Switch(LED3, OFF);
			LED_Switch(LED4, OFF);
			BUZZER_Volume(0);
		}
		
		else if(mouse_state == CRITICAL)
		{
			BATTERY_CRITICAL_PROCEDURE();
			delay_ms(5000);
		}
		
		else if(mouse_state == TEST)
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
			
			MOTR_SET_SPEED(50);
			delay_ms(1000);
				MOT_STOP
				delay_ms(500);
			MOTR_SET_SPEED(-50);
			delay_ms(1000);
				MOT_STOP
				delay_ms(500);
			MOTL_SET_SPEED(50);
			delay_ms(1000);
				MOT_STOP
				delay_ms(500);
			MOTL_SET_SPEED(-50);
			delay_ms(1000);
				MOT_STOP
				delay_ms(500);
			MOTR_SET_SPEED(100);
			MOTL_SET_SPEED(100);
			delay_ms(1000);
				MOT_STOP
				delay_ms(500);
			MOTR_SET_SPEED(-50);
			MOTL_SET_SPEED(-50);
			delay_ms(1000);
				MOT_STOP
				delay_ms(500);
				
			MOT_STOP
			delay_ms(1000);
			for(uint8_t i = 0; i < 100; i++)
			{
				MOTR_SET_SPEED(i);
				MOTL_SET_SPEED(i);
				delay_ms(50);
			}
			
			delay_ms(1000);
			MOT_STOP
			delay_ms(1000);
			for(uint8_t i = 0; i < 100; i++)
			{
				MOTR_SET_SPEED(-i);
				MOTL_SET_SPEED(-i);
				delay_ms(50);
			}
			
			delay_ms(1000);
			MOT_STOP
			delay_ms(1000);
			
			mouse_state = IDLE;
		}
		else if(mouse_state == TEST2)
		{
			delay_ms(2500);
			
			MOTR_SET_SPEED(90);
			MOTL_SET_SPEED(-90);
			
			delay_ms(3000);
			MOT_STOP
			delay_ms(1000);
			
			mouse_state = IDLE;
		}
}

void LED_Write(uint8_t number)
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
