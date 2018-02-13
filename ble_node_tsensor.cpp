/**
 * @file comunicação serial, bluetooth
 * @author CAP
 * @date 6 Abr 2017
* 
**/
#include "TSensor.h"

#if HAS_BLE == 1


#include <SoftwareSerial.h>

#define I2C_CMD_READ_CONFIG  0x01 // retorna 4 bytes   recursos + serie
#define I2C_CMD_READ_SENSORES 0x02 // retorna 12 bytes = 2 status  + 5 sensores
#define I2C_CMD_WRITE 0x10 // envia dados

struct TPackage gPack;

//struct Ti2c_node *pi2c_node=NULL;


///////////////////////////////////////////////////////
void i2c_create(byte addr) {
	uint8_t node, ret;
	uint16_t recursos, serie;
	byte c[4];

	Wire.beginTransmission(addr);
	Wire.write(I2C_CMD_READ_CONFIG);  // pega recursos
	Wire.write((uint8_t *)&gParam.rede, sizeof(TPRede));
	Wire.endTransmission();
	delay(100);
	ret=Wire.requestFrom(addr,4);
	if (ret != 4) {
		debughex(addr);
		PGM_PRINTLN (" ecfg i2c");
		return;
	}
	//debugvarln(ret);
	ret=0;
	while (Wire.available()) {
		c[ret++]=Wire.read();
	}
	serie=c[1] << 8 | c[0];
	recursos=c[3] << 8 | c[2];
	debugvar(recursos);
	debugvarln(serie);
	ret=0xff;
	for (node=0;node<MAX_TSENSOR_CLIENT;node++) {
		if (!bitRead(gParam.radio.client,node) && ret==0xff) {
			ret=node;
		} else
			if (gParam.radio.serieClient[node]==serie) {
				break;
			}
	}
	debugvar(node);
	debugvarln(ret);
	if (node==MAX_TSENSOR_CLIENT && ret!=0xff)
		node=ret;
	if (node < MAX_TSENSOR_CLIENT) {
		gParam.radio.addr[node]=addr;
		gParam.radio.serieClient[node]=serie;
		gParam.radio.recursoClient[node]=recursos;
		gParam.radio.timeClient[node]=millis() / 1000;
		bitSet(gParam.radio.client,node);
		#if USE_LEDS == 1
		pisca(LED,2,900);
		pisca(LED_VERMELHO,2,900);
		#endif
	}
	return;

	/*
	struct Ti2c_node *pAux;
	pAux = (struct Ti2c_node *)malloc(sizeof(struct Ti2c_node));
	pAux->addr=addr;
	//	pAux->ds = dsx;
	pAux->next = pi2c_node;
	pi2c_node=pAux;*/
}

////////////////////////////////////////////////////////
void reenvia_i2c_config(void)
{
	uint8_t addr, node;
	for (node=0;node<MAX_TSENSOR_CLIENT;node++) {
		if (bitRead(gParam.radio.client,node)) {
			addr=gParam.radio.addr[node];
			i2c_create(addr);
		}
	}

}
////////////////////////////////////////////////////////

static void i2c_scanner()
{
	byte error, address;

	//PGM_PRINTLN("Scanning...");

	for(address = 1; address < 127; address++ )
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (!error)
		{
			//PGM_PRINT("I2C device found at address 0x");
			if (address<16)
			//PGM_PRINT("0");
			//debughex(address);
			//PGM_PRINTLN("  !");
			i2c_create(address);
		}
		//else if (error==4)
		//{
			//Serial.print("Unknow error at address 0x");
			//if (address<16)
			//PGM_PRINT("0");
			//debughexln(address);
		//}
	}
}

void setup_ble(void) {
	PGM_PRINTLN(">setup_ble");
}

/**
 * \brief rotina loop ble
 */
uint8_t parse_ble(void)
{
	uint8_t client,addr, flag_send_data=false;
	struct TPackage recPack;
	uint8_t idx;
	uint8_t ret=false;
	PGM_PRINTLN(">ble");
	if (!gParam.radio.client)
		setup_i2c();
	for (client=0;client<MAX_TSENSOR_CLIENT;client++) {
		if (bitRead(gParam.radio.client,client)) {
			addr=gParam.radio.addr[client];
			PGM_PRINT("addr=");
			debugln(addr);
			Wire.beginTransmission(addr);
			Wire.write(I2C_CMD_READ_SENSORES);  // pega sensores
			//Wire.write(ccStatus);
			Wire.endTransmission();
			Wire.requestFrom(addr,sizeof(struct TPackage));
			delay(100);
			if (!Wire.readBytes((uint8_t *)&recPack,sizeof(struct TPackage))) {
				PGM_PRINTLN("i2c out");
				bitSet(gParam.radio.ComERR,client);
				continue;
			}
			bitClear(gParam.radio.ComERR,client);
			gPack.cmd=PKG_CMD_ERR_COMMAND_CLIENT;
			// testa se o node tem configuração
			if (recPack.cmd==PKG_CMD_GET_NODE) {
				i2c_create(addr);
				recPack.cmd=PKG_CMD_SEND_DATA;
				ret=true;
			}
				// retorno de execução de comanto
			if (recPack.cmd==PKG_CMD_SEND_DATA_CMD_STATUS) {
					//PGM_PRINT("% ");
					uint8_t status=recPack.data[0];
					//debugvarln(status);
					if (!status)
						bitSet(gParam.radio.hasCmdOK,client);
					else
						bitSet(gParam.radio.hasCmdERR,client);
					gPack.cmd=PKG_CMD_RECEIVE_DATA;
					timerWeb=0; // forca envio para o site
			}
			// recebendo dados
			if (recPack.cmd==PKG_CMD_SEND_DATA) { //3
					int t,t2=-1000;
					int *val=(int*)&recPack.data[1];
					uint16_t recursoNode=gParam.radio.recursoClient[client];
					debugvar(recursoNode);
					gParam.radio.timeClient[client]=millis()/1000;
					//t=gParam.radio.vccClient[client]=recPack.data[1] <<8 | recPack.data[2];
					t=gParam.radio.vccClient[client]=*val++;
					//PGM_PRINT("vcc="); debugln(t);
					// Temperatura
					//t=gParam.radio.dataClient[client][0]=(int)(recPack.data[3] <<8 | recPack.data[4]);
					t=gParam.radio.dataClient[client][0]=*val++;
					#if HAS_I2C_COPY == 1
					pTemp->temperatura=t;
					#endif
					debugvar(t);
					// sensores extras
					if (recursoNode & REC_SENSORES) {
						gParam.radio.sensor9_10[client]=recPack.data[0];
					}
					if (recursoNode & (REC_DS18B20_EXTRA | bit(REC_HUMIDADE))) {
						//t2=gParam.radio.dataClient[client][1]=(int)(recPack.data[5] <<8 | recPack.data[6]);
						t2=gParam.radio.dataClient[client][1]=*val++;
						debugvar(t2);
						#if HAS_I2C_COPY == 1
							if (recursoNode & bit(REC_HUMIDADE)) {
								humidade=t2;
								debugvarln(humidade);
							} else
								pTemp->p->temperatura=t2;
						#endif
					}
					// testa se node tem corrente
					if (recursoNode & 0b011) {   // REC_CORRENTE_30A e REC_CORRENTE_100A
						val=(int*)&recPack.data[7];
						idx=7;
						for (uint8_t fase=0; fase<3; fase++) {
							t2=gParam.radio.dataClient[client][fase+2]=*val++;
							//t2=gParam.radio.dataClient[client][fase+2]=(int)(recPack.data[idx++]);
							//idx++;
							PGM_PRINT("fase="); debugln(t2);
						}
						#if HAS_I2C_COPY == 1
						memcpy((uint8_t*)&correnteFase[0],(uint8_t*)&recPack.data[7],6);
						memcpy((uint8_t*)&gDataLogger.fase1,(uint8_t*)&recPack.data[7],6);
						#endif
					}
					#if HAS_DISPLAY == 1
					// col,lin
					lcd.setCursor(client*4,1);
					lcd.print(t/10);
					lcd.print("  ");
					#endif
					bitSet(gParam.radio.hasData,client);
					// pacote válido, testa se tem comando para enviar
					if (bitRead(gParam.radio.hasCmd,client)) {
							//debugvarln(rf24_cmd);
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
			}// else { PGM_PRINTLN ("INV"); }// 3
#if 1
			if (flag_send_data) {
				uint8_t *p= (uint8_t *)&gPack, cmd;
				Wire.beginTransmission(addr);
				Wire.write(I2C_CMD_WRITE);  // envia comandos
				Wire.write(p,sizeof(TPackage));
				Wire.endTransmission();
				Wire.requestFrom(addr,sizeof(struct TPackage));
				delay(70);
				Wire.readBytes((uint8_t *)&recPack,sizeof(struct TPackage));
			}
#endif
		}
	}

	PGM_PRINTLN("<ble");
	return ret;
}

#endif