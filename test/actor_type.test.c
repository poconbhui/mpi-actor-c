#include "../src/actor.h"

#include <stdlib.h>
#include <stdio.h>



/*
 * Define some simple actor functions here
 */
void test_init_fn(MPI_Comm comm_actor, MPI_Datatype actor_type, void* data) {
}

int test_main_fn(MPI_Comm comm_actor, MPI_Datatype actor_type, void* data) {
    return MPI_ACTOR_ALIVE;
}

void test_destroy_fn(MPI_Comm comm_actor, MPI_Datatype actor_type, void* data) {
}



/* Test actor creation works */
void actor_type_creation(void) {
    MPI_Datatype actor_type;

    int data_count = 10;
    MPI_Datatype data_type = MPI_INT;

    int test_data_count = 0;
    MPI_Datatype test_data_type = NULL;

    MPI_Type_create_actor(
        test_init_fn,
        test_main_fn,
        test_destroy_fn,
        data_count, data_type,
        &actor_type
    );
    MPI_Type_commit(&actor_type);


    /* Test MPI_Actor_data_count */
    MPI_Actor_data_count(actor_type, &test_data_count);
    require_true(
        "MPI_Actor_data_count should return 10", MPI_COMM_WORLD,
        test_data_count == 10
    );

    /* Test MPI_Actor_data_type */
    MPI_Actor_data_type(actor_type, &test_data_type);
    require_true(
        "MPI_Actor_data_type should return MPI_INT", MPI_COMM_WORLD,
        test_data_type == MPI_INT
    );


    MPI_Type_free(&actor_type);
}


/* Test actor duplication works */
void actor_type_duplication(void) {
    MPI_Datatype actor_type;
    MPI_Datatype actor_type_dup;

    int data_count = 10;
    MPI_Datatype data_type = MPI_INT;

    int test_data_count = 0;
    MPI_Datatype test_data_type = NULL;

    MPI_Type_create_actor(
        test_init_fn,
        test_main_fn,
        test_destroy_fn,
        data_count, data_type,
        &actor_type
    );
    MPI_Type_commit(&actor_type);


    /* Make duplicate */
    MPI_Type_dup(actor_type, &actor_type_dup);


    /* Test MPI_Actor_data_count */
    MPI_Actor_data_count(actor_type_dup, &test_data_count);
    require_true(
        "MPI_Actor_data_count should return 10 on dup", MPI_COMM_WORLD,
        test_data_count == 10
    );

    /* test mpi_actor_data_type */
    MPI_Actor_data_type(actor_type_dup, &test_data_type);
    require_true(
        "MPI_Actor_data_type should return MPI_INT on dup", MPI_COMM_WORLD,
        test_data_type == MPI_INT
    );

    /* Free original actor_type */
    MPI_Type_free(&actor_type);


    /* Data getting should still work as expected */
    test_data_count = 0;
    test_data_type = NULL;

    MPI_Actor_data_count(actor_type_dup, &test_data_count);
    require_true(
        "MPI_Actor_data_count should return 10 on dup"
        " after original is freed", MPI_COMM_WORLD,
        test_data_count == 10
    );

    /* test mpi_actor_data_type */
    MPI_Actor_data_type(actor_type_dup, &test_data_type);
    require_true(
        "MPI_Actor_data_type should return MPI_INT on dup"
        " after original is freed", MPI_COMM_WORLD,
        test_data_type == MPI_INT
    );


    /* Free duplicated actor_type */
    MPI_Type_free(&actor_type_dup);

}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);


    start_suite("Actor Type Definition");
    run_test(actor_type_creation, "Actor Type Creation");
    run_test(actor_type_duplication, "Actor Type Duplication");
    end_suite();


    MPI_Finalize();
    return EXIT_SUCCESS;
}
