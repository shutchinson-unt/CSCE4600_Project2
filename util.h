#ifndef UTIL_H
#define UTIL_H


extern void seed_rand(void);
extern unsigned long random_range(const long max);
extern void InitSD(void);
extern double SD(const unsigned int min,
                 const unsigned int max,
                 const unsigned int mean,
                 const char unit,
                 const int k);
extern void sleep_ms(unsigned int milliseconds);

#endif
