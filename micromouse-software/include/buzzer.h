#ifndef _BUZZER_H
#define _BUZZER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"

void BUZZER_Init(void);
void BUZZER_Volume(uint8_t volume);
void BUZZER_Buzz(uint8_t buzz_volume, uint8_t num_of_buzz, uint16_t buzz_time, uint16_t buzz_delay);

#endif
