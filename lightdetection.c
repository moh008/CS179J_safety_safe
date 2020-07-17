/*
* Safety Safe - Light Detection (prototype)
* (awaiting testing)
*
* Author: Abel Theodros
*/

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ACD_init()
{
	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE);
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
	const unsigned short MAX = 0x7F;
	ACD_init();
    while (1) {
	unsigned short x = ADC;
	if (x>= 2*MAX/3)
		PORTB = 0x01;
	else
		PORTB = 0x00;
    }
    return 1;
}
