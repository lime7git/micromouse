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
	
// pragma to aviod padding aligment warning
#pragma pack(push)
#pragma pack(1)  
typedef struct{
	
	eMOT motorSide;
	
	float set_rpm;
	float act_rpm;
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
	int32_t pulse_per_sec;
	int32_t prev_pulse;
	
}sMOT; 
#pragma pack(pop)

extern sMOT MOTOR_LEFT;
extern sMOT MOTOR_RIGHT;

void MOTOR_PID_INIT(sMOT *pMOTOR, eMOT motor_side, float kp, float ki, float kd);
void MOTOR_PID_CONTROLLER(sMOT *pMOTOR);



#endif
