#include "util.h"

#include <stdlib.h>
#include <time.h>

// static variables for standard distribution
static int process_sigma1 = 0;
static int process_sigma2 = 0;
static int process_sigma3 = 0;
static int process_sigma4 = 0;
static int process_sigma5 = 0;
static int process_sigma6 = 0;
static int process_sigma7 = 0;
static int process_sigma8 = 0;
static int cycle_Sigma1 = 0;
static int cycle_Sigma2 = 0;
static int cycle_Sigma3 = 0;
static int cycle_Sigma4 = 0;
static int cycle_Sigma5 = 0;
static int cycle_Sigma6 = 0;
static int cycle_Sigma7 = 0;
static int cycle_Sigma8 = 0;

static double random_double(const double min, const double max)
{
    double d = (double) rand() / RAND_MAX;
    return min + d * (max - min);
}

void seed_rand(void)
{
    srand((unsigned int) time(NULL));
}

unsigned long random_range(const long max)
{
    unsigned long bin_count, rand_count;
    unsigned long bin_size;
    unsigned long defect;

    bin_count = (unsigned long) max + 1;
    rand_count = (unsigned long) RAND_MAX + 1;
    bin_size = rand_count / bin_count;
    defect   = rand_count % bin_count;

    long r;
    do {
        r = rand();
    }
    while (rand_count - defect <= (unsigned long) r);

    return (unsigned long) r / bin_size;
}

void InitSD(void)
{
    // zero out
    process_sigma1 = 0;
    process_sigma2 = 0;
    process_sigma3 = 0;
    process_sigma4 = 0;
    process_sigma5 = 0;
    process_sigma6 = 0;
    process_sigma7 = 0;
    process_sigma8 = 0;
    cycle_Sigma1 = 0;
    cycle_Sigma2 = 0;
    cycle_Sigma3 = 0;
    cycle_Sigma4 = 0;
    cycle_Sigma5 = 0;
    cycle_Sigma6 = 0;
    cycle_Sigma7 = 0;
    cycle_Sigma8 = 0;
}

double SD(const unsigned int min,
          const unsigned int max,
          const unsigned int mean,
          const char unit,
          const int k)
{
    unsigned int sigma = (2 * mean - 2 * min) / 8;   /*create sigma*/

    unsigned int s0 = min;                  /*seperate sections of bell curve*/
    unsigned int s1 = min + 1 * sigma;
    unsigned int s2 = min + 2 * sigma;
    unsigned int s3 = min + 3 * sigma;
    unsigned int s4 = mean;
    unsigned int s5 = mean + 1 * sigma;
    unsigned int s6 = mean + 2 * sigma;
    unsigned int s7 = mean + 3 * sigma;
    unsigned int s8 = max;

    double t1 = k * 0.002;   /*  0.2% Bell Curve */
    double t2 = k * 0.021;   /*  2.1% Bell Curve */
    double t3 = k * 0.136;   /* 13.6% Bell Curve */
    double t4 = k * 0.341;   /* 34.1% Bell Curve */

    double n = random_double(min, max);  /*Generate number between min and max*/

    if (unit == 'm')  {   /*Memory*/
        if (n >= s0 && n < s1 && process_sigma1 < t1)  {
            process_sigma1 += 1;
            return n;
        }
        else if (n >= s1 && n < s2 && process_sigma2 < t2)  {
            process_sigma2 += 1;
            return n;
        }
        else if (n >= s2 && n < s3 && process_sigma3 < t3)   {
            process_sigma3 += 1;
            return n;
        }
        else if (n >= s3 && n < s4 && process_sigma4 < t4)  {
            process_sigma4 += 1;
            return n;
        }
        else if (n >= s4 && n < s5 && process_sigma5 < t4)  {
            process_sigma5 += 1;
            return n;
        }
        else if (n >= s5 && n < s6 && process_sigma6 < t3)  {
            process_sigma6 += 1;
            return n;
        }
        else if (n >= s6 && n < s7 && process_sigma7 < t2)  {
            process_sigma7 += 1;
            return n;
        }
        else if (n >= s7 && n <= s8 && process_sigma8 < t1)  {
            process_sigma8 += 1;
            return n;
        }
        else {
            return SD(min, max, mean, unit, k);
        }
    }
    else if (unit == 'c')  {   /*Cycle*/
        if (n >= s0 && n < s1 && cycle_Sigma1 < t1)  {
            cycle_Sigma1 += 1;
            return n;
        }
        else if (n >= s1 && n < s2 && cycle_Sigma2 < t2)  {
            cycle_Sigma2 += 1;
            return n;
        }
        else if (n >= s2 && n < s3 && cycle_Sigma3 < t3)   {
            cycle_Sigma3 += 1;
            return n;
        }
        else if (n >= s3 && n < s4 && cycle_Sigma4 < t4)  {
            cycle_Sigma4 += 1;
            return n;
        }
        else if (n >= s4 && n < s5 && cycle_Sigma5 < t4)  {
            cycle_Sigma5 += 1;
            return n;
        }
        else if (n >= s5 && n < s6 && cycle_Sigma6 < t3)  {
            cycle_Sigma6 += 1;
            return n;
        }
        else if (n >= s6 && n < s7 && cycle_Sigma7 < t2)  {
            cycle_Sigma7 += 1;
            return n;
        }
        else if (n >= s7 && n <= s8 && cycle_Sigma8 < t1)  {
            cycle_Sigma8 += 1;
            return n;
        }
        else {
            return SD(min, max, mean, unit, k);
        }
    }

    // catch-all error
    return 0;
}

void sleep_ms(unsigned int milliseconds)
{
    time_t seconds = (int) (milliseconds / 1000);

    milliseconds = milliseconds - ((unsigned int) seconds * 1000);

    struct timespec req;
    req.tv_sec = seconds;
    req.tv_nsec = (int) milliseconds * 1000000L;

    while (nanosleep(&req, &req) == -1);
}
