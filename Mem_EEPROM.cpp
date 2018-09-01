/**
 * @file Mem_EEPROM.cpp
 * @author CAP
 * @date 1 Dez 2015
 */

#include "TSensor.h"

#if HAS_EEPROM == 1
#include "avr/eeprom.h"
#include "eeprom.h"

#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
uint16_t iDataLoggerLog=0; ///< ponteiro para inicio primeiro registro da fila
uint16_t fDataLoggerLog=0; ///< ponteiro para inicio ultimo registro da fila
uint16_t maxDataLoggerLog;  ///< numero maximo de registro de log conforme a memoria
#endif
#if USE_AT24 == 1
uint16_t i24DataLoggerLog=0;
uint16_t f24DataLoggerLog=0;
//uint16_t max24DataLoggerLog;
#define MAX24DATALOGGER (MAX_AT24_EEPROM_MEMORY / sizeof(TDataLogger))
#endif


/**
 * @brief Le estrutura gParam da EEPROM para gParam
 * @return false: se EEPROM(0) == 0xCC ou 0xBB nao le memoria
 *   0xCC : reboot durante a rotina de inicialização
 *   0xBB :  reboot proposital, pelo sistema
 *   0xAA : reboot durante o boot, rotina de setup
 *    true: memoria EEPROM lida par gParam
 */
boolean read_gParam(void)
{
	uint8_t *p;
	uint8_t readEEPROM; ///< valor do byte 0 da memoria nao volatil. 0xFF = nao inicializada, 0x00=OK, 0xCC ou 0xBB nao le
	
	readEEPROM=eeprom_read_byte(0);// EEPROM.read(0);
	#if HAS_BLUETOOTH >=2
	SSdebug.write('E');
	SSdebug.println(readEEPROM);
	#endif
//	if (readEEPROM != 0xCC && readEEPROM!=0xBB && readEEPROM!=0xAA) return false;
	if (readEEPROM != 0xCC && readEEPROM!=0xBB) return false;
	if (readEEPROM == 0xBB) bitSet(ccStatus,CC_BOOT_RESET);
	p=(uint8_t *)&gParam.status;
	//for (int i=0; i<sizeof(gParam); i++) {
//		*p++=EEPROM.read(GPARAM_ADDR+i);
//	}
	
	eeprom_read_block ((void *)p, (void *)GPARAM_ADDR, sizeof(gParam));
	
	#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
	iDataLoggerLog=eeprom_read_word((uint16_t*)IDATALOGGER_ADDR);
	fDataLoggerLog=eeprom_read_word((uint16_t*)FDATALOGGER_ADDR);
	debugvar(iDataLoggerLog); debugvarln(fDataLoggerLog);

	// recupera log perdido
	uint16_t addr=DATALOGGER_ADDR + iDataLoggerLog * sizeof(TDataLogger);
	eeprom_read_block ((void *) &gDataLogger.id, (const void *)addr, sizeof(TDataLogger));
	if (gParam.id != gDataLogger.id) {
		PGM_PRINTLN("LOG seq");
		bitSet(flagHTTP,F_LOG_SYNC);
	}
	#endif	

	#if USE_AT24 == 1
	i24DataLoggerLog=eeprom_read_word((uint16_t*)I24DATALOGGER_ADDR);
	f24DataLoggerLog=eeprom_read_word((uint16_t*)F24DATALOGGER_ADDR);
	debugvar(i24DataLoggerLog); debugvarln(f24DataLoggerLog);
	#endif

	return true;
}

/// Grava gParam na EEPROM
boolean write_gParam(void)
{
	uint8_t *p;
	int i;
	
	
	//1bitClear(gParam.status,P_STATUS_RESET_FABRICA);
	p=(uint8_t *)&gParam.status;
	//for (i=0; i<sizeof(gParam); i++) {
//		EEPROM.write(GPARAM_ADDR+i,*p++);
//		while (!eeprom_is_ready()) delay(1);
//	}
	
	eeprom_update_block ((void *)p, (void *)GPARAM_ADDR, sizeof(gParam));
	while (!eeprom_is_ready()) delay(1);
	PGM_PRINTLN("Wg");
	//EEPROM.write(0,0xCC);
	eeprom_write_byte(0,0xCC);
	
	#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
	eeprom_write_word((uint16_t*)IDATALOGGER_ADDR,iDataLoggerLog);
	eeprom_write_word((uint16_t*)FDATALOGGER_ADDR,fDataLoggerLog);
	#endif

	#if USE_AT24 == 1
	eeprom_write_word((uint16_t*)I24DATALOGGER_ADDR,i24DataLoggerLog);
	eeprom_write_word((uint16_t*)F24DATALOGGER_ADDR,f24DataLoggerLog);
	#endif


	return true;
}
#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
#define TEMP_COEF_1 300
#define TEMP_COEF_2 0.25
////////////////////////////////////////////////////
uint8_t grava_corrente_log(uint32_t thora) {
	uint16_t addr;
	uint8_t c;
	#if HAS_TEMPERATURA == 1 
	struct TDS20B18 *pT=pTemp;
	#endif
	int i;
	uint8_t idx=0;

	#if HAS_CLOCK == 1
	// segurança memoria
	//if (thora-gDataLogger.hora < 1 &&	gDataLogger.tipo ) { PGM_PRINTLN("LOGERR"); return 0; }
	gDataLogger.hora=thora;
	#endif
	gDataLogger.id=gParam.id;

	#if HAS_TEMPERATURA == 1 && USE_TEMP_EEPROM == 1
	c=MAX_SENSORES_DS18B20;
	while (pT != NULL && c--) {
		i = ((float)(pT->temperatura+TEMP_COEF_1) * TEMP_COEF_2);
		gDataLogger.temp[idx++] = (uint8_t)(constrain(i,0,255));
		pT=pT->p;
		
	}	
	#endif
	#if HAS_HUMIDITY == 1
	gDataLogger.humidade=humidade;
	#endif
	#if HAS_SENSOR_ANALOGICO == 1
	gDataLogger.sensor_analogico[0]=sensor_analogico[0];
	#endif
	#if HAS_SENSOR_SECO == 1
	gDataLogger.sensor_seco_valor=gParam.sensor_seco.valor;
	gDataLogger.sensor_seco_trocou=gParam.sensor_seco.trocou;
	#endif


	/* EEPROM inicio */
	iDataLoggerLog = ++iDataLoggerLog % maxDataLoggerLog;
	addr = DATALOGGER_ADDR + iDataLoggerLog * sizeof(TDataLogger);

	#if DEBUG > 10
	debugvar(gParam.id);
	debugvar(addr);
	debugvar(iDataLoggerLog);
	debugvarln(fDataLoggerLog);
	debugvarln(gDataLogger.field1);
	#endif

	eeprom_update_block((const void*)&gDataLogger,(void *)addr,sizeof(TDataLogger));
	eeprom_write_word((uint16_t*)IDATALOGGER_ADDR,iDataLoggerLog);
	

	if (iDataLoggerLog == fDataLoggerLog) {
		fDataLoggerLog = ++fDataLoggerLog % maxDataLoggerLog;
		eeprom_write_word((uint16_t*)FDATALOGGER_ADDR,fDataLoggerLog);
	}
	/* EEPROM fim */
	#if USE_AT24 == 1
	/* AT24 inicio */
	if (gDataLogger.tipo != EVENTO_NULL) {
		i24DataLoggerLog = ++i24DataLoggerLog % MAX24DATALOGGER;
		addr = i24DataLoggerLog * sizeof(TDataLogger);

		#if DEBUG > 0
		debugvar(gParam.id);
		debugvar(addr);
		debugvar(i24DataLoggerLog);
		debugvarln(f24DataLoggerLog);
		#endif
		if (memAT24)
			memAT24->write(addr, (byte*)&gDataLogger,sizeof(TDataLogger));
		eeprom_write_word((uint16_t*)I24DATALOGGER_ADDR,i24DataLoggerLog);

		if (i24DataLoggerLog == f24DataLoggerLog) {
			f24DataLoggerLog = ++f24DataLoggerLog % MAX24DATALOGGER;
			eeprom_write_word((uint16_t*)F24DATALOGGER_ADDR,f24DataLoggerLog);
		}
	}
	/* AT24 fim */
	#endif
	return 1;
}
////////////////////////////////////////////////////
// tipo bits = 0  crescente
//           = 1 WEB csv
//           = 2 JSON {"data":[{"val":"<nnnn>"},...]}
//           = 3 printf formato WEB download
//  retorno
//      0 = err
//      1 = ok
////////////////////////////////////////////////////
const char str_LOG_HEADER [] PROGMEM = "sep=,\nseq,data,val1,val2,val3,val4,val5,val6,val7,val8\n";
uint8_t list_corrente_log(uint8_t tipo, int mux_id, char *p) {
	boolean fim=false;
	uint16_t addr, base;
	uint16_t fase;
	uint16_t *ii;
	uint16_t *ff;  //inicial e final
	uint8_t h, fc,t;
	char *q;
	char ev[10];
	
	#if USE_AT24 == 1
	if (bitRead(tipo,LISTA_LOG_AT24)) {
		ii=&i24DataLoggerLog;
		ff=&f24DataLoggerLog;
		base=0;
	} else 
	#endif
		{
		ii=&iDataLoggerLog;
		ff=&fDataLoggerLog;
		base=DATALOGGER_ADDR;
	}

	if (*ii == *ff) return 1;

	if (bitRead(tipo,LISTA_LOG_WEB)) {
		#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
		if (!sATCIPSENDMultiple(mux_id, POINTER_CHAR_PROGMEM, (uint8_t*)str_LOG_HEADER,true)) return 0;
		#endif
	}
	// JSON
	if (bitRead(tipo,LISTA_LOG_JSON)) {
		//p[23]=4+MAX_SENSORES_DS18B20+'0';

		sprintf_P(p,"\n\n{\"g\":\"%d\",\"api_key\":\"",bitRead(tipo,LISTA_LOG_AT24)+1);
		#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
		if (!sATCIPSENDMultiple(mux_id, POINTER_CHAR_RAM, (uint8_t*)p,true)) return 0;
		#endif

		strcpy(p,gParam.web.apy_key);
		strcat_P(p,PSTR("\",\"n\":\"8\",\"data\":["));
		#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
		sATCIPSENDMultiple(mux_id, POINTER_CHAR_RAM,(uint8_t *)p,true);
		#endif
	}
	// WEB
	#if HAS_BLUETOOTH >=5 
	if (bitRead(tipo,LISTA_LOG_PRINT)) {
		PGM_PRINTP(str_LOG_HEADER+6);
	}
	#endif

	#if DEBUG > 0
	debugvar(*ii);
	debugvar(*ff);
	#endif
	
	fc=*ff;
	
	do {
		fc =  (fc+1) % maxDataLoggerLog;

		addr = base + fc * sizeof(TDataLogger);
		#if USE_AT24 == 1
		if (bitRead(tipo,LISTA_LOG_AT24)) {
			if (memAT24)
				memAT24->read(addr,(byte *) &gDataLogger.id,  sizeof(TDataLogger));
		} else
		#endif
			eeprom_read_block ((void *) &gDataLogger.id, (const void *)addr, sizeof(TDataLogger));


		p[0]='\0';
		q=p;
		#if USE_LOG_EVENTOS == 1
		sprintf(ev,"%d,%d,%d",gDataLogger.tipo,gDataLogger.porta,gDataLogger.funcao);
		#endif
		if (bitRead(tipo,LISTA_LOG_JSON)) {
			#if USE_LOG_EVENTOS == 1
			strcpy_P(p,PSTR("{\"evento\":\""));
			strcat(p,ev);
			strcat_P(p,PSTR("\",\"val\":\""));
			#else
			strcat_P(p,PSTR("{\"val\":\""));
			#endif
		}
		utoa(gDataLogger.id,p+strlen(p),10);
		strcat(p,",");
		#if HAS_CLOCK == 1
		time_t t;
		t=gDataLogger.hora;
		if (bitRead(tipo,LISTA_LOG_JSON)) {
			ultoa(t,p+strlen(p),10);	
		} else {
			formatHora(p+strlen(p), t);
		}
		#else
				itoa(fc,p+strlen(p),10);
		#endif
		strcat(p,",");
		#if HAS_CORRENTE==1
		if (gDataLogger.fase1 == 0xFFFF) {   // testa erro na eeprom
			PGM_PRINTLN ("F");
			break;
		}
		sprintf(p+strlen(p),"null,%d,%d,%d",gDataLogger.fase1,gDataLogger.fase2,gDataLogger.fase3);
		#else
		// field1/2/3/4
		strcat_P(p,PSTR("null,null,null,null"));
		#endif
		t=0;
		#if HAS_TEMPERATURA == 1 && USE_TEMP_EEPROM == 1
		// field5/field6
		for (; t<2; t++) {
			// CSV e debug
			float temp;
			if (t<MAX_SENSORES_DS18B20) {
				fase=gDataLogger.temp[t];
				temp=((float)fase/TEMP_COEF_2) -TEMP_COEF_1; 
				strcat(p,",");
				dtostrf(temp/10,4,1,p+strlen(p));
			} else {
				#if HAS_HUMIDITY == 1
				strcat(p,",");
				itoa(humidade,p+strlen(p),10);
				#else
				strcat(p,",null");
				#endif
			}

		}
		#else
		strcat_P(p,PSTR(",null,null"));
		t=2;
		#endif
		//field7
		#if HAS_SENSOR_ANALOGICO == 1
		strcat(p,",");
		if (gParam.porta_sensor_analogico[0])
			utoa(gDataLogger.sensor_analogico[0],p+strlen(p),10);
		else
			strcat(p,",null");
		#else
			strcat(p,",null");
		#endif



		#if HAS_SENSOR_SECO == 1
		strcat(p,",");
		utoa(gDataLogger.sensor_seco_valor,p+strlen(p),10);
		strcat(p,".");
		utoa(gDataLogger.sensor_seco_trocou,p+strlen(p),10);
		#else
		strcat(p,",null");
		#endif

		if (fc == *ii) fim=true;
		
		// JSON
		if (bitRead(tipo,LISTA_LOG_JSON)) {
			strcat_P(p,PSTR("\"}"));
			if (!fim)
				strcat(p,",\n");
			else
				strcat_P(p,PSTR("]}\n"));
		} else {
			strcat(p,",");
			strcat(p,ev);
			strcat(p,"\n");
		}
		//debugln(p);
		
		debugvar(fc);
		#if HAS_BLUETOOTH >=5
		if (bitRead(tipo,LISTA_LOG_PRINT))
			SSdebug.print(p);
		else 
		#endif
			 {
			#if HAS_ESP8266 == 1 || HAS_ETH == 1 || HAS_GSM == 1
			if (!sATCIPSENDMultiple(mux_id, POINTER_CHAR_RAM, (uint8_t*)p,true)) return 0;
			#endif
			if (!(fc % 10)) {
				delay(700);
				PGM_PRINTLN("_");
			}
		}
	
	} while (!fim);
	PGM_PRINTLN("<Xlog");
	return 1;
}
#endif
////////////////////////////////////////////////////
#if USE_LOG_EVENTOS == 1
uint8_t grava_eventos_log(TEvento_tipo tipo, uint8_t porta, uint8_t funcao, uint32_t field) 
{
	uint32_t agora;
	PGM_PRINT(">grava_eventos_log  ");
	debug(tipo);
	debugvar(porta);
	debugvarln(field);
	#if HAS_CLOCK == 1
	agora=now();
	#else
	agora=0;
	#endif
	gDataLogger.tipo=tipo;
	gDataLogger.porta=porta;
	gDataLogger.funcao=funcao;
	gDataLogger.field1=field;
	return grava_corrente_log(agora);
}
#endif

/** @brief Inicializa memoria nao volatil. Se botao pressionado, reinicializa flash, apagando toda configuração
 *  @return true: firmware inicializado por botao pressionado
 *          false:le da EEPROM a configuracao para gParam
 * 
 */
boolean setup_eeprom(void) {
	int retEEPROM;
	boolean ret,rst=false;
	
	#if USE_RESET_FIRMWARE_ON_BOOT == 1
	write_gParam();
	#else
	
	#if HAS_BOTAO == 1
	ret=setup_boot_eeprom();
	PGM_PRINT("sbe=");debugln(ret);
	if (ret) {
		led2_status=LED_PISCA_FLASH;
		PGM_PRINTLN("fw");
		rst=true;
		strcpy(reqHTTP,gParam.web.server_addr);
		write_gParam();
		if (ret&1) { 
			led1_status=LED_PISCA_FLASH;
			strcpy(gParam.web.server_addr,reqHTTP);
			delay(2000);
		}
		delay(500);
	} else
	#endif // HAS_BOTAO
	#endif 
	{
		retEEPROM=read_gParam();
		if (!retEEPROM) {
			write_gParam();
			//1bitSet(gParam.status,P_STATUS_RESET_FABRICA);
		}
		debugvarln(retEEPROM);
	}

	
	//uint16_t total_free=TOTAL_EEPROM_FREE;
	uint16_t corr_log=DATALOGGER_ADDR;
	#if DEBUG > 10
	//PGM_PRINT("EEPROM max=");
	//debug(MAX_EEPROM_MEMORY);
	//PGM_PRINT(" free=");
	//debug(TOTAL_EEPROM_FREE);
	//PGM_PRINT(" log=");
	//debugln(corr_log);
	#endif
	
	//debugvarln(total_free);
	
	#if USE_LOG_EEPROM == 1 || USE_LOG_EVENTOS == 1
	maxDataLoggerLog=TOTAL_EEPROM_FREE / sizeof(TDataLogger);
	#endif

	#if DEBUG > 10
	debugvar(LAST_ADDR_MEMORY);
	debugvar(maxDataLoggerLog);
	debugvar(sizeof(TDataLogger));
	//debugvarln(total_free);
	#endif

	return rst;
}

/**
 *  @brief Testa se usuario que entrar em modo de configuração
 *  @date 02 abril 2017
 *  @return 0 botao nao pressionado
 *  @return 1 pressionado por menos de 5 segundos
 *  @return 2 pressionado por mais de 20 segundos
 */
uint8_t setup_boot_eeprom(void)
{
	uint32_t diff,mark=millis();
	uint8_t ret=0; /* <<! ret retorno da funcao */

	do {
		diff = millis()-mark;
		if (diff >= 5000) {
			led1_status=LED_LIGAR;
			PGM_PRINTLN ("rst1");
			}
		if (diff > 30000) {
			ret=2;
			PGM_PRINTLN ("rst2");
			break;
		} 
	} while (!BUTTON_READ);
	led1_status=LED_APAGA;

	if (diff > 5000 && ret!=2) ret=1;
	PGM_PRINT(">SBE diff=");
	debugln(diff);
	return ret;
}


#endif

#if USE_AT24 == 1
#include <Wire.h>
#include <AT24Cx-master/AT24CX.h>


// EEPROM object
AT24C32 *memAT24=NULL; ///< ponteiro para memoria 
/// inicializa memoria EEPROM externa i2C 4Kb
void setup_at24() {
	Wire.begin();
	int i2c = 0x50;
	for (int i=0; i<8; i++) {
		Wire.beginTransmission(i2c);
		int result = Wire.endTransmission();
		if (result==0) {
			memAT24=new AT24C32(i);
			break;
		} 
		i2c++;
	}

}
#endif
