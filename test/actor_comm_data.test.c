#include "../src/actor_comm_data.h"
#include "mpitest.h"

#include <stdio.h>
#include <stdlib.h>


void actor_comm_data_generation(void) {
    Actor_comm_data *actor_comm_data_ptr = NULL;
    MPI_Comm comm_actor = MPI_COMM_NULL;

    const int num_actor_types = 3;
    MPI_Datatype actor_types[num_actor_types];
    MPI_Datatype receptionist_type;

    int err;
    int flag;
    int i;


    /* Duplicate MPI_COMM_WORLD for comm_actor */
    MPI_Comm_dup(MPI_COMM_WORLD, &comm_actor);


    /* Generate some MPI types */
    for(i=0; i<num_actor_types; i++) {
        MPI_Datatype MPI_Int_dup;

        MPI_Type_dup(MPI_INT, &MPI_Int_dup);

        actor_types[i] = MPI_Int_dup;
    }

    MPI_Type_dup(MPI_INT, &receptionist_type);


    /* Attach data */
    err = attach_Actor_comm_data(
        num_actor_types, actor_types, receptionist_type, comm_actor
    );

    require_true(
        "attach_Actor_comm_data should return MPI_SUCCESS", comm_actor,
        err == MPI_SUCCESS
    );


    /* Retrieve data */
    Actor_comm_data *actor_comm_data_test = NULL;
    flag = -1;
    err = get_Actor_comm_data(comm_actor, &actor_comm_data_test, &flag);

    require_true(
        "get_Actor_comm_data should return MPI_SUCCESS", comm_actor,
        err == MPI_SUCCESS
    );

    require_true(
        "actor_comm_data_test should not be NULL", comm_actor,
        actor_comm_data_test != NULL
    );

    require_true(
        "flag should be 1", comm_actor,
        flag == 1
    );


    /* test values */

    require_true(
        "actor_comm_data_test->num_actor_types should equal"
        " num_actor_types",
        comm_actor,
        actor_comm_data_test->num_actor_types == num_actor_types
    );

    for(i=0; i<num_actor_types; i++) {
        /* require value equality */
        require_true(
            "actor_comm_data_test->actor_types[i] should be equal to"
            " actor_types[i]",
            comm_actor,
            actor_comm_data_test->actor_types[i] == actor_types[i]
        );
    }

    /* require pointers different */
    require_true(
        "actor_comm_data_test->actor_types pointer should not"
        " equal actor_types",
        comm_actor,
        actor_comm_data_test->actor_types != actor_types
    );


    require_true(
        "actor_comm_data_test->receptionist_type should equal"
        " receptionist_type",
        comm_actor,
        actor_comm_data_test->receptionist_type == receptionist_type
    );


}


void actor_comm_data_failure(void) {
    Actor_comm_data *actor_comm_data_ptr = NULL;
    MPI_Comm comm_actor = MPI_COMM_NULL;

    int err;
    int flag;


    MPI_Comm_dup(MPI_COMM_WORLD, &comm_actor);


    Actor_comm_data *actor_comm_data_test = NULL;
    err = get_Actor_comm_data(
        comm_actor, &actor_comm_data_test, &flag
    );

    require_true(
        "get_Actor_comm_data should return MPI_SUCCESS", comm_actor,
        err == MPI_SUCCESS
    );

    require_true(
        "get_Actor_comm_data should set flag == 0", comm_actor,
        flag == 0
    );
}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    start_suite("Actor_comm_data");
    run_test(actor_comm_data_generation, "Generation");
    run_test(actor_comm_data_failure, "Generation");
    end_suite();

    MPI_Finalize();
    return mpitest_exit_status();
}
