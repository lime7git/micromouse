#include "controller.h"

sMOUSE MOUSE;

void MOVE_CONTROLLER(sMOUSE *mouse)
{
	float prev_distance_to_travel, out;
	float prev_ang_to_achieve, out_ang;
	 
	prev_distance_to_travel = mouse->distance_to_travel;
	prev_ang_to_achieve = mouse->angle_to_achieve;
	
	mouse->distance_to_travel = sqrtf(powf((mouse->new_position_x - mouse->actual_position_x),2) + powf((mouse->new_position_y - mouse->actual_position_y),2));
	mouse->angle_to_achieve = fmodf((atan2f((mouse->new_position_x - mouse->actual_position_x),(mouse->new_position_y - mouse->actual_position_y)) * RAD_TO_DEG) - mouse->actual_angle, 360.0f);

	if(mouse->angle_to_achieve < -180.0f)
	{
		mouse->angle_to_achieve += 360.0f;
	}
	else if(mouse->angle_to_achieve > 180.0f)
	{
		mouse->angle_to_achieve -= 360.0f;
	}

	out 		= 0.2f * mouse->distance_to_travel 	 	+ 0.0f 	* (mouse->distance_to_travel - prev_distance_to_travel) / TIME_STAMP;
	out_ang = 1.0f 	* mouse->angle_to_achieve + 0.0f * (mouse->angle_to_achieve - prev_ang_to_achieve) / TIME_STAMP;
	
	if(out > 600.0f)
			out = 600.0f;
	else if(out < -600.0f)
			out = -600.0f;
	
	if(out_ang > 200.0f)
			out_ang = 200.0f;
	else if(out_ang < -200.0f)
			out_ang = -200.0f;
	
	
		if(mouse->distance_to_travel > 3.0f)
		{
			mouse->front = out;
		}
		else
		{
			mouse->front = 0.0f;
		}
	
	if(mouse->angle_to_achieve  > -5.0f && mouse->angle_to_achieve  < 5.0f)
	{
		mouse->direction = out_ang;
	}
	else 
	{
		mouse->direction = out_ang;
		mouse->front = 0.0f;
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
}
void MOVE_SET_ORIENTATION(sMOUSE *mouse, float new_angle)
{
	mouse->new_angle = new_angle;
}
