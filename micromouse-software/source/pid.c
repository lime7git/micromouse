#include "pid.h"
#include "motors.h"

sMOT MOTOR_LEFT;
sMOT MOTOR_RIGHT;

void MOTOR_PID_INIT(sMOT *pMOTOR, eMOT motor_side, float kp, float ki, float kd)
{
	pMOTOR->motorSide = motor_side;
	pMOTOR->pidEnable = false;
	pMOTOR->kp = kp;
	pMOTOR->ki = ki;
	pMOTOR->kd = kd;
	
	pMOTOR->set_rpm = 0.0f;
	pMOTOR->act_rpm = 0.0f;
	pMOTOR->e = 0.0f;
	pMOTOR->e_prev = 0.0f;
	pMOTOR->e_total = 0.0f;
	pMOTOR->out = 0.0f;
	pMOTOR->enc = 0;
	pMOTOR->encPrev = 0;
	pMOTOR->encDiff = 0;
	pMOTOR->totalDist = 0.0f;
	pMOTOR->dist = 0.0f;
}
void MOTOR_PID_CONTROLLER(sMOT *pMOTOR)
{
	pMOTOR->e_prev = pMOTOR->e;
  pMOTOR->e = pMOTOR->set_rpm - pMOTOR->act_rpm_filtered;
	pMOTOR->e_total += pMOTOR->e;
	
	if(pMOTOR->e_total > 1750.0f)		
		pMOTOR->e_total = 1750.0f;
	else if(pMOTOR->e_total < -1750.0f)	
		pMOTOR->e_total = -1750.0f;
	
	pMOTOR->out = pMOTOR->kp * pMOTOR->e + pMOTOR->ki * pMOTOR->e_total * TIME_STAMP + pMOTOR->kd * (pMOTOR->e - pMOTOR->e_prev) / TIME_STAMP;

	if(pMOTOR->out > 1.0f) 	
		pMOTOR->out = 1.0f;
	else if(pMOTOR->out < -1.0f)	
		pMOTOR->out = -1.0f;
	
	MOTOR_SET_SPEED(pMOTOR, pMOTOR->out);
}
void MOTOR_PID_ENABLE(sMOT *pMOTOR)
{
	pMOTOR->pidEnable = true;
}
void MOTOR_PID_DISABLE(sMOT *pMOTOR)
{
	pMOTOR->pidEnable = false;
}
bool MOTOR_PID_IS_ENABLE(sMOT *pMOTOR)
{
	return pMOTOR->pidEnable;
}
