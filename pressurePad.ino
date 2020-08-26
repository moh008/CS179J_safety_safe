//Based on template examples/touchpaint for Resistive Touchscreen Test and heavily modified. 
//https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2
//https://github.com/MohammadAdib/Arduino-Touchscreen-Pattern-Unlock/blob/master/PatternUnlock.ino


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>          
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>


// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 pressure = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 screen = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define BOXSIZE 40
#define PENRADIUS 3
String saved_combination = "0124678";
String combination = "";
int lastActivated = -1;
boolean activations[9];
int coords[9][2];
int radius = 9; 

boolean down = false;

void createGrid()
{
  
  screen.drawCircle(coords[0][0],coords[0][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[1][0],coords[1][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[2][0],coords[2][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[3][0],coords[3][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[4][0],coords[4][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[5][0],coords[5][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[6][0],coords[6][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[7][0],coords[7][1],radius,ILI9341_WHITE);
  screen.drawCircle(coords[8][0],coords[8][1],radius,ILI9341_WHITE);
  
}

void setup() {
 
 Serial.begin(9600);

 screen.begin();

 if (!pressure.begin())
 {
    Serial.println("Error, screen unable to start");
    while(1);
 }

  coords[0][0] = 45;
  coords[0][1] = 60;
  
  coords[1][0] = 120;
  coords[1][1] = 60;
  
  coords[2][0] = 195;
  coords[2][1] = 60;
  
  coords[3][0] = 45;
  coords[3][1] = 155;
  
  coords[4][0] = 120;
  coords[4][1] = 155;
  
  coords[5][0] = 195;
  coords[5][1] = 155;
  
  coords[6][0] = 45;
  coords[6][1] = 250;
  
  coords[7][0] = 120;
  coords[7][1] = 250;
  
  coords[8][0] = 195;
  coords[8][1] = 250;

  screen.fillScreen(ILI9341_BLACK);
  Serial.println("Touchscreen Activated");

  createGrid();
 
}

void fillCircle(int x, int y, int radius, int color) {
  screen.fillCircle(x,y,radius,color);
}

void loop() {

    // See if there's any  touch data for us
  if (pressure.bufferEmpty()) {
    return;
  }


  if (pressure.touched()){
     while (!pressure.bufferEmpty()){
       TS_Point p = pressure.getPoint();
       p.x = map(p.x, TS_MINX, TS_MAXX, 240,0);
       p.y = map(p.y, TS_MINY, TS_MAXY, 320,0);

       int x = p.x;
       int y = p.y;
       Serial.print(x);
       Serial.print(",");
       Serial.println(y); 
       processTouch(x,y);
     }
  }
  else
  {
    reset();
    
  }

   if (combination.length() == 9)
      reset();

   delay(1);
 
}

void reset()
{
  Serial.println("reset check");
  while (!pressure.bufferEmpty())
  {
    TS_Point p = pressure.getPoint();
  }
  for (int i = 0; i < 9; i++)
  {
    activations[i] = false;
  }
  lastActivated = -1;
  combination+=0;
  combination.remove(0,10);
  screen.fillScreen(ILI9341_BLACK);
  createGrid();
  delay(1000);

}

//fills in a specific circle on the grid
void activate(int n) {
  //record this data
  if(!activations[n]) {
    if (lastActivated == -1)
    {
       combination.remove(0,10);
    }
    combination += n;
    Serial.print("Combination:");
    Serial.println(combination);
    activations[n] = true;
    if(lastActivated != -1) {
      connectLine(lastActivated, n, ILI9341_WHITE);
    }
    lastActivated = n;
    if(n == 0)
      fillCircle(coords[0][0],coords[0][1],radius,ILI9341_WHITE);  
    if(n == 1)
      fillCircle(coords[1][0],coords[1][1],radius,ILI9341_WHITE);
    if(n == 2)
      fillCircle(coords[2][0],coords[2][1],radius,ILI9341_WHITE);
    if(n == 3)
      fillCircle(coords[3][0],coords[3][1],radius,ILI9341_WHITE);
    if(n == 4)
      fillCircle(coords[4][0],coords[4][1],radius,ILI9341_WHITE);
    if(n == 5)
      fillCircle(coords[5][0],coords[5][1],radius,ILI9341_WHITE);
    if(n == 6)
      fillCircle(coords[6][0],coords[6][1],radius,ILI9341_WHITE);
    if(n == 7)
      fillCircle(coords[7][0],coords[7][1],radius,ILI9341_WHITE);
    if(n == 8)
      fillCircle(coords[8][0],coords[8][1],radius,ILI9341_WHITE);      
    //check combination    
    if(combination.toInt() == saved_combination.toInt()) {
      unlock();
      return;
    }
  }
}


// unlocks the lock
void unlock() {
  //make all lines green
  int n0 = combination.substring(0,1).toInt();
  for(int i = 1; i < combination.length()+1; i++) {
    int n1 = combination.substring(i-1,i).toInt();
    connectLine(n0, n1, ILI9341_GREEN);
    n0 = n1;
  }
  delay(250);
  screen.fillScreen(ILI9341_GREEN);
  Serial.println("Welcome!");
  delay(1000);
  reset();
}


//connects the line between two of the circles
void connectLine(int n0, int n1, int color) {
  int x0 = coords[n0][0];
  int x1 = coords[n1][0];
  int y0 = coords[n0][1];
  int y1 = coords[n1][1];
  screen.drawLine(x0, y0, x1, y1, color);
}

void processTouch(int x, int y) {
  if(x > 15 && x < 75) {
    if(y > 30 && y < 90) {
      activate(8);
    }
    else if(y > 125 && y < 185) {
      activate(5);
    }
    else if(y > 210 && y < 280) {
      activate(2);
    }
  }
  //second column
  if(x > 90 && x < 150) {
    if(y > 30 && y < 90) {
      activate(7);
    }
    else if(y > 125 && y < 185) {
      activate(4);
    }
    else if(y > 210 && y < 280) {
      activate(1);
    }
  }
  //third column
  if(x > 165 && x < 225) {
    if(y > 20 && y < 110) {
      activate(6);
    }
    else if(y > 125 && y < 185) {
      activate(3);
    }
    else if(y > 210 && y < 280) {
      activate(0);
    }
  }
}
