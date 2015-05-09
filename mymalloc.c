#include "mymalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAGIC_FREE '\0'


struct MemoryPool {
    uint8_t *memory;
    size_t size;
};

MemoryPool *MemoryPool_create(const size_t size)
{
    MemoryPool *pool = malloc(sizeof(MemoryPool));
    pool->size = size;

    pool->memory = malloc(size);

    // initialize pool memory to free state
    for (size_t i = 0; i < size; ++i) {
        pool->memory[i] = MAGIC_FREE;
    }

    return pool;
}

void MemoryPool_destroy(MemoryPool *pool)
{
    if (pool != NULL) {
        if (pool->memory != NULL) {
            free(pool->memory);
            pool->memory = NULL;
        }
        free(pool);
        pool = NULL;
    }
}

void *my_malloc(MemoryPool *pool, const size_t size)
{
    size_t free_count = 0;
    for (size_t i = 0; i < pool->size; ++i) {
        // attempt to find a contiguous chunk of free memory
        if (pool->memory[i] == MAGIC_FREE) {
            ++free_count;
        }
        else {
            free_count = 0;
        }

        // if the total size of the current chunk is adequate, return address
        if (free_count == size) {
            return &pool->memory[i];
        }
    }

    // failed to allocate
    return NULL;
}


void my_free(MemoryPool *pool, void *memory, const size_t size)
{
    // attempt to find the given memory in the pool, and mark it as freed
    for (size_t i = 0; i < pool->size; ++i) {
        if (&pool->memory[i] == memory) {
            memset(&pool->memory[i], MAGIC_FREE, size);
            return;
        }
    }
}
