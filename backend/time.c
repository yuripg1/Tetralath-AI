#include <stdint.h>
#include <time.h>

#include "time.h"

int64_t get_current_time_nsec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (((int64_t)(ts.tv_sec)) * NSECS_PER_SECOND_INT64_T) + ((int64_t)(ts.tv_nsec));
}

int64_t seconds_to_nsec(const double seconds) {
    return (int64_t)(seconds * NSECS_PER_SECOND_DOUBLE);
}

double nsec_to_seconds(const int64_t nsec) {
    return ((double)(nsec)) / NSECS_PER_SECOND_DOUBLE;
}
