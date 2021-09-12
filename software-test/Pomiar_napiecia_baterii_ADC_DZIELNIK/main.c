#include "Myfun.h"
#include "HD44780.h"

extern int16_t val;

double 	vbat, 
				vmeasure, 
				voltage_divider = 2.9914;


char buf[50];


int main(void)
{
	SysTick_Config(4000000 / 1000);

	ADC_ConfInterrupt();
	LCD_Init();
	Led_Conf();
	
	while(1)
	{
		
		vmeasure = ( (double)val / 4095.0 ) * 3.3;
		vbat = vmeasure * voltage_divider;
		
		LCD_Clear();
		sprintf(buf, "Vadc = %.2f [V]", vmeasure);
		LCD_WriteText(buf);
		LCD_GoTo(0,1);
		sprintf(buf, "Vbat = %.2f [V]", vbat);
		LCD_WriteText(buf);
		
		Led_OnOff(0,LedTog);
		delay_ms(250);
		
	}
}
