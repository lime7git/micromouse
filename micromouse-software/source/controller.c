#include "controller.h"
#include "map.h"
#include "uart.h"

sMOUSE MOUSE;

float Fkp = 0.75f, Fkd = 0.07f;
float Rkp = 0.08f, Rkd = 0.01f;

void MOVE_CONTROLLER_FORWARD(sMOUSE *mouse)
{
	float out;
	float previous_distance_to_travel = mouse->distance_to_travel; 
	
//mouse->distance_to_travel = sqrtf(powf((mouse->new_position_x - mouse->actual_position_x),2) + powf((mouse->new_position_y - mouse->actual_position_y),2));
	
	if(MOUSE.face_direction == NORTH)// if facing NORTH
	{
		mouse->distance_to_travel = mouse->new_position_y - mouse->actual_position_y;
	}
	else if(MOUSE.face_direction == SOUTH) // if facing SOUTH
	{
		mouse->distance_to_travel = fabs(mouse->new_position_y - mouse->actual_position_y);
	}
	else if(MOUSE.face_direction == WEST) // if facing WEST
	{
		mouse->distance_to_travel = fabs(mouse->new_position_x - mouse->actual_position_x);
	}
	else if(MOUSE.face_direction == EAST) // if facing EAST
	{
		mouse->distance_to_travel = mouse->new_position_x - mouse->actual_position_x;
	}
	
	out = (Fkp * mouse->distance_to_travel) + (Fkd * (mouse->distance_to_travel - previous_distance_to_travel) / TIME_STAMP);
	
	if(out > FORWARD_MAX_SPEED)
		out = FORWARD_MAX_SPEED;
	else if(out < -FORWARD_MAX_SPEED)
		out = -FORWARD_MAX_SPEED;
	
	if(mouse->distance_to_travel > 5.0f)
	{
		mouse->forward = out;
	}
	else
	{
		mouse->forward = 0.0f;
		mouse->direction = 0.0f;
		mouse->state = MOUSE_STOP;
	}
}
void MOVE_CONTROLLER_DIRECTION(sMOUSE *mouse)
{
	float out;
	float previous_ang_to_achieve = mouse->angle_to_achieve;
	
	if(mouse->forward_control) // if running forward
	{
		Rkp = 0.08f;
//		if(mouse->right_side_sensor_mm < 140.0f && mouse->left_side_sensor_mm < 140.0f)	// if there is both walls
//		{
//			mouse->angle_to_achieve = mouse->right_side_sensor_mm - mouse->left_side_sensor_mm;
//		}
		if(mouse->left_side_sensor_mm < 140.0f)	// if there is left wall - track left
		{
			mouse->angle_to_achieve = 83.0f - mouse->left_side_sensor_mm;
		}
		else if(mouse->right_side_sensor_mm < 140.0f) // if there is right wall - track right
		{
			mouse->angle_to_achieve = mouse->right_side_sensor_mm - 83.0f;
		}
		else	// if there is no walls 
		{
			mouse->angle_to_achieve = fmodf((atan2f((mouse->new_position_x - mouse->actual_position_x),(mouse->new_position_y - mouse->actual_position_y)) * RAD_TO_DEG) - mouse->actual_angle, 360.0f);
		}	

	}		
	else // if rotate in place 
	{	
		mouse->angle_to_achieve = mouse->new_angle - mouse->actual_angle;
		Rkp = 0.75f;
	}
	
	if(mouse->angle_to_achieve < -180.0f)
	{
		mouse->angle_to_achieve += 360.0f;
	}
	else if(mouse->angle_to_achieve > 180.0f)
	{
		mouse->angle_to_achieve -= 360.0f;
	}

	out = (Rkp * mouse->angle_to_achieve) + (Rkd * (mouse->angle_to_achieve - previous_ang_to_achieve) / TIME_STAMP);
	
	if(out > DIRECTION_MAX_SPEED)
	{
		out = DIRECTION_MAX_SPEED;
	}
	else if(out < -DIRECTION_MAX_SPEED)
	{
		out = -DIRECTION_MAX_SPEED;
	}
	
	if(mouse->angle_to_achieve < -2.5f || mouse->angle_to_achieve > 2.5f)
	{
		mouse->forward *= 0.85f;
	}
	
	mouse->direction = out;
	
	if(!(mouse->forward_control) && fabs(mouse->angle_to_achieve) < 0.5f)
	{
		mouse->direction = 0.0f;
		mouse->state = MOUSE_STOP;
	}
}
void MOVE_CONTROLLER_ENABLE(sMOUSE *mouse)
{
	mouse->is_controller_enable = true;
}
void MOVE_CONTROLLER_DISABLE(sMOUSE *mouse)
{
	mouse->is_controller_enable = false;
}
bool MOVE_CONTROLLER_IS_ENABLE(sMOUSE *mouse)
{
	return mouse->is_controller_enable;
}
void MOVE_SET_POSITION(sMOUSE *mouse, float new_position_x, float new_position_y)
{
	mouse->new_position_x = new_position_x;
	mouse->new_position_y = new_position_y;
	mouse->state = MOUSE_MOVE_CONTROLLER;
	mouse->forward_control = true;
	mouse->rotation_control = true;
	mouse->distance_to_travel = 0.0f;
	mouse->angle_to_achieve = 0.0f;
}
void MOVE_SET_ORIENTATION(sMOUSE *mouse, float new_angle)
{
	mouse->new_angle = new_angle;
	mouse->state = MOUSE_MOVE_CONTROLLER;
	mouse->forward_control = false;
	mouse->rotation_control = true;
	
	if(new_angle == 0.0f) 				MOUSE.face_direction = NORTH;
	else if(new_angle == 90.0f) 	MOUSE.face_direction = EAST;
	else if(new_angle == -90.0f) 	MOUSE.face_direction = WEST;
	else if(new_angle == 180.0f) 	MOUSE.face_direction = SOUTH;
	else if(new_angle == -180.0f) MOUSE.face_direction = SOUTH;
	
	char buf[64];
  sprintf(buf, "$MAP=%d,%d,%d#", MOUSE.current_map_index, MOUSE.map[MOUSE.current_map_index], MOUSE.face_direction);
	UART1_Log(buf);
}
void MOVE_CELL_FORWARD(sMOUSE *mouse, uint8_t number_of_cells)
{
	if(MOUSE.face_direction == NORTH)		// if facing NORTH
	{
			
		
		if(mouse->actual_position_x == 0.0f && mouse->actual_position_y == 0.0f)	// only at first cell 
		{
			MAP_UPDATE(&MOUSE);
		
			char buf[64];
			sprintf(buf, "$MAP=%d,%d,%d#", MOUSE.current_map_index, MOUSE.map[MOUSE.current_map_index], MOUSE.face_direction);
			UART1_Log(buf);
			
			MOVE_SET_POSITION(mouse, mouse->actual_position_x, mouse->actual_position_y + ((number_of_cells * 180.0f) + 39.0f));
		}
		else MOVE_SET_POSITION(mouse, mouse->actual_position_x, mouse->actual_position_y + (number_of_cells * 180.0f));
		
		mouse->current_map_index += number_of_cells * 3;
	}
	else if(MOUSE.face_direction == SOUTH) // if facing SOUTH
	{
		MOVE_SET_POSITION(mouse, mouse->actual_position_x, mouse->actual_position_y - (number_of_cells * 180.0f));
		
		mouse->current_map_index -= number_of_cells * 3;
	}
	else if(MOUSE.face_direction == WEST) // if facing WEST
	{
		MOVE_SET_POSITION(mouse, mouse->actual_position_x - (number_of_cells * 180.0f), mouse->actual_position_y);
		
		mouse->current_map_index -= number_of_cells;
	}
	else if(MOUSE.face_direction == EAST) // if facing EAST
	{
		MOVE_SET_POSITION(mouse, mouse->actual_position_x + (number_of_cells * 180.0f), mouse->actual_position_y);
		
		mouse->current_map_index += number_of_cells;
	}
	
	
	
}
