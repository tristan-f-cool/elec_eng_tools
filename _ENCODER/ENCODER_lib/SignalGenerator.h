/*
* Signal Generator class Arduino library for Precision Motordrives
* Tristan Cool - February, 2019
*/
#ifndef SignalGenerator_h
#define SignalGenerator_h

//#include <PWM.h>
#include <inttypes.h>


class SignalGenerator{
	public:
	//Constructors:
		SignalGenerator(int mode, int pin_sig, int32_t freq);
		SignalGenerator(int mode, int pin_sig, int pin_pot);
		SignalGenerator(int mode, int pin_sig);
	//Variables:
		int32_t frequency;  		  //Hz
		int32_t frequency_prev;   //frequency change
	//Functions:
		void init();
		void read_freq();
		void set_freq(int32_t freq);
		void set_upper_freq(int32_t upper_freq);
		void set_lower_freq(int32_t lower_freq);
		void set_freq_limits(int32_t lower_freq, int32_t upper_freq);
    void set_phase(int phase);
		void set_wait(int wait);
		void update_freq();
		void pulse();
		void print_data();
	
		
	private:
	//pins:
		uint8_t pinSignalOut;
		uint8_t	pinPotentiometer;
	//Variables:
		bool freq_set;		//mode 1: user-set frequency
		bool freq_manual;	//mode 2: manual-set potentiometer frequency
		bool freq_read;		//mode 3: external system set frequency
		
		int32_t frequency_lower;   //Hz
		int32_t frequency_upper;   //Hz
    int phase;                 //degrees
		int wait;                  //delay time (ms)

		int pot_read;   //potentiometer value
};
#endif
