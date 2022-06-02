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

#define BATTERY_MEASURMENT_DELAY 2500 	// time in ms 
#define NUMBER_OF_MEASURMENTS 100.0			// number of signle sensors mesurments during one sequency X2
#define IR_LED_SWITCHING_TIME_DELAY 25 	// time in us

void ADC1_DMA_Init(void);
void ADC2_DMA_init(void);
void ADC_IRQHandler(void);
void BATTERY_CRITICAL_PROCEDURE(void);

double ADC_GET_BATTERY_VOLTAGE(void);
double ADC_GET_BATTERY_VOLTAGE_MEAN(void);
double ADC_GET_TEMPERATURE_INTERAL(void);
double ADC_GET_VREF_INTERNAL(void);
void ADC_BATTERY_VOLTAGE_UPDATE(void);

double ADC_GET_LEFT_FRONT_SENSOR_VOLTAGE(void);
double ADC_GET_RIGHT_FRONT_SENSOR_VOLTAGE(void);
double ADC_GET_LEFT_SIDE_SENSOR_VOLTAGE(void);
double ADC_GET_RIGHT_SIDE_SENSOR_VOLTAGE(void);

typedef enum{RAW = 0, MM = 1, CM = 2}eSENSORSunit;

double SENSOR_GET_LEFT_FRONT_DISTANCE(eSENSORSunit unit);
double SENSOR_GET_RIGHT_FRONT_DISTANCE(eSENSORSunit unit);
double SENSOR_GET_LEFT_SIDE_DISTANCE(eSENSORSunit unit);
double SENSOR_GET_RIGHT_SIDE_DISTANCE(eSENSORSunit unit);


#endif
