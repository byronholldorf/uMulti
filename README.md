
&micro;Multi - _(micro)Multi_
========


A simple "Operating System" designed for AVR microcontrollers. 


## Features
 * Easy to use understand API.
    * cooperative multitasking means context switching between threads is deterministic and repeatable. Synchronization is not required. Critical sections can be programmed as would normally be done on any microcontroller.
    * No interrupts are utilized. the millis() call is used for scheduling operations, but otherwise interrupts are left for you to use.
* Very minimal memory footprint.
    * As small as ~500 bytes of flash ROM.
    * Default configuration takes 20 bytes of RAM, but that can be made even smaller by reducing the default number of threads (5).
    * Each thread only needs about 20 bytes on the stack to store context.
    * Each threads' stack is actively compacted during context switches, so all free RAM is available to every thread.
* A timer system
    * Timed tasks can be scheduled milliseconds or seconds into the future (up to 49 days). 
    * Can be disabled to save RAM. Number of schedule task slots is compile-time configurable.
    * Timers can execute without the need for a thread.
* No critical sections--Interrupts are never disabled
    * all memory manipulation is done in a way that maintains a working stack at all times. Interrupts enjoy the same memory compaction as threads and can utilize all available RAM.


## Limitations
* context switching is very slow
    * each context switch moves a single thread's stack in RAM. This can make moving deep stacks very slow. The first thread (the thread calling loop()) is the only thread that is never moved. If speed is important, deep-stack operations will perform better on this thread.
* malloc / new() cannot be supported
    * Since RAM is always being compacted, dynamic memory allocation cannot be supported. In any case, usage of dynamic memory should usually be avoided on limited-memory target hardware.
* pointers to the stack are not supported.
    * Parts of the stack are moved during every context switch. Pointers to stack locations will not work.
* timings cannot be guaranteed
    * uMulti_delays should be considered "wait at least n sec/ms". It is up to you to yield in threads in a timely manner. Threads must wait until their turn to execute and hence cannot guarantee any sort of timing. Also the order of thread execution used (A-B-C-D-E-E-D-C-B-A, due to how memory compaction works) will cause some additional jitter.
* Possible problems with other libraries, especially ones that use delay().
    * since delay() is implemented as a busy-wait, libraries that use this method will block out all other threads. This is probably acceptable if the library needs to do short, time-sensitive operations.
* Different GCC versions may break the code
    * There are many presumptions about how GCC will generate code. Although it appears to work on avr-gcc 7.2.0, other versions could break it. _This may or may not be partially or fully fixed in future versions by using asm instead of c++._

## Usage

### Standard usage
method|usage
-----|-----
```void uMulti_init();```|Run once in the setup() function. Sets the stack pointer for thread zero.
```void uMulti_schedule(void (*func)());```|Schedule the given function to execute on a thread. If no thread is available, wait indefinitely until one becomes available. Not that this waiting can cause deadlocks.
```void uMulti_delay_ms(uint16_t delayMs);```<br> ```void uMulti_delay_sec(uint32_t sec);``` | Wait at least the specified amount of time before allowing this thread to continue. During that time the current thread will be paused and other threads will execute. _Note that times in excess of 49 days will never complete._
```void uMulti_yield();```| temporarily give up execution to a different thread. This is conceptually the same as uMulti_delay_ms(0), but more memory efficient.

### Timer System

If enabled add support for a timer system to have events happen at specified times in the future. Note that these events are not interrupt based; uMulti_check_timer must be called periodically for these operations to execute.

method|usage
-----|-----
```void uMulti_schedule_timer(long timeMs, void (*op)());```|schedule an operation to happen in the future.
```void uMulti_check_timer();```|Check and run any operations that need to be executed.

### ```uMultiConfig.h``` configuration

A few configuration options can be overridden by including a uMultiConfig.h file in your project. Here is an example:

```c++
/*
 * uMultiConfig.h
 * uMulti configuration
 */
// Change number of slots set aside for processes (maximum number of threads)
//#define UMULTI_MAX_PROCESSES 5

// Timer System
// uncomment to disable the Timer System
//#undef UMULTI_USE_TIMER

// Change number of slots set aside for timer operations. This is the maximum number
// of timer operations that can be waiting at any time.
//#define UMULTI_TIMER_SLOTS 5

```

## Simple Example application

```c++
// flash pins 11, 12, and 13 at different rates using 3 threads
#include "uMulti.h"

void setup() {
    uMulti_init();

    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
}

void flashSlow() {
    while(true) {
        digitalWrite(11, HIGH);
        uMulti_delay_ms(1000);
        digitalWrite(11, LOW);
        uMulti_delay_ms(1000);
    }
}

void flashFast() {
    while(true) {
        digitalWrite(12, HIGH);
        uMulti_delay_ms(100);
        digitalWrite(12, LOW);
        uMulti_delay_ms(100);
    }
}

void loop() {
    uMulti_schedule(flashSlow);
    uMulti_schedule(flashFast);
    while(true) {
        // 3 bloops
        for(int i=0;i<3;i++) {
            digitalWrite(13, HIGH);
            uMulti_delay_ms(100);
            digitalWrite(13, LOW);
            uMulti_delay_ms(100);
        }
        uMulti_delay_ms(2000);
    }
}

