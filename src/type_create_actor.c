#include "actor.h"

#include <stdlib.h>
#include <string.h>


/****************************************************************************/
/* Local functions and values                                               */
/****************************************************************************/


/* Keyval for the main function for an actor type. */
static int Actor_type_data_key = MPI_KEYVAL_INVALID;

typedef struct {
    MPI_Actor_main_function *main;
    int count;
    MPI_Datatype type;
    void *initial_data;
} Actor_type_data;


static void alloc_Actor_type_data(
    MPI_Actor_main_function, int, MPI_Datatype, void*, Actor_type_data**
);

static int copy_Actor_type_data(
    MPI_Datatype, int, void*, void*, void*, int*
);

static int free_Actor_type_data(MPI_Datatype, int, void*, void*);


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
    Actor_type_data *actor_type_data;


    /* Initialise keyvals if not already initialised */
    initialise_keys();


    /* Define the type and commit early so keys can be added to it. */
    /* MPI-2 says committing twice is allowed and a null-op         */
    MPI_Type_contiguous(
        count, type,
        actor_type
    );
    MPI_Type_commit(actor_type);


    /* Initialise Actor_type_data and set to keyval */
    alloc_Actor_type_data(
        actor_main_function,
        count, type, initial_data,
        &actor_type_data
    );

    MPI_Type_set_attr(
        *actor_type, Actor_type_data_key, actor_type_data
    );


    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_get_datatypes(
    MPI_Datatype actor_type, int *count, MPI_Datatype *type
) {
    Actor_type_data* actor_type_data;
    int flag;
    int err;


    err = MPI_Type_get_attr(
        actor_type, Actor_type_data_key, &actor_type_data, &flag
    );

    if(flag != 1 || err != MPI_SUCCESS) {
        /* Actor_type_data_key not set on this type */
        return MPI_Comm_call_errhandler(MPI_COMM_WORLD, MPI_ERR_TYPE);
    }


    *count = actor_type_data->count;
    *type  = actor_type_data->type;


    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_get_data(
    MPI_Datatype actor_type, int count, MPI_Datatype type, void* data
) {
    Actor_type_data *actor_type_data;
    int flag;
    int err;


    err = MPI_Type_get_attr(
        actor_type, Actor_type_data_key, &actor_type_data, &flag
    );

    if(flag != 1 || err != MPI_SUCCESS) {
        return MPI_Comm_call_errhandler(MPI_COMM_WORLD, MPI_ERR_TYPE);
    }


    /* Get the lesser of the input sizes and the actual size */
    int type_in_size;
    int type_actual_size;
    MPI_Type_size(type, &type_in_size);
    MPI_Type_size(actor_type_data->type, &type_actual_size);

    size_t size_in = type_in_size*count;
    size_t size_actual = type_actual_size*(actor_type_data->count);

    if(size_in > size_actual) size_in = size_actual;
    memcpy(data, actor_type_data->initial_data, size_in);


    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_get_main(
    MPI_Datatype actor_type,
    MPI_Actor_main_function **actor_main_function
) {
    Actor_type_data *actor_type_data = NULL;
    int flag;
    int err;


    err = MPI_Type_get_attr(
        actor_type, Actor_type_data_key, &actor_type_data, &flag
    );

    if(flag != 1 || err != MPI_SUCCESS) {
        return MPI_Comm_call_errhandler(MPI_COMM_WORLD, MPI_ERR_TYPE);
    }


    *actor_main_function = (*actor_type_data).main;

    
    return MPI_SUCCESS;
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
