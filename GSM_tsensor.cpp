/**
 * @file GSM_tsensor.cpp
 * @author CAP
 * @date 1 Dez 2015
*/

#include "TSensor.h"

#if HAS_GSM == 1
// libraries
#include <SIM800.h>

// APN data
//#define GPRS_APN       "bandalarga.claro.com.br" // replace your GPRS APN
#define GPRS_APN       "zap.vivo.com.br" // replace your GPRS APN
#define GPRS_LOGIN     "claro"    // replace with your GPRS login
#define GPRS_PASSWORD  "claro" // replace with your GPRS password

const char str_APN_VIVO[] PROGMEM = "ZAP.VIVO.COM.BR";
const char str_APN_CLARO[] PROGMEM = "CLARO.COM.BR";
const char str_APN_TIM[] PROGMEM = "TIM.BR";
const char str_APN_OI[] PROGMEM = "GPRS.OI.COM.BR";
const char str_AMZ[] PROGMEM = "gprs.amazoniacelular.com.br";
const char str_TELEMIG[] PROGMEM = "gprs.telemigcelular.com.br";
const char str_SERCONTEL[] PROGMEM = "sercomtel.com.br";

const char* const str_APN[]  PROGMEM =
{
	str_APN_VIVO,   //0
	str_APN_CLARO,	//1
	str_APN_TIM,	//2
	str_APN_OI,		//3
	str_AMZ,		//4
	str_TELEMIG,	//5
	str_SERCONTEL	//6
}; ///< tabela com string APN das operadoras

typedef struct _APN_reg 
{
	uint8_t cimi;
	uint8_t apn_idx;
};

/*
http://portal.syspro.com.br/manual/003-FLEXCOMM3/MANUAL/FIRMWARE/v%201.10.3.32/html/listaapns.html
IMSI(MCC/MNC)	País	Operadora	APN	Username	Password
72402	Brasil	TIM	tim.br	tim	tim
72403	Brasil	TIM	tim.br	tim	tim
72404	Brasil	TIM	tim.br	tim	tim
72405	Brasil	Claro	claro.com.br	claro	claro
72406	Brasil	VIVO	zap.vivo.com.br	vivo	vivo
72410	Brasil	VIVO	zap.vivo.com.br	vivo	vivo
72411	Brasil	VIVO	zap.vivo.com.br	vivo	vivo
72423	Brasil	VIVO	zap.vivo.com.br	vivo	vivo
72431	Brasil	OI	gprs.oi.com.br	oiwap	oioioi
72424	Brasil	Amazonia Celular	gprs.amazoniacelular.com.br	celular	celular
72419	Brasil	Telemig Celular	gprs.telemigcelular.com.br	celular	celular
72407	Brasil	SERCOMTEL	sercomtel.com.br	sercomtel	sercomtel
*/

const PROGMEM _APN_reg APN_reg[] = 
{
	{2,2},{3,2},{4,2}, // TIM
	{5,1}, // CLARO
	{6,0},{10,0},{11,0},{23,0}, // VIVO
	{31,3}, // OI
	{24,4}, // Amazon
	{19,5}, // Telemig
	{7,6} // sercontel
}; ///< tablea que converte o codigo da operadora para a tabela str_APN, que tem o endereco da operadora

/**
 * @brief Rotina pesquisa pelo retorno do comando CIMI, qual a operadora APN
 * @param p : ponteiro para o string retornado do comando CIMI
 * @return ponteiro para o string com a operadora. Se length()=0 não encontrou
 * @note usa variavel global reqHTTP
 **/
char * APN_string(char *p)
{
	uint8_t op,i,idx, idxs;
	// retira 3 primeiros digitos
	p+=5;
	// marca final para 2 digitos
	p[2]='\0';
	op=atoi(p);
	debugvarln(op);
	reqHTTP[0]='\0';
	for (i=0; i < sizeof(APN_reg)/sizeof(APN_reg[0]); i++) {
		idx=pgm_read_byte(&(APN_reg[i].cimi));
		idxs=pgm_read_byte(&(APN_reg[i].apn_idx));
		//debugvar(idx);
		//debugvarln(idxs);
		//strcpy_P(buffer, (PGM_P)pgm_read_word(&(string_table[i])));
		if (op==idx) {
			strcpy_P(reqHTTP, (PGM_P)pgm_read_word((char*)&(str_APN[idxs])));
			debugvarln(reqHTTP);
			break;
		}
	}	
	return reqHTTP;
}

const char str_CIPSTART[] PROGMEM = "AT+CIPSTART=%d,\"%s\",\"%s\",\"%d\"";
//const char str_APN_VIVO[] PROGMEM = "zap.vivo.com.br";


CGPRS_SIM800 gprs;  ///< objeto SIM800
GSM_LOCATION loc;   ///< armazena localizacao e data do SIM800
uint8_t loc_count=MAX_LOC_COUNT; // a cada 10 envios, pega localização

/** 
 *  \brief Inicializa modulo GSM, detecta operadora e conecta no GPRS
 *  \param max numero maximo de tentativas
 */
void setup_gsm(uint8_t max)
{
  byte ret;
  char apn[50];
  //gprs.buffer=reqHTTP;
  PGM_PRINTLN("Resetting...");
  ret=gprs.init();
  if (!ret) {
		led2_status=LED_PISCA_FLASH;
		return;
  }

  ret=gprs.sim_detect();
  PGM_PRINT("SIM="); debugln(ret);
  if (0 && !ret) {
	  led2_status=LED_PISCA_FLASH;
	  return;
  }

  ret = gprs.setup(max);
	if (ret) {
		led2_status=LED_PISCA_FLASH;
		return;
	}
  
  gprs.getCIMI((char*)NULL);
  strcpy(apn,APN_string(gprs.buffer));
  /*
  if (gprs.getOperatorName()) {
	  PGM_PRINT("Operator:");
	  strupr(gprs.buffer);
	  debugln(gprs.buffer);
  }
*/
	gParam.web.ip[1][0]='\0';
	ret=apn[0];
	if (!ret) {
		led2_status=LED_PISCA_FLASH;
		return;
	}
	ret = gprs.join(apn,NULL,NULL, gParam.web.ip[1]);
	if (ret) {
		PGM_PRINT("Error code:");
		debugln(ret);
		debugln(gprs.buffer);
		return;
	}

  // TESTE DE PEGOU IP DA REDE
  	if (gParam.web.ip[1][0] && gParam.web.ip[1][0]!='0') {
	  	bitSet(ccStatus,CC_CWJAP);
		ccStatus |=  CC_WIFI_BITS_CWJAP_OK;
	  	}  else {
	  	bitClear(ccStatus,CC_CWJAP);
  	}

  #if DEBUG > 0
  ret = gprs.getSignalQuality();
  if (ret) {
     PGM_PRINT("Signal:");
     debug(ret);
     PGM_PRINTLN("dB");
  }
  #endif
  #if USE_LOCATION == 1
  ret=gprs.getLocation(&loc);
  #endif
  PGM_PRINTLN("Gsm: connecting...");
  
#ifndef GSM_CSTT
  ret=0;
  for (;;ret++) {
	  if (gprs.httpInit()) break;
	  debugvarln(gprs.buffer);
	  gprs.httpUninit();
	  if (!(ret%5)) gprs.gsm_reset();
	  delay(1000);
  }
  delay(3000);
#endif

  debugvarln(gParam.web.ip[1]);
  #if HAS_WEBSERVER_FULL == 1 || HAS_WEBSERVER_TINY == 1
  server.begin();
  #endif
}


/**
 * \brief rotina periodica do GSM ,somente para SMS
 */
void parse_gsm(void)
{
	#if USE_ETH_SMS == 1
	if (gSMS.status == 's') {// testa se tem mensagem para enviar
		ret=gprs.sendSms(gSMS.numero,gSMS.msg);
		if (ret)
			ret='S';
		else
			ret='E';
		gSMS.status=ret;
	}
	#endif
}
#if USE_ETH_SMS == 0
//////////////////////////////////////////////////////////////////////////////////////////////////
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
	char p[80],ip=0;
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
		}
	}
	//PGM_PRINTLN("<recvString");
	#if DEBUG > 0|| HAS_BLUETOOTH >= 1
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
	

	recvString(achou, target, "busy","link",  timeout, false);
	return !strcmp(target,achou);
}

/**
 * \brief GSM inicia comunicação para enviar dados ao servidor
 * \param mux_id
 * \param addr endereco do servidor
 * \param port porta de conexao
 * \param http_proxy servidor proxy
 * \param http_port porta do servidor proxy, se ==0 sem proxy
 * \return 1=ok, 0=erro
 */
boolean sATCIPSTARTMultiple(uint8_t mux_id)
{
	char *data=reqHTTP;
	uint16_t http_port=gParam.web.http_port;
	static uint8_t t_reboot=MAX_T_REBOOT;
	boolean ret=true;
	
	//getStatus();
	//delay(250);
	//PGM_PRINTLN(">ST");
	esp8266Flush();
#ifdef GSM_CSTT
	#if 1
	if (http_port)
		sprintf_P(data,str_CIPSTART,mux_id,type,gParam.web.server_addr,http_port);
	else
		sprintf_P(data,str_CIPSTART,mux_id,type,gParam.web.server_addr,gParam.web.server_port);
	debugvarln(data);
	SSwifi.println(data);
	#else
	SSwifi.print("AT+CIPSTART=");
	#if USE_MUX == 1
	SSwifi.print(mux_id);
	SSwifi.print(",\"");
	#endif
	SSwifi.print(type);
	SSwifi.print("\",\"");
	SSwifi.print(addr);
	// acerto das " (aspas)
	SSwifi.print("\",");
	SSwifi.print(port);
	SSwifi.print("\r\n");
	#endif

	SSwifi.flush();
	delay(200);
	#if DEBUG > 0
	bitSet(ccStatus,CC_DEBUG);
	#endif
	recvString(data,"OK", "ALR", NULL,10000,true); // ALREADY ou Linked
	#if DEBUG > 0
	debugvarln(data);
	while (SSwifi.available() > 0)
	{
		char c=SSwifi.read();
		SSdebug.write(c);
	}
	#endif

	gprs.sendCommand("AT+CIPSTATUS");
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
#else
  if (http_port) {
		sprintf_P(gprs.buffer,PSTR("AT+HTTPPARA=\"PROIP\",\"%s\""),gParam.web.http_proxy);
		ret &=gprs.sendCommand(gprs.buffer);
		if (ret) {
			sprintf_P(gprs.buffer,PSTR("AT+HTTPPARA=\"PROPORT\",\"%d\""),http_port);
			ret &=gprs.sendCommand(gprs.buffer);
		}
	  
  }
  strcpy_P(gprs.buffer,PSTR("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\""));
  ret&=gprs.sendCommand(gprs.buffer);
  sprintf_P(gprs.buffer,PSTR("AT+HTTPPARA=\"URL\",\"http://%s/uts0\""),gParam.web.server_addr);
  if (ret) ret &=gprs.sendCommand(gprs.buffer);
 if (ret) {
		  t_reboot=MAX_T_REBOOT;
		  led2_status=LED_APAGA;
	  } else
	  if (!--t_reboot) {
		  led2_status=LED_PISCA_CURTO;
		  setup_gsm(3);
		  t_reboot=MAX_T_REBOOT;
	  }

  
#endif
/*#if HAS_SENSOR_SECO == 1
	if (seco_int) {
		parse_seco(false);
	}
#endif*/
  return ret;
}

/**
 * \brief GSM fecha conexão http
 * \param 
 * \return 0=erro, 1=ok
 */
boolean sATCIPCLOSEMulitple(uint8_t mux_id)
{
#ifdef GSM_CSTT
	
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
	debugln(data);
	if (data[0]=='O' || data[0]=='U') {
		return true;
	}
	return false;
#else
	return true;
#endif
}
/**
 * \brief envia pacote de dados
 * \param mux_id  >  0   envia o AT+HTTPDATA e os dados
 * \param mux_id ==  0  somente dados
 * \param mux_id == -1  aguarda retorno de dados
 * \return 0=erro, 1=OK
 */
boolean sATCIPSENDMultiple(int mux_id, uint8_t tipo_buffer, uint8_t *buffer, boolean flag_flush)
{
	int len;
	
	if (mux_id==-1) {
			return gprs.sendCommand(0);
	}

	//debugvar(freeMemory());
	if (tipo_buffer == POINTER_CHAR_RAM)
	len=strlen((char*)buffer);
	else
	len=strlen_P((char*)buffer);

	if (!len) return true;
	
	///	rx_empty();
	//if (bitRead(ccStatus,CC_DEBUG))	debugvar(freeMemory());
	#if DEBUG >= 1 || HAS_BLUETOOTH >= 1
	debugvar(len);
	if (1||bitRead(ccStatus,CC_DEBUG)) {
		uint8_t *b=buffer;
		for (uint16_t i = 0; i < len; i++) {
			if (tipo_buffer==POINTER_CHAR_PROGMEM) {
				SSdebug.write(pgm_read_byte(b++));
				} else {
				SSdebug.write(*b++);
			}
		} // for
		bitClear(ccStatus,CC_DEBUG);
	}
	SSdebug.write('\n');

	#endif
	
	#ifdef GSM_CSTT
	//	esp8266Flush();
	SSwifi.print("AT+CIPSEND=");
	#if USE_MUX == 1
	SSwifi.print(mux_id);
	SSwifi.print(",");
	#endif
	SSwifi.print(len);
	SSwifi.print("\r\n");
	SSwifi.flush();
	if (recvFind(">",6000)) {
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
		SSwifi.write(0x1a);
		recvString(str_ret, "SEND OK", "busy","link is", 10000,flag_flush);
		#if DEBUG > 0
		//if (bitRead(ccStatus,CC_DEBUG))
		debugln(str_ret);
		#endif
		if (!strncmp(str_ret,"SEND OK",7)) {
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
	#else
	if (mux_id>0) {
		sprintf_P(gprs.buffer,PSTR("AT+HTTPDATA=%d,5000"),mux_id);
		gprs.sendCommand(gprs.buffer,5000,"DOWNLOAD");
	}
	for (uint16_t i = 0; i < len; i++) {
		if (tipo_buffer==POINTER_CHAR_PROGMEM) {
			SSwifi.write(pgm_read_byte(buffer++));
		} else {
			SSwifi.write(*buffer++);
		}
	}
	delay(150);
	return 1; 
	#endif
}
/////////////////////////////////////////////////////////////////////////////////////////
int sHTTPREAD(void) {
 int nbytes;
 gprs.httpRead();
 nbytes = gprs.httpIsRead();
 
   if (gprs.httpState == HTTP_ERROR) {
	   PGM_PRINTLN("Read error");
	   nbytes = -1;
   }

 debugvar(nbytes);
 debugln(gprs.buffer);
 return nbytes;
}
/////////////////////////////////////////////////////////////////////////////////////////
int sATCIPSENDACTION(int mode)
{
	uint8_t ret,idx, fase=0;
	int retHTTP=0;
	char c;
	uint32_t mark;
	gprs.sendCommand("AT+HTTPACTION=1");
	gprs.m_checkTimer=millis();
	PGM_PRINTLN("CK");
	gprs.m_bytesRecv = 0;
	ret=gprs.checkbuffer("ACTION:",NULL,20000);
	if (ret==1) {
		bitSet(ccStatus,CC_WIFI_INTERNET);
		led2_status=LED_APAGA;		
		mark=millis();
		idx=0;
		while (millis() - mark < 6000 && ret) {
			while (SSwifi.available()) {
				c=SSwifi.read();
				switch (c) {
				case ',':
					if (fase==1) {
						retHTTP=idx;
						debugvarln(retHTTP);	
						ret=0;		
					}
					fase++;
					idx=0;
					break;
				 default:
					idx= idx*10+(c-'0');
				} // switch
			}
		}
			
	} else {
		bitClear(ccStatus,CC_WIFI_INTERNET);
		led2_status=LED_PISCA_LONGO; // sem conexao ao roteador TAG_2		
	}
	debugvarln(retHTTP);
	esp8266Flush();
	return retHTTP;
}

/**
 * \brief recebe pacote em buffe conforme o moo
 * \param modo
 * \param buffer[out] dados lidos serao armazenados
 * \param buffer_size[in] tamanho do buffer
 * \param timeout[in] em ms
 * \param connectionId[in] nro da conexao, GSM nao usado
 * \return numero de bytes do buffer, se 0=timeout
 */
int recvPkg(uint8_t modo, uint8_t *buffer, uint16_t buffer_size, uint32_t timeout, uint8_t *connectionId)
{
	char c;
	uint16_t ret,i;
	unsigned long start;
	
	PGM_PRINT(">recvPkg ");
	//debugvarln(freeRAM());
	buffer[0]='\0';
	start = millis();
	ret=0;
	//wifi_len=SSwifi.available();
	//debugvarln(wifi_len);
	bitClear(ccStatus,CC_MATA_BUFFER);
	//ret = wifi_len > buffer_size ? buffer_size : wifi_len;
	ret = buffer_size-1;
	i=c=0;
	while (millis() - start < timeout && i<ret) {
			while(SSwifi.available() > 0 && i<ret) {
				c=SSwifi.read();
				#if DEBUG > 0
				SSdebug.write(c);
				#endif
				if (c=='\n') {
					ret=0;
					break;
				}
				if (!i && c=='C') bitSet(ccStatus,CC_MATA_BUFFER);
				buffer[i++] = c;
				start=millis();
			}
		}
	buffer[i]='\0';

	if (bitRead(ccStatus, CC_MATA_BUFFER)) {
		esp8266Flush();
	}
	//debugvar(i);
	//debugln((char*)buffer);
	PGM_PRINTLN("<recvPkg ");
	return i;
}

#endif // USE_ETH_SMS == 0

#endif