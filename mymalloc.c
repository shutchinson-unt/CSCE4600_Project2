#include "mymalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAGIC_FREE      0xFF


struct MemoryPool {
    uint8_t *memory;
    size_t size;
};

MemoryPool *MemoryPool_create(const size_t size)
{
    MemoryPool *memory_pool = malloc(sizeof(MemoryPool));
    memory_pool->size = size;

    memory_pool->memory = malloc(size);

    for (size_t i = 0; i < size; ++i) {
        memory_pool->memory[i] = MAGIC_FREE;
    }

    return memory_pool;
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
        if (pool->memory[i] == MAGIC_FREE) {
            ++free_count;
        }
        else {
            free_count = 0;
        }

        // if the total size of the current chunk is adequate, return address
        if (free_count == size) {
            // printf("MALLOC chunk size: %zu, at ADDR: %p\n",
            //        size,
            //        &pool->memory[i]);
            return &pool->memory[i];
        }
    }

    // failed to allocate
    return NULL;
}


void my_free(MemoryPool *pool, void *memory, const size_t size)
{
    for (size_t i = 0; i < pool->size; ++i) {
        if (&pool->memory[i] == memory) {
            // printf("FREEING chunk size: %zu, at ADDR: %p\n",
            //        size,
            //        &pool->memory[i]);
            memset(&pool->memory[i], MAGIC_FREE, size);
            return;
        }
    }
}
