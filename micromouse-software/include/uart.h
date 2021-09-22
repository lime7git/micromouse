#ifndef _UART_H
#define _UART_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "gpio.h"
#include "circular_buffer.h"
#include "string.h"
#include "ui.h"
#include "motors.h"
#include "stdlib.h"
#include "adc.h"

#define PARAM_BUFFER_ROWS 4
#define PARAM_BUFFER_COLS 32

#define STATE_HELP  "\r\nINIT = 0\r\nCRITICAL = 1\r\nSTOP = 2\r\nIDLE = 3\r\nRUN = 4\r\nTEST1 = 5\r\nTEST2 = 6\r\nTEST3 = 7\r\n"

typedef enum{
	UNKNOWN,
	LED,
	MOTOR,
	BUZZER,
	STATE,
	BATTERY
} eCOMMANDS;

extern tCircular_buffer UART_Buffer;

void UART1_IT_Init(void);
void UART1_COMMAND_PARSERHandler(tCircular_buffer *hBuffer);
void COMMAND_Execute(char *command);
eCOMMANDS COMMAND_GET_TYPE(char *command);
unsigned int GET_COMMAND_PARAMS(char *command, char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS]);
void UART1_SendChar(char c);
void UART1_Log(const char *message);
void USART1_IRQHandler(void);

#endif
