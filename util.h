#ifndef SD_H
#define SD_H

#include <stddef.h>


extern void seed_rand(void);
extern unsigned long random_range(long max);
extern double SD(unsigned int min, unsigned int max, unsigned int mean, char unit, int k);
extern void sleep_ms(unsigned int milliseconds);

#endif
