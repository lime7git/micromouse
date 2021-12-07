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


void PROFILER_PD_CONTROLLER(sPDController *controller, sMOUSE *mouse)
{
	
    controller->current_translation_e = controller->translation->remaining_distance - mouse->trans;               
		
		controller->out_left = 1.0f * controller->current_translation_e + 0.05f * (controller->current_translation_e - controller->previous_translation_e);
		controller->out_right = 1.0f * controller->current_translation_e + 0.05f * (controller->current_translation_e - controller->previous_translation_e);
	
		controller->previous_translation_e = controller->current_translation_e;
 
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

void PROFILER_TRANSLATION_SET_ENABLE(sMOUSE *mouse)
{
	mouse->profillerTransEnable = true;
}
void PROFILER_TRANSLATION_SET_DISABLE(sMOUSE *mouse)
{
	mouse->profillerTransEnable = false;
}
bool PROFILER_TRANSLATION_IS_ENABLE(sMOUSE *mouse)
{
	return mouse->profillerTransEnable;
}
