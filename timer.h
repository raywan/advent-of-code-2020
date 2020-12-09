#ifndef __TIMER_H__
#define __TIMER_H__

#if defined(_WIN32)
#include <windows.h>

#define START_TIMER_BLOCK(name) \
    LARGE_INTEGER __timer_freq_##name; \
    QueryPerformanceFrequency(&__timer_freq_##name); \
    LARGE_INTEGER __timer_start_##name; \
    QueryPerformanceCounter(&__timer_start_##name);

#define END_TIMER_BLOCK(name) \
    LARGE_INTEGER __timer_end_##name; \
    QueryPerformanceCounter(&__timer_end_##name); \
    float __timer_result_##name = ((float) (__timer_end_##name.QuadPart - __timer_start_##name.QuadPart))/(float) __timer_freq_##name.QuadPart;

#define GET_TIMER_RESULT_MICROSECONDS(name) (__timer_result_##name * 1000000.0f)
#define PRINT_TIMER_RESULT(name) printf("%s: %fus\n", #name, GET_TIMER_RESULT_MICROSECONDS(name))
#endif

#endif
