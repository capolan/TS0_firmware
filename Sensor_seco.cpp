////////////////////////////////////////////////////////
// Contato_seco.cpp
////////////////////////////////////////////////////////
#include "TSensor.h"

#if HAS_SENSOR_ANALOGICO == 1
uint16_t parse_seco_analogico(uint8_t porta, uint32_t espera)
{
	uint8_t j;
	uint32_t val=0;
	boolean digital=bitRead(porta,7);
	//SSdebug.print("SA=");
	//SSdebug.println(porta);
	porta &= 0x7F;
	for (j=0 ; j< MAX_SENSOR_READING ; j++) {
		if (digital) {
			val += digitalRead(porta);
		} else {
			val += analogRead(porta);
		}
		delay(espera);
	}
	return val/j;
}

#endif
#if HAS_SENSOR_SECO == 1
volatile uint8_t seco_int;
ISR(PCINT0_vect)
{
	seco_int = 1;
}

#if defined(__AVR_ATmega1284__)
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
#else
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#if DEBUG < 10 && HAS_BLUETOOTH <5 
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
#endif

/*
    // Enable the PCINT for the entire port here, but never disable it
    // (others might also need it, so we disable the interrupt by using
    // the per-pin PCMSK register).
    *digitalPinToPCICR(_receivePin) |= _BV(digitalPinToPCICRbit(_receivePin));
    // Precalculate the pcint mask register and value, so setRxIntMask
    // can be used inside the ISR without costing too much time.
    _pcint_maskreg = digitalPinToPCMSK(_receivePin);
    _pcint_maskvalue = _BV(digitalPinToPCMSKbit(_receivePin));
 */
void setup_seco_int(uint8_t _porta)
{
	volatile uint8_t *_pcint_maskreg;
	uint8_t _pcint_maskvalue; 
	seco_int=0;
	return;
	// garantir o PCMSK0 (PCINT0-PCINT (D8-D13) e PCMSK1  (A0 - A5)
	// PCMSK2  PCINT16-PCINT23 (D0-D7) 
	// PCMSK1  PCINT8 -PCINT14 (A0-A5, RESET)
	// PCMSK0  PCINT0 -PCINT7  (D8-D13, 2xcristal)
	#if USE_SENSOR_SECO_PORTA_BAIXA == 0 && HAS_BLUETOOTH >=5
	  if (_porta > 7)   
	#endif
	{
		*digitalPinToPCICR(_porta) |= _BV(digitalPinToPCICRbit(_porta));
		_pcint_maskreg = digitalPinToPCMSK(_porta);
		_pcint_maskvalue = _BV(digitalPinToPCMSKbit(_porta));
		// habilita mask
		*_pcint_maskreg |= _pcint_maskvalue;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
void setup_seco(uint8_t _nro_sensores)
{
	struct TSensor_Seco *pss=&gParam.sensor_seco;
	uint8_t i,modo,porta;
	for (i=0 ; i< MAX_SENSOR_SECO && pss->porta[i]; i++) {
		porta=pss->porta[i] & 0x7F;
		modo=bitRead(pss->tipo[i],SECO_INPUT_PULLUP_BIT)?INPUT_PULLUP:INPUT;
		bitSet(pss->valor,modo);
		pinMode(porta,modo);
		if (porta >= A0)
			digitalWrite(porta,HIGH);
		setup_seco_int(porta);
		}
	#if HAS_OLED == 1
		oled.setupSeco(_nro_sensores);
	#endif
}

///////////////////////////////////////////////////////////////
uint8_t parse_seco_one(uint8_t i)
{
	struct TSensor_Seco *pss=&gParam.sensor_seco;
	uint32_t diff;
	uint8_t j,val, old, trocou=0, porta;
	val=0;
	for (j=0 ; j< MAX_SENSOR_READING ; j++) {
		val += digitalRead(pss->porta[i] & 0x7F); // limpar bit7... 
		delay(10);
	}
	if (val && val < MAX_SENSOR_READING)
		return false;
	val = val == MAX_SENSOR_READING;
	old=bitRead(pss->valor,i);
	bitWrite(pss->valor,i,val);

	diff = millis()-pss->mark[i];
	if (old != val) {
	// informa que trocou de estado
		if (!bitRead(pss->trocou,i)) {
			bitSet(pss->trocou,i);
			}
		if (!bitRead(pss->trocou_web,i)) {
			bitSet(pss->trocou_web,i);
			trocou=1;
		}
	// marca tempos
		if (!val) {// se ficou 0 
			pss->mark[i]=millis();
		} else {
			if (diff < pss->timeout) {// 5 segundos 
				pss->freq[i]++;
				debug(pss->freq[i]);
				PGM_PRINT(" pt=");
				debug(pss->porta[i]);
				PGM_PRINT(" diff=");
				debugln(diff);
			} else
				pss->freq[i]=0;
		}
	}

	if (val && diff >= pss->timeout) {// 5 segundos
		pss->freq[i]=0;
	}
	// gravar log eventos
	#if USE_LOG_EVENTOS == 1
	if (trocou&&bitRead(ccStatus,CC_BOOT))
		grava_eventos_log(EVENTO_SENSOR_SECO,pss->porta[i], FUNC_NULL, 0L);
	#endif
	return trocou;
}

uint8_t parse_seco(uint8_t fast)
{
	struct TSensor_Seco *pss=&gParam.sensor_seco;
	uint8_t i,trocou=0;
	for (i=0 ; i< MAX_SENSOR_SECO && pss->porta[i]; i++) {
		//debug(i);
		//SSdebug.print(pss->porta[i]);
		if (bitRead(pss->tipo[i],SECO_FAST_READ_BIT) || !fast) {
			if (parse_seco_one(i) && bitRead(pss->tipo[i],SECO_IMEDIATO_BIT)) {
				trocou=1;
				#if HAS_BLUETOOTH >=2 || DEBUG > 0
				SSdebug.println("###");
				#endif
			}
	#if DEBUG > 0 || HAS_BLUETOOTH >=3
			if (!fast) {
				SSdebug.print(i);
				SSdebug.print(": pt=");
				SSdebug.print(pss->porta[i]);
				SSdebug.print(" val=");
				SSdebug.print(bitRead(pss->valor,i));
				SSdebug.print(" tr=");
				SSdebug.print(bitRead(pss->trocou,i));
				SSdebug.print(bitRead(pss->trocou_web,i));
				SSdebug.print(": mark=");
				SSdebug.print(pss->mark[i]);
				SSdebug.print(": freq=");
				SSdebug.println(pss->freq[i]);
			}
	#endif
		} // if

	} // for
	#if HAS_BLUETOOTH >=2 && DEBUG == 0
	if (trocou) {
				SSdebug.print("s=");
				SSdebug.print(pss->valor);
				SSdebug.write(' ');
				SSdebug.println(pss->trocou);
				}
	#endif
	if (seco_int) {
		#if HAS_BLUETOOTH >=2
		SSdebug.print("SINT");
		SSdebug.println(seco_int);
		#endif
		seco_int=0;
		
	}
	#if HAS_OLED == 1
	if (bitRead(ccStatus,CC_BOOT)) {
		oled.mostraSeco(pss->valor);
		//SSdebug.print("oled=");
		//SSdebug.println(pss->valor);
		}
	#endif
	return trocou;
}

////////////////////////////////////////////////////////
// Le quais porta, com parametros, sem 2 bytes
//		NNPPNNPPNNPP
//      <portas><parametro> = bit0 INPUT ou INPUTPULLUP  
//								bit1 fast read
//								bit2 avisa imediatamente
//  TI: 0101020103010401
void process_TI(char* p)
{
	struct TSensor_Seco *pss=&gParam.sensor_seco;
	uint8_t porta,tipo,i;
	for (i=0; i< MAX_SENSOR_SECO && *p; i++) {
		//SSdebug.println(p);
		porta=(htoi(*p++)<<4) + htoi(*p++);
		tipo=(htoi(*p++)<<4) + htoi(*p++);
		#if HAS_BLUETOOTH >= 3 || DEBUG>0
		SSdebug.print(porta);SSdebug.write(' ');
		SSdebug.println(tipo);
		#endif
		if (!porta || (porta&0x7f) > A5)  {
			porta=0;
			}
		pss->porta[i]=porta;
		pss->tipo[i]=tipo;
	}
	setup_seco(i);
	bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
}

#endif