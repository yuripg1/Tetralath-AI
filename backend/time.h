#ifndef TIME_H
#define TIME_H

#define NSECS_PER_SECOND 1000000000
#define NSECS_PER_SECOND_INT64_T ((int64_t)(NSECS_PER_SECOND))
#define NSECS_PER_SECOND_DOUBLE ((double)(NSECS_PER_SECOND))

int64_t get_current_time_nsec(void);
int64_t seconds_to_nsec(const double seconds);
double nsec_to_seconds(const int64_t nsec);

#endif
