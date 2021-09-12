#ifndef _MYFUN
#define _MYFUN
#include <stm32l4xx.h>
#include <stm32l496xx.h>
#include <stdio.h>
typedef enum{LedOff = 0, LedOn = 1, LedTog = 2}eLed;
typedef enum{JoyNull = 0, Up = 1, Down = 2, Left = 3, Right = 4, Center = 5}eJoy;
void delay_ms(uint32_t ms);
void Led_Conf(void);
void Led_OnOff(uint8_t num, eLed state);
void Joy_Conf(void);
eJoy Joy_Read(void);
void Led7seg_Conf(void);
void Led7seg_WriteDigit(uint8_t pos, uint8_t num);
void Led7seg_WriteNumber(void);
void Timer(void);
void ShowTime(void);
void LPUART1_Conf_Basic(void);
void ComSendChar(USART_TypeDef *USARTx, char c);
void ComPuts(USART_TypeDef* USARTx, const char* str);
void I2C3_ConfBasic(void);
void LSM303_ACC_ReadWhoAmI(void);
void LSM303_ACC_WriteConfig(void);
void LSM303_ACC_Read(void);
void LSM303_MAG_ReadWhoAmI(void);
void LSM303_MAG_WriteConfig(void);
void LSM303_MAG_Read(void);
void ADC_ConfBasic(void);
void ADC_ConfInterrupt(void);
void ADC_Conf_DMA_2Channels(void);
void ADC_Conf_DMA_2ChannelsMeans(void);

#endif
