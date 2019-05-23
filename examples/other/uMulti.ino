#include "uMulti.h"

void setup() {
	Serial.begin(57600);
	while (! Serial);
	uMulti_init();
}


#define TEST0

#ifdef TEST0

int pizza;

void pizzatask() {
	for(int i=0;i<50; i++) {
		pizza++;
		uMulti_delay_ms(200);
	}
}

int hamburger;

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


#endif


#ifdef TEST1

uint8_t count;

void test1Func() {
	uint8_t local = count++;
	long time = micros();
	Serial.print(time);
	Serial.println("uS test1-A ");
	uMulti_delay_ms(100);
	Serial.print(micros()-time);
	Serial.println("uS test1-B ");
	uMulti_delay_ms(100);
	Serial.print(micros()-time);
	Serial.println("uS end ");
}

void quick() {
	uMulti_delay_ms(5);
	Serial.println("Do quick!");
}

void scheduler() {
	Serial.println("sched test ");
	uMulti_schedule(test1Func);
	Serial.println("sched quick ");
	uMulti_schedule(quick);
	uMulti_delay_ms(10);
}

void loop() {
	while(true) {
		uMulti_schedule(scheduler);
		uMulti_schedule(test1Func);
		uMulti_delay_ms(50);
	}
}

#endif


#ifdef TEST2

void test50() {
	Serial.println("test 50");
	uMulti_delay_ms(50);
}

void test40() {
	Serial.println("test 40");
	uMulti_delay_ms(40);
}

void test10() {
	Serial.println("test 10");
	uMulti_delay_ms(10);
}


void loop() {
	uMulti_schedule(test40);
	uMulti_schedule(test10);
	uMulti_schedule(test10);
	uMulti_schedule(test50);
	uMulti_delay_ms(20);
	uMulti_schedule(test10);
	uMulti_schedule(test10);
	uMulti_delay_ms(22);
	uMulti_schedule(test10);

	uMulti_delay_ms(15);

}


#endif

#ifdef TEST3

void timer() {
	Serial.print(millis());
	Serial.println("ms");
	uMulti_schedule_timer(250, timer);
}

void loop() {
	Serial.println("scheduling");
	//Fix deadlock. schedule waits for thread, but if we're the only thread checking, we might stop everything.
	uMulti_schedule_timer(250, timer);
	uMulti_check_timer();
	uMulti_delay_ms(100);
}


#endif


#ifdef TEST4


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
		uMulti_schedule(rocket);
		uMulti_delay_ms(150);
		uMulti_schedule(other);
	}
}


void test() {
	Serial.println("short");
}

void loop()
{
	//Serial.println("x");
	uMulti_schedule(prep);
	//Serial.println(".");
	//uMulti_schedule(blinkPins,120);
//	Serial.println("x");
//	while(true) {
//		uMulti_delay_ms(10000);
//		for(uint8_t i=0;i<100;i++) {
//			PINB = 0x0FF;
//			uMulti_delay_ms(40);
//		}
//	}
	while(true) {
		//Serial.println(".");
		uMulti_delay_ms(100);
		//uMulti_check_timer();

		//Serial.println("B");
		uMulti_yield();
	}
}
#endif