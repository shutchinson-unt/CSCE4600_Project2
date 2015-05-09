#include "simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "mymalloc.h"
#include "util.h"
#include "timer.h"


#define PID_MIN       100
#define PID_MAX       999

#define CYCLES_MIN    50
#define CYCLES_MAX    550
#define CYCLES_MEAN   300

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
    double malloc_time, free_time;
} Process;


static size_t memory = 0;
static size_t total_memory = 0;

static size_t running_count = 0;
static size_t complete_count = 0;

static size_t active_process_count = 0;
static size_t active_process_limit = 0;

static Process processes[PROCESS_COUNT] = {{ 0, 0, 0, 0, PROCESS_READY, NULL, 0, 0 }};

static struct timeval start_time, end_time;
static struct timeval malloc_start_time, malloc_end_time;


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
    printf("\n  Processes: %zu active,  %zu ready,  %zu complete,  %zu total\n",
           running_count,
           PROCESS_COUNT - running_count - complete_count,
           complete_count,
           PROCESS_COUNT);
    printf("  Memory:    %zu used (%0.3lf%%),  %zu free,  %zu total\n\n",
           total_memory - memory,
           ((double) (total_memory - memory) / (double) total_memory) * 100,
           memory,
           total_memory);
    printf("  PID    Cycles\n");
    printf(" -------------------------------------------------------------------\n");

    for (size_t i = 0; i < PROCESS_COUNT; ++i) {
        if (processes[i].state == PROCESS_RUNNING) {
            printf("  %u    ", processes[i].pid);
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

void Simulator_run(size_t initial_memory,
                   int use_custom_allocator,
                   size_t process_limit)
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

        // set process state to ready, and reset time variables.
        // this allows Simulator_run to be fully reentrant
        processes[i].state = PROCESS_READY;
        processes[i].malloc_time = 0;
        processes[i].free_time = 0;
    }

    memory = initial_memory;
    total_memory = initial_memory;

    active_process_count = 0;
    active_process_limit = process_limit;

    unsigned int timer = 0;
    unsigned int frame_count = 0;

    // disable cursor (improves flickering substantially)
    printf("\033[?25l");


    printf("INITIAL MEMORY:     %zu\n"
           "USING CUSTOM ALLOC: %d\n"
           "PROCESS LIMIT:      %zu\n",
           initial_memory,
           use_custom_allocator,
           process_limit);


    Timer_start(&start_time);

    // allocate memory pool
    MemoryPool *memory_pool = NULL;
    if (use_custom_allocator) {
        memory_pool = MemoryPool_create(total_memory);
    }
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
                if ((active_process_limit == 0 || active_process_count < active_process_limit)
                    && processes[i].state == PROCESS_READY
                    && processes[i].size < memory
                ) {
                    ++active_process_count;
                    processes[i].state = PROCESS_RUNNING;
                    memory -= processes[i].size;
                    processes[i].cycles_remaining = processes[i].cycles;

                    // attempt to allocate process' memory
                    Timer_start(&malloc_start_time);
                    if (use_custom_allocator) {
                        processes[i].data = my_malloc(memory_pool, processes[i].size);
                    }
                    else {
                        processes[i].data = malloc(processes[i].size);
                    }
                    processes[i].malloc_time = Timer_stop(malloc_start_time,
                                                          &malloc_end_time);

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
                // if the current process is complete, update the system
                if (processes[i].cycles_remaining == 0) {
                    --active_process_count;
                    processes[i].state = PROCESS_COMPLETE;
                    memory += processes[i].size;

                    // attempt to free process' memory
                    if (processes[i].data != NULL) {
                        Timer_start(&malloc_start_time);
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
                        processes[i].free_time = Timer_stop(malloc_start_time,
                                                            &malloc_end_time);
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

    // deallocate memory pool
    if (use_custom_allocator) {
        MemoryPool_destroy(memory_pool);
    }

    // measure total execution time of simulation
    double total_time = Timer_stop(start_time, &end_time);

    for (size_t i = 0; i < PROCESS_COUNT; ++i) {
        printf("%u, %zu, %zu, %0.3lf, %0.3lf\n",
               processes[i].pid,
               processes[i].cycles,
               processes[i].size,
               processes[i].malloc_time,
               processes[i].free_time);
    }
    printf("\nTotal Execution Time: %0.3lf ms\n", total_time);

    // re-enable cursor
    printf("\033[?25h");
}
