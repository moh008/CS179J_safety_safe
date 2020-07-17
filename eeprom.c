/*
 * tests.c
 *
 * Created: 2020-07-17 오전 3:09:51
 * Author : Lincoln
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "io.c"
#include "timer.h"

#define access 0x01
#define recordcheck 0x02

unsigned char EEMEM eeprom_array[1];

//EEPROM Macro
#define read_eeprom_word(address) eeprom_read_word((const uint16_t*)address)
#define write_eeprom_word(address, value) eeprom_write_word((uint16_t*)address, (uint16_t)value)
#define update_eeprom_word(address, value) eeprom_update_word((uint16_t*)address, (uint16_t)value)

unsigned char button = 0x00;
unsigned char timecnt, record_cnt, access_cnt, timestamp = 0;
enum menu_States{home_init, home_wait, access_hold, access_rl, record} state;
void menu()
{
	switch(state)
	{
		case home_init:
		state = home_wait;
		LCD_DisplayString(1, "Press 1)access 2)record");
		break;
		
		case home_wait:
		if(button == access) 
			state = access_hold;
		else if(button == recordcheck)
		{
			state = record;
			LCD_DisplayString(1, "Time accessed: ");
			LCD_Cursor(15);
			LCD_WriteData(read_eeprom_word(&eeprom_array[0]) + '0');
		}
		else
			state = home_wait;
		break;
		
		case access_hold:
		if(!button) 
			state = access_rl;
		else if(button == access)
			state = access_hold;
		break;
		
		case access_rl:
		if(access_cnt == 10)
		{
			state = home_wait;
			LCD_DisplayString(1, "Press 1)access 2)record");
			access_cnt = 0;
		}
		else if(access_cnt < 10)
			state = access_rl;
		break;
		
		case record:
		if(record_cnt == 30)
		{
			state = home_wait;
			record_cnt = 0;
			LCD_DisplayString(1, "Press 1)access  2)record");
		}
		else if(record_cnt < 30)
			state = record;
		break;
		
		default:
		break;
	}
	
	switch(state)
	{
		case home_init:
		timecnt = 0;
		LCD_ClearScreen();
		break;
		
		case home_wait:
		timecnt++;
		break;
		
		case access_hold:
		LCD_DisplayString(1, "Accessed");
		timestamp = timecnt/10;
		write_eeprom_word(&eeprom_array[0], read_eeprom_word(&eeprom_array[0]) + timestamp);
		timecnt++;
		break;
		
		case access_rl:
		access_cnt++;
		timecnt++;
		break;
		
		case record:
		timecnt++;
		record_cnt++;
		break;
		
		default:
		break;
	}
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;	//PORTC and PORTD are related to the LCD screen
	DDRD = 0xFF; PORTD = 0x00;
	 
	TimerSet(100);
	TimerOn();
	
	state = home_init;
	LCD_init();
	
	if(read_eeprom_word(&eeprom_array[0]) + '0' == '/')
	{
		write_eeprom_word(&eeprom_array[0], read_eeprom_word(&eeprom_array[0]) + 1);
	}
	
    while(1) 
    {
		button = ~PINA;
		menu();
		while (!TimerFlag){}
		TimerFlag = 0;
    }
	return 0;
}

