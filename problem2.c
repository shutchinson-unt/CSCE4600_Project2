#include "simulator.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    // run simulation using system malloc and at most 5 active processes
    Simulator_run(10000000, 0, 5);

    // run simulation using my_malloc and at most 5 active processes
    Simulator_run(10000000, 1, 5);

    return 0;
}
