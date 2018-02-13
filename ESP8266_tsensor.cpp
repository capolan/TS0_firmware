#include "TSensor.h"
#include "MemoryFree.h"

#if HAS_ETH == 1
#include "IPAddress.h"
#endif
char reqHTTP[MAX_HTTPreq];
uint16_t wifi_len=0;
uint16_t flagHTTP;


const char str_SENSORONLINE[] PROGMEM = "<p>SensorOnline @2018</div>";
const char str_SENSORONLINE_TITULO[] PROGMEM = "<h1>SensorOnline</h1>";


#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
// http://www.electrodragon.com/w/ESP8266_Firmware#Firmware_Log_0.952
//
//  FLASH TOOL 2.4
//  DETECT INFO:
//  512Kbytes  azul
//		flash vendor:
//C8h : GD
//flash devID:
//4013h
//QUAD;4Mbit
//crystal:
//26 Mhz
//
// 1024Kbytes preto AI - Cloud
//flash vendor:
//C8h : GD
//flash devID:
//4014h
//QUAD;8Mbit
//crystal:
//26 Mhz
#ifdef __AVR__
//#include "MemoryFree.h"
#include <avr/eeprom.h>
#endif

#define PAGINA_PRINCIAL 0
#define PAGINA_SETUP	1
//#define PAGINA_SETUP_WIFI_CLIENT	2
#define PAGINA_SETUP_WIFI_AP	3
#define PAGINA_SETUP_THINGSPEAK	4
#define PAGINA_SETUP_FASES 5

#define PAGINA_MOB_PRINCIPAL 6

#define PAGINA_REBOOTING 10
#define PAGINA_RESET_ESP8266 11
#define PAGINA_CSV 12

//#define PAGINA_JSON	13
#define PAGINA_RET_STATUS 14

uint8_t page=PAGINA_PRINCIAL;


#define MAX_NAME	5
#define MAX_VALUE  21
// string em FLASH
//const char str_GET[] PROGMEM = "GET";
//const char str_POST[] PROGMEM = "POST";

//char str_HTML_header[] PROGMEM = "<!DOCTYPE html>\n<html>\n<head><title>TSensor</title>";
//const char str_HTML_header[] PROGMEM = "<html>\n<head><title>TSensor</title>";

const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} h2{font-size:1.2em;color:#1fa3ec;} div,input{padding:5px;font-size:1em;} body{text-algn: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} no-repeat left center;background-size: 1em;}</style>";

#if HAS_WEBSERVER_FULL == 1
const char str_HTML_header_body1[] PROGMEM = "<!DOCTYPE html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/><title>Tsensor</title>";
const char str_HTML_header_body2[] PROGMEM = "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{font-family:verdana;}h{text-align:center}button,input[type=submit]";
const char str_HTML_header_body3[] PROGMEM = "{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}</style><meta http-equiv=\"expires\" content=\"0\" />"
"</head>\n<body>";
const char str_HTML_header_body[] PROGMEM = "<!DOCTYPE html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/><title>Tsensor</title>"
	"<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{font-family:verdana;}h{text-align:center}button,input[type=submit]"
	"{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}</style><meta http-equiv=\"expires\" content=\"0\" />"
	"</head>\n<body>";
const char str_HTML_body_f[] PROGMEM = "</body></html>";


//"<meta http-equiv=\"cache-control\" content=\"no-cache\" />"
const char str_Form_get[] PROGMEM = "<form name=go method=\"get\">";
const char str_Form_submit_gravar_f[] PROGMEM = "<input type=submit name=GO value=Salvar></form>";

const char str_Form_button[] PROGMEM = "<button onclick=\"location.href=";
const char str_Form_button_f[] PROGMEM = "</button>";

const char str_table_i[] PROGMEM = "<table>";
const char str_table_f[] PROGMEM = "</table>";
const char str_tr_td[] PROGMEM = "<tr><td>";
const char str_td_tr[] PROGMEM = "</td></tr>";
const char str_td_td[] PROGMEM = "</td><td>";
const char str_td_td_right[] PROGMEM = "</td><td align=\"right\">";

#if HAS_ESP8266 == 1
const char str_INPUT_SSID[] PROGMEM = "Rede";
const char str_INPUT_PASSWD[] PROGMEM = "Senha";
#endif
#if HAS_ETH == 1
const char str_INPUT_DHCP[] PROGMEM = "DHCP";
const char str_INPUT_IP[] PROGMEM = "IP";
const char str_INPUT_GW[] PROGMEM = "GW";
const char str_INPUT_MASK[] PROGMEM = "Mask";
const char str_INPUT_DNS[] PROGMEM = "DNS";
#endif
const char str_INPUT_PROXY[] PROGMEM = "Proxy:porta";
const char str_INPUT_SERV_ADDR[] PROGMEM = "Ender.";
const char str_INPUT_ADDR_IP_TS[] PROGMEM = "IP";
const char str_INPUT_CHAVE[] PROGMEM = "Chave";

const char str_INPUT_TOKEN_TS[] PROGMEM = "Token";
const char str_INPUT_CANAL_TS[] PROGMEM = "Canal";
const char str_INPUT_SEND_TS[] PROGMEM = "Ativar";

const char str_INPUT_CRYPTO[] PROGMEM = "Cripto";
#if USE_TWITTER == 1
const char str_INPUT_SEND_TWITTER[] PROGMEM = "Twitter";
#endif
#if USE_TS_FIELD == 1
const char str_INPUT_FIELD_CORRENTE_TS[] PROGMEM = "Campo";
#endif

const char str_graus_C[] PROGMEM = "&deg;C";
const char str_INPUT_FASES[] PROGMEM = "Fases (1,2 ou 3)";
const char str_INPUT_TENSAO_127_220[] PROGMEM = "Tensao (127/220)";
#if USE_P_STATUS_MODO_TESTE == 1
const char str_INPUT_MODO_TESTE[] PROGMEM = "Teste";
#endif

#if USE_STR_INPUT_FCN == 1
const char str_INPUT_FCN_I[] PROGMEM ="<INPUT TYPE=TEXT NAME=";
#else
const char str_INPUT_FCN_I[] PROGMEM ="<INPUT TYPE=HIDDEN NAME=";
#endif
const char str_INPUT[] PROGMEM ="<INPUT TYPE=TEXT NAME=";
const char str_INPUT_VALUE[] PROGMEM = " VALUE=\"";
const char str_INPUT_MAXLENGTH[] PROGMEM = " MAXLENGTH=";

//char str_INPUT_FCN[] PROGMEM = "Funcao:<input type=text name=" CMD_FUNCAO " value=";
const char str_INPUT_FCN[] PROGMEM = "Funcao";
const char str_Consumo[] PROGMEM = "Consumo";
const char str_Temperatura[] PROGMEM = "Temperatura";
const char str_FASE[] PROGMEM = "Fase_";
#if HAS_HUMIDITY == 1 || HAS_I2C == 1
const char str_Humidade[] PROGMEM = "Umidade";
#endif
#if HAS_SENSOR_ANALOGICO == 1
const char str_Sensor_Analogico[] PROGMEM = "Analogico";
#endif
#if HAS_SENSOR_SECO == 1
const char str_Sensor_Seco[] PROGMEM = "Seco";
#endif

#if PERMITE_DOWNLOAD == 1
const char str_Button_download[] PROGMEM = "'/d';\">Download</button>";
#endif
const char str_Button_setup[] PROGMEM = "'/s';\">Setup</button>";

#endif // HAS_WEB_SERVER
int esp8266HTTPheader(void);
void esp8266HTTPreq(void);
uint8_t esp8266HTTPresp(int, uint8_t);
uint8_t eATCWJAP(void);

void sendHTMLcsv(int connectionId);
void sendHTMLPrincipal(int);
void sendHTMLSetup(int);
static void eATMAC(void);
void esp8266psend(int connectionId, char *pwebpage);

char connectCodeWifi;
// https://gist.github.com/jmsaavedra/7964251
// caracteres especiais &$@#=;
void urldecode2(char *dst, const char *src)
{
	char a, b;
	while (*src) {
		if ((*src == '%') &&
		((a = src[1]) && (b = src[2])) &&
		(isxdigit(a) && isxdigit(b))) {
			if (a >= 'a')
			a -= 'a'-'A';
			if (a >= 'A')
			a -= ('A' - 10);
			else
			a -= '0';
			if (b >= 'a')
			b -= 'a'-'A';
			if (b >= 'A')
			b -= ('A' - 10);
			else
			b -= '0';
			*dst++ = 16*a+b;
			src+=3;
		}
		else {
			*dst++ = *src++;
		}
	}
	*dst++ = '\0';
	debugvarln(dst);
}
#if HAS_ETH == 1
//////////////////////////////////////////////////////////
void iptos(IPAddress ip, char* p)
{
	uint8_t i;
	for (i=0;i<4;i++) {
		itoa(ip[i],p+strlen(p),10);
		if (i<3)
			strcat(p,".");
	}
}
//////////////////////////////////////////////////////////
void stoip(IPAddress &ip,char* p)
{
	uint8_t i=0, val=0;
	while (*p) {
		if (*p!='.') {
			val = val*10 + *p-'0';
		} else {
			if (i<4) 
				ip[i++]=val;
			val=0;
		}
		p++;
	}
	if (i==3) ip[i]=val;
}
#endif
//////////////////////////////////////////////////////////

#if 0
//char specials[] = "$&+,/:;=?@ <>#%{}|~[]`"; ///* String containing chars you want encoded */
const char specials[] PROGMEM = "$&+,/:;=?@ <>#%{}|~[]`"; ///* String containing chars you want encoded */

static char hex_digit(char c)
{  return "01234567890ABCDEF"[c & 0x0F];
}

char* urlencode(char* dst,char* src)
{  
	unsigned char c;
	char *d = dst;
	while (c = *src++)
	{  if (strchr_P(specials,c))
		{  *d++ = '%';
			*d++ = hex_digit(c >> 4);
			*d++ = hex_digit(c);
		}
		else *d++ = c;
	}
	*d='\0';
	return dst;
}
#endif

#if HAS_WEBSERVER_FULL == 1
/////////////////////////////////////////////////////////////////////////////////////
void parse_ESP8266()
{
	uint8_t connectionId,ret;
	#if HAS_ETH == 1
	if (SSwifi = server.available())
	#else
	if(SSwifi.available()>0) // check if the esp is sending a message
	#endif
	{
	PGM_PRINTLN(">WEB");
		if(recvPkg(0,(uint8_t*)reqHTTP, MAX_HTTPreq, 2000, &connectionId) > 0)
		{
			PGM_PRINT("#");
			led1_status=LED_PISCA_CURTO;
			//if (!reqHTTP[0] || !esp8266HTTPheader()) {
			if (!reqHTTP[0]) {
				esp8266Flush();
				} else {
				esp8266HTTPreq();
				debugvarln(page);
				switch (page)
				{
					#if USE_LOG_EEPROM == 1
					case PAGINA_CSV:
					ret=esp8266HTTPresp(connectionId,HTTP_RESP_CSV);
					break;
					#endif
					default:
					ret=esp8266HTTPresp(connectionId,HTTP_RESP_TEXT);
				}
				if (ret)
				//			debugvarln(page);
				switch (page) {
					case PAGINA_PRINCIAL:
					sendHTMLPrincipal(connectionId);
					case PAGINA_RET_STATUS:
					break;
					#if USE_PAGINA_MOB_PRINCIPAL == 1
					case PAGINA_MOB_PRINCIPAL:
					sendHTMLMobilePrincipal(connectionId);
					break;
					#endif
					#if USE_LOG_EEPROM == 1
					case PAGINA_CSV:
					sendHTMLcsv(connectionId);
					break;
					#endif
					/*				case PAGINA_SETUP:
					case PAGINA_SETUP_WIFI_AP:
					case PAGINA_SETUP_THINGSPEAK:
					case PAGINA_SETUP_FASES:
					case PAGINA_REBOOTING:
					case PAGINA_RESET_ESP8266:
					*/
					default:
					sendHTMLSetup(connectionId);
					break;
					
				}
			}
			PGM_PRINTLN("<WEB");
			led1_status=0;

		}
	}
}

////////////////////////////////////////////////////
// Envia os dados
///////////////////////////////////////////////////
void sendHTMLPrincipalDados(uint8_t connectionId, char *p)
{
	int totalConsumo;
	#if HAS_I2C_COPY == 1
	uint8_t fases=B_FASE;
	#else
	uint8_t fases=gParam.rede.fases & B_FASE;
	#endif;
	struct TDS20B18 *pT;
	char c[3];
	#if HAS_CORRENTE == 1 || HAS_I2C_COPY == 1
	strcpy_P(p,str_table_i);
	strcat_P(p,str_tr_td);
	strcat_P(p,str_Consumo);
	totalConsumo=correnteFase[0]+correnteFase[1]+correnteFase[2];
	strcat_P(p,str_td_td_right);
	itoa(totalConsumo,p+strlen(p),10);
	strcat_P(p,str_td_tr);
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	for (uint8_t bit_fase=0; bit_fase < MAX_CORRENTE_FASES; bit_fase++ ) {
		if (bitRead(fases,bit_fase)) {
			strcpy_P(p,str_tr_td);
			strcat_P(p,str_FASE);
			itoa(bit_fase+1,p+strlen(p),10);
			strcat_P(p,str_td_td_right);
			utoa(correnteFase[bit_fase],p+strlen(p),10);
			strcat_P(p,str_td_tr);
			sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		}
	}
	#else
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_table_i,true);
	#endif

	#if HAS_TEMPERATURA == 1 || HAS_I2C_COPY == 1
	pT=pTemp;
	strcpy_P(c,PSTR("_1"));
	//c[1]='1';
	while (pT != NULL) {
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_Temperatura);
		strcat(p,c);
		strcat_P(p,str_td_td_right);
		//dtostrf(temperaturaAmbiente/countCorrente,5,1,p+strlen(p));
		itoa(pT->temperatura/10,p+strlen(p),10);
		strcat_P(p,str_graus_C);
		strcat_P(p,str_td_tr);
		#if HAS_HUMIDITY==1 || HAS_I2C_COPY == 1
		if (!pT->ds && pT->addr[0] & 2) {
			sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
			strcpy_P(p,str_tr_td);
			strcat_P(p,str_Humidade);
			strcat_P(p,str_td_td_right);
			itoa(humidade/10,p+strlen(p),10);
			strcat(p,"%");
			strcat_P(p,str_td_tr);
		}
		#endif
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		c[1]++;
		pT=pT->p;	
	}
	#endif

	#if HAS_SENSOR_ANALOGICO == 1
		for (uint8_t ps=MAX_SENSOR_ANALOGICO; ps;) {
			*p='\0';
			ps--;
			strcat_P(p,str_tr_td);
			strcat_P(p,str_Sensor_Analogico);
			strcat_P(p,str_td_td_right);
			if (gParam.porta_sensor_analogico[ps])
				itoa(sensor_analogico[ps],p+strlen(p),10);
			else
				strcat(p,"-");
			strcat_P(p,str_td_tr);
			sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	}
	#endif
	#if HAS_SENSOR_SECO == 1
	strcpy_P(p,str_tr_td);
	strcat_P(p,str_Sensor_Seco);
	strcat_P(p,str_td_td_right);
	itoa(gParam.sensor_seco.valor,p+strlen(p),2);
	strcat(p,"|");
	itoa(gParam.sensor_seco.trocou,p+strlen(p),2);
	strcat_P(p,str_td_tr);
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	#endif

	#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
	#if HAS_I2C_COPY == 1
	for (uint8_t client=1;client<MAX_TSENSOR_CLIENT;client++)
	#else
	for (uint8_t client=0;client<MAX_TSENSOR_CLIENT;client++)
	#endif
	{
			uint16_t recursoNode;
			int *val;
			if (bitRead(gParam.radio.client,client)) {
					recursoNode=gParam.radio.recursoClient[client];
					val=&gParam.radio.dataClient[client][0];
					strcpy_P(p,str_tr_td);
					strcat_P(p,PSTR("Node "));
					itoa(client,p+strlen(p),10);
					strcat_P(p,str_td_td);
					utoa(gParam.radio.serieClient[client],p+strlen(p),10);
					strcat(p,".");
					utoa(recursoNode,p+strlen(p),16);
					strcat_P(p,str_td_tr);
					sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
					c[1]='1';
					//if (recursoNode & REC_SENSORES) {
						//gParam.radio.sensor9_10[client]=recPack.data[0];
					//}
					// Temperatura
					strcpy_P(p,str_tr_td);
					strcat_P(p,str_Temperatura);
					strcat(p,c);
					strcat_P(p,str_td_td_right);
					itoa(*val++/10,p+strlen(p),10);
					strcat_P(p,str_graus_C);
					strcat_P(p,str_td_tr);
					sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
					// sensores extras
					c[1]++;
					if (recursoNode & (REC_DS18B20_EXTRA | bit(REC_HUMIDADE))) {
						strcpy_P(p,str_tr_td);
						if (recursoNode & bit(REC_HUMIDADE)) {
							strcat_P(p,str_Humidade);
						} else {
							strcat_P(p,str_Temperatura);
							strcat(p,c);
						}
						strcat_P(p,str_td_td_right);
						itoa(*val++/10,p+strlen(p),10);
						if (recursoNode & bit(REC_HUMIDADE)) {
							strcat(p,"%");
						} else {
							strcat_P(p,str_graus_C);
						}
						strcat_P(p,str_td_tr);
						sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
					} else
						val++;
					// testa se node tem corrente
					if (recursoNode & 0b011) {   // REC_CORRENTE_30A e REC_CORRENTE_100A
						for (uint8_t bit_fase=0; bit_fase < 3; bit_fase++ ) {
							*p='\0';
							strcat_P(p,str_tr_td);
							strcat_P(p,str_FASE);
							itoa(bit_fase,p+strlen(p),10);
							strcat_P(p,str_td_td_right);
							itoa(*val++,p+strlen(p),10);
							strcat_P(p,str_td_tr);
							sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
						} // for
					} //if
		}
	}
	


	#endif

	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_table_f,true);

	
}
////////////////////////////////////
//
//////////////////////////////////
void sendHTMLmostraIP(uint8_t connectionId, char*p)
{
	#if HAS_ESP8266 == 1
	strcat_P(p,PSTR("<div class=\"footer\">"));
	strcpy_P(p,str_INPUT_SSID);
	strcat_P(p,PSTR(":"));
	strcat(p,gParam.web.ssid);
	strcat_P(p,PSTR("<br>uptime="));
	itoa(timerWeb+gParam.rede.tempoColetaCorrente,p+strlen(p),10);
	strcat_P(p,PSTR("s_("));
	itoa(gParam.rede.tempoWebCorrente,p+strlen(p),10);
	strcat(p,"*");
	itoa(gParam.rede.tempoColetaCorrente,p+strlen(p),10);
	strcat_P(p,PSTR("s)<br>AP="));
	strcat(p,gParam.web.ip[0]);
	strcat(p,"(");
	strcat(p,gParam.web.mac);
	strcat_P(p,PSTR(")<br>Local="));
	strcat(p,gParam.web.ip[1]);
	#else
	strcpy_P(p,PSTR("<div class=\"footer\">Local="));
	strcat(p,gParam.web.ip[1]);
	#endif

	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	
	#if HAS_CLOCK == 1
	if (bitRead(ccStatus,CC_CLOCK_OK)) {
		strcpy_P(p,PSTR("<br><font size=small>"));
		formatHora(p+strlen(p),now());
		strcat_P(p,PSTR("</font>"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	}
	#endif
	strcpy_P(p,PSTR("<br>v_"));
	#ifdef __AVR__
//	dtostrf(TSENSOR_SERIE*1000+(float)VERSION_MAJOR+((float)VERSION_MINOR)/100.0,6,2,p+strlen(p));
	strcat_P(p,PSTR(VERSION));
	#endif
	strcat_P(p,PSTR("_r_"));
	ultoa(TSENSOR_RECURSO,p+strlen(p),16);	
	strcat_P(p,PSTR("."));
	utoa(ccStatus & CC_BITS_WEB,p+strlen(p),2);
	#if HAS_I2C == 1
	strcat_P(p,PSTR("."));
	utoa(gParam.radio.ComERR,p+strlen(p),2);
	#endif
	strcat_P(p,PSTR("_s_"));
	utoa(gParam.id,p+strlen(p),10);
	strcat_P(p,PSTR("<br>"));
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	#if HAS_ESP8266 == 1
	eATCWMODE(2);  // CWLIF lista os clientes conectados no AP
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)reqHTTP,true);
	#endif
	
	reqHTTP[0]='\0';
	if (gParam.web.ssid[0]) {
	//                     0123456
	strcat_P(reqHTTP,PSTR("Wifi:0_"));
	reqHTTP[5]=connectCodeWifi+'0';
	switch (connectCodeWifi)
	{
		case 0: // OK
			strcat_P(reqHTTP,PSTR("ok"));
			break;
		case 1: // timeout
		strcat_P(reqHTTP,PSTR("timeout"));
		break;
		case 2: // senha
		strcat_P(reqHTTP,PSTR("senha"));
		break;
		case 3: // ap
		strcat_P(reqHTTP,PSTR("ssid"));
		break;
		case 4: // OK
		strcat_P(reqHTTP,PSTR("falha"));
		break;
		case 5: // nowifi
		strcat_P(reqHTTP,PSTR("sem"));
		break;
	}
	}
	if (bitRead(ccStatus,CC_CWJAP))	 {
		strcat_P(reqHTTP,PSTR("<br>"));
		eATCWSAP_DEF(reqHTTP+strlen(reqHTTP));
	}
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)reqHTTP,true);

	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_SENSORONLINE,true);
}
//////////////////////////////////////////////
//
////////////////////////////////////////////
#if USE_LOG_EEPROM == 1
void sendHTMLcsv(int connectionId) {
	char p[80];
	#if USE_LOG_EEPROM == 1
	list_corrente_log(2,connectionId,p);
	#endif
	sATCIPCLOSEMulitple(connectionId);
	
}
#endif
/////////////////////////////////////////////////////
#define MAX_TX_BLOCK 80
uint8_t send_str_HTML_header(uint8_t connectionId, char *pdata)
{
	int len=strlen_P(str_HTML_header_body);
	char *p=(char*)str_HTML_header_body;
	while (len>0) {
		strncpy_P(pdata,p,MAX_TX_BLOCK);
		pdata[MAX_TX_BLOCK]=0;
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)pdata,true)) return false;
		p+=MAX_TX_BLOCK;		
		len -=MAX_TX_BLOCK;
	}

	return true;
}
/////////////////////////////////////////////////////
void sendHTMLPrincipal(int connectionId) {
	char p[100];
	if (!send_str_HTML_header(connectionId,p)) return ;

	strcpy_P(p,str_SENSORONLINE_TITULO);
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

	//	sendHTMLgauge_body(connectionId);
	
	sendHTMLPrincipalDados(connectionId,p);
	
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Form_button,true);
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Button_setup,true);
	
	#if PERMITE_DOWNLOAD == 1
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Form_button,true);
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Button_download,true);
	#endif

	sendHTMLmostraIP(connectionId,p);

	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_HTML_body_f,true);
	
	sATCIPCLOSEMulitple(connectionId);
}

//////////////////////////////////////////////////////////
void sendHTMLSetup(int connectionId) {
	char p[100];
	
	if (!send_str_HTML_header(connectionId,p)) return ;

	strcpy_P(p,PSTR("<h1>Setup</h1>"));
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

	//-----------------------------
	// WIFI setup
	if (page == PAGINA_SETUP) { // || page==PAGINA_SETUP_WIFI_CLIENT) {
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Form_get,true)) return;

		// FUNCAO 1
		#if USE_STR_INPUT_FCN == 1
		strcpy_P(p,str_INPUT_FCN);
		strcat_P(p,str_INPUT);
		#else
		strcpy_P(p,str_INPUT_FCN_I);
		#endif
		strcat(p,CMD_FUNCAO);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,"1\">");
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_table_i,true)) return;

		#if HAS_ESP8266 == 1
		// SSID
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_SSID);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_SSID);
		strcat_P(p,str_INPUT_MAXLENGTH);
		itoa(SSID_LEN,p+strlen(p),10);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,gParam.web.ssid);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		
		// PASSWD
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_PASSWD);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_PASSWD);
		strcat_P(p,str_INPUT_MAXLENGTH);
		itoa(PASSWD_LEN,p+strlen(p),10);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,gParam.web.passwd);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		#endif
		#if HAS_ETH == 1
		// use DHCP
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_DHCP);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_USE_DHCP);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,bitRead(gParam.status,P_STATUS_USE_DHCP)?"S":"N");
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		// IP da eth
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_IP);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_IP);
		strcat_P(p,str_INPUT_VALUE);
		iptos(gParam.web.eth,p);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		// mask
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_MASK);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_MASK);
		strcat_P(p,str_INPUT_VALUE);
		iptos(gParam.web.mask,p);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		// gw
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_GW);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_GW);
		strcat_P(p,str_INPUT_VALUE);
		iptos(gParam.web.gw,p);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		// dns
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_DNS);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_DNS);
		strcat_P(p,str_INPUT_VALUE);
		iptos(gParam.web.dns,p);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		#endif
		
		// PROXY
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_PROXY);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_PROXY);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,gParam.web.http_proxy);
		strcat(p,":");
		itoa(gParam.web.http_port,p+strlen(p),10);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		#if HAS_NRF24L01 == 1
		// SERV ADDR
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_SERV_ADDR);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_SER_ADDR);
		strcat_P(p,str_INPUT_VALUE);
		tam=strlen(p);
		strcat(p,"1");
		p[tam]=gParam.radio.serv_addr;
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		#endif

	}
	#if PERMITE_TROCAR_SERVER == 1
	// THINGSPEAK
	if (page==PAGINA_SETUP_THINGSPEAK) {

		//----------------------------
		// THINGSPEAK setup
		// FUNCAO 2
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Form_get,true);

		#if USE_STR_INPUT_FCN == 1
		strcpy_P(p,str_INPUT_FCN);
		strcat_P(p,str_INPUT);
		#else
		strcpy_P(p,str_INPUT_FCN_I);
		#endif
		strcat(p,CMD_FUNCAO);
		strcat_P(p,str_INPUT_VALUE);
		strcat_P(p,PSTR("2\">"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_table_i,true);

		// ADDR IP
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_ADDR_IP_TS);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_ADDR_IP_TS);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,gParam.web.server_addr);
		strcat(p,":");
		itoa(gParam.web.server_port,p+strlen(p),10);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		
		// FIELD
		#if USE_TS_FIELD == 1
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_FIELD_CORRENTE_TS);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_FIELD_CORRENTE_TS);
		strcat_P(p,str_INPUT_VALUE);
		itoa(gParam.web.field_corrente_ts,p+strlen(p),10);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		#endif

		#if USE_TWITTER == 1
		// SEND TWITTER
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_SEND_TWITTER);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_SEND_TWITTER);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,bitRead(gParam.status,P_STATUS_SEND_TWITTER)?"S":"N");
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		#endif
	}
	#endif
	// SETUP_AP
	#if PERMITE_TROCAR_AP == 1
	if (page==PAGINA_SETUP_WIFI_AP) {

		//----------------------------
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Form_get,true);
		#if USE_STR_INPUT_FCN == 1
		strcpy_P(p,str_INPUT_FCN);
		strcat_P(p,str_INPUT);
		#else
		strcpy_P(p,str_INPUT_FCN_I);
		#endif
		strcat(p,CMD_FUNCAO);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,"4\">");
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_table_i,true);
		// SSID AP
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_SSID);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_SSID);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,gParam.web.ssid_ap);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		
		// PASSWD AP
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_PASSWD);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_PASSWD);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,gParam.web.passwd_ap);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		// CANAL
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_CANAL_TS);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_CANAL_TS);
		strcat_P(p,str_INPUT_VALUE);
		itoa(gParam.web.canal_ap,p+strlen(p),10);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		// CRIPTO
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_CRYPTO);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_CRIPTO);
		strcat_P(p,str_INPUT_VALUE);
		itoa(gParam.web.cripto_ap,p+strlen(p),10);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	}
	#endif

	#if PERMITE_TROCAR_FASES == 1
	// FASES/TENSAO
	if (page==PAGINA_SETUP_FASES) {

		//----------------------------
		// FASES REDE setup
		// FUNCAO 3
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Form_get,true);

		#if USE_STR_INPUT_FCN == 1
		strcpy_P(p,str_INPUT_FCN);
		strcat_P(p,str_INPUT);
		#else
		strcpy_P(p,str_INPUT_FCN_I);
		#endif
		strcat(p,CMD_FUNCAO);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,"3\">");
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_table_i,true);
		// FASES
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_FASES);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_FASES_NRO);
		strcat_P(p,str_INPUT_VALUE);
		itoa((gParam.rede.fases & B_FASE),p+strlen(p),10);
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		
		// TENSAO
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_TENSAO_127_220);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_FASES_TENSAO);
		strcat_P(p,str_INPUT_VALUE);
		strcat(p,bitRead(gParam.rede.fases,B_TENSAO)?"220":"127");
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

	}
	#endif


	switch (page) {
		case PAGINA_REBOOTING:
		strcpy_P(p,PSTR("Reiniciando...<BR>Reconecte no WIFI.<br>"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		break;
		default:
		// CHAVE
		strcpy_P(p,str_tr_td);
		strcat_P(p,str_INPUT_CHAVE);
		strcat_P(p,str_td_td);
		strcat_P(p,str_INPUT);
		strcat(p,CMD_CHAVE);
		strcat_P(p,str_INPUT_VALUE);
		#if DEMO > 0
//		strcat(p,MD5);
		strncat(p,gParam.chave,4);
		#endif
		strcat(p,"\">");
		strcat_P(p,str_td_tr);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;

		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_table_f,true)) return;
		
		if (bitRead(gParam.status,P_STATUS_GRAVA_EEPROM)) {
			sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)PSTR("<h2>Salvo...</h2>"),true);
		}

		if (bitRead(gParam.status,P_STATUS_NO_AUTH)) {
			sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)PSTR("<h2>Chave errada</h2>"),true);
			bitClear(gParam.status,P_STATUS_NO_AUTH);
		}

		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Form_submit_gravar_f,true);

		// RESET REBOOT
		// FUNCAO 4
		strcpy_P(p,str_Form_button);
		strcat_P(p,PSTR("'/f=7';\">Reboot</button>\n"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		//-----------------------------------
		// RESET FIRMWARE
		// FUNCAO 5
		strcpy_P(p,str_Form_button);
		strcat_P(p,PSTR("'/f=8';\">Reset FW</button>\n"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);

		//-----------------------------------

		// MENU PRINCIPAL

		strcpy_P(p,str_Form_button);
		#if HAS_ESP8266 == 1
		strcat_P(p,PSTR("'/s';\">Wifi</button>\n"));
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		#endif
		#if HAS_ETH == 1
		strcat_P(p,PSTR("'/s';\">Rede</button>\n"));
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		#endif

		#if PERMITE_TROCAR_SERVER == 1
		strcpy_P(p,str_Form_button);
		strcat_P(p,PSTR("'/t';\">Servidor</button>\n"));
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return;
		#endif

		#if PERMITE_TROCAR_AP == 1
		strcpy_P(p,str_Form_button);
		strcat_P(p,PSTR("'/a';\">AP</button><br>\n"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		#endif

		#if PERMITE_TROCAR_FASES == 1
		strcpy_P(p,str_Form_button);
		strcat_P(p,PSTR("'/f';\">Energia</button>\n"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		#endif
		strcpy_P(p,str_Form_button);
		strcat_P(p,PSTR("'/';\">Voltar</button>\n"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	}

	sendHTMLmostraIP(connectionId,p);

	sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_HTML_body_f,true);
	sATCIPCLOSEMulitple(connectionId);

}
////////////////////////////////////////////////////////////////////////
//  tipo: 0 - text/html
//        1 - csv
//////////////////////////////////////////
uint8_t esp8266HTTPresp(int connectionId, uint8_t tipo) {
	char p[80];
	PGM_PRINTLN(">resp");
	esp8266Flush();
	if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_HTTP_200,true)) return false;
	//if (bitRead(ccStatus,CC_WIFI_ERROR)) return false;


#if 1

	strcpy_P(p,str_Content_type);
	#if USE_LOG_EEPROM == 1 || PERMITE_DOWNLOAD == 1
	if (tipo==HTTP_RESP_CSV) {
		strcat_P(p,PSTR("text/csv\n"));
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
		
		strcpy_P(p,PSTR("Content-disposition: attachment;filename=dados.csv\n"));
		} else {
		#endif
		#if USE_PAGINA_JSON == 1
		if (tipo==HTTP_RESP_JSON)
		strcpy_P(p,str_TS_Content_type_JSON);
		else
		#endif
		strcat_P(p,str_Content_type_text);
		if (!sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true)) return false;
		strcpy_P(p,str_TS_Connection);
		#if USE_LOG_EEPROM == 1 || PERMITE_DOWNLOAD == 1
	}
	#endif

	#endif // 1
	if (flagHTTP & (bit(F_RET_STATUS) | bit(F_RET_LER_IP))) {
		sATCIPSENDMultiple(connectionId,POINTER_CHAR_PROGMEM,(uint8_t *)str_Content_length,true);
//		pisca(LED_AMARELO,8,120);
		if (bitRead(flagHTTP,F_RET_LER_IP)) {
			uint8_t tam=strlen(gParam.web.ip[0])+strlen(gParam.web.ip[1])+1; // +4 = <br>
			itoa(tam, p, 10);
			//itoa(tam, p+strlen(p), 10);
			strcat(p,"\n\n");
			strcat(p,gParam.web.ip[0]);
			strcat(p,",");
			strcat(p,gParam.web.ip[1]);
			} else {
			strcat_P(p,PSTR("2\n\nOK"));
		}
		debugvarln(p);
	} else
	strcat(p,"\n");
	sATCIPSENDMultiple(connectionId,POINTER_CHAR_RAM,(uint8_t *)p,true);
	
	if (bitRead(flagHTTP,F_RET_STATUS)) {
		//PGM_PRINT("flagHTTP:"); debugbinln(flagHTTP);
		sATCIPCLOSEMulitple(connectionId);
		return false;
	}
	PGM_PRINTLN("<esp");

	return true;
}

////////////////////////////////////////////////////////////////////////
char *parseHTTPreq(char *p, char *name, char *value) {
	char *retP=NULL;
	uint8_t tam;
	#if DEBUG > 0
	char *n=name, *v=value;
	#endif
	*name=*value='\0';
	
	
	while (*p == '/' || *p == '?') p++;
	
	tam=0;
	while (isalnum(*p)) {
//	while (*p != '=') {
		if (tam < MAX_NAME) {
			*name++=*p;
			tam++;
			} 
		p++;
	}
	*name++='\0';
	if (*p == '=') {
		bitSet(flagHTTP,F_VAR);
		p++;
		tam=0;
		//		while (isalnum(*p) || *p=='.' || *p==':' || *p=='%') {
		while (*p != '&' && *p!=' ') {
			if (*p=='+')
				*p=' ';
			if (tam < MAX_VALUE) {
				*value++=*p;
				tam++;
				}
			p++;
		}
		*value='\0';
	}
	// testa se é / (nova pagina) ou ? (variaveis) ou '\n' fim
	switch (*p++) {
		case '\n':
		case ' ':
		break;
		case '?':
		case '&':
		case '/':
		if (*p != ' ')	retP=p;
		break;
	}

	#if DEBUG > 0
	debugvar(n);
	debugvar(v);
	debugvarln(p);
	#endif
	return retP;
}
////////////////////////////////////////////////////////////////////////
void esp8266HTTPreq(void)
{
	char name[MAX_NAME+1];
	char var[MAX_VALUE+1];
	PGM_PRINTLN(">esp8266HTTPreq");
	uint8_t cmdHTTPfuncao=0;
//	bitClear(gParam.status,P_STATUS_NO_AUTH);
		char *p=reqHTTP+5;
		//debugvarln(reqHTTP);
		
		bitClear(flagHTTP,F_VAR);
		bitClear(flagHTTP,F_RET_STATUS);
		bitClear(flagHTTP,F_RET_LER_IP);
		page=0xFF;
		do {
			p=parseHTTPreq(p,(char*)name,(char*)var);
			debugvarln(name);
		//	if (name[1]) break;
		debugbinln(flagHTTP);
			if (!bitRead(flagHTTP,F_VAR)) {
			debugvarln(page);
				if (!strncmp(name,"fav",3) || !strncmp(name,"app",3) || !strncmp(name,"tou",3)) {
					PGM_PRINTLN("favicon");
					name[0]='\0';
				}
				switch (name[0]) {
					case 0:
					p=NULL; 
					if (page==0xFF)
						page=0;
					break;
					case 's':   // setup PAGE
					page=PAGINA_SETUP;
					break;
					case 'a':   // WIFI AP setup
					page=PAGINA_SETUP_WIFI_AP;
					break;
					case 't':   // ThingSpeak SETUP
					page=PAGINA_SETUP_THINGSPEAK;
					break;
					case 'f':   // Fases SETUP
					page=PAGINA_SETUP_FASES;
					break;
					#if USE_LOG_EEPROM == 1
					case 'd':   // CSV
					page=PAGINA_CSV;
					break;
					#endif
					#if USE_PAGINA_MOB_PRINCIPAL == 1
					case 'm':
					page=PAGINA_MOB_PRINCIPAL;
					break;
					#endif
					default:
					page=PAGINA_PRINCIAL;
				}
			}
			// funcao retornar status
			if (name[0]==CMD_RET_STATUS) {
				bitSet(flagHTTP,F_RET_STATUS);
				page=PAGINA_RET_STATUS;
			}
			
			// funcao retornar ip
			if (name[0]==CMD_RET_GET_IP) {
				bitSet(flagHTTP,F_RET_LER_IP);
				page=PAGINA_RET_STATUS;
			}

			// funcao
			//			if (!strcmp(name,CMD_FUNCAO)) {
			if (name[0]==CMD_FUNCAO_C) {
				cmdHTTPfuncao=atoi(var);
			}
			if (name[0]==CMD_CHAVE_C) {
				debugvar(var);
				debugvarln(gParam.chave);
				if (strncmp(var,gParam.chave,4)) {
					bitSet(gParam.status,P_STATUS_NO_AUTH);
					}
			}

			// WIFI CLIENT
			if (cmdHTTPfuncao==FCN_CMD_WIFI) {
				#if HAS_EEPROM == 1
				bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
				#endif
				#if HAS_ESP8266 == 1
				// SSID
				if (name[0]==CMD_SSID_C) {
					//strcpy(gParam.web.ssid,var);
					urldecode2(gParam.web.ssid,var);
					debugvarln(var);
					bitWrite(ccStatus,CC_HAS_SSID,var[0]);
					bitSet(flagHTTP,F_SETUP_WIFI);
				}
				// PASSWD
				if (name[0]==CMD_PASSWD_C) {
					urldecode2(gParam.web.passwd,var);
					//	debugvarln(gParam.web.passwd);
					//	strcpy(gParam.web.passwd,var);
				}
				#endif
				#if HAS_ETH == 1
				if (name[0]==CMD_USE_DHCP_C) {
					if (toupper(var[0])=='S') bitSet(gParam.status,P_STATUS_USE_DHCP);
					if (toupper(var[0])=='N') bitClear(gParam.status,P_STATUS_USE_DHCP);
				}
				if (name[0]==CMD_IP_C) {
					stoip(gParam.web.eth,var);
				}
				if (name[0]==CMD_GW_C) {
					stoip(gParam.web.gw,var);
				}
				if (name[0]==CMD_MASK_C) {
					stoip(gParam.web.mask,var);
				}
				if (name[0]==CMD_DNS_C) {
					stoip(gParam.web.dns,var);
				}
				#endif
				// PROXY  ip:porta  exemplo 192.168.1.1:3128
				if (name[0]==CMD_PROXY_C) {
					char *dp=strchr(var,':');
					uint8_t idx=1;
					if (dp == NULL) {
						dp=strchr(var,'%');
						idx+=2;
					}
					if (dp != NULL && *var) {
						gParam.web.http_port=atoi(dp+idx);
						*dp='\0';
						strcpy(gParam.web.http_proxy,var);
						//PGM_PRINT("proxy=");
						//debugln(gParam.web.http_proxy);
						} else {
						gParam.web.http_port=0;
						gParam.web.http_proxy[0]='\0';
					}
				}
				// SERV ADDR
				#if HAS_NRF24L01
				if (name[0]==CMD_SER_ADDR_C) {
					gParam.radio.serv_addr=var[0];
				}
				#endif
			}
			
			// WIFI AP
			#if PERMITE_TROCAR_AP == 1
			if (cmdHTTPfuncao==FCN_CMD_SETUP_AP) {
				#if HAS_EEPROM == 1
				bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
				#endif
				// SSID
				//				if (!strcmp(name,CMD_SSID)) {
				if (name[0]==CMD_SSID_C) {
					strcpy(gParam.web.ssid_ap,var);
					bitSet(gParam.status,P_STATUS_SETUP_AP);
				}
				// PASSWD
				//if (!strcmp(name,CMD_PASSWD)) {
				if (name[0]==CMD_PASSWD_C) {
					urldecode2(gParam.web.passwd_ap,var);

				}
				// CANAL
				//				if (!strcmp(name,CMD_CANAL_TS)) {
				if (name[0]==CMD_CANAL_TS_C) {
					uint8_t i=atoi(var);
					if (i<=14) gParam.web.canal_ap=i;
				}
				// criptografia
				//				if (!strcmp(name,CMD_CRIPTO)) {
				if (name[0]==CMD_CRIPTO_C) {
					uint8_t i=atoi(var);
					if (i<=4) gParam.web.canal_ap=i;
				}
			}
			#endif
			
			// THINGSPEAK
			// SEND TS
			#if PERMITE_TROCAR_SERVER == 1
			if (cmdHTTPfuncao==FCN_CMD_THINGSPEAK) {
				#if HAS_EEPROM == 1
				bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
				#endif
				// TS ADD TP
				//				if (!strcmp(name,CMD_ADDR_IP_TS)) {
				if (name[0]==CMD_ADDR_IP_TS_C) {
					char *dp=strchr(var,':');
					uint8_t idx=1;
					if (dp == NULL) {
						dp=strchr(var,'%');
						idx+=2;
					}
					if (dp != NULL && *var) {
						gParam.web.server_port=atoi(dp+idx);
						*dp='\0';
						strcpy(gParam.web.server_addr,var);
						//	PGM_PRINT("proxy=");
						//	debugln(gParam.web.http_proxy);
					} else {
						gParam.web.server_port=80;
						strcpy(gParam.web.server_addr,TSSERVER_ADDR_IP);
					}
				}
				// TOKEN TS
				//				if (!strcmp(name,CMD_TOKEN_TS)) {
				if (name[0]==CMD_TOKEN_TS_C) {
					strcpy(gParam.web.apy_key,var);
				}
				
				// FIELD CORRENTE TS
				#if USE_TS_FIELD == 1
				if (!strcmp(name,CMD_FIELD_CORRENTE_TS)) {
					if (var[0] >='1' && var[0] <='6')
					gParam.web.field_corrente_ts=var[0]-'0';
				}
				#endif
			}
			#endif
			
			// FASES
			#if PERMITE_TROCAR_FASES == 1
			if (cmdHTTPfuncao==FCN_CMD_FASES) {
				#if HAS_EEPROM == 1
				bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
				#endif
				//				if (!strcmp(name,CMD_FASES_NRO)) {
				if (name[0]==CMD_FASES_NRO_C) {
					uint8_t x = atoi(var);
					//		if (x & B_FASE) gParam.rede.fases = constrain(x,0,7) | (gParam.rede.fases & B_TENSAO);
					gParam.rede.fases = x & 0b1111;
				}

				// TENSAO 127/220
				//				if (!strcmp(name,CMD_FASES_TENSAO)) {
				if (name[0]==CMD_FASES_TENSAO_C) {
					uint8_t x = atoi(var);
					if (x==1 || x==127)	bitSet(gParam.rede.fases,B_TENSAO);
					//if (x==2 || x==220)
					else bitClear(gParam.rede.fases,B_TENSAO);
				}

			}
			#endif
			
			// RESET REBOOT
			if (cmdHTTPfuncao==FCN_CMD_RESET) {
				bitSet(gParam.status,P_STATUS_RESET);
				page=PAGINA_REBOOTING;
			}
			// RESET_FW
			if (cmdHTTPfuncao==FCN_CMD_RESET_FIRMWARE) {
				bitSet(gParam.status,P_STATUS_RESET_FIRMWARE);
				page=PAGINA_REBOOTING;
			}
			
		} while (p != NULL);
		
	#if HAS_EEPROM == 1
	if (bitRead(gParam.status,P_STATUS_NO_AUTH)) {
		PGM_PRINTLN("NO_AUTH");
		bitClear(gParam.status,P_STATUS_GRAVA_EEPROM);
		bitClear(gParam.status,P_STATUS_RESET_FIRMWARE);
		bitClear(flagHTTP,F_SETUP_WIFI);
		page=PAGINA_SETUP;
	}
	#endif
	esp8266Flush();
	PGM_PRINTLN("<esp8266HTTPreq")
}

#endif // HAS_WEBSERVER_FULL

#if HAS_ESP8266 == 1 || HAS_GSM_TCP == 1
uint8_t sendData(char *command, const int timeout, char *target)
{
	uint8_t ret;
	#if DEBUG > 0 || HAS_BLUETOOTH >= 1
	SSdebug.println(command);
	#endif
	SSwifi.print(command); // send the read character to the SSwifi
	SSwifi.print("\r\n");
	SSwifi.flush();
	#if DEBUG >0 || HAS_BLUETOOTH >= 1
	bitSet(ccStatus,CC_DEBUG);
	#endif
	if (target == NULL)
	ret=recvFind("OK",timeout);
	//recvString(ret, "OK", "no ch", NULL, timeout,true);
	else
	ret=recvFind(target,timeout);
	//recvString(ret, target , NULL, NULL, timeout,true);
	#if DEBUG >0 || HAS_BLUETOOTH >= 1
	if (ret) {
		SSdebug.println("OK");
		} else {
		SSdebug.println("Err");
	}
	#endif
	esp8266Flush();
	return ret;
}
////////////////////////////////////////////////////////
/**
 * \breif inicia configuração do wifi
 * \param reset  0 - inicialização normal, sem restore do fw do esp8266
 *				1 - com reset de fw do esp8266
 *				2 - conecta em outro roterador como wifi
 **/
void setup_ESP8266(uint8_t reset)
{
	int i;
	uint8_t flag_cwmode;
	led1_status=LED_PISCA_CURTO;
	led2_status=LED_LIGAR;
	//PGM_PRINTLN(">setup_ESP");
	if (reset <2) {
	//	SSwifi.begin(38400); // your esp's baud rate might be different
	SSwifi.begin(115200); // your esp's baud rate might be different
	delay(400);
	SSwifi.print("AT+RST\r\n");
	delay(700);
		sendData("AT+RESTORE",1000,NULL); // configure as access point
		//sendData("AT+RST",1000,"ready"); // reset module
	i=recvFind("ready",4000);
	if (!sendData("ATE0",1000,NULL)) // testa retorno do ESP, se ERR retorna
	{
		#if HAS_OLED == 1
		oled.mostraHighBar("Erro wifi");
		return;
		#endif
	}
	sendData("AT+RFPOWER=82",1000,NULL); // configure as access point
	//sendData("AT+GMR",1000,NULL);
	
	sendData("AT+CWMODE=3",1000,NULL); // configure as access point
	sendData("AT+CWDHCP=2,1",1000,NULL); // configure as access point
	eATMAC();

	testar_conexao_wifi();
	if (!gParam.web.ssid_ap[0]) {
		sprintf(gParam.web.ssid_ap,"SOL_%s",gParam.web.mac+6);
		strcat(gParam.web.passwd_ap,gParam.web.mac+6);
	}
	eATCWSAP();

    } // end if reset

	flag_cwmode=gParam.web.ssid[0];
	if (flag_cwmode && reset<=2) {
		i=2;
		bitSet(ccStatus,CC_HAS_SSID);
		} else {
		i=0;
	}
	sendData("AT+CWQAP",1000,NULL); // configure as access point
	connectCodeWifi=5;
	while (i--) {
		connectCodeWifi=eATCWJAP();
		if (!connectCodeWifi) { 
			led2_status=LED_APAGA;
			eATCWSAP_DEF(reqHTTP);
			break;
		}
		if (connectCodeWifi<=5)
		delay(300);
	}
	debugbinln(ccStatus);
	esp8266Flush();
	#if USE_SSL == 1
	sendData("AT+CIPSSLSIZE=4096",1000,NULL); // configure as access point
	#endif
	eATCIFSR();   // pega IP
	debugbinln(ccStatus);
	led1_status=LED_APAGA;
}

/////////////////////////////////////////////////////////////////////////////
// Get 1 line
//   modo: 0 espera +IPD,
//   modo: 1 le uma linha ate \n
/////////////////////////////////////////////////////////////////////////////
int recvPkg(uint8_t modo, uint8_t *buffer, uint16_t buffer_size, uint32_t timeout, uint8_t *connectionId)
{
	char c;
	uint16_t ret,i;
	unsigned long start;
	boolean flag_break=false;
	#if DEBUG > 0 || HAS_BLUETOOTH >= 1
	char p[50],ip=0;
	#endif
	//debugvar(modo);
	//debugvarln(freeRAM());
	buffer[0]='\0';
	start = millis();
	while (!modo && !flag_break && millis() - start < timeout) {
		wifi_len=0;
		//if (SSwifi.find("+IPD,")) 
	//bitSet(ccStatus,CC_DEBUG);
		if (recvFind("+IPD,",1000))
		{
			delay(50);
			
			*connectionId = c=SSwifi.read()-48; // subtract 48 because the read() function returns
			// the ASCII decimal value and 0 (the first decimal number) starts at 48
			
			#if DEBUG > 0 || HAS_BLUETOOTH >= 1
			p[ip++]=c;
			#endif

			c=SSwifi.read(); // pula ,
			#if DEBUG > 0|| HAS_BLUETOOTH >= 1
			p[ip++]=c;
			p[ip++]='#';
			#endif

			// tratamento do AT+CIPINFO=1
			//while ((c=SSwifi.read()) != ':' && c!=',') {
			while (isdigit(c=SSwifi.read())) {
				wifi_len = wifi_len*10+c-'0';
			//c=SSwifi.read(); // pula ,
			#if DEBUG > 0|| HAS_BLUETOOTH >= 1
			if (ip<50) p[ip++]=c;
			#endif
			}
			//p[ip++]=c;
			while (c!=':') {
				c=SSwifi.read();
			#if DEBUG > 0|| HAS_BLUETOOTH >= 1
			if (ip<50) p[ip++]=c;
			if (ip<50) p[ip++]='$';
			#endif
			}
			debugvarln(wifi_len);
			flag_break=true;
		}
	}
	#if DEBUG > 0|| HAS_BLUETOOTH >= 1
	p[ip]=0;
	SSdebug.println(p);
	#endif	
	ret=0;
	bitClear(ccStatus,CC_MATA_BUFFER);
	if (wifi_len) {
		ret = wifi_len > buffer_size ? buffer_size : wifi_len;
		ret--;
		i=c=0;
		while (millis() - start < 4000 && i<ret) {
			while(SSwifi.available() > 0 && i<ret) {
				c=SSwifi.read();
				if (c=='\n') {
					ret=0;
					break;
				}
				if (!i && c=='C') bitSet(ccStatus,CC_MATA_BUFFER);
				buffer[i++] = c;
				#if DEBUG > 10
				SSdebug.write(c);
				#endif
				start=millis();
			}
		}
		buffer[i]='\0';
		wifi_len -= i;
	}
	if (bitRead(ccStatus, CC_MATA_BUFFER)) {
		esp8266Flush();
	}
	return i;
}
//////////////////////////////////////////////////////////////////////////////
char *recvString(char *ret, char * target1, char *target2, char *target3, uint32_t timeout, boolean flag_flush)
{
	char a;
	uint8_t id1=0;
	uint8_t id2=0;
	uint8_t id3=0;
	uint8_t len1=strlen(target1);
	uint8_t len2=strlen(target2);
	uint8_t len3=strlen(target3);
	unsigned long start = millis();
	#if DEBUG >= 1 || HAS_BLUETOOTH >= 1
	char p[40],ip=0;
	#endif
	ret[0]='\0';
	//	PGM_PRINTLN(">recvString");
	//	SSwifi._rx_buffer_head = SSwifi._rx_buffer_tail;
	while (!ret[0] && millis() - start < timeout) {
		#if HAS_SENSOR_SECO == 1
		if (seco_int) {
			parse_seco(false);
		}
		#endif
		while(SSwifi.available() > 0) {
			a = SSwifi.read();
			#if DEBUG >= 1|| HAS_BLUETOOTH >= 1
			p[ip]=a;
			ip=++ip % sizeof(p);
			#endif
			//			if(a == '\0') continue;
			if (a==target1[id1]) {
				id1++;
				if (id1==len1) {
					strcpy(ret,target1);
					break;
				}
			}
			else id1=0;
			// target2
			if (target2 != NULL && a==target2[id2]) {
				id2++;
				if (id2==len2) {
					strcpy(ret,target2);
					break;
				}
			}
			else id2=0;
			// target3
			if (target3 != NULL && a==target3[id3]) {
				id3++;
				if (id3==len3) {
					strcpy(ret,target3);
					break;
				}
			}
			else id3=0;
			start=millis();
		}
		delay(5);
	}
	//PGM_PRINTLN("<recvString");
	#if DEBUG > 1|| HAS_BLUETOOTH >= 1
	p[ip]=0;
	if (bitRead(ccStatus,CC_DEBUG)) {
			debugvarln(p);
	}
	bitClear(ccStatus,CC_DEBUG);
	#endif
	if (flag_flush) esp8266Flush();
	return ret;
}
/*******************************************************************/
boolean recvFind(char *target, uint32_t timeout)
{
	char achou[8];
	char *busy="busy";
	uint8_t cnt=3;

	do {
		recvString(achou, target, "link", busy, timeout, false);
		cnt--;
	} while (!strcmp(achou,busy) && cnt);

	return !strcmp(target,achou);
}
/////////////////////////////////////////////////////////////////////////////
#if USE_SSL == 1
const char str_CIPSTART[] PROGMEM = "AT+CIPSTART=%d,\"SSL\",\"%s\",%d,5\r\n";
#else
const char str_CIPSTART[] PROGMEM = "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,5\r\n";
#endif
boolean sATCIPSTARTMultiple(uint8_t mux_id)
{
//	static uint8_t t_reboot=MAX_T_REBOOT;
	char *data=reqHTTP;

	
	esp8266Flush();

	if (gParam.web.http_port) {
		sprintf_P(data,str_CIPSTART,mux_id,gParam.web.http_proxy,gParam.web.http_port);
	} else {
		sprintf_P(data,str_CIPSTART,mux_id,gParam.web.server_addr,gParam.web.server_port);
	}
	#if DEBUG > 0 || HAS_BLUETOOTH >= 1
	SSdebug.println(data);
	#endif
	SSwifi.print(data);

	SSwifi.flush();
	delay(50);
	#if DEBUG > 0
	bitSet(ccStatus,CC_DEBUG);
	#endif
	recvString(data,"OK", "ERR", "ALR", 10000,false); // ALREADY ou Linked
	#if DEBUG > 0 || HAS_BLUETOOTH >=2
	SSdebug.println(data);
	while (SSwifi.available() > 0)
		{
			char c=SSwifi.read();
			SSdebug.write(c);
		}
	#endif

	if (!strncmp(data,"OK",2) || !strncmp(data,"ALR",3)) {
		bitSet(ccStatus,CC_WIFI_INTERNET);
		led2_status=LED_APAGA;
	//	t_reboot=MAX_T_REBOOT;
		return true;
	}
	//	if (!strncmp(data,"ready",5)   || !strncmp(data,"Link typ",8)) {
	//	bitSet(ccStatus,CC_WIFI_ERROR);
	bitClear(ccStatus,CC_WIFI_INTERNET);
	led2_status=LED_PISCA_LONGO; // sem conexao ao roteador TAG_2
	//if (!t_reboot--) bitSet(gParam.status,P_STATUS_RESET);
	//	}
	//debugvarln(t_reboot);
	//ESP8266reset(true);
	sATCIPCLOSEMulitple(5);
	return false;
}
//////////////////////////////////////////////////////////////////////
boolean sATCIPCLOSEMulitple(uint8_t mux_id)
{
	char *data=reqHTTP;
	//eATCIPSTATUS();
	PGM_PRINTLN("CLO");
	//debugln(mux_id);
	esp8266Flush();
	#if USE_MUX == 1
	SSwifi.print("AT+CIPCLOSE=");
	SSwifi.print(mux_id);
	SSwifi.print("\r\n");
	#else
	SSwifi.print("AT+CIPCLOSE\r\n");
	#endif
	//bitSet(ccStatus,CC_DEBUG);
	#if ESP8266_VERSION_14 == 1
	recvString(data,"OK","xx","CLOSE", 10000,true);
	#else
	recvString(data,"OK","ERR","UNL", 10000,true);
	#endif
	//debugln(data);
	if (data[0]=='O' || data[0]=='U') {
		return true;
	}
	return false;
}

/////
// tipo_buffer= 0 RAM
//            = 1 PROGMEM
/////
boolean sATCIPSENDMultiple(int mux_id, uint8_t tipo_buffer, uint8_t *buffer, boolean flag_flush)
{
	char str_ret[8];
	int len;
	//debugvar(freeMemory());
	if (tipo_buffer == POINTER_CHAR_RAM)
	len=strlen((char*)buffer);
	else
	len=strlen_P((char*)buffer);

	if (!len) return true;
	
	///	rx_empty();
	//if (bitRead(ccStatus,CC_DEBUG))	debugvar(freeMemory());
	#if DEBUG >= 1 || HAS_BLUETOOTH >= 1
	SSdebug.println(len);
	bitSet(ccStatus,CC_DEBUG);
	if (1||bitRead(ccStatus,CC_DEBUG)) {
		uint8_t *b=buffer;
		for (uint16_t i = 0; i < len; i++) {
			if (tipo_buffer==POINTER_CHAR_PROGMEM) {
				SSdebug.write(pgm_read_byte(b++));
				} else {
				SSdebug.write(*b++);
			}
		} // for
	}

	#endif
//	esp8266Flush();
	SSwifi.print("AT+CIPSEND=");
	#if USE_MUX == 1
	SSwifi.print(mux_id);
	SSwifi.print(",");
	#endif
	SSwifi.print(len);
	SSwifi.print("\r\n");
	SSwifi.flush();
	if (recvFind(">",4000)) {
		PGM_PRINT("+");
		//esp8266Flush();
#if 0
		if (tipo_buffer==POINTER_CHAR_PROGMEM) {
		#if 0
			char *u;
			if (len < SERIAL_TX_BUFFER_SIZE-1 && (u=(char*)malloc(SERIAL_TX_BUFFER_SIZE))) {
				strcpy_P(u,(char*)buffer);
				SSwifi.print(u);
				SSwifi.flush();
				free(u);
			} else
		#endif
				for (uint16_t i = 0; i < len; i++) {
					// SSdebug.write(pgm_read_byte(buffer));
					SSwifi.write(pgm_read_byte(buffer++));
				}
		}else {
				SSwifi.print((char*)buffer);
		}
#else
		for (uint16_t i = 0; i < len; i++) {
			if (tipo_buffer==POINTER_CHAR_PROGMEM) {
				//			SSdebug.write(pgm_read_byte(buffer));
				SSwifi.write(pgm_read_byte(buffer++));
				}else {
				//				SSdebug.write(*buffer);
				SSwifi.write(*buffer++);
			}
		}
#endif
		//		ret=recvFind("SEND OK", 5000);
		PGM_PRINT("* ");
		recvString(str_ret, "OK", "busy","link is", 10000,flag_flush);
		#if DEBUG > 0
		//if (bitRead(ccStatus,CC_DEBUG)) 
			debugln(str_ret);
		#endif
		if (!strcmp(str_ret,"OK")) {
			//			PGM_PRINTLN("SOK");
			//delay(100);
			//SSwifi.flush();
			//eATCIPSTATUS();

			return true;
		}
		if (!strncmp(str_ret,"link",4)) {
			sATCIPCLOSEMulitple(mux_id);
		}
	}
	return false;
}

///////////////////////////////////////////////////////////
// TEsta conexao
///////////////////////////////////////////////////////////
int testar_conexao_wifi(void)
{
	uint8_t mux=0;
	uint16_t EEstatus;
	
	//	PGM_PRINT("t");
	mux=getCIPMUX;
		debugvar(mux);
	
	if (!mux || (ccStatus & CC_WIFI_BITS_AP_OK) != CC_WIFI_BITS_AP_OK) {
		if (sendData("AT+CIPMUX=1",3000,NULL))
		bitSet(ccStatus,CC_CIPMUX); // configure for multiple connections
		else
		bitClear(ccStatus,CC_CIPMUX); // configure for multiple connections
		if (sendData("AT+CIPSERVER=1,80",10000,NULL)) {// turn on server on port 80
			bitSet(ccStatus,CC_CIPSERVER);
			sendData("AT+CIPSTO=60",2000,NULL);
		} else
		bitClear(ccStatus,CC_CIPSERVER);
	}
	
	if ((ccStatus & CC_WIFI_BITS_OK) == CC_WIFI_BITS_OK) {
		bitSet(ccStatus,CC_WIFI_OK);
		led2_status=LED_APAGA;
	} else {
		bitClear(ccStatus,CC_WIFI_OK);
//		if ((ccStatus & CC_WIFI_BITS_AP_OK) == CC_WIFI_BITS_AP_OK) {
//			led2_status=LED_PISCA_CURTO; // com conexao ao roteador
//		}
	}
	
	return bitRead(ccStatus,CC_WIFI_OK);
	
	//debugbinln(ccStatus);
	
	//	PGM_PRINT("ccStatus=");
	//	debugbinln(ccStatus);
}
///////////////////////////////////////////////////////////
// cmd = 0 CWMODE
//       1 CIPMUX
//		 2 CWLIF copiando para reqHTTP
///////////////////////////////////////////////////////////
#if 1
char eATCWMODE(uint8_t cmd)
{
	char st[11];
	char *r=NULL;
	char flag_lf=false;
	uint8_t j=0,i=0,nl=0;
	boolean flag=false;
	#if DEBUG > 5
	char p[50],xp=0;
	PGM_PRINT(">eATCWMODE=");
	debugln(cmd);
	#endif
	switch (cmd) {
		case 1:
		SSwifi.print("AT+CIPMUX?\r\n");
		break;
		case 0:
		SSwifi.print("AT+CWMODE?\r\n");
		break;
		case 2:
		r=reqHTTP;
		*r='\0';
		SSwifi.print("AT+CWLIF\r\n");
		break;
	}
	
	delay(50);
	st[10]='\0';
	while (SSwifi.available() > 0) {
		char c=SSwifi.read();
		#if DEBUG > 5
		p[xp]=c;
		xp=++xp % sizeof(p);
		#endif
		if (cmd >= 2) { // CWLIF list clientes AP
			if (isdigit(c)) flag=true;
		}
		if (c=='\n') {
			nl++;
			flag=false;
			if (flag_lf)
				r=NULL;
			else {
				if (r) strcpy_P(r,PSTR("<br>")); // pular de linha ao apresentar os clientesAP conectados
			}
			flag_lf=true;
		}
		else {
			flag_lf=false;
			if (r) {
				*r++=c;
				*r='\0';
			}
			if (nl==1 && i<10)
				st[i++]=c;
		}
	}
	st[i]='\0';
	if (cmd <= 1) {
		if (!strncmp(st,"busy",4)) bitSet(gParam.status,P_STATUS_RESET_ESP8266);
		i=st[8]-'0';
		if (i<1 || i>3) i=0;
	}
	#if DEBUG > 5
	debugvarln(st);
	p[xp]=0;
	debugvarln(p);
	#endif
	return i;
}
#endif

///////////////////////////////////////////////////////////
uint8_t eATCWJAP_(void)
{
	//char achou[50];
	uint8_t ret=0;
	esp8266Flush();
	strcpy_P(reqHTTP,PSTR("AT+CWJAP=\""));
	strcat(reqHTTP,gParam.web.ssid);
	strcat(reqHTTP,"\",\"");
	strcat(reqHTTP,gParam.web.passwd);
	strcat(reqHTTP,"\"\r\n");
	ret=sendData(reqHTTP,20000,"GOT");
	if (ret && recvFind("OK",5000)) {
			//led2_status=LED_PISCA_FLASH;
		//delay(500);
		bitSet(ccStatus,CC_CWJAP);
		ret=1;
		eATCWSAP_DEF(reqHTTP);
	} else {
		bitClear(ccStatus,CC_CWJAP);
		PGM_PRINTLN("NoWifi");
	}
	esp8266Flush();
	return ret;
}
///////////////////////////////////////////////////////////
uint8_t eATCWJAP(void)
{
	char achou[10];
	uint8_t i;
	uint8_t errCode=0; // 0=ok, 1=connect timeout 2=wrong pass 3=AP not found 4=connect fail 5=nao conectou
	esp8266Flush();
	strcpy_P(reqHTTP,PSTR("AT+CWJAP=\""));
	strcat(reqHTTP,gParam.web.ssid);
	strcat(reqHTTP,"\",\"");
	strcat(reqHTTP,gParam.web.passwd);
	strcat(reqHTTP,"\"\r\n");
	SSwifi.print(reqHTTP);
	SSwifi.flush();
	//ret=sendData(reqHTTP,20000,"GOT");
	//if (ret && recvFind("OK",5000)) {
	if (recvString(achou, "GOT", "JAP:",NULL,60000, false)) {
	#if DEBUG >0 || HAS_BLUETOOTH >=2
		SSdebug.print("achou=");
		SSdebug.println(achou);
	#endif
		if (!strcmp(achou,"GOT")) {
			bitSet(ccStatus,CC_CWJAP);
		} else {
			i=0;
			while (SSwifi.available()) {
				char c=SSwifi.read();
				if (!errCode) errCode=c-'0';
				reqHTTP[i++]=c;
			}
			reqHTTP[i]=0;
		}
	} else {
		bitClear(ccStatus,CC_CWJAP);
		errCode=5;
		PGM_PRINTLN("NoWifi");
	}
	esp8266Flush();
	#if HAS_BLUETOOTH >= 1
	SSdebug.write('-');
	SSdebug.println(errCode);
	#endif
	return errCode;
}
///////////////////////////////////////////////////////////
void eATCWSAP_DEF(char *status)
{
	uint8_t i=0, is=0, virgula=3; // "HOME","00:1e:58:c3:ee:c0",6,-76
	if (connectCodeWifi) return;
	SSwifi.print("AT+CWJAP?\r\n");
	delay(100);
	if (recvFind("JAP:",5000)) {
		while (SSwifi.available() > 0) {
			char c=SSwifi.read();
			
			if (c=='\n'||c=='\r') break;
			if (!virgula)
				gParam.web.rssi[is++]=c;
			if (c==',')	virgula--;
			status[i++]=c;
			if (i>50) i=0;
		}

	} 
//	else SSdebug.println("###");
	if (is>4)
		is=0;
	gParam.web.rssi[is]='\0';
	status[i]='\0';
	//debugvarln(status);
	esp8266Flush();
}
///////////////////////////////////////////////////////////
char eATCIPSTATUS(void)
{
	char *status=reqHTTP;
	uint8_t i=0,nl=0;
	SSwifi.print("AT+CIPSTATUS\r\n");
	delay(50);
	while (SSwifi.available() > 0) {
		char c=SSwifi.read();
		//SSdebug.write(c);
		if (c=='\n'||c=='\r') nl++;
		if (nl>2 && i<sizeof(reqHTTP))
		status[i++]=c;
	}
	status[i]='\0';
	//debugvarln(status);
	return status[7]-'0';
}

///////////////////////////////////////////////////////////
const char str_CWSAP[] PROGMEM = "AT+CWSAP_CUR=\"%s\",\"%s\",%d,%d\r\n";
void eATCWSAP(void)
{
	uint8_t ret;
	sprintf_P(reqHTTP,str_CWSAP,gParam.web.ssid_ap,gParam.web.passwd_ap, 
			gParam.web.canal_ap, gParam.web.cripto_ap);
	SSwifi.print(reqHTTP);
	ret=sendData(reqHTTP,5000,"OK");
}

////////////////////////////////////////////////////////////////
void eATCIFSR_in(uint8_t idx)
{
	uint8_t k=0, flag, sair=false;
	char ip[17];
	#if DEBUG > 5
	char p[50],xp=0;
	#endif
	unsigned long time=millis();
	if (idx) {
		strcpy_P(ip,PSTR("AT+CIPSTA?\r\n"));
	} else {
		strcpy_P(ip,PSTR("AT+CIPAP?\r\n"));
	}
	
	//strcpy_P(ip,PSTR("AT+CIFSR?\r\n"));
	SSwifi.print(ip); // get ip address AP
	SSwifi.flush();
	if (recvFind(":",2000)) {

	flag=false;

	while (millis()-time < 2000 && !sair) {
		while (SSwifi.available() > 0) {
			char c=SSwifi.read();
			#if DEBUG > 5
			p[xp]=c;
			xp=++xp % sizeof(p);
			#endif
			time=millis();
			if (flag && k<sizeof(ip)-1 && (isdigit(c) || c=='.')) {
				ip[k++]=c;
				ip[k]=0;
			}
			if (c=='\"')
			{
				if (!flag) {
					flag=true;
					}	else {
						sair=true;
						break;
					}
				}
			}
		} // while
	} // while time
	esp8266Flush();
	#if DEBUG > 5
	p[xp]=0;
	debugvarln(p);
	#endif
	if (isdigit(ip[0])) {
		memcpy(gParam.web.ip[idx],ip,16);
	}
	debugln(ip);
	/*
	if (ip[0] > '0' && ip[0] <= '2') {
		bitSet(ccStatus,CC_CWJAP);
		}  else {
		bitClear(ccStatus,CC_CWJAP);
	}
	*/
}
/////////////
void eATCIFSR(void)
{
  char *ip=gParam.web.ip[1];
  *ip='\0';
  eATCIFSR_in(0);
  if (!connectCodeWifi) {
	eATCIFSR_in(1);
  }
	if (*ip > '0' && *ip <= '2') {
		bitSet(ccStatus,CC_CWJAP);
	}  else {
		bitClear(ccStatus,CC_CWJAP);
	}
 }
///////////////////////////////////////////////////////////
/// Pegar IP
///////////////////////////////////////////////////////////
void eATCIFSR_(void)
{
	uint8_t k=0,x=0, flag;
	char ip[2][17];
	#if DEBUG > 5
	char p[50],xp=0;
	#endif
	unsigned long time=millis();
	#if ESP8266_VERSION_0922 == 1
	SSwifi.print("AT+CIFSR\r\n"); // get ip address AP
	#elif ESP8266_VERSION_0952 == 1 || ESP8266_VERSION_14 == 1
	SSwifi.print("AT+CIPAP?\r\n"); // get ip address AP
	#else
	return;
	#endif
	flag=false;
	SSwifi.flush();
	while (millis()-time < 2000 && x<2) {
		while (SSwifi.available() > 0) {
			char c=SSwifi.read();
			#if DEBUG > 5
			p[xp]=c;
			xp=++xp % sizeof(p);
			#endif
			time=millis();
			#if ESP8266_VERSION_0922 == 1
			if (flag && c=='\n') {
				flag=false; x++;k=0;
			}
			if (isdigit(c)) { flag=true; }
			#endif
			
			if (flag && k<sizeof(ip[0])-1 && (isdigit(c) || c=='.')) {
				ip[x][k++]=c;
				ip[x][k]=0;
			}
			#if ESP8266_VERSION_0952 == 1 || ESP8266_VERSION_14 == 1
			if (c=='\"')
			{
				if (!flag) {
					flag=true;
					}	else {
					char ret[10];
					recvString(ret, "OK", "no change", NULL, 1000,true);
					SSwifi.print("AT+CIPSTA?\r\n"); // get ip address client
					//delay(50);
					k=0;
					x++;
					flag=false;
				}
			}
			#endif
		} // while
	} // while time
	esp8266Flush();
	#if DEBUG > 5
	p[xp]=0;
	debugvarln(p);
	#endif
	if (x) {
		memcpy(gParam.web.ip,ip,32);
		
	}
	debugln(ip[0]);
	debugln(ip[1]);
	#if HAS_DISPLAY == 1
	// col,lin
	lcd.setCursor(15,1);
	#endif
	if (ip[1][0] > '0' && ip[1][0] <= '2') {
		bitSet(ccStatus,CC_CWJAP);
		#if HAS_DISPLAY == 1
		lcd.print("0");
		#endif
		}  else {
		bitClear(ccStatus,CC_CWJAP);
		#if HAS_DISPLAY == 1
		lcd.print("X");
		#endif
	}
}
///////////////////////////////////////////////////////////
/// Pegar MAC
///////////////////////////////////////////////////////////
static void eATMAC(void)
{
	uint8_t k=0;
	uint8_t flag;
	char mac[13];
	#if DEBUG > 5
	char p[50],xp=0;
	#endif
	unsigned long time=millis();
	SSwifi.print("AT+CIPSTAMAC?\r\n"); // get ip address AP
	flag=false;
	SSwifi.flush();
	while (millis()-time < 2000 && k<12) {
		while (SSwifi.available() > 0) {
			char c=SSwifi.read();
			#if DEBUG > 5
			p[xp]=c;
			xp=++xp % sizeof(p);
			#endif
			time=millis();

			if (!flag) {
				if (c!='"') continue;
				flag=true;
				continue;
			}
			if (isalnum(c)) {
				mac[k++]=c;
				mac[k]='\0';
			}
		} // while
	} // while time
	esp8266Flush();
	PGM_PRINT("MAC=");
	debugln(mac);
	#if DEBUG > 5
	p[xp]=0;
	debugvarln(p);
	#endif
	gParam.web.mac[0]='\0';
	if (k==12)
		strncpy(gParam.web.mac,mac,13);
}
///////////////////////////////////////////////////////////
/// Scan Wifi Serial
///////////////////////////////////////////////////////////
void eATCWLAP(void)
{
	uint8_t x=0, flag, i=0, stop=false;
	char ip[10][20], c;
	
	unsigned long lap,time=millis();
	PGM_PRINTLN("LAP");
	SSwifi.print("AT+CWLAP\r\n"); // get ip address AP
	flag=0;
	SSwifi.flush();
	lap=millis();
	while (millis()-time < 10000 && !stop && x<10) {
		while (SSwifi.available() > 0 && !stop && x<10) {
			c=SSwifi.read();
			if (isalnum(c))
			reqHTTP[i++] = c;
			time=millis();
			/*	if (!flag && old_c=='O' && c=='K')	stop=true;
			
			if (c=='\n') {
			flag=0; x++;k=0;
			}
			if (flag>=2) { flag=0; }
			if (flag && k<19) {
			ip[x][k++]=c;
			ip[x][k]=0;
			}
			if (c==',') { flag++; }
			old_c=c; */
		} // while
	} // while time
	lap=millis()-lap;
	reqHTTP[i]='\0';
	debugvarln(lap);
	esp8266Flush();
	for (i=0; i<x; i++)
	debugln(ip[i]);
	debugvarln(reqHTTP);
	
}
///////////////////////////////////////////////////
// RESET ESP
//////////////////////////////////////////////////
void ESP8266reset(uint8_t reset) {
	#if HAS_BLUETOOTH > 0 || DEBUG > 0
		SSdebug.println("$");
	#endif
	#if HAS_DISPLAY == 1
	// col,lin
	lcd.setCursor(15,1);
	lcd.print("?");
	#endif

	#if HAS_ESP9266_RESET == 1
	digitalWrite(ESP8266_RST_PIN,LOW);
	delay(150);
	digitalWrite(ESP8266_RST_PIN,HIGH);
	#endif
	//if (reset==2)
	  // setup_ESP8266(reset);
}


#endif
void esp8266Flush(void) {
	unsigned long t=millis();
	char c;
	#if DEBUG > 0
	SSdebug.write('F');
	#endif
	while (millis()-t < 50) {
		//		UIPEthernet.maintain();
		while (SSwifi.available() > 0)
		{
			c=SSwifi.read();
			#if DEBUG > 0 || HAS_BLUETOOTH >= 1
			SSdebug.write(c);
			#endif
			t=millis();
		}
	}

}

#endif
