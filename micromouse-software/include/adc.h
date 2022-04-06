#ifndef _ADC_H
#define _ADC_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"

#define CONV_2_ADC_VOLTAGE(value) 		(value * 0.0008056640625)
#define CONV_2_BATTERY_VOLTAGE(value) (CONV_2_ADC_VOLTAGE(value) * 5.54545454)
#define CONV_2_CELCIUS_DEG(value)			(((CONV_2_ADC_VOLTAGE(value) - 0.76) / 0.0025 ) + 25) 

#define BATTERY_CELLS_NUMBER 2

#if BATTERY_CELLS_NUMBER == 3
	#define ADC_WATCHDOG_HIGHER_THRESHOLD_VOLTAGE		2800	// about 2.25V adc = 12.5V battery voltage
	#define ADC_WATCHDOG_LOWER_THRESHOLD_VOLTAGE 		2200	// about 1.78V adc = 9.9V battery voltage
#endif

#if BATTERY_CELLS_NUMBER == 2
	#define ADC_WATCHDOG_HIGHER_THRESHOLD_VOLTAGE	2000	// (1850) - about 1.49V adc = 8.3V battery voltage // (1900) - ???
	#define ADC_WATCHDOG_LOWER_THRESHOLD_VOLTAGE 	1480 	// about 1.19V adc = 6.6V battery voltage
#endif

void ADC1_DMA_Init(void);
void ADC2_DMA_init(void);
void ADC_IRQHandler(void);
void BATTERY_CRITICAL_PROCEDURE(void);

double ADC_GET_BATTERY_VOLTAGE(void);
double ADC_GET_TEMPERATURE_INTERAL(void);
double ADC_GET_VREF_INTERNAL(void);

double ADC_GET_LEFT_FRONT_SENSOR_VOLTAGE(void);
double ADC_GET_RIGHT_FRONT_SENSOR_VOLTAGE(void);
double ADC_GET_LEFT_SIDE_SENSOR_VOLTAGE(void);
double ADC_GET_RIGHT_SIDE_SENSOR_VOLTAGE(void);

double ADC_GET_LEFT_FRONT_SENSOR_VOLTAGE(void);
double ADC_GET_RIGHT_FRONT_SENSOR_VOLTAGE(void);
double ADC_GET_LEFT_SIDE_SENSOR_VOLTAGE(void);
double ADC_GET_RIGHT_SIDE_SENSOR_VOLTAGE(void);


extern volatile uint16_t ADC1_readings[3];
extern volatile uint16_t ADC2_readings[4];

#endif
