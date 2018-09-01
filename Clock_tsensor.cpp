#include "TSensor.h"

#if HAS_CLOCK == 1
#include <Time_c.h>         //http://www.arduino.cc/playground/Code/Time
#include <Build_Defs.h>

#if USE_REAL_RTC == 1
#include <DS1307RTC/DS1307RTC.h>
#endif

#if 1
time_t requestSync()
{
	//	SSdebug.print(TIME_REQUEST,BYTE);
	#if USE_LOCATION == 1
	if (loc.lat)
		setTime(loc.hour,loc.minute,loc.second,loc.day,loc.month,loc.year); 
	//setTime(hr,min,sec,day,month,yr);
	#endif
	return 0; // the time will be sent later in response to serial mesg
}
#endif

#if DEBUG > 10

void digitalClockDisplay(){
	char p[20];
	SSdebug.println(formatHora(p,now()));
}

#endif

char *formatHora(char *p, time_t t) {
	int h;
	h=day(t);
	p[0]=h<=9?'0':h/10+'0';
	p[1]=h%10+'0';
	p[2]='/';
	h=month(t);
	p[3]=h<=9?'0':h/10+'0';
	p[4]=h%10+'0';
	p[5]='/';
	h=year(t);
	p[6]=h / 1000 + '0';
	p[7]=(h / 100) % 10 + '0';
	p[8]=(h / 10) % 10 + '0';
	p[9]=h%10+'0';
	p[10]=' ';

	h=hour(t);
	p[11]=h<=9?'0':h/10+'0';
	p[12]=h%10+'0';
	p[13]=':';
	h=minute(t);
	p[14]=h<=9?'0':h/10+'0';
	p[15]=h%10+'0';
	p[16]=':';
	h=second(t);
	p[17]=h<=9?'0':h/10+'0';
	p[18]=h%10+'0';
	p[19]='\0';
	return p;
}

void setup_clock(void)
{
#if USE_REAL_RTC == 1
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if (timeStatus() != timeSet) {
  PGM_PRINTLN("Unable to sync with the RTC");
  } else {
  PGM_PRINTLN("RTC has set the system time");
  }
#else
	#if USE_LOCATION == 1
	requestSync();
	setSyncProvider( requestSync);  //set function to call when sync required
	debugln (formatHora(reqHTTP,now()));
	#else
	setTime((__TIME__[0]-'0')*10 + __TIME__[1]-'0',
	(__TIME__[3]-'0')*10 + __TIME__[4]-'0',
	0,
	(BUILD_DAY_CH0-'0')*10 + BUILD_DAY_CH1-'0',
	(BUILD_MONTH_CH0-'0')*10 + BUILD_MONTH_CH1-'0',
	(__DATE__[7]-'0')*1000 + (__DATE__[8]-'0')*100 + (__DATE__[9]-'0')*10 + __DATE__[10]-'0');	
	#endif
#endif
}

//                         1         2         3
//               0123456789012345678901234567890
//char date[] = "Date: Tue, 15 Nov 1994 08:12:31 GMT";
void process_date(char *data) {
	#if USE_LOCATION == 1
	requestSync();
	debugln (formatHora(reqHTTP,now()));
	#else
	uint8_t i;
	uint8_t dia,mes;
	int ano=0;
	uint8_t h,m=255,s=255;
	if (!strncmp(data,"Date:",5)) {
		// dia
		dia=atoi(data+11);
		//mes=....
		ano=atoi(data+18);
		// hora
		h=atoi(data+23);  // GMT
		m=atoi(data+26);
		s=atoi(data+29);
		
		//Jan Feb Mar	Apr	May	Jun	Jul	Aug	Sep	Oct	Nov	Dez
		switch (data[14]) {
			case 'F':
			mes=2;
			break;
			case 'S':
			mes=9;
			break;
			case 'O':
			mes=10;
			break;
			case 'N':
			mes=11;
			break;
			case 'D':
			mes=12;
			break;
			case 'A': // Apr e Aug
			if (data[15]=='p') mes=4;
			else mes=8;
			break;
			case 'J': // Jan, Jun, Jul
			if (data[15]=='a') mes=1;
			else if (data[16]='n') mes=6;
			else mes=7;
			break;
			case 'M': // Mar, May
			if (data[16]=='r') mes=3;
			else mes=5;
			break;
		}
		
		if (ano >= 2017 && m<=59 && s<=59) {
		setTime(h,m,s,dia,mes,ano);
		//adjustTime(-3600*3); // GMT - 3 horas
		bitSet(ccStatus,CC_CLOCK_OK);
		#if DEBUG > 0
		char p[21];
		debugln (formatHora(p,now()));
		#endif
		}
	}
	#endif
}

#endif
