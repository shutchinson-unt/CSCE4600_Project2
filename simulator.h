#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stddef.h>

typedef struct Process Process;

typedef struct Simulator {
    size_t memory;
    size_t running_count, complete_count;
    Process *processes;
} Simulator;


extern void Simulator_run(Simulator *simulator);

#endif
