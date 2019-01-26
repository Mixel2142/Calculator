#include <avr/io.h>
#include <avr/interrupt.h>

int keyDelay = 30;   //кол-во итераций в ISR которое нужно для срабатывания нажатия клавиши
int keyCounter = 0;  //счётчик кол-ва нажатия в ISR
 
 
 bool isKeyPres()
{
return ((~PINB) > 0b00001111);
} 


ISR(TIMER0_COMPA_vect)
{
	PORTB = PORTB >> 1;
	if(PORTB <= 0b00000000) PORTB = 0b00001000;

	unsigned char prk = PINB;
	
	if(isKeyPres())
	{
		keyCounter++;
	}
	else 
	{
		keyCounter = 0;
	}//endif
	
	if(keyCounter > keyDelay)
	{
		PORTD = ~(PINB | 0b00001111);// приравниваю у  порту Д, чтобы визуально было видно,что работает
		/*
		Примерно должно выглядеть, как 0b00100010.
		А получается, что 1 бегает от  0b00001000 до 0b00000001

		Если заменить isKeyPres() на true, то на порт Д выводится всё как должно, но оно должно выводиться во после нажатия кнопки.
		*/
		
		//pressKey = ~(PINB | 0b00001111);	
		keyCounter = 0;
	}

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