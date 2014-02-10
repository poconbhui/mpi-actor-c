#include "actor.h"

#include <stdlib.h>


/****************************************************************************/
/* Local functions and values                                               */
/****************************************************************************/


/* Keyval for the initialisation function for an actor type. */
static int actor_initialise_function_key = MPI_KEYVAL_INVALID;


/* Keyval for the main function for an actor type. */
static int actor_main_function_key = MPI_KEYVAL_INVALID;


/* Keyval for the destruction function for an actor type. */
static int actor_destroy_function_key = MPI_KEYVAL_INVALID;


/* Keyval for the number of data elements held by the actor. */
static int data_count_key = MPI_KEYVAL_INVALID;

/* Keyval copy function which allocates space to hold an int */
/* and copies the old value across.                          */
static int type_int_copy_fn(
    MPI_Datatype, int, void*, void*, void*, int*
);


/* Keyval for the MPI_Datatype held by the actor */
static int data_type_key = MPI_KEYVAL_INVALID;

/* Keyval copy function which allocates space to hold an */
/* MPI_Datatype and copies the old value across.         */
static int type_mpi_datatype_copy_fn(
    MPI_Datatype, int, void*, void*, void*, int*
);


/* Free the memory held in the attribute_val pointer */
static int attribute_val_free_fn(MPI_Datatype, int, void*, void*);


/* Initialise they keys in this file if not already initiialised */
/* Not thread safe!                                              */
static void initialise_keys(void);


/****************************************************************************/
/* Implementations start here                                               */
/****************************************************************************/


int MPI_Type_create_actor(
    MPI_Actor_initialise_function *actor_initialise_function,
    MPI_Actor_main_function *actor_main_function,
    MPI_Actor_destroy_function *actor_destroy_function,
    int count, MPI_Datatype type,
    MPI_Datatype *actor_type
) {

    /* Initialise keyvals if not already initialised */
    initialise_keys();


    /* Define the type and commit early so keys can be added to it. */
    /* MPI-2 says committing twice is allowed and a null-op         */
    MPI_Type_contiguous(
        count, type,
        actor_type
    );
    MPI_Type_commit(actor_type);


    /* Set function keyvals */
    MPI_Type_set_attr(
        *actor_type, actor_initialise_function_key, actor_initialise_function
    );
    MPI_Type_set_attr(
        *actor_type, actor_main_function_key, actor_main_function
    );
    MPI_Type_set_attr(
        *actor_type, actor_destroy_function_key, actor_destroy_function
    );


    /* Allocate data for data_count keyval and set it */
    int *data_count_key_data = malloc(sizeof(int));
    *data_count_key_data = count;
    MPI_Type_set_attr(
        *actor_type, data_count_key, data_count_key_data
    );

    /* Allocate data for data_type keyval and set it */
    MPI_Datatype *data_type_key_data = malloc(sizeof(int));
    *data_type_key_data = type;
    MPI_Type_set_attr(
        *actor_type, data_type_key, data_type_key_data
    );


    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_data_count(MPI_Datatype actor_type, int *count) {
    int *data_count_ptr = NULL;
    int flag;

    MPI_Type_get_attr(actor_type, data_count_key, &data_count_ptr, &flag);

    *count = *data_count_ptr;

    return MPI_SUCCESS;
}

/****************************************************************************/

int MPI_Actor_data_type(MPI_Datatype actor_type, MPI_Datatype *type) {
    MPI_Datatype *data_type_ptr = NULL;
    int flag;

    MPI_Type_get_attr(actor_type, data_type_key, &data_type_ptr, &flag);

    *type = *data_type_ptr;

    return MPI_SUCCESS;
}

/****************************************************************************/

static int type_int_copy_fn(
    MPI_Datatype actor_type,
    int data_count_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    int *attribute_val_in_ptr = attribute_val_in;
    int **attribute_val_out_ptr = attribute_val_out;

    *attribute_val_out_ptr = malloc(sizeof(int));
    **attribute_val_out_ptr = *attribute_val_in_ptr;

    *flag=1;

    return MPI_SUCCESS;
}

/****************************************************************************/

static int type_mpi_datatype_copy_fn(
    MPI_Datatype actor_type,
    int data_count_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    int *attribute_val_in_ptr = attribute_val_in;
    int **attribute_val_out_ptr = attribute_val_out;

    *attribute_val_out_ptr = malloc(sizeof(MPI_Datatype));
    **attribute_val_out_ptr = *attribute_val_in_ptr;

    *flag=1;

    return MPI_SUCCESS;
}

/****************************************************************************/

static int attribute_val_free_fn(
    MPI_Datatype actor_type,
    int data_count_key,
    void *attribute_val,
    void *extra_state
){
    free(attribute_val);

    return MPI_SUCCESS;
}

/****************************************************************************/

static void initialise_keys(void) {
    /* TODO: Add mutex locks etc for thread safety.  */
    /* Same problem exists in MPICH2 implementation. */

    if(actor_initialise_function_key == MPI_KEYVAL_INVALID) {
        MPI_Type_create_keyval(
            MPI_TYPE_DUP_FN,
            MPI_TYPE_NULL_DELETE_FN,
            &actor_initialise_function_key,
            NULL
        );
    }
    
    if(actor_main_function_key == MPI_KEYVAL_INVALID) {
        MPI_Type_create_keyval(
            MPI_TYPE_DUP_FN,
            MPI_TYPE_NULL_DELETE_FN,
            &actor_main_function_key,
            NULL
        );
    }

    if(actor_destroy_function_key == MPI_KEYVAL_INVALID) {
        MPI_Type_create_keyval(
            MPI_TYPE_DUP_FN,
            MPI_TYPE_NULL_DELETE_FN,
            &actor_destroy_function_key,
            NULL
        );
    }

    if(data_count_key == MPI_KEYVAL_INVALID) {
        MPI_Type_create_keyval(
            type_int_copy_fn,
            attribute_val_free_fn,
            &data_count_key,
            NULL
        );
    }

    if(data_type_key == MPI_KEYVAL_INVALID) {
        MPI_Type_create_keyval(
            type_mpi_datatype_copy_fn,
            attribute_val_free_fn,
            &data_type_key,
            NULL
        );
    }
}
