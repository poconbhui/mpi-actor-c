#include "actor.h"

#include <stdlib.h>
#include <string.h>


/****************************************************************************/
/* Local functions and values                                               */
/****************************************************************************/


/* Keyval for actor data stored on a communicator. */
static int actor_comm_struct_key = MPI_KEYVAL_INVALID;

typedef struct {
    int num_actor_types;
    MPI_Datatype *actor_types;

    MPI_Datatype receptionist_type;
} actor_comm_struct;


static void alloc_actor_comm_struct(
    int, MPI_Datatype*, MPI_Datatype, actor_comm_struct**
);


/* Allocate and copy the an actor_comm_struct */
static int copy_actor_comm_struct(
    MPI_Comm, int, void*, void*, void*, int*
);


/* Free an actor_comm_struct */
static int free_actor_comm_struct(MPI_Comm, int, void*, void*);


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
    actor_comm_struct *acs_ptr = NULL;

    int flag;


    /* Initialise the keys used for this communicator */
    initialise_keys();


    /* Duplicate the incoming communicator */
    MPI_Comm_dup(comm_old, comm_actor);

    
    /* Generate new actor_comm_struct to be attached to the communicator */
    alloc_actor_comm_struct(
        nactor_types, actor_types, receptionist_type,
        &acs_ptr
    );


    /* Attach datatypes to the communicator */
    MPI_Comm_set_attr(*comm_actor, actor_comm_struct_key, acs_ptr);


    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_get(
    MPI_Comm comm_actor, int max_num_actor_types,
    MPI_Datatype *actor_types, MPI_Datatype *receptionist_type
) {
    actor_comm_struct *acs = NULL;
    int flag;
    int i;



    MPI_Comm_get_attr(
        comm_actor, actor_comm_struct_key,
        &acs, &flag
    );


    if((*acs).num_actor_types < max_num_actor_types) {
        max_num_actor_types = (*acs).num_actor_types;
    }
    //return MPI_ERR_UNKNOWN;

    for(i=0; i<max_num_actor_types; i++) {
        actor_types[i] = (*acs).actor_types[i];
    }


    *receptionist_type = (*acs).receptionist_type;


    return MPI_SUCCESS;
}

/****************************************************************************/

static void alloc_actor_comm_struct(
    int num_actor_types, MPI_Datatype *actor_types,
    MPI_Datatype receptionist_type,
    actor_comm_struct** acs
) {
    *acs = malloc(sizeof(actor_comm_struct));

    (**acs).num_actor_types = num_actor_types;

    size_t actor_types_size = sizeof(MPI_Datatype)*num_actor_types;
    (**acs).actor_types = malloc(actor_types_size);
    memcpy((**acs).actor_types, actor_types, actor_types_size);

    (**acs).receptionist_type = receptionist_type;
    
}

/****************************************************************************/

static int copy_actor_comm_struct(
    MPI_Comm comm_actor,
    int comm_actor_types_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    actor_comm_struct  *acs_in  = attribute_val_in;
    actor_comm_struct **acs_out = attribute_val_out;


    alloc_actor_comm_struct(
        (*acs_in).num_actor_types, (*acs_in).actor_types,
        (*acs_in).receptionist_type,
        acs_out
    );


    *flag=1;


    return MPI_SUCCESS;
}

/****************************************************************************/

static int free_actor_comm_struct(
    MPI_Comm comm_actor,
    int comm_key,
    void *attribute_val,
    void *extra_state
){
    actor_comm_struct *acs = attribute_val;

    free((*acs).actor_types);
    free(acs);

    return MPI_SUCCESS;
}

/****************************************************************************/

static void initialise_keys(void) {
    /* TODO: Add mutex locks to allow for thread safety */

    if(actor_comm_struct_key == MPI_KEYVAL_INVALID) {
        MPI_Comm_create_keyval(
            copy_actor_comm_struct,
            free_actor_comm_struct,
            &actor_comm_struct_key,
            NULL
        );
    }
}
