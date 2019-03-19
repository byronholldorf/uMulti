#include "uMulti.h"

void blinkPins() {
	while(true) {
		PINB = 0x0FF;
		uMulti_delay_ms(1000);
	}
}

void splash() {
	Serial.println("splashdown 10 sec later");
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
	uMulti_schedule_timer(10000, splash);
}

void other() {
	rocket();
	Serial.println("BONUS!");
	uMulti_delay_ms(100);
}

void prep() {
	while(true) {
		Serial.println("Launch rocket");
		uMulti_schedule(rocket,110);
		uMulti_delay_ms(150);
		uMulti_schedule(other,125);
	}
}



void setup()
{
	Serial.begin(9600);
	while (! Serial);
	Serial.println("*");
	uMulti_init();
	Serial.println(".zz");
}

void loop()
{
	Serial.println("x");
	uMulti_schedule(prep,200);
	Serial.println(".");
	//uMulti_schedule(blinkPins,120);
	Serial.println("x");
//	while(true) {
//		uMulti_delay_ms(10000);
//		for(uint8_t i=0;i<100;i++) {
//			PINB = 0x0FF;
//			uMulti_delay_ms(40);
//		}
//	}
	while(true) {
//		Serial.println(".");
//		delay(100);
		uMulti_check_timer();
	}
}
