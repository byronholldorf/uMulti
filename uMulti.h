
#ifndef _uMulti_H_
#define _uMulti_H_
#include "Arduino.h"
#include <util/atomic.h>

#define UMULTI_USE_TIMER

#if __has_include("uMultiConfig.h")
#include "uMultiConfig.h"
#endif

//#define UMULTI_DEBUG

#ifndef UMULTI_MAX_PROCESSES
	#define UMULTI_MAX_PROCESSES 5
#endif

#ifndef UMULTI_TIMER_SLOTS
	#define UMULTI_TIMER_SLOTS 5
#endif

#define UMULTI_TERM_THREAD_MARKER 0xFF


void uMulti_yield();
void uMulti_delay_ms(uint16_t delayMs);
void uMulti_delay_sec(uint32_t sec);
void uMulti_schedule(void (*func)(), uint16_t stackSize);
void uMulti_init();

#ifdef UMULTI_DEBUG
void uMulti_print_stacks();
#else
#define uMulti_debug(a)
#endif

#ifdef UMULTI_USE_TIMER
void uMulti_check_timer();
void uMulti_schedule_timer(long timeMs, void (*op)());
#endif

#endif /* _uMulti_H_ */
