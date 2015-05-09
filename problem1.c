#include "simulator.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    // run simulation with at most 5 active processes
    Simulator_run(10000000, 0, 5);

    return 0;
}
