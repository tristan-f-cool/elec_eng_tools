//--- PRECISION MICRODRIVES ---
//-- OPP 959 - Handicare
//- Tristan Cool
//- February 25, 2019

//*** This program reads encoder signals from a PMD motor into a controller ***//
//*** This program generates square wave singals based on calculated system input into controller ***//
//*** TODO ***//


//-------------- Libraries ----------------------------------
#include "Encoder.h"
#include "SignalGenerator.h"


//--------------- Arduino pins -----------------------------
//--- ENCODER:
//ROTARY (magentic or optical)
#define pinA  2                   //OUTPUT A
#define pinB  4                   //OUTPUT B
//QUADRATURE
//#define pinZ  3                 //OUTPUT Z(INDEX)
//DIFFERENTIAL QUADRATURE
//#define pinA_ 5                 //OUTPUT A_NOT
//#define pinB_ 7                 //OUTPUT B_NOT
//#define pinZ_ 6                 //OUTPUT Z_NOT
//--- SIGNAL GENERATOR:
#define pinSignalA 9                //SQUARE WAVE SIGNAL - A (PWM)
#define pinSignalB 10               //SQAURE WAVE SIGNAL - B (PWM)
//#define pinFreqPot A2               //POTENTIOMETER FREQ SET


//-------------- Variables ----------------------------------
//--- ENCODER:
bool quad = false;
bool diff_quad = false;
Encoder encoder(quad,diff_quad,pinA,pinB);
bool encoder_pulse; //pulse detect
//--- SIGNAL:
int mode = 3; //1: hard-set freq, 2:pot-set freq, 3:system-set freq(encoder)
//int32_t frequency;
//int32_t freq_max;
//int32_t freq_min;
unsigned int delay_sig;
SignalGenerator signalgeneratorA(mode,pinSignalA);
SignalGenerator signalgeneratorB(mode,pinSignalB);
//--- MOTOR:
//--- TESTS:
//--- FAILSAFE: 


//************************************ SETUP ***************************************
void setup() 
{
  //Serial Monitor [Ctrl + Shift + m]
  Serial.begin (9600);
  Serial.println(F("**** PRECISION MICRODRIVES - Tristan Cool - Feb,2019 ****"));
  //INIT:
  INIT();
  //TESTS:
}


//********************************* LOOP ******************************************
void loop() 
{
  //---ENCODER:
  encoder.timing();
  encoder.read_state();
  encoder_pulse = encoder.pulse();
  encoder.print_data();
  //---SIGNAL:
  //PULSE
  if(encoder_pulse) //if encoder has registered a pulse, generate signals
  {
    PULSE();
  }
  //---TESTS:
  //---PRINT:
  //PRINT();
  //---UPDATE:
  UPDATE();
}


//------------------- FUNCTIONS -----------------------
//********* ENCODER *********

//********* SIGNAL **********
unsigned int CalculateDelay()
{
  // t = phase/(360*freq)
  //encoder uses a 90deg phase between A and B : 90/360 = 1/4
  delay_sig = 1/(4*encoder.freq);
  return delay_sig*1000; //convert to microseconds
}

void PULSE()
{
  //determine signal phase frequency delay time
  delay_sig = CalculateDelay();
  signalgeneratorA.set_freq(encoder.freq);
  signalgeneratorB.set_freq(encoder.freq);
  //check direction and adjust signal timing
  if(encoder.dir) //CW=true
  {
    //A leads B (90deg phase)
    signalgeneratorA.pulse();
    delayMicroseconds(delay_sig);
    signalgeneratorB.pulse();
  }
  else //CCW=false
  {
    //B leads A (90deg phase)
    signalgeneratorB.pulse();
    delayMicroseconds(delay_sig);
    signalgeneratorA.pulse();
  }
  signalgeneratorA.print_data();
  signalgeneratorB.print_data();
}

//********* INIT ************
void INIT()
{
  //---ENCODER:  
  encoder.init();
  //---SIGNAL:
  signalgeneratorA.init();
  //signalgeneratorA.set_freq_limits(freq_min,freq_max);
  signalgeneratorB.init();
  //signalgeneratorA.set_freq_limits(freq_min,freq_max);
}

//********* UPDATE **********
void UPDATE()
{
  //---ENCODER:
  encoder.update_state();
  //---SIGNAL:
  signalgeneratorA.update_freq();
  signalgeneratorB.update_freq();
}

//********* TESTS ***********

//********* PRINT ************
void PRINT()
{
  //---CYCLE:
  Serial.print(F("CYCLE COUNT: "));
  Serial.println(encoder.counter_cycle); 
  Serial.print(F("Progress(%): "));
  Serial.println((encoder.counter_cycle/20000)*100);
  //---ENCODER:
  //---SIGNAL:
  //---TESTS:
}
