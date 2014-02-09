#include "test.h"

#include <stdio.h>
#include <string.h>


static char suite_name[255];
static char test_name[255];

static int total_tests_run;
static int total_tests_passed;


void start_suite(char *name) {
    strcpy(suite_name, name);

    printf("\n\n----%s----\n", suite_name);

    total_tests_run = 0;
    total_tests_passed = 0;
}

static int test_passed;
void run_test(test_function* test, char *name) {
    strcpy(test_name, name);

    test_passed = 1;

    test();

    total_tests_run++;

    if(test_passed) {
        total_tests_passed++;
    }
}


void end_suite() {
    printf("|\n|---SUITE ENDED---\n");
    printf("| Tests Ran:    %d\n", total_tests_run);
    printf("| Tests Passed: %d\n", total_tests_passed);
    printf("|\n");

    if(total_tests_run == total_tests_passed) {
        printf("| SUITE PASSED!\n");
    } else {
        printf("| SUITE FAILED!!!\n");
    }

    printf("------------------\n\n");
}


void require_true(char *message_if_fail, MPI_Comm comm, int test) {
    int rank;
    int size;
    int i;

    int global_test;

    MPI_Allreduce(&test, &global_test, 1, MPI_INT, MPI_PROD, comm);

    if(!global_test) {

        MPI_Barrier(comm);
        MPI_Comm_rank(comm, &rank);
        MPI_Comm_size(comm, &size);
        
        /* If not yet failed, output name of test */
        if(test_passed == 1) {
            if(rank == 0) printf("|\n| %s:\n", test_name);
        }

        test_passed = 0;

        for(i=0; i<size; i++) {
            if(rank == i) {
                if(rank == 0) {
                    printf("| |\n");
                    printf("| |---FAILURE---\n");
                    printf("| | %s\n", message_if_fail);
                    printf("| |\n");
                    printf("| | Failed on:\n");
                }
                if(!test) {
                    printf("| | |- Process %d\n", i);
                }
            }

            MPI_Barrier(comm);
        }
    }
}
