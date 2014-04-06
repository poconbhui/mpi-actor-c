#include "actor.h"
#include "actor_comm_data.h"

#include <stdlib.h>
#include <string.h>


int MPI_Actor_create(
    MPI_Comm comm_old, int num_actor_types,
    MPI_Datatype *actor_types, MPI_Datatype receptionist_type,
    MPI_Comm *comm_actor
) {
    Actor_comm_data *actor_comm_data_ptr = NULL;

    int flag;


    /* Duplicate the incoming communicator */
    MPI_Comm_dup(comm_old, comm_actor);

    
    /* Generate and attach the Actor_comm_data to the communicator */
    attach_Actor_comm_data(
        num_actor_types, actor_types,
        receptionist_type,
        *comm_actor
    );


    return MPI_SUCCESS;
}


/****************************************************************************/


int MPI_Actor_get_num(MPI_Comm comm_actor, int *num_actor_types) {
    Actor_comm_data *actor_comm_data = NULL;
    int flag;


    get_Actor_comm_data(
        comm_actor, &actor_comm_data, &flag
    );


    *num_actor_types = (*actor_comm_data).num_actor_types;

    return MPI_SUCCESS;
}


/****************************************************************************/


int MPI_Actor_get(
    MPI_Comm comm_actor, int max_num_actor_types,
    MPI_Datatype *actor_types, MPI_Datatype *receptionist_type
) {
    Actor_comm_data *actor_comm_data = NULL;
    int flag;
    int i;


    get_Actor_comm_data(
        comm_actor, &actor_comm_data, &flag
    );


    if((*actor_comm_data).num_actor_types < max_num_actor_types) {
        max_num_actor_types = (*actor_comm_data).num_actor_types;
    }
    //return MPI_ERR_UNKNOWN;

    for(i=0; i<max_num_actor_types; i++) {
        actor_types[i] = (*actor_comm_data).actor_types[i];
    }


    *receptionist_type = (*actor_comm_data).receptionist_type;


    return MPI_SUCCESS;
}
