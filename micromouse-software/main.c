#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "adc.h"
#include "buzzer.h"
#include "ui.h"
#include "motors.h"
#include "circular_buffer.h"

extern volatile int32_t MOTR_enc;
extern volatile int32_t MOTL_enc;

int main(void)
{
	CLOCK_Init();
	GPIO_Init();
	ADC1_DMA_Init();
	BUZZER_Init();
	MOTORS_Init();
	ENCODERS_Init();
	BUTTON_EXTI_Init();
	
	tCircular_buffer_init(&UART_Buffer, 128);
	UART1_IT_Init();
	
	STATE_Selection();
	
	while(1)
	{
		MOTR_enc = TIM5->CNT;
		MOTL_enc = TIM2->CNT;

		
		// ### HANDLE SECTION 
		STATE_Handle(); 
		UART1_COMMAND_PARSERHandler(&UART_Buffer);
		
		if(BUTTON_SEL.wasPressed && LONG_PRESS(BUTTON_SEL.time))
			{
				STATE_Selection();
				
				BUTTON_SEL.wasPressed = false;
			}
		// ###	
		
	}
}
