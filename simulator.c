#include "simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "util.h"

#define PID_MIN       100
#define PID_MAX       999

#define CYCLES_MIN    10
#define CYCLES_MAX    1000
// #define CYCLES_MIN    1000
// #define CYCLES_MAX    11000

#define SIZE_MIN      500000
#define SIZE_MAX      2500000

#define PROCESS_COUNT 50

#define MEMORY_TOTAL  10000000
// #define MEMORY_TOTAL  10000000/2
// #define MEMORY_TOTAL  10000000/10




// static size_t memory = MEMORY_TOTAL;

// static size_t running_count = 0;
// static size_t complete_count = 0;





typedef enum PROCESS_STATE {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_COMPLETE,
} PROCESS_STATE;

struct Process {
    pid_t pid;
    size_t cycles, size, cycles_remaining;
    PROCESS_STATE state;
    char *data;
};



static pid_t random_pid(void)
{
    unsigned long delta;
    delta = random_range((PID_MAX) - PID_MIN);
    return (pid_t) (PID_MIN + delta);
}

static pid_t unique_pid(const Process *processes)
{
    pid_t pid = random_pid();

    for (int i = 0; i < PROCESS_COUNT; ++i) {
        if (processes[i].pid == pid) {
            return unique_pid(processes);
        }
    }

    return pid;
}

static size_t random_cycles(void)
{
    // unsigned long delta;
    // delta = random_range(CYCLES_MAX - CYCLES_MIN);
    // return (size_t) (CYCLES_MIN + delta);

    // return (size_t) SD(50, 550, 300, 'c', PROCESS_COUNT);
    return (size_t) SD(CYCLES_MIN, CYCLES_MAX, 505, 'c', PROCESS_COUNT);
}

static size_t random_size(void)
{
    // unsigned long delta;
    // delta = random_range(SIZE_MAX - SIZE_MIN);
    // return (size_t) (SIZE_MIN + delta);

    return (size_t) SD(100000, 300000, 200000, 'm', PROCESS_COUNT);
    // return (size_t) SD(250000, 700000, 450000, 'm', PROCESS_COUNT);
}

static void draw_process_table(Simulator *simulator)
{
    printf("\033[H\033[2J");
    printf(" MEMORY:    %zu used (%0.3lf%%), %zu free, %zu total\n",
           MEMORY_TOTAL - simulator->memory,
           ((double) (MEMORY_TOTAL - simulator->memory) / (double) MEMORY_TOTAL) * 100,
           simulator->memory,
           MEMORY_TOTAL);
    printf(" PROCESSES: %zu active, %zu ready, %zu complete, %zu total\n",
           simulator->running_count,
           PROCESS_COUNT - simulator->running_count - simulator->complete_count,
           simulator->complete_count,
           PROCESS_COUNT);
    printf(" PID    CYCLES\n");
    printf("==================================================\n");

    for (size_t i = 0; i < PROCESS_COUNT; ++i) {
        if (simulator->processes[i].state == PROCESS_RUNNING) {
            printf(" %u    ", simulator->processes[i].pid);
            for (size_t j = 0; j < simulator->processes[i].cycles_remaining; ++j) {
                if ((j % 25) == 0) {
                    printf("|");
                }
            }
            printf("\n");
        }
    }
}

void Simulator_run(Simulator *simulator)
{
    seed_rand();

    printf("START SIZE: %zu\n", simulator->memory);

    Process processes[PROCESS_COUNT] = {{ 0, 0, 0, 0, PROCESS_READY, NULL }};
    simulator->processes = processes;

    for (int i = 0; i < PROCESS_COUNT; ++i) {
        simulator->processes[i].pid = unique_pid(simulator->processes);
        simulator->processes[i].cycles = random_cycles();
        simulator->processes[i].size = random_size();
        printf("PID:    %u\n", simulator->processes[i].pid);
        printf("CYCLES: %u\n", simulator->processes[i].cycles);
        printf("SIZE:   %u\n\n", simulator->processes[i].size);
    }

    // disable cursor
    printf("\033[?25l");

    unsigned int timer = 0;
    unsigned int frame_count = 0;

    while (1) {
        // count the total number of running and complete processes
        simulator->running_count = 0;
        simulator->complete_count = 0;
        for (size_t i = 0; i < PROCESS_COUNT; ++i) {
            if (processes[i].state == PROCESS_COMPLETE) {
                ++simulator->complete_count;
            }
            else if (processes[i].state == PROCESS_RUNNING) {
                ++simulator->running_count;
            }
        }

        // if we've all processes have completed, exit the program
        if (simulator->complete_count == PROCESS_COUNT) {
            draw_process_table(simulator);
            break;
        }

        // every 50 cycles, reset timer and attempt to run ready processes
        if (timer == 49) {
            timer = 0;

            // set all ready processes that will fit in memory to running state
            for (size_t i = 0; i < PROCESS_COUNT; ++i) {
                if (simulator->processes[i].state == PROCESS_READY
                    && simulator->processes[i].size < simulator->memory
                ) {
                    simulator->processes[i].state = PROCESS_RUNNING;
                    simulator->memory -= simulator->processes[i].size;
                    simulator->processes[i].cycles_remaining = simulator->processes[i].cycles;
                    simulator->processes[i].data = malloc(simulator->processes[i].size);
                    if (simulator->processes[i].data == NULL) {
                        printf("Error: Failed to allocate "
                               "memory for process %u\n",
                               simulator->processes[i].pid);
                        break;
                    }
                }
            }
        }

        // update running processes
        for (size_t i = 0; i < PROCESS_COUNT; ++i) {
            if (simulator->processes[i].state == PROCESS_RUNNING) {
                if (simulator->processes[i].cycles_remaining == 0) {
                    simulator->processes[i].state = PROCESS_COMPLETE;
                    memory += simulator->processes[i].size;
                    if (simulator->processes[i].data != NULL) {
                        free(simulator->processes[i].data);
                        simulator->processes[i].data = NULL;
                    }
                }
                else {
                    --simulator->processes[i].cycles_remaining;
                }
            }
        }

        // print process table every 25 frames
        if (frame_count == 24) {
            frame_count = 0;
            draw_process_table(simulator->processes);
        }

        // update counters and sleep 10 milliseconds
        ++timer;
        ++frame_count;
        sleep_ms(10);
    }

    // re-enable cursor
    printf("\033[?25h");
}
