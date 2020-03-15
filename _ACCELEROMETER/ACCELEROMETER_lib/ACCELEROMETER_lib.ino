//--- PRECISION MICRODRIVES ---
//-- OPP 1066 - AddHaptics
//- Tristan Cool
//- November, 2019

//*** This program reads accelerometer axis***//
//*** TODO ***//


//-------------- Libraries ----------------------------------
#include "Accelerometer.h"

//--------------- Arduino pins -----------------------------

//--- ACCELEROMETER:
#define pinX  A4        //X axis
#define pinY  A2        //Y axis
#define pinZ  A0        //Z axis

//--- OLED:
#define OLED_DC      8    //OLED pin 5
#define OLED_CS      5    //OLED pin 3
#define OLED_CLK     13   //OLED pin 8 (D0)
#define OLED_MOSI    11   //OLED pin 9 (DAT)(D1)
#define OLED_RESET   10   //OLED pin 4 



//-------------- Variables ----------------------------------
//Accelerometer accelerometer();
Accelerometer accelerometer(183,182,198,0.0303,0.0303,0.0303); // xyz threshold, xyz gain

//************************************ SETUP ******************************************
void setup()
{
  //Serial Monitor [Ctrl + Shift + m]
  Serial.begin (9600);
  Serial.println(F("**** PRECISION MICRODRIVES - Tristan Cool - Dec,2019 ****"));

  accelerometer.init(pinX,pinY,pinZ);
  accelerometer.display_init(OLED_DC,OLED_CS,OLED_CLK,OLED_MOSI,OLED_RESET);
  accelerometer.auto_calibrate();
  accelerometer.display_calib();
  accelerometer.display_ready();
  //accelerometer.manual_calibrate(6,6,6);

} //END: setup

//******************************* LOOP ***************************************
void loop()
{
  accelerometer.start(true);
} //END: loop


//************** FUNCTIONS ***********************
void debug()
{
  
}
