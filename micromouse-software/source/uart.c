#include "uart.h"

tCircular_buffer UART_Buffer;

void UART1_IT_Init(void)
{
	// clock enable for A port, USART1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	// PA9, PA10 as an alternate function for USART1_TX and USART1_RX
	GPIOA->MODER &= ~GPIO_MODER_MODE9 & ~GPIO_MODER_MODE10;
	GPIOA->MODER |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1;
	
	GPIOA->AFR[1] |= 0x00000770;
	
	// USART1 setup
	USART1->BRR = 16000000 / 115200;	// baud rate 115200
	USART1->CR1 |=  USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE; // USART enable transmitter | reciever | USART enable | recieve interrupt
	NVIC_EnableIRQ(USART1_IRQn);
}

void UART1_COMMAND_PARSERHandler(tCircular_buffer *hBuffer)
{
	if(hBuffer->commands_cnt > 0)
	{
		uint8_t tmp_buf;
		uint8_t single_command[32] = {NULL};
		uint8_t tmp_cnt = 0;
		
		do
		{
			tCircular_buffer_pop(&UART_Buffer, &tmp_buf);
			
		}	while(tmp_buf != '$');
		
		tmp_cnt = 0;
		
		do
		{
			tCircular_buffer_pop(&UART_Buffer, &single_command[tmp_cnt]);
			
		}	while(single_command[tmp_cnt++] != '#');
		
		COMMAND_Execute((char *)single_command);
	}
}
void COMMAND_Execute(char *command)
{
	if(strcmp(command, "LED1ON#") == 0)
	{
		LED_Switch(LED1, ON);
	}
	else if(strcmp(command, "LED1OFF#") == 0)
	{
		LED_Switch(LED1, OFF);
	}
	else if(strcmp(command, "LED2ON#") == 0)
	{
		LED_Switch(LED2, ON);
	}
	else if(strcmp(command, "LED2OFF#") == 0)
	{
		LED_Switch(LED2, OFF);
	}
	else if(strcmp(command, "LED3ON#") == 0)
	{
		LED_Switch(LED3, ON);
	}
	else if(strcmp(command, "LED3OFF#") == 0)
	{
		LED_Switch(LED3, OFF);
	}	
	else if(strcmp(command, "LED4ON#") == 0)
	{
		LED_Switch(LED4, ON);
	}
	else if(strcmp(command, "LED4OFF#") == 0)
	{
		LED_Switch(LED4, OFF);
	}		
}
void USART1_IRQHandler(void)
{
	if ( (USART1->SR & USART_SR_RXNE ) != RESET)
	{
		volatile uint8_t data = (uint8_t)USART1->DR;
		
		tCircular_buffer_push(&UART_Buffer, data);
	}
}

