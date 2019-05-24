#include "uMulti.h"

volatile uint8_t _uMulti_currentProcess; // = 0
volatile int8_t _uMulti_schedDirection; // = 0

extern byte *__bss_end;

typedef byte* STACK;
#define _UMULTI_STACK_END (__bss_end)
#define _UMULTI_IS_LAST(a) ((a) == UMULTI_MAX_PROCESSES - 1 || !_uMulti_processes[(a) + 1].stack_start)

struct _uMulti_Process {
	STACK stack_start;
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
		uMulti_check_timer();
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

uint8_t _uMulti_nextProcess(uint8_t current) {
	uint8_t newProcess = (current + 1);
	if(_uMulti_processes[newProcess].stack_start == 0 || newProcess >= UMULTI_MAX_PROCESSES) {
		return 0;
	}

	return newProcess;
}

// called during context switch. We can't use the stack and must be inlined.
__attribute__((always_inline))
inline void _uMulti_rollLeft() {
	STACK startCopy = max(_uMulti_processes[_uMulti_currentProcess + 1].stack_start, _UMULTI_STACK_END);
	STACK endCopy = startCopy + (_uMulti_processes[_uMulti_currentProcess].stack_start - (byte*)SP);
	_uMulti_processes[_uMulti_currentProcess].stack_start = endCopy;

	// move chunk of stack to [startCopy:endCopy] (to the left)
	asm volatile(
			"1: "
			"pop __tmp_reg__;" "\n"
			"st %a0+, __tmp_reg__;" "\n"
			"cp %B0, %B1;" "\n"
			"brne 1b;" "\n"
			"cp %A0, %A1;" "\n"
			"brne 1b;" "\n"
			:: "e" (startCopy), "e" (endCopy));
	_uMulti_currentProcess--;

}

// called during context switch. We can't use the stack and must be inlined.
__attribute__((always_inline))
inline void _uMulti_rollRight() {
	STACK startCopy = _uMulti_processes[_uMulti_currentProcess + 1].stack_start;
	_uMulti_processes[_uMulti_currentProcess + 1].stack_start = (byte*)SP;
	STACK endCopy;
	if(_uMulti_currentProcess == UMULTI_MAX_PROCESSES - 2 || _uMulti_processes[_uMulti_currentProcess + 2].stack_start == 0) {
		endCopy = _UMULTI_STACK_END;
	} else {
		endCopy = _uMulti_processes[_uMulti_currentProcess + 2].stack_start;
	}

	// move [endCopy:startCopy] to stack (to the right)
	asm volatile(
			"1: "
			"ld __tmp_reg__, -%a0;" "\n"
			"push __tmp_reg__;" "\n"
			"cp %B0, %B1;" "\n"
			"brne 1b;" "\n"
			"cp %A0, %A1;" "\n"
			"brne 1b;" "\n"
			::"e" (startCopy), "e" (endCopy));
	_uMulti_currentProcess++;
}


__attribute__ ((noinline))
__attribute__ ((naked))
void _uMulti_switch() {
	// https://www.microchip.com/webdoc/AVRLibcReferenceManual/FAQ_1faq_reg_usage.html
	// r0 can be used as junk

	// save all call-saved regs
	// r2-r17, r28-r29
	// the compiler assumes the rest might be clobbered anyway
	asm volatile (
			"push r2;" "\n"
			"push r3;" "\n"
			"push r4;" "\n"
			"push r5;" "\n"
			"push r6;" "\n"
			"push r7;" "\n"
			"push r8;" "\n"
			"push r9;" "\n"
			"push r10;" "\n"
			"push r11;" "\n"
			"push r12;" "\n"
			"push r13;" "\n"
			"push r14;" "\n"
			"push r15;" "\n"
			"push r16;" "\n"
			"push r17;" "\n"
			"push r28;" "\n"
			"push r29;" "\n");

	if((_uMulti_currentProcess == 0 && _uMulti_schedDirection) ||
			(_UMULTI_IS_LAST(_uMulti_currentProcess) && !_uMulti_schedDirection)) {
		_uMulti_schedDirection = ~_uMulti_schedDirection;
	} else {
		if(_uMulti_schedDirection) {
			_uMulti_rollLeft();
		} else {
			_uMulti_rollRight();
		}
	}

	asm volatile (
			"pop r29;" "\n"
			"pop r28;" "\n"
			"pop r17;" "\n"
			"pop r16;" "\n"
			"pop r15;" "\n"
			"pop r14;" "\n"
			"pop r13;" "\n"
			"pop r12;" "\n"
			"pop r11;" "\n"
			"pop r10;" "\n"
			"pop r9;" "\n"
			"pop r8;" "\n"
			"pop r7;" "\n"
			"pop r6;" "\n"
			"pop r5;" "\n"
			"pop r4;" "\n"
			"pop r3;" "\n"
			"pop r2;" "\n"
			"ret;" "\n");

}



void uMulti_yield() {
	_uMulti_switch();
}

void uMulti_delay_sec(uint32_t sec) {
	uint32_t offset = millis();

	while((millis() - offset) / 1000 < sec) {
		uMulti_yield();
	}
}

void uMulti_delay_ms(uint16_t delayMs) {
	uint32_t offset = millis();

	while(millis() - offset < delayMs) {
		uMulti_yield();
	}
}

__attribute__ ((noinline))
__attribute__ ((naked))
void _uMulti_terminate() {
	for(uint8_t proc = _uMulti_currentProcess; proc < UMULTI_MAX_PROCESSES - 1; proc++) {
		_uMulti_processes[proc].stack_start = _uMulti_processes[proc + 1].stack_start;
	}
	_uMulti_processes[UMULTI_MAX_PROCESSES - 1].stack_start = 0;

	// current stack and our right neighbor should be at the same spot.
	_uMulti_currentProcess--;

	asm volatile (
				"pop r29;" "\n"
				"pop r28;" "\n"
				"pop r17;" "\n"
				"pop r16;" "\n"
				"pop r15;" "\n"
				"pop r14;" "\n"
				"pop r13;" "\n"
				"pop r12;" "\n"
				"pop r11;" "\n"
				"pop r10;" "\n"
				"pop r9;" "\n"
				"pop r8;" "\n"
				"pop r7;" "\n"
				"pop r6;" "\n"
				"pop r5;" "\n"
				"pop r4;" "\n"
				"pop r3;" "\n"
				"pop r2;" "\n"
				"ret;" "\n");
}


void uMulti_schedule(void (*func)()) {

	while(_uMulti_processes[UMULTI_MAX_PROCESSES - 1].stack_start != 0) {
		uMulti_yield();
	}

	// rotate the stack to the last so we can write the next process.
	uint8_t rollcount = 1;
	while(!_UMULTI_IS_LAST(_uMulti_currentProcess)) {
		_uMulti_rollRight();
		rollcount++;
	}

	_uMulti_currentProcess++;
	_uMulti_processes[_uMulti_currentProcess].stack_start = (byte*)SP;

	void (*returnSP)() = &_uMulti_terminate;

	asm volatile (
			"push %A0;" "\n"
			"push %B0;" "\n"
		::"e" (returnSP));

	asm volatile (
			"push %A0;" "\n"
			"push %B0;" "\n"
		::"e" (func));

	asm volatile (
			"push r2;" "\n"
			"push r3;" "\n"
			"push r4;" "\n"
			"push r5;" "\n"
			"push r6;" "\n"
			"push r7;" "\n"
			"push r8;" "\n"
			"push r9;" "\n"
			"push r10;" "\n"
			"push r11;" "\n"
			"push r12;" "\n"
			"push r13;" "\n"
			"push r14;" "\n"
			"push r15;" "\n"
			"push r16;" "\n"
			"push r17;" "\n"
			"push r28;" "\n"
			"push r29;" "\n");

	while(rollcount--) {
		_uMulti_rollLeft();
	}
}

void uMulti_init() {

	_uMulti_processes[0].stack_start = (byte*)SP;

#ifdef DEBUG_STACK
	memset((uint16_t)__malloc_heap_start), 0xCC, (byte*)SP - ((uint16_t)__malloc_heap_start));
#endif

}
