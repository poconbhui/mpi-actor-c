#include "actor.h"

#include <stdlib.h>


/* Key for storing actor types on a communicator */
static int comm_actor_types_key;


/* Allocate a new data space and copy all the entries over */
static int comm_actor_types_key_copy_fn(
    MPI_Comm comm_actor,
    int comm_actor_types_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    MPI_Datatype  *attribute_val_in_ptr  = attribute_val_in;
    MPI_Datatype **attribute_val_out_ptr = attribute_val_out;

    int num_entries;
    int i;


    /* Find the number of Actor types in the input attributes */
    /* MPI_DATATYPE_NULL used to mark end of entries */
    num_entries = 0;
    while(
        attribute_val_in_ptr[num_entries] != MPI_DATATYPE_NULL
        && num_entries++
    );
    num_entries++;

    /* Allocate the new attribute array */
    *attribute_val_out_ptr = malloc(sizeof(MPI_Datatype)*(num_entries));

    /* Copy entries over */
    for(i=0; i<num_entries; i++) {
        (*attribute_val_out_ptr)[i] = attribute_val_in_ptr[i];
    }


    *flag=1;


    return MPI_SUCCESS;
}


/* Simply free the data */
static int comm_key_free_fn(
    MPI_Comm comm_actor,
    int comm_key,
    void *attribute_val,
    void *extra_state
){
    free(attribute_val);

    return MPI_SUCCESS;
}


/* Initialise keyvals if not already initialised. Otherwise do nothing. */
static int initialise_keys(void) {
    static int initialised = MPI_ERR_UNKNOWN;

    if(initialised != MPI_SUCCESS) {
        MPI_Comm_create_keyval(
            comm_actor_types_key_copy_fn,
            comm_key_free_fn,
            &comm_actor_types_key,
            NULL
        );

        initialised = MPI_SUCCESS;
    }

    return initialised;
}


int MPI_Actor_create(
    MPI_Comm comm_old, int nactor_types,
    MPI_Datatype *actor_types,
    MPI_Comm *comm_actor
) {
    MPI_Datatype actor_types_attr_data[nactor_types+1];
    MPI_Datatype *actor_types_attr;
    int i;
    int flag;


    /* Initialise the keys used for this communicator */
    initialise_keys();


    /* Duplicate the incoming communicator */
    MPI_Comm_dup(comm_old, comm_actor);

    
    /* Generate new attributes list to be attached to the communicator */

    /* Set attribute data */
    for(i=0; i<nactor_types; i++) {
        actor_types_attr_data[i] = actor_types[i];
    }
    /* MPI_DATATYPE_NULL used to mark end of data */
    actor_types_attr_data[nactor_types] = MPI_DATATYPE_NULL;


    /* Use attribute copy function to allocate and set */
    /* attributes data pointer.                        */
    comm_actor_types_key_copy_fn(
        *comm_actor, comm_actor_types_key, NULL,
        actor_types_attr_data, &actor_types_attr,
        &flag
    );


    /* Attach datatypes to the communicator */
    MPI_Comm_set_attr(*comm_actor, comm_actor_types_key, actor_types_attr);


    return MPI_SUCCESS;
}


int MPI_Actor_get(
    MPI_Comm comm_actor, int nactor_types,
    MPI_Datatype *actor_types
) {
    MPI_Datatype *actor_types_attr;
    int flag;
    int i;


    MPI_Comm_get_attr(
        comm_actor, comm_actor_types_key,
        &actor_types_attr, &flag
    );

    for(i=0; i<nactor_types; i++) {
        actor_types[i] = actor_types_attr[i];
    }

    return MPI_SUCCESS;
}
