#include "ICU.h"

static volatile void (*g_CallBackPtr)(void) = NULL_Ptr;

ISR (TIMER1_CAPT_vect)
{
	if (*g_CallBackPtr != NULL_Ptr)
	(*g_CallBackPtr)();
}


void ICU_Init (ICU_Config_t *config_ptr)
{
	CLEAR_BIT(PORTD,6);										// Set ICU pin as i/p Pin
	TCCR1A = (1<<FOC1A) | (1<<FOC1A);						// Set Timer1 with normal mode & Start Counting
	TCCR1B = (TCCR1B & 0xF8) | (config_ptr -> clock);		// Set Prescaler accroding to the passed prescaler
	TCCR1B = (TCCR1B & 0xBF) | ((config_ptr -> edge)<<6);	// Set the Edge accroding to the passed edge
	TCNT1 = 0;												// Initialize Timer/Counter register with value 0
	ICR1 = 0;												// Initialize ICR register with value 0
	TIMSK |= (1<<TICIE1);									//Enable ICU interrupt at Timer1
}
void ICU_SelectEdge (ICU_Edge_t edge)
{
	TCCR1B = (TCCR1B & 0xBF) | ((edge)<<6);	 // Set the Edge accroding to the passed edge
}
u16 ICU_GetInputCaptureValue (void)
{
	return ICR1;	// Return the value which is inside ICR1
}
void ICU_ClearTimerValue (void)
{
	TCNT1 = 0;	// Clear Timer/Counter register
}
void ICU_SetCallBack (void (*ptr2func)(void))
{
	g_CallBackPtr = ptr2func;	// Assign the address of the passed function to the global pointer function
}
void ICU_deInit (void)
{
	CLEAR_BIT(TCCR1A,FOC1A);	// Stop the counter and de-initialize the Timer
	CLEAR_BIT(TCCR1B,FOC1B);
	TCNT1 = 0;					// Initialize Timer/Counter register with value 0
	ICR1 = 0;					// Initialize ICR register with value 0
	CLEAR_BIT(TIMSK,TICIE1);	// Disable ICU interrupt for Timer1
}
