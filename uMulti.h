// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _uMulti_H_
#define _uMulti_H_
#include "Arduino.h"
#include <util/atomic.h>
//
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//void loop();
//void setup();
//#ifdef __cplusplus
//} // extern "C"
//#endif

#if __has_include("uMultiConfig.h")
#include "uMultiConfig.h"
#else
#warning "uMultiConfig.h not found. Using defaults"
#endif

#ifndef UMULTI_MAX_PROCESSES
	#define UMULTI_MAX_PROCESSES 5
#endif

#define UMULTI_NULL_STACK_MARKER 0xFF
#define UMULTI_TERM_THREAD_MARKER 0xFF

#ifndef RAMSTART
	#define RAMSTART 0x100
#endif

#ifndef RAMEND
	#define RAMEND 0x8FF
#endif



void uMulti_yield();
void uMulti_delay_ms(uint16_t delayMs);
void uMulti_delay_sec(uint16_t delayMs);
void uMulti_schedule(void (*func)());
void uMulti_init();

#endif /* _uMulti_H_ */
