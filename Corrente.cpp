////////////////////////////////////////////////////////
// CORRENTE.h
////////////////////////////////////////////////////////
#include "TSensor.h"
//#include "EmonLib.h"                   // Include Emon Library

uint16_t correnteFase[3];

#define TESTAR_SENSORES 0

#if HAS_CORRENTE == 1

#if HAS_EMONLIB == 1
#include "EmonLib.h"
EnergyMonitor emon[MAX_CORRENTE_FASES];                   // Create an instance

#else
#if defined(__arm__)
#define ADC_BITS    12
#else
#define ADC_BITS    10
#endif

#define ADC_COUNTS  (1<<ADC_BITS)
//#define ADC_COUNTS  0b010000000000

#endif

uint8_t CORRENTE_PIN[] = {CORRENTE_1,CORRENTE_2,CORRENTE_3};


long readVcc() {
	return emon[0].readVcc();
}

///////////////////////////////////////////////////
// IRMS
///////////////////////////////////////////////////
#if HAS_EMONLIB == 0

#if 1

double readCalcIrms(int pin, int NUMBER_OF_SAMPLES)
{
int lastSampleI,sampleI;
double lastFilteredI, filteredI, Irms;
double sqI,sumI=0;              //sq = squared, sum = Sum, inst = instantaneous

	double ICAL = gParam.rede.calibragemFase1;
	#if USE_REAL_VCC == 1
	int SUPPLYVOLTAGE = readVcc();
	#else
	int SUPPLYVOLTAGE = gParam.rede.vcc_supply;
	#endif
	
	filteredI = 0; //ADC_COUNTS>>1;
	sampleI = analogRead(pin);
	for (int n = 0; n < NUMBER_OF_SAMPLES; n++)
	{
		lastSampleI = sampleI;
		sampleI = analogRead(pin);
		lastFilteredI = filteredI;
		filteredI = 0.996*(lastFilteredI+sampleI-lastSampleI);

		// Root-mean-square method current
		// 1) square current values
		sqI = filteredI * filteredI;
		// 2) sum
		sumI += sqI;
	}

	double I_RATIO = ICAL *((SUPPLYVOLTAGE/1000.0) / (ADC_COUNTS));
	Irms = I_RATIO * sqrt(sumI / NUMBER_OF_SAMPLES);

	//Reset accumulators
	sumI = 0;
	//--------------------------------------------------------------------------------------
	
	return Irms;
}

#else
double readCalcIrms(int pin, int Number_of_Samples)
{
	int sampleI;
	double filteredI, offsetI, Irms;
	double sqI,sumI=0;              //sq = squared, sum = Sum, inst = instantaneous
	double ICAL = gParam.rede.calibragemFase1;
	#if USE_REAL_VCC == 1
	int SupplyVoltage = readVcc();
	#else
	int SupplyVoltage = gParam.rede.vcc_supply;
	#endif
	
	offsetI = ADC_COUNTS>>1;
	for (unsigned int n = 0; n < Number_of_Samples; n++)
	{
		sampleI = analogRead(pin);

		// Digital low pass filter extracts the 2.5 V or 1.65 V dc offset,
		//  then subtract this - signal is now centered on 0 counts.
		offsetI = (offsetI + (sampleI-offsetI)/1024);
		filteredI = sampleI - offsetI;

		// Root-mean-square method current
		// 1) square current values
		sqI = filteredI * filteredI;
		// 2) sum
		sumI += sqI;
	}

	double I_RATIO = ICAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
	Irms = I_RATIO * sqrt(sumI / Number_of_Samples);

	//Reset accumulators
	//sumI = 0;
	//--------------------------------------------------------------------------------------
	
	return Irms;
}
#endif
#endif

///////////////////////////////////////////////////////////////////////
//  Testar existencia dos sensores
///////////////////////////////////////////////////////////////////////
#if TESTAR_SENSORES == 1
void testarSensores(void) {
	uint16_t ret;
	if (bitRead(gParam.rede.fases,B_FORCE_FASE)) return;
	ret=analogRead(CORRENTE_1) > 100;
	bitWrite(gParam.rede.fases,B_FASE_1,ret);
	ret = analogRead(CORRENTE_2) > 100;
	bitWrite(gParam.rede.fases,B_FASE_2,ret);
	#if MAX_CORRENTE_FASES < 3
	bitClear(gParam.rede.fases,B_FASE_3);
	#else
	ret = analogRead(CORRENTE_3) > 100;
	bitWrite(gParam.rede.fases,B_FASE_3,ret);
	#endif
	//debug(ret);
	//PGM_PRINT("  sens=");
	//debugln(gParam.rede.fases);
}
#endif
///////////////////////////////////////////////////////////////////////
// Ler Corrente Gerada
// PARSE
/////////////////////////////////////////////////////////////////////
	
void parse_corrente(void)
{
	double Irms;
	uint16_t amostras=gParam.rede.nro_amostras_corrente;
	uint8_t bit_fase;
	uint8_t fases=gParam.rede.fases;
	#if TESTAR_SENSORES == 1
	testarSensores();
	#endif
	#if DEBUG > 0
	long tempoCorrente=millis();
	//SSdebug.write('#');
	#endif
	uint8_t tensao;
	tensao=bitRead(fases,B_TENSAO)?TENSAO_220:TENSAO_127;
	//Irms=0;
	#if HAS_BLUETOOTH > 0
	SSdebug.print("fases=");
	SSdebug.println(fases,BIN);
	#endif
	memset(correnteFase,0,3);
	for (bit_fase=0; bit_fase<MAX_CORRENTE_FASES ; bit_fase++) {
//		emon.current(CORRENTE_PIN[bit_fase], gParam.rede.calibragemFase1);             // Current: input pin, calibration.
//		emon.current(A0, gParam.rede.calibragemFase1);             // Current: input pin, calibration.
		if (bitRead(fases,bit_fase) && !bitRead(fases,B_PORTA_A0+bit_fase)) {
			#if HAS_EMONLIB == 1
			//cli();
//			Irms = emon.calcIrms(amostras);  // Calculate Irms only
			Irms = emon[bit_fase].calcIrms(amostras);  // Calculate Irms only
			//sei();
			#else
			Irms = readCalcIrms(CORRENTE_PIN[bit_fase], amostras);
			#endif
			correnteFase[bit_fase] = (uint16_t)(Irms * tensao);
			//correnteFase1 = constrain(correnteFase1,0,9999);
			#if DEBUG > 0
			debugvar(Irms);
			debugln(correnteFase[bit_fase]);
			#endif
			#if HAS_OLED == 1
				oled.setField(bit_fase+1,correnteFase[bit_fase]);
			#endif
		}
	}

	#if DEBUG >0
	tempoCorrente= millis()-tempoCorrente;
	debugvarln(tempoCorrente);
	#if HAS_EMONLIB == 1
	//int vcc=emon1.readVcc();
	//debugvar(vcc);
	#endif
	//debugvar(Irms);
	#endif
	//debugln(correnteFase[0]);
}

///////////////////////////////////////////////////////////////////////////////////////
// SETUP
///////////////////////////////////////////////////////////////////////////////////////////////
void setup_corrente(void) {
	for (uint8_t i=MAX_CORRENTE_FASES;i;i--)
		emon[i-1].current(CORRENTE_PIN[i-1], gParam.rede.calibragemFase1);             // Current: input pin, calibration.
	return;
}

#endif