#include "uMulti.h"

volatile uint8_t _uMulti_currentProcess;

struct _uMulti_Process {
	volatile byte stackPL;
	volatile byte stackPH;
};

volatile _uMulti_Process _uMulti_processes[UMULTI_MAX_PROCESSES];

/**
 * Tell the compiler we've clobbered the registers so it generates code to store them on the stack
 */
void _uMulti_clobber() {
	asm( "":::"r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","r15","r16","r17","r28","r29","memory");
}


uint8_t _uMulti_nextProcess(uint8_t current) {
	uint8_t newProcess = current;
	do {
		newProcess = (newProcess + 1) % UMULTI_MAX_PROCESSES;
	} while (_uMulti_processes[newProcess].stackPH == UMULTI_NULL_STACK_MARKER);
	return newProcess;
}

void _uMulti_switch(uint8_t oldProc, uint8_t newProc){
	if(_uMulti_currentProcess == UMULTI_TERM_THREAD_MARKER) {
		_uMulti_processes[oldProc].stackPH = UMULTI_NULL_STACK_MARKER;
	} else {
		_uMulti_processes[oldProc].stackPL = SPL;
		_uMulti_processes[oldProc].stackPH = SPH;
	}
	_uMulti_clobber();
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		SPH = _uMulti_processes[newProc].stackPH;
		SPL = _uMulti_processes[newProc].stackPL;
	}
	_uMulti_currentProcess = newProc;
}

void uMulti_yield() {
	uint8_t newProcess = _uMulti_nextProcess(_uMulti_currentProcess);
	_uMulti_switch(_uMulti_currentProcess, newProcess);
}

void uMulti_delay_sec(uint32_t delayMs) {
	uint32_t offset = millis();

	while((millis() - offset) / 1000 < delayMs) {
		uMulti_yield();
	}
}

void uMulti_delay_ms(uint16_t delayMs) {
	uint32_t offset = millis();

	while(millis() - offset < delayMs) {
		uMulti_yield();
	}
}

void uMulti_schedule(void (*func)()) {
	volatile uint8_t proc;
	do {
		proc = 1;
		while(proc < UMULTI_MAX_PROCESSES) {
			if(_uMulti_processes[proc].stackPH == UMULTI_NULL_STACK_MARKER) {
				break;
			}
			proc++;
		}
		if(proc >= UMULTI_MAX_PROCESSES) {
			uMulti_yield();
		}
	} while (proc >= UMULTI_MAX_PROCESSES);


	uint16_t stack = ((RAMEND - RAMSTART) / UMULTI_MAX_PROCESSES) * (UMULTI_MAX_PROCESSES - proc) + RAMSTART;

	// cheap trick. store the stack, then return, but we'll immediately switch to a different stack and reuse this
	// one later
	_uMulti_switch(_uMulti_currentProcess, _uMulti_currentProcess);
	if(_uMulti_processes[proc].stackPH == UMULTI_NULL_STACK_MARKER) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			SPL = stack & 0x0FF;
			SPH = stack >> 8;
		}

		// save new stack but return with old. This call will actually return above, on the original stack.
		// a later switch will drop us back here
		_uMulti_switch(proc, _uMulti_currentProcess);
		func();

		uint8_t terminateProcess = _uMulti_currentProcess;
		_uMulti_currentProcess = UMULTI_TERM_THREAD_MARKER;
		_uMulti_switch(terminateProcess, 0);
		// never returns
	}
}

void uMulti_init() {
	for(uint8_t i=0; i<UMULTI_MAX_PROCESSES; i++) {
		_uMulti_processes[i].stackPH = UMULTI_NULL_STACK_MARKER;
	}

	_uMulti_processes[0].stackPH = SPH;
	_uMulti_processes[0].stackPL = SPL;
}
