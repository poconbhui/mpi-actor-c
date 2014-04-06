#include "../src/actor.h"
#include "mpitest.h"

#include <stdlib.h>
#include <stdio.h>



/*
 * Define a simple actor type here
 */
int test_main_fn(MPI_Comm comm_actor, void* actor_state) {
    return MPI_ACTOR_ALIVE;
}



/* Test actor creation works */
void actor_type_creation(void) {
    MPI_Datatype actor_type;

    int data_count = 10;
    MPI_Datatype data_type = MPI_INT;
    int data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    int test_data_count = 0;
    MPI_Datatype test_data_type = MPI_DATATYPE_NULL;
    int *test_data = calloc(data_count, sizeof(int));

    MPI_Actor_main_function *test_test_main_fn;

    int err;


    err = MPI_Type_create_actor(
        test_main_fn,
        data_count, data_type, data,
        &actor_type
    );
    require_true(
        "MPI_Type_create_actor should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    err = MPI_Type_commit(&actor_type);
    require_true(
        "MPI_Type_commit should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );


    /* Test MPI_Actor_get_datatypes */
    err = MPI_Actor_get_datatypes(
        actor_type, &test_data_count, &test_data_type
    );
    require_true(
        "MPI_Actor_get_datatypes should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_datatypes should return 10", MPI_COMM_WORLD,
        test_data_count == 10
    );
    require_true(
        "MPI_Actor_datatypes should return MPI_INT", MPI_COMM_WORLD,
        test_data_type == MPI_INT
    );


    /* Test MPI_Actor_get_data */
    err = MPI_Actor_get_data(
        actor_type, test_data_count, test_data_type, test_data
    );
    require_true(
        "MPI_Actor_get_data should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_get_data should set the 0th data element to 0",
        MPI_COMM_WORLD,
        test_data[0] == 0
    );
    require_true(
        "MPI_Actor_get_data should set the 5th data element to 5",
        MPI_COMM_WORLD,
        test_data[5] == 5
    );
    require_true(
        "MPI_Actor_get_data should set the 9th data element to 9",
        MPI_COMM_WORLD,
        test_data[9] == 9
    );


    /* Test MPI_Actor_get_main */
    err = MPI_Actor_get_main(actor_type, &test_test_main_fn);
    require_true(
        "MPI_Actor_get_main should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_get_main should return test_main_fn", MPI_COMM_WORLD,
        test_test_main_fn == test_main_fn
    );


    MPI_Type_free(&actor_type);
}


/* Test actor duplication works */
void actor_type_duplication(void) {
    MPI_Datatype actor_type;
    MPI_Datatype actor_type_dup;

    int data_count = 10;
    MPI_Datatype data_type = MPI_INT;
    int data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    int test_data_count = 0;
    MPI_Datatype test_data_type = MPI_DATATYPE_NULL;
    int *test_data = calloc(10, sizeof(int));

    MPI_Actor_main_function *test_test_main_fn;

    int err;
    int i;


    err = MPI_Type_create_actor(
        test_main_fn,
        data_count, data_type, data,
        &actor_type
    );
    require_true(
        "MPI_Type_create_actor should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    err = MPI_Type_commit(&actor_type);
    require_true(
        "MPI_Type_commit should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );


    /* Make duplicate */
    err = MPI_Type_dup(actor_type, &actor_type_dup);
    require_true(
        "MPI_Type_dup should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );


    /* Test MPI_Actor_get_datatypes */
    err = MPI_Actor_get_datatypes(
        actor_type_dup, &test_data_count, &test_data_type
    );
    require_true(
        "MPI_Actor_get_datatypes should return MPI_SUCCESS"
        " for actor_type_dup.",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_data_count should return 10 on dup", MPI_COMM_WORLD,
        test_data_count == 10
    );

    /* Test MPI_Actor_get_data */
    err = MPI_Actor_get_data(
        actor_type_dup, test_data_count, test_data_type, test_data
    );
    require_true(
        "MPI_Actor_get_data should return MPI_SUCCESS",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_get_data should set the 0th data element to 0",
        MPI_COMM_WORLD,
        test_data[0] == 0
    );
    require_true(
        "MPI_Actor_get_data should set the 5th data element to 5",
        MPI_COMM_WORLD,
        test_data[5] == 5
    );
    require_true(
        "MPI_Actor_get_data should set the 9th data element to 9",
        MPI_COMM_WORLD,
        test_data[9] == 9
    );

    /* Test MPI_Actor_get_main */
    err = MPI_Actor_get_main(actor_type_dup, &test_test_main_fn);
    require_true(
        "MPI_Actor_get_main should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_get_main should return test_main_fn", MPI_COMM_WORLD,
        test_test_main_fn == test_main_fn
    );


    /* Free original actor_type */
    err = MPI_Type_free(&actor_type);
    require_true(
        "MPI_Type_free should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );


    /* Data getting should still work as expected */
    test_data_count = 0;
    test_data_type = MPI_DATATYPE_NULL;
    for(i=0; i<data_count; i++) test_data[i] = 0;

    /* Test MPI_Actor_get_datatypes */
    err = MPI_Actor_get_datatypes(
        actor_type_dup, &test_data_count, &test_data_type
    );
    require_true(
        "MPI_Actor_get_datatypes should return MPI_SUCCESS"
        " after free",
        MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_data_count should return 10"
        " after free", MPI_COMM_WORLD,
        test_data_count == 10
    );

    /* Test MPI_Actor_get_data */
    err = MPI_Actor_get_data(
        actor_type_dup, test_data_count, test_data_type, test_data
    );
    require_true(
        "MPI_Actor_get_data should return MPI_SUCCESS"
        " after free", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_get_data should set the 0th data element to 0"
        " after free",
        MPI_COMM_WORLD,
        test_data[0] == 0
    );
    require_true(
        "MPI_Actor_get_data should set the 5th data element to 5"
        " after free",
        MPI_COMM_WORLD,
        test_data[5] == 5
    );
    require_true(
        "MPI_Actor_get_data should set the 9th data element to 9"
        " after free",
        MPI_COMM_WORLD,
        test_data[9] == 9
    );

    /* Test MPI_Actor_get_main */
    err = MPI_Actor_get_main(actor_type_dup, &test_test_main_fn);
    require_true(
        "MPI_Actor_get_main should return MPI_SUCCESS"
        " after free", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );
    require_true(
        "MPI_Actor_get_main should return test_main_fn"
        " after free", MPI_COMM_WORLD,
        test_test_main_fn == test_main_fn
    );


    /* Free duplicated actor_type */
    err = MPI_Type_free(&actor_type_dup);
    require_true(
        "MPI_Type_free should return MPI_SUCCESS", MPI_COMM_WORLD,
        err == MPI_SUCCESS
    );

}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);


    start_suite("MPI_Type_create_actor");
    run_test(actor_type_creation, "Actor Type Creation");
    run_test(actor_type_duplication, "Actor Type Duplication");
    end_suite();


    MPI_Finalize();
    return mpitest_exit_status();
}
