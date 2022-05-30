#ifndef _ENCODERS_H
#define _ENCODERS_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include <math.h>
#include "controller.h"
#include "pid.h"

#define CIRCUMFERENCE_OF_WHEEL 100.53f		
#define DISTANCE_BETWEEN_WHEELS 85.0f 	
#define ENC_IMP_PER_ROTATE 909.72f		
#define ROTATE_CALIB 1.0f 

#define DEG_TO_RAD 0.0174532925f
#define RAD_TO_DEG 57.295779513f

#define M_PI 3.14159265359f

void ENCODERS_Init(void);
int32_t ENCODER_GET_VALUE(sMOT *pMOTOR);
void CALCULATE_ACTUAL_POSITION(sMOUSE *pMOUSE, sMOT *pMOTOR_LEFT, sMOT *pMOTOR_RIGHT);


#endif
