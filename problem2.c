#include "simulator.h"
#include "mymalloc.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    Simulator_run(10000000, 1);


    // char *test = my_malloc(300000);

    // if (test) {
    //     printf("FREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDDDDDDD\n");
    //     my_free(test);
    // }

    return 0;
}
