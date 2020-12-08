#ifndef __TIMER_H__
#define __TIMER_H__

#if defined(_WIN32)
#include <windows.h>

#define START_TIMER_BLOCK(name) \
    LARGE_INTEGER __timer_freq_name; \
    QueryPerformanceFrequency(&__timer_freq_name); \
    LARGE_INTEGER __timer_start_name; \
    QueryPerformanceCounter(&__timer_start_name);

#define END_TIMER_BLOCK(name) \
    LARGE_INTEGER __timer_end_name; \
    QueryPerformanceCounter(&__timer_end_name); \
    float __timer_result_name = ((float) (__timer_end_name.QuadPart - __timer_start_name.QuadPart))/(float) __timer_freq_name.QuadPart;

#define GET_TIMER_RESULT_MICROSECONDS(name) (__timer_result_name * 1000000.0f)
#endif

#endif
