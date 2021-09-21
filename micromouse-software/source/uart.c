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
	USART1->BRR = (16000000 / 115200);	// baud rate 115200
	USART1->CR1 |=  USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_PCE | USART_CR1_M; // USART enable transmitter | reciever | USART enable | recieve interrupt | parity check enable | 9 bits frame
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
			tCircular_buffer_pop(&UART_Buffer, &tmp_buf);
			
			if(tmp_buf != '#') single_command[tmp_cnt] = tmp_buf;
			tmp_cnt++;
		}	while(tmp_buf != '#');
		
		COMMAND_Execute((char *)single_command);
	}
}
void COMMAND_Execute(char *command)
{
	switch(COMMAND_GET_TYPE(command))
	{
		case UNKNOWN:
		{
			char buf[64];
			sprintf(buf, "Unknown command! \t $%s# \r\n", command);
			
			UART1_Log(buf);
			
			break;
		}
		case LED:
		{
			eLEDnum led_num = (eLEDnum)(atoi(&command[4]));
			eLEDstate led_state = (eLEDstate)(atoi(&command[6]));
			
			LED_Switch(led_num, led_state);
			
			break;
		}
		case MOTOR:
		{
			
			break;
		}
		case BUZZER:
		{
			
			break;
		}
		case STATE:
		{
			if(command[6] == '?')
			{
				char buf[32];
				sprintf(buf, "Mouse state = %i \r\n", mouse_state);
				UART1_Log(buf);
			}
			else
			{
				int state = atoi(&command[6]);
				
				if((state >= 0) && (state <= 15))
				{
					mouse_state = (eMouseState)state;
				}
				else UART1_Log("Unknown state! \r\n");
			}
			
			break;
		}
	}
}
eCOMMANDS COMMAND_GET_TYPE(char *command)
{
	unsigned int counter = 0;
	char command_type[16];
	eCOMMANDS type = UNKNOWN;
	
	while(command[counter] != '=')
	{
		command_type[counter] = command[counter];
		
		counter++;
	}
	
	if(strncmp(command_type, "LED", counter) == 0) type = LED;
	else if(strncmp(command_type, "MOTOR", counter) == 0) type = MOTOR;
	else if(strncmp(command_type, "BUZZER", counter) == 0) type = BUZZER;
	else if(strncmp(command_type, "STATE", counter) == 0) type = STATE;
	
	return type;
}
void UART1_SendChar(char c)
{
	while(!(USART1->SR & USART_SR_TXE)) {}
		
	USART1->DR = c;
}
void UART1_Log(const char *message)
{
	while(*message)
	{
		UART1_SendChar(*message++);
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

