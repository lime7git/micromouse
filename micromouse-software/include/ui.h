#ifndef _UI_H
#define _UI_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdbool.h"

typedef enum {INIT = 0, IDLE = 1, RUN = 2, STOP = 3, TEST = 4, TEST2 = 5, CRITICAL = 15}eMouseState;

extern volatile eMouseState mouse_state;

void STATE_Selection(void);
void STATE_Handle(void);
void LED_Write(uint8_t number);

#endif
