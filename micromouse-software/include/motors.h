#ifndef _MOTORS_H
#define _MOTORS_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "math.h"

#define MOTR_IN1_OFF 		GPIOB->ODR &= ~GPIO_ODR_OD7;
#define MOTR_IN1_ON 		GPIOB->ODR |= GPIO_ODR_OD7;
#define MOTR_IN2_OFF 		GPIOB->ODR &= ~GPIO_ODR_OD6;
#define MOTR_IN2_ON 		GPIOB->ODR |= GPIO_ODR_OD6;
#define MOTL_IN1_OFF 		GPIOB->ODR &= ~GPIO_ODR_OD5;
#define MOTL_IN1_ON 		GPIOB->ODR |= GPIO_ODR_OD5;
#define MOTL_IN2_OFF 		GPIOB->ODR &= ~GPIO_ODR_OD4;
#define MOTL_IN2_ON 		GPIOB->ODR |= GPIO_ODR_OD4;

#define MOTR_FORWARD		{MOTR_IN1_ON 		MOTR_IN2_OFF}
#define MOTR_BACKWARDS	{MOTR_IN1_OFF		MOTR_IN2_ON}

#define MOTL_FORWARD		{MOTL_IN1_ON		MOTL_IN2_OFF}
#define MOTL_BACKWARDS	{MOTL_IN1_OFF		MOTL_IN2_ON}

#define MOT_FORWARD			{MOTR_FORWARD		MOTL_FORWARD}
#define MOT_BACKWARDS		{MOTR_BACKWARDS	MOTL_BACKWARDS}

#define MOTR_STOP		{MOTR_IN1_OFF	MOTR_IN2_OFF}
#define MOTL_STOP 	{MOTL_IN1_OFF MOTL_IN2_OFF}
#define MOT_STOP		{MOTR_STOP MOTL_STOP}

#define MOTR_SET_PWM(value)		TIM8->CCR1 = value
#define MOTL_SET_PWM(value)		TIM8->CCR2 = value

void MOTORS_Init(void);
void MOTR_SET_SPEED(float speed);
void MOTL_SET_SPEED(float speed);

void ENCODERS_Init(void);

extern volatile int32_t MOTR_enc;
extern volatile int32_t MOTL_enc;

#endif
