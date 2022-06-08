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

#define  WEST    1    // binary 0000 0001 
#define  SOUTH   2    // binary 0000 0010 
#define  EAST    4    // binary 0000 0100 
#define  NORTH   8    // binary 0000 1000

#define FORWARD_MAX_SPEED 180.0f
#define DIRECTION_MAX_SPEED 180.0f

typedef struct{
	
	eMouseState state;
	uint8_t face_direction;
	bool is_controller_enable;
	bool forward_control;
	bool rotation_control;
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
	
	double left_front_sensor_mm;
	double right_front_sensor_mm;
	double left_side_sensor_mm;
	double right_side_sensor_mm;
	
	uint8_t map[18];
	uint8_t current_map_index;
	
} sMOUSE;

extern sMOUSE MOUSE;

void MOVE_CONTROLLER_FORWARD(sMOUSE *mouse);
void MOVE_CONTROLLER_DIRECTION(sMOUSE *mouse);

void MOVE_CONTROLLER_ENABLE(sMOUSE *mouse);
void MOVE_CONTROLLER_DISABLE(sMOUSE *mouse);
bool MOVE_CONTROLLER_IS_ENABLE(sMOUSE *mouse);
void MOVE_SET_POSITION(sMOUSE *mouse, float new_posX, float new_posY);
void MOVE_SET_ORIENTATION(sMOUSE *mouse, float new_ang);
void MOVE_CELL_FORWARD(sMOUSE *mouse, uint8_t number_of_cells);

#endif
