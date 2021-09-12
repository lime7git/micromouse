#ifndef _UART_H
#define _UART_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "gpio.h"
#include "circular_buffer.h"
#include "string.h"

extern tCircular_buffer UART_Buffer;

void UART1_IT_Init(void);
void UART1_COMMAND_PARSERHandler(tCircular_buffer *hBuffer);
void COMMAND_Execute(char *command);
void USART1_IRQHandler(void);

#endif
