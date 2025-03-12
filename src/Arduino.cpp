
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <stdio.h>
#include <stddef.h>
#include "Arduino.h"
#include <cinttypes>

static struct tim_reg_t
{
    volatile uint64_t ticks = 0; /**< System ticks counter (Jiffies) */
    volatile uint64_t ts = 0;
} __systime;

uint64_t sys_t::get_ticks()
{
#if 0
    struct timeval tv;
    gettimeofday(&tv, NULL);
    __systime.ticks = (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return __systime.ticks;
#else 
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    __systime.ticks = static_cast<uint64_t>(duration.count());
#endif
    return __systime.ticks;
}

tim_reg_t *sys_t::ticks_ptr() { return &__systime; }

void sys_t::sleep_until(uint32_t timeout)
{
    sys_t::delay(timeout);
}

bool sys_t::waits_for(uint64_t timeout, void *arg, bool (*cb)(void *))
{
    uint64_t start = get_ticks();
    while (get_ticks() < (start + timeout)) {
        if (cb(arg)) {
            return true;
        }
    }

    return false;
}

void sys_t::delay(uint32_t ms)
{
    if (ms == 0) {
        return;
    }

    uint64_t start = get_ticks();
    while (get_ticks() < (start + ms)) {
    }
}

#if 0 // already in TinyGPS++
unsigned long millis(void) {
#ifdef _WIN32
    // Windows: FILETIME is in 100-nanosecond intervals since January 1, 1601.
    FILETIME ft;
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    // Convert to Unix epoch (January 1, 1970) in milliseconds.
    return (uli.QuadPart - 116444736000000000ULL) / 10000;
#else
    // POSIX: gettimeofday returns time in seconds and microseconds.
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}
#endif

double degrees(double radians)
{
    return radians * (180.0 / M_PI);
}

double sq(double in)
{
    return in * in;
}

double radians(double degrees)
{
    return degrees * (M_PI / 180.0);
}
