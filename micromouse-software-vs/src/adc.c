#include "adc.h"
#include "uart.h"
#include "gpio.h"
#include "buzzer.h"
#include "ui.h"
#include "motors.h"
#include "controller.h"

volatile uint16_t ADC1_readings[3];
volatile uint16_t ADC2_readings[2];

void ADC1_DMA_Init(void)
{
	// enable clock for port A and clock for ADC1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	
	// set PA2 pin as an analog input
	GPIOA->MODER |= GPIO_MODER_MODE2;
	
	// ADC configuration
	ADC->CCR |= ADC_CCR_TSVREFE; 	// enable internal temperature sensor and Vref channel 
	ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_DMA | ADC_CR2_ADON | (0 << ADC_CR2_EOCS_Pos) | ADC_CR2_DDS;		//  continous conversion mode | enable DMA | enable ADC1 | interrupt after evry channel conversion | DMA always request for ADC (no overrun)
	ADC1->CR1 |= ADC_CR1_SCAN | (0 << ADC_CR1_RES_Pos) | ADC_CR1_EOCIE; // scan mode | 12-bit resolution | end of conversion interrupt enable
	ADC1->CR1 |= ADC_CR1_AWDEN | ADC_CR1_AWDSGL | (0 << ADC_CR1_JAWDEN_Pos) | ADC_CR1_AWDIE | (2 << ADC_CR1_AWDCH_Pos);	// enable watchdog for selected channel ||| enable watchdog interrupt | select channel 2 for watchdog
	ADC1->HTR = ADC_WATCHDOG_HIGHER_THRESHOLD_VOLTAGE;
	ADC1->LTR = ADC_WATCHDOG_LOWER_THRESHOLD_VOLTAGE;
	ADC1->SQR1 |= (2 << ADC_SQR1_L_Pos); // number of conversions ( 0010 : 3 conversions)
	ADC1->SQR3 |= (2 << ADC_SQR3_SQ1_Pos) | (16 << ADC_SQR3_SQ2_Pos) | (17 << ADC_SQR3_SQ3_Pos); // select channel for conversions
	ADC1->SMPR2 |= ADC_SMPR2_SMP2; // channel 2 sampling time (480 cycles)
	ADC1->SMPR1 |= ADC_SMPR1_SMP16 | ADC_SMPR1_SMP17; // channel 16,17 sampling time (480 cycles)
	ADC1->CR2 |= ADC_CR2_SWSTART;
	NVIC_EnableIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, 0); // higher settable priority 
	
	// DMA configuration
	DMA2_Stream4->PAR 	= (uint32_t)&ADC1->DR;
	DMA2_Stream4->M0AR 	= (uint32_t)ADC1_readings;
	DMA2_Stream4->NDTR 	= (uint16_t)3;	
	DMA2_Stream4->CR 	 |= (0 << DMA_SxCR_CHSEL_Pos) | (0 << DMA_SxCR_DIR_Pos) | DMA_SxCR_MINC | DMA_SxCR_CIRC | (1 << DMA_SxCR_PSIZE_Pos) | (1 << DMA_SxCR_MSIZE_Pos) | DMA_SxCR_EN; // channel 0 | pheripheral to memory | memory increment | circural | 16 bit data sieze | 16 bit data size | dma en
}

void ADC2_DMA_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	
	// set PA7/PC2 pin as an analog input
	GPIOA->MODER |= GPIO_MODER_MODE7;
	GPIOC->MODER |= GPIO_MODER_MODE2;
	
	ADC2->CR2 |= ADC_CR2_CONT | ADC_CR2_DMA | ADC_CR2_ADON | (0 << ADC_CR2_EOCS_Pos) | ADC_CR2_DDS;		//  continous conversion mode | enable DMA | enable ADC1 | interrupt after evry channel conversion | DMA always request for ADC (no overrun)
	ADC2->CR1 |= ADC_CR1_SCAN | (0 << ADC_CR1_RES_Pos) | ADC_CR1_EOCIE; // scan mode | 12-bit resolution | end of conversion interrupt enable
	ADC2->SQR1 |= (1 << ADC_SQR1_L_Pos); // number of conversions ( 0010 : 3 conversions)
	ADC2->SQR3 |= (12 << ADC_SQR3_SQ1_Pos) | (7 << ADC_SQR3_SQ2_Pos); // select channel for conversions
	ADC2->SMPR1 |= ADC_SMPR1_SMP12;
	ADC2->SMPR2 |= ADC_SMPR2_SMP7; // channel sampling time (480 cycles)
	ADC2->CR2 |= ADC_CR2_SWSTART;
	NVIC_EnableIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, 1); // higher settable priority 
	
	DMA2_Stream3->PAR 	= (uint32_t)&ADC2->DR;
	DMA2_Stream3->M0AR 	= (uint32_t)ADC2_readings;
	DMA2_Stream3->NDTR 	= (uint16_t)2;	
	DMA2_Stream3->CR 	 |= (1 << DMA_SxCR_CHSEL_Pos) | (0 << DMA_SxCR_DIR_Pos) | DMA_SxCR_MINC | DMA_SxCR_CIRC | (1 << DMA_SxCR_PSIZE_Pos) | (1 << DMA_SxCR_MSIZE_Pos) | DMA_SxCR_EN; // channel 0 | pheripheral to memory | memory increment | circural | 16 bit data sieze | 16 bit data size | dma en
}

void ADC_IRQHandler(void)
{
		if((ADC1->SR & ADC_SR_AWD) != RESET)
		{
			ADC1->SR &= ~ADC_SR_AWD;
			
//			MOUSE.state = CRITICAL;
//			MOUSE.state = CRITICAL;
//			MOUSE.state = CRITICAL;
		}
}

void BATTERY_CRITICAL_PROCEDURE(void)
{
			static char buf[64];
			sprintf(buf,"\r\n### BATTERY WATCHDOG ###\r\nBattery voltage = %.2f\r\n",ADC_GET_BATTERY_VOLTAGE());
			UART1_Log(buf);
	
			BUZZER_Buzz(128, 5, 50, 50);
				
			MOT_STOP
			MOTR_SET_PWM(0);
			MOTL_SET_PWM(0);
			LED_Switch(LED1, OFF);
			LED_Switch(LED2, OFF);
			LED_Switch(LED3, OFF);
			LED_Switch(LED4, OFF);
			BUZZER_Volume(0);
	
			MOUSE.state = MOUSE_STOP;
			MOUSE.state = MOUSE_STOP;
			MOUSE.state = MOUSE_STOP;
}

double ADC_GET_BATTERY_VOLTAGE(void) 			{	return CONV_2_BATTERY_VOLTAGE(ADC1_readings[0]);	}
double ADC_GET_TEMPERATURE_INTERAL(void)	{	return CONV_2_CELCIUS_DEG(ADC1_readings[1]);			}
double ADC_GET_VREF_INTERNAL(void)				{	return CONV_2_ADC_VOLTAGE(ADC1_readings[2]);			}