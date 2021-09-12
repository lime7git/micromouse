#include "Myfun.h"
#include "HD44780.h"
volatile uint32_t tick = 0;
volatile uint8_t lcdpos = 0;
volatile uint16_t lcdvalue = 0;
int h, m, s, ms;

uint8_t whoacc;
int16_t accx = 0;
double accx_g;
int16_t accy = 0;
double accy_g;
int16_t accz = 0;
double accz_g;

uint8_t whomag;
int16_t magx = 0;
double magx_g;
int16_t magy = 0;
double magy_g;
int16_t magz = 0;
double magz_g;

volatile uint16_t val = 0;
volatile uint16_t tab[200];

void delay_ms(uint32_t ms)
{
	tick = 0;
	while(tick < ms);
}
void Led_Conf(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	GPIOC->MODER &= ~GPIO_MODER_MODER6;
	GPIOC->MODER |= GPIO_MODER_MODER6_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER7;
	GPIOC->MODER |= GPIO_MODER_MODER7_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER8;
	GPIOC->MODER |= GPIO_MODER_MODER8_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER9;
	GPIOC->MODER |= GPIO_MODER_MODER9_0;
	GPIOE->MODER &= ~GPIO_MODER_MODER4;
	GPIOE->MODER |= GPIO_MODER_MODER4_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER3;
	GPIOD->MODER |= GPIO_MODER_MODER3_0;
	GPIOE->MODER &= ~GPIO_MODER_MODER5;
	GPIOE->MODER |= GPIO_MODER_MODER5_0;
	GPIOE->MODER &= ~GPIO_MODER_MODER6;
	GPIOE->MODER |= GPIO_MODER_MODER6_0;
}
void Led_OnOff(uint8_t num, eLed state)
{
	switch(num)
	{
		case 0:
			if(state == LedOff)					GPIOC->ODR &= ~GPIO_ODR_OD6;
			else if(state == LedOn)			GPIOC->ODR |= GPIO_ODR_OD6;
			else if(state == LedTog)		GPIOC->ODR ^= GPIO_ODR_OD6;
		break;
		case 1:
			if(state == LedOff)					GPIOC->ODR &= ~GPIO_ODR_OD7;
			else if(state == LedOn)			GPIOC->ODR |= GPIO_ODR_OD7;
			else if(state == LedTog)		GPIOC->ODR ^= GPIO_ODR_OD7;
		break;
		case 2:
			if(state == LedOff)					GPIOC->ODR &= ~GPIO_ODR_OD8;
			else if(state == LedOn)			GPIOC->ODR |= GPIO_ODR_OD8;
			else if(state == LedTog)		GPIOC->ODR ^= GPIO_ODR_OD8;
		break;
		case 3:
			if(state == LedOff)					GPIOC->ODR &= ~GPIO_ODR_OD9;
			else if(state == LedOn)			GPIOC->ODR |= GPIO_ODR_OD9;
			else if(state == LedTog)		GPIOC->ODR ^= GPIO_ODR_OD9;
		break;
		case 4:
			if(state == LedOff)					GPIOE->ODR &= ~GPIO_ODR_OD4;
			else if(state == LedOn)			GPIOE->ODR |= GPIO_ODR_OD4;
			else if(state == LedTog)		GPIOE->ODR ^= GPIO_ODR_OD4;
		break;
		case 5:
			if(state == LedOff)					GPIOD->ODR &= ~GPIO_ODR_OD3;
			else if(state == LedOn)			GPIOD->ODR |= GPIO_ODR_OD3;
			else if(state == LedTog)		GPIOD->ODR ^= GPIO_ODR_OD3;
		break;
		case 6:
			if(state == LedOff)					GPIOE->ODR &= ~GPIO_ODR_OD5;
			else if(state == LedOn)			GPIOE->ODR |= GPIO_ODR_OD5;
			else if(state == LedTog)		GPIOE->ODR ^= GPIO_ODR_OD5;
		break;
		case 7:
			if(state == LedOff)					GPIOE->ODR &= ~GPIO_ODR_OD6;
			else if(state == LedOn)			GPIOE->ODR |= GPIO_ODR_OD6;
			else if(state == LedTog)		GPIOE->ODR ^= GPIO_ODR_OD6;
		break;
	}
}
void SysTick_Handler(void)
{
	tick++;
	Led7seg_WriteNumber();
	Timer();
}
void Joy_Conf(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	GPIOE->MODER &= ~GPIO_MODER_MODE0;
	GPIOE->MODER &= ~GPIO_MODER_MODE1;
	GPIOE->MODER &= ~GPIO_MODER_MODE2;
	GPIOE->MODER &= ~GPIO_MODER_MODE3;
	GPIOE->MODER &= ~GPIO_MODER_MODE15;
}
eJoy Joy_Read(void)
{
	eJoy state = JoyNull;
	if((GPIOE->IDR & GPIO_IDR_ID0) == RESET) state = Right;
	if((GPIOE->IDR & GPIO_IDR_ID1) == RESET) state = Left;
	if((GPIOE->IDR & GPIO_IDR_ID2) == RESET) state = Down;
	if((GPIOE->IDR & GPIO_IDR_ID3) == RESET) state = Up;
	if((GPIOE->IDR & GPIO_IDR_ID15) == RESET) state = Center;
	return state;
}
void Led7seg_Conf(void)
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
	PWR->CR2 |= PWR_CR2_IOSV;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
	GPIOB->MODER &= ~GPIO_MODER_MODE2;
	GPIOB->MODER |= GPIO_MODER_MODE2_0;
	GPIOB->MODER &= ~GPIO_MODER_MODE3;
	GPIOB->MODER |= GPIO_MODER_MODE3_0;
	GPIOB->MODER &= ~GPIO_MODER_MODE4;
	GPIOB->MODER |= GPIO_MODER_MODE4_0;
	GPIOB->MODER &= ~GPIO_MODER_MODE5;
	GPIOB->MODER |= GPIO_MODER_MODE5_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE0;
	GPIOG->MODER |= GPIO_MODER_MODE0_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE1;
	GPIOG->MODER |= GPIO_MODER_MODE1_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE2;
	GPIOG->MODER |= GPIO_MODER_MODE2_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE3;
	GPIOG->MODER |= GPIO_MODER_MODE3_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE4;
	GPIOG->MODER |= GPIO_MODER_MODE4_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE5;
	GPIOG->MODER |= GPIO_MODER_MODE5_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE6;
	GPIOG->MODER |= GPIO_MODER_MODE6_0;
	GPIOG->MODER &= ~GPIO_MODER_MODE9;
	GPIOG->MODER |= GPIO_MODER_MODE9_0;
}
void Led7seg_WriteDigit(uint8_t pos, uint8_t num)
{
	GPIOB->ODR &= ~GPIO_ODR_OD2;
	GPIOB->ODR &= ~GPIO_ODR_OD3;
	GPIOB->ODR &= ~GPIO_ODR_OD4;
	GPIOB->ODR &= ~GPIO_ODR_OD5;
	GPIOG->ODR &= ~GPIO_ODR_OD0;
	GPIOG->ODR &= ~GPIO_ODR_OD1;
	GPIOG->ODR &= ~GPIO_ODR_OD2;
	GPIOG->ODR &= ~GPIO_ODR_OD3;
	GPIOG->ODR &= ~GPIO_ODR_OD4;
	GPIOG->ODR &= ~GPIO_ODR_OD5;
	GPIOG->ODR &= ~GPIO_ODR_OD6;
	GPIOG->ODR &= ~GPIO_ODR_OD9;
	
	switch(pos)
	{
		case 0:	GPIOB->ODR |= GPIO_ODR_OD5;	break;
		case 1:	GPIOB->ODR |= GPIO_ODR_OD4;	break;
		case 2:	GPIOB->ODR |= GPIO_ODR_OD3;	break;
		case 3:	GPIOB->ODR |= GPIO_ODR_OD2;	break;
	}
	switch(num)
	{
		case 0:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_1 | GPIO_ODR_ODR_2 | 
										GPIO_ODR_ODR_3 | GPIO_ODR_ODR_4 | GPIO_ODR_ODR_5;
			break;
		case 1:
			GPIOG->ODR |= GPIO_ODR_ODR_1 | GPIO_ODR_ODR_2;
			break;
		case 2:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_1 | GPIO_ODR_ODR_3 |
										GPIO_ODR_ODR_4 | GPIO_ODR_ODR_6;
			break;
		case 3:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_1 | GPIO_ODR_ODR_2 |
										GPIO_ODR_ODR_3 | GPIO_ODR_ODR_6;
			break;
		case 4:
			GPIOG->ODR |= GPIO_ODR_ODR_1 | GPIO_ODR_ODR_2 | GPIO_ODR_ODR_5 |
										GPIO_ODR_ODR_6;
			break;
		case 5:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_2 | GPIO_ODR_ODR_3 |
										GPIO_ODR_ODR_5 | GPIO_ODR_ODR_6;
			break;
		case 6:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_2 | GPIO_ODR_ODR_3 |
										GPIO_ODR_ODR_4 | GPIO_ODR_ODR_5 | GPIO_ODR_ODR_6;
			break;
		case 7:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_1 | GPIO_ODR_ODR_2;
			break;
		case 8:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_1 | GPIO_ODR_ODR_2 |
										GPIO_ODR_ODR_3 | GPIO_ODR_ODR_4 | GPIO_ODR_ODR_5 |
										GPIO_ODR_ODR_6;
			break;
		case 9:
			GPIOG->ODR |= GPIO_ODR_ODR_0 | GPIO_ODR_ODR_1 | GPIO_ODR_ODR_2 |
										GPIO_ODR_ODR_3 | GPIO_ODR_ODR_5 | GPIO_ODR_ODR_6;
			break;
	}
}
void Led7seg_WriteNumber(void)
{	
	uint8_t tab[4];
	tab[0] = (lcdvalue % 10) / 1;
	tab[1] = (lcdvalue % 100) / 10;
	tab[2] = (lcdvalue % 1000) / 100;
	tab[3] = (lcdvalue % 10000) / 1000;
	Led7seg_WriteDigit(lcdpos, tab[lcdpos]);
	lcdpos++;
	if(lcdpos >= 4)
		lcdpos = 0;
}
void Timer(void)
{
	ms++;
	if(ms >= 1000)	{ms = 0; s++;}
	if(s >= 60)			{s = 0; m++;}
	if(m >= 60)			{m = 0; h++;}
	if(h >= 24)			{h = 0;}
}
void ShowTime(void)
{
	LCD_Clear();
	LCD_WriteNumber(h);
	LCD_WriteText(":");
	LCD_WriteNumber(m);
	LCD_WriteText(":");
	LCD_WriteNumber(s);
	LCD_WriteText(".");
	LCD_WriteNumber(ms);
}
void LPUART1_Conf_Basic(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
	
	GPIOC->MODER &= ~GPIO_MODER_MODER0 & ~GPIO_MODER_MODER1;
	GPIOC->MODER |= GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1;
	GPIOC->AFR[0] |= 0x00000088;
	
	LPUART1->BRR = (256 * 4000000) / 57600;
	LPUART1->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}
void ComSendChar(USART_TypeDef *USARTx, char c)
{
	while((USARTx->ISR & USART_ISR_TXE) == RESET){;}
	USARTx->TDR = c;
}
void ComPuts(USART_TypeDef* USARTx, const char* str)
{
	while(*str)
		ComSendChar(USARTx, *str++);
}
void I2C3_ConfBasic(void)
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
	PWR->CR2 |= PWR_CR2_IOSV;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C3EN;
	GPIOG->MODER &= ~GPIO_MODER_MODE8 & ~GPIO_MODER_MODE7;
	GPIOG->MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE7_1;
	GPIOG->AFR[0] = 0x40000000;
	GPIOG->AFR[1] |= 0x00000004;
	I2C3->TIMINGR = 0x04;
	I2C3->CR1 = I2C_CR1_PE;
}
void LSM303_ACC_ReadWhoAmI(void)
{
	I2C3->CR2 = (1 << I2C_CR2_NBYTES_Pos) | (0x1d << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) {;}
	I2C3->TXDR = 0x0f;
	while((I2C3->ISR & I2C_ISR_TC) == RESET) {;}
	I2C3->CR2 = (1 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | (0x1d << 	(I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_RXNE) == RESET) {;}
	whoacc = I2C3->RXDR;
}
void LSM303_ACC_WriteConfig(void)
{
	I2C3->CR2 = (2 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | (0x1d << 	(I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x20;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x67;
	
	I2C3->CR2 = (2 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | (0x1d << 	(I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x23;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x0c;
}
void LSM303_ACC_Read(void)
{
	uint8_t tab[6];
	I2C3->CR2 = (1 << I2C_CR2_NBYTES_Pos) | (0x1d << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) {;}
	I2C3->TXDR = 0x28;
	while((I2C3->ISR & I2C_ISR_TC) == RESET) {;}
	I2C3->CR2 = (6 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | (0x1d << 	(I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START;
	for(int i=0;i<6;i++)
	{
		while((I2C3->ISR & I2C_ISR_RXNE) == RESET) {;}
		tab[i] = I2C3->RXDR;
	}
		
	accx = (int16_t)(((uint16_t)tab[1] << 8) + ((uint16_t)tab[0] << 0));
	accx_g = (double)accx / 16384.0;
	accy = (int16_t)(((uint16_t)tab[3] << 8) + ((uint16_t)tab[2] << 0));
	accy_g = (double)accx / 16384.0;
	accz = (int16_t)(((uint16_t)tab[5] << 8) + ((uint16_t)tab[4] << 0));
	accz_g = (double)accx / 16384.0;
}
void LSM303_MAG_ReadWhoAmI(void)
{
	I2C3->CR2 = (1 << I2C_CR2_NBYTES_Pos) | (0x1e << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) {;}
	I2C3->TXDR = 0x0f;
	while((I2C3->ISR & I2C_ISR_TC) == RESET) {;}
	I2C3->CR2 = (1 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | (0x1e << 	(I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_RXNE) == RESET) {;}
	whomag = I2C3->RXDR;
}
void LSM303_MAG_WriteConfig(void)
{
	I2C3->CR2 = (5 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | (0x1e << 	(I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x20;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x7c;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x60;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x00;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) ;
	I2C3->TXDR = 0x0c;
}
void LSM303_MAG_Read(void)
{
	uint8_t tab[6];
	I2C3->CR2 = (1 << I2C_CR2_NBYTES_Pos) | (0x1e << (I2C_CR2_SADD_Pos + 1)) | I2C_CR2_START;
	while((I2C3->ISR & I2C_ISR_TXIS) == RESET) {;}
	I2C3->TXDR = 0x28;
	while((I2C3->ISR & I2C_ISR_TC) == RESET) {;}
	I2C3->CR2 = (6 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | (0x1e << 	(I2C_CR2_SADD_Pos + 1)) | I2C_CR2_RD_WRN | I2C_CR2_START;
	for(int i=0;i<6;i++)
	{
		while((I2C3->ISR & I2C_ISR_RXNE) == RESET) {;}
		tab[i] = I2C3->RXDR;
	}

	magx = (int16_t)(((uint16_t)tab[1] << 8) + ((uint16_t)tab[0] << 0));
	magx_g = (double)magx / 2048.0;
	magy = (int16_t)(((uint16_t)tab[3] << 8) + ((uint16_t)tab[2] << 0));
	magy_g = (double)magx / 2048.0;
	magz = (int16_t)(((uint16_t)tab[5] << 8) + ((uint16_t)tab[4] << 0));
	magz_g = (double)magx / 2048.0;
}
void ADC_ConfBasic(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_ADCEN;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL;
	GPIOC->MODER |= GPIO_MODER_MODER2;
	ADC3->CR = ADC_CR_ADEN | ADC_CR_ADVREGEN;
	ADC3->CFGR |= ADC_CFGR_DISCEN;
	ADC3->SQR1 |= (3<< ADC_SQR1_SQ1_Pos);
	ADC3->SMPR1 |= ADC_SMPR1_SMP3;
	ADC3->ISR |= ADC_ISR_EOC;
	ADC3->CR |= ADC_CR_ADSTART;
}
void ADC_ConfInterrupt(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN | RCC_AHB2ENR_ADCEN;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL;
	GPIOF->MODER |= GPIO_MODER_MODER3;
	ADC3->CR = ADC_CR_ADEN | ADC_CR_ADVREGEN;
	ADC3->SQR1 |= (6<<ADC_SQR1_SQ1_Pos);
	ADC3->SMPR1 |= ADC_SMPR1_SMP6;
	ADC3->CFGR |= ADC_CFGR_CONT;
	ADC3->IER |= ADC_IER_EOCIE;
	NVIC_EnableIRQ(ADC3_IRQn);
	ADC3->CR |= ADC_CR_ADSTART;
}
void ADC3_IRQHandler(void)
{
	if((ADC3->ISR & ADC_ISR_EOC) != RESET)
	{
		val = ADC3->DR;
	}
}
void ADC_Conf_DMA_2Channels(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_ADCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL;

	GPIOC->MODER |= GPIO_MODER_MODER2;
	
	DMA1_Channel3->CPAR = (uint32_t)&ADC3->DR;
	DMA1_Channel3->CMAR = (uint32_t)tab;
	DMA1_Channel3->CNDTR = (uint16_t)2;
	DMA1_CSELR->CSELR = (0 << DMA_CSELR_C3S_Pos);
	DMA1_Channel3->CCR |= DMA_CCR_CIRC | DMA_CCR_MINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 | DMA_CCR_EN;
	
	ADC123_COMMON->CCR |= ADC_CCR_TSEN;
	ADC3->CR = ADC_CR_ADEN | ADC_CR_ADVREGEN;
	ADC3->SQR1 = (1<<ADC_SQR1_L_Pos) | (3<<ADC_SQR1_SQ1_Pos) | (17<<ADC_SQR1_SQ2_Pos);
	ADC3->SMPR1 = ADC_SMPR1_SMP3;
	ADC3->SMPR2 = ADC_SMPR2_SMP17;
	ADC3->CFGR |= ADC_CFGR_DMACFG | ADC_CFGR_DMAEN |	ADC_CFGR_CONT;
	ADC3->CR |= ADC_CR_ADSTART;
}
void ADC_Conf_DMA_2ChannelsMeans(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_ADCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL;

	GPIOC->MODER |= GPIO_MODER_MODER2;
	
	DMA1_Channel3->CPAR = (uint32_t)&ADC3->DR;
	DMA1_Channel3->CMAR = (uint32_t)tab;
	DMA1_Channel3->CNDTR = (uint16_t)200;
	DMA1_CSELR->CSELR = (0 << DMA_CSELR_C3S_Pos);
	DMA1_Channel3->CCR |= DMA_CCR_CIRC | DMA_CCR_MINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 | DMA_CCR_EN;
	
	ADC123_COMMON->CCR |= ADC_CCR_TSEN;
	ADC3->CR = ADC_CR_ADEN | ADC_CR_ADVREGEN;
	ADC3->SQR1 = (1<<ADC_SQR1_L_Pos) | (3<<ADC_SQR1_SQ1_Pos) | (17<<ADC_SQR1_SQ2_Pos);
	ADC3->SMPR1 = ADC_SMPR1_SMP3;
	ADC3->SMPR2 = ADC_SMPR2_SMP17;
	ADC3->CFGR |= ADC_CFGR_DMACFG | ADC_CFGR_DMAEN |	ADC_CFGR_CONT;
	ADC3->CR |= ADC_CR_ADSTART;
}
