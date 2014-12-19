#include "simulator.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    Simulator_run(10000000 / 2, 0);
    Simulator_run(10000000 / 10, 0);

    return 0;
}
