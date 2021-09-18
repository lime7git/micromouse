#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdio.h"

typedef struct{
	
	float Front;
	float Dir;
	
	float pos_x;
	float pos_y;
	float ang;
	float distance;

	float new_pos_x;
	float new_pos_y;
	float new_ang;
	
	float distance_to_travel;
	float ang_to_achieve;

} sMOUSE;

extern sMOUSE MOUSE;

#endif
