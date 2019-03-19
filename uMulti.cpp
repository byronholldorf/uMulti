#include "uMulti.h"

volatile uint8_t _uMulti_currentProcess;


struct _uMulti_Process {

	union {
		byte* stack_current;
		uint16_t stack;
	};
	uint16_t stack_end;

};

volatile _uMulti_Process _uMulti_processes[UMULTI_MAX_PROCESSES];


////////////////// TIMER OPERATIONS ////////////////////
#ifdef UMULTI_USE_TIMER

struct _uMulti_Timer {
	void (*op)();
	uint32_t milliTarget;
};

_uMulti_Timer _uMulti_timer_slots[UMULTI_TIMER_SLOTS];


uint8_t _uMulti_timerShouldExecute(_uMulti_Timer& t) {
	uint32_t time = millis();
	return t.milliTarget != 0 && (t.milliTarget - time > 4294967296L - 60000L);
}


uint8_t _uMulti_findEmptyTimerSlot() {
	while(true) {
		for(uint8_t i=0; i<UMULTI_TIMER_SLOTS; i++) {
			if(_uMulti_timer_slots[i].milliTarget == 0) {
				return i;
			}
		}
		uMulti_delay_ms(1);
	}
}


void uMulti_check_timer() {
	for(uint8_t i=0; i<UMULTI_TIMER_SLOTS; i++) {
		if(_uMulti_timerShouldExecute(_uMulti_timer_slots[i])) {
			_uMulti_timer_slots[i].milliTarget = 0;
			_uMulti_timer_slots[i].op();
		}
	}
}

void uMulti_schedule_timer(long timeMs, void (*op)()) {
	uint8_t slot = _uMulti_findEmptyTimerSlot();

	_uMulti_timer_slots[slot].milliTarget = millis() + timeMs;
	_uMulti_timer_slots[slot].op = op;
}

#endif

////////////////// DEBUG OPERATIONS ////////////////////
#ifdef UMULTI_DEBUG
void uMulti_print_stacks() {
	Serial.print(F("*UMULTI:"));
	for(int i=0; i<5; i++) {
		Serial.print(_uMulti_processes[i].stack);
		Serial.print("/");
		Serial.print(_uMulti_processes[i].stack_end);
		Serial.print("   ");
	}
	Serial.println(" ");
	Serial.flush();
}

void debug(const char* text, uint16_t num) {
	Serial.print(F("*UMULTI:"));
	Serial.print(text);
	Serial.println(num);
	Serial.flush();
}

void debug(const char* text) {
	Serial.print(F("*UMULTI:"));
	Serial.print(text);
	Serial.flush();
}
#endif

////////////////////////////////////////////////////////

uint8_t _uMulti_nextProcess(uint8_t current) {
	uint8_t newProcess = (current + 1);
	if(_uMulti_processes[newProcess].stack_end == 0 || newProcess >= UMULTI_MAX_PROCESSES) {
		return 0;
	}

	return newProcess;
}


void _uMulti_compressMem(uint8_t proc) {
	uMulti_debug("A1");
	uint8_t cur = proc + 1;
	uint16_t numBytes = _uMulti_processes[proc].stack_end
			- _uMulti_processes[proc - 1].stack_end;
	uMulti_debug("A2");
	while(cur < UMULTI_MAX_PROCESSES && _uMulti_processes[cur].stack_end != 0) {
		memcpy(
			(byte*)(_uMulti_processes[cur].stack - numBytes),
			(byte*)(_uMulti_processes[cur].stack),
			_uMulti_processes[cur].stack_end - _uMulti_processes[cur].stack+1);

		_uMulti_processes[cur - 1].stack = _uMulti_processes[cur].stack - numBytes;
		_uMulti_processes[cur - 1].stack_end = _uMulti_processes[cur].stack_end - numBytes;
		cur++;
	}
	_uMulti_processes[cur - 1].stack_end = 0;
	uMulti_debug("AE");

}

void _uMulti_switch(uint8_t oldProc, uint8_t newProc){
	asm("push r2\n"
		"push r3\n"
		"push r4\n"
		"push r5\n"
		"push r6\n"
		"push r7\n"
		"push r8\n"
		"push r9\n"
		"push r10\n"
		"push r11\n"
		"push r12\n"
		"push r13\n"
		"push r14\n"
		"push r15\n"
		"push r16\n"
		"push r17\n"
		"push r28\n"
		"push r29");
	uMulti_debug("B1");

	if(_uMulti_currentProcess == UMULTI_TERM_THREAD_MARKER) {
		// run on stack 0. It's the only one we know we won't move
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			SP = _uMulti_processes[0].stack;
		}
		uMulti_debug("B2");
		_uMulti_compressMem(oldProc);
		uMulti_debug("B3");
	} else {
		uMulti_debug("B4");
		_uMulti_processes[oldProc].stack = SP;
		uMulti_debug("B5");
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			SP = _uMulti_processes[newProc].stack;
		}

		uMulti_debug("B6");
	}

	_uMulti_currentProcess = newProc;
	uMulti_debug("BE");

	 asm("pop 29\n"
		 "pop r28\n"
		 "pop r17\n"
		 "pop r16\n"
		 "pop r15\n"
		 "pop r14\n"
		 "pop r13\n"
		 "pop r12\n"
		 "pop r11\n"
		 "pop r10\n"
		 "pop r9\n"
		 "pop r8\n"
		 "pop r7\n"
		 "pop r6\n"
		 "pop r5\n"
		 "pop r4\n"
		 "pop r3\n"
		 "pop r2\n");
}


void uMulti_yield() {
	uint8_t newProcess = _uMulti_nextProcess(_uMulti_currentProcess);
	uMulti_debug("D1");
	_uMulti_switch(_uMulti_currentProcess, newProcess);
	uMulti_debug("DE");
}

void uMulti_delay_sec(uint32_t sec) {
	uint32_t offset = millis();

	while((millis() - offset) / 1000 < sec) {
		uMulti_yield();
	}
}

void uMulti_delay_ms(uint16_t delayMs) {
	uint32_t offset = millis();
	uMulti_debug("C1");

	while(millis() - offset < delayMs) {
		uMulti_debug("C2");
		uMulti_yield();
		uMulti_debug("C3");
	}
	uMulti_debug("CE");
}

void uMulti_schedule(void (*func)(), uint16_t stackSize) {
	volatile uint8_t proc;
	uMulti_debug("S1");
	while(_uMulti_processes[UMULTI_MAX_PROCESSES - 1].stack_end != 0) {
		uMulti_yield();
	}

	proc = 1;
	while(_uMulti_processes[proc].stack_end != 0) {
		proc++;
	}

	uint16_t stack = _uMulti_processes[proc - 1].stack_end + stackSize;

	// cheap trick. store the stack, then return, but we'll immediately switch to a different stack and reuse this
	// one later
	uMulti_debug("S2");
	_uMulti_switch(_uMulti_currentProcess, _uMulti_currentProcess);
	uMulti_debug("S3");
	if(_uMulti_processes[proc].stack_end == 0) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			SP = stack;
		}
		uMulti_debug("S4");
		_uMulti_processes[proc].stack_end = stack;

		// save new stack but return with old. This call will actually return above, on the original stack.
		// a later switch will drop us back here
		_uMulti_switch(proc, _uMulti_currentProcess);
		uMulti_debug("S5");
		func();
		uMulti_debug("S6");

		uint8_t terminateProcess = _uMulti_currentProcess;
		_uMulti_currentProcess = UMULTI_TERM_THREAD_MARKER;
		uMulti_debug("S7");
		_uMulti_switch(terminateProcess, 0);
		// never returns
	}
	uMulti_debug("SE");
}

void uMulti_init() {

	_uMulti_processes[0].stack = SP;
	_uMulti_processes[0].stack_end = ((uint16_t)__malloc_heap_start);

#ifdef DEBUG_STACK
	memset((uint16_t)__malloc_heap_start), 0xCC, SP - ((uint16_t)__malloc_heap_start));
#endif

}
