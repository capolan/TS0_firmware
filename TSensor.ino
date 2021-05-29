/**
* @file TSensor.ino
* @author CAP
* @date 1 Dez 2015
*
*  DELAY ASM
*  http://stackoverflow.com/questions/23477849/create-delay-in-arduino-uno-using-assembly-language-without-using-timer
*/
#include "TSensor.h"

#ifdef __AVR__
#include "avr/eeprom.h"
#include "eeprom.h"
#include "MemoryFree.h"
#endif
#include <Build_Defs.h>

#if HAS_OLED == 1
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//#include "Oled_display/Oled_display.h"
#define OLED_RESET 4
Oled_display oled(OLED_RESET);
#endif

#if HAS_NRF24L01 == 1
//#include <SPI.h>
//#include <Mirf.h>
//#include <nRF24L01.h>
//#include <MirfHardwareSpiDriver.h>
#endif

#if HAS_I2C == 1
//#include <Wire.h>
#endif

#if HAS_ETH == 1
//#include <SPI.h>
//#include <UIPEthernet.h>
#endif

#if HAS_GSM == 1
//#include "SIM800.h"
#endif

#if HAS_GSM_TCP == 1
//#include "GPRSInterface-master/GPRSInterface.h"
//#include "GPRSInterface-master/GPRS.h"
#endif


#if HAS_DHT == 1
#include <dht.h>
#endif

#if HAS_CLOCK == 1
//#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time_c.h>         //http://www.arduino.cc/playground/Code/Time
//#include <Wire.h>
#endif

#if USE_REAL_RTC == 1
//#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
//#include <Wire.h>
//#include <DS1307RTC/DS1307RTC.h>
#endif

#if USE_AT24 == 1
//#include <Wire.h>//
//#include <AT24Cx-master/AT24CX.h>
#endif

#if HAS_CORRENTE == 1 && HAS_EMONLIB == 1
#include "EmonLib.h"
#endif

#if HAS_DS18B20 == 1 || HAS_DS18B20_EXTRA == 1
#include <OneWire.h>
//#include <DallasTemperature.h>
#endif

#if HAS_SENSOR_ANALOGICO == 1
uint16_t sensor_analogico[MAX_SENSOR_ANALOGICO];
#endif

#if HAS_SENSOR_PORTA_9 == 1 || HAS_SENSOR_PORTA_10 == 1 || HAS_RELE == 1
volatile uint8_t sensor_9_10=0;
#endif

#if HAS_DISPLAY == 1
//#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
#endif


#ifdef __AVR_ATmega328P__
#if DEBUG > 0 || HAS_BLUETOOTH >= 1 || HAS_BLE == 1 
#include "SoftwareSerial.h"
SoftwareSerial SSdebug(RXWIFI,TXWIFI);  // RX TX  TS-0
#endif

#if USE_WIFI_HARDWARE == 0
SoftwareSerial SSwifi(ESP8266_RX,ESP8266_TX);  // RX TX
#endif

#elif defined(__AVR_ATmega1284__)
#define SSdebug Serial1
#else // atmega2560
#endif
char buf[15]; ///< buffer serial de entrada

#if HAS_BLE == 1
#define BLEserial SSdebug
#endif

#if HAS_RFID == 1
#include "SoftwareSerial.h"
SoftwareSerial SerialRFID(RXRFID,TXRFID);  // RX TX  TS-0

uint8_t idxf;
char rfidf[20];
static void parse_rfid(void);
#endif

#if HAS_MODBUS == 1
//#include <ModbusRtu_SoftwareSerial.h>
#endif


uint8_t ibuf=0;   ///< indice de char *buf[] para entrada serial, debug
uint16_t ccStatus=0; ///< bit de status do funcionamento do sistema, ver defines CC_*
uint8_t countCorrente=0;

#if USE_TIMER1 == 1
volatile uint16_t timer1; ///< contador de interrupções do timers
#endif
#if USE_TIMER2 == 1 || USE_WATCHDOG_TIMER == 1
volatile uint16_t timer2; ///< contador para ligar/apagar leds na rotina de interrupção watchdog
#endif
uint8_t timerWeb;
#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS==1
uint8_t contadorLog=1;
#endif

//#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
struct TDataLogger gDataLogger; ///< estrutura para armazenar os sensores em memoria nao volatil
//#endif

/// Estrutura geram de armazenamento/controle, é gravada/lida da EEPROM
struct TParametros gParam = {
	0
	#if HAS_ETH == 1 && USE_DHCP == 1
	| bit(P_STATUS_USE_DHCP)
	#endif
	,
	1,   // byte  3 da EEPROM
	{ MD5[0], MD5[1], MD5[2], MD5[3] },
	#if HAS_SENSOR_ANALOGICO == 1
	{ SENSOR_ANALOGICO_PIN ,  SENSOR_ANALOGICO_PIN_2},   // porta sensor analogico  = 0 sem sensor
	#endif
	{
		#ifdef B_TEMPERATURA
		B_TEMPERATURA,
		#else
		B_FASE,    // bit(B_FORCE_FASE)|B_FASE todas 3 fases
		#endif
		VCC_SUPPLY,
		TEMPO_COLETA_CORRENTE,
		CONTADOR_WEB_CORRENTE,
		CONTADOR_LOG,
		(float)CALIBRAGEM_CORRENTE, // 60.6,  // 100A
		NRO_AMOSTRAS_CORRENTE
	},
	{
		"",
		"",
		#if HAS_ETH == 1
//		{0x00,0x01,0x02,((TSENSOR_SERIE>>8)&0xff)+1,(TSENSOR_SERIE&0xff),0x01},
		{0x18,0xfe,0x30,((TSENSOR_SERIE>>8)&0xff)+1,(TSENSOR_SERIE&0xff),0x01},
		#else
		"", // MAC
		#endif
		PROXY_HTTP,
		PROXY_PORT,
		#if HAS_ESP8266 == 1
		SSID,
		PASSWORD,
		// AP
		SSID_AP, // ssid_ap
		PASSWD_AP, // passwd_ap
		CANAL_AP, // canal    -- se canal == 255, sem configuracao
		CRIPTO_AP, // cripto
		"",
		#endif
		#if HAS_ETH == 1
		{192,168,1,100},
		{192,168,1,1},
		{255,255,255,0},
		{8,8,8,8},
		#endif

		#if HAS_THINGSPEAK == 1 || HAS_TS_SERVER == 1
		#if HAS_TS_SERVER == 1
		TSSERVER_ADDR_IP,
		SERVER_PORT,
		#else
		THINGSPEAK_ADDR_IP,
		SERVER_PORT,
		#endif
		"",
		1,   // field 1
		#endif
	}
	#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
	,{
		SERV_ADDR_1, //serv_addr
		{0}, //addr
		0,// cliente
		60, // inatividade padrão 5min
		0,
		0,   // hasCMD
		0,   // hasCMDOK
		0,   // hasCMDERR
		0,   // erro comunicao i2c
		{0}, // sensor9 e sensor10
		{0}, // seqClient
		{0},
		{0}, // recurso
		{0}, // vccCliente
		#if 1
		{{0,0,0,0,0}}, // dataCliente
		#if HAS_CLOCK == 1
		{0,0,0,0} // timeCliente
		#endif
		#endif
	}
	#endif
	#if HAS_IR_REMOTE == 1
	,{
		38, // uint8_t khz;   // 38 Mhz
		32, // 6 * 8  AIR HITACHI uint8_t nro_bits; // numero de bits a enviar
		2, // uint8_t repetition; // numero de repeticoes do comando
		4700, //uint8_t repetition_interval; // intervalo em us entre as repetições
		4600, 4300, //uint16_t start_pulse, start_pause; // inicio da comunicação
		700, 1500, 450, //uint16_t pulse_len, pause_high, pause_low; // high=1  low=1
		{{0b10110010,0b01001101,0b00111111,0b11000000,0b00100000,0b11011111}, // 20C ON  COOL fan HIGH AIR HITACHI
		{0b10110010,0b01001101,0b01111011,0b10000100,0b11100000,0b00011111}} // 20C OFF COOL fan HIGH AIR HITACHI
	}
	#endif
	#if HAS_MODBUS == 1
	,{ {0,0,0,{0,0,0,0,0}}
}
#endif
#if HAS_SENSOR_SECO == 1
,{
	0,  // valor
	0,   // trocou
	0, // trocou_web
	5000, // time out em millis
	#if USE_SENSOR_SECO_PORTA_BAIXA == 1
	{0x80,1,4,5,10}, // porta   0 = fim  0x80 = porta D0 (bit 7 ligado)
	#else
	{7,8,11,12,13}, // porta   0 = fim
	#endif
	{_BV(SECO_FAST_READ_BIT) | _BV(SECO_IMEDIATO_BIT) | _BV(SECO_INPUT_PULLUP_BIT),
		_BV(SECO_FAST_READ_BIT) | _BV(SECO_IMEDIATO_BIT) |_BV(SECO_INPUT_PULLUP_BIT),
		_BV(SECO_FAST_READ_BIT) |_BV(SECO_IMEDIATO_BIT) | _BV(SECO_INPUT_PULLUP_BIT),
		_BV(SECO_FAST_READ_BIT) |_BV(SECO_IMEDIATO_BIT) | _BV(SECO_INPUT_PULLUP_BIT),
		_BV(SECO_FAST_READ_BIT) |_BV(SECO_IMEDIATO_BIT) | _BV(SECO_INPUT_PULLUP_BIT)
	},
	{
	},
	{
	}
	//_BV(SECO_IMEDIATO_BIT) | _BV(SECO_INPUT_PULLUP_BIT)} //| _BV(SECO_IMEDIATO_BIT)
}
#endif
};

const char str_TSensor_online[] PROGMEM = "&T=";

const char str_field_ts[] PROGMEM = "&field";

//char str_Config_header[] PROGMEM = 	"POST http://" TSSERVER_ADDR_IP "/obj/ti/0/config_ler.php HTTP/1.0\n";

const char str_TS_header[] PROGMEM = "POST http://%s/uts0 HTTP/1.0\r\n";

//const char str_TS_Host[] PROGMEM = "Host: " TSSERVER_ADDR_IP "\r\n";

const char str_TS_Host[] PROGMEM = "Host: ";

const char str_TS_Connection[] PROGMEM = "Connection: keep-alive\r\n";
//const char str_User_Agent_header[] PROGMEM = "User-Agent: Tsensor\n";
const char str_TS_Content_type[] PROGMEM = "Content-Type: application/x-www-form-urlencoded\r\n";
const char str_TS_Content_type_JSON[] PROGMEM = "Content-Type: application/json\r\n";
const char str_Content_type_text[] PROGMEM = "text/html\r\n";

const char str_HTTP_200[] PROGMEM = "HTTP/1.0 200 OK\r\n";
const char str_Content_type[] PROGMEM = "Content-type: ";
const char str_Content_length[] PROGMEM = "Content-Length: ";


#if HAS_ESP8266 == 1 || HAS_ETH == 1
const char str_TS_api_key[] PROGMEM = "\n\napi_key=";
#else
const char str_TS_api_key[] PROGMEM = "api_key=";
#endif

#define GETTS_CONFIG_BOOT '0' ///< busca configuração no setup
#define GETTS_CONFIG_LOOP '1' ///< busca configuração no loop, após inicializacao
#define GETTS_CONFIG_WIFI '2' ///< wifi ?

void SSerialEvent();
void(* resetFunc) (void) = 0; //declare reset function @ address 0
int getTSConfig(uint8_t);
static uint8_t executa_comando(uint8_t);

volatile uint8_t led1_status=0;
volatile uint8_t led2_status=0;

static void process_TS(char *);
static void process_TN(char *);

static uint8_t updated_flag='0';
static uint32_t mark_reboot; // controle reboot automatico

#if HAS_IR_REMOTE == 1
uint8_t* hex_decode(const char *in, size_t len,uint8_t *out)
{
	unsigned int i, t, hn, ln;

	for (t = 0,i = 0; i < len; i+=2,++t) {

		hn = in[i] > '9' ? in[i] - 'A' + 10 : in[i] - '0';
		ln = in[i+1] > '9' ? in[i+1] - 'A' + 10 : in[i+1] - '0';

		out[t] = (hn << 4 ) | ln;
	}

	return out;
}
#endif

#if HAS_ETH == 1
#include "IPAddress.h"
extern void stoip(IPAddress&,char*);
#endif

long mark;
void testa_integridade(void) {
	int offset;
	if (strncmp(gParam.chave,MD5,4)) {
		//SSdebug.println(gParam.chave);
		PGM_PRINTLN("+INTEG+");
		bitSet(gParam.status,P_STATUS_RESET);
	}
	//offset=offsetof(struct TParametros, web.ssid)+BASE_ADDR;
	/*if (millis()-mark > 2000) {
//		SSdebug.println(offset);
		mark=millis();
		#if HAS_BLUETOOTH >=2 || DEBUG > 0
		SSdebug.write('!');
		SSdebug.println(freeRAM());
		#endif
	}*/
}

/****************************************************************************/
#if USE_LEDS == 1 || HAS_RELE == 1
void pisca(uint8_t led,uint8_t i,uint16_t freq) {
	for (;i;--i) {
		digitalWrite(led,HIGH);
		delay(freq);
		digitalWrite(led,LOW);
		delay(freq);
	}
}
#endif

#if HAS_MODBUS == 1 || HAS_SENSOR_SECO == 1
////////////////////////////////////////////////////////////////
// sempre maiusculo
int htoi (char c) {  //does not check that input is valid
	if (c<='9')
	return c-'0';
	else
	return c-'A'+10;
}
#endif
////////////////////////////////////////////////////
// SETUP
///////////////////////////////////////////////////
void setup(void)
{
	uint8_t oled_etapa=1;
	boolean button_reset=false;
	#if HAS_OLED == 1
	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
	// init done
	
	// Show image buffer on the display hardware.
	// Since the buffer is intialized with an Adafruit splashscreen
	// internally, this will display the splashscreen.
	oled.splashScreen(2000);
	oled.clearDisplay();
	oled.etapasTextBar(oled_etapa++,5,PSTR("Ver." VERSION));
	#endif
	#if DEBUG > 0 || HAS_BLUETOOTH >= 1
	SSdebug.begin(9600);
	//SSdebug.listen();
	delay(500);
	SSdebug.println(">>");
	#elif HAS_BLE == 1
	BLEserial.begin(9600);
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("BLE"));
	#endif
	#endif
	
	#if HAS_RFID == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("RFID"));
	#endif
	SerialRFID.begin(9600);
	SerialRFID.listen();
	#endif
	//SSdebug.println(MAX_SENSORES_DS18B20);

	#if HAS_DISPLAY == 1
	PGM_PRINTLN("TSensor  LCD");
	setup_display();
	lcd.print("Tsensor");
	#endif
	
	#if HAS_ESP9266_RESET ==1
	pinMode(ESP8266_RST_PIN,OUTPUT);
	//digitalWrite(ESP8266_RST_PIN,HIGH);
	#ifdef __AVR__
	PORTD |= bit(PORTD4);
	#endif
	#endif

	#if USE_LEDS == 1
	pinMode(LED,OUTPUT);
	pinMode(LED_VERMELHO,OUTPUT);
	#if (TSENSOR_RECURSO & bit(REC_LED2)) > 0
	pinMode(LED_AMARELO,OUTPUT);
	#endif
	#endif

	#if HAS_SENSOR_PORTA_9 == 1
	pinMode(SENSOR_9_PIN,INPUT_PULLUP);
	#endif
	#if HAS_SENSOR_PORTA_10 == 1
	pinMode(SENSOR_10_PIN,INPUT_PULLUP);
	#endif

	#if HAS_SENSOR_SECO == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("Seco"));
	#endif
	setup_seco(5);
	parse_seco(false);
	gParam.sensor_seco.trocou=0;
	gParam.sensor_seco.trocou_web=0;
	#endif

	#if HAS_TIMER_INT == 1
	#if DEBUG > 10
	PGM_PRINTLN("setup_timer");
	#endif
	setup_timer();
	#endif
	
	#if HAS_BOTAO == 1
	setup_botao();
	#endif
	
	#if HAS_CLOCK == 1
	setup_clock();
	#endif
	
	#if USE_AT24 == 1
	setup_at24();
	#endif


	#if HAS_EEPROM == 1
	button_reset=setup_eeprom();
	// marca que está fazendo o boot
	//eeprom_write_byte(0,0xAA);
	#else
	//1bitSet(gParam.status,P_STATUS_RESET_FABRICA);
	#endif

	#if HAS_ESP8266 == 1
	#if HAS_BOTAO == 0
	button_reset=true;
	#endif
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("Wifi"));
	#endif
	setup_ESP8266(button_reset);
	#endif

	#if HAS_ETH == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("Ethernet"));
	#endif
	setup_eth();
	#endif
	#if HAS_GSM == 1 || HAS_GSM_TCP == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("GSM"));
	#endif
	setup_gsm(10);
	debugbin(ccStatus);
	#if 0
	PGM_PRINTLN("opss");
	for (;;) {
		delay(1000);
		digitalWrite(LED,LOW);
		delay(1000);
		digitalWrite(LED,HIGH);
		SSdebug.write('.');
	}
	#endif
	#endif
	#if HAS_RELE == 1
	pinMode(RELE_PIN,OUTPUT);
	#endif
	
	#if 1
	//1bitClear(gParam.status,P_STATUS_RESET_FABRICA);
	bitClear(gParam.status,P_STATUS_RESET);
	bitClear(gParam.status,P_STATUS_GRAVA_EEPROM);
	bitClear(gParam.status,P_STATUS_RESET_ESP8266);
	#else
	gParam.status &= ~P_STATUS_BITS_LIMPA;
	#endif

	#if HAS_MODBUS == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("Modbus"));
	#endif
	setup_modbus();
	#endif
	
	#if HAS_CORRENTE == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("Corrente"));
	#endif
	setup_corrente();
	for (uint8_t i=4;i;i--)
	parse_corrente();
	#if USE_LOG_EEPROM == 1
	memset((uint8_t*)&gDataLogger.fase1,0,6);
	#endif
	#endif

	led1_status=0;
	#if HAS_TEMPERATURA == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("Temperat"));
	#endif
	setup_Temperature();
	debugbinln(ccStatus);
	readTemperature();
	#endif
	
	#if HAS_NRF24L01 == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("Radio"));
	#endif
	setup_nrf24();
	#if 0
	PGM_PRINT("N:");
	debugln(gParam.radio.client);
	for (uint8_t node=0;node<MAX_TSENSOR_CLIENT;node++) {
		if (bitRead(gParam.radio.client,node)) {
			PGM_PRINT("node"); debug(node); PGM_PRINT("=");
			debugln(gParam.radio.serieClient[node]);
		}
	}
	#endif
	#endif

	#if HAS_I2C == 1
	#if HAS_OLED == 1
	oled.etapasTextBar(oled_etapa++,5,PSTR("i2c"));
	#endif
	setup_i2c();
	#if HAS_I2C_COPY == 1
	uint8_t addr[8] = {3,0,0,0,0,0,0,0};
	Temp_create(addr, NULL);
	#endif
	#endif
	PGM_PRINT("\nccStatus=");
	debugbinln(ccStatus);
    debugln(gParam.web.http_proxy);
	#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
	if ((ccStatus & CC_WIFI_BITS_CWJAP_OK) == CC_WIFI_BITS_CWJAP_OK) {
		PGM_PRINTLN("config");
		getTSConfig(GETTS_CONFIG_BOOT+bitRead(ccStatus,CC_BOOT_RESET));  // se pegar a configuração, acerta o flag do WIFI_INTERNET
	}
	#else
	led1_status=led2_status=LED_APAGA;
	#endif
	
	//debugbinln(ADC_COUNTS);
	#if USE_TIMER1 == 1
	timer1=millis();
	#endif
	
	#if HAS_DISPLAY == 1
	lcd.clear();
	#endif
	
	#if HAS_EVENTOS == 1
	limpa_eventos(0);
	#endif
	#if HAS_EEPROM == 1
	// marca que está fazendo o boot
	//	EEPROM.write(0,0xCC);
	eeprom_write_byte(0,0xCC);
	#endif

	bitSet(ccStatus,CC_BOOT);
	bitSet(ccStatus,CC_CORRENTE);  // força leitura de todos sensores
	#if 0 && HAS_ETH ==1
	if (!bitRead(ccStatus,CC_HAS_DHCP))
	timerWeb = gParam.rede.tempoWebCorrente; // força envio no primeiro loop
	#endif
	PGM_PRINTLN("loop");
	//SSdebug.println(gParam.chave);
	//timer0_millis=0xFFFFFFFF - (120*1000);
	//timer0_millis=4294907295;
	#if HAS_OLED == 1
	delay(1000);
	oled.clearDisplay();
	oled.mostraIcon();
	oled.parse(true);
	#endif
	
}

//volatile uint16_t index=0;
//volatile uint16_t index_err=0;

//////////////////////////////////////////////////
void loop(void)
{
	uint8_t ret;
	uint8_t botao,l2;
	static uint8_t t_reboot=MAX_T_REBOOT;

	#if HAS_OLED == 1
	oled.parse(false);
	if (!bitRead(ccStatus,CC_INIT_ETH)) {     // bit 0 indica link
		oled.mostraHighBar("Sem link");
	} else
	if (!bitRead(ccStatus,CC_CWJAP)) {     // bit 0 indica link
		oled.mostraHighBar("Wifi sem rede");
	} else {
	switch (connectCodeWifi)
	{
		case 0: // OK
		oled.mostraHighBar("Wifi OK");
		break;
		case 1: // timeout
		oled.mostraHighBar("Wifi Timeout");
		break;
		case 2: // senha
		oled.mostraHighBar("Senha errada");
		break;
		case 3: // ap
		oled.mostraHighBar("SSID errada");
		break;
		case 4: // OK
		oled.mostraHighBar("Wifi falha");
		break;
		case 5: // nowifi
		oled.mostraHighBar("Wifi bug");
		break;
	}
	}
	#endif
	#if HAS_ETH == 1
	if (!bitRead(ccStatus,CC_INIT_ETH))
	setup_eth();
	else
	parse_eth();
	#endif
	#if HAS_NRF24L01 == 1
	if (ret=parse_nrf24()) {// nova configuracao/cliente
		if (ret&1) {
			LEDA_ON;
			updateDadosFeed(TUS_BUSTA_CONFIG,0);
			LEDA_OFF;
		}
		// testa se tem dados na RX FIFO
		if (ret&2) {
			return;
		}
		
	}
	//return;
	#endif
	#if HAS_BOTAO == 1
	botao=parse_botao();

	#if TESTE_RELE == 1
	return;
	#endif
	
	if (bitRead(botao,BTN_PRESS_CURTO))	{
		//	eATCWLAP();
		#if HAS_I2C == 1
		setup_i2c();
		#endif
		#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
		if (bitRead(ccStatus,CC_CWJAP)) {
			timerWeb=0;
			}	else {
			led1_status=LED_LIGAR;
			#if HAS_ESP9266_RESET == 1
			ESP8266reset(2);
			#endif
			#if HAS_GSM == 1
			setup_gsm(2);
			#endif
			led1_status=LED_PISCA_FLASH;
		}
		#endif
	}
	#if 0
	if (bitRead(botao,BTN_PRESS_LONGO)) {
		uint8_t l1=led1_status;
		l2=led2_status;
		led1_status=led2_status=LED_PISCA_FLASH;
		delay(2000);
		strcpy(gParam.web.ssid_ap,SSID_AP);
		gParam.web.passwd_ap[0]='\0';
		gParam.web.canal_ap=CANAL_AP;
		gParam.web.cripto_ap=CRIPTO_AP;
		eATCWSAP();
		delay(1000);
		bitSet(gParam.status,P_STATUS_RESET_FIRMWARE);
		led1_status=l1;
		led2_status=l2;
	}
	#endif
	#endif // BOTAO
	
	// leituras imediatas
	#if HAS_SENSOR_SECO == 1
	if (parse_seco(true)) { // ler somente os fast
		timerWeb=0;
		PGM_PRINT("trocou=");
		debugbinln(gParam.sensor_seco.trocou);
	}
	#endif
	#if HAS_EVENTOS == 1
	parse_eventos();
	#endif

	#if DEBUG > 0 && USE_TIMER1 == 1
	if (bitRead(ccStatus,CC_TICK)) {
		bitClear(ccStatus,CC_TICK);
	}
	#endif
	
	// testa tempo para coletar amostra da corrente consumida
	#if USE_TIMER1 == 1
	if (!(timer1 % gParam.rede.tempoColetaCorrente))
	#else
	if (!(millis()/1000 % gParam.rede.tempoColetaCorrente) && timerWeb)
	#endif
	{
		if (!bitRead(ccStatus,CC_TICK_2)) {  // coletar corrente
			bitSet(ccStatus,CC_TICK_2);
			bitSet(ccStatus,CC_CORRENTE);
			#if HAS_MONITORAMENTO == 1
			--timerWeb;
			#endif

			#if HAS_SENSOR_PORTA_9 == 1 || HAS_SENSOR_PORTA_10 == 1
			int pin=SENSOR_10_READ;
			debugvar(pin);
			debugvarln(sensor_9_10);
			#endif

		}
	} else
	bitClear(ccStatus,CC_TICK_2);
	
	// coleta amostra de corrente
	if (bitRead(ccStatus,CC_CORRENTE)) {
		l2=led2_status;
		PGM_PRINT("millis=");
		debugln(millis());
		//SSdebug.println(timer0_millis);
		//int temperaturaAmbiente;
		led2_status=LED_LIGAR;
		#if HAS_I2C == 1
		if (parse_i2c()) {
			PGM_PRINTLN("NOVO I2C");
			updateDadosFeed(TUS_BUSTA_CONFIG,0);
		}
		#endif
		#if HAS_MODBUS == 1
		parse_modbus();
		#endif

		bitClear(ccStatus,CC_CORRENTE);
		countCorrente++;
		#if HAS_TEMPERATURA == 1
		readTemperature();
		
		#if HAS_DISPLAY == 1
		lcd.home();
		lcd.print(temperaturaAmbiente/countCorrente);
		
		lcd.setCursor(11,0);
		uint8_t h=hour();
		if (h<10) lcd.write(' ');
		lcd.print(h);
		lcd.write(':');
		h=minute();
		if (h<10) lcd.write(' ');
		lcd.print(h);
		#endif
		#endif

		#if HAS_HUMIDITY == 1
		humidade=readHumidity();
		#if DEBUG>0 || HAS_BLUETOOTH > 0
		SSdebug.print(" H=");
		SSdebug.print(humidade);
		SSdebug.print(" =");
		SSdebug.println(mark_reboot);
		#endif
		#endif

		#if DEBUG > 0
		SSdebug.write('\n');
		#endif
		
		#if HAS_NRF24L01==1 && DEBUG > 10
		uint8_t st;
		Mirf.readRegister( CONFIG, &st, sizeof(st) );
		PGM_PRINT ("cf="); debugbin(st);
		Mirf.readRegister( STATUS, &st, sizeof(st) );
		PGM_PRINT (" st="); debugbin(st);
		Mirf.readRegister( FIFO_STATUS, &st, sizeof(st) );
		PGM_PRINT (" fifo_st="); debugbin(st);
		Mirf.readRegister( EN_AA, &st, sizeof(st) );
		PGM_PRINT (" en_aa="); debugbin(st);
		Mirf.readRegister( EN_RXADDR, &st, sizeof(st) );
		PGM_PRINT (" en_rx="); debugbin(st);
		Mirf.readRegister( OBSERVE_TX, &st, sizeof(st) );
		PGM_PRINT (" obs_tx="); debugbinln(st);
		char Raddr[6]="EILCT";
		Mirf.getADDR(RX_ADDR_P0,(uint8_t *)Raddr);
		debugvarln(Raddr);
		Mirf.getADDR(RX_ADDR_P1,(uint8_t *)Raddr);
		debugvarln(Raddr);
		Mirf.getADDR(RX_ADDR_P2,(uint8_t *)Raddr);
		debugvarln(Raddr);
		#endif
		
		#if HAS_SENSOR_ANALOGICO == 1
		for (uint8_t ps=MAX_SENSOR_ANALOGICO; ps;) {
			ps--;
			if (gParam.porta_sensor_analogico[ps]) {
				sensor_analogico[ps] = parse_seco_analogico(gParam.porta_sensor_analogico[ps], 20); // 20 milisegundo entre lidas
				#if HAS_BLUETOOTH >=1 || DEBUG > 0
				SSdebug.print("\nRD=");
				SSdebug.println(sensor_analogico[ps]);
				#endif
				#if HAS_OLED == 1
				oled.setField(ps+7,sensor_analogico[ps],'a');
				#endif
			}
		}
		#endif

		#if HAS_SENSOR_SECO == 1
		if (parse_seco(false)) {
			timerWeb=0;
		}  // ler todos sensores
		#endif
		//		#if DEBUG > 0
		//		gDataLogger.fase1 =correnteFase1;
		//		gDataLogger.fase2 =correnteFase2;
		//		gDataLogger.fase3 =correnteFase3;
		//		countCorrente=1;
		#if HAS_CORRENTE == 1
		parse_corrente();
		gDataLogger.fase1 +=correnteFase[0];
		gDataLogger.fase2 +=correnteFase[1];
		gDataLogger.fase3 +=correnteFase[2];
		#if DEBUG > 0 || HAS_BLUETOOTH >= 2
		SSdebug.print(correnteFase[0]);
		SSdebug.write(':');
		SSdebug.print(correnteFase[1]);
		SSdebug.write(':');
		SSdebug.println(correnteFase[2]);
		//SSdebug.write('  #');
		//SSdebug.println(countCorrente);
		//SSdebug.print(gDataLogger.fase1 / countCorrente);
		//SSdebug.write(':');
		//SSdebug.print(gDataLogger.fase2 / countCorrente);
		//SSdebug.write(':');
		//SSdebug.println(gDataLogger.fase3 / countCorrente);
		#endif
		#endif
		led2_status=l2;
	}
	// testa tempo para enviar a web a corrente gerada
	if (!timerWeb) {
		#if HAS_CLOCK == 1
		time_t thora=now();
		#else
		uint32_t thora=millis();
		#endif
		timerWeb = gParam.rede.tempoWebCorrente;
		if (!countCorrente) countCorrente++;
		#if HAS_CORRENTE == 1
		gDataLogger.fase1 /= countCorrente;
		gDataLogger.fase2 /= countCorrente;
		gDataLogger.fase3 /= countCorrente;
		#endif
		//grava_corrente_log();
		#if HAS_BLUETOOTH > 0
		SSdebug.println(ccStatus,BIN);
		#endif
		debugbinln(gParam.status);
		// regra para reboot
		#if HAS_ESP8266 == 1 || HAS_ETH == 1
		#if HAS_ESP8266 == 1
		if (bitRead(ccStatus,CC_HAS_SSID)) 
		{
		PGM_PRINTLN("-------------");
			if (gParam.web.rssi[0]=='\0')
				eATCWSAP_DEF(reqHTTP);
		#endif
		#if  HAS_ETH == 1
		if (bitRead(ccStatus,CC_HTTP_200_OK))
		{
		#endif
			if (bitRead(ccStatus,CC_WIFI_INTERNET)) {
				mark_reboot=0;
			} else {
				if (mark_reboot) {
					if (millis()-mark_reboot > 150000) {  // 900000 reboot em 15min sem comunicacao
						bitSet(gParam.status,P_STATUS_RESET);
						#if DEBUG > 0 || HAS_BLUETOOTH >=3
						SSdebug.println("&&&");
						#endif
					}
				} else
					mark_reboot=millis();				
			}
		}
		#endif


		#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
		//		if (bitRead(ccStatus,CC_CWJAP) && !bitRead(ccStatus,CC_WIFI_INTERNET))
		if (bitRead(ccStatus,CC_CWJAP) && !bitRead(ccStatus,CC_HTTP_200_OK))
		{
			PGM_PRINTLN("===========");
			getTSConfig(GETTS_CONFIG_LOOP);  // se pegar a configuração, acerta o flag do WIFI_INTERNET
		}
		#endif

		#if HAS_GSM == 1
		if (bitRead(ccStatus,CC_CWJAP)) {
			t_reboot=MAX_T_REBOOT;
			led2_status=LED_APAGA;
		} else
		if (!--t_reboot) {
			led2_status=LED_PISCA_CURTO;
			bitSet(gParam.status,P_STATUS_RESET);
			t_reboot=MAX_T_REBOOT;
			PGM_PRINTLN("T_REB");
			//	bitSet(gParam.status,P_STATUS_RESET);
		}
		#endif
	//WWWW
		
		// testa envio thingspeak
		#if HAS_THINGSPEAK==1 && (HAS_ETH==1 || HAS_ESP8266==1 || HAS_GSM == 1)
		//#if HAS_ETH == 1
		//if (bitRead(ccStatus,CC_LINK_ETH))
		//#endif
		ret=0; // indica transmissão sem sucesso
		//if (bitRead(ccStatus,CC_WIFI_INTERNET) //&&		bitRead(gParam.status,P_STATUS_SEND_THINGSPEAK)
		if (bitRead(ccStatus,CC_HTTP_200_OK))
		{
			debugvarln(gParam.status);
			ret=updateDadosFeed(TUS_ENVIA_DADOS,thora);
			//bitSet(ccStatus,CC_TEST_WIFI);
			debugvarln(ret);
			#if USE_LOG_EEPROM == 1
			if (ret && bitRead(flagHTTP,F_LOG_SYNC)) {
				ret=updateDadosFeed(TUS_ENVIA_LOG,0);
				bitClear(flagHTTP,F_LOG_SYNC);
			}
			#endif
			#if HAS_EVENTOS == 1
			if (ret && bitRead(flagHTTP,F_LOG_SYNC_EVENTOS)) {
				ret=updateDadosFeed(TUS_ENVIA_LOG_AT24,0);
				bitClear(flagHTTP,F_LOG_SYNC_EVENTOS);
			}
			#endif
		}
		#endif
		#if USE_LOCATION == 1 && HAS_CLOCK == 1
		if (bitRead(ccStatus,CC_CWJAP)) {
			if (!loc_count-- && gprs.getLocation(&loc)) {
				process_date(NULL);
				loc_count=MAX_LOC_COUNT;
			}
			if (loc.lat) {
				t_reboot=MAX_T_REBOOT;
				led2_status=LED_APAGA;
			} else
			if (!--t_reboot) {
				led2_status=LED_PISCA_CURTO;
				setup_gsm(3);
				t_reboot=MAX_T_REBOOT;
				//	bitSet(gParam.status,P_STATUS_RESET);
			}
		}
		#endif

		#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
		debugvarln(contadorLog);
		if (!--contadorLog) {
			#if USE_LOG_EEPROM == 1
			ret=grava_corrente_log(thora);
			#else
			ret=grava_eventos_log(EVENTO_NULL,0,FUNC_NULL,0L);
			#endif
			contadorLog=gParam.rede.tempoLOG;
			//			ret=1; // força incremento do contado gParam.id
		}
		if (ret)
		#endif
		{

			// incrementa contador fora
			gParam.id++;
			#if HAS_SENSOR_SECO == 1
			gParam.sensor_seco.trocou_web=0;
			#endif
			#if HAS_EVENTOS == 1
			//limpa_eventos(~(bit(EVENTO_ACIONADO)));
			eventos_feed=0;
			#endif
		}
		countCorrente=1;
		//memset(&gDataLogger.fase1,0,6);
		#if HAS_ESP8266 == 1
		#if DEBUG > 0 || HAS_BLUETOOTH > 0
		eATCWMODE(2);  // CWLIF lista os clientes conectados no AP
		#endif
		#endif
	}	
	if (!bitRead(ccStatus,CC_CWJAP))
		led1_status=LED_PISCA_FLASH;
	#if HAS_ESP8266 == 1 && HAS_WEBSERVER_FULL == 1
	parse_ESP8266();
	#endif
	#if HAS_GSM == 1
	parse_gsm();
	#endif
	// testa memoria

	#if HAS_EEPROM == 1
	if (bitRead(gParam.status,P_STATUS_RESET_FIRMWARE)) {
		//EEPROM.write(0,0xFF);
		PGM_PRINTLN("rstfw");
		eeprom_write_byte(0,0xFF);
		delay(50);
		resetFunc();
	}
	#endif

	if (bitRead(gParam.status,P_STATUS_RESET)) {
		PGM_PRINTLN("rst");
		delay(1000);
		eeprom_write_byte(0,0xBB);
		delay(10);
		#if HAS_ESP8266 == 1
		ESP8266reset(2);
		#endif
		resetFunc();
	}

	#if HAS_EEPROM == 1
	if (bitRead(gParam.status,P_STATUS_GRAVA_EEPROM)) {
		bitClear(gParam.status,P_STATUS_GRAVA_EEPROM);
		write_gParam();
		#if HAS_I2C == 1
		reenvia_i2c_config();
		#endif
	}
	#endif

	#if HAS_ESP8266 == 1
	if (bitRead(gParam.status,P_STATUS_SETUP_AP)) {
		bitClear(gParam.status,P_STATUS_SETUP_AP);
		eATCWSAP();
	}

	if (bitRead(gParam.status,P_STATUS_RESET_ESP8266)) {
		bitClear(gParam.status,P_STATUS_RESET_ESP8266);
		ESP8266reset(true);
	}

	if (bitRead(flagHTTP,F_SETUP_WIFI)) {
		bitClear(flagHTTP,F_SETUP_WIFI);
		setup_ESP8266(2); // inicializa wifi
		timerWeb=0;
	}
	#endif
	#if DEBUG > 10 || HAS_BLUETOOTH >=5 || HAS_BLE == 1 
	loop_serial();
	#endif
	#if HAS_RFID == 1
	parse_rfid();
	if (bitRead(flagHTTP,F_RFID_RECEIVED)) {
		SSdebug.println(rfidf);
		bitClear(flagHTTP,F_RFID_RECEIVED);
	}
	#endif
	#if TINY_VERSION == 0
	testa_integridade();
	#endif
}
#if HAS_RFID == 1
void parse_rfid(void)
{
	char c;
	if (bitRead(flagHTTP,F_RFID_RECEIVED)) return;
	while (SerialRFID.available()) {
		c=SerialRFID.read();
		SSdebug.write(c);
		if (idxf>=20) idxf=0;
		if (c==STX) idxf=0;
		if (isalnum(c)) rfidf[idxf++]=0;
		if (c==ETX) {
			rfidf[idxf]='\0';
			bitSet(flagHTTP,F_RFID_RECEIVED);
			timerWeb=0; 
		}
	}
};
#endif
#if DEBUG > 10 || HAS_BLUETOOTH >=5 || HAS_BLE == 1
/**
* \brief rotina le entrada serial. bit F_SERIAL_RECEIVED == true quando tem uma linha no buffer \p buf.
*		se buf[0]='N' comunicacao de um BLE, caso contrário debug.
*/
void loop_serial(void)
{
	SSerialEvent();
	// envia consumo para internet
	if (bitRead(flagHTTP,F_SERIAL_RECEIVED)) {
		SSdebug.println(buf);
		#if HAS_BLE == 1
		if (buf[0] == 'N')
		parse_ble(buf);
		#endif
		#if DEBUG > 10 || HAS_BLUETOOTH >=5
		if (buf[0] != 'N')
		loop_debug();
		#endif

		bitClear(flagHTTP,F_SERIAL_RECEIVED);
	}
}

/**
* \brief rotina le entrada serial. bit F_SERIAL_RECEIVED == true quando tem uma linha no buffer \p buf.
*/
void SSerialEvent()
{
	if (bitRead(flagHTTP,F_SERIAL_RECEIVED)) return; //get out if previous message not yet proceSSdebugd
	while (SSdebug.available()) {
		char a = (char)SSdebug.read();
		SSdebug.write('_');
		SSdebug.write(a);
		buf[ibuf++] = a;
		if (a == '\n' || a=='\r' || a=='z') {
			bitSet(flagHTTP,F_SERIAL_RECEIVED);
			buf[ibuf-1] = '\0';
			ibuf=0;
		}
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
#if DEBUG > 10 || HAS_BLUETOOTH >=5
/**
* \brief loop para ler entrada serial do debug ou BLE
*/
void loop_debug(void)
{
	int var=atoi(buf+1);
	uint32_t t=atol(buf+1);
	switch (buf[0]) {
		case 'O':
		bitSet(gParam.status,P_STATUS_RESET);
		SSdebug.println("Reboot");
		delay(1000);
		break;
		case 'W':
		bitSet(gParam.status,P_STATUS_RESET_FIRMWARE);
		SSdebug.println("Reset");
		delay(1000);
		break;
		#if HAS_ESP8266 == 1
		case 'A':
		sendData(buf,1000,NULL);
		break;
		#endif
		// Configura rede WIFI
		case 'i':
		#if HAS_ESP8266 == 1
		if (buf[1] == 's') {
			strcpy(gParam.web.ssid, buf+2);
		}
		if (buf[1] == 'w') {
			strcpy(gParam.web.passwd, buf+2);
		}
		#endif
		if (buf[1] == 'y') {
			strcpy(gParam.web.http_proxy, buf+2);
		}
		if (buf[1] == 'p') {
			gParam.web.http_port = atoi(buf+2);
		}
		if (buf[1] == 'd') {
			var = atoi(buf+2);
			bitWrite(gParam.status,P_STATUS_USE_DHCP,var);
		}
		PGM_PRINT("Rede=");
		SSdebug.println(buf);
		bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
		break;
		// Lista sensores seco
		case 's':
		#if HAS_SENSOR_SECO == 1
		PGM_PRINT("seco=");
		debugbin(gParam.sensor_seco.valor);
		PGM_PRINT(" tr=");
		debugbinln(gParam.sensor_seco.trocou);
		for (uint8_t i=0 ; i< MAX_SENSOR_SECO; i++) {
			debug(i);
			PGM_PRINT(" pt=");
			debugln(gParam.sensor_seco.porta[i]);
		}

		if (var==1)
		gParam.sensor_seco.trocou_web=0;
		#endif
		#if HAS_SENSOR_ANALOGICO == 1
		debug(gParam.porta_sensor_analogico[0]);
		PGM_PRINT("sens=");
		debugln(sensor_analogico[0]);
		#endif
		break;
		case 't':
		timerWeb=0;
		break;
		#if HAS_ETH == 1
		case 'w':
		PGM_PRINTLN("rst eth");
		setup_eth();
		break;
		#endif
		#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
		case 'l':
		//			list_corrente_log(_BV(3), 0, reqHTTP);
		list_corrente_log(var, 0, reqHTTP);
		break;
		#endif
		#if HAS_CLOCK == 1
		case 'c':
		if (t>now()) {
			setTime(t);
			#if USE_REAL_RTC==1
			RTC.set(t);
			#endif
		}
		debugln(formatHora(reqHTTP,now()));
		break;
		#endif
		#if HAS_RELE == 1
		case 'r':
		//pisca(LED,5,1000);
		buf[0]=var;
		digitalWrite(RELE_PIN,buf[0]);
		digitalWrite(4,buf[0]);
		break;
		#endif
		#if HAS_GSM == 1
		case 'n':
		gprs.sendSms("5551991125090",buf+1);
		break;
		#endif
	}
}

#endif

#if HAS_ETH==1 || HAS_ESP8266 ==1 || HAS_GSM == 1

/**
* brief trata retorno do envio dos dados ao servidor, configuração, data, etc..
* \param data area de armazenamento
* \param connectioIn nro da conexao
*/
int endThingSpeak(char *data, uint8_t *connectionId) {
	uint16_t retHTTP, total;
	unsigned long t;
	led1_status=1;
	
	t=millis();
	#if HAS_GSM == 1 && USE_ETH_SMS == 0
	//gprs.buffer=data;
	retHTTP=sATCIPSENDACTION(0);
	total=sHTTPREAD();
	debugvarln(total);
	#else
	//	PGM_PRINTLN(">endThingSpeak");
	#if HAS_ESP8266 == 1
	recvPkg(0,(uint8_t*)data, 13, 8000, connectionId);
	#elif HAS_ETH == 1
	recvPkg(0,(uint8_t*)data, 13, 30000, connectionId);
	#endif
	// 012345678901
	// HTTP/1.1 200 OK
	retHTTP=atoi(data+9);
	#endif


	#if DEBUG > 0 || HAS_BLUETOOTH >= 2
	debugvarln(data);
	t=millis()-t;
	SSdebug.write("\nt=");
	SSdebug.println(t);
	#endif
	if (retHTTP == 200) {
		bitSet(ccStatus,CC_RET_HTTP);
		bitSet(ccStatus,CC_HTTP_200_OK);
		led2_status=LED_APAGA;
		while (SSwifi.available() > 0 && !bitRead(ccStatus,CC_MATA_BUFFER)) {
			//bitSet(ccStatus,CC_DEBUG);
			total=recvPkg(1,(uint8_t*)data, 79, 5000, connectionId);
			#if DEBUG > 0
			debugln(data);
			#endif
			// DATE
			#if HAS_CLOCK == 1
			if (!strncmp(data,"Date:",5)) {
				process_date(data);
			}
			#endif
			// TS - comando para o no central
			if (!strncmp(data,"TS:",3) || !strncmp(data,"TA:",3)) {
				process_TS(data+4);
			}
			// TU modbus
			#if HAS_MODBUS == 1
			if (!strncmp(data,"TU:",3)) {
				process_TU(data+4);
			}
			#endif
			// TI sensor_seco
			#if HAS_SENSOR_SECO == 1
			if (!strncmp(data,"TI:",3)) {
				process_TI(data+4);
			}
			#if HAS_EVENTOS == 1
			if (!strncmp(data,"TG:",3)) {
				process_TG(data+4);
			}
			#endif
			#endif
			#if TINY_VERSION == 0
			if (!strncmp(data,"TN:",3)) {
				process_TN(data+4);
			}
			#endif
		} // while
		} else {
		bitClear(ccStatus,CC_RET_HTTP);
		led2_status=LED_PISCA_LONGO;  //TAG_2
	}
	#if HAS_ESP8266 == 1
	esp8266Flush();
	#endif
	#if DEBUG >0 || HAS_BLUETOOTH > 0
	SSdebug.print("<eT ");
	SSdebug.println(retHTTP);
	#endif
	return retHTTP;
}

/**
* \brief enviar o header do retorno HTTP
*/
uint8_t send_html_header(uint8_t connectionId, char *data, uint8_t tipo) {

	sprintf_P (data,str_TS_header,gParam.web.server_addr);
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)data,true)) return 0;
	/*
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_TS_header_1,true)) return 0;
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)gParam.web.server_addr,true)) return 0;
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_TS_header_2,true)) return 0;
	*/
	//sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_User_Agent_header,true);
	//	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)data,true);

	//sprintf_P (data,str_TS_Host,gParam.web.server_addr);
	
	strcpy_P(data,str_TS_Host);
	strcat(data,gParam.web.server_addr);
	strcat(data,"\r\n");
	
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)data,true);
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_TS_Connection,true);
	if (tipo <=1)
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_TS_Content_type,true);
	#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
	else
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_TS_Content_type_JSON,true);
	#endif
	
}

/************************************************************************************/
#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
int processa_node(uint8_t flag, uint8_t tipo, char *pData) {
	char hello[]="0_n=";  // hello[2]=nro do node

	char *data=pData;
	char *p,*nro=reqHTTP+MAX_HTTPreq-8;
	uint16_t mark,tam=0;
	uint8_t flag_limpa;
	uint16_t recursoNode;
	PGM_PRINT(">procNode:");
	debugvar(flag);
	PGM_PRINT(" hD=");
	debugbinln(gParam.radio.hasData);
	#ifdef __AVR_ATmega2560__
	if (pData == NULL)
	data=(char*)malloc(100);
	*data='\0';
	#endif

	if (flag <5) {
		sATCIPSENDMultiple(flag,POINTER_CHAR_RAM, (uint8_t*)data,false);
		mark=0;
		} else {
		mark=strlen(data);
	}
	for (uint8_t node=0; node < MAX_TSENSOR_CLIENT; node++) {
		p=data+mark;
		flag_limpa=false;
		hello[0]='0';
		if (bitRead(gParam.radio.client,node)) { // if1
			#if HAS_MODBUS==1
			if (gParam.radio.addr[node]==0xFF) {
				recursoNode=0xFF;
			} else
			#endif
			recursoNode=gParam.radio.recursoClient[node];
			hello[0]++;
			hello[2]=node+'1';
			// nro de série
			PGM_PRINT("hasData=");
			debugln(gParam.radio.hasData);
			if (!tipo || bitRead(gParam.radio.hasData,node)) {
				if (!tipo) {
					strcat_P(data,PSTR("&N")); // indica para grava no INTRA_SENSOR node1,node2,node3
				}
				strcat_P(data,PSTR("&ns")); // serie
				strcat(data,hello+2);
				itoa(gParam.radio.serieClient[node],nro,10);
				strcat(data,nro);
				//debugvarln(gParam.radio.serieClient[node]);
				strcat_P(data,PSTR("&nn")); // recurso
				strcat(data,hello+2);
				itoa(recursoNode,nro,10);
				strcat(data,nro);
				//debugvarln(gParam.radio.recursoClient[node]);
			}
			if (bitRead(gParam.radio.hasData,node)) {
				char ret=0;
				int *dataClient=&gParam.radio.dataClient[node][0];
				#if HAS_MODBUS == 1
				for (uint8_t idxf=0; idxf<MAX_TSENSOR_SENSOR;idxf++) {
					strcat_P(data,str_field_ts);
					strcat(data,hello);
					itoa(*dataClient++,nro,10);
					strcat(data,nro);
					hello[0]++;
					//	debugvarln(tam);
					tam +=strlen(data)-mark;
					if (flag <5) {
						sATCIPSENDMultiple(flag,POINTER_CHAR_RAM, (uint8_t*)data,false);
						*data=mark='\0';
					}
					*p='\0';
				}
				#else
				strcat_P(data,str_field_ts);
				strcat(data,hello);
				itoa(*dataClient,nro,10);
				strcat(data,nro);
				if (recursoNode & (REC_DS18B20_EXTRA | bit(REC_HUMIDADE))) {
					hello[0]++;
					dataClient++;
					strcat_P(data,str_field_ts);
					strcat(data,hello);
					itoa(*dataClient,nro,10);
					strcat(data,nro);
					} else {
					hello[0]++;
					dataClient++;
				}
				// testa se node tem corrente
				if (recursoNode & 0b011) {   // REC_CORRENTE_30A e REC_CORRENTE_100A
					for (uint8_t fase=0; fase<3; fase++) {
						hello[0]++;
						dataClient++;
						strcat_P(data,str_field_ts);
						strcat(data,hello);
						itoa(*dataClient,nro,10);
						strcat(data,nro);
					}
				}
				#endif // HAS_MODBUS
				tam +=strlen(data)-mark;
				if (flag <5) {
					sATCIPSENDMultiple(flag,POINTER_CHAR_RAM, (uint8_t*)data,false);
				}
				*p='\0';

				// retorno de comando OK ou ERR
				if (bitRead(gParam.radio.hasCmdOK,node))
				ret++;
				else
				if (bitRead(gParam.radio.hasCmdERR,node))
				ret=2;
				if (ret) {
					strcat_P(data,PSTR("&cmd"));
					strcat(data,hello+2);
					itoa(ret,nro,10);
					strcat(data,nro);
				}
				// vcc
				#if HAS_MODBUS == 0
				strcat(data,"&vcc");
				strcat(data,hello+2);
				itoa(gParam.radio.vccClient[node],nro,10);
				strcat(data,nro);
				#if HAS_SENSOR_PORTA_9 == 1 || HAS_SENSOR_PORTA_9 == 1 || HAS_RELE==1
				// sensor 9, 10 ou rele
				// bits 0 e 1, sensor 9 e 10
				// bits 2 e 3, indicar troca se sinal
				// bit  4 situacao do RELE

				if (recursoNode & REC_SENSORES) {
					strcat_P(data,PSTR("&se"));
					strcat(data,hello);
					itoa(gParam.radio.sensor9_10[node],nro,10);
					strcat(data,nro);
				}
				#endif
				#endif // MODBUS
				} else {   // sem dados, definir se desativa o node
				PGM_PRINT("TIMEOUT=");
				debugln((millis()/1000)-gParam.radio.timeClient[node]);
				if (tipo && (millis()/1000)-gParam.radio.timeClient[node] > gParam.radio.inatividade) {
					strcat_P(data,PSTR("&nx")); // serie
					strcat(data,hello+2);
					itoa(gParam.radio.serieClient[node],nro,10);
					strcat(data,nro);
					bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
					flag_limpa=true;
					PGM_PRINT("clean=");
					debugln(node);
				}
				// testa erro comunicação I2C
				if (bitRead(gParam.radio.ComERR,node)) {
					strcat_P(data,PSTR("&ni")); // serie
					strcat(data,hello+2);
					itoa(gParam.radio.serieClient[node],nro,10);
					strcat(data,nro);
					bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
					flag_limpa=true;
				}
			}
			if (flag<5) {
				bitClear(gParam.radio.hasData,node);
				if (flag_limpa) {
					bitClear(gParam.radio.client,node);
					gParam.radio.serieClient[node]=0;
				}
				//bitSet(ccStatus,CC_DEBUG);
				//debugvarln(data);
				sATCIPSENDMultiple(flag,POINTER_CHAR_RAM, (uint8_t*)data,false);
			}
		} // if1
		tam += strlen(data)-mark;
		*p='\0';
		//debugvar (tam); debugvarln(data);
	} // for
	#ifdef __AVR_ATmega2560__
	if (pData == NULL) free(data);
	#endif
	
	PGM_PRINTLN("<pc");
	return tam;
}
#endif
/**
* \brief envia dados do sensores coletados, é necessário a api_key
* \param tipo: 0 = config   1=dados    2= JSON download dados offline
* \param thora timestamp da hora a ser enviada, pode ser null
**/
int updateDadosFeed(uint8_t tipo, uint32_t thora)
{
	static uint16_t last_id=0;
	static uint8_t t_reboot=MAX_T_REBOOT;
	char data[100];
	char *hello=reqHTTP+21;
	char *nro=reqHTTP+MAX_HTTPreq-8;
	//debugvarln(tipo);
	//	char nro[7];
	int retHTTP=0,tam;
	uint8_t i;
	uint8_t connectionId=1;
	#if HAS_I2C_COPY == 1
	uint8_t fases=gParam.rede.fases;
	#else
	uint8_t fases=B_FASE;
	#endif
	struct TDS20B18 *pT;
	uint16_t total;
	#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
	total=fDataLoggerLog;

	PGM_PRINT(">ts");
	// se DADOS_OFFLINE
	if (tipo==TUS_ENVIA_LOG && iDataLoggerLog == total) return true;
	#endif

	#if 1
	if (gParam.web.apy_key[0]=='\0') {
		PGM_PRINTLN("sem key");
		getTSConfig(GETTS_CONFIG_BOOT);  // se pegar a configuração, acerta o flag do WIFI_INTERNET

		led1_status = led2_status= LED_PISCA_FLASH; // TAG_1
		return false;
	}
	//debugvar(gParam.web.http_port);
	#if HAS_GSM == 1 && USE_ETH_SMS == 0
	//gprs.buffer=reqHTTP;
	connectionId=0;
	#endif
//	retHTTP=sATCIPSTARTMultiple(connectionId,gParam.web.server_addr,gParam.web.server_port,gParam.web.http_proxy,gParam.web.http_port);
	retHTTP=sATCIPSTARTMultiple(connectionId);
	if (!retHTTP)
	{
		//	PGM_PRINTLN("Err TS");
		return false;
	}
	led1_status=LED_PISCA_FLASH;

	#if HAS_ESP8266 == 1 || HAS_ETH == 1
	if (!send_html_header(connectionId,data, tipo)) goto CLOSE_CONN;
	#endif
	
	#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
	if (tipo==2) { // JSON
		strcpy_P(hello,str_Content_length);
		tam =10000;  // correto maxDataLoggerLog * 50 + acrescimo ? (20 bytes)
		itoa(tam,hello+strlen(hello),10);
		//debugvarln (tam);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)hello,true)) goto CLOSE_CONN;
		i=_BV(LISTA_LOG_JSON);
		if (bitRead(tipo,TUS_ENVIA_LOG_AT24))
		i |= _BV(LISTA_LOG_AT24);
		list_corrente_log(i,connectionId,data);
		
		goto FIM;
	}
	#endif
	// monta string dos dados
	hello[0]='1';
	hello[1]='=';
	data[0]=hello[2]='\0';
	if (tipo==1) {
		#if HAS_SENSOR_SECO == 1
		if (fases & B_FASE) {
			strcat_P(data,str_field_ts);
			strcat(data,hello);
			itoa(gDataLogger.field1,nro,10);
			strcat(data,nro);
		}
		#endif
		#if HAS_CORRENTE == 1 || HAS_I2C_COPY == 1
		uint16_t *fase=&gDataLogger.fase1;
		for (uint8_t b=0; b<3; b++) {
			hello[0]++;
			if (bitRead(fases,b)) {
				strcat_P(data,str_field_ts);
				strcat(data,hello);
				utoa(*(fase++),nro,10);
				strcat(data,nro);
			}
			
		}
		#else
		hello[0]='4';
		#endif
		
		#if HAS_TEMPERATURA == 1 || HAS_I2C_COPY == 1
		pT=pTemp;
		while (pT) {
			strcat_P(data,str_field_ts);
			hello[0]++;
			strcat(data,hello);
			//dtostrf(temperaturaAmbiente,4,1,nro);
			itoa(pT->temperatura,nro,10);
			strcat(data,nro);
			#if HAS_HUMIDITY == 1 || HAS_I2C_COPY == 11
			if (!pT->ds && (pT->addr[0] & 2) && humidade!=-1000) { // DEVICE_NOT_FOUND
				strcat_P(data,str_field_ts);
				hello[0]++;
				strcat(data,hello);
				itoa(humidade,nro,10);
				strcat(data,nro);
			}
			#endif
			//if (hello[0] == '8') hello[0]='1';
			pT=pT->p;
			if (hello[0] > '8') {
				bitSet(gParam.status,P_STATUS_RESET);
				break;
			}
		}

		#endif

		#if HAS_SENSOR_ANALOGICO == 1
		for (uint8_t ps=MAX_SENSOR_ANALOGICO; ps;) {
			ps--;
			if (gParam.porta_sensor_analogico[ps]) {
				strcat_P(data,str_field_ts);
				hello[0]='8'-ps;
				strcat(data,hello);
				itoa(sensor_analogico[ps]*10,nro,10);
				strcat(data,nro);
			}
		}
		#endif

		#if HAS_SENSOR_SECO == 1
		// valor do sensor
		strcat(data,"&sc=");
		itoa(gParam.sensor_seco.valor,nro,10);
		strcat(data,nro);
		// flag
		strcat(data,"&sf=");
		itoa(gParam.sensor_seco.trocou_web,nro,10);
		strcat(data,nro);
		#endif
		#if HAS_EVENTOS == 1
		// valor do sensor
		strcat(data,"&ee=");
		itoa(eventos_feed,nro,10);
		strcat(data,nro);
		#endif
	}
	
	tam=strlen(data);
	#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
	#ifdef __AVR_ATmega2560__
	tam +=processa_node(0xFF,tipo, NULL);
	#else
	tam +=processa_node(0xFF,tipo, data);
	#endif
	#endif
	debugvar (tam); debugvarln(data);


	if (!tipo) {
		#if USE_ENVIAR_IP == 1
		// &ip0=<addr2>&ip1=<addr1>   =5+5+LENs
		tam +=10+strlen(gParam.web.ip[0])+strlen(gParam.web.ip[1]);
		#endif
		} else {
		// updated_flag &f=xx
		tam +=5;
		#if USE_REAL_VCC == 1
		// &vcc=nnnn
		tam += 5+strlen(itoa((int)readVcc(),hello,10));
		#endif

	}
	#if HAS_RELE == 1 || HAS_SENSOR_PORTA_10 == 1 || HAS_SENSOR_PORTA_9 == 1
	#if HAS_RELE ==1
	sensor_9_10=digitalRead(RELE_PIN) << 4 | sensor_9_10 & 0x0f;
	#endif

	// &rr=n
	tam +=5+sensor_9_10/10;
	#endif
	
	// testa se teve erro no ultimo envio, se sim, passe indicador  &x
	if (!bitRead(ccStatus,CC_RET_HTTP))
	tam +=2;
	// &sq=nnnn
	tam += 4+strlen(utoa((uint16_t)gParam.id,hello,10));

	// &ts=nnnn
	tam += 4+strlen(ultoa((uint32_t)thora,hello,10));
	strcpy_P(hello,str_Content_length);
	// USE_LOCATION GSM
	// &l=-51.175896&g=-29.911957
	#if USE_LOCATION == 1
	tam +=28;
	#endif
	//    8+   16
	// api_key=<token>
	tam +=24;
	itoa(tam,hello+strlen(hello),10);
	debugvarln (tam);
	//debugvarln (hello);
	#if HAS_ESP8266 == 1 || HAS_ETH == 1
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)hello,true)) goto CLOSE_CONN;
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_TS_api_key,true)) goto CLOSE_CONN;
	#else
	if (!sATCIPSENDMultiple(tam,POINTER_CHAR_PROGMEM,(uint8_t *)str_TS_api_key,true)) goto CLOSE_CONN;
	connectionId=0;
	#endif

	// envia os dados
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)gParam.web.apy_key,true)) goto CLOSE_CONN;
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM, (uint8_t*)data,true)) goto CLOSE_CONN;

	#if HAS_NRF24L01 == 1 || HAS_I2C == 1|| HAS_MODBUS == 1
	//debugvarln(data);
	data[0]='\0';
	processa_node(connectionId,tipo, data);
	#endif
	
	if (!tipo) {
		#if USE_ENVIAR_IP == 1
		strcpy_P(data,PSTR("&ip0="));
		strcat(data,gParam.web.ip[0]);
		strcat_P(data,PSTR("&ip1="));
		strcat(data,gParam.web.ip[1]);
		#endif
		
		} else {
		data[0]='\0';
		#if USE_REAL_VCC == 1
		// &vcc=nnnn
		total=readVcc();
		strcat_P(data,PSTR("&vcc="));
		itoa(total,data+5,10);
		#endif
		// comando + status
		strcat_P(data,PSTR("&f=xx"));
		if (!isalnum(updated_flag)) updated_flag='0';
		if (!isalnum(rf24_cmd)) rf24_cmd='C';
		#if USE_REAL_VCC == 1
		data[12]=updated_flag;
		data[13]=rf24_cmd;
		#else
		data[3]=updated_flag;
		data[4]=rf24_cmd;
		#endif
		if (updated_flag != '0')
		bitSet(flagHTTP,F_HAS_RETURN);


		if (!bitRead(ccStatus,CC_RET_HTTP)) {
			strcat_P(data,PSTR("&x"));
		}

		strcat_P(data,PSTR("&sq="));
		utoa(gParam.id,data+strlen(data),10);
		
		//		gDataLogger.id=gParam.id;
		#if HAS_EEPROM == 1
		eeprom_write_word((uint16_t*)ID_DATALOGGER_ADDR,gParam.id);
		#endif


		strcat_P(data,PSTR("&ts="));
		ultoa(thora,data+strlen(data),10);
		

		#if HAS_RELE == 1 || HAS_SENSOR_PORTA_9 || HAS_SENSOR_PORTA_10
		// &rr=n
		strcat_P(data,PSTR("&rr="));
		//		data[8]=RELE_READ+'0';
		itoa(sensor_9_10,data+strlen(data),10);
		#endif
	}
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM, (uint8_t*)data,false)) goto CLOSE_CONN;
	#if USE_LOCATION == 1
	strcpy_P(data,PSTR("&l="));
	dtostrf(loc.lat,-11,6,data+strlen(data));
	strcat_P(data,PSTR("&t="));
	dtostrf(loc.lon,-11,6,data+strlen(data));
	nro=data;
	while (*nro++) {
		if (*nro==' ') *nro='0';
	}
	debugvarln(data);
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM, (uint8_t*)data,true)) goto CLOSE_CONN;
	#endif
	// final
	#if HAS_GSM == 1 && USE_ETH_SMS == 0
	if (!sATCIPSENDMultiple(-1,POINTER_CHAR_RAM, (uint8_t*)data,true)) goto CLOSE_CONN;
	#endif
	
	FIM:
	//debugvarln(data);
	retHTTP=endThingSpeak(reqHTTP,&connectionId); // data
	#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
	if (tipo==TUS_ENVIA_LOG && tam) retHTTP=200;
	#endif
	CLOSE_CONN:
	total=sATCIPCLOSEMulitple(connectionId);
	debugvar(updated_flag);
	if (total && retHTTP==200) {
		PGM_PRINTLN("ok");
		// OFFLINE
		debugvar(last_id);
		debugvarln(gParam.id);
		if (last_id+1 != gParam.id) {
			bitSet(flagHTTP,F_LOG_SYNC);
			PGM_PRINTLN("++SYNC");
		}
		last_id=gParam.id;



		if (updated_flag!='0' && bitRead(flagHTTP,F_HAS_RETURN))
		updated_flag='0';
		bitClear(flagHTTP,F_HAS_RETURN);
		#if HAS_SENSOR_PORTA_9==1 || HAS_SENSOR_PORTA_10==1
		sensor_9_10 &= ~(bit(SENSOR_9_TROCOU) | bit(SENSOR_10_TROCOU));
		#endif
		#if HAS_SENSOR_SECO == 11
		// so zera se tx ok
		//gParam.sensor_seco.trocou=0;
		#endif
		//rf24_cmd='a';
		
		#if HAS_DISPLAY == 1
		// col,lin
		lcd.setCursor(15,1);
		lcd.write('O');
		#endif
		//bitSet(ccStatus,CC_TWITTER_ONLINE);
		#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
		gParam.radio.hasCmdOK=0;
		gParam.radio.hasCmdERR=0;
		#endif
		
		} else {// erro RET HTTP != 200
		PGM_PRINTLN("err");
		#if HAS_DISPLAY == 1
		// col,lin
		lcd.setCursor(15,1);
		lcd.write('E');
		#endif
	}
	#if TINY_VERSION == 0
	if (bitRead(flagHTTP,F_HAS_CMD)) { // ??????
		if (executa_comando(rf24_cmd))
		updated_flag='K';
		else
		updated_flag='X';
		bitClear(flagHTTP,F_HAS_CMD);
	}
	#endif
	debugln(updated_flag);


	PGM_PRINTLN("<ts");

	led1_status=LED_APAGA;
	#endif
	return retHTTP;
}
/**
* \brief envia dados do sensores coletados, é necessário a api_key
* \param cmd : GETTS_CONFIG*
* \return 1=ok, 0=erro
*/
int getTSConfig(uint8_t cmd)
{
	char data[100];
	unsigned long t;
	uint8_t i;
	int tam2=0;
	PGM_PRINT(">getTSConfig ");
	#if HAS_BLUETOOTH >=2 || DEBUG > 0
	SSdebug.println(freeRAM());
	SSdebug.write('!');
	#endif
	
	int retHTTP=0,total;
	uint8_t connectionId=1;
	#if HAS_ETH == 1
	if (!Enc28J60.linkStatus()) {
		return 0;

	}
	#endif
	#if HAS_BLUETOOTH >=5
	SSdebug.println(">tg ");
	//	SSdebug.println(gParam.web.http_proxy);
	//	SSdebug.println(gParam.web.server_addr);
	#endif
	#if HAS_GSM == 1 && USE_ETH_SMS == 0
	//gprs.buffer=reqHTTP;
	connectionId=0;
	#endif
	retHTTP=sATCIPSTARTMultiple(connectionId);
	if (!retHTTP)
	{
		PGM_PRINTLN("\nECfg:"); // sem acesso internet ou servidor fora
		return 0;
	}
	led1_status=LED_PISCA_FLASH;
	#if HAS_ESP8266 == 1 || HAS_ETH == 1
	if (!send_html_header(connectionId,data,0)) return false;
	strcpy_P(data,PSTR("g=0"));
	data[2]=cmd;
	#else
	strcpy_P(data,PSTR("g=2"));
	#endif

	strcat_P(data,PSTR("&m=TS"));
	itoa(TSENSOR_MODELO,data+strlen(data),10);
	strcat_P(data,PSTR("&s="));
	itoa(TSENSOR_SERIE,data+strlen(data),10);

	strcat_P(data,PSTR("&rec="));
	uint32_t rec=TSENSOR_RECURSO;
	ultoa(rec,data+strlen(data),10);
	
	#if USE_LOG_EEPROM == 1
	
	strcat_P(data,PSTR("&fm="));
	utoa(maxDataLoggerLog,data+strlen(data),10);
	strcat_P(data,PSTR("&fi="));
	utoa(iDataLoggerLog,data+strlen(data),10);
	strcat_P(data,PSTR("&fc="));
	utoa(fDataLoggerLog,data+strlen(data),10);
	//sprintf_P(data+strlen(data),"&fm=%ud&fi=%ud&fc=%ud", maxDataLoggerLog,iDataLoggerLog,fDataLoggerLog);
	#endif

	#ifdef MD5
	strcat_P(data,PSTR("&md5="));
	strncat(data,gParam.chave,4);
	#endif
	
	#if USE_ENVIAR_IP == 2
	#if HAS_ESP8266 == 1
	strcat_P(data,PSTR("&ip0="));
	strcat(data,gParam.web.ip[0]);
	#endif
	strcat_P(data,PSTR("&ip1="));
	strcat(data,gParam.web.ip[1]);

	#if HAS_NRF24L01 == 1
	strcat_P(data,PSTR("&a=_"));
	*(data+strlen(data)-1)=gParam.radio.serv_addr;
	#endif

	#if HAS_ESP8266 == 1
	// &ap=   &wp=  &mac= &si=
	tam2 = strlen(gParam.web.ssid_ap)+strlen(gParam.web.passwd_ap)+
	strlen(gParam.web.rssi)+strlen(gParam.web.mac)+17;
	#endif
	#if HAS_ETH == 1
	strcat_P(data,PSTR("&mac="));
	strcat(data,gParam.web.mac);
	#endif
	#endif

	debugvarln(data);
	#if HAS_ESP8266 == 1 || HAS_ETH == 1
	strcpy_P(reqHTTP,str_Content_length);
	itoa(strlen(data)+tam2,reqHTTP+strlen(reqHTTP),10);
	strcat(reqHTTP,"\n\n");
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)reqHTTP,false)) return false;
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)data,false)) return false;
	#if HAS_ESP8266 == 1
	strcpy_P(data,PSTR("&ap="));
	strcat(data,gParam.web.ssid_ap);
	strcat_P(data,PSTR("&wp="));
	strcat(data,gParam.web.passwd_ap);
	strcat_P(data,PSTR("&mac="));
	strcat(data,gParam.web.mac);
	strcat_P(data,PSTR("&si="));
	strcat(data,gParam.web.rssi);
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)data,false)) return false;
	#endif
	
	#else
	if (!sATCIPSENDMultiple(strlen(data),POINTER_CHAR_RAM,(uint8_t *)data,false)) return false;
	if (!sATCIPSENDMultiple(-1,POINTER_CHAR_RAM,(uint8_t *)data,false)) return false;
	#endif
	
	endThingSpeak(reqHTTP, &connectionId);

	total=sATCIPCLOSEMulitple(connectionId);
	#if DEBUG > 0
	if (total) {
		PGM_PRINTLN("ok");
		} else {
		PGM_PRINTLN("err");
	}
	#endif
	

	//PGM_PRINTLN("<getTSConfig");
	led1_status=LED_APAGA;
	return retHTTP;
}
/*************************************************/
static void process_TS(char *d0) {
	char *dp, fn;
	// SSID
	// 01234
	// TS: 0 :<api_key>:<ssid>    :<passwd>              :<inatividade>:<fases>       :<ler_corrente>:<contador_web>:<contador_log>:<vcc_supply>
	//     : 0:FSBNUY8UWGNGTPAY::          :600:          7:       5:               12:              0          :-20
	
	// TA: 1 :<ip>     :<tensao>  :<porta_sens_analogico>:<ap_ssid>    :<ap_passwd>   :<ap_canal>    :<ap_cripto>
	// TS: 2 : enviar log da EEPROM
	// TS: 3 : enviar log de eventos da EEPROM
	// <updated_flag>
	PGM_PRINT(">_TS:");
	//SSdebug.write('\n');
	debugln(d0);
	dp=strchr(d0,':');
	if (dp != NULL) {
		*(dp++)='\0';
		//updated_flag = *d0;
		//if (updated_flag == 'W')   // gravar em EPROM
		#if USE_LOG_EEPROM == 1
		if (*d0=='2') bitSet(flagHTTP,F_LOG_SYNC);
		#endif
		#if USE_LOG_EVENTOS == 1
		if (*d0=='3') bitSet(flagHTTP,F_LOG_SYNC_EVENTOS);
		#endif
		if (*d0!='0' && *d0!='1') { return;} // não é para configurar
		fn=*d0;
		//SSdebug.println(fn);
		// <api_key> ou IP
		d0=dp;
		dp=strchr(d0,':');
		if (dp != NULL) {
			*(dp++)='\0';
			if (fn=='1') {
				//SSdebug.println(d0);
				if (strlen(d0)>=8 && strlen(d0)<=sizeof(TSSERVER_ADDR_IP)) strcpy(gParam.web.server_addr,d0);
				} else {
				if (strlen(d0)==16) strcpy(gParam.web.apy_key,d0);
			}
			bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
			// <ssid>
			d0=dp;
			dp=strchr(d0,':');
			if (dp != NULL) {
				*(dp++)='\0';
				if (fn=='1') { // tensao
					//tensao=bitRead(fases,B_TENSAO)?TENSAO_220:TENSAO_127;
					bitWrite(gParam.rede.fases,B_TENSAO,*d0=='2');
				}
				#if HAS_ESP8266 == 1
				else
				if (*d0) strcpy(gParam.web.ssid,d0);
				#endif
				// <passwd>
				d0=dp;
				dp=strchr(d0,':');
				if (dp != NULL) {
					*(dp++)='\0';
					#if HAS_ESP8266 == 1
					if (fn=='0') { // senha
						if (*d0) strcpy(gParam.web.passwd,d0);
					}
					#endif
					#if HAS_SENSOR_ANALOGICO == 1
					if (fn=='1') { // porta sensor analogico
						if (*d0) gParam.porta_sensor_analogico[0]=atoi(d0);
						d0=strchr(d0,',');
						if (d0++)
						if (*d0) gParam.porta_sensor_analogico[1]=atoi(d0);
					}
					#endif
					// <vcc>
					d0=dp;
					dp=strchr(d0,':');
					if (dp != NULL) {
						*(dp++)='\0';
						if (fn=='0') { 
						#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
						gParam.radio.inatividade=constrain(atoi(d0),60,3600);
						#endif
						}
						//PGM_PRINT(" vcc="); debugvarln(atoi(d0));
						// <fases>
						d0=dp;
						dp=strchr(d0,':');
						if (dp != NULL) {
							*(dp++)='\0';
							if (fn=='0') { // fases
								uint8_t fases=atoi(d0);
								if (!fases)
								bitClear(gParam.rede.fases,B_FORCE_FASE);
								else
								gParam.rede.fases=fases;
							}
							// <tempo_ler_corrente>
							d0=dp;
							dp=strchr(d0,':');
							if (dp != NULL) {
								*(dp++)='\0';
								if (fn=='0') { 
									// min=5seg e max=5min
									gParam.rede.tempoColetaCorrente=constrain(atoi(d0),5,300);
									// <contador_web>
								}
								d0=dp;
								dp=strchr(d0,':');
								if (dp != NULL) {
									*(dp++)='\0';
									if (fn=='0') { // 
										gParam.rede.tempoWebCorrente=constrain(atoi(d0),1,100);
										//  <tempoWebCorrente>
									}
									d0=dp;
									dp=strchr(d0,':');
									if (dp != NULL) {
										*(dp++)='\0';
										if (*d0) gParam.rede.tempoLOG=atoi(d0);
										// vcc
										d0=dp;
										dp=strchr(d0,':');
										if (dp != NULL) {
											*(dp++)='\0';
											gParam.rede.vcc_supply=atoi(d0);
										} // vcc
									} // contador_log
								} // contador_web
							} // tempo_ler_corrente
						} // fases
					} // inatividade
				} // passwd
			} // ssid
		} // api_key
		updated_flag='K';
		//rf24_cmd='B'; // boot
		rf24_cmd='C'-bitRead(ccStatus,CC_BOOT_RESET); // boot =C, boot reset planejado=B
		//bitSet(ccStatus,CC_HAS_STATUS_MSG);
		//strcpy_P(TS_status,PSTR("Cfg ok"));
	} // updated_flag
}
/*************************************************/
#if TINY_VERSION == 0
static void process_TN(char *d0) {
	char *dp;
	uint16_t val;
	char *par1;
	PGM_PRINTLN(">pTN");
	//char node[]="x ok";
	// nro_no == 0 - para nó principal o comando
	// TN: <updated_flag_no> ou <nro_no>:<pk comando>:<comando>:<parametro1>:<parametro2>:<parametro3>
	// <updated_flag>
	//		6:<pk>:1:1:5:1000:0
	//debugvarln(d0);
	dp=strchr(d0,':');
	if (dp != NULL) {
		*(dp++)='\0';
		val=atoi(d0);
		// testa se existe cliente
		#if HAS_NRF24L01 == 1 || HAS_I2C == 1
		if (val>MAX_TSENSOR_CLIENT || !bitRead(gParam.radio.client,val-1)) {
			updated_flag='X'; // não é para configurar
			return;
		}
		#endif
		// <nro_no>
		if (!val) {
			bitSet(flagHTTP,F_HAS_CMD);  // tem comando recebido do site
			} else {
			#if HAS_NRF24L01 == 1 || HAS_I2C == 1
			bitSet(gParam.radio.hasCmd,--val);
			updated_flag ='A'+val;
			#else
			updated_flag='X'; // não é para configurar
			return;
			#endif
		}
		// <comando>
		d0=dp;
		dp=strchr(d0,':');
		if (dp != NULL) { //cmd
			*(dp++)='\0';
			rf24_cmd= *d0;
			debugvarln(rf24_cmd);
			// <parametro1>
			d0=dp;
			dp=strchr(d0,':');
			if (dp != NULL) { // par1
				*(dp++)='\0';
				rf24_par1=atoi(d0);
				par1=d0;
				//debugvarln(rf24_par1);
				// <parametro2>
				d0=dp;
				dp=strchr(d0,':');
				if (dp != NULL) { // par2
					*(dp++)='\0';
					rf24_par2=atoi(d0);
					//debugvarln(rf24_par2);
					// <parametro3>
					d0=dp;
					//	dp=strchr(d0,':');
					//	if (dp != NULL) { // par3
					//		*(dp++)='\0';
					rf24_par3=atoi(d0);
					//	} // parametro3
				} // parametro2
			} // parametro1
		} // cmd
	} // updated_flag

}

/******************************************************/
static uint8_t executa_comando(uint8_t cmd) {
	int i=rf24_par1;
	int j=rf24_par2;
	int k=rf24_par3;
	uint8_t ret=true;
	debugvar(cmd);
	debugvar(i);
	debugvar(j);
	debugvarln(k);
	switch (cmd) {
		#if USE_LEDS == 1
		case FR24_CMD_LED1:
		pisca(LED,i,j);
		break;
		case FR24_CMD_LED2:
		pisca(LED_VERMELHO,i,j);
		break;
		#if (TSENSOR_RECURSO & REC_LED2)
		case FR24_CMD_LED3:
		pisca(LED_AMARELO,i,j);
		break;
		#endif
		#endif
		case FR24_CMD_REBOOT:
		PGM_PRINT("======");
		bitSet(gParam.status,P_STATUS_RESET);
		break;
		case FR24_RESET_FIRMWARE:
		bitSet(gParam.status,P_STATUS_RESET_FIRMWARE);
		break;
		#if HAS_NRF24L01 == 1
		case FR24_REMOVE_NODE:
		i--;
		if (i < MAX_TSENSOR_CLIENT && bitRead(gParam.radio.client,i)) {
			bitClear(gParam.radio.client,i);
			gParam.radio.serieClient[i]=0;
			rf24_cmd=FR24_REMOVE_NODE;
			bitSet(gParam.radio.hasCmd,i);
			updated_flag ='A'+i;
			} else {
			ret=false;
		}
		bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
		break;
		case FR24_LIMITES:
		i--; // numero do node
		gParam.radio.min[i]=j;
		gParam.radio.max[i]=k;
		bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
		break;
		#endif
		#if HAS_RELE == 1
		case FR24_CMD_RELE:
		PGM_PRINTLN("rele ");
		if (i<2)
		digitalWrite(RELE_PIN,i);
		else
		pisca(RELE_PIN,j,k);
		break;
		#endif
		#if HAS_LEDS == 1
		case FR24_OUTPUT:
		pinMode(i,OUTPUT);
		if (j<2)
		digitalWrite(i,j);
		else
		pisca(i,j-2,k);
		PGM_PRINTLN("rele ");
		break;
		#endif
		#if HAS_IR_REMOTE == 1
		case FR24_IR_SETUP:
		hex_decode(par,26,&gParam.ir.khz);
		break;
		case FR24_SEND_IR_CMD1:
		case FR24_SEND_IR_CMD2:
		send_ir_command(cmd-FR24_SEND_IR_CMD1);
		break;
		#endif
		default:
		ret=false;
	}
	timerWeb=0;
	return ret;
}
#endif
/***********************************************************************************/
#endif