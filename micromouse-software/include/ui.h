#ifndef _UI_H
#define _UI_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdbool.h"

typedef enum {
	
	MOUSE_INIT = 0, 
	MOUSE_CRITICAL = 1, 
	MOUSE_STOP = 2, 
	MOUSE_IDLE = 3, 
	MOUSE_RUN = 4,
	MOUSE_TEST = 5, 
	MOUSE_PID = 6, 
	MOUSE_MANUAL = 7,
	MOUSE_PROFILER = 8,
	MOUSE_MOVE_CONTROLLER = 9
	
}	eMouseState;

void STATE_Handle(void);

/**
* @name : LED_DISPLAY_DIGIT_IN_BINARY 
* ----------------------------- *
* @description : This functions display a digit value on a 4 LED diodes
* @params : A unsigned int value in range (0-16)
* @return : none
*/
void LED_DISPLAY_DIGIT_IN_BINARY(uint8_t number);


void STATE_Selection(void);

#endif
