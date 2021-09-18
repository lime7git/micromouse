#include "pid.h"

sMOT MOTOR_LEFT;
sMOT MOTOR_RIGHT;

void MOTOR_PID_INIT(sMOT *motor, eMOT motor_side, float kp, float ki, float kd)
{
	motor->motorSide = motor_side;
	motor->kp = kp;
	motor->ki = ki;
	motor->kd = kd;
	
	motor->set_rpm = 0.0f;
	motor->act_rpm = 0.0f;
	motor->e = 0.0f;
	motor->e_prev = 0.0f;
	motor->e_total = 0.0f;
	motor->out = 0.0f;
	motor->enc = 0;
	motor->encPrev = 0;
	motor->encDiff = 0;
	motor->pulse_per_sec = 0;
	motor->prev_pulse = 0;
	motor->totalDist = 0.0f;
	motor->dist = 0.0f;
}
