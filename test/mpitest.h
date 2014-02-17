#ifndef MPI_ACTOR_TEST_H_
#define MPI_ACTOR_TEST_H_

#include <mpi.h>


typedef void test_function(void);

void start_suite(char *name);
void run_test(test_function* test, char *name);
void end_suite(void);

int suite_exit_status(void);

void require_true(char *message_if_fail, MPI_Comm comm, int test);


#endif  /* MPI_ACTOR_TEST_H_ */
