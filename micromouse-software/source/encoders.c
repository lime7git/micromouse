#include "encoders.h"

void ENCODERS_Init(void)
{
	// enable clock for port A, B and enable TIM2 and TIM5
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	// GPIO alternative functions
	GPIOA->MODER &= ~GPIO_MODER_MODER0;
	GPIOA->MODER |= GPIO_MODER_MODER0_1;
	GPIOA->MODER &= ~GPIO_MODER_MODER1;
	GPIOA->MODER |= GPIO_MODER_MODER1_1;
	GPIOA->AFR[0] |= 0x00000022;
	
	GPIOA->MODER &= ~GPIO_MODER_MODER15;
	GPIOA->MODER |= GPIO_MODER_MODER15_1;
	GPIOB->MODER &= ~GPIO_MODER_MODER3;
	GPIOB->MODER |= GPIO_MODER_MODER3_1;
	GPIOA->AFR[1] |= 0x10000000;
	GPIOB->AFR[0] |= 0x00001000;
	
	TIM5->PSC = 0;
	TIM5->ARR = 0xFFFFFFFF;
	
	TIM5->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // IC1 mapped on TI1, IC2 mapped on TI2
	TIM5->CCER 	|= (1 << TIM_CCER_CC1P_Pos) | (0 << TIM_CCER_CC2P_Pos) | (0 << TIM_CCER_CC1NP_Pos) | (0 << TIM_CCER_CC2NP_Pos) | TIM_CCER_CC1E | TIM_CCER_CC2E; // TI1 inverted, TI2 non-inverted , enable channel 1 and 2
	TIM5->SMCR 	|= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1; // encoder mode 4 edges
	TIM5->CR1 	|= TIM_CR1_CEN; // counter enable
	
	TIM2->PSC = 0;
	TIM2->ARR = 0xFFFFFFFF;
	
	TIM2->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
	TIM2->CCER 	|= (0 << TIM_CCER_CC1P_Pos) | (0 << TIM_CCER_CC2P_Pos) | (0 << TIM_CCER_CC1NP_Pos) | (0 << TIM_CCER_CC2NP_Pos) | TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM2->SMCR 	|= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1; // encoder mode 4 edges
	TIM2->CR1 	|= TIM_CR1_CEN;
}
int32_t ENCODER_GET_VALUE(sMOT *pMOTOR)
{
	int32_t value;
	
	switch (pMOTOR->motorSide)
	{
		case LEFT_MOTOR:
		{
			value = (int32_t)TIM5->CNT;
		break;
		}
		
		case RIGHT_MOTOR:
		{
			value = (int32_t)TIM2->CNT;
		break;
		}
	}
	
	return value;
}
void CALCULATE_ACTUAL_POSITION(sMOUSE *pMOUSE, sMOT *pMOTOR_LEFT, sMOT *pMOTOR_RIGHT)
{
	float distance;
	float actual_angle_temporary;
	
	pMOTOR_LEFT->encPrev 			= pMOTOR_LEFT->enc;
	pMOTOR_LEFT->enc 					= ENCODER_GET_VALUE(pMOTOR_LEFT);
	pMOTOR_LEFT->encDiff 			= pMOTOR_LEFT->enc - pMOTOR_LEFT->encPrev;
	pMOTOR_LEFT->dist 				= (float)pMOTOR_LEFT->encDiff / ENC_IMP_PER_ROTATE * CIRCUMFERENCE_OF_WHEEL;
	pMOTOR_LEFT->totalDist 		+= pMOTOR_LEFT->dist;
	
	pMOTOR_RIGHT->encPrev 		= pMOTOR_RIGHT->enc;
	pMOTOR_RIGHT->enc 				= ENCODER_GET_VALUE(pMOTOR_RIGHT);
	pMOTOR_RIGHT->encDiff 		= pMOTOR_RIGHT->enc - pMOTOR_RIGHT->encPrev;
	pMOTOR_RIGHT->dist 				= (float)pMOTOR_RIGHT->encDiff / ENC_IMP_PER_ROTATE * CIRCUMFERENCE_OF_WHEEL;
	pMOTOR_RIGHT->totalDist 	+= pMOTOR_RIGHT->dist;
	
	actual_angle_temporary = ((pMOTOR_LEFT->totalDist - pMOTOR_RIGHT->totalDist) * ROTATE_CALIB) / DISTANCE_BETWEEN_WHEELS * RAD_TO_DEG;
	pMOUSE->actual_angle = fmodf(actual_angle_temporary, 360.0f);
	
	if(pMOUSE->actual_angle < -180.0f)
	{
		pMOUSE->actual_angle += 360.0f;
	}
	else if(pMOUSE->actual_angle > 180.0f)
	{
		pMOUSE->actual_angle -= 360.0f;
	}

	distance = (pMOTOR_LEFT->dist + pMOTOR_RIGHT->dist) / 2.0f;
	
	pMOUSE->actual_position_x += distance * sinf(pMOUSE->actual_angle * DEG_TO_RAD);
	pMOUSE->actual_position_y += distance * cosf(pMOUSE->actual_angle * DEG_TO_RAD);
	
	pMOTOR_LEFT->act_rpm 	= ((float)pMOTOR_LEFT->encDiff 	/ TIME_STAMP * 60.0f) / ENC_IMP_PER_ROTATE;
	pMOTOR_RIGHT->act_rpm = ((float)pMOTOR_RIGHT->encDiff / TIME_STAMP * 60.0f) / ENC_IMP_PER_ROTATE;
	
	pMOTOR_LEFT->act_rpm_filtered = 0.854f * pMOTOR_LEFT->act_rpm_filtered + 0.0728f * pMOTOR_LEFT->act_rpm + 0.0728f * pMOTOR_LEFT->prev_rpm;
	pMOTOR_LEFT->prev_rpm = pMOTOR_LEFT->act_rpm;
	
	pMOTOR_RIGHT->act_rpm_filtered = 0.854f * pMOTOR_RIGHT->act_rpm_filtered + 0.0728f * pMOTOR_RIGHT->act_rpm + 0.0728f * pMOTOR_RIGHT->prev_rpm;
	pMOTOR_RIGHT->prev_rpm = pMOTOR_RIGHT->act_rpm;
	
}
