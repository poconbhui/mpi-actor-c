#include "actor.h"
#include "mpitest.h"

#include <stdlib.h>


int test_receptionist_main(
    MPI_Comm actor_comm, void* void_actor_state
) {
    int *actor_state = void_actor_state;

    if(actor_state[0] == 0) {
        int msg = 0;
        int tag = 0;
        int self_id;

        MPI_Actor_get_id(actor_comm, &self_id);
        MPI_Actor_send(&msg, 1, MPI_INT, self_id, tag, actor_comm);

        actor_state[0] = 1;
        return MPI_ACTOR_ALIVE;
    }
    else if(actor_state[0] == 1) {
        int msg = -1;
        int tag = 0;

        MPI_Actor_recv(
            &msg, 1, MPI_INT,
            MPI_ACTOR_ANY_ID, MPI_ACTOR_ANY_TAG,
            actor_comm
        );

        actor_state[0] = 2;
        return MPI_ACTOR_DEAD;
    }
    else {
        actor_state[0] = 3;
        return MPI_ACTOR_DEAD;
    }
}

MPI_Datatype gen_test_receptionist(void) {
    static MPI_Datatype receptionist = MPI_DATATYPE_NULL;

    int num_receptionist_data = 1;
    MPI_Datatype receptionist_data_type = MPI_INT;
    int receptionist_data[] = { 0 };

    if(receptionist == MPI_DATATYPE_NULL) {
        MPI_Type_create_actor(
            test_receptionist_main,
            num_receptionist_data, receptionist_data_type, receptionist_data,
            &receptionist
        );
    }

    return receptionist;
}

void actor_start_receptionist(void) {
    MPI_Comm comm_actor;

    int num_actor_types = 0;
    int *actor_types = NULL;

    int receptionist_state[1] = { 0 };

    int root_rank = 0;

    MPI_Datatype receptionist = gen_test_receptionist();

    MPI_Actor_create(
        MPI_COMM_WORLD,
        num_actor_types, NULL, receptionist,
        &comm_actor
    );


    MPI_Actor_start(
        comm_actor, root_rank, receptionist_state
    );
    require_true(
        "MPI_Actor_start should set receptionist_state to 2 after input of 0",
        MPI_COMM_WORLD,
        receptionist_state[0] == 2
    );


    receptionist_state[0] = 2;
    MPI_Actor_start(
        comm_actor, root_rank, receptionist_state
    );

    require_true(
        "MPI_Actor_start should set receptionist_state to 3 after input of 2",
        MPI_COMM_WORLD,
        receptionist_state[0] == 3
    );

}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    start_suite("MPI_Actor_start");
    run_test(
        actor_start_receptionist, "MPI_Actor_start receptionist behaviours"
    );
    end_suite();

    MPI_Finalize();
    return mpitest_exit_status();
}
