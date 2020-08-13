
/* For CS 179J - Safety Safe
 * Document Created: 8/11/2020 21:19:12 PM
 * Author : Ruth Navarrete
 *
 * Task Log:
 *  Much of the code is from Milestone 2 with minor modifications of
 *    hardcoding to test values and moving code to modularized functions
 *  1) create lightTimeout_test() in compliance with standard IEEE 29119-4 5.3.4
 *     08/11/2020 21:23:44 began
 *     08/--/2020 11:53:04 completed
 *  2) create correctPattern_test() in compliance with standard IEEE 29119-4 5.3.4
 *     08/11/2020 21:23:44 began
 *     08/--/2020 --:--:-- completed
 *  3) create wrongPattern_test() in compliance with standard IEEE 29119-4 5.3.4
 *     08/11/2020 21:23:44 began
 *     08/--/2020 --:--:-- completed
 *     
 * References for IEEE 29119-4 5.3.4
 *   page 111 (hardcopy)/119 (electronic page number)
 *     gives example of how to complete testing
 *
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED A4 //define LED
#define BUZZER A5 //buzzer pin
#define accessPin  A2
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
byte timestamp;

// function to test whether the light timeout after a set time
// for demonstration, time is set to 15 seconds
// tested in compliance of standard IEEE 29119-4 5.3.4 Branch Condition Testing
void lightTimeout_test(int test_value) {
  lcd.clear();
  lcd.setCursor(0,0);
  timeout = millis();
  
  if(openClose == 0) {
    lcd.print("Please Scan RFID");
    digitalWrite(LED, LOW);
  }
  else {
    if(test_value) {// test_value represents (timeout >= (timeoutStart + 30000))
      lcd.print("Close Safe");
      timeoutSound();
    }
    else {
      eeprom();
    }
    digitalWrite(LED, HIGH);
  }
  delay(200);

  RFID();
}

// function to test pattern entry
// correct pattern should allow entry
// incorrect pattern should not allow entry
// tested in compliance of standard IEEE 29119-4 5.3.4 Branch Condition Testing
void pattern_test(int test_value) {
  //
}

void setup() {
  // put your setup code here, to run once:
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
  if (switchMode == PULLDOWN) {
    pinMode(accessPin, INPUT);
    pinMode(recordPin, INPUT);
  }
  else {
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

void loop() {
  lightTimeout_test(1);
  lightTimeout_test(0);
  
  pattern_test(1);
  pattern_test(0);
}

//Buzzer Functions Module
void validSound() {
  delay(500);
  tone(BUZZER, 500);
  delay(300);
  noTone(BUZZER);

  tone(BUZZER, 550);
  delay(300);
  noTone(BUZZER);
}

void invalidSound() {
  tone(BUZZER, 300);
  delay(300);
  noTone(BUZZER);

  tone(BUZZER, 200);
  delay(300);
  noTone(BUZZER);
}

void timeoutSound() {
  tone(BUZZER, 200);
  delay(300);
  noTone(BUZZER);
}

//Servo Functions Module
void servoOpen() {
  Servo.write(180);
}

void servoClose() {
  Servo.write(0);
}

//LCD Functions Module
void lcdOpen() {
  openClose = 1;
  Serial.println("Access Granted");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access Granted");
}

void lcdClose() {
  openClose = 0;
  Serial.println("Locking");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Locking");
  delay(1000);
}

void lcdLocked() {
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
void RFID() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //Show UID on serial monitor DEBUG
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Result : ");

  content.toUpperCase();

  if (content.substring(1) == ("17 A1 B6 60")) { //change here the UID of the card/cards that you want to give access
    if(openClose == 0) { //OPEN
      lcdOpen();
      validSound();
      servoOpen();
      timeoutStart = millis();
      EEPROM.write(0, timeoutStart/1000);
    }
    else { //CLOSE
      lcdClose();
      servoClose();
      lcdLocked();
    }
  }
  else { //invalid RFID key
    lcdInvalid();
    invalidSound();
  }
}

void eeprom() {
  lcd.clear();
  lcd.print("Last Access");
  lcd.setCursor(0,1);
  lcd.print(EEPROM.read(0)/3600);
  lcd.print("Hr ");
  lcd.print((EEPROM.read(0)%3600)/60);
  lcd.print("Min ");
  lcd.print(EEPROM.read(0)%60);
  lcd.print("Sec");
}