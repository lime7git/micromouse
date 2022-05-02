#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "math.h"
#include "ui.h"
#include "pid.h"

#define RAD_TO_DEG 57.295779513f
#define TIME_STAMP 0.01f

typedef struct{
	
	eMouseState state;
	bool is_controller_enable;
	double battery_voltage;
	
	float forward;
	float direction;
	
	float actual_position_x;
	float actual_position_y;
	float actual_angle;

	float new_position_x;
	float new_position_y;
	float new_angle;
	
	float distance_to_travel;
	float angle_to_achieve;
	
} sMOUSE;

extern sMOUSE MOUSE;

void MOVE_CONTROLLER_FORWARD(sMOUSE *mouse);
void MOVE_CONTROLLER_DIRECTION(sMOUSE *mouse);

void MOVE_CONTROLLER_ENABLE(sMOUSE *mouse);
void MOVE_CONTROLLER_DISABLE(sMOUSE *mouse);
bool MOVE_CONTROLLER_IS_ENABLE(sMOUSE *mouse);
void MOVE_SET_POSITION(sMOUSE *mouse, float new_posX, float new_posY);
void MOVE_SET_ORIENTATION(sMOUSE *mouse, float new_ang);

void MOTOR_SPEED_PROFILER_INIT(sMOT *motor, float max_velocity, float acceleration);
void MOTOR_SPEED_PROFILER(sMOUSE *mouse, sMOT *motor);

#endif
