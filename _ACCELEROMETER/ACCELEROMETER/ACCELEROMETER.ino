//--- PRECISION MICRODRIVES ---
//-- OPP 1066 - AddHaptics
//- Tristan Cool
//- November, 2019

//*** This program reads accelerometer axis    ***//
//*** Data values are displayed on OLED screen ***//


//-------------- Libraries ----------------------------------
#include "SSD1306LessRAM.h"
#include "glcdfont.h"
#include "icons.h"

//--------------- Arduino pins -----------------------------

//--- ACCELEROMETER:
#define pinX  A4        //X axis
#define pinY  A2        //Y axis
#define pinZ  A0        //Z axis

//--- OLED (128x64 LM096-128064):
// VCC = 3.3VDC           //OLED pin 1
// GND                    //OLED pin 2
#define OLED_DC      8    //OLED pin 5
#define OLED_CS      5    //OLED pin 3
#define OLED_CLK     13   //OLED pin 8 (D0)
#define OLED_MOSI    11   //OLED pin 9 (DAT)(D1)
#define OLED_RESET   10   //OLED pin 4 


//-------------- Variables ----------------------------------
uint16_t X_read,Y_read,Z_read;      //10 bit accelerometer signal (raw)
float X_G,Y_G,Z_G;                  //Accelerometer axis in G (gain)

float X_gain,Y_gain,Z_gain;         //Accelerometer calibration factor (~80mV/G)
int X_th,Y_th,Z_th;                 //Accelerometer threshold (noise)
int X_bias,Y_bias,Z_bias;           //Accelerometer calibrate (bias)
float X_max,Y_max,Z_max;            //G value

float vector_sum;     //Vector sum of x,y,z values
float average;        //Avergae axis G value
float std_dev;        //Standard Deviation

SSD1306  oled( OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS );

char CALIB[]  =        "Calibration...";
char MAX_DATA[]  =     "MAX Accel. X-Y-Z [G]";
char ANALYSE_DATA[]  = "SUM / AVG./ STD-DEV.";
char READY[]  =        "~READY~ ";

//const char CALIB[]  =        "Calibration...";
//const char MAX_DATA[]  =     "MAX Accel. X-Y-Z [G]";
//const char ANALYSE_DATA[]  = "SUM / AVG./ STD-DEV.";
//const char READY[]  =        "~READY~ ";
//const char * const ACCEL_DATA[]  = {
//  &CALIB[0],
//  &MAX_DATA[0],
//  &ANALYSE_DATA[0],
//  &READY[0],
//};


//************************************ SETUP ******************************************
/*
 * CALIBRATE:
 * Lay accelerometer flat, print values of 1 axis, record min value (threshold)
 * Flip accelerometer, record values
 * Calculate difference, this is the value of 1G (Gain is 1/difference)
 * Divide reading of accelerometer flat by this difference, this is the bias factor
 * The absolute value for axis in G is: Reading/Gain - Bias
 * 
 * Z: 165,198 --> 1G = 33 --> 1/33 = 0.0303 --> 198/33 = 6G ==> Z = (read*0.0303)-6
 * Y: 165,198 --> 1G = 33 --> 1/33 = 0.0303 --> 198/33 = 6G ==> Y = (read*0.0303)-6
 * X: 167,200 --> 1G = 33 --> 1/33 = 0.0303 --> 200/33 = 6G ==> X = (read*0.0303)-6
*/

void setup()
{
  Serial.begin(9600);
  Serial.println(F("**** PRECISION MICRODRIVES - Tristan Cool - Dec,2019 ****"));
  
  //init (pins)
  pinMode(pinX,INPUT);
  pinMode(pinY,INPUT);
  pinMode(pinZ,INPUT);

  oled.ssd1306_init( SSD1306_SWITCHCAPVCC );
  oled.clear_display();
  oled.drawbitmap( 2, 16,  PMD_LOGO_GLCD_BMP, LOGO_WIDTH, LOGO_HEIGHT, WHITE );
  delay(2000);
  oled.clear_display();

  //init (gain, threshold)
  X_th = 183;//182
  X_gain = 0.0303; 
  X_bias = 6;
  X_max = 0;
  Y_th = 182; //182
  Y_gain = 0.0303;
  Y_bias = 6;
  Y_max = 0;
  Z_th = 198; //197
  Z_gain = 0.0303;
  Z_bias = 6;
  Z_max = 0;

  auto_calibrate();
  //manual_calibrate(6,6,6);
  display_calib();
  display_ready();

} //END: setup

//******************************* LOOP ***************************************
void loop()
{
  //RUN
  read_accel();
  if(X_read > X_th && Y_read > Y_th && Z_read > Z_th)
  {
    //Serial.println(F("*** ACCEL. X-Y-Z (raw)   ***"));
    Serial.println(F("*** ACCEL. X-Y-Z (G)     ***"));
    while(X_read > X_th || Y_read > Y_th || Z_read > Z_th)
    {
      //print_raw();
      print_accel();
      read_accel();
    }
    print_max();    //print to Serial Monitor
    analyse();      //print to Serial Monitor
    display_data(); //print to OLED
    display_ready();
  }
  X_max = Y_max = Z_max = 0;
  
} //END: loop


//************** FUNCTIONS ***********************
void debug()
{
  //DEBUG
}

void read_accel()
{
  X_read = analogRead(pinX);
  Y_read = analogRead(pinY);
  Z_read = analogRead(pinZ);

  X_G = abs((X_read*X_gain)-X_bias);
  Y_G = abs((Y_read*Y_gain)-Y_bias);
  Z_G = abs((Z_read*Z_gain)-Z_bias);

  if(X_G > X_max)
    X_max = X_G;
  if(Y_G > Y_max)
    Y_max = Y_G;
  if(Z_G > Z_max)
    Z_max = Z_G;
}

void auto_calibrate()
{
  read_accel();
  //set threshold to base values + small tolerance
  X_th = X_read + 3;
  Y_th = Y_read + 3;
  Z_th = Z_read + 3;
  //(DC)bias is initial reading with gain (in G)
  X_bias = X_read*X_gain;
  Y_bias = Y_read*Y_gain;
  Z_bias = Z_read*Z_gain;
  Serial.println(F("*** CALIBRATION X-Y-Z (Bias G) ***"));
  Serial.print(X_bias);
  Serial.print(F("G ; "));
  Serial.print(Y_bias);
  Serial.print(F("G ; "));
  Serial.print(Z_bias);
  Serial.println(F("G"));
}

void manual_calibrate(int x_bias, int y_bias, int z_bias)
{
  X_bias = x_bias;
  Y_bias = y_bias;
  Z_bias = z_bias;
  Serial.println(F("*** AUTO-CALIBRATION X-Y-Z (Bias G) ***"));
  Serial.print(X_bias);
  Serial.print(F("G ; "));
  Serial.print(Y_bias);
  Serial.print(F("G ; "));
  Serial.print(Z_bias);
  Serial.println(F("G"));
}

void print_raw()
{
  Serial.print(X_read);
  Serial.print(F("   ; "));
  Serial.print(Y_read);
  Serial.print(F("   ; "));
  Serial.println(Z_read);
}

void print_accel()
{
  Serial.print(X_G);
  Serial.print(F("G ; "));
  Serial.print(Y_G);
  Serial.print(F("G ; "));
  Serial.print(Z_G);
  Serial.println(F("G"));
}

void print_max()
{
  Serial.println(F("*** ACCEL. X-Y-Z (Max. G) ***"));
  Serial.print(X_max);
  Serial.print(F("G ; "));
  Serial.print(Y_max);
  Serial.print(F("G ; "));
  Serial.print(Z_max);
  Serial.println(F("G"));
}

void analyse()
{
  vector_sum = X_max + Y_max + Z_max;
  average = vector_sum / 3;
  float variance = (X_max*X_max + Y_max*Y_max + Z_max*Z_max)/3;
  std_dev = sqrt(variance);
  
  Serial.println(F("***** ACCEL. Analyse       *****"));
  Serial.print(F("* Vector sum (G): "));
  Serial.println(vector_sum);
  Serial.print(F("* Axis average (G): "));
  Serial.println(average);
  Serial.print(F("* Standard Deviation: "));
  Serial.println(std_dev);

  bool one_std_dev;
  if((X_max*1.05 > average-std_dev) && (X_max*0.95 < average+std_dev))
  {
    if((Y_max*1.05 > average-std_dev) && (Y_max*0.95 < average+std_dev))
    {
      if((Z_max*1.05 > average-std_dev) && (Z_max*0.95 < average+std_dev))
      {
        one_std_dev = true;
      }
      else
        one_std_dev = false;
    }
    else
      one_std_dev = false;
  }
  else
    one_std_dev = false;
  
  if(one_std_dev)
    Serial.println(F("All axes are within 1 standard deviation (5% tolerance)."));
  else
    Serial.println(F("Not all axes are within 1 standard deviation."));
}

void display_calib()
{
  oled.clear_display();
  oled.drawbitmap( 40, 17,  PMD_LOGO_GLCD_BMP_mini, 48, 48, WHITE );
  
  //Display calibration status
  const char* head = (char*)( &CALIB[0]);
  oled.drawstring(30, 1, head, WHITE);
  delay(2000);
}

void display_ready()
{
  oled.clear_display();
  oled.drawbitmap( 40, 17,  PMD_LOGO_GLCD_BMP_mini, 48, 48, WHITE );
  const char* head = (char*)( &READY[0]);
  oled.drawstring(40, 1, head, WHITE);
}

void display_data()
{
  //X-Y-Z max values
  char X[4];
  dtostrf(X_max,2,2,X);
  const char* data = (char*)(&X[0]);
  const char Y[4];
  dtostrf(Y_max,2,2,Y);
  const char* data1 = (char*)(&Y[0]);
  const char Z[4];
  dtostrf(Z_max,2,2,Z);
  const char* data2 = (char*)(&Z[0]);
  
  //SUM, AVG, STD-DEV values
  const char SUM[4];
  dtostrf(vector_sum,2,2,SUM);
  const char* data3 = (char*)(&SUM[0]);
  const char AVG[4];
  dtostrf(average,2,2,AVG);
  const char* data4 = (char*)(&AVG[0]);
  const char STDEV[4];
  dtostrf(std_dev,2,2,STDEV);
  const char* data5 = (char*)(&STDEV[0]);

  //X-Y-Z display
  oled.clear_display();
  const char* head = (char*)( &MAX_DATA[0]);
  oled.drawstring(1, 1, head, WHITE);
  
  oled.drawstring(1,3, data, WHITE);
  oled.drawstring(8,3, data1, WHITE);
  oled.drawstring(15,3, data2, WHITE);

  //SUM, AVG, STD-DEV display
  const char* head2 = (char*)( &ANALYSE_DATA[0]);
  oled.drawstring(1, 5, head2, WHITE);

  oled.drawstring(1,6, data3, WHITE);
  oled.drawstring(8,6, data4, WHITE);
  oled.drawstring(15,6, data5, WHITE);
  
  delay(3000);
}
