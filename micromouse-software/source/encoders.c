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
	
	TIM5->PSC = 1;
	TIM5->ARR = 0xFFFFFFFF;
	
	TIM5->CCMR1 |= (1 << TIM_CCMR1_CC1S_Pos) | (1 << TIM_CCMR1_CC2S_Pos) | (0 << TIM_CCMR1_IC1F_Pos) | (0 << TIM_CCMR1_IC2F_Pos) | (0 << TIM_CCMR1_IC1PSC_Pos) | (0 << TIM_CCMR1_IC2PSC_Pos);
	TIM5->CCER |= (0 << TIM_CCER_CC1P_Pos) | (1 << TIM_CCER_CC2P_Pos) | (0 << TIM_CCER_CC1NP_Pos) | (0 << TIM_CCER_CC2NP_Pos) | TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM5->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1; // encoder mode 2 edges
	TIM5->CR1 |= (0 << TIM_CR1_ARPE_Pos) | (0 << TIM_CR1_CKD_Pos) | TIM_CR1_CEN;
	
	TIM2->PSC = 1;
	TIM2->ARR = 0xFFFFFFFF;
	
	TIM2->CCMR1 |= (1 << TIM_CCMR1_CC1S_Pos) | (1 << TIM_CCMR1_CC2S_Pos) | (0 << TIM_CCMR1_IC1F_Pos) | (0 << TIM_CCMR1_IC2F_Pos) | (0 << TIM_CCMR1_IC1PSC_Pos) | (0 << TIM_CCMR1_IC2PSC_Pos);
	TIM2->CCER |= (0 << TIM_CCER_CC1P_Pos) | (0 << TIM_CCER_CC2P_Pos) | (0 << TIM_CCER_CC1NP_Pos) | (0 << TIM_CCER_CC2NP_Pos) | TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM2->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1; // encoder mode 2 edges
	TIM2->CR1 |= (0 << TIM_CR1_ARPE_Pos) | (0 << TIM_CR1_CKD_Pos) | TIM_CR1_CEN;
}
int32_t ENCODER_GET_VALUE(sMOT *pMOTOR)
{
	static int32_t value;
	
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
	pMOTOR_LEFT->encPrev = pMOTOR_LEFT->enc;
	pMOTOR_LEFT->enc = ENCODER_GET_VALUE(pMOTOR_LEFT);
	pMOTOR_LEFT->encDiff = pMOTOR_LEFT->enc - pMOTOR_LEFT->encPrev;
	pMOTOR_LEFT->dist = (float)pMOTOR_LEFT->encDiff / ENC_IMP_PER_ROTATE * CIRCUMFERENCE_OF_WHEEL;
	pMOTOR_LEFT->totalDist += pMOTOR_LEFT->dist;
	
	pMOTOR_RIGHT->encPrev = pMOTOR_RIGHT->enc;
	pMOTOR_RIGHT->enc = ENCODER_GET_VALUE(pMOTOR_RIGHT);
	pMOTOR_RIGHT->encDiff = pMOTOR_RIGHT->enc - pMOTOR_RIGHT->encPrev;
	pMOTOR_RIGHT->dist = (float)pMOTOR_RIGHT->encDiff / ENC_IMP_PER_ROTATE * CIRCUMFERENCE_OF_WHEEL;
	pMOTOR_RIGHT->totalDist += pMOTOR_RIGHT->dist;
	
	float tempAng = ((pMOTOR_LEFT->totalDist - pMOTOR_RIGHT->totalDist) * ROTATE_CALIB) / DISTANCE_BETWEEN_WHEELS * RAD_TO_DEG;
	pMOUSE->ang = fmodf(tempAng, 360.0f);
	
	if(pMOUSE->ang < -180.0f)
	{
		pMOUSE->ang += 360.0f;
	}
	else if(pMOUSE->ang > 180.0f)
	{
		pMOUSE->ang -= 360.0f;
	}

	pMOUSE->distance = (pMOTOR_LEFT->dist + pMOTOR_RIGHT->dist) / 2.0f;
	
	pMOUSE->pos_x += pMOUSE->distance * sinf(pMOUSE->ang * DEG_TO_RAD);
	pMOUSE->pos_y += pMOUSE->distance * cosf(pMOUSE->ang * DEG_TO_RAD);
}
void MOTOR_CALCULATE_SPEED(sMOT *pMOTOR)
{
	pMOTOR->pulse_per_sec = ENCODER_GET_VALUE(pMOTOR) - pMOTOR->prev_pulse;
	pMOTOR->prev_pulse = ENCODER_GET_VALUE(pMOTOR);
	
	pMOTOR->act_rpm = (float)pMOTOR->pulse_per_sec * 100.0f * 60.0f / ENC_IMP_PER_ROTATE;
}
