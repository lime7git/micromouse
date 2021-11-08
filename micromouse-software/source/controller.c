#include "controller.h"

sMOUSE MOUSE;

	static float prev_distance_to_travel, out;
	static float prev_ang_to_achieve, out_ang;

void MOVE_CONTROLLER(sMOUSE *mouse)
{
	//static float prev_distance_to_travel, out;
	//static float prev_ang_to_achieve, out_ang;
	 
	prev_distance_to_travel = mouse->distance_to_travel;
	prev_ang_to_achieve = mouse->ang_to_achieve;
	
	mouse->distance_to_travel = sqrtf(powf((mouse->new_pos_x - mouse->pos_x),2) + powf((mouse->new_pos_y - mouse->pos_y),2));
	mouse->ang_to_achieve = fmodf((atan2f((mouse->new_pos_x - mouse->pos_x),(mouse->new_pos_y - mouse->pos_y)) * RAD_TO_DEG) - mouse->ang, 360.0f);

	if(mouse->ang_to_achieve < -180.0f)
	{
		mouse->ang_to_achieve += 360.0f;
	}
	else if(mouse->ang_to_achieve > 180.0f)
	{
		mouse->ang_to_achieve -= 360.0f;
	}

	out 		= 0.2f * mouse->distance_to_travel 	 	+ 0.0f 	* (mouse->distance_to_travel - prev_distance_to_travel) / TIME_STAMP;
	out_ang = 1.0f 	* mouse->ang_to_achieve + 0.0f * (mouse->ang_to_achieve - prev_ang_to_achieve) / TIME_STAMP;
	
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
			mouse->Front = out;
		}
		else
		{
			mouse->Front = 0.0f;
		}
	
	if(mouse->ang_to_achieve  > -5.0f && mouse->ang_to_achieve  < 5.0f)
	{
		mouse->Dir = out_ang;
	}
	else 
	{
		mouse->Dir = out_ang;
		mouse->Front = 0.0f;
	}
}
void MOVE_CONTROLLER_ENABLE(sMOUSE *mouse)
{
	mouse->controllerEnable = true;
}
void MOVE_CONTROLLER_DISABLE(sMOUSE *mouse)
{
	mouse->controllerEnable = false;
}
bool MOVE_CONTROLLER_IS_ENABLE(sMOUSE *mouse)
{
	return mouse->controllerEnable;
}
void MOVE_SET_POSITION(sMOUSE *mouse, float new_posX, float new_posY)
{
	mouse->new_pos_x = new_posX;
	mouse->new_pos_y = new_posY;
}
void MOVE_SET_ORIENTATION(sMOUSE *mouse, float new_ang)
{
	mouse->new_ang = new_ang;
}
