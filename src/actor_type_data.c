#include "actor.h"
#include "actor_type_data.h"

#include <stdlib.h>
#include <string.h>


/* Keyval for Actor_type_data  */
static int Actor_type_data_key = MPI_KEYVAL_INVALID;

/* Allocate a new instance of Actor_type_data on the heap */
static void alloc_Actor_type_data(
    MPI_Actor_main_function, int, MPI_Datatype, void*, Actor_type_data**
);

/* Allocate a new instance of Actor_type_data and copy the data across */
static int copy_Actor_type_data(
    MPI_Datatype, int, void*, void*, void*, int*
);

/* Free an instance of Actor_type_data from the heap */
static int free_Actor_type_data(MPI_Datatype, int, void*, void*);

/* Initialise they keys in this file if not already initiialised */
/* Not thread safe!                                              */
static void initialise_keys(void);


/****************************************************************************/


int attach_Actor_type_data(
    MPI_Actor_main_function *actor_main_function,
    int count, MPI_Datatype type, void *initial_data,
    MPI_Datatype actor_type
) {
    Actor_type_data *actor_type_data_ptr = NULL;
    int err;


    initialise_keys();


    /* Initialise Actor_type_data and set to keyval */
    alloc_Actor_type_data(
        actor_main_function,
        count, type, initial_data,
        &actor_type_data_ptr
    );

    err = MPI_Type_set_attr(
        actor_type, Actor_type_data_key, actor_type_data_ptr
    );


    return err;
}



int get_Actor_type_data(
    MPI_Datatype actor_type, Actor_type_data **actor_type_data, int *flag
) {
    int err;


    err = MPI_Type_get_attr(
        actor_type, Actor_type_data_key, actor_type_data, flag
    );

    return err;
}


/****************************************************************************/


static void alloc_Actor_type_data(
    MPI_Actor_main_function *main,
    int count, MPI_Datatype type, void *initial_data,
    Actor_type_data** actor_type_data
) {
    *actor_type_data = malloc(sizeof(Actor_type_data));

    (**actor_type_data).main  = main;
    (**actor_type_data).count = count;
    (**actor_type_data).type  = type;

    int type_size;
    MPI_Type_size(type, &type_size);
    (**actor_type_data).initial_data = malloc(type_size*count);
    memcpy((**actor_type_data).initial_data, initial_data, type_size*count);
}

/****************************************************************************/

static int copy_Actor_type_data(
    MPI_Datatype actor_type,
    int data_count_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    Actor_type_data  *actor_type_data_in  = attribute_val_in;
    Actor_type_data **actor_type_data_out = attribute_val_out;

    alloc_Actor_type_data(
        actor_type_data_in->main,
        actor_type_data_in->count,
        actor_type_data_in->type,
        actor_type_data_in->initial_data,
        actor_type_data_out
    );

    *flag=1;

    return MPI_SUCCESS;
}

/****************************************************************************/

static int free_Actor_type_data(
    MPI_Datatype actor_type,
    int data_count_key,
    void *attribute_val,
    void *extra_state
){
    Actor_type_data *actor_type_data = attribute_val;

    free(actor_type_data->initial_data);
    free(actor_type_data);

    return MPI_SUCCESS;
}

/****************************************************************************/

static void initialise_keys(void) {
    /* TODO: Add mutex locks etc for thread safety.  */
    /* Same problem exists in MPICH2 implementation. */
    
    if(Actor_type_data_key == MPI_KEYVAL_INVALID) {
        MPI_Type_create_keyval(
            copy_Actor_type_data,
            free_Actor_type_data,
            &Actor_type_data_key,
            NULL
        );
    }
}
