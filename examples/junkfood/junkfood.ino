#include <uMulti.h>

void setup() {
	Serial.begin(9600);
	while (! Serial);
	uMulti_init();
}

int pizza;
int hamburger;


void pizzatask() {
	for(int i=0;i<50; i++) {
		pizza++;
		uMulti_delay_ms(200);
	}
}

void hamburgertask() {
	for(int i=0;i<50; i++) {
		hamburger++;
		uMulti_delay_ms(125);
	}
}


void loop() {
	uMulti_schedule(pizzatask);
	uMulti_schedule(hamburgertask);
	while(true) {
		uMulti_delay_sec(1);
		Serial.print("Pizza: ");
		Serial.print(pizza);
		Serial.print("    Hamburger: ");
		Serial.print(hamburger);
		Serial.println();
	}
}

