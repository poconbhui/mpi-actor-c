#include "../src/actor_type_data.h"
#include "mpitest.h"

#include <stdio.h>
#include <stdlib.h>


int test_actor_main_fn(MPI_Comm actor_comm, void *state) {
    return MPI_ACTOR_ALIVE;
}


void actor_type_data_generation(void) {
    MPI_Comm comm_actor = MPI_COMM_NULL;

    Actor_type_data *actor_type_data_ptr = NULL;
    MPI_Datatype actor_type = MPI_DATATYPE_NULL;

    MPI_Actor_main_function  *test_fn = &test_actor_main_fn;
    int count = 10;
    MPI_Datatype datatype = MPI_DATATYPE_NULL;
    int initial_data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    int err;
    int flag;
    int i;


    /* Duplicate MPI_COMM_WORLD for comm_actor */
    MPI_Comm_dup(MPI_COMM_WORLD, &comm_actor);

    MPI_Type_dup(MPI_INT, &actor_type);
    MPI_Type_commit(&actor_type);
    MPI_Type_dup(MPI_INT, &datatype);
    MPI_Type_commit(&actor_type);


    /* Attach data */
    err = attach_Actor_type_data(
        test_fn, count, datatype, initial_data, actor_type
    );

    require_true(
        "attach_Actor_type_data should return MPI_SUCCESS", comm_actor,
        err == MPI_SUCCESS
    );


    /* Retrieve data */
    Actor_type_data *actor_type_data_test = NULL;
    flag = -1;
    err = get_Actor_type_data(actor_type, &actor_type_data_test, &flag);

    require_true(
        "get_Actor_type_data should return MPI_SUCCESS", comm_actor,
        err == MPI_SUCCESS
    );

    require_true(
        "actor_type_data_test should not be NULL", comm_actor,
        actor_type_data_test != NULL
    );

    require_true(
        "flag should be 1", comm_actor,
        flag == 1
    );


    /* test values */

    require_true(
        "actor_type_data_test->main should equal test_fn", comm_actor,
        actor_type_data_test->main == test_fn
    );

    require_true(
        "actor_type_data_test->type should equal datatype", comm_actor,
        actor_type_data_test->type == datatype
    );

    require_true(
        "actor_type_data_test->num_actor_types should equal count",
        comm_actor,
        actor_type_data_test->count == count
    );

    for(i=0; i<count; i++) {
        /* require value equality */
        require_true(
            "actor_type_data_test->initial_data[i] should be equal to"
            " initial_data[i]",
            comm_actor,
            ((int*)actor_type_data_test->initial_data)[i] == initial_data[i]
        );
    }

    /* require pointers different */
    require_true(
        "actor_type_data_test->initial_data pointer should not"
        " initial_data",
        comm_actor,
        actor_type_data_test->initial_data != initial_data
    );


}


void actor_type_data_failure(void) {
    Actor_type_data *actor_type_data_ptr = NULL;
    MPI_Comm comm_actor = MPI_COMM_NULL;

    MPI_Datatype actor_type = MPI_DATATYPE_NULL;

    int err;
    int flag;


    MPI_Comm_dup(MPI_COMM_WORLD, &comm_actor);
    MPI_Type_dup(MPI_INT, &actor_type);
    MPI_Type_commit(&actor_type);


    Actor_type_data *actor_type_data_test = NULL;
    err = get_Actor_type_data(
        actor_type, &actor_type_data_test, &flag
    );

    require_true(
        "get_Actor_type_data should return MPI_SUCCESS", comm_actor,
        err == MPI_SUCCESS
    );

    require_true(
        "get_Actor_type_data should set flag == 0", comm_actor,
        flag == 0
    );
}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    start_suite("Actor_type_data");
    run_test(actor_type_data_generation, "Generation");
    run_test(actor_type_data_failure, "Generation");
    end_suite();

    MPI_Finalize();
    return mpitest_exit_status();
}
