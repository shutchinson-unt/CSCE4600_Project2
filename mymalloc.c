#include "mymalloc.h"

#include <unistd.h>
#include <assert.h>
#include <stdio.h>


typedef struct Block Block;
struct Block {
    Block *next;
    size_t size;
    unsigned int is_free;
    intptr_t magic;
    long pad;       // force alignment of blocks in memory
};

static const size_t kBlockSize = sizeof(Block);
static void *arena = NULL;

static Block *get_free_block(Block **tail, size_t size)
{
    // walk linked list until we find a free block that
    // can hold the requested memory size
    Block *current = arena;
    while (current != NULL
           && (!current->is_free || current->size < size)
    ) {
        // update the pointer to the current end of the arena
        *tail = current;
        current = current->next;
    }

    // once we found an adequately sized block, return its address
    return current;
}

static Block *grow_heap(Block *tail, size_t size)
{
    // create a new block at the current location of the program break
    Block *block = sbrk(0);

    // attempt to increase the program break with enough space to hold
    // the requested memory, and the block that holds it
    void *request = sbrk((intptr_t) (size + kBlockSize));

    assert((void *) block == request);

    // return NULL if we failed to increment the program break
    if (request == (void *) -1) {
        return NULL;
    }

    // if this request was made after previous allocations,
    // append the new block to the current arena end
    if (tail != NULL) {
        tail->next = block;
    }

    block->next = NULL;
    block->size = size;
    block->is_free = 0;
    block->magic = 0x12345678;

    return block;
}

void *my_malloc(size_t size)
{
    // return NULL if user attempts to allocate an invalid size
    if (size <= 0) {
        return NULL;
    }

    Block *block;

    // if this is the first request for memory in our arena
    if (arena == NULL) {
        // grow the heap, and store its address in temporary block
        block = grow_heap(NULL, size);
        if (block == NULL) {
            return NULL;
        }

        // assign the temporary block's address to arena
        arena = block;
    }
    else {
        // add the requested memory to an adequately sized block,
        // and update the arena's address to the last element
        Block *tail = arena;
        block = get_free_block(&tail, size);

        // if no block of adequate size were found, grow the heap
        if (block == NULL) {
            block = grow_heap(tail, size);
            // return NULL if growing the heap one time was not adequate
            if (block == NULL) {
                return NULL;
            }
        }
        else {
            // we found a block to hold our memory, mark it as "filled"
            block->is_free = 0;
            block->magic = 0x77777777;
        }
    }

    // return the address of block + 1
    return ++block;
}

void my_free(void *memory)
{
    // do nothing if an invalid memory address is given
    if (memory == NULL) {
        return;
    }

    // attempt to retrieve the block object belonging
    // to the given memory address
    Block *block = ((Block *) memory) - 1;

    assert(block->is_free == 0);
    assert(block->magic == 0x77777777 || block->magic == 0x12345678);

    // mark the block as free
    block->is_free = 1;
    block->magic = 0x55555555;

    // Block *tail = NULL;
    // Block *current = arena;
    // while (current != NULL
    //        && !current->is_free
    // ) {
    //     // update the pointer to the current end of the arena
    //     tail = current;
    //     current = current->next;
    // }

    // brk(tail);
}
