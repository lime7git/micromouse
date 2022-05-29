#ifndef _PID_H
#define _PID_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "stdbool.h"

#define TIME_STAMP 0.01f

typedef enum{
	LEFT_MOTOR = 0,
	RIGHT_MOTOR = 1
}eMOT;
	
typedef struct{
	
	eMOT motorSide;
	
	float set_rpm;
	float act_rpm;
	float act_rpm_filtered;
	float prev_rpm;
	float e;
	float e_prev;
	float e_total;
	float out;
	float kp;
	float ki;
	float kd;
	float totalDist;
	float dist;
	
	int32_t enc;
	int32_t encPrev;
	int32_t encDiff;
	
	bool pidEnable;
	
}sMOT; 

extern sMOT MOTOR_LEFT;
extern sMOT MOTOR_RIGHT;

void MOTOR_PID_INIT(sMOT *pMOTOR, eMOT motor_side, float kp, float ki, float kd);
void MOTOR_PID_CONTROLLER(sMOT *pMOTOR);
void MOTOR_PID_ENABLE(sMOT *pMOTOR);
void MOTOR_PID_DISABLE(sMOT *pMOTOR);
bool MOTOR_PID_IS_ENABLE(sMOT *pMOTOR);

#endif
