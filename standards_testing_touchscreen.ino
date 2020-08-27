/* For CS 179J - Safety Safe
 * Document Created: 8/11/2020 21:19:12 PM
 * Author : Ruth Navarrete
 *
 * Task Log:
 *  Minor modifications of hardcoding to test values
 *     and moving code to modularized functions
 *  1) create touchscreen test in compliance with standard IEEE 29119-4 5.3.4
 *     08/27/2020 13:49:24 began
 *     08/27/2020 13:54:49 completed
 *     
 * References for IEEE 29119-4 5.3.4
 *   page 111 (hardcopy)/119 (electronic page number)
 *     gives example of how to complete testing
 *
 */
 
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
  activate(1);
  delay(2500);
  activate(0);
  delay(2500);
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
void activate(int test_value) {
  // modified to remove unneeded code for testing
  //check combination    
  if(test_value) {
    unlock();
    return;
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
  // modified to remove unneeded code for testing
  
}
