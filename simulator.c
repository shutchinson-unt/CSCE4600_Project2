#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


static size_t memory = MEMORY_TOTAL;

static size_t running_count = 0;
static size_t complete_count = 0;


typedef enum PROCESS_STATE {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_COMPLETE,
} PROCESS_STATE;

typedef struct Process {
    pid_t pid;
    size_t cycles, size;
    PROCESS_STATE state;
    size_t cycles_remaining;
    char *data;
} Process;


static void sleep_ms(unsigned int milliseconds)
{
    time_t seconds = (int) (milliseconds / 1000);

    milliseconds = milliseconds - ((unsigned int) seconds * 1000);

    struct timespec req;
    req.tv_sec = seconds;
    req.tv_nsec = (int) milliseconds * 1000000L;

    while (nanosleep(&req, &req) == -1);
}

static unsigned long random_range(long max)
{
    unsigned long bin_count, rand_count;
    unsigned long bin_size;
    unsigned long defect;

    bin_count = (unsigned long) max + 1;
    rand_count = (unsigned long) RAND_MAX + 1;
    bin_size = rand_count / bin_count;
    defect   = rand_count % bin_count;

    long r;
    do {
        r = rand();
    }
    while (rand_count - defect <= (unsigned long) r);

    return (unsigned long) r / bin_size;
}

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
    unsigned long delta;
    delta = random_range(CYCLES_MAX - CYCLES_MIN);
    return (size_t) (CYCLES_MIN + delta);
}

static size_t random_size(void)
{
    unsigned long delta;
    delta = random_range(SIZE_MAX - SIZE_MIN);
    return (size_t) (SIZE_MIN + delta);
}


static void draw_process_table(const Process *processes)
{
    printf("\033[H\033[2J");
    printf(" MEMORY:    %zu used (%0.3lf%%), %zu free, %zu total\n",
           MEMORY_TOTAL - memory,
           (double) ((double) (MEMORY_TOTAL - memory) / (double) MEMORY_TOTAL) * 100,
           memory,
           MEMORY_TOTAL);
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
                if ((j % 25) == 0) {
                    printf("|");
                }
            }
            printf("\n");
        }
    }
}


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    srand((unsigned int) time(NULL));

    printf("START SIZE: %zu\n", memory);

    Process processes[PROCESS_COUNT] = {{ 0 }};

    for (int i = 0; i < PROCESS_COUNT; ++i) {
        processes[i].pid = unique_pid(processes);
        processes[i].cycles = random_cycles();
        processes[i].size = random_size();
        printf("PID:    %u\n", processes[i].pid);
        printf("CYCLES: %u\n", processes[i].cycles);
        printf("SIZE:   %u\n\n", processes[i].size);
    }

    // disable cursor
    printf("\033[?25l");

    unsigned int timer = 0;
    unsigned int frame_count = 0;

    while (1) {
        // check if all processes are complete
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
        if (complete_count == PROCESS_COUNT) {
            draw_process_table(processes);
            break;
        }

        // every 50 cycles, reset timer and attempt to run ready processes
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
                    processes[i].data = malloc(processes[i].size);
                    if (processes[i].data == NULL) {
                        printf("Error: Failed to allocate memory for process %u\n",
                               processes[i].pid);
                    }
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
                        free(processes[i].data);
                        processes[i].data = NULL;
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
            draw_process_table(processes);
        }

        // update counters and sleep 10 milliseconds
        ++timer;
        ++frame_count;
        sleep_ms(10);
    }

    // re-enable cursor
    printf("\033[?25h");

    return 0;
}
