#include "actor_comm_data.h"

#include <stdlib.h>
#include <string.h>


/* Keyval for actor data stored on a communicator. */
static int Actor_comm_data_key = MPI_KEYVAL_INVALID;


/* Allocate and set a new Actor_comm_data */
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

int attach_Actor_comm_data(
    int num_actor_types, MPI_Datatype* actor_types,
    MPI_Datatype receptionist_type,
    MPI_Comm comm_actor
) {
    Actor_comm_data *actor_comm_data_ptr = NULL;
    int err = MPI_ERR_UNKNOWN;


    initialise_keys();


    /* Generate new Actor_comm_data to be attached to the communicator */
    alloc_Actor_comm_data(
        num_actor_types, actor_types, receptionist_type,
        &actor_comm_data_ptr
    );


    /* Attach datatypes to the communicator */
    err = MPI_Comm_set_attr(
        comm_actor, Actor_comm_data_key,
        actor_comm_data_ptr
    );

    return err;
}

/****************************************************************************/

int get_Actor_comm_data(
    MPI_Comm comm_actor, Actor_comm_data **actor_comm_data, int *flag
) {
    int err = MPI_ERR_UNKNOWN;


    initialise_keys();


    err = MPI_Comm_get_attr(
        comm_actor, Actor_comm_data_key, actor_comm_data, flag
    );

    return err;
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
