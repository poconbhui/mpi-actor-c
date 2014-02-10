#include "actor.h"

#include <stdlib.h>
#include <string.h>


/****************************************************************************/
/* Local functions and values                                               */
/****************************************************************************/


/* Keyval for the main function for an actor type. */
static int actor_type_struct_key = MPI_KEYVAL_INVALID;

typedef struct {
    MPI_Actor_main_function *main;
    int count;
    MPI_Datatype type;
    void *initial_data;
} actor_type_struct;


static void alloc_actor_type_struct(
    MPI_Actor_main_function, int, MPI_Datatype, void*, actor_type_struct**
);

static int copy_actor_type_struct(
    MPI_Comm, int, void*, void*, void*, int*
);

static int free_actor_type_struct(MPI_Datatype, int, void*, void*);


/* Initialise they keys in this file if not already initiialised */
/* Not thread safe!                                              */
static void initialise_keys(void);


/****************************************************************************/
/* Implementations start here                                               */
/****************************************************************************/


int MPI_Type_create_actor(
    MPI_Actor_main_function *actor_main_function,
    int count, MPI_Datatype type, void *initial_data,
    MPI_Datatype *actor_type
) {
    actor_type_struct *actor_data;


    /* Initialise keyvals if not already initialised */
    initialise_keys();


    /* Define the type and commit early so keys can be added to it. */
    /* MPI-2 says committing twice is allowed and a null-op         */
    MPI_Type_contiguous(
        count, type,
        actor_type
    );
    MPI_Type_commit(actor_type);


    /* Initialise actor_type_struct and set to keyval */
    alloc_actor_type_struct(
        actor_main_function,
        count, type, initial_data,
        &actor_data
    );

    MPI_Type_set_attr(
        *actor_type, actor_type_struct_key, actor_data
    );


    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_get_datatypes(
    MPI_Datatype actor_type, int *count, MPI_Datatype *type
) {
    actor_type_struct* actor_data;
    int flag;
    int err;


    err = MPI_Type_get_attr(
        actor_type, actor_type_struct_key, &actor_data, &flag
    );

    if(flag != 1 || err != MPI_SUCCESS) {
        /* actor_type_struct_key not set on this type */
        return MPI_Comm_call_errhandler(MPI_COMM_WORLD, MPI_ERR_TYPE);
    }


    *count = actor_data->count;
    *type  = actor_data->type;


    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_get_data(
    MPI_Datatype actor_type, int count, MPI_Datatype type, void* data
) {
    actor_type_struct *actor_data;
    int flag;
    int err;


    err = MPI_Type_get_attr(
        actor_type, actor_type_struct_key, &actor_data, &flag
    );

    if(flag != 1 || err != MPI_SUCCESS) {
        return flag;
    }


    /* Get the lesser of the input sizes and the actual size */
    int type_in_size;
    int type_actual_size;
    MPI_Type_size(type, &type_in_size);
    MPI_Type_size(actor_data->type, &type_actual_size);

    size_t size_in = type_in_size*count;
    size_t size_actual = type_actual_size*(actor_data->count);

    if(size_in > size_actual) size_in = size_actual;
    memcpy(data, actor_data->initial_data, size_in);


    return MPI_SUCCESS;
}

/****************************************************************************/

static void alloc_actor_type_struct(
    MPI_Actor_main_function *main,
    int count, MPI_Datatype type, void *initial_data,
    actor_type_struct** actor_data
) {
    *actor_data = malloc(sizeof(actor_type_struct));

    (**actor_data).main  = main;
    (**actor_data).count = count;
    (**actor_data).type  = type;

    int type_size;
    MPI_Type_size(type, &type_size);
    (**actor_data).initial_data = malloc(type_size*count);
    memcpy((**actor_data).initial_data, initial_data, type_size*count);
}

/****************************************************************************/

static int copy_actor_type_struct(
    MPI_Datatype actor_type,
    int data_count_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    actor_type_struct *old_actor  = attribute_val_in;
    actor_type_struct **new_actor = attribute_val_out;

    alloc_actor_type_struct(
        old_actor->main,
        old_actor->count,
        old_actor->type,
        old_actor->initial_data,
        new_actor
    );

    *flag=1;

    return MPI_SUCCESS;
}

/****************************************************************************/

static int free_actor_type_struct(
    MPI_Datatype actor_type,
    int data_count_key,
    void *attribute_val,
    void *extra_state
){
    actor_type_struct *old_actor = attribute_val;

    free(old_actor->initial_data);
    free(old_actor);

    return MPI_SUCCESS;
}

/****************************************************************************/

static void initialise_keys(void) {
    /* TODO: Add mutex locks etc for thread safety.  */
    /* Same problem exists in MPICH2 implementation. */
    
    if(actor_type_struct_key == MPI_KEYVAL_INVALID) {
        MPI_Type_create_keyval(
            copy_actor_type_struct,
            free_actor_type_struct,
            &actor_type_struct_key,
            NULL
        );
    }
}
