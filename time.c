#include <stdint.h>
#include <time.h>

#include "time.h"

int64_t timespec_to_nsec(const struct timespec *ts) {
    return (int64_t)ts->tv_sec * 1000000000LL + (int64_t)ts->tv_nsec;
}

int64_t get_current_time_nsec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return timespec_to_nsec(&ts);
}

int64_t seconds_to_nsec(double seconds) {
    return (int64_t)(seconds * 1e9);
}

double nsec_to_seconds(int64_t nsec) {
    return nsec / 1e9;
}
