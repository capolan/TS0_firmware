#ifndef _TSENSOR_DEF_
#define _TSENSOR_DEF_
/*
 * Definição das Caixas
 * Macros
 
 * @2017
 * Carlos Alexandre Polanczyk 
 */

#if TSENSOR_SERIE >= 21 && TSENSOR_SERIE < 40 // WIFI
#undef DEBUG
#define DEBUG 1
//#define TSSERVER_ADDR_IP "52.35.8.102"   // AWS

#undef HAS_BLUETOOTH
#define HAS_BLUETOOTH 0
#define TEMPO_COLETA_CORRENTE 20
#define SSID_AP ""
//#define CANAL_TS "53093"
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,21
#define HAS_ESP8266 1
#define HAS_DHT11 1
#define HAS_DS18B20 1
#define HAS_HUMIDITY 1
//#define HAS_CORRENTE 1
#define MAX_CORRENTE_FASES 3
#define HAS_BOTAO 1
#define HAS_WEBSERVER_FULL 1
#define USE_LOG_EEPROM 0
#define HAS_CLOCK 0
#define HAS_RELE 1
#define USE_LEDS 1
#define PERMITE_TROCAR_AP 1
#define HAS_SENSOR_ANALOGICO 1
#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_DHT11 | bit(REC_WIFI) | bit(REC_HUMIDADE) | bit(REC_RELE) |\
		bit(REC_SENSOR_ANALOGICO) | bit(REC_LED1) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 21
#define MD5 "3a9a" // "3a9af96e1e9d75a4f9783964120b0f51"
#define CALIBRAGEM_CORRENTE 0xb000  // A2=100A   e A1,A0=temperatura   60.6 -> 100A   30.0 -> 30A
#define B_TEMPERATURA bit(B_PORTA_A0) | bit(B_PORTA_A1) | bit(B_PORTA_A2) // porta A0,A1,A2
#endif

#if TSENSOR_SERIE == 22
#define MD5 "26db" // "26db5ec01f743a6afd4a1222de6e68f"
#define SENSOR_ANALOGICO_PIN A0
#define SENSOR_ANALOGICO_PIN_2 0
#endif

#if TSENSOR_SERIE == 23
#define MD5 "847f" // "847fd04911c5e2f8971b17b14c483a6a"
#endif

#if TSENSOR_SERIE == 24
#define MD5 "456c" // "456c9de861d864a08e22826916f5eeb0"
#undef HAS_DHT11
#undef HAS_DHT22
#define HAS_DS18B20 1
#undef HAS_HUMIDITY
#define SENSOR_ANALOGICO_PIN A0
#define SENSOR_ANALOGICO_PIN_2 0
#define CALIBRAGEM_CORRENTE 60.6  // 100A
#endif

#if TSENSOR_SERIE == 25
#define MD5 "e3c4" // "e3c42efdae41175e1f95071fbafc1aa8"
#endif

#if TSENSOR_SERIE == 26
#define MD5 "a703" // "a703e244b7bc5ae16ac8d5fadb5ac4b2"
#define CALIBRAGEM_CORRENTE 0xb000  // A2=100A   e A1,A0=temperatura   60.6 -> 100A   30.0 -> 30A
#define B_TEMPERATURA bit(B_PORTA_A0) | bit(B_PORTA_A1) | bit(B_PORTA_A2) // porta A0,A1,A2
#endif

#if TSENSOR_SERIE == 27
#define MD5 "e48e" // "e48e228b226b0db4f841522d45bac71a"
#endif

#if TSENSOR_SERIE == 28
#define MD5 "4499" // "44998c70ce64c89bba61b915e72ca66b"
#endif

#if TSENSOR_SERIE == 29
#define MD5 "70c3" // "70c32041c5fda4d81098a22e408c04b9"
#endif

#if TSENSOR_SERIE == 30
#define MD5 "d75d" // "d75df037f4e7c86e3d31a00212e5ee54"
#endif

#if TSENSOR_SERIE == 31
#define MD5 "b2fd" // "b2fd79e791ab5c420e6ce0542033cbe5"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if TSENSOR_SERIE >= 41 && TSENSOR_SERIE <= 50 // ETHERNET
// hardwareSerial.h  comentar linha define para liberar memória
#undef DEBUG
#define DEBUG 0

#undef HAS_BLUETOOTH
#define HAS_BLUETOOTH 2

#define SSID_AP "ESP_nnnnnn"
//#define CANAL_TS "53093"
// http://www.miraclesalad.com/webtools/md5.php
// 45.55.77.192,TS0,21
#define HAS_ESP8266 0
#define HAS_ETH 1
//#define HAS_DS18B20 1
#define HAS_DHT11 1
#define HAS_HUMIDITY 1
#define HAS_CORRENTE 0
#define MAX_CORRENTE_FASES 3
#define HAS_BOTAO 1
#define USE_LEDS 1
#define HAS_CLOCK 0
#define HAS_RELE 0
#define HAS_SENSOR_ANALOGICO 0
#define HAS_EVENTOS 0
#define USE_LOG_EVENTOS 0
#define USE_LOG_EEPROM 0
#define HAS_WEBSERVER_FULL 0
#define HAS_WEBSERVER_TINY 1
#define TINY_VERSION 1
#define TSENSOR_RECURSO (bit(REC_CORRENTE_30A) | REC_DHT11 | bit(REC_ETHERNET) | bit(REC_HUMIDADE) | \
bit(REC_SENSOR_ANALOGICO) | bit(REC_LED1) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#define TSENSOR_RECURSO (REC_DHT11 | bit(REC_ETHERNET) | bit(REC_HUMIDADE) | \
 bit(REC_LED1) |bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

/*
 * ETHERNET
 */
#if TSENSOR_SERIE == 41 // ETH
#define MD5 "dc2c" //"dc2cde9e7f2f3fe98126a100b27f13cf"
#define TEMPO_COLETA_CORRENTE 30
#define HAS_CORRENTE 0
#define HAS_SENSOR_SECO 1
#define USE_SENSOR_SECO_PORTA_BAIXA 1
#define SENSOR_ANALOGICO_PIN A0
#define SENSOR_ANALOGICO_PIN_2 0
//#define TSSERVER_ADDR_IP "52.35.8.102"   // AWS
//#define PROXY_HTTP	"201.54.136.25"
//#define PROXY_PORT	3128

#define TSENSOR_RECURSO (bit(REC_SENSOR_SECO) | bit(REC_ETHERNET) | REC_DHT11|bit(REC_SENSOR_ANALOGICO)| \
		bit(REC_HUMIDADE) | bit(REC_LED1)| bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))

#endif

#if TSENSOR_SERIE == 42 // ETH
#define MD5 "0bec" //"0bec241a53d5493d69ad8a909d393381"
#undef HAS_SENSOR_ANALOGICO
#define HAS_CORRENTE 0
#define TSENSOR_RECURSO (bit(REC_ETHERNET) | REC_DHT11| \
		bit(REC_HUMIDADE) | bit(REC_LED1)| bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 43 
#define MD5 "47b6" //"47b67e2be16f6ec0063e026a707a20b7"
#endif

#if TSENSOR_SERIE == 44 
#define MD5 "dbee" //"dbee4c97f38df0b02ea10c8398e74b48"
#endif

#if TSENSOR_SERIE == 45 
#define MD5 "6c86" //"6c868273f456685ce67dafbac6b5ec23"
#endif

#if TSENSOR_SERIE == 46
#define MD5 "4dbd" //"4dbdccc6b696107af339c9d170b457fc"
#endif

#if TSENSOR_SERIE == 47 
#define MD5 "040c" //"040c4b4d4a0ade0e08e0a2f73437dfc8"
#undef HAS_DHT11 
#define HAS_DHT22 1
#endif

#if TSENSOR_SERIE == 48 
#define MD5 "8e6e" //"8e6eb14568064678a39bac2a491f8faa"
#define HAS_SENSOR_ANALOGICO 1
#define HAS_CORRENTE 0
#define SENSOR_ANALOGICO_PIN A0
#define SENSOR_ANALOGICO_PIN_2 0
#define TSENSOR_RECURSO (bit(REC_ETHERNET) | REC_DHT11| bit(REC_SENSOR_ANALOGICO) | \
bit(REC_HUMIDADE) | bit(REC_LED1)| bit(REC_LED3) | bit(REC_BOTAO)|bit(REC_ALIMENTACAO))
#endif

#if TSENSOR_SERIE == 49
#define MD5 "d3c5" //"d3c5d80fb1eab33df2204bb7dcab6246"
#endif


#if TSENSOR_SERIE == 50
#define MD5 "88b2" //"88b2e6637379c253b074d0a11d1abfc2"
#endif

#if TSENSOR_SERIE == 51 
#define MD5 "bd32" //"bd32b746557ed8b009f019e73c88b62f"
#endif

#if TSENSOR_SERIE == 52
#define MD5 "1f0d" //"1f0d9a1b2f2e087cb6fa6d2e05623656"
#endif

#if TSENSOR_SERIE == 53
#define MD5 "6909" //"69e90d6de3208aac08fc97bb63afe2a4"
#endif

#if TSENSOR_SERIE == 54
#define MD5 "2749" //"2749fe3441750115dc44926cc6e475fd"
#endif

#if TSENSOR_SERIE == 55
#define MD5 "dee8" //"dee8e8c6b316cf14b10c31223a2e7f2c"
#endif

#endif // TSENSOR_DEF