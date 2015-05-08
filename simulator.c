#include "simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "util.h"
#include "mymalloc.h"


#define PID_MIN       100
#define PID_MAX       999

#define CYCLES_MIN    50
#define CYCLES_MAX    550
#define CYCLES_MEAN   300
// #define CYCLES_MIN    1000
// #define CYCLES_MAX    11000
// #define CYCLES_MEAN   6000

#define SIZE_MIN      100000
#define SIZE_MAX      300000
#define SIZE_MEAN     200000

#define PROCESS_COUNT 50


typedef enum PROCESS_STATE {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_COMPLETE,
} PROCESS_STATE;

typedef struct Process {
    pid_t pid;
    size_t cycles, size, cycles_remaining;
    PROCESS_STATE state;
    char *data;
} Process;


static size_t memory = 0;
static size_t total_memory = 0;

static size_t running_count = 0;
static size_t complete_count = 0;

static Process processes[PROCESS_COUNT] = {{ 0, 0, 0, 0, PROCESS_READY, NULL }};


static pid_t random_pid(void)
{
    unsigned long delta;
    delta = random_range((PID_MAX) - PID_MIN);
    return (pid_t) (PID_MIN + delta);
}

static pid_t unique_pid(void)
{
    pid_t pid = random_pid();

    for (int i = 0; i < PROCESS_COUNT; ++i) {
        if (processes[i].pid == pid) {
            return unique_pid();
        }
    }

    return pid;
}

static size_t random_cycles(void)
{
    return (size_t) SD(CYCLES_MIN, CYCLES_MAX, CYCLES_MEAN, 'c', PROCESS_COUNT);
}

static size_t random_size(void)
{
    return (size_t) SD(SIZE_MIN, SIZE_MAX, SIZE_MEAN, 'm', PROCESS_COUNT);
}

static void draw_process_table(void)
{
    printf("\033[H\033[2J");
    printf(" MEMORY:    %zu used (%0.3lf%%), %zu free, %zu total\n",
           total_memory - memory,
           ((double) (total_memory - memory) / (double) total_memory) * 100,
           memory,
           total_memory);
    printf(" PROCESSES: %zu active, %zu ready, %zu complete, %zu total\n",
           running_count,
           PROCESS_COUNT - running_count - complete_count,
           complete_count,
           PROCESS_COUNT);
    printf(" PID    CYCLES\n");
    printf("==================================================\n");

    for (size_t i = 0; i < PROCESS_COUNT; ++i) {
        if (processes[i].state == PROCESS_RUNNING) {
            printf(" %u    ", processes[i].pid);
            for (size_t j = 0; j < processes[i].cycles_remaining; ++j) {
                // if ((j % 200) == 0) {
                if ((j % 25) == 0) {
                    printf("|");
                }
            }
            printf("\n");
        }
    }
}

void Simulator_run(size_t initial_memory, int use_custom_allocator)
{
    // seed pseudo-random number generator
    seed_rand();

    // setup non-reentrant variables for Standard Deviation,
    // which allows Simulator_run to be fully reentrant
    InitSD();

    for (size_t i = 0; i < PROCESS_COUNT; ++i) {
        // randomize pid, cycles, and memory size for each process
        processes[i].pid = unique_pid();
        processes[i].cycles = random_cycles();
        processes[i].size = random_size();

        // set process state to ready,
        // which allows Simulator_run to be fully reentrant
        processes[i].state = PROCESS_READY;
    }

    memory = initial_memory;
    total_memory = initial_memory;

    unsigned int timer = 0;
    unsigned int frame_count = 0;

    MemoryPool *memory_pool = NULL;
    if (use_custom_allocator) {
        memory_pool = MemoryPool_create(total_memory);
    }

    // disable cursor (improves flickering substantially)
    printf("\033[?25l");

    while (1) {
        // count the total number of running and complete processes
        running_count = 0;
        complete_count = 0;
        for (size_t i = 0; i < PROCESS_COUNT; ++i) {
            if (processes[i].state == PROCESS_COMPLETE) {
                ++complete_count;
            }
            else if (processes[i].state == PROCESS_RUNNING) {
                ++running_count;
            }
        }

        // if we've all processes have completed, exit the program
        if (complete_count == PROCESS_COUNT) {
            draw_process_table();
            break;
        }

        // every 50 cycles, attempt to start ready processes
        if (timer == 49) {
            timer = 0;

            // set all ready processes that will fit in memory to running state
            for (size_t i = 0; i < PROCESS_COUNT; ++i) {
                if (processes[i].state == PROCESS_READY
                    && processes[i].size < memory
                ) {
                    processes[i].state = PROCESS_RUNNING;
                    memory -= processes[i].size;
                    processes[i].cycles_remaining = processes[i].cycles;

                    if (use_custom_allocator) {
                        processes[i].data = my_malloc(memory_pool, processes[i].size);
                    }
                    else {
                        processes[i].data = malloc(processes[i].size);
                    }

                    if (processes[i].data == NULL) {
                        printf("Error: Failed to allocate "
                               "memory for process %u\n",
                               processes[i].pid);
                        break;
                    }

                    draw_process_table();
                }
            }
        }

        // update running processes
        for (size_t i = 0; i < PROCESS_COUNT; ++i) {
            if (processes[i].state == PROCESS_RUNNING) {
                if (processes[i].cycles_remaining == 0) {
                    processes[i].state = PROCESS_COMPLETE;
                    memory += processes[i].size;
                    if (processes[i].data != NULL) {
                        if (use_custom_allocator) {
                            my_free(memory_pool,
                                    processes[i].data,
                                    processes[i].size);
                            processes[i].data = NULL;
                        }
                        else {
                            free(processes[i].data);
                            processes[i].data = NULL;
                        }
                    }
                }
                else {
                    --processes[i].cycles_remaining;
                }
            }
        }

        // print process table every 25 frames
        if (frame_count == 24) {
            frame_count = 0;
            draw_process_table();
        }

        // update counters and sleep 10 milliseconds
        ++timer;
        ++frame_count;
        sleep_ms(10);
    }

    if (use_custom_allocator) {
        MemoryPool_destroy(memory_pool);
    }

    // re-enable cursor
    printf("\033[?25h");
}
