/*
* NRF24L01
*
*/
#include "TSensor.h"

uint8_t rf24_cmd; // comando para os nos
int rf24_par1;
int rf24_par2;
int rf24_par3;

#if HAS_NRF24L01 == 1


#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>


struct TPackage gPack;


void setup_nrf24(void) {
	uint8_t Raddr[6];
	#ifdef __AVR_ATmega2560__
	Mirf.cePin = 49;
	Mirf.csnPin = 53;
	#elif defined ( __AVR_ATtiny85__ )
	Mirf.cePin = 4;
	Mirf.csnPin = 3;
	#else
	#if TSENSOR_SERIE == 2
	Mirf.cePin = 8;
	Mirf.csnPin = 9;
//  default da biblioteca Mirf.cpp para estas portas
//	#else
//	Mirf.cePin = 7;
//	Mirf.csnPin = 8;
	#endif
	#endif
	PGM_PRINTLN("setup_radio");

	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	// devido a SPI.CPP que coloca en HIGH a porta 10 led amarelo
	#if TSENSOR_SERIE >= 3
	LEDA_OFF;	
	#endif
	
	/*Mirf.powerDown();
	Mirf.flushTx();
	Mirf.flushRx();
	Mirf.configRegister(STATUS,0x0E);
	*/
	strcpy_P((char*)Raddr,PSTR(SERV_ADDR));
	Raddr[0]=gParam.radio.serv_addr;
	debugvarln((char*)Raddr);
	Mirf.setRADDR((byte *)Raddr,0);
	Mirf.setRADDR((byte *)Raddr,1);
	Mirf.payload = sizeof(gPack);
	Mirf.channel=120;
	Mirf.config();
	// 0x02 1Mbps
	// bit 1 e 2 
	//Mirf.configRegister(RF_SETUP,0x06); //1Mbps
	
	//Mirf.configRegister(STATUS,RX_DR);
	// Mirf.configRegister(RF_SETUP,0x26); 250kbps
    //strcpy_P((char*)Raddr,PSTR("EILCT"));
	for (uint8_t node=0;node<MAX_TSENSOR_CLIENT;node++) {
		if (bitRead(gParam.radio.client,node)) {
			Raddr[0]=node+'0';
			Mirf.setRADDR((byte *)Raddr,node+2);
			debugvar(node);
		}
	}
	
}

//////////////////////////////////
// PARSE
//////////////////////////////////
uint8_t parse_nrf24(void)
{
	uint8_t ret=0;
	uint8_t st;
	unsigned long time;
	char Taddr[6]="EILCT";
	char Raddr[6]="EILCT";
	struct TPackage recPack;
	boolean flag_send_data=false;
	uint8_t node_livre=0xff;
	uint8_t node;
	uint16_t addr_base=MAX_TSENSOR_CLIENT*(gParam.radio.serv_addr-SERV_ADDR_BASE);

	if(!Mirf.isSending() && Mirf.dataReady()){
//	if(Mirf.dataReady()){
		#if HAS_DISPLAY == 1
		lcd.setCursor(6,0);
		lcd.write('1');
		#endif
		#if DEBUG > 0
		uint8_t PIPE;
		Mirf.readRegister( STATUS, &PIPE, sizeof(PIPE) );
		PGM_PRINT("1.ST="); debugbinln(PIPE);
		#endif
		
		Mirf.getData((uint8_t*)&recPack);
	//	if (Mirf.dataReady()) {
	//		uint8_t rx_data;
	//		Mirf.readRegister(RX_PW_P1,&rx_data,1);
	//		debugvarln(rx_data);
	//	}

		#if DEBUG > 10
		Mirf.readRegister( STATUS, &PIPE, sizeof(PIPE) );	
		PGM_PRINT("2.ST="); debugbinln(PIPE);
		#endif

		//debugvarln(recPack.addr);
		debugvar(recPack.seq);
		debugvar(recPack.serie);
		//debugvarln(recPack.cmd);
		gPack=recPack;
//		if (Raddr[4]=='E') {
		if (recPack.addr=='X') {
			uint16_t recurso=recPack.data[0] <<8 | recPack.data[1];
			uint16_t serie=recPack.serie;
			uint16_t check;
			uint16_t check_in=recPack.data[2] <<8 | recPack.data[3];
			check=recurso ^ serie;
			// procura cliente/addr livre para alocar
			gPack.cmd=PKG_CMD_ERR_FULL_CLIENT;
			
			if (check != check_in) {
				gPack.cmd=PKG_CMD_ERR_UNKNOW_CLIENT;
				debugvar(recurso);
				debugvar(serie);
				debugvarln(check);
			} else {
				
				for (node=0;node<MAX_TSENSOR_CLIENT;node++) {
					if (!bitRead(gParam.radio.client,node)) {
						if (node_livre==0xff) node_livre=node;
					} else
						if (gParam.radio.serieClient[node]==serie) {
							node_livre=0xff;
							break;
						}
				}
				debugvar(node); debugvarln(node_livre);
				if (node == MAX_TSENSOR_CLIENT && node_livre < MAX_TSENSOR_CLIENT) {
					node=node_livre;
				}
				if (node < MAX_TSENSOR_CLIENT) {
						gParam.radio.serieClient[node]=serie;
						gParam.radio.seqClient[node]=0;
						gParam.radio.recursoClient[node]=recurso;
						gParam.radio.timeClient[node]=millis()/1000;
					//debugvarln(gParam.radio.recursoClient[node]);
					
						gPack.cmd=PKG_CMD_SET_NODE;
						gPack.addr=Raddr[0]=node+'0'+addr_base;
						gPack.data[0]=Raddr[0];
//						gPack.data[0]=TSENSOR_SERIE << 8;
//						gPack.data[1]=TSENSOR_SERIE & 0xff;
						if (node_livre!=0xff) {
							bitSet(gParam.status,P_STATUS_GRAVA_EEPROM);
							ret++;
							#if DEBUG>0
							SSdebug.write('+');
							#endif
						}
						bitSet(gParam.radio.client,node);
						PGM_PRINT("new=");debugln(Raddr[0]);
						// ativa NFR24L01 para receber o msg
						flag_send_data=true;
						#if HAS_DISPLAY == 1
						lcd.setCursor(6,0);
						lcd.write(Raddr[0]);
						#endif

				}
			} // check != check_in
		} else { //x
			uint8_t client=(recPack.addr-'0') % addr_base; // TCLIn
			debugvarln(client);
			Taddr[0]=recPack.addr;
			#if HAS_DISPLAY == 1
			lcd.setCursor(8+client,0);
			lcd.write(Taddr[0]);
			#endif
			gPack.cmd=PKG_CMD_ERR_COMMAND_CLIENT;
			// retorno de execução de comanto
			if (recPack.cmd==PKG_CMD_SEND_DATA_CMD_STATUS) {
				PGM_PRINT("% ");
				uint8_t status=recPack.data[0];
				debugvarln(status);
				if (!status)
					bitSet(gParam.radio.hasCmdOK,client);
				else
					bitSet(gParam.radio.hasCmdERR,client);
				gPack.cmd=PKG_CMD_RECEIVE_DATA;
				timerWeb=0; // forca envio para o site
			}
			// recebendo dados
			if (recPack.cmd==PKG_CMD_SEND_DATA) { //3
				// testa se cliente existe
				if (!bitRead(gParam.radio.client,client) || gParam.radio.serieClient[client]!=recPack.serie) { //2
					gPack.cmd=PKG_CMD_ERR_UNKNOW_CLIENT;
					gPack.data[0]=gParam.radio.serv_addr;
					PGM_PRINTLN("inv client");
								flag_send_data=true;
					
					} else {
					int t,t2=-1000;
					int	_min=gParam.lim.min[client];
					int _max=gParam.lim.max[client];
					gParam.radio.timeClient[client]=now();
					gParam.radio.vccClient[client]=recPack.data[0] <<8 | recPack.data[1];
					// Temperatura
					t=gParam.radio.dataClient[client][0]=(int)(recPack.data[2] <<8 | recPack.data[3]);
					PGM_PRINT("tmp ="); debugln(t);
					// sensores extras
					if (gParam.radio.recursoClient[client] & REC_SENSORES) {
						gParam.radio.sensor9_10[client]=recPack.data[4];
					}
					if (gParam.radio.recursoClient[client] & & (REC_DS18B20_EXTRA | bit(REC_HUMIDADE))) {
						t2=gParam.radio.dataClient[client][1]=(int)(recPack.data[5] <<8 | recPack.data[6]);
						PGM_PRINT("tmp2="); debugln(t2);
					}
					#if HAS_DISPLAY == 1
						// col,lin
					lcd.setCursor(client*4,1);
					lcd.print(t/10);
					lcd.print("  ");
					#endif
					bitSet(gParam.radio.hasData,client);
					// testa limites
					if (!_min || !_max) {
						if (t<_min || t>_max) {
							if (!bitRead(gParam.radio.lim_flag,client)) {
								bitSet(gParam.radio.lim_flag,client);
								timerWeb=0; // força envio
							}
							
						} else
							if (bitRead(gParam.radio.lim_flag,client)) {
								bitClear(gParam.radio.lim_flag,client);
								timerWeb=0; // força envio
							}
					}
					
					if (recPack.seq == gParam.radio.seqClient[client]) 
						gPack.cmd=PKG_CMD_ERR_SEQ_DUPLO;
					 else
						if (recPack.seq == gParam.radio.seqClient[client]+1) { // 1
						// pacote válido, testa se tem comando para enviar
							if (bitRead(gParam.radio.hasCmd,client)) {
								debugvarln(rf24_cmd);
								gPack.cmd=PKG_CMD_RECEIVE_DATA_WITH_CMD;
								gPack.data[0]=rf24_cmd;
								gPack.data[1]=rf24_par1>>8;
								gPack.data[2]=rf24_par1;
								gPack.data[3]=rf24_par2>>8;
								gPack.data[4]=rf24_par2;
								gPack.data[5]=rf24_par3>>8;
								gPack.data[6]=rf24_par3;
								bitClear(gParam.radio.hasCmd,client);
								flag_send_data=true;
							}
							else
								gPack.cmd=PKG_CMD_RECEIVE_DATA;
						} else {
							gPack.cmd=PKG_CMD_ERR_SEQ;
							} // 1

					gParam.radio.seqClient[client]=recPack.seq;

				} //2
			} else { PGM_PRINTLN ("INV"); }// 3
		} // if (!recPack.addr) }
	}

	if (flag_send_data) {
		if (Raddr[0] != 'E') {
			//uint8_t node=Raddr[0]-'0';
			Mirf.setRADDR((byte *)Raddr,node+2);
			debugvarln(Raddr);
		}
		Mirf.setTADDR((byte *)Taddr);
		PGM_PRINT(">send ");
		debug(Taddr);
		//debugvar(gPack.addr);
		debugvarln(gPack.cmd);
		#if DEBUG > 10
		uint8_t st;
		Mirf.readRegister( STATUS, &st, sizeof(st) );
		PGM_PRINT ("1.st="); debugbinln(st);
		#endif
		Mirf.send((uint8_t*)&gPack);
		time=millis();
		#if DEBUG > 10
		Mirf.readRegister( STATUS, &st, sizeof(st) );
		PGM_PRINT ("2.st="); debugbinln(st);
		#endif
		while(Mirf.isSending()) {
			if ( ( millis() - time ) > 1000 ) {
				#if DEBUG  > 0
				PGM_PRINTLN("TO2");
				#endif
				break;
			}
		}
		#if 0
		Mirf.readRegister( STATUS, &st, sizeof(st) );
		PGM_PRINT ("st="); debugbin(st);
		//PGM_PRINT("reply cmd:");
		//debugln(gPack.cmd);
		#endif
	//	Mirf.setRADDR((byte *)SERV_ADDR,0);
	Mirf.readRegister( OBSERVE_TX, &st, sizeof(st) );
	PGM_PRINT (" obs_tx="); debugbinln(st);
	#if HAS_DISPLAY == 1
	lcd.setCursor(0,1);
	lcd.print(st,BIN);
	lcd.print("   ");
	#endif
	if ((st & 0xf0) > 0x40) {
			Mirf.configRegister(RF_CH,Mirf.channel);
			PGM_PRINTLN (" @");
	}
	#if 0
	Mirf.getADDR(RX_ADDR_P0,(uint8_t *)Raddr);
	debugvarln(Raddr);
	Mirf.getADDR(RX_ADDR_P1,(uint8_t *)Raddr);
	debugvarln(Raddr);
	for (uint8_t node=0;node<MAX_TSENSOR_CLIENT;node++) {
		if (bitRead(gParam.radio.client,node)) {
			Mirf.getADDR(RX_ADDR_P2+node,(uint8_t *)Raddr);
			PGM_PRINT(" n=");
			debug(Raddr);
		}
	}
	#endif
	//Mirf.setRADDR((byte *)SERV_ADDR,0);
	}

	//if (Mirf.dataReady())	bitSet(ret,1);
	return ret;
}

//////////////////////////////////////////////////////////////

//#ifndef  SoftwareSerial_h
#if 0
ISR(PCINT0_vect)
{
	// CAP
	if (digitalRead(2)==HIGH) {
		f_wdt |= bit(PCINT_FLAG);
	}
	//	else
	//	SoftwareSerial::handle_interrupt();
}
#endif

#endif