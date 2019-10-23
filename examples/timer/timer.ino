#include <uMulti.h>

void setup() {
	Serial.begin(9600);
	while (! Serial);
	uMulti_init();
}

void every3Sec() {
    static int16_t calls;
    Serial.print(" 3 second timer: ");
    Serial.println(calls++);
    uMulti_schedule_timer(3000, every3Sec);
}
void five() {
    Serial.println("5...");
}
void four() {
    Serial.println("4...");
}
void three() {
    Serial.println("3...");
}
void two() {
    Serial.println("2...");
}
void one() {
    Serial.println("1...");
}

void blastoff() {
    Serial.println("Blastoff!");
}

void loop() {
    every3Sec();
    uMulti_schedule_timer(1000, five);
    uMulti_schedule_timer(2000, four);
    uMulti_schedule_timer(3000, three);
    uMulti_schedule_timer(4000, two);
    uMulti_schedule_timer(5000, one);
    uMulti_schedule_timer(6000, blastoff);

	while(true) {
		uMulti_check_timer();
	}
}