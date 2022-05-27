#include "controller.h"

sMOUSE MOUSE;

float Fkp = 2.5f, Fkd = 0.075f;
float Rkp = 1.5f, Rkd = 0.9f;

void MOVE_CONTROLLER_FORWARD(sMOUSE *mouse)
{
	float previous_distance_to_travel, out;
	
	previous_distance_to_travel = mouse->distance_to_travel;
	
//	mouse->distance_to_travel = sqrtf(powf((mouse->new_position_x - mouse->actual_position_x),2) + powf((mouse->new_position_y - mouse->actual_position_y),2));
	
	
	
		if(mouse->actual_angle > -10.0f && mouse->actual_angle < 10.0f)		// if facing NORTH
	{
		mouse->distance_to_travel = mouse->new_position_y - mouse->actual_position_y;
	}
	else if(mouse->actual_angle > -170.0f && mouse->actual_angle < 170.0f) // if facing SOUTH
	{
		mouse->distance_to_travel = mouse->new_position_y - mouse->actual_position_y;
	}
	else if(mouse->actual_angle > -80.0f && mouse->actual_angle < -110.0f) // if facing WEST
	{
		mouse->distance_to_travel = mouse->new_position_x - mouse->actual_position_x;
	}
	else if(mouse->actual_angle > 80.0f && mouse->actual_angle < 110.0f) // if facing EAST
	{
		mouse->distance_to_travel = mouse->new_position_x - mouse->actual_position_x;
	}
	
	out = (Fkp * mouse->distance_to_travel) + (Fkd * (mouse->distance_to_travel - previous_distance_to_travel) / TIME_STAMP);
	
	if(out > 360.0f)
		out = 360.0f;
	else if(out < -360.0f)
		out = -360.0f;
	
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
	float previous_ang_to_achieve, out;
	 
	previous_ang_to_achieve = mouse->angle_to_achieve;
	
	if(mouse->forward_control)
	{
		
		//		if(mouse->right_side_sensor_mm < 140.0f && mouse->left_side_sensor_mm < 140.0f)
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
	else mouse->angle_to_achieve = mouse->new_angle - mouse->actual_angle;
	

	
	
	if(mouse->angle_to_achieve < -180.0f)
	{
		mouse->angle_to_achieve += 360.0f;
	}
	else if(mouse->angle_to_achieve > 180.0f)
	{
		mouse->angle_to_achieve -= 360.0f;
	}

	out = (Rkp * mouse->angle_to_achieve) + (Rkd * (mouse->angle_to_achieve - previous_ang_to_achieve) / TIME_STAMP);
	
	if(out > 360.0f)
			out = 360.0f;
	else if(out < -360.0f)
			out = -360.0f;

	if(mouse->angle_to_achieve < -2.5f || mouse->angle_to_achieve > 2.5f)
	{
		mouse->forward *= 0.05f;
	}
	
	
	mouse->direction = out;
	
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
}
void MOVE_SET_ORIENTATION(sMOUSE *mouse, float new_angle)
{
	mouse->new_angle = new_angle;
	mouse->state = MOUSE_MOVE_CONTROLLER;
	mouse->forward_control = false;
	mouse->rotation_control = true;
}
void MOVE_ONE_CELL_FORWARD(sMOUSE *mouse)
{
	if(mouse->actual_angle > -10.0f && mouse->actual_angle < 10.0f)		// if facing NORTH
	{
		MOVE_SET_POSITION(mouse, mouse->actual_position_x, mouse->actual_position_y + 168.0f);
	}
	else if(mouse->actual_angle > -170.0f && mouse->actual_angle < 170.0f) // if facing SOUTH
	{
		MOVE_SET_POSITION(mouse, mouse->actual_position_x, mouse->actual_position_y - 168.0f);
	}
	else if(mouse->actual_angle > -80.0f && mouse->actual_angle < -110.0f) // if facing WEST
	{
		MOVE_SET_POSITION(mouse, mouse->actual_position_x - 168.0f, mouse->actual_position_y);
	}
	else if(mouse->actual_angle > 80.0f && mouse->actual_angle < 110.0f) // if facing EAST
	{
		MOVE_SET_POSITION(mouse, mouse->actual_position_x + 168.0f, mouse->actual_position_y);
	}
	
	
}
