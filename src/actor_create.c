#include "actor.h"

#include <stdlib.h>
#include <string.h>


/****************************************************************************/
/* Local functions and values                                               */
/****************************************************************************/


/* Keyval for actor data stored on a communicator. */
static int Actor_comm_data_key = MPI_KEYVAL_INVALID;

typedef struct {
    int num_actor_types;
    MPI_Datatype *actor_types;

    MPI_Datatype receptionist_type;
} Actor_comm_data;


static void alloc_Actor_comm_data(
    int, MPI_Datatype*, MPI_Datatype, Actor_comm_data**
);


/* Allocate and copy the an Actor_comm_data */
static int copy_Actor_comm_data(
    MPI_Comm, int, void*, void*, void*, int*
);


/* Free an Actor_comm_data */
static int free_Actor_comm_data(MPI_Comm, int, void*, void*);


/* Initialise keyvals if not already initialised. Otherwise do nothing. */
/* NOT thread safe.                                                     */
static void initialise_keys(void);


/****************************************************************************/
/* Implementations start here                                               */
/****************************************************************************/


int MPI_Actor_create(
    MPI_Comm comm_old, int nactor_types,
    MPI_Datatype *actor_types, MPI_Datatype receptionist_type,
    MPI_Comm *comm_actor
) {
    Actor_comm_data *actor_comm_data_ptr = NULL;

    int flag;


    /* Initialise the keys used for this communicator */
    initialise_keys();


    /* Duplicate the incoming communicator */
    MPI_Comm_dup(comm_old, comm_actor);

    
    /* Generate new Actor_comm_data to be attached to the communicator */
    alloc_Actor_comm_data(
        nactor_types, actor_types, receptionist_type,
        &actor_comm_data_ptr
    );


    /* Attach datatypes to the communicator */
    MPI_Comm_set_attr(
        *comm_actor, Actor_comm_data_key,
        actor_comm_data_ptr
    );


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



    MPI_Comm_get_attr(
        comm_actor, Actor_comm_data_key,
        &actor_comm_data, &flag
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

/****************************************************************************/

static void alloc_Actor_comm_data(
    int num_actor_types, MPI_Datatype *actor_types,
    MPI_Datatype receptionist_type,
    Actor_comm_data** actor_comm_data
) {
    *actor_comm_data = malloc(sizeof(Actor_comm_data));

    (**actor_comm_data).num_actor_types = num_actor_types;

    size_t actor_types_size = sizeof(MPI_Datatype)*num_actor_types;
    (**actor_comm_data).actor_types = malloc(actor_types_size);
    memcpy((**actor_comm_data).actor_types, actor_types, actor_types_size);

    (**actor_comm_data).receptionist_type = receptionist_type;
    
}

/****************************************************************************/

static int copy_Actor_comm_data(
    MPI_Comm comm_actor,
    int comm_actor_types_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    Actor_comm_data  *actor_comm_data_in  = attribute_val_in;
    Actor_comm_data **actor_comm_data_out = attribute_val_out;


    alloc_Actor_comm_data(
        (*actor_comm_data_in).num_actor_types,
        (*actor_comm_data_in).actor_types,
        (*actor_comm_data_in).receptionist_type,
        actor_comm_data_out
    );


    *flag=1;


    return MPI_SUCCESS;
}

/****************************************************************************/

static int free_Actor_comm_data(
    MPI_Comm comm_actor,
    int comm_key,
    void *attribute_val,
    void *extra_state
){
    Actor_comm_data *actor_comm_data = attribute_val;

    free((*actor_comm_data).actor_types);
    free(actor_comm_data);

    return MPI_SUCCESS;
}

/****************************************************************************/

static void initialise_keys(void) {
    /* TODO: Add mutex locks to allow for thread safety */

    if(Actor_comm_data_key == MPI_KEYVAL_INVALID) {
        MPI_Comm_create_keyval(
            copy_Actor_comm_data,
            free_Actor_comm_data,
            &Actor_comm_data_key,
            NULL
        );
    }
}
