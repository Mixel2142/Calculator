#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/sleep.h>

#define On_C 0b00011000
#define Plus 0b00010001
#define Sub  0b00100001
#define Did  0b10000001
#define Mul  0b01000001
#define Equ  0b00010010
#define k0	 0b00010100
#define k1   0b00101000
#define k2 	 0b00100100
#define k3 	 0b00100010
#define k4 	 0b01001000
#define k5 	 0b01000100
#define k6 	 0b01000010
#define k7   0b10001000
#define k8   0b10000100
#define k9   0b10000010
#define Nun  0b00000000

uint8_t pressKey = Nun; //ничего не нажато
uint8_t keyDelay = 2;//60;   //кол-во итераций в ISR которое нужно для срабатывания нажатия клавиши
uint8_t keyCounter = 0;  //счётчик кол-ва нажатия в ISR

uint16_t powerCounter = 0;
uint16_t powerStop = 10000;//244*120;// 2минуты
bool power = false;

#define NumOne true
#define NumTwo false
bool writeTo = NumOne;

bool inf = false;
int32_t numberOne = 0;
int32_t numberTwo = 0;
/*
int32_t from  -2'147'483'648 to 2'147'483'647
*/
#define firstZnakoMesto		0b00000001
#define secondZnakoMesto		0b00000010
#define thirdZnakoMesto		0b00000100
#define fourthZnakoMesto		0b00001000
#define go2PreviusZnakoMesto	PORTB = PORTB >> 1 
#define currentZnakoMesto		PORTB

#define i 0b00000100
#define n 0b01010100
#define f 0b01110001
#define p 0b01110011
#define e 0b01111001
#define q 0b01100111
#define u 0b00111110
#define m 0b00110111
#define l 0b00111000
#define d 0b01011110
#define s 0b01101101
#define b 0b01111100
#define minus 0b01000000

#define startPositionPortB 0b00001000

#define delaySign 300

uint8_t dispVal[4] = {Nun,Nun,Nun,0b00111111};


int32_t mod(int32_t num)
{
	return num < 0 ? num * (-1) : num;
}


uint8_t getLength(int32_t num)
{
	uint8_t result = 0;
	while( num != 0)
	{	
		num /= 10;
		result++;
	}
	return result;
}

inline bool isKeyPres()
{
	return (PINB > 0b00001111);
} 

bool isKeyPres(uint8_t key)
{
	return (PINB == key);
}

uint8_t setPortD(uint8_t num)
{
	switch (num)
	{
		case 0:
			return	0b00111111;
			break;
		case 1:
			return	0b00000110;
			break;
		case 2:
			return	0b01011011;
			break;
		case 3:
			return	0b01001111;
			break;
		case 4:
			return	0b01100110;
			break;
		case 5:
			return	0b01101101;
			break;
		case 6:
			return	0b01111101;
			break;
		case 7:
			return	0b00100111;
			break;
		case 8:
			return	0b01111111;
			break;
		case 9:
			return	0b01101111;
			break;
		default:
			return	0b00000001;
	}
}

void drawDigits()
{
	switch (PORTB)
	{
		case fourthZnakoMesto:
			PORTD = dispVal[0];
			break;

		case thirdZnakoMesto:
			PORTD = dispVal[1];
			break;

		case secondZnakoMesto:
			PORTD = dispVal[2];
			break;

		case firstZnakoMesto:
			PORTD = dispVal[3];
			break;

		default:
			break;
	}
}


int32_t pow_dec(int32_t num, uint8_t pw)
{
	uint8_t counter = 1;
	while(pw != counter)
	{
		num *= 10;
		counter++;
	}
	return pw == 0 ? 1 : num;
}

void makeDisplayValue()
{

	dispVal[0] = Nun;
	dispVal[1] = Nun;
	dispVal[2] = Nun;
	dispVal[3] = 0b00111111;

	if(writeTo)//NumOne
	{
		if(inf)
		{
			dispVal[0] = Nun;
			dispVal[1] = i;
			dispVal[2] = n;
			dispVal[3] = f;
		}
		else
		{
			if(numberOne < 0)
			{
				if(getLength(numberOne) < 4)
				{
					for(uint8_t j = 0; j < getLength(numberOne); j++)
					{
						dispVal[3-j] = setPortD(mod(numberOne/pow_dec(10,j)%10));
					}
					dispVal[3-getLength(numberOne)] = minus;
				}
				else
				{
					dispVal[3] = setPortD(getLength(numberOne)-1);
					dispVal[2] = e;
					dispVal[1] = setPortD(mod(numberOne/pow_dec(10,getLength(numberOne)-1)%10));
					dispVal[0] = minus;
				}
			}//OK
			else
			{
			
				if(getLength(numberOne) <= 4)
				{
				
					for(uint8_t j = 0; j < getLength(numberOne); j++)
					{
						dispVal[3-j] = setPortD(numberOne/pow_dec(10,j)%10);
					}
				
				}
				else
				{	
					dispVal[3] = setPortD(getLength(numberOne)-1);
					dispVal[2] = e;
					dispVal[1] = setPortD(numberOne/pow_dec(10,getLength(numberOne)-1)%10);
					dispVal[0] = Nun;
				}
			}//OK
		}
	}
	else//NumTwo
	{
		for(uint8_t j = 0; j < getLength(numberTwo); j++)
		{
			dispVal[3-j] = setPortD(numberTwo/pow_dec(10,j)%10);
		}
	}
}

ISR(PCINT0_vect)// wake up
{


}

ISR(TIMER0_COMPA_vect)//Прерывание по сравнению, канал A таймера/счетчика 0 КЛАВИАТУРА
{
	if(power)
	{	
		go2PreviusZnakoMesto;
		if(currentZnakoMesto < firstZnakoMesto) currentZnakoMesto = fourthZnakoMesto; // поочередное открывание ключей
		
		drawDigits();
		
		if(isKeyPres())
		{
			keyCounter++;
			powerCounter = 0;
		}
		else
		{
			powerCounter++;
			if(powerCounter > powerStop)
			{
				powerCounter = 0;
				keyCounter = 0;
				power = false;
				PORTD = Nun;
				PORTB = startPositionPortB;
			}
		}

		if(keyCounter > keyDelay)
		{
			pressKey = PINB; 
			keyCounter = 0;
		}
	}
	else
	{
		if(isKeyPres(On_C))
		{
			keyCounter++;
		}
		if(keyCounter > keyDelay)
		{
			keyCounter = 0;
			power = true;
		}
	
	}
}

void key_disp_timer_init()
{
	TCCR0A  = _BV(WGM01);	//включили CTC
	TCCR0B |= _BV(CS00);
	TCCR0B |= _BV(CS02);	// Установил делитель на 1024 // 8 MHz / 1024 = 7'812Hz
	OCR0A   = 80;   		// Установил делитель на 32 // 8 MHz / 32 = 244Hz ~ 60 меганий на 1 секцию диодов
	TIMSK0  = _BV(OCIE0A);	// Включил работу с TCCR0B
	sei();					// разрешил прерывания
}

int main()
{
	ADCSRA &= ~_BV(ADEN); // Отключаем АЦП
	//MCUCR = _BV(BODS) | _BV(BODSE);// Отключаем детектор пониженного напряжения питания

	DDRD  = 0b11111111;// порты D 0-7  в режиме вывода (для дисплея)
	PORTD = 0b00000000;
	DDRB  = 0b11111111;// порты B 0-3 в режиме вывода|порты B 4-7 в режиме ввода(для клавы) 
	PORTB = startPositionPortB;
	//set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Устанавливаем интересующий нас режим
	//sleep_mode(); // Переводим МК в спящий режим
	
	uint8_t sign = Nun; // запоминает нужное действие (+-=:*)
		
	key_disp_timer_init();
	
	while(1)
	{
		if(power)
		{
			if(pressKey)
			{
				switch (pressKey)
				{
					case On_C:
						if(sign == Equ)
						{
							inf	= false;
							numberTwo = 0;
							numberOne = 0;
							writeTo = NumOne;
							sign = Nun;
						}
						
						if(writeTo)//NumOne
						{
							numberOne = numberOne/10;
						}
						else//NumTwo
						{
							numberTwo = numberTwo/10;
						}
						break;

					case Plus:
						writeTo = !writeTo;
						sign = Plus;
						dispVal[0] = p;
						dispVal[1] = l;
						dispVal[2] = u;
						dispVal[3] = s;
						_delay_ms(delaySign);
						break;

					case Sub:
						writeTo = !writeTo;
						sign = Sub;
						dispVal[0] = Nun;
						dispVal[1] = s;
						dispVal[2] = u;
						dispVal[3] = b;
						_delay_ms(delaySign);
						break;

					case Did:
						writeTo = !writeTo;
						sign = Did;
						dispVal[0] = Nun;
						dispVal[1] = d;
						dispVal[2] = i;
						dispVal[3] = d;
						_delay_ms(delaySign);
						break;

					case Mul:
						writeTo = !writeTo;
						sign = Mul;
						dispVal[0] = m;
						dispVal[1] = m;
						dispVal[2] = u;
						dispVal[3] = l;
						_delay_ms(delaySign);
						break;

					case Equ:
						writeTo = NumOne;
						dispVal[0] = Nun;
						dispVal[1] = e;
						dispVal[2] = q;
						dispVal[3] = u;
						_delay_ms(delaySign);
						switch (sign)
						{
						case Plus:
							numberOne += numberTwo;
							break;

						case Sub:
							numberOne -= numberTwo;
							break;

						case Mul:
							numberOne *= numberTwo;
							break;

						case Did:
							if(numberTwo == 0)
							{
								inf = true;
								numberOne = 0;
								numberTwo = 0;
							}
							else numberOne /= numberTwo;
							break;

						default:
							break;
						}
						sign = Equ;
						numberTwo = 0;
						break;

					case k0:
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10;
						}
						break;

					case k1: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 1;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 1;
						}
						break;

					case k2:
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 2;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 2;
						}
						break; 

					case k3: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 3;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 3;
						}
						break;

					case k4: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 4;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 4;
						}
						break;

					case k5: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 5;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 5;
						}
						break;

					case k6: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 6;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 6;
						}
						break;

					case k7: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 7;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 7;
						}
						break;

					case k8: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 8;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 8;
						}
						break;

					case k9: 
						if(writeTo)//NumOne
						{
							numberOne = numberOne*10 + 9;
						}
						else//NumTwo
						{
							numberTwo = numberTwo*10 + 9;
						}
						break;

					default:
						break;
				}
				
				pressKey = Nun;
				if(numberTwo > 9999)numberTwo %= 10000;
				makeDisplayValue();
				if(numberOne > 9999)
				{
					if(sign == Equ)
					{
						makeDisplayValue();
						numberOne = 0;
					}
					else
					{
						numberOne %= 10000;
						makeDisplayValue();
					}
				}
				

			}//if pressKey
		}
		else
		{
				
		}
	
		
	}

	
}

