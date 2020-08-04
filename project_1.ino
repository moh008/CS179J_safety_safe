//used https://www.viralsciencecreativity.com/post/arduino-rfid-sensor-mfrc522-tutorial as a template and then heavily modified 

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>
 
#define SS_PIN 10
#define RST_PIN 9
#define LED A4 //define LED
#define BUZZER A5 //buzzer pin
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo Servo; //define servo name
LiquidCrystal lcd(8, 2, 4, 5, 6, 7); // LCD screen, 8 = rs and 2 = EN

//variable declarations
int openClose = 0;
unsigned long timeoutStart = 0;
unsigned long timeout = 0;

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

  //Displays
  Serial.println("Put your card to the reader...");
  Serial.println();

  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  delay(100);

  
}
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
    if(timeout >= (timeoutStart + 5000))
    {
      lcd.print("Close Safe");
      tone(BUZZER, 200);
      delay(300);
      noTone(BUZZER);
    }
    else
    {
      lcd.print("Last Accessed:");
    }
    digitalWrite(LED, HIGH);
  }
  delay(200);
 
  
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

  
  //Show UID on serial monitor
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
  
  if (content.substring(1) == "17 A1 B6 60") //change here the UID of the card/cards that you want to give access
  {
    if(openClose == 0)//OPEN
    {
      openClose = 1;
      Serial.println("Access Granted");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Access Granted");
      
      delay(500);
      tone(BUZZER, 500);
      delay(300);
      noTone(BUZZER);
  
      tone(BUZZER, 550);
      delay(300);
      noTone(BUZZER);
    
      Servo.write(180);
      timeoutStart = millis();
      //delay(5000);
      //Servo.write(0);
    }
    else//CLOSE
    {
      openClose = 0;
      Serial.println("Locking");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Locking");

      delay(1000);
      Servo.write(0);

      Serial.println("Locked");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Locked");
      delay(1000);

      
    }
    
  }
 
 else   {
    Serial.println("No Access");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access Denied");
    
    tone(BUZZER, 300);
    delay(300);
    noTone(BUZZER);

    tone(BUZZER, 200);
    delay(300);
    noTone(BUZZER);
  }
} 
