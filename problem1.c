#include "simulator.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    // run simulation
    Simulator_run(10000000, 0, 5);

    return 0;
}
