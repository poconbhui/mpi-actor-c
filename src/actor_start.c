#include "actor.h"

#include <stdlib.h>
#include <string.h>


/* Do one tick of the actor model.                                     */
/*   - Check for library messages and perform the appropriate actions. */
/*   - Reset flags for message sending/receiving.                      */
/*   - Find the next actor to run and run it.                          */
/*   - Send any library messages necessary.                            */
static int actor_model_tick(MPI_Comm actor_comm);


/*
 * Actor instance manipulation functions
 */

/* Definition of an instance of an actor. */
typedef struct {
    MPI_Datatype actor_type;
    void *state;
} Actor_instance;

/* Definition of a NULL, empty instance */
const static Actor_instance ACTOR_INSTANCE_NULL = { MPI_DATATYPE_NULL, NULL };


/* Allocate a new instance of an actor */
static void alloc_Actor_instance(
    MPI_Datatype actor_type, Actor_instance *actor_instance
);

/* Free an instance of an actor */
static void free_Actor_instance(
    Actor_instance *actor_instance
);

/* Get the current state of an actor */
static void get_Actor_instance_state(
    Actor_instance actor_instance, void *state
);

/* Set the current state of an actor */
static void set_Actor_instance_state(
    Actor_instance actor_instance, void *state
);

static int run_Actor_instance(
    Actor_instance actor_instance, MPI_Comm comm_actor
);



static int Actor_start_data_key();
typedef struct {
    MPI_Comm lib_comms;
    MPI_Comm actor_comms;
} Actor_start_data;


/****************************************************************************/

int MPI_Actor_start(MPI_Comm comm_actor, int root, void *receptionist_state) {
    MPI_Datatype   receptionist_type     = MPI_DATATYPE_NULL;
    Actor_instance receptionist_instance = ACTOR_INSTANCE_NULL;

    int rank;


    /* Get the receptionist type */
    MPI_Actor_get(comm_actor, 0, NULL, &receptionist_type);


    MPI_Comm_rank(comm_actor, &rank);

    if(rank == root) {
        alloc_Actor_instance(receptionist_type, &receptionist_instance);
        set_Actor_instance_state(receptionist_instance, receptionist_state);
    }


    /* Do actor model main loop while the receptionist isn't dead */
    while(actor_model_tick(comm_actor));


    if(rank == root) {
        get_Actor_instance_state(receptionist_instance, receptionist_state);
    }

    return MPI_SUCCESS;
}

int MPI_Actor_send(
    void *data, int count, MPI_Datatype datatype,
    int actor_id, int tag,
    MPI_Comm comm_actor
) {}


int MPI_Actor_recv(
    void *data, int count, MPI_Datatype datatype,
    int actor_id, int tag,
    MPI_Comm comm_actor
) {}


int MPI_Actor_get_id(
    MPI_Comm comm_actor, int *actor_id
) { *actor_id = -1; }

/*****************************************************************************/

static int actor_model_tick(MPI_Comm comm_actor) {
    /* Check for library messages and perform the appropriate actions. */

    /* Reset flags for message sending/receiving. */

    /* Find the next actor to run and run it. */

    /* Send any library messages necessary. */

    return 0;
}

/*****************************************************************************/

static void alloc_Actor_instance(
    MPI_Datatype actor_type, Actor_instance *actor_instance
) {
    int count;
    int datatype;
    int datatype_size;


    /* Set actor_type */
    (*actor_instance).actor_type = actor_type;


    /* Allocate state */
    MPI_Actor_get_datatypes(actor_type, &count, &datatype);

    MPI_Type_size(datatype, &datatype_size);

    (*actor_instance).state = malloc(datatype_size*count);


    /* Set state */
    MPI_Actor_get_data(actor_type, count, datatype, (*actor_instance).state);
}

/*****************************************************************************/

static void get_Actor_instance_state(
    Actor_instance actor_instance, void *state
) {
    MPI_Datatype actor_type = actor_instance.actor_type;

    int count;
    int datatype;
    int datatype_size;


    /* Get data size */
    MPI_Actor_get_datatypes(actor_type, &count, &datatype);

    MPI_Type_size(datatype, &datatype_size);


    /* Copy data across */
    memcpy(state, actor_instance.state, datatype_size*count);
}

/*****************************************************************************/

static void set_Actor_instance_state(
    Actor_instance actor_instance, void *state
) {
    MPI_Datatype actor_type = actor_instance.actor_type;

    int count;
    int datatype;
    int datatype_size;


    /* Get data size */
    MPI_Actor_get_datatypes(actor_type, &count, &datatype);

    MPI_Type_size(datatype, &datatype_size);


    /* Copy data across */
    memcpy(actor_instance.state, state, datatype_size*count);
}

/*****************************************************************************/

static int run_Actor_instance(
    Actor_instance actor_instance, MPI_Comm comm_actor
) {
    MPI_Actor_main_function *main = NULL;
    void *state = NULL;


    // Get main function and state
    MPI_Actor_get_main(
        actor_instance.actor_type,
        &main
    );

    state = actor_instance.state;


    // Run actor instance
    main(comm_actor, state);
}

/*****************************************************************************/

static int Actor_start_data_key(void) {
    static int key = MPI_KEYVAL_INVALID;

    if(key == MPI_KEYVAL_INVALID) {
    }

    return key;
}
