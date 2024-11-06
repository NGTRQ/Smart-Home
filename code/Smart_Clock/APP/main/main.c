#include <avr/io.h>
#include <avr/interrupt.h>
#include "../../HAL/LCD/LCD.h"
#include "../../MCAL/DIO/DIO.h"
#include "../../MCAL/ADC/ADC.h"
#include "../../MCAL/UART/UART.h"
#include "../../UTLIS/Std_Types.h"
#include "../../UTLIS/Common_Macros.h"

void Timer0_init(void);
volatile u8 g_interruptCounter = 0;
volatile u8 seconds = 0;
ISR (TIMER0_OVF_vect)
{
	g_interruptCounter++;
	if (g_interruptCounter == 4)	//	one second
	{
		seconds++;
		g_interruptCounter = 0;
		TCNT0 = 7;	//Initialize Timer/Counter register with value 7
	}
}

int main ()
{
	u8 minuts = 29;
	u8 hours = 17;
	u16 adc_value;
	u8 Temperature;
	u16 ADC_Reading;
	u16 mv_Value;
	u16 LDR_Value;
	ADC_Init();
	USART_Init();
	LCD_Init();
	Timer0_init();
	SET_BIT(SREG,7);	//Enable I-Bit

	while (1)
	{

		// Clock Code
		if (seconds == 60)
		{
			LCD_CLEAR();
			seconds = 0;
			minuts++;
			if (minuts == 60)
			{
				LCD_CLEAR();
				minuts = 0;
				hours++;
				if (hours == 24)
				{
					hours = 0;
				}
			}
		}
		Go_To_Row_Col(0,0);
		Display_Str("Time:  ");
		Integer_TO_String(hours);
		Display_Str(":");
		Integer_TO_String(minuts);
		Display_Str(":");
		Integer_TO_String(seconds);

		adc_value = ADC_readChannel(1);		// read adc value at PA0
		Temperature = adc_value / 2.01;   	// finding the temperature
		Go_To_Row_Col(1,0);
		Display_Str("Temp:  ");   			//Display temperature
		Integer_TO_String(Temperature);
		Display_Str(" C     ");

		if(Temperature > 30 && Temperature > 50)
		{
			USART_sendByte('H');	// Alarm On
		}
		else if(Temperature > 30)
		{
			USART_sendByte('F');	// Fan On
		}
		else
		{
			USART_sendByte('X');
		}

		// LDR Code
		ADC_Reading = ADC_readChannel(0);
		mv_Value=(ADC_Reading * 5000)/256;
		LDR_Value = mv_Value * 3.8;

		if(LDR_Value < 400)
		{
			USART_sendByte('L');	// Led On
		}
		else
		{
			USART_sendByte('X');
		}
	}
	return 0;
}

void Timer0_init(void)
{
	TCNT0 = 7;				// Initialize Timer/Counter register with value 7;
	SET_BIT(TIMSK,TOIE0);	// Enable Overflow mode interrupt
	SET_BIT(TCCR0,CS02);	// PreScaler 1024
	SET_BIT(TCCR0,CS00);
	SET_BIT(TCCR0,FOC0);	//  Set Timer0 with normal mode & Start Counting
}
