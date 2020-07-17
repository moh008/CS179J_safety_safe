/*
 * For CS 179J - Safety Safe
 * Document Created: 7/14/2020 7:50:52 PM
 * Author : Ruth Navarrete
 *
 * Task Log:
 *  1) illuminate the box
 *     7/14/2020 19:54:54 began
 *     7/15/2020 09:25:58 completed illuminate box task
 *                        - LED turns on with light signal,
 *                          for testing button supplied input for signal
 *                        -	tested to ensure that LED will turn on for 60 sec with signal
 *                          and then turn off after time has elapsed
 *                        - tested with module "LightsTesting_SM"
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "header/io.h"
#include "header/timer.h"
#include "header/bit.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

// A struct to collect all items related to a task.
typedef struct task {
	int state;                  // Task's current state
	unsigned long period;       // Task period
	unsigned long elapsedTime;  // Time elapsed since last task tick
	int (*TickFct)(int);        // Task tick function
} task;


// current tasks (in order of tasks written):
// - Lights_SM
// - LightsTesting_SM

// array to hold tasks
task tasks[2];
const unsigned short tasksNum = 2;

// information held constant
const unsigned long periodGCD = 500;
const unsigned long periodLights = 1000;
const unsigned long periodLightsTest = 500;

// shared variables
unsigned char boxlight;
unsigned char timeout;
unsigned char tmr;
unsigned char lock;

// Lights_SM
enum Lights_States {lights_start, lights_on, lights_off};
int Lights_SM (int state) {
	switch (state) { // transitions
		case lights_start:
			if (boxlight == 1) {
				state = lights_on;
				PORTA = (PORTA | 0x01);
				tmr = 60;
				lock = 0;
			}
			else {
				state = lights_off;
				PORTA = (PORTA & 0xFE);
				lock = 1;
				timeout = 0;
			}
			break;
			
		case lights_on:
			if (tmr == 0) {
				state = lights_off;
				PORTA = (PORTA & 0xFE);
				boxlight = 0;
			}
			else {
				state = lights_on;
			}
			break;
			
		case lights_off:
			if (boxlight == 1) {
				state = lights_on;
				PORTA = (PORTA | 0x01);
				tmr = 160;
				lock = 0;
			}
			else {
				state = lights_off;
			}
			break;
	} // transitions
	
	switch (state) { // actions
		case lights_start:
			tmr = 0;
			lock = 0;
			break;
		
		case lights_on:
			tmr--;
			break;
		
		case lights_off:
			break;
	} // actions
	return state;
};

// LightsTest_SM
enum Lights_StatesTest {lightsTest_start, lightsTesting};
int LightsTesting_SM (int state) {
	switch (state) { // transitions
		case lightsTest_start:
			state = lightsTesting;
			break;
			
		case lightsTesting:
			state = lightsTesting;
			break;
	} // transitions
	
	switch (state) { // actions
		case lightsTest_start:
			boxlight = 0;
			break;
			
		case lightsTesting:
			if ((~PINA) & 0x02) {
				boxlight = 1;
			}
			break;
	} // actions
	return state;
};

int main(void) {
	DDRA = 0xFD; PORTA = 0x02;  // Configure PORTA as input/output, initialize to 0s
		
	unsigned char i = 0;
	
	// LightsTesting_SM information, to be removed with integration
	tasks[i].state = lightsTest_start;
	tasks[i].period = periodLightsTest;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &LightsTesting_SM;
	i++;
	
	tasks[i].state = lights_start;
	tasks[i].period = periodLights;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Lights_SM;
	i++;
		
	TimerSet(periodGCD);
	TimerOn();
		
	while (1) {
		for (i = 0; i < tasksNum; i++) {
			if (tasks[i].elapsedTime >= tasks[i].period) {
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += periodGCD;
		}
		while (!TimerFlag) { }
		TimerFlag = 0;
	}
	return 0;
}