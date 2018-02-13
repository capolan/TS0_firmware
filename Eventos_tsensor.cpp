#include "TSensor.h"

#if HAS_EVENTOS == 1
#include "Time_c.h"
// Evento_tsensor.cpp

#define MAX_SENSOR_SECO_EVENTOS 5

struct TDEvento eventos[] = {
// evento,  {porta,,}, func, status, masrk
	{EVENTO_CONTAR_TEMPO_MAX,{12,8,1,1}	,FUNC_PISCA,0,0},  // porta e 8 segundos motor arranque por +8segundos
	{EVENTO_CRONOMETRO,{13,4}			,FUNC_NULL,0,0},  // porta e 4 segundos
	{EVENTO_COMBINACAO_2,{13,8,0b00,1}	,FUNC_NULL,0,0}, // porta1, porta2, combincao, tempo em seg funcionando e sem pressao oleo
	{EVENTO_FREQUENCIA,{12,3,5,1}		,FUNC_PISCA,0,0},    // porta 1 freq arranque por 3x em 5seg nao pegou
	{EVENTO_FICOU_1,{11},FUNC_NULL,0,0}					// botao emergencia
	//{EVENTO_MAX_VALOR,0,{A5,1,0}}
};

uint8_t eventos_feed; // indica o indice do alarme acima, em bits

void aciona_porta(uint8_t porta, uint8_t param1, uint16_t param2) {
	digitalWrite(porta,param1);
}

void (*evento_func[])(uint8_t,uint8_t,uint16_t) = {
		aciona_porta,
#if HAS_RELE == 1
		pisca
#endif
		};
void evento_acao(uint8_t jump_index)
{
	/* Call the function specified by jump_index */
	evento_func[jump_index](9,10,10);
}

///////////////////////////////////////////////////////////////////////////
void setup_eventos(void)
{
}

////////////////////////////////////////////////////////////////////////
uint8_t pega_sensor_seco(uint8_t porta)
{
	struct TSensor_Seco *pss=&gParam.sensor_seco;
	uint8_t i;
	for (i=0 ; i< MAX_SENSOR_SECO && pss->porta[i]; i++) {
		if (porta == pss->porta[i])
			return i;
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////
void limpa_eventos(uint8_t val)
{
	uint8_t i;
	for (i=sizeof(eventos)/sizeof(eventos[0]); i ;i--)
		eventos[i].status &=val;
}
////////////////////////////////////////////////////////////////////////
uint8_t parse_eventos(void)
{
	uint8_t i, ret, ret2;
	uint8_t bitPorta1, bitPorta2;
	int val,referencia;
	uint32_t diff;
	TEVENTO_FUNC idx; // ponteiro para tabela de funcoes
	struct TSensor_Seco *pss=&gParam.sensor_seco;
	ret2=0;
	for (i=sizeof(eventos)/sizeof(eventos[0]); i ;)
	{
		i--;
		bitPorta1=pega_sensor_seco(eventos[i].param[0]);
		if (bitPorta1 == -1) {
			bitSet(eventos[i].status,EVENTO_ERRO);   // ERRO
			continue;
		}
		if (eventos[i].status & (_BV(EVENTO_ACIONADO) | _BV(EVENTO_DESATIVADO)))
			continue;
		// TESTE TIPO DO EVENTO
		switch (eventos[i].tipo) {
		case EVENTO_FICOU_0:
			if (bitRead(pss->trocou,bitPorta1) && !bitRead(pss->valor,bitPorta1)) {
				ret=1;
				}
		break;
		case EVENTO_FICOU_1:
		if (bitRead(pss->trocou,bitPorta1) && bitRead(pss->valor,bitPorta1)) {
			ret=1;
		}
		break;
		case EVENTO_CONTAR_TEMPO_MIN:
		if (bitRead(pss->trocou,bitPorta1)) {
			bitClear(pss->trocou,bitPorta1);
			if (bitRead(pss->valor,bitPorta1)) {  //voltou a 1
				bitClear(eventos[i].status,EVENTO_MARCA_INICIAL);
				PGM_PRINT("2.TEMPO == ");
				debugln(eventos[i].mark);
				eventos[i].mark=now() - eventos[i].mark;
				PGM_PRINT("3.TEMPO == ");
				debugln(eventos[i].mark);
				if (eventos[i].mark <= eventos[i].param[1]) {
					ret=1;
				}
			} else {  // 
				eventos[i].mark=now();
				PGM_PRINT("1.TEMPO == ");
				debugln(eventos[i].mark);
				bitSet(eventos[i].status,EVENTO_MARCA_INICIAL);
			}
		}
		break;
		case EVENTO_CONTAR_TEMPO_MAX:
		if (bitRead(pss->trocou,bitPorta1)) {
			bitClear(pss->trocou,bitPorta1);
			if (bitRead(pss->valor,bitPorta1)) {  //voltou a 1
				bitClear(eventos[i].status,EVENTO_MARCA_INICIAL);
				PGM_PRINT("2.TEMPOM == ");
				debugln(eventos[i].mark);
				eventos[i].mark=now() - eventos[i].mark;
				PGM_PRINT("3.TEMPOM == ");
				debugln(eventos[i].mark);
			} else {  //
				eventos[i].mark=now();
				PGM_PRINT("1.TEMPOM == ");
				debugln(eventos[i].mark);
				bitSet(eventos[i].status,EVENTO_MARCA_INICIAL);
			}
		}
		if (bitRead(eventos[i].status,EVENTO_MARCA_INICIAL) && (now()-eventos[i].mark) > eventos[i].param[1]) {
					ret=1;
					PGM_PRINTLN("@@");
					bitClear(eventos[i].status,EVENTO_MARCA_INICIAL);
		}
		break;
		case EVENTO_CRONOMETRO:
		if (bitRead(pss->trocou,bitPorta1)) {
			bitClear(pss->trocou,bitPorta1);
			if (bitRead(pss->valor,bitPorta1)) {  //voltou a 1
				bitClear(eventos[i].status,EVENTO_MARCA_INICIAL);
				PGM_PRINT("2.CRONO == ");
				debugln(eventos[i].mark);
				diff=now() - eventos[i].mark;
				//eventos[i].mark=now() - eventos[i].mark;
				PGM_PRINT("3.CRONO == ");
				debugln(diff);
				ret=1;
				} else {  //
				eventos[i].mark=now();
				PGM_PRINT("1.CRONO == ");
				debugln(eventos[i].mark);
				bitSet(eventos[i].status,EVENTO_MARCA_INICIAL);
			}
		}
		break;
		case EVENTO_TROCA_ESTADO:
		if (bitRead(pss->trocou,bitPorta1)) {
			ret=1;
				PGM_PRINT("===========");
		}
		break;
		case EVENTO_COMBINACAO_2:
			val=gParam.sensor_seco.valor;
			bitPorta2=pega_sensor_seco(eventos[i].param[1]);
			if (bitPorta2 == -1) {
				bitSet(eventos[i].status,EVENTO_ERRO);   // ERRO
				continue;
			}
			bitPorta1 = (bitRead(val,bitPorta1) << 1) | bitRead(val,bitPorta2);
			if (eventos[i].param[2] == bitPorta1) {
				if (bitRead(eventos[i].status,EVENTO_MARCA_INICIAL)) {
//					PGM_PRINT("2.COMB == ");
	//				debugln(eventos[i].mark);
					diff=now() - eventos[i].mark;
					if (diff >= eventos[i].param[3] && !bitRead(eventos[i].status,EVENTO_ALARMADO)) {
						bitClear(eventos[i].status,EVENTO_MARCA_INICIAL);
						bitSet(eventos[i].status,EVENTO_ALARMADO);
						eventos[i].mark=diff;
						PGM_PRINT("3.COMB == ");
						debugln(eventos[i].mark);
						ret=1;
					}
				} else {  //
					eventos[i].mark=now();
					PGM_PRINT("1.COMB == ");
					debugln(eventos[i].mark);
					bitSet(eventos[i].status,EVENTO_MARCA_INICIAL);
				}
			} else {
				bitClear(eventos[i].status,EVENTO_MARCA_INICIAL);
				bitClear(eventos[i].status,EVENTO_ALARMADO);
			}
		break;
		case EVENTO_FREQUENCIA:
			if (pss->freq[bitPorta1] >= eventos[i].param[1]) {
				ret=1;
				pss->freq[bitPorta1]=0;
			}
		break;
		#if HAS_SENSOR_ANALOGICO == 1
		case EVENTO_MIN_VALOR:
		val=parse_seco_analogico(eventos[i].param[1], 20); // 20 milisegundo entre lidas
		referencia=eventos[i].param[1]<<8 | eventos[i].param[2];
		if (val < referencia) {
			ret=1;
		}
		break;
		case EVENTO_MAX_VALOR:
		val=parse_seco_analogico(eventos[i].param[1], 20); // 20 milisegundo entre lidas
		referencia=eventos[i].param[1]<<8 | eventos[i].param[2];
		if (val > referencia) {
			ret=1;
		}
		break;
		#endif
		} // switch
		if (ret) {
				ret2++;
				idx=eventos[i].func_idx;
				bitSet(eventos[i].status,EVENTO_ACIONADO);
				bitSet(eventos_feed,i); // so envio limpa os bits
				eventos[i].mark=now();
				// gravar log evento
				grava_eventos_log(eventos[i].tipo, eventos[i].param[0],idx,diff);
				bitClear(eventos[i].status,EVENTO_ACIONADO);
				bitClear(pss->trocou,bitPorta1);
				#if HAS_RELE == 1
				if (idx!=FUNC_NULL) {
					PGM_PRINTLN("func");
					if (eventos[i].param[3]) {
						(*evento_func[idx]) (RELE_PIN,eventos[i].param[2],eventos[i].param[3]*1000);

						}
				}
				#endif
	//			evento_func[0](10,10,10);
				PGM_PRINT(">>>> i=");
				debug(i);
				PGM_PRINT(": pt=");
				debug(eventos[i].param[0]);
				PGM_PRINT(": mark=");
				#if HAS_CLOCK == 1
				debugln(formatHora(reqHTTP,eventos[i].mark));
				#endif
				timerWeb=ret=0;
		}
	} // for
	return ret2;
}
//////////////////////////////////////////////////////////
// Le quais porta, com parametros, sem 2 bytes
//   A: 1 - limpa todos os sensores acionados
//   B: 55 =desliga rele
//      CC = liga rele
//  TG: ABB
//  TG: EFAABBCCDD
//    E = evento
//    F = funcao
//    AA= parametro 1
//    BB= parametro 2
//    CC= parametro 3
//    DD= parametro 4
//////////////////////////////////////////////////////////
void process_TG(char* p)
{
	struct TDEvento *pe=&eventos[0];
	uint8_t evento,funcao,i;
	for (i=0; i< MAX_SENSOR_SECO_EVENTOS; i++, pe++) {
		debugvarln(p);
		if (*p) {
			pe->tipo=(htoi(*p++)<<4);
			pe->func_idx=(htoi(*p++)<<4);
			pe->param[0]=(htoi(*p++)<<4) + htoi(*p++);
			pe->param[1]=(htoi(*p++)<<4) + htoi(*p++);
			pe->param[2]=(htoi(*p++)<<4) + htoi(*p++);
			pe->param[3]=(htoi(*p++)<<4) + htoi(*p++);
			pe->status=0;
		} else {
			pe->tipo=EVENTO_NULL;
		}
	} // FOR
//	bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
}
#endif