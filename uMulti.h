
#ifndef _uMulti_H_
#define _uMulti_H_
#include "Arduino.h"
#include <util/atomic.h>

#define UMULTI_USE_TIMER

#if __has_include("uMultiConfig.h")
#include "uMultiConfig.h"
#endif

#ifndef UMULTI_MAX_PROCESSES
	#define UMULTI_MAX_PROCESSES 5
#endif

#ifndef UMULTI_TIMER_SLOTS
	#define UMULTI_TIMER_SLOTS 5
#endif


/**
 * Defer operations to any other threads that need processing. This (or one of the delay functions) should be called relatively often.
 */
void uMulti_yield();
/**
 * Pause this thread for delayMs millisecods, yielding to other threads until the time has expired.
 */
void uMulti_delay_ms(uint16_t delayMs);
/**
 * Pause this thread for sec seconds, yielding to other threads until the time has expired. This will not work for values greater than 4.29M seconds (49d 17hr).
 */
void uMulti_delay_sec(uint32_t sec);
/**
 * Schedule a function for execution as a thread. If no slots are available, block until one becomes available. 
 */
void uMulti_schedule(void (*func)());
/**
 * Call this in the setup() function to initialize uMulti
 */
void uMulti_init();

#ifdef UMULTI_USE_TIMER
/**
 * Check all the timers and run any that need to be run
 */
void uMulti_check_timer();
/**
 * Schedule a new operation to run after timeMs milliseconds.
 */
void uMulti_schedule_timer(long timeMs, void (*op)());
#endif

#endif /* _uMulti_H_ */
