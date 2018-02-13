/**
*  Modbus master example 1:
*  The purpose of this example is to query an array of data
*  from an external Modbus slave device.
*  The link media can be USB or RS232.
*
*  Recommended Modbus slave:
*  diagslave http://www.modbusdriver.com/diagslave.html
*
*  In a Linux box, run
*  "./diagslave /dev/ttyUSB0 -b 19200 -d 8 -s 1 -p none -m rtu -a 1"
* 	This is:
* 		serial port /dev/ttyUSB0 at 19200 baud 8N1
*		RTU mode and address @1
*/
//#define REAL

#include "TSensor.h"

#if HAS_MODBUS == 1

#ifdef REAL
#include <ModbusRtu.h>
#define PPP 3
#else
#include "ModbusRtu_SoftwareSerial.h"
#include <SoftwareSerial.h>
SoftwareSerial sserial(A4,A5);
#define PPP 4
#endif

#ifndef MODBUS_ENABLE_PIN
#define MODBUS_ENABLE_PIN 2
#endif



/**
*  Modbus object declaration
*  u8id : node id = 0 for master, = 1..247 for slave
*  u8serno : serial port (use 0 for Serial)
*  u8txenpin : 0 for RS-232 and USB-FTDI
*               or any pin number > 1 for RS-485
*/
Modbus master(0,PPP,MODBUS_ENABLE_PIN); // this is master and RS-232 or USB-FTDI

/**
* This is an structe which contains a query to an slave device
*/



void setup_modbus(void) {
	#ifdef REAL
	master.begin( 9600 ); // baud-rate at 19200
	#else
	master.begin(&sserial, 9600 ); // baud-rate at 9600
	#endif
	master.setTimeOut( 1000 ); // if there is no answer in 2000 ms, roll over

	#if DEBUG > 0 && HAS_ESP8266 == 0
	PGM_PRINTLN(">pTU");
	process_TU("38010300000008");
	#endif
}
///////////////////////////////////////////////////////////////
// retorno
//   0 : erro na leitura
//   1 : ok e tem mais emndereços para ler
//   2 : ok e fim de endereços
///////////////////////////////////////////////////////////////
uint8_t parse_one_modbus_addr(uint8_t idx, uint8_t *i, uint16_t *au16data, uint8_t *dado) {
	modbus_t telegram;
	struct TPModbus *mb=&gParam.mb[idx];
	unsigned long u32wait=millis();
	uint8_t u8state=1, ret=0;
	debugvar(idx);
	debugvar(*i);
	debugvar(*dado);
	sserial.listen();
	while (u8state && millis() - u32wait < 1000) {
		switch( u8state ) {
			case 1:
			PGM_PRINTLN(">mb s");
			telegram.u8id = mb->slave; // slave address
			telegram.u8fct = mb->comando; // function code (this one is registers read)
			telegram.u16RegAdd = mb->addr[(*i)++]; // start address in slave
			//debugvar(telegram.u16RegAdd);
			if (mb->tipo) {
				*dado +=mb->addr[*i]; 
				telegram.u16CoilsNo = min(mb->addr[(*i)++],MAX_TSENSOR_SENSOR); // number of elements (coils or registers) to read
			} else {
				(*dado)++;
				telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read				
			}
			debugvar(*dado);
			telegram.au16reg = au16data; // pointer to a memory array in the Arduino
			master.query( telegram ); // send query (only once)
			u8state++;
			break;
			case 2:
			//PGM_PRINTLN(">mb p");
			master.poll(); // check incoming messages
			if (master.getState() == COM_IDLE) {
				u8state = 0;
				ret=1;
				#if DEBUG>0
				for (uint8_t i=0;i<telegram.u16CoilsNo;i++){
					debugln(au16data[i]);
				}
				#endif
			}
			break;
		} // switch
	}
	if (ret && mb->addr[*i] == 0xFFFF)
		ret++;  // fim dos endereços para ler
	#if DEBUG > 0
	SSdebug.listen();
	#endif
	PGM_PRINTLN("<mb");
	return ret;
}

///////////////////////////////////////////////////////////////
uint8_t parse_modbus(void) 
{
	uint8_t i=0 ,ret=1,node,dado=0;
	struct TPnrf24 *radio;
	radio=&gParam.radio;
	PGM_PRINTLN(">PMB")
	for (node=0; node<MAX_TSENSOR_CLIENT;node++) {
		debugvarln(node);
		if (bitRead(radio->client,node) && radio->addr[node]==0xFF) {
			do {
				debugvar(i);				
				debugvarln(dado);
				ret=parse_one_modbus_addr(radio->recursoClient[node], &i, (uint16_t *)&radio->dataClient[node][dado], &dado);
				debugvarln(ret);
			} while (i<MAX_MODBUS_ADDR && ret&0x01);
			bitWrite(radio->hasData,node,ret);
		}
	}

	PGM_PRINT("<PMB:")
	debugbinln(radio->hasData);
	return ret;
}


///////////////////////////////////////////////////////////////
void mb_create(uint8_t slave, uint16_t idx)
{
	byte ret, node;
	ret=0xff;
	PGM_PRINTLN ("<mbc");
	for (node=0;node<MAX_TSENSOR_CLIENT;node++) {
		if (!bitRead(gParam.radio.client,node) && ret==0xff) {
			ret=node;
		} else
		if (gParam.radio.serieClient[node]==slave) {
			break;
		}
	}
	if (node==MAX_TSENSOR_CLIENT && ret!=0xff)
	node=ret;
	if (node < MAX_TSENSOR_CLIENT) {
		debugvar(slave);
		debugvarln(idx);

		gParam.radio.addr[node]=0xFF;
		gParam.radio.serieClient[node]=slave;
		gParam.radio.recursoClient[node]=idx;
		gParam.radio.timeClient[node]=millis() / 1000;
		bitSet(gParam.radio.client,node);
		#if 0
		pisca(LED,2,250);
		pisca(LED_VERMELHO,2,250);
		#endif
	}
	PGM_PRINTLN ("<mbc");

}
////////////////////////////////////////////////////////
void process_TU(char* p)
{
	uint8_t tu_slave,tu_tipo, tu_comando;
	uint16_t ll,hh;
	uint8_t slave=0, achou=0xff, reg;
	struct TPModbus *mb;
	//debugvarln(p);
	tu_slave=(htoi(*p++)<<4) + htoi(*p++);
	tu_tipo=(htoi(*p++)<<4) + htoi(*p++);
	tu_comando=(htoi(*p++)<<4) + htoi(*p++);

	for (slave=0; slave < MAX_MODBUS_SLAVE; slave++) {
		mb=&gParam.mb[slave];
		if (mb->slave == tu_slave) {
			achou=slave;
			break;
		}
		if (achou==0xff && !mb->slave)
			achou=slave;
	}
	if (achou==0xff)
	return;
	debugvar(achou);
	mb=&gParam.mb[achou];
	mb->slave=tu_slave;
	mb->tipo=tu_tipo;
	mb->comando=tu_comando;

	debugvar(tu_slave);
	debugvar(tu_tipo);
	for (reg=0; reg < MAX_MODBUS_ADDR && isxdigit(*p); reg++) {
		hh=(htoi(*p++)<<4) + htoi(*p++);
		ll=(htoi(*p++)<<4) + htoi(*p++);
		mb->addr[reg]=(hh << 8) + ll;
		debug(mb->addr[reg]);
	}
	if (reg<MAX_MODBUS_ADDR)
		mb->addr[reg]=0xFFFF;
	mb_create(tu_slave,achou);
	bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
}
#endif