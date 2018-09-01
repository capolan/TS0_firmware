// https://www.analysir.com/blog/2014/10/03/driving-infrared-led-directly-arduino-pin/
// http://www.vishay.com/docs/81009/tsal6100.pdf   IR spec


#include "TSensor.h"

#if HAS_IR_REMOTE == 1
//#include "IRremoteInt.h"
#include <IRremote/IRremoteInt.h>
//khz:nro_bits:repeticoes:rep_interval:START_PULSE:START_PAUSE:IR_PULSE_LEN:IR_PAUSE_LOW:IR_PAUSE_HIGH:cmd1-6bytes:cmd2-6bytes
// 1     1        1          1            2           2          2               2            2             6           6
// total de 26 bytes =  56 HEX (*2)
/*
byte IrCmd1[] = {0b10110010,0b01001101,0b00111111,0b11000000,0b00100000,0b11011111}; // 20C ON  COOL fan HIGH AIR HITACHI

byte IrCmd24[] = {0b10110010,0b01001101,0b00111111,0b11000000,0b01000000,0b10111111}; // 24C ON  COOL fan HIGH AIR HITACHI

byte IrCmd2[] = {0b10110010,0b01001101,0b01111011,0b10000100,0b11100000,0b00011111}; // 20C OFF COOL fan HIGH AIR HITACHI
uint8_t IrBits = sizeof(IrCmd1) * 8 ;
uint8_t khz = 38;

int IR_START_PULSE = 4500;
int IR_START_PAUSE = 4300;
int IR_PULSE_LEN = 700;
int IR_PAUSE_HIGH = 1500;
int IR_PAUSE_LOW = 450;
uint8_t IR_REPETITION = 2;
int IR_REPETITION_INTERVAL = IR_START_PULSE + 1000;
*/

// no atmega328p somente os pinos3 = timer2 ou pino9 = timer1
const int IRPIN = TIMER_PWM_PIN; //3 atmega329  9 atmega2560;

//*********************************************************************************************
#if DEBUG > 10
void parse_ir(){
	// send control pulses
	if (stringComplete) {
		Serial.print("sending...");
		Serial.println(inputString);
		switch (inputString[0]) {
			case '0':
			sendBlock(IrCmd2, sizeof(IrCmd2));
			break;
			case '1':
			case '2':
			sendBlock(IrCmd1, sizeof(IrCmd1));
			break;
			case '4':
			sendBlock(IrCmd24, sizeof(IrCmd24));
			break;
		}
		stringComplete=false;
	}
}
#endif
//+=============================================================================
// Custom delay function that circumvents Arduino's delayMicroseconds limit

void custom_delay_usec(unsigned long uSecs) {
	if (uSecs > 4) {
		unsigned long start = micros();
		unsigned long endMicros = start + uSecs - 4;

		if (endMicros < start) { // Check if overflow
			while ( micros() > start ) {} // wait until overflow
		}
		while ( micros() < endMicros ) {} // normal wait
	}
	//else {
	//  __asm__("nop\n\t"); // must have or compiler optimizes out
	//}
}

//+=============================================================================
// Sends an IR mark for the specified number of microseconds.
// The mark output is modulated at the PWM frequency.
//
void  mark (unsigned int time)
{
	#if DEBUG == 1
	Serial.print(time);
	Serial.print(",");
	#endif
	if (time > 0) {
		TIMER_ENABLE_PWM; // Enable pin 3 PWM output
		custom_delay_usec(time);
	}
}

//+=============================================================================
// Leave pin off for time (given in microseconds)
// Sends an IR space for the specified number of microseconds.
// A space is no output, so the PWM output is disabled.
//
void  space (unsigned int time)
{
	#if DEBUG == 1
	Serial.print(-(int)time);
	Serial.print(",");
	#endif

	TIMER_DISABLE_PWM; // Disable pin 3 PWM output
	if (time > 0) custom_delay_usec(time);
}

//+=============================================================================
// Enables IR output.  The khz value controls the modulation frequency in kilohertz.
// The IR output will be on pin 3 (OC2B).
// This routine is designed for 36-40KHz; if you use it for other values, it's up to you
// to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
// TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
// controlling the duty cycle.
// There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
// To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
// A few hours staring at the ATmega documentation and this will all make sense.
// See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.
//
void  enableIROut (int khz)
{
	// Disable the Timer2 Interrupt (which is used for receiving IR)
	TIMER_DISABLE_INTR; //Timer2 Overflow Interrupt

	pinMode(IRPIN, OUTPUT); // pinMode(TIMER_PWM_PIN, OUTPUT);
	digitalWrite(IRPIN, LOW); // When not sending PWM, we want it low

	// COM2A = 00: disconnect OC2A
	// COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
	// WGM2 = 101: phase-correct PWM with OCRA as top
	// CS2  = 000: no prescaling
	// The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
	TIMER_CONFIG_KHZ(khz);
}


//*********************************************************************************************
// idx : indice do gParam.ir.cmd[]
//********************************************************************************************
void send_ir_command(uint8_t idxIR){
	// send control pulses
	uint8_t nroBits;
	struct TPIR *pir=&gParam.ir;

	PGM_PRINTLN(">IR");

	enableIROut(pir->khz);

	for (uint8_t l=0 ; l < gParam.ir.repetition ; l ++) {
		nroBits  = pir->nro_bits;
		// BLOCK 1
		// long start pulse
		mark(pir->start_pulse);
		// long start delay
		space(pir->start_pause);
		// send data
		for (int i=0; i < 6; i++ && nroBits){
			for (char b=7 ; b>=0 ; b--, nroBits--) {
				// regular pulse before data
				mark(pir->pulse_len);
				//send bit (pause)
				if (bitRead(pir->cmd[idxIR][i],b)){
					space(pir->pause_high);
					} else {
					space(pir->pause_low);
				}
			}
		}
		// regular pulse before data
		mark(pir->pulse_len);
		space(pir->repetition_interval);

		#if DEBUG == 1
		Serial.print("#");
		#endif
	}

	setup_timer();
	//space(0);  // Always end with the LED off
	// repeat code every 5 seconds for demonstration	delay(2500);
}


#endif