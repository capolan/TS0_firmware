#include "TSensor.h"

#if HAS_OLED == 1
#include "Oled_display.h"
#endif

// https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html
// https://www.maximintegrated.com/en/app-notes/index.mvp/id/148
// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

// Error Codes
#define DEVICE_NOT_FOUND -1000
#define DEVICE_DISCONNECTED_C -127
#define DEVICE_DISCONNECTED_F -196.6
#define DEVICE_DISCONNECTED_RAW -7040

uint16_t humidade=0;

#if HAS_TEMPERATURA == 1

struct TDS20B18 *pTemp=NULL;
//void Temp_create(uint8_t sens, byte addr[8], OneWire *dsx) {
void Temp_create(uint8_t addr[], OneWire *dsx, uint8_t porta) {
	struct TDS20B18 *pAux;
	// contar nro de vezes que foi chamada;
	if (humidade++>8) {
		delete(dsx);
		return;
	}
	pAux = (struct TDS20B18 *)malloc(sizeof(struct TDS20B18));
	memcpy(pAux->addr,addr,8);
	pAux->porta=porta;
	pAux->ds = dsx;
	pAux->p = pTemp;
	pTemp=pAux;
}

boolean Temp_search(uint8_t pin) {
	struct TDS20B18 *pAux=pTemp;
	while (pAux) {
		if (pAux->porta == pin)
			return true;
		pAux=pAux->p;
	}
	return false;
}

#if HAS_DS18B20 == 1 || HAS_DS18B20_EXTRA == 1
#include <OneWire.h>
//#include <DallasTemperature.h>
//
bool readPowerSupply( OneWire *_wire, const uint8_t* deviceAddress){

	bool ret = false;
	_wire->reset();
	_wire->select(deviceAddress);
	_wire->write(READPOWERSUPPLY);
	if (_wire->read_bit() == 0) ret = true;
	_wire->reset();
	return ret;

}
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
///DallasTemperature sensors(&oneWire);

/* Calculates CRC based upon Maxim's 8 bit polynimia X^8 + X^5 + X^4 + 1.
Data is read starting at the least significant bit.
thepiandi.blogspot.com MJL  071014
http://thepiandi.blogspot.com.br/2014/07/cyclic-redundancy-check-crc-of-ds18b20.html
*/

boolean setup_real_Temperature(uint8_t porta)
{
	//	PGM_PRINTLN("Dallas Temperature IC Control");
	//	sensors.begin
	PGM_PRINTLN(">setup_DS18B20");
	OneWire *pds;
	byte addr[8];
	boolean achou=false, power;
	if (Temp_search(porta)) return true;
	
	pds=new OneWire(porta);
	while ( pds->search((uint8_t *)addr)) {
		#if DEBUG > 0
	  SSdebug.println();
		for(int i = 0; i < 8; i++) {
			PGM_PRINT("0x");
			if (addr[i] < 16) {
				SSdebug.write('0');
			}
			SSdebug.print(addr[i], HEX);
			if (i < 7) {
				PGM_PRINT(", ");
			}
		}
		SSdebug.write('\n');
		#endif

		if (OneWire::crc8(addr, 7) == addr[7]) {
			Temp_create(addr, pds, porta);
			/*power=readPowerSupply(pds,addr);
			SSdebug.print("pw=");
			SSdebug.println(power);
			*/achou=true;
			} else {
			PGM_PRINTLN("CRC Err");
		}

	}
	
	pds->reset_search();
	delay(250);
	#if DEBUG >=0
	
	uint8_t type_s;
	// the first ROM byte indicates which chip
	switch (addr[0]) {
		case 0x10:
		PGM_PRINTLN("  Chip = DS18S20");  // or old DS1820
		type_s = 1;
		break;
		case 0x28:
		PGM_PRINTLN("  Chip = DS18B20");
		type_s = 0;
		break;
		case 0x22:
		PGM_PRINTLN("  Chip = DS1822");
		type_s = 0;
		break;
		default:
		PGM_PRINTLN("Not a DS18x20");
	}
	#endif
	if (!achou)
		delete(pds);
	return achou;
}
/***************************************/
int readTemperatureDS18B20(struct TDS20B18 *pAux)
{
	byte cfg,data[12];
	OneWire *pds;
	int raw;
	
	if (pAux==NULL) {
		return DEVICE_NOT_FOUND;
	}
		pds=pAux->ds;
	if (!pds->reset())
		return DEVICE_DISCONNECTED_C;
			
		pds->select(pAux->addr);
		pds->write(STARTCONVO, 1);        // start conversion, with parasite power on at the end
		
		delay(188);     // maybe 750ms (12bits) is enough, maybe not  187 10bits   375b  11 bits 94  9bits 
		// we might do a ds.depower() here, but the reset will take care of it.
		
		pds->reset();
		pds->select(pAux->addr);
		pds->write(READSCRATCH);         // Read Scratchpad

		for (uint8_t i = 0; i < 9; i++) {           // we need 9 bytes
			data[i] = pds->read();
		}
		// Convert the data to actual temperature
		// because the result is a 16 bit signed integer, it should
		// be stored to an "int16_t" type, which is always 16 bits
		// even when compiled on a 32 bit processor.
		raw = (data[TEMP_MSB] << 8) | data[TEMP_LSB];

		cfg = (data[CONFIGURATION] & 0x60);
		//debugvarln(cfg);
		// at lower res, the low bits are undefined, so let's zero them
		if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
		else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
		else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
		//// default is 12 bit resolution, 750 ms conversion time

		pAux->temperatura=raw=((raw*10) >>4);
		if (cfg != 0x20) { // 10 bits
			pds->reset();
			pds->select(pAux->addr);
			pds->write(WRITESCRATCH);        // write scratchpad configuracao
			pds->write(0);
			pds->write(0);
			//	   ds.write((data[4] | 0x20) & 0xBF);
			pds->write(TEMP_10_BIT);
			pds->reset();
		}

	//return (float)(raw+(signed byte)(gParam.rede.vcc_supply >> 8)) / 16.0;
	// return (float)(raw) / 16.0;
	//   int cru=(raw*10) >>4;
	// debugvarln(raw);
	//return ((raw*10) >>4) +gParam.rede.vcc_supply;
	return raw;
}

int readTemperatureDHT(uint8_t);
/////////////////////////////////////////////////////////////
void setup_temp_fases(uint8_t addr[] )
{
	uint8_t fases = gParam.rede.fases;
	boolean achou;
	if (bitRead(fases,B_PORTA_A0)) {
		achou=setup_real_Temperature(A0);
		#if HAS_DHT == 1
		addr[0]=2;
		debugln(achou);
		delay(1000);
		if (readTemperatureDHT(A0) > DEVICE_NOT_FOUND) {
			PGM_PRINTLN(">>>");
			Temp_create(addr, NULL, A0);
		}
		#endif
	}
	if (bitRead(fases,B_PORTA_A1)) {
		achou=setup_real_Temperature(A1);
	}
	if (bitRead(fases,B_PORTA_A2)) {
		achou=setup_real_Temperature(A2);
	}

}

#endif
///////////////////////////////////////////////////////////
// LM35
//////////////////////////////////////////////////////////

#if HAS_LM35 == 1
#define SAMPLES_TEMP 100
#define CELSIUS_BASE 0.4887585532746823069403714565

int readTemperatureLM35(void)
{
	#if USE_REAL_VCC == 1
	uint16_t Vref=readVcc(); //.0; // 1100 se INTERNAL1V1
	#else
	uint16_t Vref=gParam.rede.vcc_supply;
	#endif
	int j;
	double Vin=0;
	uint16_t sample;
	for(j=0 ; j<SAMPLES_TEMP ; j++)
	{
		sample = analogRead(TEMPERATURA_AMBIENTE_PIN);    // Each sample is a value from 0 to 1023. Reading "j" values will help making the reading more accurate.
		Vin += sample;
		//Vin = Vin + ( sample * Vref/1024.0);  // Here you convert from ADC to miliVolt -> Voltage = ADC*Vref/1024. ADC = [0;1023]
		// For a 10-bit ADC with an 1100mV reference, the most you can measure without saturating the ADC would be (1100mV - 1100mV/1024) = 1098.9mV.
		// In other words, a reading of 1023 from your ADC equals (1023 * 1100/1024mV) equals 1098.9 mV.
		// Remember, 10 bits can only represent a value as large as b1111111111 or decimal 1023.
	}
	Vin=Vin/SAMPLES_TEMP;
	#if 0
	debugln(Vref);
	debugln(sample);
	debugln(Vin);
	#endif
	Vin=Vin*((double)(Vref)/1024);
	return Vin;
}
#endif

#if HAS_DHT == 1

#include <dht.h>
dht DHT;

int readTemperatureDHT(uint8_t pin)
{
	#if HAS_DHT11 == 1
	int chk=DHT.read11(pin);
	#elif HAS_DHT22 == 1
	int chk=DHT.read22(pin);
	#else
	return DEVICE_NOT_FOUND; // forçar erro
	#endif;
	#if DEBUG > 0
	switch (chk)
	{
		case DHTLIB_OK:
				PGM_PRINTLN("OK,\t");
//		debug(DHT.humidity);
//		debugln(DHT.temperature);
		break;
		case DHTLIB_ERROR_CHECKSUM:
		PGM_PRINT("Checksum err");
		break;
		case DHTLIB_ERROR_TIMEOUT:
		PGM_PRINT("Time out err");
		break;
		default:
		PGM_PRINT("Unknown err");
		break;
	}
	#endif
	if (chk != DHTLIB_OK) {
		DHT.humidity = DHT.temperature = DEVICE_NOT_FOUND/10;
		PGM_PRINTLN(" DHT err");
	} 
	return (int)(DHT.temperature*10);
}

int readHumidity(void) {
	return (int)(DHT.humidity * 10);
}
#endif


// variavel temperatura usada para para contar quantas vezes a 
// procedure Temp_create foi usada;
/////////////////////////////////////////////////////////////
void setup_Temperature(void)
{
	uint8_t addr[8] = {0,0,0,0,0,0,0,0};
	humidade=0;
	#if HAS_DS18B20 == 1
	setup_real_Temperature(TEMPERATURA_AMBIENTE_PIN);
	#endif
	#if HAS_DS18B20_EXTRA == 1
	setup_real_Temperature(TEMPERATURA_AMBIENTE_2_PIN);
	#endif
	#if HAS_DS18B20_EXTRA_2 == 1
	setup_real_Temperature(TEMPERATURA_AMBIENTE_3_PIN);
	#endif
	#if HAS_DS18B20 == 1
	setup_temp_fases(addr);
	#endif
	#if HAS_LM35 == 1
	addr[0]=1;
	Temp_create(addr, NULL, TEMPERATURA_AMBIENTE_PIN);
	#endif
	#if HAS_DHT == 1
	addr[0]=2;
	delay(1000);
	if (readTemperatureDHT(TEMPERATURA_DHT_PIN) > DEVICE_NOT_FOUND)
		Temp_create(addr, NULL, TEMPERATURA_AMBIENTE_PIN);
	#endif
}

void readTemperature(void)
{
	struct TDS20B18 *pAux=pTemp;
	uint8_t oled_idx=5;int oled_val;
	while (pAux != NULL) {
		#if HAS_DS18B20 == 1 || HAS_DS18B20_EXTRA == 1
		if (pAux->ds) {
			oled_val=readTemperatureDS18B20(pAux);
		} 
		#endif
		#if HAS_LM35 == 1 || HAS_DHT == 1
		if (!pAux->ds) {
			switch (pAux->addr[0]) {
				#if HAS_LM35 == 1
				case 1: // LM35
				oled_val=pAux->temperatura=readTemperatureLM35();
				break;
				#endif
				#if HAS_DHT == 1
				case 2: // DHT11
				oled_val=pAux->temperatura=readTemperatureDHT(pAux->porta);
				#if HAS_OLED == 1
				oled.setField(oled_idx++,DHT.humidity,'%');
				#endif
				break;
				#endif
			} // switch
		}
		#endif
		#if DEBUG > 0 || HAS_BLUETOOTH >=1
		SSdebug.write('T');
		SSdebug.print(pAux->temperatura);
		#endif
		#if HAS_OLED == 1
		oled.setField(oled_idx++,oled_val/10,'c');
		if (oled_idx > 9) oled_idx=1;
		#endif
		pAux=pAux->p;
	}
}
#endif


