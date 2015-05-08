#include "timer.h"

#include <stddef.h>


void Timer_start(struct timeval *start_time)
{
    gettimeofday(start_time, NULL);
}

double Timer_stop(const struct timeval start_time, struct timeval *end_time)
{
    gettimeofday(end_time, NULL);
    return (double) 1000 * (end_time->tv_sec - start_time.tv_sec)
           + (end_time->tv_usec - start_time.tv_usec) / 1000.0;
}


