/*
 * uMultiConfig.h
 * uMulti configuration
 */
// Change number of slots set aside for processes (maximum number of threads)
#define UMULTI_MAX_PROCESSES 2

// Timer System
// uncomment to disable the Timer System
#undef UMULTI_USE_TIMER

// Change number of slots set aside for timer operations. This is the maximum number
// of timer operations that can be waiting at any time.
#define UMULTI_TIMER_SLOTS 10