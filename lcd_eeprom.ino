// include the library code:
#include <LiquidCrystal.h>
#include <EEPROM.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 2, 4, 5, 6, 7);

byte accessPin = 10;
byte recordPin = A3;
byte nextPin = A4;

enum SwitchModes {PULLUP, PULLDOWN};
SwitchModes switchMode = PULLUP;

byte timestamp, access, recordcheck, next, i;

void loop(){
  access = digitalRead(accessPin);
  recordcheck = digitalRead(recordPin);
  
  if(access == HIGH && recordcheck == HIGH)
  {
    delay(100);
  }
  
  else if(access == LOW)
  {
    timestamp = millis()/1000;
    EEPROM.write(i, timestamp);
    
    lcd.clear();
    lcd.print("Accessed");
    delay(1000);
    
    lcd.clear();
    lcd.print("Press button");
    lcd.setCursor(0, 1);
    lcd.print("Access | Record");
    if(i >= 4)
    {
      i = 0;
    }
    else
    {
      i = i + 1;
    }
  }
  
  else if(recordcheck == LOW)
  {
    lcd.clear();
    lcd.print("Time accessed");
    lcd.setCursor(0, 1);
    lcd.print(EEPROM.read(i-1)/3600);
    lcd.print("Hr ");
    lcd.print((EEPROM.read(i-1)%3600)/60);
    lcd.print("Min ");
    lcd.print(EEPROM.read(i-1)%60);
    lcd.print("Sec");
       
    while(recordcheck == LOW)
    {
      access = digitalRead(accessPin);
      if(access == LOW)
      {
        break;
      }
      next = digitalRead(nextPin);
      if(next == LOW)
      {
        if(i != 0)
        {
          i = i - 1;
        }
        else if(i == 0)
        {
          i = 4;
        }
        lcd.clear();
    	lcd.print("Time accessed");
    	lcd.setCursor(0, 1);
    	lcd.print(EEPROM.read(i)/3600);
    	lcd.print("Hr ");
    	lcd.print((EEPROM.read(i)%3600)/60);
    	lcd.print("Min ");
    	lcd.print(EEPROM.read(i)%60);
    	lcd.print("Sec");
        delay(300);
      }
    }
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
    pinMode(nextPin, INPUT);
  }
  else
  {
    pinMode(accessPin, INPUT_PULLUP);
    pinMode(recordPin, INPUT_PULLUP);
    pinMode(nextPin, INPUT_PULLUP);
  }
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Press button");
  lcd.setCursor(0, 1);
  lcd.print("Access | Record");
  i = 0;
}
