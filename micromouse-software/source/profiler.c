#include "profiler.h"
#include "motors.h"

void PROFILER_TRANSLATION_CONTROLLER(sProfiler *profiler)
{
	
		if(profiler->state == PROFILER_RUN) 
    { 
				/* check whether braking should already begin */
        if( (((profiler->current_velocity - profiler->next_velocity) * (profiler->current_velocity + profiler->next_velocity)) / (2.0f * profiler->acceleration)) >= (profiler->distance_to_travel - profiler->remaining_distance) )
        { 
            profiler->state = PROFILER_DECELERATION; 						
            profiler->max_velocity = profiler->next_velocity; 
        } 
    } 
    
		/* check whether mouse have travelled all distance or it is very close to the target */
    if(profiler->remaining_distance >= profiler->distance_to_travel || (profiler->state == PROFILER_DECELERATION && profiler->current_velocity == 0.0f)) 
    { 
        profiler->state = PROFILER_FINISH; 
        profiler->max_velocity = profiler->next_velocity; 
    } 
    
    if(profiler->current_velocity < profiler->max_velocity) 
    { 
        profiler->current_velocity += profiler->acceleration; 
        if(profiler->current_velocity > profiler->max_velocity) 
            profiler->current_velocity = profiler->max_velocity; 
    } 
    
    if(profiler->current_velocity > profiler->max_velocity) 
    { 
        profiler->current_velocity -= profiler->acceleration; 
        if(profiler->current_velocity < profiler->max_velocity) 
            profiler->current_velocity = profiler->max_velocity; 
    } 
    
		profiler->remaining_distance += profiler->current_velocity;
	
}

void PROFILER_ROTATION_CONTROLLER(sProfiler *profiler)
{
	
		if(profiler->state == PROFILER_RUN) 
		{ 
				/* check whether braking should already begin */
				if( ((profiler->current_velocity * profiler->current_velocity) / (2.0f * profiler->acceleration)) >= (fabsf(profiler->distance_to_travel) - fabsf(profiler->distance_to_travel)) )
				{ 
						profiler->state = PROFILER_DECELERATION; 						
						profiler->max_velocity = profiler->next_velocity; 
				} 
		} 
		
		if((profiler->state == PROFILER_DECELERATION) && (profiler->current_velocity == 0.0f)) 
    { 
        profiler->state = PROFILER_FINISH; 
        profiler->max_velocity = profiler->next_velocity; 
    } 
		
		if(profiler->current_velocity < profiler->max_velocity) 
    { 
        profiler->current_velocity += profiler->acceleration; 
        if(profiler->current_velocity > profiler->max_velocity) 
            profiler->current_velocity = profiler->max_velocity; 
    } 
    
    if(profiler->current_velocity > profiler->max_velocity) 
    { 
        profiler->current_velocity -= profiler->acceleration; 
        if(profiler->current_velocity < profiler->max_velocity) 
            profiler->current_velocity = profiler->max_velocity; 
    } 
	
		if(profiler->distance_to_travel > 0)
		{
			profiler->remaining_distance += profiler->current_velocity;
		}
		else
		{
			profiler->remaining_distance -= profiler->current_velocity;
		}
		
}

void PROFILER_PD_CONTROLLER(sPDController *controller, sMOUSE *mouse)
{
	
    controller->current_translation_e = controller->translation->remaining_distance - mouse->trans; 
		controller->current_rotation_e = controller->rotation->remaining_distance - mouse->ang;
		
		/* 
		*	Left motor pwm = translation + rotation
		* Right motor pwm = translation - rotation
		*/
	
		controller->out_left = (1.0f * controller->current_translation_e + 0.05f * (controller->current_translation_e - controller->previous_translation_e))
														+ (1.0f * controller->current_rotation_e + 0.05f * (controller->current_rotation_e - controller->previous_rotation_e));
		
		controller->out_right = (1.0f * controller->current_translation_e + 0.05f * (controller->current_translation_e - controller->previous_translation_e))
														- (1.0f * controller->current_rotation_e + 0.05f * (controller->current_rotation_e - controller->previous_rotation_e));
	
		controller->previous_translation_e = controller->current_translation_e;
		controller->previous_rotation_e = controller->current_rotation_e;
 
		if(controller->out_left > 100.0f) 	
			controller->out_left = 100.0f;
		else if(controller->out_left < -100.0f)	
			controller->out_left = -100.0f;
	
		if(controller->out_right > 100.0f) 	
			controller->out_right = 100.0f;
		else if(controller->out_right < -100.0f)	
			controller->out_right = -100.0f;
	
    MOTOR_SET_SPEED(&MOTOR_LEFT, controller->out_left);
		MOTOR_SET_SPEED(&MOTOR_RIGHT, controller->out_right);
		
}

void PROFILER_PD_CONTROLLER_INIT(sPDController *controller, sProfiler *translation, sProfiler *rotation)
{
	
		controller->translation = translation;
		controller->rotation = rotation;
	
}

void PROFILER_MOVE(sPDController *profiler, float translation_distance_to_travel, float translation_next_velocity, float translation_max_velocity, float translation_acceleration,
	float rotation_distance_to_travel, float rotation_next_velocity, float rotation_max_velocity, float rotation_acceleration)
{

		if((profiler->translation->state == PROFILER_FINISH) && (profiler->rotation->state == PROFILER_FINISH))
		{
			profiler->translation->distance_to_travel = translation_distance_to_travel;
			profiler->translation->next_velocity = translation_next_velocity;
			profiler->translation->max_velocity = translation_max_velocity;
			profiler->translation->acceleration = translation_acceleration;
			
			profiler->rotation->distance_to_travel = rotation_distance_to_travel;
			profiler->rotation->next_velocity = rotation_next_velocity;
			profiler->rotation->max_velocity = rotation_max_velocity;
			profiler->rotation->acceleration = rotation_acceleration;
			
			profiler->translation->state = PROFILER_RUN;
			profiler->rotation->state = PROFILER_RUN;
		}
	
}

void PROFILER_TRANSLATION_SET_ENABLE(sMOUSE *mouse)
{
	mouse->is_profiler_translation_enable = true;
}
void PROFILER_TRANSLATION_SET_DISABLE(sMOUSE *mouse)
{
	mouse->is_profiler_translation_enable = false;
}
bool PROFILER_TRANSLATION_IS_ENABLE(sMOUSE *mouse)
{
	return mouse->is_profiler_translation_enable;
}
void PROFILER_ROTATION_SET_ENABLE(sMOUSE *mouse)
{
	mouse->is_profiler_rotation_enable = true;
}
void PROFILER_ROTATION_SET_DISABLE(sMOUSE *mouse)
{
	mouse->is_profiler_rotation_enable = false;
}
bool PROFILER_ROTATION_IS_ENABLE(sMOUSE *mouse)
{
	return mouse->is_profiler_rotation_enable;
}
