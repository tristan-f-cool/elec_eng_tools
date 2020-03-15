#include "SignalGenerator.h"

#include <stdio.h>
#include <string.h>
#include "Arduino.h"

SignalGenerator::SignalGenerator(int mode, int pin_sig, int32_t freq)
{
	switch(mode)
	{
		case 1:
			freq_set = true;
			freq_manual = false;
			freq_read = false;
			break;
		case 2:
			freq_set = false;
			freq_manual = true;
			freq_read = false;
			break;
		case 3:
			freq_set = false;
			freq_manual = false;
			freq_read = true;
			break;
	}
	pinSignalOut = pin_sig;
	frequency = freq;
}

SignalGenerator::SignalGenerator(int mode, int pin_sig, int pin_pot)
{
	switch(mode)
	{
		case 1:
			freq_set = true;
			freq_manual = false;
			freq_read = false;
			break;
		case 2:
			freq_set = false;
			freq_manual = true;
			freq_read = false;
			break;
		case 3:
			freq_set = false;
			freq_manual = false;
			freq_read = true;
			break;
	}
	pinSignalOut = pin_sig;
	pinPotentiometer = pin_pot;
}

SignalGenerator::SignalGenerator(int mode, int pin_sig)
{
	switch(mode)
	{
		case 1:
			freq_set = true;
			freq_manual = false;
			freq_read = false;
			break;
		case 2:
			freq_set = false;
			freq_manual = true;
			freq_read = false;
			break;
		case 3:
			freq_set = false;
			freq_manual = false;
			freq_read = true;
			break;
	}
	pinSignalOut = pin_sig;
}

void SignalGenerator::init()
{
	/*
	if(freq_manual)
		pinMode(pinPotentiometer, INPUT);
	*/
	//TODO
	//InitTimerSafe();
	frequency_lower = 1;
	frequency_upper = 100000;
	wait=0;
  phase=90;
	
	frequency = frequency_lower;  //default value
	frequency_prev = frequency;   //set initial frequency
	
	Serial.println(F("Signal Generator:......initialized"));
}

void SignalGenerator::read_freq()
{
    pot_read = analogRead(pinPotentiometer);
    frequency = map(pot_read,0,1023,frequency_lower,frequency_upper);
}

void SignalGenerator::set_freq(int32_t freq_input)
{
	if(frequency != frequency_prev)
	{
		if(freq_input < frequency_lower)
			freq_input = frequency_lower;
		if(freq_input > frequency_upper)
			freq_input = frequency_upper;
		frequency = freq_input;
	}
}

void SignalGenerator::set_upper_freq(int32_t upper_freq)
{
	frequency_upper = upper_freq;
}

void SignalGenerator::set_lower_freq(int32_t lower_freq)
{
	if(lower_freq < 0)
		lower_freq = 0;
	frequency_lower = lower_freq;
}

void SignalGenerator::set_freq_limits(int32_t lower_freq, int32_t upper_freq)
{
	set_lower_freq(lower_freq);
	set_upper_freq(upper_freq);
}

void SignalGenerator::set_phase(int phase)
{
  phase = phase;
}

void SignalGenerator::set_wait(int wait)
{
	wait = wait;
}

void SignalGenerator::update_freq()
{
	frequency_prev = frequency;
}

void SignalGenerator::pulse()
{
	/*
	if(freq_manual)
		readFREQ();
	*/
	if(frequency != frequency_prev)
	{
		//pwmWriteHR(pinSignalOut,32768); //Set duty cycle to 50% by default for 16-bit 65536/2 = 32768
		//SetPinFrequencySafe(pinSignalOut,frequency);
		Serial.println(F("!..signal injected"));
		//delay(wait);
	}
}

void SignalGenerator::print_data()
{
	Serial.println(F("--------- SIGNAL GENERATOR ----------"));
	Serial.print(F("Frequency(Hz): "));
	Serial.println(frequency);
}
