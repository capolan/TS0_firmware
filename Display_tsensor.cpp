#include "TSensor.h"

#if HAS_DISPLAY == 1
#define BACKLIGHT_PIN     13

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

void setup_display(void)
{
	// Switch on the backlight
	//pinMode ( BACKLIGHT_PIN, OUTPUT );
	//digitalWrite ( BACKLIGHT_PIN, HIGH );
	
	lcd.begin(16,2);               // initialize the lcd
	lcd.home();

}

#endif