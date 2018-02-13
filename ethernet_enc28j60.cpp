#include "TSensor.h"

#if HAS_ETH == 1
#include <SPI.h>
#include <UIPEthernet.h>
//#include <enc28j60.h>

#include "MemoryFree.h"

// TESTE
//#define TEST_ENC28

#if HAS_WEBSERVER_FULL == 1
EthernetServer server = EthernetServer(80);
#endif
#if HAS_WEBSERVER_TINY == 1
EthernetServer server = EthernetServer(80);
#endif
EthernetClient client;

#if USE_ETH_SMS == 1
struct TSMS gSMS = {
	0,
	MD5,
	"",  // numero
	""  // msg
	};
// s=chave:numero:msg
char processa_sms(char *q) {
	char *p;
	char ret=0;
	uint8_t fase=1; // fase 1=chave, 2=numero, 3=msg
	#if HAS_BLUETOOTH >= 2
	SSdebug.print("sms=");
	SSdebug.println(q);
	#endif
	memset((void*)&gSMS.numero,0,sizeof(TSMS)-6);
	for (p=q;*p && !ret && *q;) {
		q++;
		if (*q == ':' || !*q) {
			switch (fase) {
				case 1:
				if (strncmp(p,gSMS.chave,4)) {
					ret='1'; // chave inválida
				} else {
					p=++q;
					fase++;
				}
				break;
				case 2:
				*q='\0';
				strncpy(gSMS.numero,p,MAX_NRO_SMS);
				for (p=gSMS.numero;isdigit(*p);p++);
				if (*p) {
					ret='2'; // ERRO nro telefone errado
				}
				p=++q;
				fase++;
				break;
				case 3:
				*q='\0';
				strncpy(gSMS.msg,p,MAX_MSG_SMS);
				ret='s';
				break;
			}
		}
	}
	gSMS.status=ret;
	return ret;
}
#endif
//
// http://ww1.microchip.com/downloads/en/DeviceDoc/39662c.pdf
// https://github.com/ntruchsess/arduino_uip/issues/167
//
#ifdef TEST_ENC28
void test_hang_eth (void) {
	static uint32_t nowTime = millis();
    uint8_t mac[6] = {0x00,0x01,0x02,((TSENSOR_SERIE>>8)&0xff)+1,(TSENSOR_SERIE&0xff),0x07};
	uint8_t reg_ECON1=Enc28J60.readReg((uint8_t) ECON1);
	uint8_t reg_EIR=Enc28J60.readReg((uint8_t) EIR);
	uint8_t reg_PKT=Enc28J60.readReg((uint8_t) EPKTCNT);
	#if HAS_BLUETOOTH >=2
	if (reg_PKT) {
		SSdebug.print("PKT=");
		SSdebug.print(reg_PKT);
	}
	#endif
	if ((uint32_t) (millis() - nowTime) > 2000) { // 5 segundos
	#if HAS_BLUETOOTH >=2
	SSdebug.print(" ECON1=");
	SSdebug.print(reg_ECON1);
	SSdebug.print(" EIR=");
	SSdebug.println (reg_EIR);
	#endif
		// Enc28J60 is Enc28J60Network class that defined in Enc28J60Network.h
		// ENC28J60 ignore all incoming packets if ECON1.RXEN is not set
		if  (! (reg_ECON1 & ECON1_RXEN)) {
			#if HAS_BLUETOOTH >=2
			SSdebug.println("REinit");
			#endif
			//Enc28J60.init(mac);
		}
		if (reg_EIR & EIR_RXERIF) {
			#if HAS_BLUETOOTH >=2
			SSdebug.println("RXERIF");
			#endif
			Enc28J60.writeOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_RXERIF);
			//Enc28J60.init(mac);
		}
		nowTime=millis();
	}
}
#endif

void setup_eth(void)
{
//  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
	uint8_t mac[6] = {0x18,0xfe,0x30,((TSENSOR_SERIE>>8)&0xff)+1,(TSENSOR_SERIE&0xff),0x01};

//  uint8_t *mac = gParam.web.mac;
  uint8_t i;
  IPAddress myIP; 
  uint8_t dhcp=bitRead(gParam.status,P_STATUS_USE_DHCP);
  if (bitRead(ccStatus,CC_BOOT)) {
	if (!Enc28J60.linkStatus()) {
		led2_status=LED_PISCA_FLASH;
		return;
	}
  }

  #if DEBUG>0 || HAS_BLUETOOTH >= 2
  SSdebug.println(dhcp);
  SSdebug.write('s');
  SSdebug.println(freeRAM());
  #endif
  led1_status=LED_PISCA_LONGO;
  if (dhcp) {
		if (Ethernet.begin(mac)) {
			PGM_PRINTLN("dhcp ok");
  #if DEBUG==0 && HAS_BLUETOOTH >=1
	SSdebug.write('$');
  #endif
		} else {
			Ethernet.begin(mac,gParam.web.eth,gParam.web.dns,gParam.web.gw,gParam.web.mask);
			PGM_PRINTLN("no dhcp");
		}
  } else {
	    Ethernet.begin(mac,gParam.web.eth,gParam.web.dns,gParam.web.gw,gParam.web.mask);
  }
  ccStatus |= CC_WIFI_BITS_CWJAP_OK;

  i=Enc28J60.linkStatus();
  bitWrite(ccStatus,CC_LINK_ETH,i);
  if (!i)
	bitClear(ccStatus,CC_INIT_ETH);
  #if DEBUG > 0 || HAS_BLUETOOTH > 0
  SSdebug.println(i,BIN);
  #endif
  #if DEBUG > 0 
  SSdebug.println(Ethernet.localIP());
  SSdebug.println(Ethernet.gatewayIP());
  #endif
 //gParam.web.ip[0][0]='\0';
  //gParam.web.ip[1][0]='\0';
  myIP=Ethernet.localIP();
  // acerta endereco MAC
  #if 1
  gParam.web.mac[12]=0;
	for (uint8_t ii=5; ii<6;ii--) {
		i=mac[ii] >> 4;
		i=i>9?i+55:i+'0'; // 'A'(65) - 10 = 55 
		gParam.web.mac[ii*2]=i;
		i=mac[ii] & 0x0f;
		i=i>9?i+55:i+'0';
		gParam.web.mac[ii*2+1]=i;
	}
   #endif
  reqHTTP[0]='\0';
  for (i=0;i<4 && strlen(reqHTTP)<16;i++) {
	itoa(myIP[i],reqHTTP+strlen(reqHTTP),10);
	if (i<3) strcat_P(reqHTTP,PSTR("."));
  }
  memcpy(gParam.web.ip[1],reqHTTP,16);
  #if DEBUG==0 && HAS_BLUETOOTH >=1
  SSdebug.println(gParam.web.ip[1]);
  #endif
  #if HAS_WEBSERVER_FULL == 1 || HAS_WEBSERVER_TINY == 1
  server.begin();
  #endif
  led1_status=LED_APAGA;
  #if DEBUG>0 || HAS_BLUETOOTH >= 1
  SSdebug.write('S');
  SSdebug.println(freeRAM());
  #endif
}


///////////////////////////////////////////////////////////////////////////
void parse_server(void)
{
 size_t size;
 char *p,*q, cmd='=';
 char data[100];
 uint8_t fases=gParam.rede.fases & B_FASE;

 if (client = server.available())
 {
	PGM_PRINT("Web");
	 while(client.available() > 0)
	 {
		 //size = client.read((uint8_t*)reqHTTP,min(MAX_HTTPreq-1,size));
		 size = client.read((uint8_t*)reqHTTP,MAX_HTTPreq-1);
		 esp8266Flush();
		 //Serial.write(msg,size);
		 reqHTTP[size]='\0';
		 debugvar(size);
		 debugvarln(reqHTTP);
#if 1
		 urldecode2(reqHTTP, reqHTTP);
		 if ((p=strchr(reqHTTP,'?'))) {
			q=p+1; // cmd
			 p=strchr(q,' ');
			*p='\0';
	 		 debugvarln(q);
			 cmd=*q;
		 } 

 		q+=2;
		 switch (cmd) {
		 #if USE_ETH_SMS == 1
		 case 'm': // SMS m=chave:numero:msg
		 cmd=processa_sms(q);
		 break;
		 #endif
		 #if TINY_VERSION == 0
		 case 's': // s=<ip_servidor><:porta>
			for (p=gParam.web.server_addr; isdigit(*q) || q=='.' ; q++) {
				*p++=*q;
				if (!*q) break;
			}
			if (*q++ == ':') {   // porta
				gParam.web.server_port=constrain(atoi(q),80,65534);
			}
			*p='\0';
			#if HAS_EEPROM == 1
			bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
			#endif
		 break;
		 #endif
		 case 'i': // I:D:IP:GW:MASK:
				//  http://<ip>?I=1:192.168.0.1:192.168.1.1:255.255.255.0:
				//  http://<ip>?I=1:aabbccddAABBCCDDaabbccdd
			// DHCP
			if (*q=='0') bitClear(gParam.status,P_STATUS_USE_DHCP);
			if (*q=='1') bitSet(gParam.status,P_STATUS_USE_DHCP);
			q+=2;
			p=strchr(q,':');
			if (!p) break;
			*p++='\0';
			debugvarln(q);
			debugvarln(p);
			#if HAS_EEPROM == 1
			bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
			#endif
			// IP
			stoip(gParam.web.eth,q);
			 q=p;
			 p=strchr(p,':');
			 if (p) {
				 *p++='\0';
				// GW
				stoip(gParam.web.gw,q);
				q=p;
				p=strchr(p,':');
				if (*p) {
					*p++='\0';
					// MASK
					stoip(gParam.web.mask,q);
				}
			}
			break;
		 case 'y': // proxy
				//  http://<ip>?P=192.168.1.250:3128:
			 p=strchr(q,':');
			 *p++='\0';
			strcpy(gParam.web.http_proxy,q);
			gParam.web.http_port=atoi(p);
			#if HAS_EEPROM == 1
			bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
			#endif
		 break;
		 case 't':
			timerWeb=0;
			break;
		 case 'O':
			bitSet(gParam.status,P_STATUS_RESET);
			break;   
		 } // case

	// REPONSE
	strcpy_P(reqHTTP,str_HTTP_200);
	client.print(reqHTTP);
	strcpy_P(reqHTTP,str_Content_type);
	strcat_P(reqHTTP,str_Content_type_text);
	//client.print(reqHTTP);
	strcat_P(reqHTTP,str_Content_length);
	client.print(reqHTTP);

	// DADOS
	strcpy_P(data,PSTR("\nD:"));
	data[1]=cmd;
	// status
	utoa(ccStatus,data+strlen(data),2);
	strcat(data,":");
	// serie
	utoa(TSENSOR_SERIE,data+strlen(data),10);
	strcat(data,":");
	//RECURSOS
	ultoa(TSENSOR_RECURSO,data+strlen(data),16);
	strcat(data,":");
	// server
	strcat(data,gParam.web.server_addr);
	strcat(data,":");
	utoa(gParam.web.server_port,data+strlen(data),10);
	strcat(data,":");
	// proxy
	strcat(data,gParam.web.http_proxy);
	strcat(data,":");
	utoa(gParam.web.http_port,data+strlen(data),10);
	strcat(data,":");
	#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
		utoa(gParam.radio.client,data+strlen(data),10);
		strcat(data,":");
	#endif
	 #if HAS_CORRENTE == 1 || HAS_I2C_COPY == 1
	//for (uint8_t bit_fase=0; bit_fase < MAX_CORRENTE_FASES; bit_fase++ ) {
	for (uint8_t bit_fase=0; bit_fase < 3; bit_fase++ ) {
	//	if (bitRead(fases,bit_fase)) {
			utoa(correnteFase[bit_fase],data+strlen(data),10);
			strcat(data,":");
	//	}
	}
	#endif
	#if HAS_TEMPERATURA == 1 || HAS_I2C_COPY == 1
	itoa(pTemp->temperatura,data+strlen(data),10);
	strcat(data,":");
	#endif
	#if HAS_HUMIDITY == 1 || HAS_I2C_COPY == 1
	utoa(humidade,data+strlen(data),10);
	strcat(data,":");
	 #endif
	 #if USE_ETH_SMS == 1
	strcat(data,"_:");
	data[strlen(data)-2]=gSMS.status;	 
	 #endif

	reqHTTP[0]='\0';
	#if HAS_NRF24L01 == 1 || HAS_I2C == 1 || HAS_MODBUS == 1
 	for (uint8_t node=0; node < MAX_TSENSOR_CLIENT; node++) {
	 strcat(reqHTTP,"\nX:");
	 *(reqHTTP+strlen(reqHTTP)-2)=node+'1';
	 // serie
	 utoa(gParam.radio.serieClient[node],reqHTTP+strlen(reqHTTP),10);
	 strcat(reqHTTP,":");
	 // rescursos
	 utoa(gParam.radio.recursoClient[node],reqHTTP+strlen(reqHTTP),16);
	 strcat(reqHTTP,":");
	 for (uint8_t sens=0; sens < MAX_TSENSOR_SENSOR; sens++) {
		utoa(gParam.radio.dataClient[node][sens],reqHTTP+strlen(reqHTTP),10);
		strcat(reqHTTP,":");
	 }
	}
	#endif
	 client.println(strlen(data)+strlen(reqHTTP));
	// client.println();
	 client.print(data);
	 client.println(reqHTTP);

	 debugvarln(data);
	 debugvarln(reqHTTP);
	 #endif // 1
	 client.stop();
    }
 }
}
///////////////////////////////////////////////////////////////////////////
void parse_eth(void)
{
	size_t size;
	bool   link=Enc28J60.linkStatus();
	bitWrite(ccStatus,CC_LINK_ETH,link);
	#ifdef TEST_ENC28
	test_hang_eth ();
	#endif
	if (!link) {
		led2_status=LED_PISCA_FLASH;
		return;
	}
	led2_status=LED_APAGA;
	#if HAS_WEBSERVER_FULL == 1
		parse_ESP8266();
	#elif HAS_WEBSERVER_TINY == 1
		parse_server();
	#else
/*	if (client = server.available()) {
		PGM_PRINT("Web ");
		while(client.available() > 0)
		{
			//size = client.read((uint8_t*)reqHTTP,min(MAX_HTTPreq-1,size));
			int size = client.read((uint8_t*)reqHTTP,MAX_HTTPreq-1);
		}
	}
	*/
	#endif
	
}
/////////////////////////////////////////////////////////////////////////////////
//boolean sATCIPSTARTMultiple(uint8_t mux_id,char *addr, uint16_t port, char *http_proxy,uint16_t http_port)
boolean sATCIPSTARTMultiple(uint8_t mux_id)
{
	char *addr=gParam.web.server_addr;
	uint16_t port=gParam.web.server_port;
	char *http_proxy=gParam.web.http_proxy;
	uint16_t http_port=gParam.web.http_port;
	static uint8_t t_reboot=MAX_T_REBOOT;
	byte ip[4];    
	uint8_t i;
	uint8_t conn;
	char *q,*p;
	
	#if DEBUG > 0 || HAS_BLUETOOTH  >= 2
	SSdebug.println(freeRAM());
	SSdebug.println(addr);
	#endif
	p=http_port?http_proxy:addr;
	/*if (!isdigit(*p)) {
		conn=client.connect(p,http_port?http_port:port);
	} else */
	   {
		for (i=0; i<4; i++) {
			q=strchr(p,'.');
			ip[i]=atoi(p);
			//SSdebug.println(ip[i]);
			p=q+1;	
		}
		conn=client.connect(ip,http_port?http_port:port);
	}
	#if DEBUG > 0 || HAS_BLUETOOTH >= 2
	p=http_port?http_proxy:addr;
	SSdebug.println(freeRAM());
	SSdebug.write('[');
	SSdebug.print(p);
	SSdebug.write(']');
	#else
	if (!ip[0]) {
		bitSet(gParam.status,P_STATUS_RESET);
		return false;		
	}
	#endif
//    bitWrite(ccStatus,CC_WIFI_INTERNET,conn);
	#if DEBUG > 0 || HAS_BLUETOOTH  >= 1
		SSdebug.write('c');
		SSdebug.println(conn);
	#endif
	  if (conn) {
		t_reboot=MAX_T_REBOOT;
		led2_status=LED_APAGA;
		bitSet(ccStatus,CC_WIFI_INTERNET);
	} else
		if (!--t_reboot) {
			Enc28J60.powerOff();
			delay(500);
			Enc28J60.powerOn();
			//led2_status=LED_PISCA_CURTO;
			//setup_eth();
			bitSet(gParam.status,P_STATUS_RESET);
			t_reboot=MAX_T_REBOOT;
		}
	#if DEBUG > 0 || HAS_BLUETOOTH >= 1
	else {
		SSdebug.write('B');
		SSdebug.println(t_reboot);
	}
	#endif

	  return conn;
}

boolean sATCIPCLOSEMulitple(uint8_t mux_id)
{
	client.flush();
	client.stop();
	//Ethernet.maintain();
	delay(40);
	return true;
}
boolean sATCIPSENDMultiple(int mux_id, uint8_t tipo_buffer, uint8_t *buffer, boolean flag_flush)
{
	uint16_t len;
	char *buf;
	int free_ram=freeRAM();
	/*
	if (tipo_buffer == POINTER_CHAR_RAM) {
		len=strlen((char*)buffer);
		} else {
		len=strlen_P((char*)buffer);
	}
	*/
	len=tipo_buffer?strlen_P((char*)buffer):strlen((char*)buffer);
	#if DEBUG>0 || HAS_BLUETOOTH >= 1
	SSdebug.print('s');
	SSdebug.println(free_ram);
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
	if (free_ram<0 || free_ram > 1000) {
			bitSet(gParam.status,P_STATUS_RESET);
	}
#if 0
	if (tipo_buffer == POINTER_CHAR_RAM) {
		client.print((char*)buffer);
		} else {
		len=strlen_P((char*)buffer);
		buf=(char*)malloc(len+1);
		if (buf) {
			strcpy_P(buf,(char*)buffer);
			client.print(buf);
			client.flush();
			free(buf);
		} else {
			for (uint16_t i = 0; i < len; i++) 
				client.write(pgm_read_byte(buffer++));
		}
	}
#else
	for (uint16_t i = 0; i < len; i++) {
			if (tipo_buffer==POINTER_CHAR_PROGMEM) {
				client.write(pgm_read_byte(buffer++));
			}else {
				client.write(*buffer++);
			}
		//	delay(5);
		}
//	delay(10);
#endif
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
	wifi_len=client.available();
	debugvar(wifi_len);
	#if DEBUG > 0 || HAS_BLUETOOTH >= 1
	SSdebug.write('\n');
	#endif
	bitClear(ccStatus,CC_MATA_BUFFER);
		ret = wifi_len > buffer_size ? buffer_size : wifi_len;
		ret--;
		i=c=0;
		while (millis() - start < timeout && i<ret) {
			while(client.available() > 0 && i<ret) {
				c=client.read();
				if (c=='\n') {
					ret=0;
					break;
				}
				if (!i && c=='C') bitSet(ccStatus,CC_MATA_BUFFER);
				buffer[i++] = c;
				#if DEBUG > 0 || HAS_BLUETOOTH >= 1
				SSdebug.write(c);
				#endif
				start=millis();
			}
			#if HAS_SENSOR_SECO == 1
			if (seco_int)
				parse_seco(false);
			#endif

		}
		buffer[i]='\0';

	if (bitRead(ccStatus, CC_MATA_BUFFER)) {
		esp8266Flush();
	}
	#if DEBUG > 0 || HAS_BLUETOOTH >= 5
	SSdebug.write('\n');
	#endif
	//debugvar(i);
	//debugln((char*)buffer);
	return i;
}


#endif