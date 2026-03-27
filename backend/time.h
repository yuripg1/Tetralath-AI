#ifndef TIME_H
#define TIME_H

int64_t get_current_time_nsec(void);
int64_t seconds_to_nsec(const double seconds);
double nsec_to_seconds(const int64_t nsec);

#endif
