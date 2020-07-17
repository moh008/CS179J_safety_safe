/*
 * Project01.c
 *
 * Created: 7/17/2020 3:29:08 AM
 * Author : moker
 */ 

#include <avr/io.h>

void set_PWM(double frequency){
	static double current_frequency;
	if (frequency != current_frequency){
		if (!frequency){
			TCCR3B &= 0x08;
		}
		else {
			TCCR3B |= 0x03;
		}

		if (frequency < 0.954){
			OCR3A = 0xFFFF;
		}
		else if (frequency > 31250){
			OCR3A = 0x0000;
		}

		else{
			OCR3A = (short)(8000000/(128*frequency))-1;
		}
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32)|(1 << CS31)|(1<<CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{Init,C4,D4,E4} state;
unsigned char button = 0x00;

void Tick()
{
	button = ~PINA & 0x07;

	switch(state)
	{
		case Init:
		if (button == 0x01)
		{
			state = C4;
		}
		else if (button == 0x02)
		{
			state = D4;
		}
		else if (button == 0x04)
		{
			state = E4;
		}
		else
		{
			state = Init;
		}
		break;

		case C4:
		if (button == 0x01)
		{
			state = C4;
		}
		else
		{
			state = Init;
		}
		break;

		case D4:
		if (button == 0x02)
		{
			state = D4;
		}
		else
		{
			state = Init;
		}
		break;

		case E4:
		if (button == 0x04)
		{
			state = E4;
		}
		else
		{
			state = Init;
		}
		break;

		default: state = Init;
		break;
	}

	switch(state)
	{
		case Init:
		set_PWM(0);
		break;

		case C4:
		set_PWM(261.63);
		break;

		case D4:
		set_PWM(293.66);
		break;

		case E4:
		set_PWM(329.63);
		break;
	}
}


int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	/* Insert your solution below */
	
	PWM_on();
	state = Init;
	
	while (1) {
		Tick();
	}
	return 1;
}