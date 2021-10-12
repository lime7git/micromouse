#ifndef _UI_H
#define _UI_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdbool.h"

typedef enum {
							INIT = 0, 
							CRITICAL = 1, 
							STOP = 2, 
							IDLE = 3, 
							RUN = 4,
							TEST = 5, 
							PID = 6, 
							MANUAL = 7
}eMouseState;

extern volatile eMouseState mouse_state;

void STATE_Selection(void);
void STATE_Handle(void);
void LED_Write(uint8_t number);

#endif
