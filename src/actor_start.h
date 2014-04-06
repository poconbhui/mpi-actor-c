#ifndef ACTOR_START_H_
#define ACTOR_START_H_

int MPI_Actor_start(MPI_Comm comm_actor, int root, void* receptionist_state);



#define MPI_ACTOR_ANY_ID (-1)
#define MPI_ACTOR_ANY_TAG MPI_ANY_TAG

int MPI_Actor_send(
    void *data, int count, MPI_Datatype datatype,
    int actor_id, int tag,
    MPI_Comm comm_actor
);


int MPI_Actor_recv(
    void *data, int count, MPI_Datatype datatype,
    int actor_id, int tag,
    MPI_Comm comm_actor
);


int MPI_Actor_get_id(
    MPI_Comm comm_actor, int *actor_id
);


#endif  // ACTOR_START_H_
