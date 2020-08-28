//used https://www.viralsciencecreativity.com/post/arduino-rfid-sensor-mfrc522-tutorial as a template and then heavily modified 

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
 
#define SS_PIN 10
#define RST_PIN 9
#define LED A4 //define LED
#define BUZZER A5 //buzzer pin
#define nextPin  A2
#define recordPin  A3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo Servo; //define servo name
LiquidCrystal lcd(8, 2, 4, 5, 6, 7); // LCD screen, 8 = rs and 2 = EN

//variable declarations
int openClose = 0;
unsigned long timeoutStart = 0;
unsigned long timeout = 0;
enum SwitchModes {PULLUP, PULLDOWN};
SwitchModes switchMode = PULLUP;
unsigned long timestamp = 0;
byte i = 1;
byte j = 1;
int buttonState = 0;
int nextButton = 0;


// Setup for main module
void setup() 
{
  lcd.begin(16,2);
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Servo.attach(3); //servo pin
  Servo.write(0); //servo start position
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  //Debugg
  Serial.println("Put your card to the reader...");
  Serial.println();
  
  //EEPROM
  if (switchMode == PULLDOWN)
  {
    pinMode(nextPin, INPUT);
    pinMode(recordPin, INPUT);
  }
  else
  {
    pinMode(nextPin, INPUT_PULLUP);
    pinMode(recordPin, INPUT_PULLUP);
  }
  
//  clears EEPROM when reprogramming
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  i = 1;
  j = 1;
}

//Main / Light Module
void loop() 
{
  lcd.clear();
  lcd.setCursor(0,0);
  timeout = millis();
  
  if(openClose == 0)
  {
    lcd.print("Please Scan RFID");
    digitalWrite(LED, LOW);
  }
  else
  {
    if(timeout >= (timeoutStart + 30000))
    {
      lcd.print("Close Safe");
      timeoutSound();
    }
    else
    {
      recordcheck();
    }
    digitalWrite(LED, HIGH);
  }
  delay(200);

 RFID();
} 

//Buzzer Functions Module
void validSound()
{
  delay(500);
  tone(BUZZER, 500);
  delay(300);
  noTone(BUZZER);

  tone(BUZZER, 550);
  delay(300);
  noTone(BUZZER);
}

void invalidSound()
{
  tone(BUZZER, 300);
  delay(300);
  noTone(BUZZER);

  tone(BUZZER, 200);
  delay(300);
  noTone(BUZZER);
}

void timeoutSound()
{
  tone(BUZZER, 200);
  delay(300);
  noTone(BUZZER);
}

//Servo Functions Module
void servoOpen()
{
  Servo.write(180);
}

void servoClose()
{
  Servo.write(0);
}

//LCD Functions Module
void lcdOpen()
{
  openClose = 1;
  Serial.println("Access Granted");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access Granted");
}

void lcdClose()
{
  openClose = 0;
  Serial.println("Locking");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Locking");
  delay(1000);
}

void lcdLocked()
{
  Serial.println("Locked");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Locked");
  delay(1000);
}

void lcdInvalid()
{
  Serial.println("No Access");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access Denied");
}

//RFID Function Module
void RFID()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  //Show UID on serial monitor DEBUG
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Result : ");


   
  content.toUpperCase();
  
  if (content.substring(1) == ("17 A1 B6 60")) //change here the UID of the card/cards that you want to give access
  {
    if(openClose == 0)//OPEN
    {
        lcdOpen();
        validSound();
        servoOpen();
        recordcheckInc();
        
        timeoutStart = millis();
        EEPROM.write(0, timeoutStart/1000);
    }
    else//CLOSE
    {
        lcdClose();
        servoClose();
        lcdLocked();
    }
  }
 else//invalid RFID key   
 {
    lcdInvalid();
    invalidSound();
  }
}

  
void eeprom(){

    lcd.clear();
    lcd.print("Last Access");
    lcd.setCursor(0,1);
    lcd.print(EEPROM.read(i-1)/3600);
    lcd.print("Hr ");
    lcd.print((EEPROM.read(i-1)%3600)/60);
    lcd.print("Min ");
    lcd.print(EEPROM.read(i-1)%60);
    lcd.print("Sec");

}
void recordcheckInc() {
   timestamp = millis()/1000;
   EEPROM.write(i, timestamp);
   Serial.println(i);
   Serial.println(timestamp);
   
   if(i >= 5)
    {
      i = 1;
    }
    else
    {
      i = i + 1;
    }

    
}
void recordcheck()
{
    lcd.clear();
    lcd.print("Time accessed");
    lcd.setCursor(0, 1);
    lcd.print(EEPROM.read(i)/3600);
    lcd.print("Hr ");
    lcd.print((EEPROM.read(i)%3600)/60);
    lcd.print("Min ");
    lcd.print(EEPROM.read(i)%60);
    lcd.print("Sec");

    j = i;
    buttonState = digitalRead(recordPin);
    while(buttonState == LOW)
    {
      nextButton = digitalRead(nextPin);
      if(nextButton == LOW)
      {
        if(j != 1)
        {
          j = j - 1;
        }
        else if(j == 1)
        {
          j = 5;
        }
      lcd.clear();
      lcd.print("Time accessed");
      lcd.setCursor(0, 1);
      lcd.print(EEPROM.read(j)/3600);
      lcd.print("Hr ");
      lcd.print((EEPROM.read(j)%3600)/60);
      lcd.print("Min ");
      lcd.print(EEPROM.read(j)%60);
      lcd.print("Sec");

      Serial.println(j);
      Serial.println(EEPROM.read(j));
      delay(300);
      }
      buttonState = digitalRead(recordPin);
    }
}
