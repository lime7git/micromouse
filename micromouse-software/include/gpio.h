#ifndef _GPIO_H
#define _GPIO_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include "stdbool.h"

#define LED1_TOG		GPIOC->ODR ^= GPIO_ODR_OD4
#define LED1_ON 		GPIOC->ODR &= ~GPIO_ODR_OD4
#define LED1_OFF 		GPIOC->ODR |= GPIO_ODR_OD4

#define LED2_TOG		GPIOC->ODR ^= GPIO_ODR_OD5
#define LED2_ON 		GPIOC->ODR &= ~GPIO_ODR_OD5
#define LED2_OFF 		GPIOC->ODR |= GPIO_ODR_OD5

#define LED3_TOG		GPIOB->ODR ^= GPIO_ODR_OD0
#define LED3_ON 		GPIOB->ODR &= ~GPIO_ODR_OD0
#define LED3_OFF 		GPIOB->ODR |= GPIO_ODR_OD0

#define LED4_TOG		GPIOB->ODR ^= GPIO_ODR_OD12
#define LED4_ON 		GPIOB->ODR &= ~GPIO_ODR_OD12
#define LED4_OFF 		GPIOB->ODR |= GPIO_ODR_OD12

#define IR_LEFT_FRONT_ON 			GPIOA->ODR |= GPIO_ODR_OD8
#define IR_LEFT_FRONT_OFF 		GPIOA->ODR &= ~GPIO_ODR_OD8

#define IR_RIGHT_FRONT_ON 		GPIOB->ODR |= GPIO_ODR_OD13
#define IR_RIGHT_FRONT_OFF 		GPIOB->ODR &= ~GPIO_ODR_OD13

#define IR_LEFT_SIDE_ON				GPIOC->ODR |= GPIO_ODR_OD8
#define IR_LEFT_SIDE_OFF 			GPIOC->ODR &= ~GPIO_ODR_OD8

#define IR_RIGHT_SIDE_ON 			GPIOB->ODR |= GPIO_ODR_OD15
#define IR_RIGHT_SIDE_OFF 		GPIOB->ODR &= ~GPIO_ODR_OD15

#define TEST_PIN_ON 			GPIOB->ODR |= GPIO_ODR_OD9
#define TEST_PIN_OFF 			GPIOB->ODR &= ~GPIO_ODR_OD9

#define TEST_PIN2_ON 			GPIOB->ODR |= GPIO_ODR_OD8
#define TEST_PIN2_OFF 		GPIOB->ODR &= ~GPIO_ODR_OD8


#define IR_ALL_OFF		\
	IR_LEFT_FRONT_OFF;	\
	IR_RIGHT_FRONT_OFF; \
	IR_LEFT_SIDE_OFF; 	\
	IR_RIGHT_SIDE_OFF; 

typedef enum{LED1 = 0, LED2 = 1, LED3 = 2, LED4 = 3, LED_ALL = 4}eLEDnum;
typedef enum{OFF = 0, ON = 1, TOG = 2}eLEDstate;

void GPIO_Init(void);

typedef struct {
	bool isPressed;
	bool wasPressed;
	uint16_t time; // [ms]
}BUTTONS;

extern volatile BUTTONS BUTTON_OK;
extern volatile BUTTONS BUTTON_SEL;

#define CNT_2_MILISECONDS(value) (uint16_t)(value / 1.5f)

#define SHORT_PRESS(value) (value > 25 && value < 250)
#define NORMAL_PRESS(value) (value > 250 && value < 1500)
#define LONG_PRESS(value) (value > 1500)

void BUTTON_EXTI_Init(void);
void EXTI15_10_IRQHandler(void);

void LED_Switch(eLEDnum num, eLEDstate state);

#endif
