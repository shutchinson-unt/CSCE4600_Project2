#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stddef.h>


extern void *my_malloc(size_t size);
extern void my_free(void *memory);

#endif
