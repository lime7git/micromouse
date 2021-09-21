#ifndef _UART_H
#define _UART_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "gpio.h"
#include "circular_buffer.h"
#include "string.h"
#include "ui.h"

typedef enum{
	UNKNOWN,
	LED,
	MOTOR,
	BUZZER,
	STATE
} eCOMMANDS;

extern tCircular_buffer UART_Buffer;

void UART1_IT_Init(void);
void UART1_COMMAND_PARSERHandler(tCircular_buffer *hBuffer);
void COMMAND_Execute(char *command);
eCOMMANDS COMMAND_GET_TYPE(char *command);
void UART1_SendChar(char c);
void UART1_Log(const char *message);
void USART1_IRQHandler(void);

#endif
