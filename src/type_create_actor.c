#include "actor.h"
#include "actor_type_data.h"

#include <stdlib.h>
#include <string.h>


int MPI_Type_create_actor(
    MPI_Actor_main_function *actor_main_function,
    int count, MPI_Datatype type, void *initial_data,
    MPI_Datatype *actor_type
) {

    /* Define the type and commit early so keys can be added to it. */
    /* MPI-2 says committing twice is allowed and a null-op         */
    MPI_Type_contiguous(
        count, type,
        actor_type
    );
    MPI_Type_commit(actor_type);


    attach_Actor_type_data(
        actor_main_function, count, type, initial_data,
        *actor_type
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


    err = get_Actor_type_data(
        actor_type, &actor_type_data, &flag
    );

    if(flag != 1 || err != MPI_SUCCESS) {
        /* Actor_type_data not set on this type */
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


    err = get_Actor_type_data(
        actor_type, &actor_type_data, &flag
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


    err = get_Actor_type_data(
        actor_type, &actor_type_data, &flag
    );

    if(flag != 1 || err != MPI_SUCCESS) {
        return MPI_Comm_call_errhandler(MPI_COMM_WORLD, MPI_ERR_TYPE);
    }


    *actor_main_function = (*actor_type_data).main;

    
    return MPI_SUCCESS;
}

