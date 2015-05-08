#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>


extern void Timer_start(struct timeval *start_time);
extern double Timer_stop(const struct timeval start_time,
                         struct timeval *end_time);

#endif
