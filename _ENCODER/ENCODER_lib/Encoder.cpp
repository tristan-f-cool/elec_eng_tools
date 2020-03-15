#include "Encoder.h"

#include <stdio.h>
#include <string.h>
#include "Arduino.h"

Encoder::Encoder(bool quad, bool diff, uint8_t A, uint8_t B)
{
	quad = quad;
	diff = diff;
	pinA = A;
	pinB = B;
}

Encoder::Encoder(bool quad, bool diff, uint8_t A, uint8_t B, uint8_t Z)
{
	quad = quad;
	diff = diff;
	pinA = A;
	pinB = B;
	pinZ = Z;
}

Encoder::Encoder(bool quad, bool diff, uint8_t A, uint8_t B, uint8_t Z, uint8_t A_, uint8_t B_, uint8_t Z_)
{
	quad = quad;
	diff = diff;
	pinA = A;
	pinB = B;
	pinZ = Z;
	pinA_ = A_;
	pinB_ = B_;
	pinZ_ = Z_;
}

void Encoder::init()
{
	//Read inital encoder states
	pinMode(pinA, INPUT);
	pinMode(pinB, INPUT);
	prev_stateA  = digitalRead(pinA);
	prev_stateB  = digitalRead(pinB);
	/*
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
	*/
	counter_edge = 0;
	pos = 0;
	counter_cycle = 0;
	timeLH = 0.0;	
	timeHL = 0.0;
	period = 0.0;
	freq = 0.0;
	dir = false;		

	Serial.println(F("Encoder:....initialized"));
}

void Encoder::read_state()
{
	//Read current encoder states
	stateA  = digitalRead(pinA);
	stateB  = digitalRead(pinB);
	/*
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
	*/
}

bool Encoder::rotation()
{
	//if A leads B, rotation is CW
	if(stateB !=  stateA)
		dir = true;  //CW
	else
		dir = false; //CCW

	return dir;
}

float Encoder::durationLH(int pin)
{
	//return microseconds between signal flip-flop LOW to HIGH
	return pulseIn(pin,HIGH);
}

float Encoder::durationHL(int pin)
{
	//return microseconds between signal flip-flop HIGH to LOW
	return pulseIn(pin,LOW);
}

void Encoder::timing()
{
	//Record pulse time - convert to milliseconds
	timeLH = durationLH(pinA)*1000;
	timeHL = durationHL(pinA)*1000;
	
}

float Encoder::frequency()
{
	//period = millis();       //half-wave period (ms)
	//freq = 1000/(period*2);  //calculate full period (x2) and convert to seconds (x1000)
	
	period = timeLH + timeHL;
	freq = 1/period;
	
	return freq;
}

bool Encoder::pulse()
{
	//Check encoder pulse
	if(stateA != prev_stateA)
	{
		//Record period and calculate frequency
		freq = frequency();
		//Determine rotation direction
		if(rotation()== true) //CW
		      pos++;
		else //CCW
		      pos--;
		//Calculate position and count cycles
		counter_edge++;
		if(counter_edge  > 3560) //1780 x 2
    {
			counter_cycle++;
      counter_edge = 0;
    }
    //Serial.println(F("Encoder....pulse"));
    return true;  //succesful pulse
    }
    return false; //no pulse
}

void Encoder::update_state()
{
	//Update encoder states
	prev_stateA  = stateA;
	prev_stateB  = stateB;
	/*  
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
	*/
}

void Encoder::print_data()
{
  pos = map(pos,-890,890,-100,100); // 1780/2
	//Print encoder data
	Serial.println(F("--------- ENCODER ----------"));
	Serial.print(F("Edge count: "));
	Serial.println(counter_edge);
	Serial.print(F("Pulse time: "));
	Serial.println(timeLH);
	Serial.print(F("Period(ms): "));
	Serial.println(period);
	Serial.print(F("Frequency(Hz): "));
	Serial.println(freq);
	Serial.print(F("Rotation: "));
	if(dir == 1)
		Serial.println(F("CW"));
	else if(dir == 0)
		Serial.println(F("CCW"));
  Serial.print(F("Position(deg): "));
  Serial.print(pos);
	Serial.print(F("A: "));
	Serial.println(stateA);
	Serial.print(F("B: "));
	Serial.println(stateB);
	/*
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
	*/
}
