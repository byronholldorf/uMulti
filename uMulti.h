
#ifndef _uMulti_H_
#define _uMulti_H_
#include "Arduino.h"
#include <util/atomic.h>


#if __has_include("uMultiConfig.h")
#include "uMultiConfig.h"
#endif

#ifndef UMULTI_MAX_PROCESSES
	#define UMULTI_MAX_PROCESSES 5
#endif

#define UMULTI_TERM_THREAD_MARKER 0xFF


void uMulti_yield();
void uMulti_delay_ms(uint16_t delayMs);
void uMulti_delay_sec(uint32_t sec);
void uMulti_schedule(void (*func)(), uint16_t stackSize);
void uMulti_init();

#endif /* _uMulti_H_ */
