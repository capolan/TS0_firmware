/**
 * @file TSensor.h
 * @author CAP
 * @date 1 Dez 2015
 * @brief Definições principais, incluido por todos os arquivos .cpp.
 *
 */


#ifndef TSENSOR
#define TSENSOR
#include <Arduino.h>
#include <OneWire.h>

#include "TSensor_config.h"
#include "TSensor_def.h"

#ifdef __AVR_ATmega1284__
#define HAS_OLED 1
#endif

#if HAS_OLED == 1
//#include "Oled_display.h"
extern Oled_display oled;
#endif

#if HAS_GSM == 1
//#include <SIM800.h>
#endif
#define USE_LOCATION HAS_GSM

#ifndef USE_ETH_SMS
#define USE_ETH_SMS 0
#endif

#ifndef USE_SSL
#define USE_SSL 0
#endif

#if HAS_CORRENTE == 1
#include <EmonLib.h>
#endif

//#include <IPAddress.h>
//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <avr/power.h>
#if DEBUG > 0 || HAS_BLUETOOTH >= 1
#include <SoftwareSerial.h>
#endif

#ifndef TINY_VERSION
#define TINY_VERSION 0
#endif

#ifndef HAS_I2C
#define HAS_I2C 0
#endif
#define HAS_I2C_COPY HAS_I2C

#ifndef HAS_BLE
#define HAS_BLE 0
#endif

#ifndef HAS_RFID
#define HAS_RFID 0
#endif

#if HAS_RFID == 1
#define RXRFID 7
#define TXRFID 8

#define STX 0x02
#define ETX 0x03
#endif


#ifndef USE_LOG_EEPROM
#if TINY_VERSION==0
#define USE_LOG_EEPROM 1  // 100bytes RAM
#else
#define USE_LOG_EEPROM 0
#endif
#endif // USE_LOG_EEPROM

#ifndef USE_LOG_EVENTOS
#define USE_LOG_EVENTOS 0
#endif

#ifndef USE_DHCP
#define USE_DHCP  1
#endif

#ifndef HAS_GSM_TCP
#define HAS_GSM_TCP 0
#endif

#ifndef HAS_WEBSERVER_FULL
#define HAS_WEBSERVER_FULL 1
#endif
#ifndef HAS_WEBSERVER_TINY
#define HAS_WEBSERVER_TINY 0
#endif
#define MAX_T_REBOOT 4


//// MODULOS
#define USE_ENVIAR_IP 2
#define USE_TALKBACK 0
#define USE_MUX 1
#define USE_PAGINA_MOB_PRINCIPAL 0 // 194bytes
#define USE_TEMP_EEPROM 1	// grava em eeprom a tempratura - log
#define USE_TS_FIELD 0 // 150bytes configuracao do campo na WEB default=canal1
#define USE_REAL_VCC 1	// ler a tensao do processasor
#define USE_WIFI_HARDWARE	1 // conxao wifi via harware uart
#define USE_STR_INPUT_FCN 0

#ifndef HAS_CORRENTE
#define HAS_CORRENTE 0
#endif
#define HAS_TIMER_INT 1

#ifndef USE_LEDS
#define USE_LEDS 1
#endif

#ifndef HAS_BOTAO
#define HAS_BOTAO 1
#endif

#ifndef HAS_CLOCK
#if TINY_VERSION == 1
#define HAS_CLOCK 0
#else
#define HAS_CLOCK 1
#endif
#endif


#ifndef HAS_SENSOR_ANALOGICO
#define HAS_SENSOR_ANALOGICO 0
#endif
#ifndef SENSOR_ANALOGICO_PIN
#define SENSOR_ANALOGICO_PIN A5
#endif
#ifndef SENSOR_ANALOGICO_PIN_2
#define SENSOR_ANALOGICO_PIN_2 A4
#endif
#define MAX_SENSOR_ANALOGICO 2


#ifndef HAS_NRF24L01
#define HAS_NRF24L01 0
#endif
#ifndef HAS_ESP8266
#define HAS_ESP8266 1
#endif

#ifndef HAS_ETH
#define HAS_ETH 0 ///< indica existencia de modulo Ethernet ENC28J60
#endif

#ifndef HAS_GSM
#define HAS_GSM 0 ///< indica existencia de modulo GSM SIM800L
#endif

#ifndef USE_REAL_RTC
#define USE_REAL_RTC 0
#endif

#ifndef USE_AT24
#define USE_AT24 0 ///< indicao uso da memoria i2c
#endif

#ifndef HAS_BLUETOOTH
#define HAS_BLUETOOTH 0 ///< indica se tem bluetooth, usado na depuracao, 1=debug, 2=+debug, 10= +++debug
#endif

#if HAS_ESP8266 == 1
#define HAS_ESP9266_RESET 1
#else
#define HAS_ESP9266_RESET 0
#endif

#ifndef USE_CORRENTE_FIELD_1
#define USE_CORRENTE_FIELD_1 1
#endif

#ifndef HAS_IR_REMOTE
#define HAS_IR_REMOTE 0
#endif

#ifndef HAS_DISPLAY
#define HAS_DISPLAY 0
#endif

#ifndef HAS_MODBUS
#define HAS_MODBUS 0
#endif

#ifndef HAS_EVENTOS
#define HAS_EVENTOS 0
#endif

#ifndef HAS_MONITORAMENTO
#define HAS_MONITORAMENTO 1
#endif

#define HAS_THINGSPEAK 1
#define HAS_TS_SERVER 1 // servidor TS
#define HAS_EMONLIB 1
#ifdef __AVR__
#define HAS_EEPROM 1
#else
#define HAS_EEPROM 0
#endif

#ifndef HAS_RELE
#define HAS_RELE 0
#endif

/***************/
/*  TEMP       */
/***************/
#ifdef HAS_DHT11 
#define HAS_DHT 1
#else
#define HAS_DHT11 0
#endif

#ifdef HAS_DHT22
#define HAS_DHT 1
#else
#define HAS_DHT22 0
#endif

#ifndef HAS_HUMIDITY
#define HAS_HUMIDITY 0
#endif

#if defined(HAS_DS18B20_EXTRA_2)
#define HAS_TEMPERATURA 1
#define HAS_TEMPERATURA_2 1
#define MAX_SENSORES_DS18B20 3
#elif defined(HAS_DS18B20_EXTRA)
#define HAS_DS18B20_EXTRA_2 0
#define HAS_TEMPERATURA 1
#define HAS_TEMPERATURA_2 1
#define MAX_SENSORES_DS18B20 2
#elif defined(HAS_DS18B20) || defined(HAS_LM35) || defined(HAS_DHT)
#define HAS_DS18B20_EXTRA_2 0
#define HAS_TEMPERATURA 1
#define HAS_TEMPERATURA_2 0
#define MAX_SENSORES_DS18B20 1
#else
#define HAS_TEMPERATURA 0
#define HAS_TEMPERATURA_2 0
#define HAS_LM35 0
#define HAS_DHT 0
#define HAS_DS18B20 0
#define HAS_DS18B20_EXTRA 0
#define HAS_DS18B20_EXTRA_2 0
#define MAX_SENSORES_DS18B20 0
#endif

#if HAS_DS18B20 == 1 || HAS_DS18B20_EXTRA == 1|| HAS_DS18B20_EXTRA_2 == 1
#define MULTIPLE_DS18B20 1
#else
#define MULTIPLE_DS18B20 0
#endif

#if HAS_CLOCK == 1
#include "Time_c.h"
#endif

#define CORRENTE_1 A0
#define CORRENTE_2 A1
#define CORRENTE_3 A2
#ifndef TEMPERATURA_AMBIENTE_PIN
#if defined(__AVR_ATmega328p__) || defined(__AVR_ATmega1284__)
#define TEMPERATURA_AMBIENTE_PIN A3
#else
#define TEMPERATURA_AMBIENTE_PIN A3 //SCL
#endif
#endif
#define TEMPERATURA_AMBIENTE_2_PIN A2

#ifndef TEMPERATURA_AMBIENTE_3_PIN
#define TEMPERATURA_AMBIENTE_3_PIN A1
#endif

#ifndef TEMPERATURA_DHT_PIN
#define TEMPERATURA_DHT_PIN TEMPERATURA_AMBIENTE_PIN
#endif

#ifndef MAX_CORRENTE_FASES
#define MAX_CORRENTE_FASES 3
#endif

#ifndef USE_SENSOR_SECO_PORTA_BAIXA
#define USE_SENSOR_SECO_PORTA_BAIXA 0
#endif

#define ESP8266_VERSION_0922 0 // 0924
#define ESP8266_VERSION_0952 1 //
#define ESP8266_VERSION_14   0 //


#ifdef __AVR_ATmega2560__
#define LED 13
#define LED_PORT PB7
#define LED_ON digitalWrite(LED,HIGH)
#define LED_OFF digitalWrite(LED,LOW)
#define LED_READ digitalRead(LED)
#else

#if TSENSOR_SERIE >= 3 || TSENSOR_SERIE == 0
#define RXWIFI 5
#define TXWIFI 6
// BOTAO
#define BOTAO_PIN 2 // INT0 PD2  PORTD2
#define BUTTON_READ (PIND & bit(PORTD2))
// LED VERDE
#ifndef LED
#define LED 3 //13
#define LED_PORT PORTD3 //5
#define LED_SETUP DDRD |= bit(LED_PORT) // output https://hekilledmywire.wordpress.com/2011/02/23/direct-port-manipulation-using-the-digital-ports-tutorial-part-3/
#define LED_ON PORTD |= bit(LED_PORT)
#define LED_OFF PORTD &= ~bit(LED_PORT)
#define LED_READ (PIND & bit(LED_PORT))
#endif
// LED_VERMELHO
#ifndef LED_VERMELHO
#define LED_VERMELHO 9
#define LEDV_PORT PORTB1
#define LEDV_SETUP DDRB |= bit(LEDV_PORT) // output 
#define LEDV_ON PORTB |= bit(LEDV_PORT)
#define LEDV_OFF PORTB &= ~bit(LEDV_PORT)
#define LEDV_READ (PINB & bit(LEDV_PORT))
#endif

// LED_AMARELO
#define LED_AMARELO 10
#define LEDA_PORT PORTB2
#define LEDA_SETUP DDRB |= bit(LEDA_PORT) // output
#define LEDA_ON PORTB |= bit(LEDA_PORT)
#define LEDA_OFF PORTB &= ~bit(LEDA_PORT)
#define LEDA_READ (PINB & bit(LEDA_PORT))


#endif

#endif // __AVR_ATmega2560__ 

// http://www.atmel.com/images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet_Complete.pdf
//		pag 73
// PCIE2  PCINT23:16
// PCIE1  PCINT14:8
// PCIE0  PCINT7:0
#ifndef BOTAO_PIN
#define BOTAO_PIN 10 // PCINT2 PB2  PORTB2
#define BUTTON_READ (PINB & bit(PORTB2))
#endif

#if HAS_RELE == 1
#ifndef RELE_PIN
#define RELE_PIN	10//	3 //6
//#define RELE_READ (PIND & bit(PORTD5))
#define RELE_READ (PINB & bit(PORTB2))
#endif
#endif

#if USE_LEDS == 1
// LED VERDE
#ifndef LED
#define LED 13 //3 //13
#define LED_PORT 5 //3//5
#define LED_SETUP DDRB |= bit(LED_PORT) // output
#define LED_ON PORTB |= bit(LED_PORT)
#define LED_OFF PORTB &= ~bit(LED_PORT)
#define LED_READ (PINB & bit(LED_PORT))
#endif
// LED VERMELHO
#ifndef LED_VERMELHO
#define LED_VERMELHO 5 // PORTD5
#define LEDV_PORT 5
#define LEDV_SETUP DDRD |= bit(LEDV_PORT) // output
#define LEDV_ON PORTD |= bit(LEDV_PORT)
#define LEDV_OFF PORTD &= ~bit(LEDV_PORT)
#define LEDV_READ (PIND & bit(LEDV_PORT))
#endif
// LED AMARELO
#ifndef LED_AMARELO
#define LED_AMARELO 8
#define LEDA_PORT PB0
#define LEDA_SETUP DDRB |= bit(LEDA_PORT) // output
#define LEDA_ON led1_status=LED_LIGAR;
#define LEDA_OFF	led1_status=LED_APAGA;
#define LEDA_READ
#endif

#else // USE_LEDS
// LED VERDE
#define LED 13 //3 //13
#define LED_ON 
#define LED_OFF 
#define LED_READ 
// LED VERMELHO
#define LED_VERMELHO 5 // PORTD5
#define LEDV_ON 
#define LEDV_OFF 
//#define LEDV_READ 
// LED AMARELO
#define LED_AMARELO 8
#define LEDA_ON 
#define LEDA_OFF	
#endif // USE_LEDS

// sensor_9_10

#ifndef HAS_SENSOR_PORTA_9
#define HAS_SENSOR_PORTA_9	0
#endif
#ifndef HAS_SENSOR_PORTA_10
#define HAS_SENSOR_PORTA_10	0
#endif

#define SENSOR_9_PIN LED_VERMELHO
#define SENSOR_10_PIN LED_AMARELO
#define SENSOR_9_READ LEDV_READ
#define SENSOR_10_READ LEDA_READ
#define SENSOR_9_SETUP LEDA_SETUP
#define SENSOR_10_SETUP LEDV_SETUP


#define SENSOR_9_POSICAO 0
#define SENSOR_10_POSICAO 1
#define SENSOR_9_TROCOU 2
#define SENSOR_10_TROCOU 3
#define RELE_POSICAO 4


#ifndef RXWIFI
#define RXWIFI 6 // PD6 PCINT22
#define TXWIFI 7 // PD7 PCINT23
#endif

//#define SSwifi Serial
#define ESP8266_RX 0
#define ESP8266_TX 1
#define ESP8266_RST_PIN	4

#ifdef __AVR_ATmega2560__
#define SSwifi Serial1
#define SSdebug Serial
#elif defined(__AVR_ATmega1284__)
#define SSwifi Serial
#define SSdebug Serial1
#else
#if DEBUG > 0 || HAS_BLUETOOTH >= 1
extern SoftwareSerial SSdebug;
#endif
#if USE_WIFI_HARDWARE == 1

#if HAS_ESP8266 == 1 || HAS_GSM == 1 || HAS_GSM_TCP == 1
#define SSwifi Serial
//#define SERIAL_RX_BUFFER_SIZE 256
//#define SERIAL_TX_BUFFER_SIZE 50
#endif

#else
extern SoftwareSerial SSwifi;
#endif
//#define SSdebug Serial
#endif

#if HAS_ETH == 1 
#define SSwifi client
#define PERMITE_TROCAR_AP 0
#endif
// 3.3V
//#define VCC_SUPPLY	3283		// USBASP
//#define VCC_SUPPLY	3356		// 5v 1.2A/9v 0.6A  dlink  3352 e 3362
//#define VCC_SUPPLY	3352		// 5v 0.5A  iphone
// 5V
#if USE_REAL_VCC == 1
#define VCC_SUPPLY	0 // valor a ser somado/subtraido da tensao do processador
#else
#define VCC_SUPPLY	5100 // 4950 // leitura no LM35 //5200		// USBASP 5v 0.5A iphone
#endif

#undef SSID_AP
#ifndef SSID_AP
//+CWSAP:"ESP_9D88F5","",1,0
#define SSID_AP ""
#endif
#define PASSWD_AP "1234"
#define CANAL_AP 11
#define CRIPTO_AP 4

#define MAX_HTTPreq 100
#define HTTP_NONE	0
#define HTTP_GET	1
#define HTTP_POST	2

#define HTTP_RESP_TEXT 0
#define HTTP_RESP_CSV 1
#define HTTP_RESP_JSON 2

#define FCN_CMD_WIFI 1
#define FCN_CMD_THINGSPEAK 2
#define FCN_CMD_FASES 3
#define FCN_CMD_SETUP_AP 4

#define FCN_CMD_RESET 7
#define FCN_CMD_RESET_FIRMWARE 8
#define FCN_CMD_RESET_ESP8266 9

#define CMD_FUNCAO "f"
#define CMD_FUNCAO_C 'f'
#define CMD_RET_STATUS 'R'   // seta reqHTTP bit.F_RET_STATUS = true
#define CMD_RET_GET_IP 'I'	// retorna o IP do TS
#define CMD_SSID	"s"
#define CMD_SSID_C	's'
#define CMD_PASSWD	"p"
#define CMD_PASSWD_C	'p'
#define CMD_CHAVE "z"   // chave md5
#define CMD_CHAVE_C 'z'
#define CMD_PROXY "y"
#define CMD_PROXY_C 'y'
#define CMD_SER_ADDR "a"
#define CMD_SER_ADDR_C 'a'

#define CMD_USE_DHCP "h"
#define CMD_USE_DHCP_C 'h'

#define CMD_IP	"s"
#define CMD_IP_C	's'
#define CMD_GW	"p"
#define CMD_GW_C	'p'
#define CMD_DNS	"n"
#define CMD_DNS_C	'n'
#define CMD_MASK	"m"
#define CMD_MASK_C	'm'


#define CMD_CRIPTO "y"
#define CMD_CRIPTO_C 'y'

#define CMD_SEND_TWITTER "w"
#define CMD_TOKEN_TS	"t"
#define CMD_TOKEN_TS_C	't'
#define CMD_CANAL_TS	"c"
#define CMD_CANAL_TS_C	'c'
#define CMD_PLUGIN_TS	"p"
#define CMD_PLUGIN_TS_C	'p'
#define CMD_ADDR_IP_TS	"i"
#define CMD_ADDR_IP_TS_C	'i'

#define CMD_HORA	"h"
#define CMD_DATA	"d"

#define CMD_WRITE_EEPROM	"w"

#define CMD_FIELD_CORRENTE_TS "b"
#define CMD_FASES_NRO "n"
#define CMD_FASES_NRO_C 'n'
#define CMD_FASES_TENSAO "v"
#define CMD_FASES_TENSAO_C 'v'
#define CMD_MODO_TESTE "t"
#define CMD_VCC_SUPPLY	"s"

// TIMER
#define USE_TIMER1 0  // 16 bits   conta 1 segundo
#define USE_TIMER2 0  // 8 bits
#define USE_WATCHDOG_TIMER 1

#define OCR2A_COUNT 250        //125   250
#define TIMER2A_CYCLE 62     //125   62,5

// CORRENTE
// fases geradas
#define B_FASE_1 0
#define B_FASE_2 1
#define B_FASE_3 2
#define B_FASE B0111   // bit 0,1 e 2
#define B_FORCE_FASE 3
#define B_TENSAO 4    // bit 4
#define B_PORTA_A0 5 // 0=corrente, 1=temp
#define B_PORTA_A1 6 // 0=corrente, 1=temp
#define B_PORTA_A2 7 // 0=corrente, 1=temp
//#define B_TEMPERATURA 0x70

#if USE_LOG_EVENTOS == 0 && HAS_SENSOR_SECO==0
struct TDataLogger {
	uint16_t id;
	#if HAS_CLOCK == 1
	time_t hora;
	#endif
	#if HAS_CORRENTE == 1 || HAS_I2C_COPY == 1
	uint16_t fase1,fase2,fase3;
	#endif
	#if HAS_TEMPERATURA == 1
	#if MAX_SENSORES_DS18B20 == 0
	uint8_t temp[1];
	#else
	uint8_t temp[MAX_SENSORES_DS18B20];
	#endif
	#endif
	#if HAS_HUMIDITY == 1
	uint8_t humidade;
	#endif
	#if HAS_SENSOR_ANALOGICO == 1
	uint16_t sensor_analogico[2];
	#endif
	#if HAS_SENSOR_SECO == 1
	uint8_t sensor_seco_valor;
	uint8_t sensor_seco_trocou;
	#endif
	};
#endif

/// Define os tipos de evento
enum TEvento_tipo {
	EVENTO_NULL = 0,	///< sem evento definido
	EVENTO_SENSOR_SECO, ///< nao evento, indica troca de estado
	EVENTO_FICOU_0,		///< trocou para estado 0, rele fechado
	EVENTO_FICOU_1,		///< trocou para 1, rele aberto
	EVENTO_TROCA_ESTADO,	///< trocou de estado, independente da posicao
	EVENTO_CONTAR_TEMPO_MIN, ///< sem tempo de acionamento for < param[2] alarma
	EVENTO_CONTAR_TEMPO_MAX, ///< sem tempo de acionamento for > param[2] alarma
	EVENTO_CRONOMETRO,   ///< cronometra tempo que ficou acionado 
	EVENTO_COMBINACAO_2,  ///< quando param[0] e param[1] tiverem igual a param[2] - 2 bits por param[3] seguns alarma
	EVENTO_FREQUENCIA, ///< ociona quando evento ocorrente em param[1] vezes em timeout tempo
	EVENTO_MIN_VALOR,	///< valor minimo para acionar evento, unsigned int
	EVENTO_MAX_VALOR  ///< valor maximo para acionar evento, regra : porta analogica 0-1023   param[2] << 8 || param[3] é a referencia
};

/// bits que definem status do evento
enum TEvento_status_bit {EVENTO_ACIONADO =0, EVENTO_MARCA_INICIAL, EVENTO_ALARMADO, EVENTO_DESATIVADO, EVENTO_ERRO=7};

/// Funcao a ser executada caso evento eh detectado
enum TEVENTO_FUNC { FUNC_ACIONA_PORTA, FUNC_PISCA, FUNC_NULL=0xff };
	
/// Define os eventos que devem ser tratados, cada evento esta armazenado nesta estrutura
struct TDEvento {
	TEvento_tipo tipo;   ///< 0 - troca, 1 -combinação de 2 bits, 3 - frequencia
	uint8_t param[4];	///< parametros do tipo do evento
	TEVENTO_FUNC func_idx; ///< funcao a ser executada caso evento eh detectado
	uint8_t status; ///< bits TEvento_Status_bit
	unsigned long mark; ///< usado para marcar o tempo dos eventos
};

#if USE_LOG_EVENTOS == 1 || HAS_SENSOR_SECO == 1
struct TDataLogger {
	uint16_t id;
	#if HAS_CLOCK == 1
	time_t hora;
	#endif
	#if HAS_CORRENTE == 1 || HAS_I2C_COPY == 1
	uint16_t fase1,fase2,fase3;
	#endif
	#if HAS_TEMPERATURA == 1
	#if MAX_SENSORES_DS18B20 == 0
	uint8_t temp[1];
	#else
	uint8_t temp[MAX_SENSORES_DS18B20];
	#endif
	#endif
	#if HAS_HUMIDITY == 1
	uint8_t humidade;
	#endif
	#if HAS_SENSOR_ANALOGICO == 1
	uint16_t sensor_analogico[2];
	#endif
	#if HAS_SENSOR_SECO == 1
	uint8_t sensor_seco_valor;
	uint8_t sensor_seco_trocou;
	uint16_t field1;
	#endif
	//TEvento_tipo 
	uint8_t tipo;
	uint8_t porta;
	uint8_t funcao;

};
#endif

#define MAX_SENSOR_READING 3
#if HAS_SENSOR_SECO == 1
#define MAX_SENSOR_SECO 6

//
// tipo
//   bit 0 : INPUT ou INPUT_PULLUP
//   bit 1 : aviso imediato
//	 bit 2 : leitura rápida, sem intervalo
#define SECO_INPUT_PULLUP_BIT 0
#define SECO_FAST_READ_BIT 1
#define SECO_IMEDIATO_BIT 2
struct TSensor_Seco {
	uint8_t valor;
	uint8_t trocou;
	uint8_t trocou_web;
	uint16_t timeout;
	uint8_t porta[MAX_SENSOR_SECO];
	uint8_t tipo[MAX_SENSOR_SECO];
	uint8_t freq[MAX_SENSOR_SECO]; // millis
	uint32_t mark[MAX_SENSOR_SECO]; // millis
};
#endif

#ifndef TEMPO_COLETA_CORRENTE
#define TEMPO_COLETA_CORRENTE 30
#endif
#define CONTADOR_WEB_CORRENTE 4 // 4 x TEMPO_COLETA_CORRENTE
#define CONTADOR_LOG 1
#define NRO_AMOSTRAS_CORRENTE 1480
#ifndef CALIBRAGEM_CORRENTE
#define CALIBRAGEM_CORRENTE 0  // 60.6 -> 100A   30.0 -> 30A
#endif

#define CALIBRAGEM_CORRENTE_30A 30
#define CALIBRAGEM_CORRENTE_100A 60.6

#define TENSAO_127	127
#define TENSAO_220	220

struct TPRede {
	uint8_t fases;
	int vcc_supply;
	uint16_t tempoColetaCorrente;
	uint8_t tempoWebCorrente;
	uint8_t tempoLOG; // frequencia de gravação no LOG, multiplo de tempoWebCorrente
	uint8_t calibragemFase1; // bit 0 = A0, 1=A1, 2=A2  valor 0= 30.0  30A   1=60.6   100A
	uint16_t nro_amostras_corrente;
};

// NRF24L01/I2C
#ifndef MAX_TSENSOR_CLIENT
#define MAX_TSENSOR_CLIENT 4
#endif
#ifndef MAX_TSENSOR_SENSOR
#define MAX_TSENSOR_SENSOR 5  // nro de sensores em cada cliente
#endif
#define SERV_ADDR	"SILCT"

#ifndef SERV_ADDR_1
#define SERV_ADDR_1	'U'
#endif

#define SERV_ADDR_BASE	'S'

struct TPnrf24 {
	char serv_addr;
	uint8_t addr[MAX_TSENSOR_CLIENT];
	uint8_t client; // bit-0 primeiro client field=6 
					// bit1- segundo client field=7
					// bit2- terceiro client field=8
	uint16_t inatividade; // em segundos
	uint8_t hasData;   // se tem dados valido
	uint8_t hasCmd;   // se tem comando
	uint8_t hasCmdOK;   // se tem retorno comando OK
	uint8_t hasCmdERR;   // se tem reotrno comando ERR
	uint8_t ComERR;    // erro comunicação i2c
	uint8_t sensor9_10[MAX_TSENSOR_CLIENT];
	uint8_t seqClient[MAX_TSENSOR_CLIENT];
	uint16_t serieClient[MAX_TSENSOR_CLIENT]; // nro série do cliente
	uint16_t recursoClient[MAX_TSENSOR_CLIENT]; // nro série do cliente
	uint16_t vccClient[MAX_TSENSOR_CLIENT]; // vcc em 2 byte , d.d (ex.:5096 volts)
	int dataClient[MAX_TSENSOR_CLIENT][MAX_TSENSOR_SENSOR]; // ultimo dado enviado pelos clientes
	#if HAS_CLOCK == 1
	time_t timeClient[MAX_TSENSOR_CLIENT]; // tempo da última conexão
	#else
	uint32_t timeClient[MAX_TSENSOR_CLIENT]; // tempo da última conexão
	#endif
};
	
	
extern uint8_t rf24_cmd; // comando para os nos
extern int rf24_par1;
extern int rf24_par2;
extern int rf24_par3;

#define PKG_CMD_GET_NODE '0'  // 48
#define PKG_CMD_SET_NODE '1'
#define PKG_CMD_SEND_DATA '2'	// 50
#define PKG_CMD_SEND_DATA_CMD_STATUS '3'	
#define PKG_CMD_RECEIVE_DATA '5'
#define PKG_CMD_RECEIVE_DATA_WITH_CMD '6'  // recebeu dados OK, mas leva comandos
#define PKG_CMD_ERR_TIMEOUT 'E'
#define PKG_CMD_ERR_UNKNOW_CLIENT 'U'	// 85 cliente desconhecido
#define PKG_CMD_ERR_FULL_CLIENT 'F'   // 70  lotada tabela de clientes
#define PKG_CMD_ERR_SEQ 'S' // 83 nro de seq msg invalida
#define PKG_CMD_ERR_COMMAND_CLIENT 'C' // 67 comando inválido
#define PKG_CMD_ERR_SEQ_DUPLO 'D' // 68

#define FR24_CMD_SLEEP_CYCLES '0' // comando para trocar o tempo de sleep
#define FR24_CMD_LED1	'1'
#define FR24_CMD_LED2	'2'
#define FR24_CMD_LED3	'3'
#define FR24_CMD_VCC_SUPPLY	'V'
#define FR24_CMD_RELE	'R'	// relé
#define FR24_CMD_REBOOT	'X'	// reboot
#define FR24_LED_OFF '4' // ativa funcionamento dos leds
#define FR24_LED_ON '5'
#define FR24_REMOVE_NODE 'D' // par1 = nro do node
#define FR24_RESET_FIRMWARE 'F' // reset de firmware
#define FR24_LIMITES 'L'
#define FR24_OUTPUT 'O'   // output PIN, <0=low, 1=high, 2=pisca>, vezes, freq

#if HAS_NRF24L01 == 1	
#define MAX_NRF24_PACKAGE 7	
#else
#define MAX_NRF24_PACKAGE 13  // com 3 fases
#endif
struct TPackage {
	uint8_t addr;  // endereco do remetente
	uint8_t cmd;
	#if HAS_NRF24L01 == 1
	uint8_t seq;
	#endif
	uint8_t modelo; 
	uint16_t serie;
	uint8_t data[MAX_NRF24_PACKAGE];
};


#define SSID_LEN 30
#define PASSWD_LEN 30
#define TOKEN_TS_LEN 17

#if HAS_ETH ==1
#include "IPAddress.h"
#endif

struct TPWeb {
	char ip[2][16];
	char mac[13];
	// PROXY
	char http_proxy[16];
	uint16_t http_port;
	#if HAS_ESP8266 == 1
	// CLIENTE
	char ssid[SSID_LEN];
	char passwd[PASSWD_LEN];
	char ssid_ap[SSID_LEN];
	char passwd_ap[PASSWD_LEN];
	uint8_t canal_ap;  // 255 (0xFF) sem configuracao
	uint8_t cripto_ap; // 0-OPEN 2 WPA_PSK 3-WPA2_PSK 4-WPA_WPA2_PSK
	char rssi[5]; // sinal do wifi cliente
	#endif
	#if HAS_ETH ==1
	// IP,GW,MASK
	 IPAddress eth,gw,mask,dns;
	#endif
	// AP
	#if HAS_THINGSPEAK == 1 || HAS_TS_SERVER == 1
	char server_addr[sizeof(TSSERVER_ADDR_IP)+1]; // "184.106.153.149"
	uint16_t server_port;
//	unsigned long updateDadosFeed;
	char apy_key[TOKEN_TS_LEN];
	uint8_t field_corrente_ts;
	#endif
};

#if HAS_MODBUS == 1
#define MAX_MODBUS_SLAVE 1
#define MAX_MODBUS_ADDR 5
struct TPModbus {
	uint8_t slave;
	uint8_t tipo;
	uint8_t comando;
	uint16_t addr[MAX_MODBUS_ADDR];
};
#endif

#define P_STATUS_RESET 0
#define P_STATUS_RESET_FIRMWARE 1
#define P_STATUS_RESET_ESP8266 2
#define P_STATUS_GRAVA_EEPROM 3

#define P_STATUS_BITS_LIMPA 0b01111

#define P_STATUS_RESET_FABRICA 4
#define P_STATUS_WIFI_ERROR 5
//#define P_STATUS_SEND_THINGSPEAK 6
#define P_STATUS_SETUP_AP 7
//#define P_STATUS_MODO_TESTE 8
#define P_STATUS_SEND_EEPROM_LOG 9
#define P_STATUS_NO_AUTH 10
#define P_STATUS_USE_DHCP 11

#if HAS_IR_REMOTE == 1
struct TPIR {
	uint8_t khz;   // 38 Mhz
	uint8_t nro_bits; // numero de bits a enviar
	uint8_t repetition; // numero de repeticoes do comando
	uint8_t repetition_interval; // intervalo em us entre as repetições
	uint16_t start_pulse, start_pause; // inicio da comunicação
	uint16_t pulse_len, pause_high, pause_low; // high=1  low=1
	uint8_t cmd[2][6]; // 2 comandos;
};
#endif

struct TParametros {
	uint16_t status;
	uint16_t id;   // sequencia de pacotes...dados
	char chave[4];
	#if HAS_SENSOR_ANALOGICO == 1
	uint8_t porta_sensor_analogico[MAX_SENSOR_ANALOGICO];
	#endif
	struct TPRede rede;
	struct TPWeb web;
	#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
	struct TPnrf24 radio;
	#endif
	#if HAS_IR_REMOTE == 1
	struct TPIR ir;
	#endif
	#if HAS_MODBUS == 1
	struct TPModbus mb[MAX_MODBUS_SLAVE];
	#endif
	#if HAS_SENSOR_SECO == 1
	struct TSensor_Seco sensor_seco;
	#endif
};

#if HAS_TEMPERATURA == 1
struct TDS20B18 {
	byte addr[8];
	uint8_t porta;
	OneWire *ds;
	struct TDS20B18 *p;
	int temperatura;
};
#endif


//
//  ccStatus
#define CC_CIPMUX 0  // wifi
#define CC_LINK_GSM 0 // GSM
#define CC_LINK_ETH 0 // cabo
#define CC_CIPSERVER 1 // wifi
#define CC_INIT_ETH 1 // cabo
#define CC_WIFI_OK 2
#define CC_SETUP_ETH 2 // indica que já passou pela 
#define CC_WIFI_BITS_OK    0b0011  // sem o bit da internet
#define CC_WIFI_BITS_AP_OK 0b0111

#define CC_CWJAP 3 // Join Access Point
#define CC_WIFI_BITS_CWJAP_OK 0b01111
#define CC_WIFI_INTERNET 4
#define CC_HAS_SSID 5
#define CC_HAS_DHCP 5
#define CC_RET_HTTP 6 // status de retorno do enviar feed...se retorno HTTP/1.0 200 (ok) 0-OK, 1-ERR
#define CC_BITS_WEB 0b1111111

#define CC_HTTP_200_OK 7
#define CC_TICK   8
#define CC_TICK_2 9
#define CC_CORRENTE 10
#define CC_CLOCK_OK	11
#define CC_DEBUG 12
#define CC_BOOT_RESET 13
#define CC_BOOT 14
#define CC_MATA_BUFFER 15

// uint16_t flagHTTP;
#define F_VAR 0
#define F_HAS_RETURN 1
#define F_LOG_SYNC_EVENTOS 2
#define F_SERIAL_RECEIVED 3
#define F_HAS_CMD 4
#define F_RET_STATUS 5 // retorna somente STATUS
#define F_RET_LER_IP 6 // retorna ip[]
#define F_LOG_SYNC 7 // erro na sincronia do log, tem mais na eeprom do que foi enviado
#define F_SETUP_WIFI 8 // configurou wifi via browser

#define F_RFID_RECEIVED 15



#if HAS_EEPROM == 1
enum TLISTA_LOG  {LISTA_LOG_AT24,LISTA_LOG_WEB, LISTA_LOG_JSON, LISTA_LOG_PRINT};

#if USE_AT24 == 1
#define MAX_EEPROM_MEMORY (1024-4)
#define I24DATALOGGER_ADDR   1020
#define F24DATALOGGER_ADDR   1022
#define MAX_AT24_EEPROM_MEMORY 4096
#else
#define MAX_EEPROM_MEMORY 1024
#endif

#define BASE_ADDR 1
#define GPARAM_ADDR (offsetof(struct TParametros, status)+BASE_ADDR)
#define ID_DATALOGGER_ADDR (offsetof(struct TParametros, id)+BASE_ADDR)
#define IDATALOGGER_ADDR		(GPARAM_ADDR+sizeof(gParam))
#define FDATALOGGER_ADDR		(IDATALOGGER_ADDR + 2)
#define LAST_ADDR_MEMORY		(FDATALOGGER_ADDR + 2)
#define TOTAL_EEPROM_FREE (MAX_EEPROM_MEMORY - LAST_ADDR_MEMORY)

#define DATALOGGER_ADDR LAST_ADDR_MEMORY

#endif

// LEDS
#define LED_APAGA 0
#define LED_LIGAR 0xFF
/*
#define LED_PISCA_FLASH 0b010    // bit 2 (nro=04)
#define LED_PISCA_CURTO 0b011000   // bit 5  
#define LED_PISCA_LONGO 0b01100000  // nro=96
*/

#define LED_PISCA_FLASH 1
#define LED_PISCA_CURTO 10
#define LED_PISCA_LONGO 30

// COMANDOS
#define FR24_CMD_SLEEP_CYCLES '0' // comando para trocar o tempo de sleep
#define FR24_CMD_LED1	'1'
#define FR24_CMD_LED2	'2'
#define FR24_CMD_LED3	'3'
#define FR24_CMD_VCC_SUPPLY	'V'
#define FR24_CMD_RELE	'R'	// relé
#define FR24_CMD_REBOOT	'X'	// reboot
#define FR24_LED_OFF '4' // ativa funcionamento dos leds
#define FR24_LED_ON '5'

#define FR24_IR_SETUP	'I'
#define FR24_SEND_IR_CMD1 'a'
#define FR24_SEND_IR_CMD2 'b'

#define MAX_NRO_SMS 15
#define MAX_MSG_SMS 50

struct TSMS {
	uint8_t status;
	char chave[4];
	char numero[MAX_NRO_SMS+1]; // 55519911223344
	char msg[MAX_MSG_SMS+1];
	};

extern volatile uint8_t led1_status;
extern volatile uint8_t led2_status;

extern float calcula_Y(float);

///////////////////////////////
// wiring.c
/////////////////////////////
extern volatile unsigned long timer0_millis;
extern volatile unsigned long timer0_overflow_count;
///////////////////////////////
// TSensor.ino
/////////////////////////////
enum TupdateDadosFeed { TUS_BUSTA_CONFIG=0, TUS_ENVIA_DADOS, TUS_ENVIA_LOG, TUS_ENVIA_LOG_AT24};
extern int updateDadosFeed(uint8_t,uint32_t);
extern uint16_t ccStatus;
extern struct TParametros gParam;
extern struct TDataLogger gDataLogger;
extern uint16_t wifi_len;
extern uint8_t timerWeb;
extern char* urlencode(char* ,char*);
extern uint8_t countCorrente;
extern void pisca(uint8_t,uint8_t,uint16_t);
#if HAS_SENSOR_ANALOGICO == 1
extern uint16_t sensor_analogico[];
#endif
#if HAS_SENSOR_PORTA_9==1 || HAS_SENSOR_PORTA_10==1
extern volatile uint8_t sensor_9_10;
#endif
extern int htoi (char );
extern void loop_serial(void);
////////////////////////////////
// CORRENTE
///////////////////////////////
#if HAS_CORRENTE == 1
extern void setup_corrente(void);
extern void parse_corrente(void);
extern uint16_t lerCorrenteGerada(void);
#endif
extern uint16_t correnteFase[];

///////////////////////////////////
// Timer_Interrupts
///////////////////////////////////
extern long readVcc();
#if HAS_TIMER_INT == 1
#if HAS_TEMPERATURA == 1
extern void readTemperature(void);
extern void setup_Temperature(void);

extern struct TDS20B18 *pTemp;
//extern void Temp_create(uint8_t sens, byte addr[8]);
extern void Temp_create(uint8_t addr[], OneWire *dsx, uint8_t porta);
#endif
#if HAS_HUMIDITY == 1
int readHumidity(void);
#endif
extern uint16_t humidade;

extern void setup_timer();
extern volatile uint16_t timer1;
extern volatile uint16_t timer2;

#endif
///////////////////////////////
// EEPROM.C
//////////////////////////////
#if HAS_EEPROM == 1
extern boolean setup_eeprom(void);
extern boolean write_gParam(void);
extern uint8_t setup_boot_eeprom(void);
#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
extern uint16_t iDataLoggerLog;
extern uint16_t fDataLoggerLog;
extern uint16_t maxDataLoggerLog;
extern uint8_t list_corrente_log(uint8_t, int, char *);
extern uint8_t grava_corrente_log(uint32_t);
#endif
#endif
#if USE_AT24 == 1
#include <AT24Cx-master/AT24CX.h>
extern void setup_at24();
extern AT24C32 *memAT24;
#endif
///////////////////////////////////
#if HAS_BOTAO == 1
#define BTN_PRESS_CURTO 1
#define BTN_PRESS_LONGO 2
extern void setup_botao(void);
extern int parse_botao(void);
extern volatile uint8_t button1;
#endif
/////////////////////////////
// ESP8266
////////////////////////////////
extern void sendHTMLPrincipalDados(uint8_t, char);
extern void setup_ESP8266(uint8_t);
extern void parse_ESP8266();
extern void ESP8266reset(uint8_t);
extern void esp8266HTTPreq(void);
extern uint8_t sendData(char *, const int, char*);
extern char reqHTTP[];
extern int recvPkg(uint8_t,uint8_t *, uint16_t , uint32_t , uint8_t *);
extern boolean sATCIPSTARTMultiple(uint8_t mux_id);
extern boolean sATCIPCLOSEMulitple(uint8_t mux_id); 
#define POINTER_CHAR_RAM 0
#define POINTER_CHAR_PROGMEM 1
extern boolean sATCIPSENDMultiple(int, uint8_t, uint8_t *, boolean);
#define getStatus eATCIPSTATUS
extern char eATCIPSTATUS(void);
extern void eATCIFSR(void);
#define getCWMODE eATCWMODE(0)
#define getCIPMUX eATCWMODE(1)
#define getCWLIF eATCWMODE(2)
extern char eATCWMODE(uint8_t);
extern void eATCWSAP(void);
extern void eATCWLAP(void);
extern void eATCWSAP_DEF(char *);
#if HAS_ETH == 1
extern void stoip(IPAddress &,char*);
extern void iptos(IPAddress ip, char* p);
#endif
extern void urldecode2(char *, const char *);

extern int testar_conexao_wifi(void);
extern void esp8266Flush(void);
extern char * recvString(char *, char *, char *, char *, uint32_t, boolean);
extern boolean recvFind(char *target, uint32_t timeout);
extern const char str_Consumo[] PROGMEM;
extern const char str_TS_Connection[] PROGMEM;
extern const char str_TS_Content_type_JSON[] PROGMEM;
extern const char str_Content_length[] PROGMEM;
extern const char str_HTTP_200[] PROGMEM;
extern const char str_Content_type[] PROGMEM;
extern const char str_Content_type_text[] PROGMEM;
extern uint16_t flagHTTP;
extern char connectCodeWifi;
//////////////////////////////////////////////
// ETH
//////////////////////////////////////////
#if HAS_ETH == 1
#include <UIPEthernet.h>
extern void setup_eth(void);
extern void parse_eth(void);
extern EthernetClient client;
extern EthernetServer server;
#endif
////////////////////////////////////////////////
// GSM
///////////////////////////////////////////////
extern void setup_gsm(uint8_t);
#if HAS_GSM == 1
extern int sATCIPSENDACTION(int);
extern int sHTTPREAD(void);
extern CGPRS_SIM800 gprs;
extern GSM_LOCATION loc;
#define MAX_LOC_COUNT 10
extern uint8_t loc_count; // a cada 10 envios, pega localização

extern void parse_gsm(void);
#if USE_ETH_SMS == 1
extern struct TSMS gSMS;
#endif
//extern GSMClient client;
//extern GPRS gprs;
//extern GSM gsmAccess;
#endif
//////////////////////////////////////////
// I2C
///////////////////////////////////////////
extern void setup_i2c(void);
extern uint8_t parse_i2c(void);
extern void reenvia_i2c_config(void);
/////////////////////////////////////////////
// SENSOR_SECO
/////////////////////////////////////////////
#if HAS_SENSOR_SECO == 1
extern void setup_seco(uint8_t);
extern uint8_t parse_seco(uint8_t);
extern struct TSensor_Seco sensor_seco;
extern void process_TI(char*);
extern void process_TG(char*);
extern volatile uint8_t seco_int;
#endif
extern uint16_t parse_seco_analogico(uint8_t porta, uint32_t espera);

///////////////////////////////////////////
//  EVENTOS
///////////////////////////////////////////
#if USE_LOG_EVENTOS == 1
extern void setup_eventos(void);
extern uint8_t parse_eventos(void);
extern void limpa_eventos(uint8_t);
extern uint8_t grava_eventos_log(TEvento_tipo,uint8_t, uint8_t,uint32_t);
extern struct TDEvento eventos[];
extern uint8_t eventos_feed; // indica o indice do alarme acima, em bits
#endif
///////////////////////////////////////////
//  MODBUS
///////////////////////////////////////////
extern void setup_modbus(void);
extern uint8_t parse_modbus(void);
extern void process_TU(char*);

//////////////////////////////////////////
// NRF24L01
///////////////////////////////////////////
extern void setup_nrf24(void);
extern boolean setNodeAddr(void);
extern uint8_t parse_nrf24(void);
/////////////////////////////////////////
// REAL_CLOCK
///////////////////////////////
#if HAS_CLOCK == 1
extern void setup_clock(void);
extern void process_date(char*);
extern char *formatHora(char *, time_t);
extern void digitalClockDisplay();
#endif
///////////////////////////////////////
// IR_REMOTE
//////////////////////////////////////
#if HAS_IR_REMOTE == 1
extern void send_ir_command(uint8_t);
extern struct TPIR gIR;
#endif
///////////////////////////////////////
// DISPLAY
//////////////////////////////////////
#if HAS_DISPLAY == 1
#include "LiquidCrystal_I2C.h"
extern void setup_display(void);
extern LiquidCrystal_I2C lcd;
#endif

#if DEBUG >= 1 
#define debug(x)	SSdebug.print(x);
#define debugln(x)  SSdebug.println(x);

#define debugbin(x)	SSdebug.print(x,BIN);
#define debughex(x)	SSdebug.print(x,HEX);
#define debugbinln(x)	SSdebug.println(x,BIN);
#define debughexln(x)	SSdebug.println(x,HEX);

#define debugvar(x)	   SSdebug.print(F(#x)); SSdebug.print("=["); SSdebug.print(x); SSdebug.print("] ");
#define debugvarln(x)  SSdebug.print(F(#x)); SSdebug.print("=["); SSdebug.print(x); SSdebug.print("]\n");

//#define PGM_PRINT(a) {	const char *p0=PSTR(a); while (pgm_read_byte(p0) != '\0') {SSdebug.write(pgm_read_byte(p0++)); } SSdebug.write(' ');}
//#define PGM_PRINTLN(a){ const char *p0=PSTR(a); while (pgm_read_byte(p0) != '\0') {SSdebug.write(pgm_read_byte(p0++)); } SSdebug.write('\n') ;}
#define PGM_PRINT(a) SSdebug.print(F(a));
#define PGM_PRINTLN(a) SSdebug.println(F(a));
#define PGM_PRINTP(a) {	const char *p0=a; while (pgm_read_byte(p0) != '\0') {SSdebug.write(pgm_read_byte(p0++)); }}
#define PGM_PRINTPLN(a) {	const char *p0=a; while (pgm_read_byte(p0) != '\0') {SSdebug.write(pgm_read_byte(p0++)); } SSdebug.write('\n') ;}

#else // DEBUG == 1
#define debug(x)
#define debughex(x)
#define debugln(x)
#define debugbin(x)
#define debughexln(x)
#define debugbinln(x)
#define debugvar(x)
#define debugvarln(x)
#define PGM_PRINT(a)
#define PGM_PRINTLN(a)
#define PGM_PRINTP(a)
#define PGM_PRINTPLN(a)
#endif
/*
 * Botao
 *   Boot:
 *     pressionado: reset de fábrica (firmware)
 *   Normal:
 *
 * LEDS: verde, amarelo, vermelho
 * boot
 *  VM: ON VD:curto configurando wifi (boot)
 *  VM: curto VD:on conectou ao roteador (boot)
 *  VM: off VD: flash sem conexão com roteador (nao configurado) (boot)
 * operação normal
 *  VM: off  VD:off conectado roteador, sem tx e rx

 *  VM: ON VD:off   erro wifi
 *  VM:curto VD:off conectou ao AP
 *  VM: -   VD:curto conexão de browser local
 *  VM: -   VD:flash conexão servidor (enviando dados) e apaga
 *  VM: flash VD: flash sem configuração do servidor (API_KEY) TAG_1
 *  VM: longo VD: off  sem conexão com o servidor/roteador  TAG_2
 */
#endif

// DONE: 1. config fases da rede OK
// DONE: 2. config parametros do wifi OK
// DONE: 3. boot da EEPROM OK
// DONE: 4. configuração das fases :0=auto 1=001, 2=011, 3 111
// DONE: 5. erro no status de retorno K (TS:) OK
// DONE: 6. Limpar P_STATUS_SEND_TWITTER/HAS_TWITTER/USE_TWITTER OK
// DONE: 7. server: configuração dos limites, nova tabela OK
// DONE: 8. cliente nrf24l01 ok
// DONE: 9. frequencia da leitura e do envio por qtde ok
// DONE: 10. carga dos gráficos quando por browser
// DONE: 11. server: enviar twiter/push NOP
// DONE: 12: server: configuração de email e envio OK
// DONE: 13: modo sleep OK
// DONE: 14: enviar tensao da bateria (alerta) - enviar como sensor OK
// DONE: 15: enviar alerta de nao recebimento de mensagem (10min?), e no retorno (job)
// DONE: 16: configurar recursos, escravo enviar no setup OK
// DONE: 17: configurar setup com 2 mestres
// DONE: 18: app: setup limites, email, sms ok
// DONE: 19: app: ativar comandos remotos principal e nós
// DONE: 20: aumentar limite de 3 nós.
// DONE: 21: sensores 9 e 10 escravo e principal OK
// DONE: 22: comandos do relé
// DONE: 23: sensores SCL/SDA A4 e A5 (ultrasom?)
// DONE: 24: TS: webservice /o/ ok
// DONE: 25: app: passar 4 bytes da chave ok
// DONE: 26: app: desabilitar botão barcode ok
// DONE: 27: app: reload dos dados Modulo
// DONE: 28: gravar a chave na nvram e parâmetro para alterar pela WEB (26/05/2016)
// TODO: 29: setup_boot
// TODO: 30: alterar IP do servidor sem apagar