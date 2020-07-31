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
 *                        -  tested to ensure that LED will turn on for 60 sec with signal
 *                          and then turn off after time has elapsed
 *                        - tested with module "LightsTesting_SM"
 *  2) translate to ardunio
 *     7/28/2020 13:22:17 begain
 *     7/31/2020 12:06:53 completed
 *     used https://github.com/j-bellavance/Tutorials/blob/master/State%20machines%20Tutorial
 *       as reference on using state machines with Arduino
 *     used https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 *       as reference on timing
 *     used https://www.tinkercad.com/dashboard
 *       as simulator
 */

const int LED_PIN = 13;
const int BUTTON_PIN = 2;

// shared variables
unsigned char boxlight;
unsigned char timeout;
unsigned long tmr;
unsigned char lock;

// enums states
enum Lights_States {lights_start, lights_on, lights_off};
Lights_States lightSM_state = lights_start;
unsigned long t0_Lights;
unsigned long t1_Lights;
const unsigned long periodLights = 5000;  // number of milliseconds

enum LightsTest_States {lightsTest_start, lightsTesting};
LightsTest_States lightTestSM_state = lightsTest_start;

// State Machines
void Lights_SM () {
  switch (lightSM_state) { // transitions
    case lights_start:
      if (boxlight == 1) {
        lightSM_state = lights_on;
        digitalWrite(LED_PIN, HIGH);
        t0_Lights = millis();
        lock = 0;
      }
      else {
        lightSM_state = lights_off;
        digitalWrite(LED_PIN, LOW);
        lock = 1;
        timeout = 0;
      }
      break;
      
    case lights_on:
      t1_Lights = millis();
      if (t1_Lights - t0_Lights >= periodLights) {
        lightSM_state = lights_off;
        digitalWrite(LED_PIN, LOW);
        boxlight = 0;
      }
      else {
        lightSM_state = lights_on;
      }
      break;
      
    case lights_off:
      if (boxlight == 1) {
        lightSM_state = lights_on;
        digitalWrite(LED_PIN, HIGH);
        t0_Lights = millis();
        lock = 0;
      }
      else {
        lightSM_state = lights_off;
      }
      break;
  } // transitions
  
  switch (lightSM_state) { // actions
    case lights_start:
      lock = 0;
      break;
    
    case lights_on:
      break;
    
    case lights_off:
      break;
  } // actions
};

void LightsTest_SM () {
 switch (lightTestSM_state) { // transitions
    case lightsTest_start:
      lightTestSM_state = lightsTesting;
      break;
      
    case lightsTesting:
      lightTestSM_state = lightsTesting;
      break;
  } // transitions
  
  switch (lightTestSM_state) { // actions
    case lightsTest_start:
      boxlight = 0;
      break;
      
    case lightsTesting:
      boxlight = digitalRead(BUTTON_PIN);
      break;
  } // actions
};

// the setup function runs once when you press reset or power the board
void setup() {
  //Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  t0_Lights = millis();
}

// the loop function runs over and over again forever
void loop() {
  Lights_SM();
  LightsTest_SM();
}
