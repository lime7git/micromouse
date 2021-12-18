#ifndef _PROFILER_H
#define _PROFILER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdbool.h"
#include "controller.h"
#include "math.h"

typedef enum {
	
	PROFILER_FINISH = 0,
	PROFILER_DECELERATION = 1,
	PROFILER_RUN = 2
	
}	eProfilerState;

#pragma pack(push)
#pragma pack(1) 
typedef struct{
	
	float distance_to_travel;	/* target point for profiler to achieve */
	float remaining_distance;	/* calculated distance left to travel */
	float	max_velocity;				/* maximum velocity that can be reached in this run */
	float next_velocity;			/* maximum velocity in next run after finish actual run */
	float current_velocity;		/* current velocity */
	float acceleration;				/* acceleration - adding or subtracting from current velocity every time step */

	eProfilerState state;			/* current profiler state, possible states :
																PROFILER_FINISH = 0,
																PROFILER_DECELERATION = 1,
																PROFILER_RUN = 2	*/
	
}	sProfiler;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1) 
typedef struct{
	
	sProfiler *translation;
	sProfiler *rotation;
	
	float current_translation_e;
	float previous_translation_e;	
	float current_rotation_e;
	float previous_rotation_e;
	
	float out_left;		/* output from PD controller to left motor */
	float out_right;	/* output from PD controller to right motor */
	
}	sPDController; 
#pragma pack(pop)

extern sProfiler Translation;
extern sProfiler Rotation;
extern sPDController Controller;

/**
* @name : PROFILER_TRANSLATION_CONTROLLER 
* ----------------------------- *
* @description : none
* @params : pointer to a translation profiler structure
* @return : none
*/
void PROFILER_TRANSLATION_CONTROLLER(sProfiler *profiler);

/**
* @name : PROFILER_ROTATION_CONTROLLER 
* ----------------------------- *
* @description : none
* @params : pointer to a rotation profiler structure
* @return : none
*/
void PROFILER_ROTATION_CONTROLLER(sProfiler *profiler);

/**
* @name : PROFILER_PD_CONTROLLER 
* ----------------------------- *
* @description : none
* @params : pointer to ### and mouse structure
* @return : none
*/
void PROFILER_PD_CONTROLLER(sPDController *controller, sMOUSE *mouse);

/*
* ###TO DO### FUNCTION TO INITIATE NEW MOVEMENT
*/

void PROFILER_PD_CONTROLLER_INIT(sPDController *controller, sProfiler *translation, sProfiler *rotation);

void PROFILER_MOVE(sPDController *profiler, float translation_distance_to_travel, float translation_next_velocity, float translation_max_velocity, float translation_acceleration,
		float rotation_distance_to_travel, float rotation_next_velocity, float rotation_max_velocity, float rotation_acceleration);

void PROFILER_TRANSLATION_SET_ENABLE(sMOUSE *mouse);
void PROFILER_TRANSLATION_SET_DISABLE(sMOUSE *mouse);
bool PROFILER_TRANSLATION_IS_ENABLE(sMOUSE *mouse);

void PROFILER_ROTATION_SET_ENABLE(sMOUSE *mouse);
void PROFILER_ROTATION_SET_DISABLE(sMOUSE *mouse);
bool PROFILER_ROTATION_IS_ENABLE(sMOUSE *mouse);

#endif
