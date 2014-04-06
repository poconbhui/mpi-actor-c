#ifndef ACTOR_COMM_DATA_H_
#define ACTOR_COMM_DATA_H_


#include "actor.h"


typedef struct {
    int num_actor_types;
    MPI_Datatype *actor_types;

    MPI_Datatype receptionist_type;
} Actor_comm_data;


int attach_Actor_comm_data(
    int num_actor_types, MPI_Datatype* actor_types,
    MPI_Datatype receptionist_type,
    MPI_Comm actor_comm
);

int get_Actor_comm_data(
    MPI_Comm comm_actor, Actor_comm_data **actor_comm_data, int *flag
);


#endif  /* ACTOR_COMM_DATA_H_ */
