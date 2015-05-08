#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stddef.h>


typedef struct MemoryPool MemoryPool;

extern MemoryPool *MemoryPool_create(const size_t size);
extern void MemoryPool_destroy(MemoryPool *pool);

extern void *my_malloc(MemoryPool *pool, const size_t size);
extern void my_free(MemoryPool *pool, void *memory, const size_t size);

#endif
