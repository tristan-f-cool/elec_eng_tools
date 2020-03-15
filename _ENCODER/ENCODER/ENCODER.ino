//--- PRECISION MICRODRIVES ---
//-- OPP 959 - Handicare
//- Tristan Cool
//- February 21, 2019

//*** This program reads encoder signals from a PMD motor into the Handicare controller ***//
//*** TODO ***//


//-------------- Libraries ----------------------------------


//--------------- Arduino pins -----------------------------

//--- ENCODER:
//ROTARY (magentic or optical)
#define pinA  6                   //OUTPUT A
#define pinB  8                   //OUTPUT B
//QUADRATURE
#define pinZ  10                  //OUTPUT Z(INDEX)
//DIFFERENTIAL QUADRATURE
#define pinA_ 7                   //OUTPUT A_NOT
#define pinB_ 9                   //OUTPUT B_NOT
#define pinZ_ 11                  //OUTPUT Z_NOT


//-------------- Variables ----------------------------------

//--- ENCODER
int counter_edge      = 0;    //step counter
int pos               = 0;    //motor gear relative position
int counter_cycle     = 0;    //cycles
bool dir              = 0;    //rotation direction (CW=1, CCW=0)
unsigned long period  = 0;    //period (ms)
unsigned long timeHL  = 0.0;  //signal timing from HIGH/LOW(us)
unsigned long timeLH  = 0.0;  //signal timing from LOW/HIGH(us)
float freq            = 0.0;  //frequency (Hz)

bool quad       = false;
bool diff_quad  = false;

int stateA;
int prev_stateA;
int stateB;
int prev_stateB;
//Quadrature
int stateZ;
int prev_stateZ;
//Differential Quadrature
int stateA_;
int prev_stateA_;
int stateB_;
int prev_stateB_;
int stateZ_;
int prev_stateZ_;


//************************************ SETUP ******************************************
void setup() 
{
  //Serial Monitor [Ctrl + Shift + m]
  Serial.begin (9600);
  Serial.println(F("PRECISION MICRODRIVES - Tristan Cool - Feb,2019"));

  //initialize encoder
  start();
}


//******************************* LOOP ***************************************
void loop() 
{
  //read initial encoder states
  readENCODER();

  //encoder pulse
  PULSE();
  
  //print encoder data
  PRINT();

  //encoder tests
  //TODO

  //update encoder states
  updateENCODER();
}


//************** FUNCTIONS ***********************

void start()
{
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  prev_stateA  = digitalRead(pinA);
  prev_stateB  = digitalRead(pinB);
 
  if(quad)
  {
    pinMode(pinZ, INPUT);
    prev_stateZ  = digitalRead(pinZ);
  }
  if(diff_quad)
  {
    pinMode(pinA_,INPUT);
    pinMode(pinB_,INPUT);
    pinMode(pinZ_,INPUT);
    prev_stateA_ = digitalRead(pinA_);
    prev_stateB_ = digitalRead(pinB_);
    prev_stateZ_ = digitalRead(pinZ_);
  }
  Serial.println(F("Encoder:....initialized"));
}

void readENCODER()
{
  stateA  = digitalRead(pinA);
  stateB  = digitalRead(pinB);
  
  if(quad)
  {
    stateZ  = digitalRead(pinZ);
  }
  if(diff_quad)
  {
    stateA_ = digitalRead(pinA_);
    stateB_ = digitalRead(pinB_);
    stateZ_ = digitalRead(pinZ_);
  }
}

void updateENCODER()
{
  prev_stateA  = stateA;
  prev_stateB  = stateB;
  
  if(quad)
  {
    prev_stateZ  = stateZ;
  }
  if(diff_quad)
  {
    prev_stateA_ = stateA_;
    prev_stateB_ = stateB_;
    prev_stateZ_ = stateZ_;
  }
}

void PRINT()
{ 
  Serial.print(F("Count: "));
  Serial.println(counter_cycle);
  Serial.print(F("Completition(%): "));
  Serial.println((counter_cycle/20000)*100);
  //Print encoder data
  Serial.println(F("--------- ENCODER ----------"));
  Serial.print(F("Edge count: "));
  Serial.println(counter_edge);
  Serial.print(F("Pulse time: "));
  Serial.println(timeLH);
  Serial.print(F("Period (ms): "));
  Serial.println(period);
  Serial.print(F("Frequency (Hz): "));
  Serial.println(freq);
  Serial.print(F("Rotation: "));
  if(dir == 1)
    Serial.println(F("CW"));
  else if(dir == 0)
    Serial.println(F("CCW"));
  Serial.print(F("A: "));
  Serial.println(stateA);
  Serial.print(F("B: "));
  Serial.println(stateB);
  if(quad)
  {
    Serial.print(F("Z: "));
    Serial.println(stateZ);  
  }
  if(diff_quad)
  {
    Serial.print(F("A_: "));
    Serial.println(stateA_);
    Serial.print(F("B_: "));
    Serial.println(stateB_);
    Serial.print(F("Z_: "));
    Serial.println(stateZ_);  
  }
  Serial.println(F("----------------------------"));
}

bool ROTATION()
{
  //if A leads B, rotation is CW
  if(stateB !=  stateA)
    dir = true;  //CW
  else
    dir = false; //CCW
  return dir;
}

void FREQUENCY()
{
  //period = millis();       //half-wave period (ms)
  //freq = 1000/(period*2);  //calculate full period (x2) and convert to seconds (x1000)

  //calculate microseconds between signal flip-flop LOW to HIGH --> convert to milliseconds
  timeLH = pulseIn(pinA,HIGH)*1000;
  timeHL = pulseIn(pinA,LOW)*1000;

  //calculate period and frequency
  period = timeLH + timeHL;
  freq = 1/period;
}

void PULSE()
{
  //Check rotation and count
  if(stateA != prev_stateA)
  {
    //Record period and calculate frequency
    FREQUENCY();
    
    if(ROTATION()== true) //CW
    {
      counter_edge++;
    }
    else //CCW
    {
      counter_edge--;
    }
    //Calculate position (TODO: map to range) and count cycles (TODO: calibrate)
    pos = counter_edge;
    if(pos == -500)
      counter_cycle++;
  }
}

