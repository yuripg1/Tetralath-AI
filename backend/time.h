#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <time.h>

int64_t timespec_to_nsec(const struct timespec *ts);

int64_t get_current_time_nsec();

int64_t seconds_to_nsec(const double seconds);

double nsec_to_seconds(const int64_t nsec);

#endif
