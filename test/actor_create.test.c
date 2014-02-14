#include "../src/actor.h"
#include "mpitest.h"

#include <stdio.h>
#include <stdlib.h>


int test_main_fn(
    MPI_Comm comm_actor, MPI_Datatype actor_type,
    void *message, int tag,
    void* state
) {
    return MPI_ACTOR_ALIVE;
}


/* Generate the test_actor_type */
MPI_Datatype gen_test_actor_type() {
    static MPI_Datatype test_actor_type = MPI_DATATYPE_NULL;

    int data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    if(test_actor_type == MPI_DATATYPE_NULL) {
        MPI_Type_create_actor(
            test_main_fn,
            10, MPI_INT, data,
            &test_actor_type
        );
    }

    return test_actor_type;
}

MPI_Datatype gen_test_receptionist_type() {
    static MPI_Datatype test_receptionist_type = MPI_DATATYPE_NULL;

    if(test_receptionist_type == MPI_DATATYPE_NULL) {
        MPI_Datatype test_actor_type = gen_test_actor_type();
        MPI_Type_dup(test_actor_type, &test_receptionist_type);
    }

    return test_receptionist_type;
}


void comm_actor_creation(void) {
    MPI_Comm comm_actor = MPI_COMM_NULL;

    MPI_Datatype test_actor_type = gen_test_actor_type();
    MPI_Datatype test_receptionist_type = gen_test_receptionist_type();

    int num_actor_types = 1;
    MPI_Datatype actor_type_list[] = { test_actor_type };

    int err;


    /* Create the actor communicator */
    err = MPI_Actor_create(
        MPI_COMM_WORLD, num_actor_types,
        actor_type_list, test_receptionist_type,
        &comm_actor
    );


    /* Check return values */
    require_true(
        "MPI_Actor create return value should be MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "comm_actor should not be MPI_COMM_NULL", MPI_COMM_WORLD,
        comm_actor != MPI_COMM_NULL
    );


    /* Check retrieving information from the actor communicator */
    MPI_Datatype test_actor_type_list[num_actor_types];
    MPI_Datatype test_test_receptionist_type = MPI_DATATYPE_NULL;
    err = MPI_Actor_get(
        comm_actor, num_actor_types, test_actor_type_list,
        &test_test_receptionist_type
    );

    require_true(
        "MPI_Actor_get return value should be MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "test_actor_type_list should contain the test_actor at position 0",
        MPI_COMM_WORLD,
        test_actor_type_list[0] == test_actor_type
    );

    require_true(
        "test_test_receptionist_type be the test_receptionist",
        MPI_COMM_WORLD,
        test_test_receptionist_type == test_receptionist_type
    );


    /* Free the communicator */
    err = MPI_Comm_free(&comm_actor);

    require_true(
        "MPI_Comm_free should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

}


void comm_actor_duplication(void) {
    MPI_Comm comm_actor = MPI_COMM_NULL;
    MPI_Comm comm_actor_dup = MPI_COMM_NULL;
    MPI_Comm comm_actor_dup_dup = MPI_COMM_NULL;

    MPI_Datatype test_actor_type = gen_test_actor_type();
    MPI_Datatype test_receptionist_type = gen_test_receptionist_type();

    int num_actor_types = 1;
    MPI_Datatype actor_type_list[] = { test_actor_type };

    MPI_Datatype test_actor_type_list[num_actor_types];
    MPI_Datatype test_test_receptionist_type = MPI_DATATYPE_NULL;

    int err;


    /* Create the actor communicator */
    MPI_Actor_create(
        MPI_COMM_WORLD, num_actor_types,
        actor_type_list, test_receptionist_type,
        &comm_actor
    );


    /* Create a duplicate of the original */
    err = MPI_Comm_dup(comm_actor, &comm_actor_dup);
    require_true(
        "MPI_Comm_dup should return MPI_SUCCESS for comm_actor_dup",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    test_actor_type_list[0] = MPI_DATATYPE_NULL;
    test_test_receptionist_type = MPI_DATATYPE_NULL;
    err = MPI_Actor_get(
        comm_actor_dup, num_actor_types, test_actor_type_list,
        &test_test_receptionist_type
    );
    require_true(
        "MPI_Actor_get should return MPI_SUCCESS for comm_actor_dup",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "MPI_Actor_get should return the test_actor_type for comm_actor_dup",
        MPI_COMM_WORLD,
        test_actor_type_list[0] == test_actor_type
    );

    require_true(
        "MPI_Actor_get should return the test_receptionist_type"
        " for comm_actor_dup",
        MPI_COMM_WORLD,
        test_test_receptionist_type == test_receptionist_type
    );


    /* Free the original communicator, and tests should still pass */
    MPI_Comm_free(&comm_actor);

    test_actor_type_list[0] = MPI_DATATYPE_NULL;
    test_test_receptionist_type = MPI_DATATYPE_NULL;
    err = MPI_Actor_get(
        comm_actor_dup, num_actor_types, test_actor_type_list,
        &test_test_receptionist_type
    );
    require_true(
        "MPI_Actor_get should return MPI_SUCCESS for comm_actor_dup after free",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "MPI_Actor_get should return the test_actor_type for comm_actor_dup"
        " after free",
        MPI_COMM_WORLD,
        test_actor_type_list[0] == test_actor_type
    );

    require_true(
        "MPI_Actor_get should return the test_receptionist_type"
        " for comm_actor_dup after free",
        MPI_COMM_WORLD,
        test_test_receptionist_type == test_receptionist_type
    );


    /* Create a duplicate of the duplicate */
    MPI_Comm_dup(comm_actor_dup, &comm_actor_dup_dup);
    require_true(
        "MPI_Comm_dup should return MPI_SUCCESS for comm_actor_dup_dup",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    /* All previous tests should still pass */

    test_actor_type_list[0] = MPI_DATATYPE_NULL;
    test_test_receptionist_type = MPI_DATATYPE_NULL;
    err = MPI_Actor_get(
        comm_actor_dup_dup, num_actor_types, test_actor_type_list,
        &test_test_receptionist_type
    );
    require_true(
        "MPI_Actor_get should return MPI_SUCCESS for comm_actor_dup_dup",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "MPI_Actor_get should return the test_actor_type for"
        " comm_actor_dup_dup",
        MPI_COMM_WORLD,
        test_actor_type_list[0] == test_actor_type
    );

    require_true(
        "MPI_Actor_get should return the test_receptionist_type for"
        " comm_actor_dup_dup",
        MPI_COMM_WORLD,
        test_test_receptionist_type == test_receptionist_type
    );


    /* Free the first duplicate communicator, and tests should still pass */
    MPI_Comm_free(&comm_actor_dup);

    test_actor_type_list[0] = MPI_DATATYPE_NULL;
    test_test_receptionist_type = MPI_DATATYPE_NULL;
    err = MPI_Actor_get(
        comm_actor_dup_dup, num_actor_types, test_actor_type_list,
        &test_test_receptionist_type
    );
    require_true(
        "MPI_Actor_get should return MPI_SUCCESS for comm_actor_dup_dup"
        " after free",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "MPI_Actor_get should return the test_actor_type for"
        " comm_actor_dup_dup after free",
        MPI_COMM_WORLD,
        test_actor_type_list[0] == test_actor_type
    );

    require_true(
        "MPI_Actor_get should return the test_receptionist_type for"
        " comm_actor_dup_dup after free",
        MPI_COMM_WORLD,
        test_test_receptionist_type == test_receptionist_type
    );

    /* Free the dup dup communicator */
    MPI_Comm_free(&comm_actor_dup_dup);

}


void comm_actor_null_arguments(void) {
    MPI_Comm comm_actor = MPI_COMM_NULL;

    int num_actor_types = 0;
    int *actor_types = NULL;

    MPI_Datatype test_receptionist_type = gen_test_receptionist_type();

    int err;


    err = MPI_Actor_create(
        MPI_COMM_WORLD,
        num_actor_types, NULL, test_receptionist_type,
        &comm_actor
    );


    /* Check return values */
    require_true(
        "MPI_Actor create return value should be MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "comm_actor should not be MPI_COMM_NULL", MPI_COMM_WORLD,
        comm_actor != MPI_COMM_NULL
    );


    /* Check retrieving information from the actor communicator */
    MPI_Datatype *test_actor_type_list = NULL;
    MPI_Datatype test_test_receptionist_type = MPI_DATATYPE_NULL;
    err = MPI_Actor_get(
        comm_actor, num_actor_types, test_actor_type_list,
        &test_test_receptionist_type
    );

    require_true(
        "MPI_Actor_get return value should be MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

    require_true(
        "test_actor_type_list should be the NULL pointer",
        MPI_COMM_WORLD,
        test_actor_type_list == NULL
    );

    require_true(
        "test_test_receptionist_type be the test_receptionist",
        MPI_COMM_WORLD,
        test_test_receptionist_type == test_receptionist_type
    );

}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    start_suite("MPI_Actor_create");
    run_test(comm_actor_creation, "Creation, getting, deleting");
    run_test(comm_actor_duplication, "Communicator duplication");
    run_test(
        comm_actor_null_arguments,
        "Communicator creation with silly arguments"
    );
    end_suite();

    MPI_Finalize();
    return mpitest_exit_status();
}
