#ifndef _PROFILER_H
#define _PROFILER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdbool.h"
#include "controller.h"

typedef enum {
	FINISH = 0,
	DECELERATION = 1,
	RUNNING = 2
}eProfilerState;

int PROFILER(void);
void PROFILER_ENABLE(sMOUSE *mouse);
void PROFILER_DISABLE(sMOUSE *mouse);
bool PROFILER_IS_ENABLE(sMOUSE *mouse);
bool is_required_decelaration(void);
bool is_achieve_target(void);

extern int target_S;
extern int zad_S;
extern eProfilerState state; 
extern float max_velocity;        // docelowa_V - docelowa predkosc, do której stopniowo dazy profiler 
extern float next_velocity;                // predkosc w nastepnym ruchu (po przejechaniu zadanej odleglosci) 
extern float act_velocity;            // aktualnie zadana predkosc 
extern float acceleration;

#endif
