#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "math.h"
#include "ui.h"

#define RAD_TO_DEG 57.295779513f
#define TIME_STAMP 0.01f

#pragma pack(push)
#pragma pack(1) 
typedef struct{
	
	eMouseState state;
	bool controllerEnable;
	bool is_profiler_translation_enable;
	bool is_profiler_rotation_enable;
	double batteryVoltage;
	
	float Front;
	float Dir;
	float velocity;
	float trans;
	
	float pos_x;
	float pos_y;
	float ang;
	float distance;
	float prev_distance;

	float new_pos_x;
	float new_pos_y;
	float new_ang;
	
	float distance_to_travel;
	float ang_to_achieve;
	
} sMOUSE;
#pragma pack(pop)

extern sMOUSE MOUSE;

void MOVE_CONTROLLER(sMOUSE *mouse);
void MOVE_CONTROLLER_ENABLE(sMOUSE *mouse);
void MOVE_CONTROLLER_DISABLE(sMOUSE *mouse);
bool MOVE_CONTROLLER_IS_ENABLE(sMOUSE *mouse);
void MOVE_SET_POSITION(sMOUSE *mouse, float new_posX, float new_posY);
void MOVE_SET_ORIENTATION(sMOUSE *mouse, float new_ang);

#endif
