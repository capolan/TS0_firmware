#include "TSensor.h"

#if HAS_GSM_TCP == 1
// PIN Number
#define PINNUMBER "3636"
#include "GPRSInterface-master/GPRSInterface.h"
#include "GPRSInterface-master/GPRS.h"

// APN data
#define GPRS_APN       "claro.com.br" // replace your GPRS APN
#define GPRS_LOGIN     "claro"    // replace with your GPRS login
#define GPRS_PASSWORD  "claro" // replace with your GPRS password

GPRSInterface gprs(1,0,9600,GPRS_APN,GPRS_LOGIN,GPRS_PASSWORD);

void setup_gsm(uint8_t max)
{
	int i;
	uint8_t ret;
	char *p;
	led1_status=LED_PISCA_CURTO;
	led2_status=LED_LIGAR;
	PGM_PRINTLN(">setup_gsm ");
	//	SSwifi.begin(38400); // your esp's baud rate might be different
	gprs.init(max);
	PGM_PRINTLN(" ready ");
	ret=gprs.gprsReady(3);
	if (ret) return;
	// registrou cartão SIM
	// registrou GPRS
	bitSet(ccStatus,CC_LINK_GSM);
	PGM_PRINTLN(" connect ");

	ret=gprs.connect();
	debugln(ret);
	if (!ret) return;

	p=gprs.getIPAddress();

	debugvarln(p);
	ccStatus |= CC_WIFI_BITS_CWJAP_OK;

	//getIP();   // MACRO
	led1_status=LED_APAGA;

	#if HAS_WEBSERVER_FULL == 1 || HAS_WEBSERVER_TINY == 1
	server.begin();
	#endif
}


///////////////////////////////////////////////////////////////////////////
void parse_server(void)
{
}
///////////////////////////////////////////////////////////////////////////
void parse_gsm(void)
{
	#if HAS_WEBSERVER_FULL == 1
	parse_ESP8266();
	#endif
	#if HAS_WEBSERVER_TINY == 1
	parse_server();
	#endif
	
}

TCPSocketConnection sock;

boolean sATCIPSTARTMultiple(uint8_t mux_id, char *addr, uint16_t port,char *http_proxy, uint16_t proxy_port)
{
	static uint8_t t_reboot=MAX_T_REBOOT;
	byte ip[4];
	uint8_t i;
	boolean conn;
	char *q,*p=addr;
	for (i=0; i<4; i++) {
		q=strchr(p,'.');
		ip[i]=atoi(p);
		debugvar(ip[i]);
		p=q+1;
	}
	conn=sock.connect(addr,port);
	bitWrite(ccStatus,CC_WIFI_INTERNET,conn);
	debugvarln(conn);
	return conn;
}

boolean sATCIPCLOSEMulitple(uint8_t mux_id)
{
	sock.close();
	return true;
}
boolean sATCIPSENDMultiple(int mux_id, uint8_t tipo_buffer, uint8_t *buffer, boolean flag_flush)
{
	uint16_t len;
	char *buf;

	#if DEBUG>0 //&& defined(__AVR_ATmega2560__)
	if (tipo_buffer == POINTER_CHAR_RAM) {
		len=strlen((char*)buffer);
		} else {
		len=strlen_P((char*)buffer);
	}
	uint8_t *b=buffer;
	if (1||bitRead(ccStatus,CC_DEBUG))
	SSdebug.write('[');
	for (uint16_t i = 0; i < len; i++) {
		if (tipo_buffer==POINTER_CHAR_PROGMEM) {
			SSdebug.write(pgm_read_byte(b++));
			}else {
			SSdebug.write(*b++);
		}
	}
	SSdebug.write(']');
	#endif

	if (tipo_buffer == POINTER_CHAR_RAM) {
		sock.send((char*)buffer,len,DATA);
	} else {
			sock.send((char*)buffer,len,PROG);
	}
	return true;
}

int recvPkg(uint8_t modo, uint8_t *buffer, uint16_t buffer_size, uint32_t timeout, uint8_t *connectionId)
{
	char c;
	uint16_t ret,i;
	unsigned long start;
	boolean flag_break=false;
	
	//debugvarln(freeRAM());
	buffer[0]='\0';
	start = millis();
	ret=0;
	wifi_len=SSwifi.available();
	//debugvarln(wifi_len);
	bitClear(ccStatus,CC_MATA_BUFFER);
	ret = wifi_len > buffer_size ? buffer_size : wifi_len;
	ret--;
	i=c=0;
	while (millis() - start < timeout && i<ret) {
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

	if (bitRead(ccStatus, CC_MATA_BUFFER)) {
		esp8266Flush();
	}
	//debugvar(i);
	//debugln((char*)buffer);
	return i;
}



#endif

