#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>


void key_disp_timer_init()
{
	TCCR0A = (1 << WGM01);
	TCCR0B |= (1 << CS02)|(1 << CS00);// Установил делитель на 1024 // 16 MHz / 1024 = 15'625Hz
	OCR0A = 249;
	TIMSK0 = (1 << OCIE0A); // Включил работу с TCCR0B
	sei();
}

ISR(TIMER0_COMPA_vect)
{
	PORTB = 0b01010101;
	PORTD =	0b11110000;	
}

int main(void)
{
	
	DDRD  = 0b11111111;
	PORTD = 0b10101010;
	DDRB  = 0b00001111;
	PORTB = 0b00000001;

	void key_disp_timer_init();
	
	while(1);
}