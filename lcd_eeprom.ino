// include the library code:
#include <LiquidCrystal.h>
#include <EEPROM.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 2, 4, 5, 6, 7);

byte accessPin = 10;
byte recordPin = A3;

enum SwitchModes {PULLUP, PULLDOWN};
SwitchModes switchMode = PULLUP;

byte timestamp;

  
void loop(){
  byte access = digitalRead(accessPin);
  byte recordcheck = digitalRead(recordPin);
  if(access == HIGH && recordcheck == HIGH)
  {
    delay(100);
  }
  
  else if(access == LOW)
  {
    timestamp = millis()/1000;
    EEPROM.write(0, timestamp);
    lcd.clear();
    lcd.print("Accessed");
    delay(1000);
    
    lcd.clear();
    lcd.print("Press button");
    lcd.setCursor(0, 1);
    lcd.print("Access | Record");
  }
  
  else if(recordcheck == LOW)
  {
    lcd.clear();
    lcd.print("Time accessed");
    lcd.setCursor(0, 1);
    lcd.print(EEPROM.read(0));
    
    delay(2000);
    
    lcd.clear();
    lcd.print("Press button");
    lcd.setCursor(0, 1);
    lcd.print("Access | Record");
  }
}

void setup()
{
  if (switchMode == PULLDOWN)
  {
    pinMode(accessPin, INPUT);
    pinMode(recordPin, INPUT);
  }
  else
  {
    pinMode(accessPin, INPUT_PULLUP);
    pinMode(recordPin, INPUT_PULLUP);
  }
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Press button");
  lcd.setCursor(0, 1);
  lcd.print("Access | Record");
  
}