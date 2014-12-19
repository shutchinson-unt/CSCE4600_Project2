// #include "mymy_malloc.h"

#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    {
        int *test;
        test = my_malloc(sizeof(int));
        assert(test != NULL);
        my_free(test);
    }

    {
        int **test;
        test = my_malloc(sizeof(int *) * 20);
        assert(test != NULL);
        for (unsigned int i = 0; i < 20; ++i) {
            test[i] = my_malloc(sizeof(int) * 10);
            assert(test[i] != NULL);
            for (unsigned int j = 0; j < 10; ++j) {
                test[i][j] = (int) (i * j);
            }
        }

        for (unsigned int i = 0; i < 20; ++i) {
            for (unsigned int j = 0; j < 10; ++j) {
                printf("(%u, %u): %d\n", j, i, test[i][j]);
            }
        }

        for (unsigned int i = 0; i < 20; ++i) {
            my_free(test[i]);
        }
        my_free(test);
    }

    {
        struct some_struct {
            int a, b, c;
        } some_struct;

        struct some_struct *test;
        test = my_malloc(sizeof(some_struct));
        assert(test != NULL);
        my_free(test);
    }

    return 0;
}
