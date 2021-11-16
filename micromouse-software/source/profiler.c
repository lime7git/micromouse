#include "profiler.h"

eProfilerState state = FINISH;            // stan profilera; mozliwe wartosci: JAZDA, HAMOWANIE, KONIEC 
float max_velocity = 600.0f;        // docelowa_V - docelowa predkosc, do której stopniowo dazy profiler 
float next_velocity = 0;                // predkosc w nastepnym ruchu (po przejechaniu zadanej odleglosci) 
float act_velocity = 0;            // aktualnie zadana predkosc 
float acceleration = 0.1f;                // zadane przyspieszenie 
int zad_S = 0;                    // wyliczona droga do przejechania - wykorzystywana jako setpoint regulatora PD 
int target_S = 0;                // zadana calkowita droga do przejechania 

int PROFILER(void)
{
	
	 if(state == RUNNING) 
    { 
        if(is_required_decelaration()) 
        { 
            state = DECELERATION; 
            max_velocity = next_velocity; 
        } 
    } 
    
    if(is_achieve_target() && state != FINISH) 
    { 
        state = FINISH; 
        max_velocity = next_velocity; 
    } 
		
    
    if(act_velocity < max_velocity) 
    { 
        act_velocity += acceleration; 
        if(act_velocity > max_velocity) 
            act_velocity = max_velocity; 
    } 
    
    if(act_velocity > max_velocity) 
    { 
        act_velocity -= acceleration; 
        if(act_velocity < max_velocity) 
            act_velocity = max_velocity; 
    } 
    
    return zad_S = (int)act_velocity; 
	
}
void PROFILER_ENABLE(sMOUSE *mouse)
{
	mouse->profillerEnable = true;
}
void PROFILER_DISABLE(sMOUSE *mouse)
{
	mouse->profillerEnable = false;
}
bool PROFILER_IS_ENABLE(sMOUSE *mouse)
{
	return mouse->profillerEnable;
}
bool is_required_decelaration(void)
{
	if(((act_velocity - next_velocity)*(act_velocity + next_velocity)/(2 * acceleration) >= (float)(target_S - zad_S)))
	{
		return true;
	}
	else return false;
}

bool is_achieve_target(void)
{
	if(zad_S >= target_S || (state == DECELERATION && act_velocity == 0.0f)) 
	{
		return true;
	}
	else return false;
}

