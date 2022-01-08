#include "controller.h"

sMOUSE MOUSE;

void MOVE_CONTROLLER_FORWARD(sMOUSE *mouse)
{
	float previous_distance_to_travel, out;
	
	previous_distance_to_travel = mouse->distance_to_travel;
	
	mouse->distance_to_travel = sqrtf(powf((mouse->new_position_x - mouse->actual_position_x),2) + powf((mouse->new_position_y - mouse->actual_position_y),2));
	
	out = 0.75f * mouse->distance_to_travel + 0.0f * (mouse->distance_to_travel - previous_distance_to_travel) / TIME_STAMP;
	
	if(out > 650.0f)
		out = 650.0f;
	else if(out < -650.0f)
		out = -650.0f;
	
	if(mouse->distance_to_travel > 3.0f)
	{
		mouse->forward = out;
	}
	else
	{
		mouse->forward = 0.0f;
		
		mouse->state = MOUSE_STOP;
	}
}
void MOVE_CONTROLLER_DIRECTION(sMOUSE *mouse)
{
	float previous_ang_to_achieve, out;
	 
	previous_ang_to_achieve = mouse->angle_to_achieve;
	
	mouse->angle_to_achieve = fmodf((atan2f((mouse->new_position_x - mouse->actual_position_x),(mouse->new_position_y - mouse->actual_position_y)) * RAD_TO_DEG) - mouse->actual_angle, 360.0f);

	if(mouse->angle_to_achieve < -180.0f)
	{
		mouse->angle_to_achieve += 360.0f;
	}
	else if(mouse->angle_to_achieve > 180.0f)
	{
		mouse->angle_to_achieve -= 360.0f;
	}

	out = 1.0f 	* mouse->angle_to_achieve + 0.0f * (mouse->angle_to_achieve - previous_ang_to_achieve) / TIME_STAMP;
	
	if(out > 650.0f)
			out = 650.0f;
	else if(out < -650.0f)
			out = -650.0f;

	if(mouse->angle_to_achieve  > -3.0f && mouse->angle_to_achieve  < 3.0f)
	{
		mouse->direction = out;
	}
	else 
	{
		mouse->direction = out;
		mouse->forward *= 0.75f;
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
void MOTOR_SPEED_PROFILER_INIT(sMOT *motor, float max_velocity, float acceleration)
{
	motor->profiler_velocity = 0.0f;
	motor->max_velocity = max_velocity;
	motor->acceleration = acceleration;
}
void MOTOR_SPEED_PROFILER(sMOUSE *mouse, sMOT *motor)
{
	switch(motor->motorSide)
	{
		case LEFT_MOTOR:
		{
			if(motor->profiler_velocity < (mouse->forward + mouse->direction)) 
			{
				motor->profiler_velocity += motor->acceleration;
			}
			else if(motor->profiler_velocity > (mouse->forward + mouse->direction))
			{
				motor->profiler_velocity -= motor->acceleration;
			}
			
			if(motor->profiler_velocity > motor->max_velocity)
				motor->profiler_velocity = motor->max_velocity;
			else if(motor->profiler_velocity < -(motor->max_velocity))
				motor->profiler_velocity = -(motor->max_velocity);
			
			motor->set_rpm = motor->profiler_velocity;
			
			break;
		}
			
		case RIGHT_MOTOR:
		{
			if(motor->profiler_velocity < (mouse->forward - mouse->direction))
			{
				motor->profiler_velocity += motor->acceleration;
			}
			else if(motor->profiler_velocity > (mouse->forward - mouse->direction))
			{
				motor->profiler_velocity -= motor->acceleration;
			}
			
			if(motor->profiler_velocity > motor->max_velocity)
				motor->profiler_velocity = motor->max_velocity;
			else if(motor->profiler_velocity < -(motor->max_velocity))
				motor->profiler_velocity = -(motor->max_velocity);
			
			motor->set_rpm = motor->profiler_velocity;
			
			break;
		}
	}	
}
