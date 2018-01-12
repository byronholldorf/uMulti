#include "uMulti.h"

void blinkPins() {
	while(true) {
		PINB = 0x0FF;
		uMulti_delay_ms(1000);
	}
}

void rocket() {
	Serial.println("...3");
	uMulti_delay_ms(1000);
	Serial.println("...2");
	uMulti_delay_ms(1000);
	Serial.println("...1");
	uMulti_delay_ms(1000);
	Serial.println("BLASTOFF");
	Serial.flush();
}

void prep() {
	while(true) {
		Serial.println("Launch rocket");
		uMulti_schedule(rocket);
		uMulti_delay_ms(1200);
	}
}

void setup()
{
	Serial.begin(9600);
	while (! Serial);
	Serial.println("*****************");
	uMulti_init();
}

void loop()
{
	uMulti_schedule(prep);
	uMulti_schedule(blinkPins);
	while(true) {
		uMulti_delay_ms(10000);
		for(uint8_t i=0;i<100;i++) {
			PINB = 0x0FF;
			uMulti_delay_ms(40);
		}
	}
}
