#include "simulator.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    Simulator simulator;
    Simulator_run(&simulator);

    return 0;
}
