/*
* Encoder class Arduino library for Precision Motordrives
* Tristan Cool - February, 2019
*/
#ifndef Encoder_h
#define Encoder_h

#include <inttypes.h>

class Encoder{
	
	public:
	//Constructors:
		Encoder(bool quad, bool diff, uint8_t A, uint8_t B);													//Rotary encoder: Magnetic or Optical
		Encoder(bool quad, bool diff, uint8_t A, uint8_t B, uint8_t Z ); 										//Quadrature
		Encoder(bool quad, bool diff, uint8_t A, uint8_t B, uint8_t Z, uint8_t A_, uint8_t B_, uint8_t Z_);		//Differential Quadrature
		
	//Variables:
		int counter_edge;			//step counter A
		int pos;				//motor gear relative position
		int counter_cycle;
		unsigned long timeHL;		//signal timing from HIGH/LOW(us)
		unsigned long timeLH;		//signal timing from LOW/HIGH(us)
		unsigned long period;		//period (ms)
		float freq;					//frequency (Hz)
		bool dir;				//CW=1 ; CCW=0
		
		int stateA;
		int prev_stateA;
		int stateB;
		int prev_stateB;
		int stateZ;
		int prev_stateZ;
		int stateA_;
		int prev_stateA_;
		int stateB_;
		int prev_stateB_;
		int stateZ_;
		int prev_stateZ_;
				
	//Functions:
		void init();
		void read_state();
		bool rotation();
		float durationHL(int signal);
		float durationLH(int signal);
		void timing();
		float frequency();
		bool pulse();
		void update_state();	
		void print_data();
		
	private:
	//pins:
		uint8_t pinA;
		uint8_t pinB;
		uint8_t pinZ;
		uint8_t pinA_;
		uint8_t pinB_;
		uint8_t pinZ_;
		
	//Variables:
		bool quad;
		bool diff_quad;
		
};

#endif
