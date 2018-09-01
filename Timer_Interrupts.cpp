/**
 * @file Timer_Interrupts.cpp
 * @author CAP
 * @date 1 Dez 2015
 */
#include "TSensor.h"

#if HAS_TIMER_INT == 1
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

/// Rotina de interrupção para ligar/apagar leds
#if USE_TIMER2 == 1
ISR(TIMER2_COMPA_vect)
#else // USE_WATCHDOG_TIMER ==1
ISR(WDT_vect)
#endif
{
	uint8_t i;
	timer2++;
	#if USE_LEDS == 1
	switch (led1_status) {
		case LED_APAGA: LED_OFF;
		break;
		case LED_LIGAR: LED_ON;
		break;
		default:
		if (!(timer2 % led1_status)) {
			if (LED_READ) {
				LED_OFF;
				}else{
				LED_ON;
			}
		}
	}
	
	switch (led2_status) {
		case LED_APAGA: LEDV_OFF;
		break;
		case LED_LIGAR: LEDV_ON;
		break;
		default:
		if (!(timer2 % led2_status)) {
			if (LEDV_READ) {
				LEDV_OFF;
				}else{
				LEDV_ON;
			}
		}
	}
	#endif

	#if HAS_SENSOR_PORTA_9 == 1
	i=SENSOR_9_READ > 0;
	if (bitRead(sensor_9_10,SENSOR_9_POSICAO) != i) {
		bitWrite(sensor_9_10, SENSOR_9_POSICAO, i);
		bitSet(sensor_9_10,SENSOR_9_TROCOU);
		timerWeb=0;
	}
	#endif
	#if HAS_SENSOR_PORTA_10 == 1
	i=SENSOR_10_READ > 0;
	if (bitRead(sensor_9_10,SENSOR_10_POSICAO) != i) {
		bitWrite(sensor_9_10, SENSOR_10_POSICAO, i);
		bitSet(sensor_9_10,SENSOR_10_TROCOU);
		timerWeb=0;
	}
	#endif
}


#if USE_TIMER1 == 1
ISR(TIMER1_COMPA_vect)
{
	bitSet(ccStatus,CC_TICK);
	timer1++;
}
#endif

///////////////////////////////////////////////
// setup_timer()
///////////////////////////////////////////////
void setup_timer() {
	cli();//stop interrupts
	#if USE_WATCHDOG_TIMER == 1
	/* Clear MCU Status Register. Not really needed here as we don't need to know why the MCU got reset. page 44 of datasheet */
	MCUSR = 0;
	/* Disable and clear all Watchdog settings. Nice to get a clean slate when dealing with interrupts */
	WDTCSR = (1<<WDCE)|(1<<WDE);
	// Setup Watchdog for interrupt and not reset, and a approximately 500ms timeout P.45
	//WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP0);
	WDTCSR =  (1<<WDIE) | WDTO_30MS;
	#endif

	#if USE_TIMER2 == 1
	//set timer2 interrupt at 8kHz
	TCCR2A = 0;
	TCCR2B = 0;// same for TCCR2B
	//TCNT2  = 0;//initialize counter value to 0
	// set compare match register for 8Khz
	// OCR2A = OCR2A_COUNT;// = (16*10^6) / (125*8) - 1 (must be <256)

	// turn on Fast PWM mode
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	//TCCR2A |= (1 << WGM21);
	// 1024 prescaler, Fast PWM
	TCCR2B |= (1 << CS20)|(1 << CS21)|(1 << CS22);
	// enable timer compare interrupt
	TIMSK2 |= (1 << OCIE2A); // timer de 1 segundo
	//	TIMSK2 |= (1 << OCIE2B);
	timer2=0;
	#endif

	// 1segundo
	#if USE_TIMER1 == 1
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;//initialize counter value to 0
	// set compare match register for 1hz
	OCR1A = 15624;// = (16*10^6) / (125*8) - 1 (must be <256)
	// turn on CTC mode pag 160  http://www.atmel.com/images/doc2549.pdf
	TCCR1B |= (1 << WGM12);
	// Set 1024 prescaler pag 161
	TCCR1B |= (1 << CS12) | (1 << CS10);
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	timer1=0;
	#endif

	
	sei();//allow interrupts

}


#if USE_REAL_VCC == 1 && HAS_CORRENTE == 0
long readVcc() {
	long result;
	//not used on emonTx V3 - as Vcc is always 3.3V - eliminates bandgap error and need for calibration http://harizanov.com/2013/09/thoughts-on-avr-adc-accuracy/

	#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
	ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	ADCSRB &= ~_BV(MUX5);   // Without this the function always returns -1 on the ATmega2560 http://openenergymonitor.org/emon/node/2253#comment-11432
	#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
	ADMUX = _BV(MUX5) | _BV(MUX0);
	#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	ADMUX = _BV(MUX3) | _BV(MUX2);
	
	#endif


	#if defined(__AVR__)
	delay(2);                                        // Wait for Vref to settle
	ADCSRA |= _BV(ADSC);                             // Convert
	while (bit_is_set(ADCSRA,ADSC));
	result = ADCL;
	result |= ADCH<<8;
	result = 1126400L / result;                     //1100mV*1024 ADC steps http://openenergymonitor.org/emon/node/1186
	return result;
	#elif defined(__arm__)
	return (3300);                                  //Arduino Due
	#else
	return (3300);                                  //Guess that other un-supported architectures will be running a 3.3V!
	#endif
}
#endif

#endif // HAS_TIMER_INT