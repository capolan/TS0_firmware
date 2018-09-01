#include "TSensor.h"
#if 0
struct TPontos {
		float x,y;
	} ;
	
// 90kva
#define NRO_PONTOS 2
//struct TPontos agua_xy[] = {{204,60},{251,52},{294,47},{445,32},{720,20}};
// leitura lida, leitura real
struct TPontos agua_xy[] = {{25.7,23.7},{26.3,24}};

/*
 * Calcula valor Y dado o valor X (0<=x<=1023)
 */
#define COEF_A(x1,y1,x2,y2) ((float)((float)y2-(float)y1)/((float)x2-(float)x1))
#define COEF_B(a,x1,y1) (float)y1-a*x1

float calcula_Y(float x) {
	struct TPontos *pontos = agua_xy;
	uint8_t nro=sizeof(agua_xy)/sizeof(TPontos);
	float ret, a, b, i;
	struct TPontos *p2=pontos;
//	Serial.print("x=");
//	Serial.println(x);
	// 1 ponto = regra de 3
	if (nro == 1) {
		ret = x * (*pontos).y / (*pontos).x;
		return ret;
	}
	
	i=2;
	pontos++;
	while (x > (*pontos).x && i++ < nro) {
//		Serial.print("x=");
//		Serial.println((*pontos).x);
//		Serial.print("y=");
//		Serial.println((*pontos).y);
		pontos++;
	}
	
//	Serial.print("x1=");
//	Serial.println((*(pontos-1)).x);
//	Serial.print("y1=");
//	Serial.println((*(pontos-1)).y);
	
//	Serial.print("x2=");
//	Serial.println((*pontos).x);
//	Serial.print("y2=");
//	Serial.println((*pontos).y);
	
	a = COEF_A((*(pontos-1)).x, (*(pontos-1)).y, (*pontos).x, (*pontos).y);
	b = COEF_B(a,(*pontos).x, (*pontos).y);
	
	ret = a * x + b;

//	Serial.print("a=");
//	Serial.print(a,2);
//	Serial.print("  b=");
//	Serial.println(b,2);
//	Serial.print("x=");
//	Serial.println(x);
	return ret;
}
#endif