/**
 * @file Botao_tsensor.cpp
 * @author CAP
 * @date 9 Set 2016
 * @brief Controle do uso do botão interno. Tem funcoes de reset de fw, e e reset do IP do servidor.
 * Apos iniciado, ao pressionar, envia pacote de dados para servidor.
 *
 */

#include "TSensor.h"
#include <stddef.h>

//#include "Mirf.h"

#if HAS_BOTAO == 1
#define BTN_ATUAL 0 ///< botao nao acionado
#define BTN_PRESS_CURTO 1  ///< botao pressionado com temp curto
#define BTN_PRESS_LONGO 2 ///< botao pressionado com tempo longo

#define BTN_DEBOUNCE 30   ///< deboucing de 30ms
#define BTN_TEMPO_CURTO 300  ///< botao curto detectado por pressão menos que 300ms
#define BTN_TEMPO_LONGO 700   ///< botao pressionado por tempo longo >700ms

volatile unsigned long tButton1;
volatile uint8_t button1; /**< button1 bits que indical a situacao do botao, ver macros BTN_*. Usado na interrupção. */

/**
 * @brief inicializacao do botao em PULLUP. Utiliza interrupcao para determinar a click
 */
void setup_botao(void){
	//cli();//stop interrupts
	pinMode(BOTAO_PIN,INPUT_PULLUP);
	button1 = 0;
	// PCINT 2 botão
	#if BOTAO_PIN == 10
	PCICR |= bit(PCIE0);  // habilita PCIE0  PCINT7:0
	PCMSK0 |= bit(PCINT2); // habilita mascara
	#elif BOTAO_PIN == 2
	#if defined __AVR_ATmega1284__
	EICRA |= (1 << ISC20);    // set INT0 to trigger on ANY logic change
	EIMSK |= (1 << INT2);     // Turns on INT0
	#else
	EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
	EIMSK |= (1 << INT0);     // Turns on INT0
	#endif
	#endif
	//sei();
}

/// Parse botao
int parse_botao(void) {
	uint8_t btn;
	if (bitRead(button1,BTN_ATUAL))
		btn=0;
	else {
		btn=button1;
		button1 &= ~(bit(BTN_PRESS_CURTO) | bit(BTN_PRESS_LONGO));
	}
	if (btn) debugln(btn);
	#if TESTE_RELE == 1
	if (bitRead(button1,BTN_ATUAL)) {
		digitalWrite(6,HIGH);
		digitalWrite(10,HIGH);
	} else {
		digitalWrite(6,LOW);		
		digitalWrite(10,LOW);
	}
	#endif

	return btn;
}

/**
 * @brief Rotina de interrupcao
 */
#if BOTAO_PIN == 10
ISR(PCINT0_vect)
#elif BOTAO_PIN == 2
ISR(INT0_vect)
#endif
{
	unsigned long mark;
	if (BUTTON_READ) { // botão solto
		if (bitRead(button1,BTN_ATUAL)) {
			mark=millis()-tButton1;
			if (mark > BTN_DEBOUNCE)
			if (mark < BTN_TEMPO_CURTO) bitSet(button1,BTN_PRESS_CURTO);
			else
				if (mark > BTN_TEMPO_LONGO) bitSet(button1,BTN_PRESS_LONGO);
			bitClear(button1,BTN_ATUAL);
		}
	}
	else { // botão pressionado
		if (!bitRead(button1,BTN_ATUAL)) {
			tButton1=millis();
			bitSet(button1,BTN_ATUAL);
		}
	}
}

#endif