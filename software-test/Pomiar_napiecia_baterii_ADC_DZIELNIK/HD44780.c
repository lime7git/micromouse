#include "HD44780.h"
volatile uint32_t lcdtick1 = 0;
static void LCD_delay50us(void)
{
	lcdtick1 = 0;
	while(lcdtick1 < 1);
}
static void LCD_delayms(uint32_t ms)
{
	lcdtick1 = 0;
	while(lcdtick1 < (20*ms));
}
static void LCD_Setbits(uint8_t bits)
{
	if(bits & 0x01)		LCD_DB4_HIGH;
	else							LCD_DB4_LOW;

	if(bits & 0x02)		LCD_DB5_HIGH;
	else							LCD_DB5_LOW;

	if(bits & 0x04)		LCD_DB6_HIGH;
	else							LCD_DB6_LOW;

	if(bits & 0x08)		LCD_DB7_HIGH;
	else							LCD_DB7_LOW;
}
static void LCD_Write(uint8_t data)
{
	LCD_E_HIGH;
	LCD_Setbits(data >> 4);
	LCD_E_LOW;
	
	LCD_E_HIGH;
	LCD_Setbits(data);
	LCD_E_LOW;
	
	LCD_delay50us();
}
static void LCD_WriteCommand(uint8_t commandToWrite)
{
	LCD_RS_LOW;
	LCD_Write(commandToWrite);
}
static void LCD_WriteData(uint8_t dataToWrite)
{
	LCD_RS_HIGH;
	LCD_Write(dataToWrite);
}
void LCD_WriteText(char* text)
{
	while(*text)
	  LCD_WriteData(*text++);
}
void LCD_GoTo(uint8_t x, uint8_t y)
{
	LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}
void LCD_Clear(void)
{
	LCD_WriteCommand(HD44780_CLEAR);
	LCD_delayms(2);
}
void LCD_Home(void)
{
	LCD_WriteCommand(HD44780_HOME);
	LCD_delayms(2);
}
void LCD_Init(void)
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIODEN | RCC_AHB2ENR_GPIOEEN | RCC_AHB2ENR_GPIOFEN;
	
	TIM6->CNT = 0;
	TIM6->PSC = 4-1;
	TIM6->ARR = 50-1; // przerwanie co 50 us
	TIM6->DIER |= TIM_DIER_UIE;
	TIM6->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	
	LCD_RS_GPIO->MODER &= ~LCD_RS_MODER_R;
	LCD_RS_GPIO->MODER |= LCD_RS_MODER;
	LCD_RS_GPIO->PUPDR |= LCD_RS_PUPDR;
	
	LCD_RW_GPIO->MODER &= ~LCD_RW_MODER_R;
	LCD_RW_GPIO->MODER |= LCD_RW_MODER;
	LCD_RW_GPIO->PUPDR |= LCD_RW_PUPDR;
	
	LCD_E_GPIO->MODER &= ~LCD_E_MODER_R;
	LCD_E_GPIO->MODER |= LCD_E_MODER;
	LCD_E_GPIO->PUPDR |= LCD_E_PUPDR;
	
	LCD_DB4_GPIO->MODER &= ~LCD_DB4_MODER_R;
	LCD_DB4_GPIO->MODER |= LCD_DB4_MODER;
	LCD_DB4_GPIO->PUPDR |= LCD_DB4_PUPDR;
	
	LCD_DB5_GPIO->MODER &= ~LCD_DB5_MODER_R;
	LCD_DB5_GPIO->MODER |= LCD_DB5_MODER;
	LCD_DB5_GPIO->PUPDR |= LCD_DB5_PUPDR;
	
	LCD_DB6_GPIO->MODER &= ~LCD_DB6_MODER_R;
	LCD_DB6_GPIO->MODER |= LCD_DB6_MODER;
	LCD_DB6_GPIO->PUPDR |= LCD_DB6_PUPDR;
	
	LCD_DB7_GPIO->MODER &= ~LCD_DB7_MODER_R;
	LCD_DB7_GPIO->MODER |= LCD_DB7_MODER;
	LCD_DB7_GPIO->PUPDR |= LCD_DB7_PUPDR;
	
	LCD_BKL_GPIO->MODER &= ~LCD_BKL_MODER_R;
	LCD_BKL_GPIO->MODER |= LCD_BKL_MODER;
	LCD_BKL_GPIO->PUPDR |= LCD_BKL_PUPDR;
	
	LCD_delayms(15);
	LCD_RS_LOW;
	LCD_RW_LOW;
	LCD_E_LOW;
	LCD_BKL_HIGH;

	for(uint8_t i=0; i<3; i++)
	{
		LCD_E_HIGH;
		LCD_Setbits(0x03);
		LCD_E_LOW;
		LCD_delayms(5);
	}

	LCD_E_HIGH;
	LCD_Setbits(0x02);
	LCD_E_LOW;

	LCD_delayms(1);
	LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
	LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wy³¹czenie wyswietlacza
	LCD_WriteCommand(HD44780_CLEAR); // czyszczenie zawartosæi pamieci DDRAM
	LCD_delayms(2);
	LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// inkrementacja adresu i przesuwanie kursora
	LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // w³¹cz LCD, bez kursora i mrugania
}
void TIM6_DAC_IRQHandler(void)
{
	if((TIM6->SR & TIM_SR_UIF) != RESET)
	{
		lcdtick1++;
		TIM6->SR &= ~TIM_SR_UIF;
	}
}
void LCD_WriteNumber(int value)
{
	char buf[16];
	sprintf(buf, "%d" ,value);
	LCD_WriteText(buf);
}
