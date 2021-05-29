/**
 * @file TSensor_config.h
 * @author CAP
 * @date 1 Dez 2015
 */
#ifndef TSENSOR_CONFIG
#define TSENSOR_CONFIG

#define TSENSOR_MODELO 0 //1   ///< modelo do TSensor
#define TSENSOR_SERIE 26 //16//48 //42 //1   ///< numero de serie

//***************************************
// NAO ESQUECER: hardwareSerial.h
//
// versao 1.6.9
// #define SERIAL_TX_BUFFER_SIZE 64
// #define SERIAL_RX_BUFFER_SIZE 180
//
// softwareSerial.h
// #define _SS_MAX_RX_BUFF 32
//
// softwareSerial.cpp
// PCINT2 D0,D1,D2,D3,D4,D5,D6,D7
//#if defined(PCINT2_vect)
//ISR(PCINT2_vect)
//{
//	SoftwareSerial::handle_interrupt();
//}
//#endif

//#if 0 // CAP
//#if defined(PCINT1_vect)
//ISR(PCINT1_vect, ISR_ALIASOF(PCINT2_vect));
//#endif

// PCINT0 D8,D9,D10,D11,D12,D13
//#if defined(PCINT0_vect)
//ISR(PCINT0_vect, ISR_ALIASOF(PCINT2_vect));
//#endif
//#endif   // CAP
//

#define xstr(a)   str(a)
#define str(a)   #a

#define VERSION_MAJOR	0
#define VERSION_MINOR	50

#define VERSION xstr(TSENSOR_SERIE) "." xstr(VERSION_MAJOR) "." xstr(VERSION_MINOR)

#ifdef ARDUINO_AVR_ATmega1284
#define __AVR_ATmega1284__
#endif

#define HOME 0
#define GVT 0
#define IPHONE 0
#define WORK 0
#define HSLCDI 0
#define FABRICA 0
#define SERVIDORES 0
#define JME 0
#define AUTOGERADORA 0

#ifdef _DEBUG
#define DEBUG _DEBUG
#else
#define DEBUG 10
#endif

#define HAS_ESP8266 1
#define HAS_NRF24L01 0
#define TINY_VERSION 0
#define TESTE_RELE 0
#define SERV_ADDR_1	'S'
#define USE_LEDS 1
#define USE_SSL 0

//#define HAS_WEBSERVER_FULL 1
//#define HAS_WEBSERVER_TINY 0

#define DEMO 1
#define PERMITE_TROCAR_SERVER 1
#define PERMITE_TROCAR_AP 1
#define PERMITE_TROCAR_FASES 0
#define PERMITE_DOWNLOAD 0

#define USE_RESET_FIRMWARE_ON_BOOT 0

#if DEBUG == 1  && !defined(DEMO)
#define DEMO 1
#endif

//#define bit(b) (1UL << (b))
//  RECURSOS
#define REC_CORRENTE_30A 0
#define REC_CORRENTE_100A 1
// temperatura
// 000 sem sensor temp
// 001 LM35
// 010 DHT11
// 011 DS18B20
// 100 DS18B20 segundo sensor
#define REC_LM35          0b000100
#define REC_DHT11         0b001000
#define REC_DS18B20       0b001100
#define REC_DS18B20_EXTRA 0b010000
#define REC_DS18B20_EXTRA_2 5
#define REC_LED1 6
#define REC_LED2 7
#define REC_LED3 8
#define REC_BOTAO 9
#define REC_ALIMENTACAO 10  ///< alimentação da rede
#define REC_SENSOR_ANALOGICO 11 ///< luminosidade, fluxo de água, fumaca
#define REC_SENSOR_9 12 ///< PIR, sensor de contato, digital 0 ou 1
#define REC_SENSOR_10 13 ///< PIR, sensor de contato, digital 0 ou 1
#define REC_HUMIDADE 14
#define REC_RELE 15
#define REC_SENSORES bit(REC_RELE)|bit(REC_SENSOR_9)|bit(REC_SENSOR_10)
#define REC_WIFI 16
#define REC_ETHERNET 17
#define REC_GSM (bit(16) | bit(17))
#define REC_MODBUS 18 
#define REC_NRF24L01 19
#define REC_SENSOR_SECO 20

// WIFI
#if HOME == 1
//#define SSID        "SALA"
#define SSID        "HOME2G"
#define PASSWORD    "123456789a"
#define PROXY_HTTP	""
#define PROXY_PORT	0
#elif GVT == 1
#define SSID        "GVT-EE72"
#define PASSWORD    "123456789a"
#define PROXY_HTTP	""
#define PROXY_PORT	0
#elif WORK == 1
#define SSID        "hslpucrs"
#define PASSWORD    "hslgestaum"
#define SSID        "SUPORTE"
#define PASSWORD    "@123456789&"
//#define PROXY_HTTP	"10.40.49.80"
#define PROXY_HTTP	"201.54.136.25"
#define PROXY_PORT	3128
#elif IPHONE == 1
#define SSID        "iPhone6"
#define PASSWORD    "123456789a"
#define PROXY_HTTP	""
#define PROXY_PORT	0
#elif SERVIDORES == 1
#define SSID        "FINANCEIRO"
#define PASSWORD    "@FARMACIA&"
#define PROXY_HTTP	"201.54.136.25"
#define PROXY_PORT	3128
#elif HSLCDI == 1
#define SSID        "SENSOR"
#define PASSWORD    "@sensor&"
#define PROXY_HTTP	"201.54.136.25"
#define PROXY_PORT	3128
#elif JME == 1
#define SSID        "jme"
#define PASSWORD    "3864619386"
#define PROXY_HTTP	""
#define PROXY_PORT	0
#elif AUTOGERADORA == 1
#define SSID        "Autogeradora"
#define PASSWORD    "aregotua"
#define PROXY_HTTP	""
#define PROXY_PORT	0
#elif LACAVA == 1
#define SSID        "LACAVA"
#define PASSWORD    "1000100010"
#define PROXY_HTTP	""
#define PROXY_PORT	0
#else
#define SSID        ""
#define PASSWORD    ""
#define PROXY_HTTP	""
#define PROXY_PORT	0
#endif

#define SERVER_PORT	80


// THINGSPEAK
#define THINGSPEAK_ADDR "api.thingspeak.com"
//#define THINGSPEAK_ADDR "184.106.153.149"
//#define THINGSPEAK_ADDR_IP "184.106.153.149"
//#define TSSERVER_ADDR_IP "ts0.sensoronline.net"

#define TSSERVER_ADDR_IP "45.55.77.192"		// DO
//#define TSSERVER_ADDR_IP "52.35.8.102"   // AWS

#define HOST_PORT   80


#if TSENSOR_SERIE == 0 // somente para compilacao
//#define WRITEAPIKEY_THINGSPEAK "RBWZQFSWYMK5MPV7"
//#define CANAL_TS "34833"
#define SSID_AP ""
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,2
#define MD5 "e8ee" // e8ee98ac0a157359fac10c53699b3da6
//#define TSSERVER_ADDR_IP "52.35.8.102"   // AWS

#if 1
#define TEMPO_COLETA_CORRENTE 10
#define HAS_CORRENTE 1
#define HAS_DHT11 1
#define HAS_HUMIDITY 1
#define HAS_DS18B20 1
#define CALIBRAGEM_CORRENTE 0xb100  // A2=100A   e A1,A0=30A   60.6 -> 100A   30.0 -> 30A
#define B_TEMPERATURA bit(B_PORTA_A0)  | 0b0110 // porta A0
#define HAS_SENSOR_SECO 1
#define HAS_SENSOR_ANALOGICO 1
#define HAS_BOTAO 1
#define HAS_RELE 0
#define HAS_EVENTOS 0
#define HAS_BLUETOOTH 5
#define USE_LOG_EVENTOS 0
#define USE_LOG_EEPROM 1
#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) | REC_WIFI | REC_DHT11|bit(REC_SENSOR_ANALOGICO)| bit(REC_LED1)| bit(REC_LED3) | \
	bit(REC_CORRENTE_30A) |  (REC_BOTAO)|bit(REC_ALIMENTACAO))

#else
#define TEMPO_COLETA_CORRENTE 10
#define HAS_CORRENTE 0
#define CALIBRAGEM_CORRENTE 0xb0100  // A2=100A   e A1,A0=30A   60.6 -> 100A   30.0 -> 30A
//#define HAS_DHT11 1
//#define HAS_HUMIDITY 1
#define HAS_DS18B20 1
#define B_TEMPERATURA bit(B_PORTA_A0)   // porta A2
//#define HAS_SENSOR_ANALOGICO 1
#define HAS_SENSOR_SECO 0
#define USE_SENSOR_SECO_PORTA_BAIXA 0
//#define HAS_BOTAO 1
#define HAS_RELE 1
#define HAS_RFID 1
#define HAS_EVENTOS 0
#define HAS_BLUETOOTH 2
#define USE_LOG_EVENTOS 0
#define USE_LOG_EEPROM 1
#define HAS_WEBSERVER_FULL 1
#define HAS_WEBSERVER_TINY 0
#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) | REC_WIFI | REC_DHT11|bit(REC_SENSOR_ANALOGICO)| \
		bit(REC_CORRENTE_100A) | bit(REC_LED1)| bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif
#endif


#if TSENSOR_SERIE == 1
#define WRITEAPIKEY_THINGSPEAK "GZ1MHC0IF7R3J4S2"  // TESTE
#define MD5 "5fa4" //c32b4c7c356a6420efdfc8280eba"
// CHAVE VDUTX85M5VYH6Q6U
//#define CANAL_TS "26573"
#endif

#if TSENSOR_SERIE == 2 // protótipo placa
//#define WRITEAPIKEY_THINGSPEAK "RBWZQFSWYMK5MPV7"
//#define CANAL_TS "34833"
#define SSID_AP "ESP_9D24FE"
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,2
#define MD5 "3f83" ///ff96d1fea4b92e27c1b4449b1aa0"
#define HAS_LM35 1
#define HAS_CORRENTE 1
#define HAS_RELE 0
#define TSENSOR_RECURSO (REC_CORRENTE_30A | REC_LM35 |bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 3 // TS-3  0.3
#define SSID_AP "ESP_FCA0A5"
#define CANAL_TS "50188"
// 45.55.77.192,TS0,3
#define MD5 "9213" // 15ca22f3dd0097619ad10cea1a83" 
#define HAS_DS18B20 1
//#define HAS_DHT11 1
//#define HAS_HUMIDITY 1
//#define HAS_MODBUS 1 /// ADDR 56
#define HAS_ESP8266 1
#define HAS_CORRENTE 1
#define MAX_CORRENTE_FASES 3
#define MAX_TSENSOR_CLIENT 1
#define MAX_TSENSOR_SENSOR 5
#define HAS_CLOCK 1
#define MODBUS_ENABLE_PIN 10 // LED_AMARELO
#define HAS_BOTAO 1
#define TSENSOR_RECURSO  ( bit(REC_CORRENTE_30A) |REC_DS18B20| bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 4 // TS-4  v0.40
//#define WRITEAPIKEY_THINGSPEAK "FSBNUY8UWGNGTPAY"
//#define CANAL_TS "53093"
#define SSID_AP "ESP_9D88F5" 
//#define CANAL_TS "53093"
// 45.55.77.192,TS0,4
#define MD5 "5ae6" // "07ae6aa698d365a8e4c50919e324"
#define HAS_LM35 1
#define HAS_DS18B20_EXTRA 1
#define TEMPERATURA_AMBIENTE_PIN_2 A2
//#define HAS_DS18B20 1
//#define TEMPERATURA_AMBIENTE_PIN A2
#define HAS_CORRENTE 1
#define MAX_CORRENTE_FASES 2
#define HAS_BOTAO 1
#define HAS_BLUETOOTH 5
#define HAS_WEBSERVER_FULL 1
#define HAS_WEBSERVER_TINY 0
#define USE_LOG_EEPROM 0
#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_DS18B20|bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
//#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_LM35 |REC_DS18B20_EXTRA|bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 5 // TS-5  0.5
//#define WRITEAPIKEY_THINGSPEAK "FSBNUY8UWGNGTPAY"
//#define SSID_AP "ESP_F92A0B"  queimou
#define SSID_AP "ESP_8BB349"
//#define CANAL_TS "54809"
//#define SSID_AP "ESP_9D8CCC" // (AT 0.5, SDK 1.4)
#define HAS_GSM_TCP 1
// 45.55.77.192,TS0,5
#define MD5 "8ca4" //8b1422545afa89b16915dcd76ce6"
#define MAX_TSENSOR_SENSOR 5
#define HAS_CORRENTE 1
#define MAX_CORRENTE_FASES 3
#define MAX_TSENSOR_CLIENT 1
#define MAX_TSENSOR_SENSOR 5
#define CALIBRAGEM_CORRENTE 30.0  // 60.6,  // 100A
#define HAS_RELE 1
#define HAS_LM35 1
#define HAS_BOTAO 1
//#define HAS_SENSOR_ANALOGICO 1
#define HAS_CLOCK 1
#define HAS_SENSOR_SECO 1
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 0

//#define HAS_MODBUS 1 /// ADDR 56
#define MODBUS_ENABLE_PIN 10 // LED_AMARELO
#define TSENSOR_RECURSO  (bit(REC_CORRENTE_30A) |REC_LM35| bit(REC_RELE) | bit(REC_HUMIDADE)| bit(REC_LED1) |bit(REC_SENSOR_9) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))

//#define TSENSOR_RECURSO (_BV(REC_CORRENTE_30A) | REC_LM35 | bit(REC_SENSOR_ANALOGICO) | bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) |bit(REC_RELE)| bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 6 // TS-6  0.6
#define SSID_AP "ESP_9D311F"
#define CANAL_TS "10007"
// 45.55.77.192,TS0,6
#define MD5 "a019" //a019063e2143175fbde4b895551db5d5
#define HAS_DS18B20 1
#define HAS_DS18B20_EXTRA 1
#define HAS_DS18B20_EXTRA_2 1
//#define HAS_SENSOR_ANALOGICO 1
#define TEMPERATURA_AMBIENTE_3_PIN A0
//#define HAS_BLUETOOTH 2
//#define MULTIPLE_DS18B20 0
//#define TEMPERATURA_AMBIENTE_PIN A0
#define TSENSOR_RECURSO (bit(REC_DS18B20_EXTRA_2) | REC_DS18B20_EXTRA | REC_DS18B20 |bit(REC_LED1) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_BOTAO))
#endif

#if TSENSOR_SERIE == 7 // TS-7  0.7
//#define WRITEAPIKEY_THINGSPEAK "FSBNUY8UWGNGTPAY"
#define SSID_AP "ESP_9D8E59"  //SDK 1.3
//#define HAS_I2C 1
// 45.55.77.192,TS0,7
#define MD5 "d859" //d8597c3c026710c40651e50802bfb0e4
//#define CANAL_TS "10011"
#define HAS_DS18B20 1
//#define HAS_SENSOR_ANALOGICO 1
#define HAS_RELE 0
#define HAS_BOTAO 0
//#define HAS_LM35 0
#define TSENSOR_RECURSO (REC_DS18B20 |bit(REC_LED1) |bit(REC_LED3) | bit(REC_ALIMENTACAO) )
#define LED 9 //3 //13
#define LED_PORT PORTB1 //PORTD3 //5
#define LED_ON PORTB |= bit(LED_PORT)
#define LED_OFF PORTB &= ~bit(LED_PORT)
#define LED_READ (PINB & bit(LED_PORT))
// LED_VERMELHO
#define LED_VERMELHO 9
#define LEDV_PORT PORTD3
#define LEDV_ON PORTD |= bit(LEDV_PORT)
#define LEDV_OFF PORTD &= ~bit(LEDV_PORT)
#define LEDV_READ (PIND & bit(LEDV_PORT))
#endif

#if TSENSOR_SERIE == 8 // TS-8
#define SSID_AP "ESP_8BB349"
#define MD5 "57a6" //57a63330f44564adf5a6d915ae3ce406
//#define "10012"
#define TEMPO_COLETA_CORRENTE 20
#define HAS_CORRENTE 0
#define HAS_DHT11 1
#define HAS_HUMIDITY 1
#define HAS_SENSOR_SECO 1
#define HAS_SENSOR_ANALOGICO 1
#define HAS_BOTAO 1
#define HAS_RELE 0
#define HAS_EVENTOS 0
#define HAS_BLUETOOTH 3
#define USE_LOG_EVENTOS 0
#define USE_LOG_EEPROM 0
#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) | REC_WIFI | REC_DHT11| bit(REC_HUMIDADE) |bit(REC_SENSOR_ANALOGICO)| bit(REC_LED1)| bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 9 // TS-9
#define SSID_AP "ESP_8BB23B"
//#define CANAL "10013"
#define MD5 "6ff0" //6ff0ac4533cc3294790776f294ebbdd4
#define HAS_CORRENTE 0
#define MAX_CORRENTE_FASES 3
#define HAS_ESP8266 0
#define HAS_GSM 1
#define HAS_GSM_TCP 0
#define HAS_DHT11 1
#define HAS_HUMIDITY 1
#define HAS_WEBSERVER_FULL 0
#define HAS_BLUETOOTH  5
#define HAS_MONITORAMENTO 1
#define HAS_EVENTOS 0
#define USE_LOG_EVENTOS 0
#define USE_LOG_EEPROM 0
#define USE_REAL_RTC 0
#define USE_AT24 0
#define HAS_CLOCK 1
#define HAS_SENSOR_SECO 1
#define HAS_SENSOR_ANALOGICO 1
#define HAS_BOTAO 1
#define HAS_RELE 1
#define CALIBRAGEM_CORRENTE 60.6  // 100A
//#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) | REC_GSM | bit(REC_RELE) | bit(REC_LED1)| bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) | REC_DHT11 | bit(REC_HUMIDADE)| bit(REC_WIFI) | bit(REC_LED1) | bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 10 // TS-10  CDI1
#define SSID_AP "ESP_8BB33C"
#define "10014"
#define MD5 "8fc6" //8fc6b4ac9bab6fdc81683ad57f5d6afe
#define HAS_DHT22 1
#define HAS_HUMIDITY 1
#define TSENSOR_RECURSO  (REC_DHT11 | bit(REC_HUMIDADE) | bit(REC_LED1) | bit(REC_LED3) | bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 11 // TS-11
#define SSID_AP "ESP_8BB7D3"
#define "10015"
#define MD5 "04ec" //04ec5a4da4f257927d876aec06bb96ac
#define HAS_DHT22 1
#define HAS_HUMIDITY 1
#define TSENSOR_RECURSO  (REC_DHT11 | bit(REC_HUMIDADE) | bit(REC_LED1) | bit(REC_LED3) | bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 12 // TS-12
#define SSID_AP "ESP_F92BA0"
#define MD5 "f16c" //f16cb64d63b248fda9eeeb672b8db82a
#define HAS_DHT22 1
#define HAS_HUMIDITY 1
#define TSENSOR_RECURSO  (REC_DHT11 | bit(REC_HUMIDADE) | bit(REC_LED1) | bit(REC_LED3) | bit(REC_ALIMENTACAO))
#endif


#if TSENSOR_SERIE == 13 // TS-13  GSM
//#define WRITEAPIKEY_THINGSPEAK "TI13DDY20K42DMVN"
//#define CANAL_TS "50188"
#define SSID_AP "ESP_nnnnnn"
#define CANAL_TS "10018"
// 45.55.77.192,TS0,13
#define MD5 "8cdf" // 8cdfcd534ae6706b6f63aa14f97ca7a9
#define HAS_ETH 1
#define TEMPO_COLETA_CORRENTE 5
#define HAS_CORRENTE 0
#define HAS_DHT11 1
#define HAS_HUMIDITY 1
#define HAS_SENSOR_ANALOGICO 1
#define HAS_SENSOR_SECO 1
#define USE_SENSOR_SECO_PORTA_BAIXA 1
#define HAS_BOTAO 0
#define HAS_RELE 0
#define USE_LEDS 1
#define HAS_EVENTOS 0
#define HAS_BLUETOOTH 0
#define USE_LOG_EVENTOS 0
#define USE_LOG_EEPROM 0
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 1
#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) | bit(REC_ETHERNET) | REC_DHT11|bit(REC_SENSOR_ANALOGICO)| \
	bit(REC_HUMIDADE) | bit(REC_LED1)| bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 14 // TS-14  atmega
//#define WRITEAPIKEY_THINGSPEAK "TI13DDY20K42DMVN"
//#define CANAL_TS "50188"
#define SSID_AP "ESP_nnnnnn"
#define CANAL_TS "XXXXX"
// 45.55.77.192,TS0,14
#define MD5 "4158" // 41587b9f618f261bdd8684e7a13ac43a
#define HAS_ETH 1
#define HAS_GSM 1
#define USE_ETH_SMS 1

#define HAS_I2C 0
#define HAS_BOTAO 0
#define HAS_EEPROM 1
#define USE_LOG_EEPROM 0
#define HAS_ESP8266 0
#define TINY_VERSION 1
#define USE_LEDS 1
#define HAS_CLOCK 1
#define HAS_DHT11 1
#define HAS_HUMIDITY 1
#define TEMPERATURA_AMBIENTE_PIN SCL
//#define HAS_LM35 1
//#define HAS_DS18B20 1
//#define HAS_SENSOR_ANALOGICO 1
#define HAS_CORRENTE 0
#define MAX_CORRENTE_FASES 1
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 1
#define HAS_BLUETOOTH 5
#define DEBUG 1
#define USE_DHCP  1
#define USE_LEDS 0
//#define PROXY_HTTP	"192.168.1.251"
//#define PROXY_PORT	3128

//#define DEBUG 0
//#define HAS_SENSOR_PORTA_10 1
//#define USE_RESET_FIRMWARE_ON_BOOT 1
#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_DHT11 |bit(HAS_HUMIDITY) | bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_ETHERNET))
//#define TSENSOR_RECURSO (bit(REC_ETHERNET) | bit(REC_CORRENTE_30A) | REC_DS18B20 | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 15 // atmega328p + ECN28J60
//#define CANAL_TS "10020"
// CHAVE LNZTRYFZMZ0KSNKY
#define SSID_AP "ESP_nnnnnn"
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,15
#define MD5 "aca5"  //aca5e41a0738f78ce72d1970164a4a17
#define HAS_ETH 1
#define HAS_I2C 0
#define HAS_BOTAO 1
#define HAS_EEPROM 1
#define USE_LOG_EEPROM 0
#define HAS_ESP8266 0
#define TINY_VERSION 1
#define USE_LEDS 1
#define HAS_DHT11 1
#define HAS_HUMIDITY 1
//#define HAS_LM35 1
//#define HAS_DS18B20 1
#define HAS_SENSOR_ANALOGICO 1
#define HAS_SENSOR_SECO 1
#define USE_SENSOR_SECO_PORTA_BAIXA 1
#define HAS_CORRENTE 0
#define MAX_CORRENTE_FASES 1
#define TEMPO_COLETA_CORRENTE 2
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 1
#define USE_DHCP  1
#define HAS_BLUETOOTH 1
#define DEBUG 0
//#define HAS_SENSOR_PORTA_10 1
//#define USE_RESET_FIRMWARE_ON_BOOT 1
#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) |bit(REC_SENSOR_ANALOGICO)|REC_DHT11 | bit(REC_HUMIDADE)| bit(REC_LED1) | bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_ETHERNET))
//#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_LM35|bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_WIFI) | bit(REC_MODBUS))
#endif

#if TSENSOR_SERIE == 16 // atmega328p + ECN28J60
//#define CANAL_TS "10022"
// CHAVE LNZTRYFZMZ0KSNKY
#define SSID_AP "ESP_nnnnnn"
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,16
#define MD5 "9e74"  //9e7482968c18ee829e5e476ae0240bfa
#define HAS_ETH 1
#define HAS_I2C 0
#define HAS_BOTAO 1
#define HAS_EEPROM 1
#define USE_LOG_EEPROM 0
#define HAS_ESP8266 0
#define TINY_VERSION 1
#define USE_LEDS 1
#define HAS_DHT22 1
#define HAS_HUMIDITY 1
//#define HAS_LM35 1
//#define HAS_DS18B20 1
//#define HAS_SENSOR_ANALOGICO 1
#define HAS_CORRENTE 0
#define MAX_CORRENTE_FASES 1
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 1
#define USE_DHCP  1
#define HAS_BLUETOOTH 1
#define DEBUG 0
#define TSENSOR_RECURSO (REC_DHT11 |bit(REC_HUMIDADE)|bit(REC_LED1) | bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_ETHERNET))
#endif

#if TSENSOR_SERIE == 17 // atmega328p + SIM800L
//#define CANAL_TS "10053"
// CHAVE LNZTRYFZMZ0KSNKY
#define SSID_AP "ESP_nnnnnn"
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,17
#define MD5 "f8ea"  //f8ea9aa4a1d0ed831aa71fd9e00d2ae1
#define HAS_GSM 1
#define HAS_I2C 0
#define HAS_BOTAO 1
#define HAS_EEPROM 1
#define HAS_CLOCK 1
#define USE_LOG_EEPROM 0
#define HAS_ESP8266 0
#define TINY_VERSION 1
#define USE_LEDS 0
//#define HAS_DHT11 1
//#define HAS_HUMIDITY 1
//#define HAS_DS18B20 1
//#define HAS_SENSOR_ANALOGICO 1
#define HAS_CORRENTE 0
#define MAX_CORRENTE_FASES 1
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 0
#define USE_DHCP  1
#define HAS_BLUETOOTH 2
#define DEBUG 1
#define USE_ETH_SMS 0
//#define HAS_SENSOR_PORTA_10 1
//#define USE_RESET_FIRMWARE_ON_BOOT 1
#define TSENSOR_RECURSO (REC_DS18B20 |REC_GSM | bit(REC_CORRENTE_30A)| bit(REC_LED1) | bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
//#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_LM35|bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_WIFI) | bit(REC_MODBUS))
#endif


#if TSENSOR_SERIE == 18 // atmega328p + ECN28J60
//#define CANAL_TS "10054"
// CHAVE LNZTRYFZMZ0KSNKY
#define SSID_AP "ESP_nnnnnn"
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,15
#define MD5 "41b9"  //41b977c4964024ea329bd580b9f413ee
#define HAS_ETH 1
#define HAS_GSM 0
#define USE_ETH_SMS 0
#define HAS_I2C 0
#define HAS_BOTAO 1
#define HAS_EEPROM 1
#define HAS_CLOCK 1
#define USE_LOG_EEPROM 0
#define HAS_ESP8266 0
#define TINY_VERSION 1
#define USE_LEDS 1
//#define HAS_DHT11 1
//#define HAS_HUMIDITY 1
#define HAS_LM35 1
//#define HAS_DS18B20 1
//#define HAS_SENSOR_ANALOGICO 1
//#define SENSOR_ANALOGICO_PIN A5
#define HAS_CORRENTE 0
#define MAX_CORRENTE_FASES 1
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 1
#define USE_DHCP  1
#define HAS_BLUETOOTH 2
#define DEBUG 0
//#define HAS_SENSOR_PORTA_10 1
//#define USE_RESET_FIRMWARE_ON_BOOT 1
#define TSENSOR_RECURSO (REC_LM35 | bit(REC_SENSOR_ANALOGICO) | bit(REC_LED1) | bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_ETHERNET))
//#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_LM35|bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO) | bit(REC_WIFI) | bit(REC_MODBUS))
#endif

#define WRITEAPYKEY_TALKBACK "2R38FNS3WAQ6107B" // TGG-0 talkback
#define TALKBACKID "1764"

#ifndef TSENSOR_RECURSO
#define TSENSOR_RECURSO bit(REC_CORRENTE_30A)| REC_LM35 |REC_DS18B20_EXTRA |bit(REC_LED1) |bit(REC_LED2) |bit(REC_LED3) |bit(REC_LM35) |bit(REC_BOTAO)|bit(REC_ALIMENTACAO) 
#endif

#ifndef DEMO
#define DEMO 0
#endif



#endif

// TODO : loop para gerador 29/09
// TODO : funções de regras 29/09
/**
 * @todo loop para gerador
 */