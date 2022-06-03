#include "adc.h"
#include "uart.h"
#include "gpio.h"
#include "ui.h"
#include "motors.h"
#include "controller.h"
#include "clock.h"
#include "timer.h"
#include <math.h>

volatile static uint16_t ADC1_readings[300];
volatile static uint16_t ADC2_readings[4];

volatile static double sensor_enviroment_value;
volatile static double sensor_raw_value;
volatile static double sensor_mean_enviroment = 0.0;
volatile static double sensor_mean_raw = 0.0;

volatile static uint32_t pervious_battery_tick = 0;

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
	DMA2_Stream4->NDTR 	= (uint16_t)300;	
	DMA2_Stream4->CR 	 |= (0 << DMA_SxCR_CHSEL_Pos) | (0 << DMA_SxCR_DIR_Pos) | DMA_SxCR_MINC | DMA_SxCR_CIRC | (1 << DMA_SxCR_PSIZE_Pos) | (1 << DMA_SxCR_MSIZE_Pos) | DMA_SxCR_EN; // channel 0 | pheripheral to memory | memory increment | circural | 16 bit data sieze | 16 bit data size | dma en
}

void ADC2_DMA_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	
	// set PC0/PC2/PA5/PA7 pin as an analog input
	GPIOA->MODER |= GPIO_MODER_MODE7;
	GPIOA->MODER |= GPIO_MODER_MODE5;
	GPIOC->MODER |= GPIO_MODER_MODE2;
	GPIOC->MODER |= GPIO_MODER_MODE0;
	
	ADC2->CR2 |= ADC_CR2_CONT | ADC_CR2_DMA | ADC_CR2_ADON | (0 << ADC_CR2_EOCS_Pos) | ADC_CR2_DDS;		//  continous conversion mode | enable DMA | enable ADC1 | interrupt after evry channel conversion | DMA always request for ADC (no overrun)
	ADC2->CR1 |= ADC_CR1_SCAN | (0 << ADC_CR1_RES_Pos) | ADC_CR1_EOCIE; // scan mode | 12-bit resolution | end of conversion interrupt enable
	ADC2->SQR1 |= (3 << ADC_SQR1_L_Pos); // number of conversions ( 0011 : 4 conversions)
	ADC2->SQR3 |= (12 << ADC_SQR3_SQ1_Pos) | (10 << ADC_SQR3_SQ2_Pos) | (7 << ADC_SQR3_SQ3_Pos) | (5 << ADC_SQR3_SQ4_Pos); // select channel for conversions
	ADC2->SMPR1 |= ADC_SMPR1_SMP12;
	ADC2->SMPR1 |= ADC_SMPR1_SMP10;
	ADC2->SMPR2 |= ADC_SMPR2_SMP7; // channel sampling time (480 cycles)
	ADC2->SMPR2 |= ADC_SMPR2_SMP5; 
	ADC2->CR2 |= ADC_CR2_SWSTART;
	NVIC_EnableIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, 1); 
	
	DMA2_Stream3->PAR 	= (uint32_t)&ADC2->DR;
	DMA2_Stream3->M0AR 	= (uint32_t)ADC2_readings;
	DMA2_Stream3->NDTR 	= (uint16_t)4;	
	DMA2_Stream3->CR 	 |= (1 << DMA_SxCR_CHSEL_Pos) | (0 << DMA_SxCR_DIR_Pos) | DMA_SxCR_MINC | DMA_SxCR_CIRC | (1 << DMA_SxCR_PSIZE_Pos) | (1 << DMA_SxCR_MSIZE_Pos) | DMA_SxCR_EN; // channel 0 | pheripheral to memory | memory increment | circural | 16 bit data sieze | 16 bit data size | dma en
}

void ADC_IRQHandler(void)
{
		if((ADC1->SR & ADC_SR_AWD) != RESET)
		{
			ADC1->SR &= ~ADC_SR_AWD;
			
			if(MOUSE.battery_voltage < CONV_2_BATTERY_VOLTAGE(ADC_WATCHDOG_LOWER_THRESHOLD_VOLTAGE))
			{
				//MOUSE.state = MOUSE_CRITICAL;
			}	
		}
}

void BATTERY_CRITICAL_PROCEDURE(void)
{
			MOT_STOP
			MOTR_SET_PWM(0);
			MOTL_SET_PWM(0);
			LED_Switch(LED1, OFF);
			LED_Switch(LED2, OFF);
			LED_Switch(LED3, OFF);
			LED_Switch(LED4, OFF);
	
			MOUSE.state = MOUSE_STOP;
	
			static char buf[64];
			sprintf(buf,"\r\n### BATTERY WATCHDOG ###\r\nBattery voltage = %.2f\r\n",ADC_GET_BATTERY_VOLTAGE());
			UART1_Log(buf);
}

double ADC_GET_BATTERY_VOLTAGE(void) 			{	return CONV_2_BATTERY_VOLTAGE(ADC1_readings[0]);	}
double ADC_GET_BATTERY_VOLTAGE_MEAN(void)
{
	uint32_t adc_mean = 0;
	
	for(uint16_t i = 0; i < 100; i++)
	{
		adc_mean += ADC1_readings[i * 3]; 
	}
	adc_mean = adc_mean / 100;
	
	return CONV_2_BATTERY_VOLTAGE(adc_mean);
}
double ADC_GET_TEMPERATURE_INTERAL(void)	{	return CONV_2_CELCIUS_DEG(ADC1_readings[1]);			}
double ADC_GET_VREF_INTERNAL(void)				{	return CONV_2_ADC_VOLTAGE(ADC1_readings[2]);			}
void ADC_BATTERY_VOLTAGE_UPDATE(void)
{
	if(GET_SYSTEM_TICK() - pervious_battery_tick > BATTERY_MEASURMENT_DELAY)
	{
		MOUSE.battery_voltage = ADC_GET_BATTERY_VOLTAGE_MEAN();
		pervious_battery_tick = GET_SYSTEM_TICK();
	}
}

double ADC_GET_LEFT_FRONT_SENSOR_VOLTAGE(void) 	{	return CONV_2_ADC_VOLTAGE(ADC2_readings[0]);	}
double ADC_GET_RIGHT_FRONT_SENSOR_VOLTAGE(void)	{	return CONV_2_ADC_VOLTAGE(ADC2_readings[2]);	}
double ADC_GET_LEFT_SIDE_SENSOR_VOLTAGE(void)		{	return CONV_2_ADC_VOLTAGE(ADC2_readings[1]);	}
double ADC_GET_RIGHT_SIDE_SENSOR_VOLTAGE(void)	{	return CONV_2_ADC_VOLTAGE(ADC2_readings[3]);	}

double SENSOR_GET_LEFT_FRONT_DISTANCE(eSENSORSunit unit)
{
	IR_ALL_OFF
	
	sensor_mean_enviroment = 0.0;
	sensor_mean_raw = 0.0;
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_enviroment += ADC2_readings[0];
		}
	IR_LEFT_FRONT_ON;
	delay_us(IR_LED_SWITCHING_TIME_DELAY);
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_raw += ADC2_readings[0];
		}
	IR_LEFT_FRONT_OFF;
	
	if(unit == RAW)
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
	}
	else
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
		sensor_raw_value = (2393.3 / (log(sensor_raw_value - 114.6))) - 213.0;
		if(sensor_raw_value > 220.0f) sensor_raw_value = 220.0f;
	}
	
	if(unit == CM) sensor_raw_value /= 10.0;
	
	return sensor_raw_value;
}
double SENSOR_GET_RIGHT_FRONT_DISTANCE(eSENSORSunit unit)
{
	IR_ALL_OFF
	
	sensor_mean_enviroment = 0.0;
	sensor_mean_raw = 0.0;
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_enviroment += ADC2_readings[2];
		}
	IR_RIGHT_FRONT_ON;
	delay_us(IR_LED_SWITCHING_TIME_DELAY);
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_raw += ADC2_readings[2];
		}
	IR_RIGHT_FRONT_OFF;
	
	if(unit == RAW)
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
	}
	else
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
		sensor_raw_value = (2871.4 / (log(sensor_raw_value - 415.3))) - 263.4;
		if(sensor_raw_value > 220.0f) sensor_raw_value = 220.0f;
	}
	
	if(unit == CM) sensor_raw_value /= 10.0;
	
	return sensor_raw_value;
}
double SENSOR_GET_LEFT_SIDE_DISTANCE(eSENSORSunit unit)
{
	IR_ALL_OFF
	
	sensor_mean_enviroment = 0.0;
	sensor_mean_raw = 0.0;
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_enviroment += ADC2_readings[1];
		}
	IR_LEFT_SIDE_ON;
	delay_us(IR_LED_SWITCHING_TIME_DELAY);
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_raw += ADC2_readings[1];
		}
	IR_LEFT_SIDE_OFF;
		
	if(unit == RAW)
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
	}
	else
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
		sensor_raw_value = (2318.0 / (log(sensor_raw_value - 155.2))) - 235.1;
		if(sensor_raw_value > 220.0f) sensor_raw_value = 220.0f;
	}
	
	if(unit == CM) sensor_raw_value /= 10.0;
	
	return sensor_raw_value;		
}
double SENSOR_GET_RIGHT_SIDE_DISTANCE(eSENSORSunit unit)
{
	IR_ALL_OFF
	
	sensor_mean_enviroment = 0.0;
	sensor_mean_raw = 0.0;
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_enviroment += ADC2_readings[3];
		}
	IR_RIGHT_SIDE_ON;
	delay_us(IR_LED_SWITCHING_TIME_DELAY);
		for(int i = 0; i < NUMBER_OF_MEASURMENTS; i++)
		{
			sensor_mean_raw += ADC2_readings[3];
		}
	IR_RIGHT_SIDE_OFF;
		
	if(unit == RAW)
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
	}
	else
	{
		sensor_raw_value = (sensor_mean_raw / NUMBER_OF_MEASURMENTS) - (sensor_mean_enviroment / NUMBER_OF_MEASURMENTS);
		sensor_raw_value = (3225.2 / (log(sensor_raw_value - 62.1))) - 345.9;
		if(sensor_raw_value > 220.0f) sensor_raw_value = 220.0f;
	}

	if(unit == CM) sensor_raw_value /= 10.0;
	
	return sensor_raw_value;
}
