#include "simulator.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    // // run simulation with 50% system memory and at most 5 active processes
    // Simulator_run(10000000 / 2, 0, 5);
    // // run simulation with 50% system memory and at most 10 active processes
    // Simulator_run(10000000 / 2, 0, 10);
    // // run simulation with 50% system memory and unlimited active processes
    // Simulator_run(10000000 / 2, 0, 0);

    // run simulation with 10% system memory and at most 5 active processes
    Simulator_run(10000000 / 10, 0, 5);
    // run simulation with 10% system memory and at most 10 active processes
    Simulator_run(10000000 / 10, 0, 10);
    // run simulation with 10% system memory and unlimited active processes
    Simulator_run(10000000 / 10, 0, 0);

    return 0;
}
